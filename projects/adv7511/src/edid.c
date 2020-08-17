/***************************************************************************//**
*   @file   edid.c
*   @brief  EDID control module source.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "edid.h"
#include <stdlib.h>

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static bool edid_msg_enable;
static struct edid_info edid_info;
static struct edid_audio_info edid_audio_info;
static uint8_t blk_num;
static uint8_t blk_cnt;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * Calculate checksum for an EDID block.
 *
 * This is a module helper function only available in this module.
 *
 * @param [in] blk_ptr  - Pointer to the array containing the EDID block.
 * @param [in] blk_indx - Block number.
 *
 * @return checksum of the EDID block
 */
static uint8_t edid_get_block_checksum(uint8_t *blk_ptr, uint8_t blk_indx)
{
	uint16_t i;
	uint8_t cs = 0;

	for(i = 0; i < 127; i++)
		cs += blk_ptr[i];

	return (256 - cs);
}

/**
 * Parse and display timing parameters for the sink, from the EDID message.
 *
 * Module helper function.
 *
 * @param [in] curr_ptr - Pointer to the relevant part of the EDID block	.
 * @param [in] max_len - Max length of the part.
 *
 * @return void
 */
static void edid_parse_desc(uint8_t *curr_ptr, uint32_t max_len)
{
	uint8_t by;
	uint32_t i, j;
	struct strg_desc *strg_desc;
	struct mon_range_desc *mon_range_desc;
	struct std_timing *t_desc;

	for(i = 0; i < max_len; i += 18) {
		if(*curr_ptr == 0) { /* Non timing Desc */
			strg_desc = (struct strg_desc *)curr_ptr;
			by = curr_ptr[sizeof(struct strg_desc)];
			curr_ptr[sizeof(struct strg_desc)] = 0;
			switch (curr_ptr[3]) {
			case 0xfc:
				EDID_MSG("Mon Name:   %s\n\r", strg_desc->string);
				break;
			case 0xfd:
				mon_range_desc = (struct mon_range_desc *)curr_ptr;
				EDID_MSG("Mon Freq:\n\r");
				EDID_MSG("    Min V Freq = %d Hz\n\r",
					 mon_range_desc->min_v_freq);
				EDID_MSG("    Max V Freq = %d Hz\n\r",
					 mon_range_desc->max_v_freq);
				EDID_MSG("    Min H Freq = %d KHz\n\r",
					 mon_range_desc->min_h_freq);
				EDID_MSG("    Max H Freq = %d KHz\n\r",
					 mon_range_desc->max_h_freq);
				break;
			case 0xfe:
				EDID_MSG("Man String: %s\n\r", strg_desc->string);
				break;
			case 0xff:
				EDID_MSG("Mon Serial: %s\n\r", strg_desc->string);
				break;
			}
			curr_ptr[sizeof(struct strg_desc)] = by;
		} else {
			t_desc = (struct std_timing *)curr_ptr;
			EDID_MSG("Mon Timing:\n\r");
			by = t_desc->flags;
			j = t_desc->pixel_clk[0] + (t_desc->pixel_clk[1] * 256);
			EDID_MSG("    Pixel clock = %d.%d MHz\n\r", (int)(j / 100),
				 (int)(j % 100));
			EDID_MSG("    H Active    = %d\n\r",
				 ((t_desc->h_act_blnk44 & 0xf0) * 16) + t_desc->h_active);
			EDID_MSG("    V Active    = %d\n\r",
				 ((t_desc->v_act_blnk44 & 0xf0) * 16) + t_desc->v_active);
			EDID_MSG("    %s\n\r", (by & 0x80)? "Interlaced" : "Progressive");
			EDID_MSG("    %s\n\r", (by & 0x60)? "Stereo" : "No stereo");
			EDID_MSG("    Separate sync = %x\n\r", (by & 0x18) >> 3);
			EDID_MSG("    %s\n\r", (by & 0x04)? "+ve VSync" : "-ve Vsync");
			EDID_MSG("    %s\n\r", (by & 0x02)? "+ve HSync" : "-ve HSync");
		}
		curr_ptr += sizeof(struct strg_desc);
	}
}

/**
 * Display and save short audio descriptors.
 */
static void edid_parse_display_short_audio(uint8_t *blk_ptr, uint8_t desc_l)
{
	uint8_t i;
	uint16_t a_format, a_sample, a_bit;
	const char *format_strgs[] = {
		"Reserved",      "Linear PCM",       "AC-3",             "MPEG1",
		"MP3",           "MPEG2",            "AAC",              "DTS",
		"ATRAC",         "One Bit Audio",    "Dolby Digital +",  "DTS-HD",
		"MAT(MLP)",      "DST",              "WMA Pro",          "Reserved"
	};

	for(i = 0; i < desc_l; i += 3) {
		a_format = ((blk_ptr[(i + 1)] & 0x78) >> 3);
		edid_audio_info.format = a_format;
		EDID_MSG("       Format Code          = %d (%s)\n\r",
			 a_format, format_strgs[a_format&0x0F]);
		EDID_MSG("        Max. No. of Channels= %x\n\r",
			 ((blk_ptr[i+1] & 0x07) + 1));

		edid_audio_info.max_channels = (blk_ptr[(i + 1)] & 0x07) + 1;
		a_sample = (blk_ptr[(i + 2)] & 0x7f);
		edid_audio_info.sample_rate = a_sample;
		EDID_MSG("        Sampling Freq. (KHz)= %s%s%s%s%s%s%s\n\r",
			 (a_sample&0x01) ? "32  " : "",
			 (a_sample&0x02) ? "44.1  " : "",
			 (a_sample&0x04) ? "48  " : "",
			 (a_sample&0x08) ? "88.2  " : "",
			 (a_sample&0x10) ? "96  " : "",
			 (a_sample&0x20) ? "176.4  " : "",
			 (a_sample&0x40) ? "192" : "");

		a_bit = blk_ptr[(i + 3)];
		edid_audio_info.bit = a_bit;
		if(a_format == 0x01) {
			EDID_MSG("        length (bits)       = %s%s%s\n\r",
				 (a_bit&0x01) ? "16  " : "",
				 (a_bit&0x02) ? "20  " : "",
				 (a_bit&0x04) ? "24" : "");
		} else if((a_format >= 2) && (a_format <= 8)) {
			EDID_MSG("        Max. Bit Rate       = %d KHz\n\r",
				 (a_bit * 8));
			edid_audio_info.bit = a_bit*8;
		} else if(a_format >= 9 && a_format <= 15) {
			if(a_bit)
				EDID_MSG("        Vendor Code         = %d\n\r",
					 a_bit);
		}
	}
}

/**
 * Display and save short video descriptors.
 */
static void edid_parse_display_short_video(uint8_t *blk_ptr,
		uint8_t desc_l, uint8_t *blk_start)
{
	uint8_t i, vic;

	edid_info.svd_offset = (uint16_t)((uint32_t)blk_ptr -
					  (uint32_t)blk_start) + 1;
	for(i = 0; i < desc_l; i++) {
		vic = blk_ptr[(i + 1)] & 0x7f;
		EDID_MSG("       %s VIC=%d\n\r",
			 (blk_ptr[(i + 1)] & 0x80) ? "*" : " ", vic);
	}
}

/**
 * Display deep color options of the sink.
 */
static void edid_parse_display_deep_color(uint8_t *blk_ptr)
{
	if(blk_ptr[6] & 0x10)
		EDID_MSG("       30-bit Deep color (RGB)\n\r");
	if(blk_ptr[6] & 0x20)
		EDID_MSG("       36-bit Deep color (RGB)\n\r");
	if(blk_ptr[6] & 0x40)
		EDID_MSG("       48-bit Deep color (RGB)\n\r");
	if(blk_ptr[6] & 0x08)
		EDID_MSG("       YCbCr Deep color supported\n\r");
}

/**
 * Display content type of the sink.
 */
static void edid_parse_display_cnt_type(uint8_t *blk_ptr)
{
	if(blk_ptr[8] & 0x01)
		EDID_MSG("       Content Type: Graphics (CNC0)\n\r");
	if(blk_ptr[8] & 0x02)
		EDID_MSG("       Content Type: Photo    (CNC1)\n\r");
	if(blk_ptr[8] & 0x04)
		EDID_MSG("       Content Type: Cinema   (CNC2)\n\r");
	if(blk_ptr[8] & 0x08)
		EDID_MSG("       Content Type: Game     (CNC3)\n\r");
}

/**
 * Display 3D structure capabilities of the sink.
 */
static void edid_parse_display_3d_cap(uint8_t video_3d_structure_all_h,
				      uint8_t video_3d_structure_all_l)
{
	if(video_3d_structure_all_l & 0x01)
		EDID_MSG("        Sink supports Frame Packing 3D formats.\n\r");
	if(video_3d_structure_all_l & 0x02)
		EDID_MSG("        Sink supports Field Alternative 3D formats which consists of field picture.\n\r");
	if(video_3d_structure_all_l & 0x04)
		EDID_MSG("        Sink supports Line Alternative 3D formats.\n\r");
	if(video_3d_structure_all_l & 0x08)
		EDID_MSG("        Sink supports Side-by-Side (Full) 3D formats.\n\r");
	if(video_3d_structure_all_l & 0x10)
		EDID_MSG("        Sink supports L + Depth 3D formats.\n\r");
	if(video_3d_structure_all_l & 0x20)
		EDID_MSG("        Sink supports L + Depth + Graphics + Graphics-Depth 3D formats.\n\r");
	if(video_3d_structure_all_h & 0x01)
		EDID_MSG("        Sink supports Side-by-Side (Half) with all Sub-sampling Methods 3D formats.\n\r");
}

/**
 * Display 3d structure options of the sink.
 */
static void edid_parse_display_3d_struct(uint8_t *blk_ptr, uint8_t offs)
{
	if((blk_ptr[offs] & 0x0F) == 0x00)
		EDID_MSG("        3D_Structure is : Frame Packing\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x01)
		EDID_MSG("        3D_Structure is : Field Alternative\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x02)
		EDID_MSG("        3D_Structure is : Line Alternative\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x03)
		EDID_MSG("        3D_Structure is : Side-by-Side  (Full)\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x04)
		EDID_MSG("        3D_Structure is : L + Depth\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x05)
		EDID_MSG("        3D_Structure is : L + Depth + Graphics + Graphics-Depth\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x08)
		EDID_MSG("        3D_Structure is : Side-by-Side  (Half)\n\r");
}

/**
 * Display 3d subsampling options of the sink.
 */
static void edid_parse_display_3d_subsmpl(uint8_t *blk_ptr, uint8_t offs)
{
	if((blk_ptr[offs] & 0x0F) == 0x00)
		EDID_MSG("Sink supports all eight combination of sub-sampling position\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x01)
		EDID_MSG("Horizontal sub-sampling : Sink supports all four combination of sub-sampling position\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x02)
		EDID_MSG("Horizontal sub-sampling : Odd/Left picture, Odd/Right picture\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x03)
		EDID_MSG("Horizontal sub-sampling : Odd/Left picture, Even/Right picture\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x04)
		EDID_MSG("Horizontal sub-sampling : Even/Left picture, Odd/Right picture\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x05)
		EDID_MSG("Horizontal sub-sampling : Even/Left picture, Even/Right picture\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x06)
		EDID_MSG("Quincunx Matrix : Sink supports all four combination of sub-sampling position\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x07)
		EDID_MSG("Quincunx Matrix : Odd/Left picture, Odd/Right picture\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x08)
		EDID_MSG("Quincunx Matrix : Odd/Left picture, Even/Right picture\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x09)
		EDID_MSG("Quincunx Matrix : Even/Left picture, Odd/Right picture\n\r");
	if((blk_ptr[offs] & 0x0F) == 0x0A)
		EDID_MSG("Quincunx Matrix : Even/Left picture, Even/Right picture\n\r");
}

/**
 * Display 3D structure details of the sink.
 */
static void edid_parse_display_3d_detail(uint8_t *blk_ptr,
		uint8_t hdmi_3d_len, uint8_t offs)
{
	uint8_t i;

	for(i = 0; i < hdmi_3d_len; i++) {
		EDID_MSG("       2D_VIC_order_X  = %d, 3D_Structure_X  = %d\n\r",
			 ((blk_ptr[(offs + i)] & 0xF0) >> 4),
			 (blk_ptr[(offs + i)] & 0x0F));

		edid_parse_display_3d_struct(blk_ptr, (offs + i));

		if((blk_ptr[(offs + i)] & 0x0F) >= 8) {
			EDID_MSG("         3D_Detail_X  = %d, ",
				 ((blk_ptr[(offs + i + 1)] & 0xF0) >> 4));

			if((blk_ptr[(offs + i)] & 0x0F) == 8) {
				edid_parse_display_3d_subsmpl(blk_ptr, (offs + i + 1));
			}

			i++;
		}
	}
}

/**
 * Display and save the VSDB HDMI identification code.
 */
static void edid_parse_display_vsdb(uint8_t *blk_ptr,
				    uint8_t desc_l, uint8_t **spa_ptr)
{
	bool additional_video = false, interlaced_latency = false;
	bool audio_video_latency = false;
	uint8_t k, i;
	uint8_t video_3d_present, video_3d_multi_present, hdmiviclen, hdmi_3d_len;
	uint8_t video_3d_structure_all_h = 0, video_3d_structure_all_l = 0;
	uint8_t video_3d_mask_h = 0, video_3d_mask_l = 0;

	if ((blk_ptr[1] != 0x03) || (blk_ptr[2] != 0x0c) ||
	    (blk_ptr[3] != 0x00))
		return;

	*spa_ptr = blk_ptr + 4;
	if(desc_l > 5) {
		edid_info.deep_color = (blk_ptr[6] >> 4) & 0x07;
		edid_parse_display_deep_color(blk_ptr);
	}
	if(desc_l > 6)
		if(blk_ptr[7])
			EDID_MSG("       Max TMDS clock = %d MHz\n\r",
				 blk_ptr[7] * 5);
	if(desc_l > 7) {
		edid_parse_display_cnt_type(blk_ptr);
		if(blk_ptr[8] & 0x20)
			additional_video = true;
		if(blk_ptr[8] & 0x40)
			interlaced_latency = true;
		if(blk_ptr[8] & 0x80)
			audio_video_latency = true;
	}
	k = 9; /* Next blk_ptr pointer */
	if(desc_l > 8) {
		if(audio_video_latency) {
			EDID_MSG("       Video Latency is 0x%x , Audio Latency is 0x%x\n\r",
				 blk_ptr[k], blk_ptr[(k + 1)]);
			k += 2;
		}
		if(interlaced_latency) {
			EDID_MSG("       Interlaced Video Latency is 0x%x , Interlaced Audio Latency is 0x%x\n\r",
				 blk_ptr[k], blk_ptr[(k + 1)]);
			k += 2;
		}
	}
	if((desc_l > 8) && additional_video) {
		video_3d_present = (blk_ptr[k] & 0x80) >> 7;
		video_3d_multi_present = (blk_ptr[k] & 0x70) >> 5;
		EDID_MSG("       3D_present = %d, 3D_multi_present = %d, ",
			 video_3d_present, video_3d_multi_present);
		k += 1;
		hdmiviclen = (blk_ptr[k] & 0xE0) >> 5;
		hdmi_3d_len = blk_ptr[k] & 0x1F;
		EDID_MSG("HDMI_VIC_LEN = %d, HDMI_3D_LEN = %d\n\r",
			 hdmiviclen, hdmi_3d_len);

		if(video_3d_present == 0)
			EDID_MSG("       HDMI Sink does not support3D formats\n\r");

		if(hdmi_3d_len == 0)
			EDID_MSG("       HDMI Sink supports only mandatory 3D formats\n\r");

		for(i = 1; i < (hdmiviclen + 1); i++) /* extended video resolutions */
			EDID_MSG("       HDMI_VIC_%d  = 0x%x\n\r", i,
				 blk_ptr[(k + i)]);
		k += (hdmiviclen + 1);

		/* 3D resolutions */
		if(video_3d_present == 0 || hdmi_3d_len == 0)
			return;

		if(video_3d_multi_present == 1 ||
		    video_3d_multi_present == 2) {
			video_3d_structure_all_h = blk_ptr[k];
			video_3d_structure_all_l = blk_ptr[(k + 1)];
			EDID_MSG("       Video3DStructureAll = 0x%02x%02x\n\r",
				 video_3d_structure_all_h,
				 video_3d_structure_all_l);
			k += 2;
			hdmi_3d_len -= 2;

			edid_parse_display_3d_cap(video_3d_structure_all_h,
						  video_3d_structure_all_l);
		}
		if( video_3d_multi_present == 2) {
			video_3d_mask_h = blk_ptr[k];
			video_3d_mask_l = blk_ptr[(k + 1)];
			EDID_MSG("       Video3DMask = 0x%02x%02x\n\r",
				 video_3d_mask_h, video_3d_mask_l);
			k += 2;
			hdmi_3d_len -= 2;
		}
		edid_parse_display_3d_detail(blk_ptr, hdmi_3d_len, k);
	}
}

/**
 * Display and save the speaker allocation data block
 */
static void edid_parse_display_speaker_alloc(uint8_t *blk_ptr,
		uint8_t desc_l)
{
	uint8_t i;

	for(i = 0; i < desc_l; i++)
		EDID_MSG("       0x%02x\n\r", blk_ptr[(i + 1)]);
}

/**
 * Display and save the extended tag data block
 */
static void edid_parse_display_ext_tag(uint8_t *blk_ptr, uint8_t desc_l)
{
	uint8_t i;

	i = blk_ptr[1];
	EDID_MSG("       Extended tag code is %d\n\r", i);
	if(i == 5)
		EDID_MSG("       Colorimetry data block 0x%x 0x%x\n\r",
			 blk_ptr[2], blk_ptr[3]);
	if(i == 0)
		EDID_MSG("       Video Capability Data Block (VCDB) 0x%x\n\r",
			 blk_ptr[2]);
}

/**
 * Parse the data part of an CEA block.
 *
 * edid_parse_cea_block() helper function.
 *
 * @param [in] blk_ptr - Pointer to the data part in the EDID block.
 * @param [in] len - Length of DBC area.
 * @param [out] spa_ptr - Pointer to receive a pointer to the SPA.
 *
 * @return void
 */
static void edid_parse_data_blk_collection(uint8_t *blk_ptr, uint8_t len,
		uint8_t **spa_ptr)
{
	uint8_t tag, desc_l, *blk_start;
	const char *strgs[] = {
		"Reserved", "Audio", "Video", "VSDB", "Speaker allocation",
		"VESA DTC", "Reserved", "Extended tag"
	};

	*spa_ptr = NULL;
	blk_start = blk_ptr;

	EDID_MSG("Data block collection information:\n\r");
	while(len) {
		tag = (blk_ptr[0] & 0xe0) >> 5;
		desc_l = blk_ptr[0] & 0x1f;
		EDID_MSG("    %s data block\n\r", strgs[tag]);
		switch(tag) {
		case 1:
			/*======================================
			 * Short Audio Descriptors
			 *=====================================*/
			edid_parse_display_short_audio(blk_ptr, desc_l);
			break;
		case 2:
			/*======================================
			 * Short Video Descriptors
			 *=====================================*/
			edid_parse_display_short_video(blk_ptr, desc_l, blk_start);
			break;
		case 3:
			/*======================================
			 * VSDB HDMI identification code
			 *=====================================*/
			edid_parse_display_vsdb(blk_ptr, desc_l, spa_ptr);
			break;
		case 4:
			/*======================================
			 * Speaker allocation Data Block
			 *=====================================*/
			edid_parse_display_speaker_alloc(blk_ptr, desc_l);
			break;
		case 7:
			/*======================================
			 * Extended tag data block
			 *=====================================*/
			edid_parse_display_ext_tag(blk_ptr, desc_l);
			break;
		}
		desc_l++;
		len -= desc_l;
		blk_ptr += desc_l;
	}
}


#if EDID_3D_REPLACEMENT   /*defined: to filter out chip not supported 3D */
/*=====================================================================================
 * static uint16_t edid_get_chip_3d_format(uint8_t vic, uint8_t deepcolor)
 * Description: to get 3D Formats which are supported by CHIP forthe particular VIC
 * Parameters:  vic: VIC number
 *              deepcolor: SINK supported DeepColor
 * Return:      3D formats supported by CHIP
 *=====================================================================================*/
static uint16_t edid_get_chip_3d_format(uint8_t vic, uint8_t deepcolor)
{
	uint16_t chip3drx, chip3dtx, val;

	chip3drx = VIC_3D_Table_RX[vic];    /* Rx chip supported 3D format */
	chip3dtx = VIC_3D_Table_TX[vic];    /* Tx chip supported 3D format */
#if(TX_DEVICE == 7510) /* 3D format fordeep color >= 36-bit is different */
	for(val = 0; val < 3; val++)
		if(vic == VIC_3D_DeepColor_TX[(3 * val)])
			if(deepcolor >= VIC_3D_DeepColor_TX[(3 * val + 1)]) {
				chip3dtx = VIC_3D_DeepColor_TX[(3 * val + 2)];
				break;
			}
#endif
	val = chip3drx & chip3dtx;     /* both Rx and Tx chips supported 3D formats */
	return val;
}

/*===========================================================================
 * static void data_blk_3d_filter(uint8_t *edid_blk)
 * Description: to check SINK supported 3D Formast;
 *              then to filter out CHIP not supported 3D Formats
 * Parameters:  edid_blk: porter of EDID Block-1
 * Return:      EDID Block-1 with 3D supported by both CHIP and SINK
 *==========================================================================*/
static void data_blk_3d_filter(uint8_t *edid_blk)
{
	uint8_t blk_buf[128];
	uint8_t i, addr, bitx, bity;
	uint8_t tag, length, hdmi_3d_len, vic;
	uint8_t t_offset, vsdb_offset, v3d_offset;
	uint8_t v3d_vic_buf[16], v3d_vic_len, v3d_deep_color;
	uint16_t v3d_structure, v3d_mask, v3d_chip, v3d_support;
	uint16_t v2d_order_buf[16], mn;

	v3d_deep_color = RX_CD_24; /* DeepColor will be used to check 3D format */
	vsdb_offset = 0; /* VSDB offset in Block-1 */
	v3d_offset = 0; /* 3D_present offset in VSDB */
	v3d_vic_len = 0; /* number of VICs in EDID */
	t_offset = edid_blk[2]; /* Timing Description offset in Block-1 */

	for(i = 4; i < t_offset; i = (i + (length + 1))) {
		tag = (edid_blk[i] & 0xE0) >> 5;
		length = (edid_blk[i]) & 0x1F;

		/* Video Data Block */
		if(tag == 2) {
			if(length < 16) /* at most first 16 VICs */
				v3d_vic_len = length;
			else
				v3d_vic_len = 16;

			for(mn = 0; mn < v3d_vic_len; mn++) {
				vic = edid_blk[(i + 1 + mn)] & 0x7f;
				v3d_vic_buf[mn] = vic; /* to backup VIC for3D checking later */
			}
		}

		if(tag == 3) { /* VSDB: Vendor Specific Data Block */
			if((edid_blk[(i + 1)] == 0x03) && (edid_blk[(i + 2)] == 0x0c) &&
			    (edid_blk[(i + 3)] == 0x00)) { /* ID = 0x000C03 */
				if(length > 7) { /* has enough data */
					if(edid_blk[(i + 6)] & 0x10)
						v3d_deep_color = RX_CD_30;
					if(edid_blk[(i + 6)] & 0x20)
						v3d_deep_color = RX_CD_36;
					if(edid_blk[(i + 6)] & 0x40)
						v3d_deep_color = RX_CD_48;

					if(edid_blk[(i + 8)] & 0x20) { /* HDMI_Video_present = 1 */
						vsdb_offset = i;
						v3d_offset = 9;
						if(edid_blk[(i + 8)] & 0x40) /* I_Latency_Fields_Present = 1 */
							v3d_offset += 2;
						if(edid_blk[(i + 8)] & 0x80) /* Latency_Fields_Present = 1 */
							v3d_offset += 2;

						if(v3d_offset >= length) {
							v3d_offset = 0;
						} else {
							if((edid_blk[(i + v3d_offset)] & 0x80) == 0) /* 3D_present = 0, no 3D */
								v3d_offset = 0;
							if((edid_blk[(i + v3d_offset + 1)] & 0x1F) ==
							    0) /* HDMI_3D_LEN = 0, only mandatory 3D format */
								v3d_offset = 0;
						}
					}
				}
			}
		} /* end of if(tag==3) */
	}
	if(v3d_vic_len == 0)
		v3d_offset = 0; /* when no valid VIC, no 3D supported */

	if(v3d_offset) { /* Vendor Specific Data Block with 3D present */
		/*=============================================================
		 * to copy EDID from first byte to the end of VSDB to blk_buf[]
		 *=============================================================*/
		length = edid_blk[vsdb_offset] & 0x1F;
		memcpy(blk_buf, edid_blk, (length + 1 + vsdb_offset));

		/*=====================================================
		 * to get 3D_Structure_ALL_15..0 & 3D_MASK_15..0
		 *=====================================================*/
		addr = vsdb_offset + v3d_offset;
		v3d_mask = (blk_buf[addr] & 0x60) >> 5;    /* = 3D_Multi_present */
		hdmi_3d_len = blk_buf[(addr + 1)] & 0x1F;    /* = HDMI_3D_LEN */
		length = (blk_buf[(addr + 1)] & 0xE0) >> 5;    /* = HDMI_VIC_LEN */
		addr = addr + 2 + length;
		v3d_structure = (blk_buf[addr] << 8) | (blk_buf[(addr +
							1)]); /* 3D_Structure_All */
		v3d_support = v3d_structure; /* v3d_support forSINK & chip supported formats */
		if(v3d_mask == 2) { /* has 3D_MASK*/
			v3d_mask = (blk_buf[(addr + 2)] << 8) | (blk_buf[(addr + 3)]);  /* 3D_MASK */
			tag = 2;
		} else {
			v3d_mask = 0xFFFF; /* all 16-bit are enabled */
			tag = 0;
		}

		/* ===================================================================
		 * to update 3D_Structure_ALL according to CHIP supported 3D Formats
		 * =================================================================== */
		EDID_MSG("------ SINK has 3D Formats, to compare with CHIP supported 3D Formats ------ \n\r");
		for(i = 0; i < v3d_vic_len; i++) { /* to check 3D support forVICs in EDID */
			v2d_order_buf[i] = 0;    /* default: no 3D formats for2D_order_x */
			if((v3d_mask >> i) &
			    0x01) { /* 1. only when 3D_Mask bit=1, 3D_Structure_ALL used forthis VIC */
				vic = v3d_vic_buf[i];    /* VIC number */
				v3d_chip = edid_get_chip_3d_format(vic, v3d_deep_color);
				EDID_MSG("        VIC=%d, SINK 3D = 0x%04x, CHIP 3D = 0x%04x \n\r",
					 vic, v3d_structure, v3d_chip);
				mn = v3d_chip &
				     v3d_structure;     /* 2. to get chip & sink supported format at the particular VIC */
				v3d_support = mn &
					      v3d_support;    /* 3. to filter out not supported 3D format forall VICs */
				if(v3d_support != mn) {
					v2d_order_buf[i] = mn -
							   v3d_support;    /* 4. to backup format supported by this VIC, not by all VICs */
					EDID_MSG("           CHANGE EDID: 3D_STRUCTURE_ALL:0x%04x \n\r",
						 v3d_support);
				}
			}
		}
		blk_buf[addr]= (v3d_support & 0xFF00) >>
			       8;     /* 5. to udpate 3D_Structure_ALL */
		blk_buf[(addr + 1)] = v3d_support & 0x00FF;
		addr = addr + 2 +
		       tag;    /* move to address of 2D_VIC_order_x & 3D_Structure_x */
		hdmi_3d_len = hdmi_3d_len - 2 - tag;

		/* ==================================================================================
		 * to update 2D_VIC_order_X & 3D_Structure_X according to CHIP supported 3D Formats
		 * ==================================================================================*/
		length = 0;
		i = 0;
		while(i < hdmi_3d_len) {
			v3d_structure = blk_buf[(addr + i)] & 0x0F; /* = 3D_Structure_X */
			tag = 0;
			if(v3d_structure >= 8) /* has 3D_Detail_x */
				tag = 1;
			mn = (blk_buf[(addr + i)] & 0xF0) >> 4;    /* = 2D_VIC_order_X */
			vic = v3d_vic_buf[mn];
			v3d_chip = edid_get_chip_3d_format(vic, v3d_deep_color);
			EDID_MSG("       2D_VIC_order_%02d=%02d, VIC=%03d, 3D_Structure_X=%02d, Chip Support=0x%04x \n\r",
				 i, mn, vic, v3d_structure, v3d_chip);
			v3d_support = v3d_chip & (1<< v3d_structure);
			if(v3d_support  == 0) {
				EDID_MSG("           CHANGE EDID: this 3D_Structure is NOT supported by CHIP, to remove it \n\r");
				length = length + 1 + tag;  /* = number of 2D_VIC_order_x to be removed */
				hdmi_3d_len = hdmi_3d_len - 1 - tag;
				/* ========================================================================================
				 * to remove this 2D_VIC_order_x and 3D_Structure_x by moving forward of other data
				 * ======================================================================================== */
				for(mn=i; mn<hdmi_3d_len; mn++)
					blk_buf[(addr + mn)] = blk_buf[(addr + mn + 1 + tag)];
			} else {
				i = i + 1 + tag;    /* move to next one */
			}
		} /* end of: while() for2D_order_x & 3D_STRUCTURE_X */
		/* ========================================================================================
		 * to reduce the length & HDMI_3D_LEN in Vendor Specific Data Block
		 * ========================================================================================*/
		mn = vsdb_offset + v3d_offset;
		bitx = blk_buf[(mn + 1)] & 0xE0;
		bity = (blk_buf[(mn + 1)] & 0x1F) - length;     /* to modify HDMI_3D_LEN */
		blk_buf[(mn + 1)] = bitx | bity;
		bitx = blk_buf[vsdb_offset] & 0xE0;
		bity = (blk_buf[vsdb_offset] & 0x1F) - length;   /* to modify length of VSDB */
		blk_buf[vsdb_offset] = bitx | bity;

		/* ============================================================================================
		 * to add formats which are removed from 3D_STRUCTURE_ALL to 2D_VIC_order_x and 3D_STRUCTURE_x
		 * ============================================================================================*/
		length = 0;
		addr += i;
		for(i = 0; i < v3d_vic_len; i++) { /* all VICs */
			v3d_structure = v2d_order_buf[i];
			if(v3d_structure) { /* some formats to be added */
				vic = (i << 4) & 0xF0;
				for(mn = 0; mn < 16; mn++) { /* 16-bit for3D formats */
					if(v3d_structure & (1 << mn)) { /* ifone format is supported */
						EDID_MSG("          ADD: 2D_VIC_order_x=%02d, 3D_Structure_X=%02d \n\r", i, mn);
						blk_buf[addr] = vic | mn;
						addr++;
						length++;
						if(mn > 7) {
							blk_buf[addr] = 0;
							addr++;
							length++;
						}
					}
				}
			}
		}
		/* ========================================================================================
		 * to add the length & HDMI_3D_LEN in Vendor Specific Data Block
		 * ========================================================================================*/
		mn = vsdb_offset + v3d_offset;
		bitx = blk_buf[(mn + 1)] & 0xE0;
		bity = (blk_buf[(mn + 1)] & 0x1F) + length;     /* to modify HDMI_3D_LEN */
		blk_buf[(mn + 1)] = bitx | bity;
		bitx = blk_buf[vsdb_offset] & 0xE0;
		bity = (blk_buf[vsdb_offset] & 0x1F) + length;   /* to modify length of VSDB */
		blk_buf[vsdb_offset] = bitx | bity;

		/* ====================================
		 * to modify the Offset of Block-1
		 * ===================================*/
		t_offset = blk_buf[2];
		bitx = edid_blk[vsdb_offset] & 0x1F;    /* length of original VSDB */
		bity = blk_buf[vsdb_offset] & 0x1F;    /* length of new VSDB */
		blk_buf[2] = t_offset - bitx +
			     bity;   /* Timing Description offset in Block-1 */
		tag = vsdb_offset + bitx + 1;     /* address of original EDID to be copied */

		/* ====================================
		 * to update EDID buffer
		 * ===================================*/
		length = 127 - tag;          /* number of data to be copied */
		mn = 126;              /* number of Padding, 127th forchecksum */
		while(edid_blk[mn] == 0) { /* to find out Padding */
			mn--;
			length--;
		}
		mn = 127 - addr;
		while(mn < length) { /* while(spare space) < data to be copied  */
			length -= 18;
			EDID_MSG("          DELETE One Timing Descriptor to save 18-byte \n\r");
		}
		memcpy((blk_buf + addr), (edid_blk + tag), length);
		for(i = (addr + length); i < 127; i++)
			blk_buf[i] = 0;    /* Padding: 0 */
		blk_buf[127] = edid_get_block_checksum(blk_buf, 1);  /* to update checksum */
		memcpy(edid_blk, blk_buf, 128); /* to copy back */

	}    /* end of: if(v3d_offset) */
}

#endif/* end of: EDID_3D_REPLACEMENT */

/**
 * Parse and display parameters of a CEA extension block.
 *
 * This is a helper method of the module available only inside it.
 *
 * @param [in]  blk_ptr - Pointer to the CEA extension block.
 * @param [out] spa_offset - Pointer to the SPA offset, or 0 in case of error.
 * @param [in] block_num - Number of EDID block.
 *
 * @return true if success, false if block error.
 */
static bool edid_parse_cea_block(uint8_t *blk_ptr, uint16_t *spa_offset,
				 uint8_t block_num)
{
	uint8_t *spa_ptr;
	struct cea_ext *cea_blk;

	EDID_MSG("\n\r========================= EDID BLOCK %d =========================\n\r",
		 block_num);
	cea_blk = (struct cea_ext *)blk_ptr;
	if(edid_get_block_checksum(blk_ptr, 1) != blk_ptr[127]) {
		EDID_MSG("Checksum failed on block %d.\n\r", block_num);
		return false;
	}
	if(cea_blk->tag != 2) {
		EDID_MSG("Not a CEA Extension block (tag 0x%x). Exiting\n\r",
			 cea_blk->tag);
		if(cea_blk->tag == 0xF0)
			EDID_MSG(" This is an EDID Block Map Extension\n\r");
		return true;
	}
	EDID_MSG("CEA extension block revision %d\n\r", cea_blk->rev_num);
	if(cea_blk->desc_offs == 0) {
		EDID_MSG("\n\rEmpty EDID Block (d=0)\n\r");
		return true;
	}
	if(cea_blk->desc_offs && (cea_blk->desc_offs < 4)) {
		EDID_MSG("\n\rUndefined EDID Block (d=%d)\n\r", cea_blk->desc_offs);
		return true;
	}
	if(cea_blk->rev_num > 1) {
		EDID_MSG("Underscan=%s  Audio=%s  YCbCr4:4:4=%s  YCbCr4:2:2=%s\n\r",
			 (cea_blk->res & 0x80) ? "Yes" : "No",
			 (cea_blk->res & 0x40) ? "Yes" : "No",
			 (cea_blk->res & 0x20) ? "Yes" : "No",
			 (cea_blk->res & 0x10) ? "Yes" : "No");
	}
	if(cea_blk->desc_offs > 4) {
#if EDID_3D_REPLACEMENT
		data_blk_3d_filter(blk_ptr);    /* to filter out not supported 3D formats */
#endif
		edid_parse_data_blk_collection(cea_blk->data, cea_blk->desc_offs - 4,
					       &spa_ptr);
		if(edid_info.svd_offset) {
			edid_info.svd_offset--;   /* Point to the start of the SVD */
			edid_info.svd_offset += (uint16_t) ((uint32_t)(cea_blk->data) -
							    (uint32_t)cea_blk);
			edid_info.svd_offset += (((uint16_t)block_num)*128);
		}
		if(spa_ptr) {
			edid_info.svd_offset = (uint16_t)(((uint32_t)spa_ptr) -
							  ((uint32_t)cea_blk) +
							  (((uint32_t)block_num) * 128));
			*spa_offset = edid_info.svd_offset;
			EDID_MSG("SPA location is at 0x%x, SPA = %d.%d.%d.%d\n\r",
				 *spa_offset,
				 (spa_ptr[0] & 0xf0) >> 4, (spa_ptr[0] & 0x0f),
				 (spa_ptr[1] & 0xf0) >> 4, (spa_ptr[1] & 0x0f));
		}
	}
	if(cea_blk->desc_offs)
		edid_parse_desc(((uint8_t *)cea_blk) + cea_blk->desc_offs,
				127 - cea_blk->desc_offs);
	blk_ptr[127] = edid_get_block_checksum(blk_ptr, 1);

	return true;
}

/**
 * Parse and display the EDID message received.
 *
 * @param [in] edid_ptr - Pointer to the EDID message.
 * @param [out] spa_offset - Pointer to the SPA offset or 0
 * @param [in] seg_num - EDID segment number.
 *
 * @return number of total additional segments, 0 if none or if checksum error
 *         in segment 0
 */
uint8_t ADIAPI_mw_edid_parse(uint8_t *edid_ptr, uint16_t *spa_offset,
			     uint8_t seg_num)
{
	struct edid_struct *edid;

	if(seg_num == 0) {
		EDID_MSG("------------------------- EDID BLOCK 0 -------------------------\n\r");

		edid = (struct edid_struct *)edid_ptr;
		*spa_offset = 0;
		blk_cnt = edid->ext_blk_count;
		blk_num = 1;

		/*=============================
		 * Init EDID info
		 *============================*/
		edid_info.deep_color = 0;
		edid_info.spa_offset = 0;
		edid_info.svd_offset = 0;
		edid_info.csum_err = false;

		edid_audio_info.format = 0;
		edid_audio_info.sample_rate = 0;
		edid_audio_info.max_channels = 0;
		edid_audio_info.bit = 0;

		if(edid_get_block_checksum(edid_ptr, 0) != edid_ptr[127]) {
			EDID_MSG("Checksum failed on block 0.\n\r");
			edid_info.csum_err = true;
			return 0;
		}
		EDID_MSG("Edid Version %d.%d\n\r", edid->edid_rev[0],
			 edid->edid_rev[1]);

		edid_parse_desc((uint8_t *)(&(edid->detailed_timing)), 72);
		edid_ptr[127] = edid_get_block_checksum(edid_ptr, 0);

		EDID_MSG("Edid extensions blocks: %d\n\r", blk_cnt);
	}

	if(edid_info.csum_err)
		return 0;

	while((blk_num <= blk_cnt) && (blk_num <= 3)) {
		if(!edid_parse_cea_block((edid_ptr + ((blk_num&1)*128)),
					 spa_offset, blk_num)) {
			edid_info.csum_err = true;
			return 0;
		}
		blk_num++;
		if((blk_num & 1) == 0)
			break;
	}

	if((blk_num > blk_cnt) || (blk_num > 3)) {
		EDID_MSG("########################### EDID END ###########################\n\r\n\r");
	} else {
		EDID_MSG("\n\r************************* SEGMENT %d END ************************\n\r\n\r",
			 seg_num);
	}
	*spa_offset = edid_info.spa_offset;

	return (blk_cnt >> 1);
}

/**
 * Parse the EDID message received without displaying to the terminal.
 *
 * @param [in] edid_ptr - Pointer to the EDID message.
 * @param [out] spa_offset - Pointer to the SPA offset or 0
 * @param [in] seg_num - EDID segment number.
 *
 * @return number of total additional segments, 0 if none or if checksum error
 *         in segment 0
 */
uint8_t ADIAPI_mw_edid_parse_silent(uint8_t *edid_ptr, uint16_t *spa_offset,
				    uint8_t seg_num)
{
	uint8_t ret_val;
	bool edid_msg_org;

	edid_msg_org = edid_msg_enable;
	edid_msg_enable = false;
	ret_val = ADIAPI_mw_edid_parse(edid_ptr, spa_offset, seg_num);
	edid_msg_enable = edid_msg_org;

	return ret_val;
}

/**
 * Get the information about the last parsed EDID.
 *
 * @param [out] info_ptr - Pointer to the EDID structure where to copy the
 *                         information.
 *
 * @return void
 */
void ADIAPI_mw_edid_get_info(struct edid_info *info_ptr)
{
	memcpy(info_ptr, &edid_info, sizeof(struct edid_info));
}

/**
 * Get the audio information about the last parsed EDID.
 *
 * @param [out] audio_info_ptr - Pointer to the EDID structure where to
 *                               copy the information.
 *
 * @return void
 */
void ADIAPI_mw_edid_get_audio_info(struct edid_audio_info *audio_info_ptr)
{
	memcpy(audio_info_ptr, &edid_audio_info, sizeof(struct edid_audio_info));
}

/**
 * Enable or disable EDID verbose output.
 *
 * Useful for debugging and getting information about the sink.
 *
 * @param [in] enable - True to enable sending EDID information to the serial
 *                      connection, false to disable it.
 *
 * @return void
 */
void ADIAPI_mw_edid_enable_debug_msg(bool enable)
{
	edid_msg_enable = enable;
}

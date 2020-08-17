/***************************************************************************//**
*   @file   edid.h
*   @brief  EDID control module header.
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

#ifndef _EDID_H_
#define _EDID_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "wrapper.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* when defined, SINK 3D is filtered by chip supported 3D formats */
#define EDID_MSG if(edid_msg_enable)DBG_MSG
#define STD_DESC_LEN sizeof(struct std_timing)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct std_timing {
	uint8_t    pixel_clk[2];  /* Divide by 100 to get MHZ */
	uint8_t    h_active;      /* In pixels */
	uint8_t    h_blanking;
	uint8_t    h_act_blnk44;
	uint8_t    v_active;      /* In lines */
	uint8_t    v_blanking;
	uint8_t    v_act_blnk44;
	uint8_t    h_sync_offs;   /* In pixels */
	uint8_t    h_sync_width;
	uint8_t    v_offs_pulse;  /* In lines */
	uint8_t    hv_offs_pulse; /* H offs: H sync width: V Offs: V sync width */
	uint8_t    h_sizemm;
	uint8_t    v_sizemm;
	uint8_t    hv_sizemm44;
	uint8_t    h_border;
	uint8_t    v_border;
	uint8_t    flags;
};

struct strg_desc {
	uint8_t    zero_3[3];
	uint8_t    tag;           /* 0xfc=MonName 0xfe=String 0xff=MonSerial */
	uint8_t    zero;
	uint8_t    string[13];
};

struct mon_range_desc {
	uint8_t    zero_3[3];
	uint8_t    tag_0xfd;
	uint8_t    zero;
	uint8_t    min_v_freq;    /* Hz */
	uint8_t    max_v_freq;
	uint8_t    min_h_freq;
	uint8_t    max_h_freq;
	uint8_t    max_pix_clk;   /* Multiply by 10 to get MHz */
	uint8_t    sec_timing_tag;
	uint8_t    sec_timing[7];
};

struct vsdb_block {
	uint8_t    tag_len;               /* b0-4=Len  b5-7=Tag(3)    */
	uint8_t    reg_id[3];              /* 0x03 0x0C 0x00           */
	uint8_t    spa[2];                /* AB:CD                    */
	uint8_t    deep_clr;               /* If Len > 5               */
	uint8_t    max_tmds;               /* If Len > 5               */
	uint8_t    latency_flags;          /* If Len > 5               */
};

/* Structure that maps an EDID block 0 */
struct edid_struct {
	uint8_t    header[8];              /* 0x00 */
	uint8_t    man_id[2];               /* 0x08 */
	uint8_t    prod_code[2];
	uint8_t    serial[4];
	uint8_t    man_week;
	uint8_t    man_year;
	uint8_t    edid_rev[2];             /* 0x12 */
	uint8_t    vid_inp_def;              /* 0x14 */
	uint8_t    max_h_size_cm;
	uint8_t    max_v_size_cm;
	uint8_t    disp_gamma;
	uint8_t    feat_support;
	uint8_t    color_specs[10];         /* 0x19 */
	uint8_t    est_timing[3];           /* 0x23 */
	uint8_t    stnd_timing[16];         /* 0x26 */
	uint8_t    detailed_timing[72];     /* 0x36 */
	uint8_t    ext_blk_count;            /* 0x7E */
	uint8_t    checksum;               /* 0x7F */
};

/* Structure that maps an EDID extension block (CEA block) */
struct cea_ext {
	uint8_t    tag;
	uint8_t    rev_num;
	uint8_t    desc_offs;
	uint8_t    res;
	uint8_t    data[123];
	uint8_t    checksum;
};

struct edid_info {
	uint8_t   deep_color;
	uint16_t  spa_offset;
	bool      csum_err;
	uint16_t  svd_offset;
};

struct edid_audio_info {
	uint8_t   format;
	uint8_t   sample_rate;
	uint8_t   max_channels;
	uint16_t  bit;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Parse and display the EDID message received. */
uint8_t ADIAPI_mw_edid_parse(uint8_t *edid_ptr, uint16_t *spa_offset,
			     uint8_t seg_num);

/* Parse the EDID message received without displaying to the terminal. */
uint8_t ADIAPI_mw_edid_parse_silent(uint8_t *edid_ptr, uint16_t *spa_offset,
				    uint8_t seg_num);

/* Get the information about the last parsed EDID. */
void ADIAPI_mw_edid_get_info(struct edid_info *info_ptr);

/* Get the audio information about the last parsed EDID. */
void ADIAPI_mw_edid_get_audio_info(struct edid_audio_info *audio_info_ptr);

/* Enable or disable EDID verbose output. */
void ADIAPI_mw_edid_enable_debug_msg(bool enable);

#endif

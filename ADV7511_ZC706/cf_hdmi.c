/***************************************************************************//**
 *   @file   cf_hdmi.c
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "xil_io.h"
#include "xparameters.h"
#include "cf_hdmi.h"
#include "cf_hdmi_demo.h"
#include "xil_cache.h"

/***************************************************************************//**
 * @brief DDR write.
 *
 * @return None.
*******************************************************************************/
void DDRVideoWr(void)
{
	u32 n;
	u32 d;
	u32 dcnt;

	dcnt = 0;
	xil_printf("DDR write: started (length %d)\n\r", IMG_LENGTH);
	for (n = 0; n < IMG_LENGTH; n++)
	{
		for (d = 0; d < ((IMG_DATA[n]>>24) & 0xff); d++)
		{
			Xil_Out32((VIDEO_BASEADDR+(dcnt*4)), (IMG_DATA[n] & 0xffffff));
			dcnt = dcnt + 1;
		}
	}
	Xil_DCacheFlush();
	xil_printf("DDR write: completed (total %d)\n\r", dcnt);
}

/***************************************************************************//**
 * @brief DDR write.
 *
 * @return None.
*******************************************************************************/
void DDRAudioWr(void)
{
	u32 n;
	u32 scnt;
	u32 sincr;

	xil_printf("DDR audio write: started\n\r");
	scnt = 0;
	sincr = (65536*2)/AUDIO_LENGTH;
	for (n = 0; n < 32; n++)
	{
		Xil_Out32((AUDIO_BASEADDR+(n*4)), 0x00); // init descriptors
	}
	Xil_Out32((AUDIO_BASEADDR+0x00), (AUDIO_BASEADDR + 0x40)); // next descriptor
	Xil_Out32((AUDIO_BASEADDR+0x08), (AUDIO_BASEADDR + 0x80)); // start address
	Xil_Out32((AUDIO_BASEADDR+0x40), (AUDIO_BASEADDR + 0x00)); // next descriptor
	Xil_Out32((AUDIO_BASEADDR+0x48), (AUDIO_BASEADDR + 0x80)); // start address
	Xil_Out32((AUDIO_BASEADDR+0x18), (0x8000000 | (AUDIO_LENGTH*8))); // no. of bytes
	Xil_Out32((AUDIO_BASEADDR+0x58), (0x4000000 | (AUDIO_LENGTH*8))); // no. of bytes
	Xil_Out32((AUDIO_BASEADDR+0x1c), 0x00); // status
	Xil_Out32((AUDIO_BASEADDR+0x5c), 0x00); // status
	for (n = 0; n < AUDIO_LENGTH; n++)
	{
		Xil_Out32((AUDIO_BASEADDR+0x80+(n*4)), ((scnt << 16) | scnt));
		scnt = (n > (AUDIO_LENGTH/2)) ? (scnt-sincr) : (scnt+sincr);
	}
	Xil_DCacheFlush();
	xil_printf("DDR audio write: completed (total %d)\n\r", AUDIO_LENGTH);
}

/***************************************************************************//**
 * @brief Initialize the HDMI Video Pcore.
 *
 * @return None.
*******************************************************************************/
void InitHdmiVideoPcore(void)
{
	DDRVideoWr();

	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000003); // enable circular mode
	Xil_Out32((VDMA_BASEADDR + 0x05c), VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x060), VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x064), VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x058), (H_STRIDE*4)); // h offset (2048 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x054), (H_ACTIVE*4)); // h size (1920 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x050), V_ACTIVE); // v size (1080)

	Xil_Out32((CFV_BASEADDR + 0x08), ((H_WIDTH << 16) | H_COUNT));
	Xil_Out32((CFV_BASEADDR + 0x0c), ((H_DE_MIN << 16) | H_DE_MAX));
	Xil_Out32((CFV_BASEADDR + 0x10), ((V_WIDTH << 16) | V_COUNT));
	Xil_Out32((CFV_BASEADDR + 0x14), ((V_DE_MIN << 16) | V_DE_MAX));
	Xil_Out32((CFV_BASEADDR + 0x04), 0x00000000); // disable
	Xil_Out32((CFV_BASEADDR + 0x04), 0x00000001); // enable
}

/***************************************************************************//**
 * @brief Initialize the HDMI Audio Pcore.
 *
 * @return None.
*******************************************************************************/
void InitHdmiAudioPcore(void)
{
	DDRAudioWr();

	Xil_Out32((CFA_BASEADDR + 0x04), 0x040); // sample frequency
	Xil_Out32((CFA_BASEADDR + 0x00), 0x103); // clock ratio, data enable & signal enable
}

/***************************************************************************//**
 * @brief Generate audio clicks.
 *
 * @return None.
*******************************************************************************/
void AudioClick(void)
{
    /* Generating audio clicks. */
    Xil_Out32((AUDIO_BASEADDR+0x1c), 0x00); // status
    Xil_Out32((AUDIO_BASEADDR+0x5c), 0x00); // status
    Xil_DCacheFlush();
    Xil_Out32((ADMA_BASEADDR + 0x00), 0); // clear dma operations
    Xil_Out32((ADMA_BASEADDR + 0x08), AUDIO_BASEADDR); // head descr.
	Xil_Out32((ADMA_BASEADDR + 0x00), 1); // enable dma operations
    Xil_Out32((ADMA_BASEADDR + 0x10), (AUDIO_BASEADDR+0x40)); // tail descr.
}

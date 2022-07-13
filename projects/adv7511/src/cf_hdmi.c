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
#include "axi_dmac.h"
#include "clk_axi_clkgen.h"
#if defined (_XPARAMETERS_PS_H_)
#include "xdmaps.h"
#endif

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/
#define MIN(x, y)				(x < y ? x : y)
#define MAX(x, y) 				(x > y ? x : y)
#define NO_OS_DIV_ROUND_UP(x,y)		(((x) + (y) - 1) / (y))
#define CLAMP(val, min, max)	(val < min ? min : (val > max ? max :val))
#define ABS(x)					(x < 0 ? -x : x)

enum detailedTimingElement {
	PIXEL_CLOCK,
	H_ACTIVE_TIME,
	H_BLANKING_TIME,
	H_SYNC_OFFSET,
	H_SYNC_WIDTH_PULSE,
	V_ACTIVE_TIME,
	V_BLANKING_TIME,
	V_SYNC_OFFSET,
	V_SYNC_WIDTH_PULSE
};

static const unsigned long detailedTiming[7][9] = {
	{25180000, 640, 144, 16, 96, 480, 29, 10, 2},
	{40000000, 800, 256, 40, 128, 600, 28, 1, 4},
	{65000000, 1024, 320, 136, 24, 768, 38, 3, 6},
	{74250000, 1280, 370, 110, 40, 720, 30, 5, 5},
	{84750000, 1360, 416, 136, 72, 768, 30, 3, 5},
	{108000000, 1600, 400, 32, 48, 900, 12, 3, 6},
	{148500000, 1920, 280, 44, 88, 1080, 45, 4, 5}
};

/***************************************************************************//**
 * @brief DDRVideoWr.
*******************************************************************************/
void DDRVideoWr(unsigned short horizontalActiveTime,
		unsigned short verticalActiveTime)
{
	unsigned long  pixel      = 0;
	unsigned long  backup     = 0;
	unsigned short line       = 0;
	unsigned long  index      = 0;
	unsigned char  repetition = 0;

	while(line < verticalActiveTime) {
		for(index = 0; index < IMG_LENGTH; index++) {
			for (repetition = 0; repetition < ((IMG_DATA[index]>>24) & 0xff);
			     repetition++) {
				backup = pixel;
				while((pixel - line*horizontalActiveTime) < horizontalActiveTime) {
					Xil_Out32((VIDEO_BASEADDR+(pixel*4)), (IMG_DATA[index] & 0xffffff));
					pixel += 640;
				}
				pixel = backup;
				if((pixel - line*horizontalActiveTime) < 639) {
					pixel++;
				} else {
					line++;
					if(line == verticalActiveTime) {
						Xil_DCacheFlush();
						return;
					}
					pixel = line*horizontalActiveTime;
				}
			}
		}
	}
	Xil_DCacheFlush();
}

/***************************************************************************//**
 * @brief DDRAudioWr.
*******************************************************************************/
void DDRAudioWr(void)
{
	u32 n     = 0;
	u32 scnt  = 0;
	u32 sincr = 0;

	sincr = (65536*2)/AUDIO_LENGTH;
#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
	for (n = 0; n < 32; n++) {
		Xil_Out32((AUDIO_BASEADDR+(n*4)), 0x00); // init descriptors
	}
	Xil_Out32((AUDIO_BASEADDR+0x00), (AUDIO_BASEADDR + 0x40)); // next descriptor
	Xil_Out32((AUDIO_BASEADDR+0x08), (AUDIO_BASEADDR + 0x80)); // start address
	Xil_Out32((AUDIO_BASEADDR+0x40), (AUDIO_BASEADDR + 0x00)); // next descriptor
	Xil_Out32((AUDIO_BASEADDR+0x48), (AUDIO_BASEADDR + 0x80)); // start address
	Xil_Out32((AUDIO_BASEADDR+0x18),
		  (0x8000000 | (AUDIO_LENGTH*8))); // no. of bytes
	Xil_Out32((AUDIO_BASEADDR+0x58),
		  (0x4000000 | (AUDIO_LENGTH*8))); // no. of bytes
	Xil_Out32((AUDIO_BASEADDR+0x1c), 0x00); // status
	Xil_Out32((AUDIO_BASEADDR+0x5c), 0x00); // status
#endif
	for (n = 0; n < AUDIO_LENGTH; n++) {
#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
		Xil_Out32((AUDIO_BASEADDR+0x80+(n*4)), ((scnt << 16) | scnt));
#elif defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
		Xil_Out32((AUDIO_BASEADDR+(n*4)), ((scnt << 16) | scnt));
#endif
		scnt = (n > (AUDIO_LENGTH/2)) ? (scnt-sincr) : (scnt+sincr);
	}
	Xil_DCacheFlush();
}

/***************************************************************************//**
 * @brief InitHdmiVideoPcore.
*******************************************************************************/
void InitHdmiVideoPcore(unsigned short horizontalActiveTime,
			unsigned short horizontalBlankingTime,
			unsigned short horizontalSyncOffset,
			unsigned short horizontalSyncPulseWidth,
			unsigned short verticalActiveTime,
			unsigned short verticalBlankingTime,
			unsigned short verticalSyncOffset,
			unsigned short verticalSyncPulseWidth)
{
	unsigned short horizontalCount	   = 0;
	unsigned short verticalCount	   = 0;
	unsigned short horizontalBackPorch = 0;
	unsigned short verticalBackPorch   = 0;
	unsigned short horizontalDeMin	   = 0;
	unsigned short horizontalDeMax	   = 0;
	unsigned short verticalDeMin	   = 0;
	unsigned short verticalDeMax	   = 0;

	DDRVideoWr(horizontalActiveTime, verticalActiveTime);

	horizontalCount = horizontalActiveTime +
			  horizontalBlankingTime;
	verticalCount = verticalActiveTime +
			verticalBlankingTime;
	horizontalBackPorch = horizontalBlankingTime -
			      horizontalSyncOffset -
			      horizontalSyncPulseWidth;
	verticalBackPorch = verticalBlankingTime -
			    verticalSyncOffset -
			    verticalSyncPulseWidth;
	horizontalDeMin = horizontalSyncPulseWidth +
			  horizontalBackPorch;
	horizontalDeMax = horizontalDeMin +
			  horizontalActiveTime;
	verticalDeMin = verticalSyncPulseWidth +
			verticalBackPorch;
	verticalDeMax = verticalDeMin +
			verticalActiveTime;

	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_HTIMING1),
		  ((horizontalActiveTime << 16) | horizontalCount));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_HTIMING2),
		  horizontalSyncPulseWidth);
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_HTIMING3),
		  ((horizontalDeMax << 16) | horizontalDeMin));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_VTIMING1),
		  ((verticalActiveTime << 16) | verticalCount));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_VTIMING2),
		  verticalSyncPulseWidth);
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_VTIMING3),
		  ((verticalDeMax << 16) | verticalDeMin));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_RESET), 0x1);
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_SOURCE_SEL), 0x0);
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_SOURCE_SEL), 0x1);

#if defined(PLATFORM_ZED) || defined(PLATFORM_ZC702) || \
	defined(PLATFORM_ZC706)
	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_CTRL,
		  0x0); // reset DMAC
	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_CTRL,
		  AXI_DMAC_CTRL_ENABLE); // enable DMAC
	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_FLAGS,
		  DMA_CYCLIC | DMA_LAST); // enable circular mode
	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_SRC_ADDRESS,
		  VIDEO_BASEADDR); // start address
	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_X_LENGTH,
		  ((horizontalActiveTime*4)-1)); // h size


	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_SRC_STRIDE,
		  (horizontalActiveTime*4)); // h offset
	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_Y_LENGTH,
		  (verticalActiveTime-1)); // v size
	Xil_Out32(VDMA_BASEADDR + AXI_DMAC_REG_TRANSFER_SUBMIT,
		  0x1); // submit transfer	Xil_Out32(VDMA_BASEADDR + DMAC_REG_CTRL,
#else
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_DMA_CTRL),
		  0x00000003); // enable circular mode
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_START_1),
		  VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_START_2),
		  VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_START_3),
		  VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_FRMDLY_STRIDE),
		  (horizontalActiveTime*4)); // h offset
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_H_SIZE),
		  (horizontalActiveTime*4)); // h size
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_V_SIZE),
		  verticalActiveTime); // v size
#endif
}

/***************************************************************************//**
 * @brief SetVideoResolution.
*******************************************************************************/
void SetVideoResolution(struct axi_clkgen *clkgen, unsigned char resolution)
{
	axi_clkgen_set_rate(clkgen, detailedTiming[resolution][PIXEL_CLOCK]);
	InitHdmiVideoPcore(detailedTiming[resolution][H_ACTIVE_TIME],
			   detailedTiming[resolution][H_BLANKING_TIME],
			   detailedTiming[resolution][H_SYNC_OFFSET],
			   detailedTiming[resolution][H_SYNC_WIDTH_PULSE],
			   detailedTiming[resolution][V_ACTIVE_TIME],
			   detailedTiming[resolution][V_BLANKING_TIME],
			   detailedTiming[resolution][V_SYNC_OFFSET],
			   detailedTiming[resolution][V_SYNC_WIDTH_PULSE]);
}

/***************************************************************************//**
 * @brief InitHdmiAudioPcore.
*******************************************************************************/
void InitHdmiAudioPcore(void)
{
#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
	DDRAudioWr();
#endif

	Xil_Out32((CFA_BASEADDR + 0x04), 0x040); // sample frequency
#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
	Xil_Out32((CFA_BASEADDR + 0x00),
		  0xc03); // clock ratio, data enable & signal enable
#elif defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
	Xil_Out32((CFA_BASEADDR + 0x00),
		  0x103); // clock ratio, data enable & signal enable
#endif
}

/***************************************************************************//**
 * @brief AudioClick.
*******************************************************************************/
void AudioClick(void)
{
#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
	/* Generating audio clicks. */
	Xil_Out32((AUDIO_BASEADDR+0x1c), 0x00); // status
	Xil_Out32((AUDIO_BASEADDR+0x5c), 0x00); // status
	Xil_DCacheFlush();
	Xil_Out32((ADMA_BASEADDR + 0x00), 0); // clear dma operations
	Xil_Out32((ADMA_BASEADDR + 0x08), AUDIO_BASEADDR); // head descr.
	Xil_Out32((ADMA_BASEADDR + 0x00), 1); // enable dma operations
	Xil_Out32((ADMA_BASEADDR + 0x10), (AUDIO_BASEADDR+0x40)); // tail descr.
#elif defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
	u32 			Status;
	XDmaPs_Cmd		DmaCmd;
	XDmaPs			DmaInstance;
	XDmaPs			*DmaInst = &DmaInstance;
	XDmaPs_Config	*DmaCfg;

	memset(&DmaCmd, 0, sizeof DmaCmd);

	DmaCmd.ChanCtrl.EndianSwapSize	= 0;
	DmaCmd.ChanCtrl.DstCacheCtrl 	= 0;
	DmaCmd.ChanCtrl.DstProtCtrl 	= 0;
	DmaCmd.ChanCtrl.DstBurstLen 	= 1;
	DmaCmd.ChanCtrl.DstBurstSize 	= 4;
	DmaCmd.ChanCtrl.DstInc 			= 0;
	DmaCmd.ChanCtrl.SrcCacheCtrl 	= 0;
	DmaCmd.ChanCtrl.SrcProtCtrl 	= 0;
	DmaCmd.ChanCtrl.SrcBurstLen 	= 1;
	DmaCmd.ChanCtrl.SrcBurstSize 	= 4;
	DmaCmd.ChanCtrl.SrcInc 			= 1;
	DmaCmd.BD.SrcAddr = (u32) AUDIO_BASEADDR;
	DmaCmd.BD.DstAddr = (u32) (CFA_BASEADDR + 0x0C);
	DmaCmd.BD.Length = AUDIO_LENGTH * 4;

	/* DMAC Program */

	DmaCfg = XDmaPs_LookupConfig(ADMA_DEVICE_ID);
	if (DmaCfg == NULL)
		xil_printf("XDmaPs_LookupConfig() Failed\n\r");

	Status = XDmaPs_CfgInitialize(DmaInst, DmaCfg, DmaCfg->BaseAddress);
	if (Status != XST_SUCCESS)
		xil_printf("XDmaPs_CfgInitialize() Failed\n\r");

	DDRAudioWr();

	Status = XDmaPs_Start(DmaInst, 0, &DmaCmd, 0);
	if (Status != XST_SUCCESS)
		xil_printf("XDmaPs_Start() Failed\n\r");
#endif
}

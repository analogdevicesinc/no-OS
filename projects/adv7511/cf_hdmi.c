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
#include "dmac_core.h"
#if defined (_XPARAMETERS_PS_H_)
#include "xdmaps.h"
#endif

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/
#define MIN(x, y)				(x < y ? x : y)
#define MAX(x, y) 				(x > y ? x : y)
#define DIV_ROUND_UP(x,y)		(((x) + (y) - 1) / (y))
#define DIV_ROUND_CLOSEST(x, y)	(unsigned long)(((double)x / y) + 0.5)
#define CLAMP(val, min, max)	(val < min ? min : (val > max ? max :val))
#define ABS(x)					(x < 0 ? -x : x)

static const unsigned long clkgen_filter_table[] =
{
	0x01001990, 0x01001190, 0x01009890, 0x01001890,
	0x01008890, 0x01009090, 0x01009090, 0x01009090,
	0x01009090, 0x01000890, 0x01000890, 0x01000890,
	0x08009090, 0x01001090, 0x01001090, 0x01001090,
	0x01001090, 0x01001090, 0x01001090, 0x01001090,
	0x01001090, 0x01001090, 0x01001090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x08001090, 0x08001090, 0x08001090,
	0x08001090, 0x08001090, 0x08001090, 0x08001090,
	0x08001090, 0x08001090, 0x08001090
};

static const unsigned long clkgen_lock_table[] =
{
	0x060603e8, 0x060603e8, 0x080803e8, 0x0b0b03e8,
	0x0e0e03e8, 0x111103e8, 0x131303e8, 0x161603e8,
	0x191903e8, 0x1c1c03e8, 0x1f1f0384, 0x1f1f0339,
	0x1f1f02ee, 0x1f1f02bc, 0x1f1f028a, 0x1f1f0271,
	0x1f1f023f, 0x1f1f0226, 0x1f1f020d, 0x1f1f01f4,
	0x1f1f01db, 0x1f1f01c2, 0x1f1f01a9, 0x1f1f0190,
	0x1f1f0190, 0x1f1f0177, 0x1f1f015e, 0x1f1f015e,
	0x1f1f0145, 0x1f1f0145, 0x1f1f012c, 0x1f1f012c,
	0x1f1f012c, 0x1f1f0113, 0x1f1f0113, 0x1f1f0113
};

enum detailedTimingElement
{
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

static const unsigned long detailedTiming[7][9] =
{
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

	while(line < verticalActiveTime)
	{
		for(index = 0; index < IMG_LENGTH; index++)
		{
			for (repetition = 0; repetition < ((IMG_DATA[index]>>24) & 0xff); repetition++)
			{
				backup = pixel;
				while((pixel - line*horizontalActiveTime) < horizontalActiveTime)
				{
					Xil_Out32((VIDEO_BASEADDR+(pixel*4)), (IMG_DATA[index] & 0xffffff));
					pixel += 640;
				}
				pixel = backup;
				if((pixel - line*horizontalActiveTime) < 639)
				{
					pixel++;
				}
				else
				{
					line++;
					if(line == verticalActiveTime)
					{
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
#if (CURRENT_PLATFORM == PLATFORM_KC705) || \
	(CURRENT_PLATFORM == PLATFORM_AC701) || (CURRENT_PLATFORM == PLATFORM_VC707)
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
#if (CURRENT_PLATFORM == PLATFORM_KC705) || \
	(CURRENT_PLATFORM == PLATFORM_AC701) || (CURRENT_PLATFORM == PLATFORM_VC707)
		Xil_Out32((AUDIO_BASEADDR+0x80+(n*4)), ((scnt << 16) | scnt));
#elif (CURRENT_PLATFORM == PLATFORM_ZC702) || \
		(CURRENT_PLATFORM == PLATFORM_ZC706) || \
		(CURRENT_PLATFORM == PLATFORM_ZED)
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

#if (CURRENT_PLATFORM == PLATFORM_ZED) || \
	(CURRENT_PLATFORM == PLATFORM_ZC702) || \
	(CURRENT_PLATFORM == PLATFORM_ZC706)
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_CTRL,
			  0x0); // reset DMAC
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_CTRL,
			  DMAC_CTRL_ENABLE); // enable DMAC
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_FLAGS,
			  DMAC_FLAGS_CYCLIC | DMAC_FLAGS_TLAST); // enable circular mode
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_SRC_ADDRESS,
			  VIDEO_BASEADDR); // start address
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_X_LENGTH,
			  ((horizontalActiveTime*4)-1)); // h size
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_SRC_STRIDE,
			  (horizontalActiveTime*4)); // h offset
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_Y_LENGTH,
			  (verticalActiveTime-1)); // v size
	Xil_Out32(VDMA_BASEADDR + DMAC_REG_START_TRANSFER,
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
void SetVideoResolution(unsigned char resolution)
{
	CLKGEN_SetRate(detailedTiming[resolution][PIXEL_CLOCK], 200000000);
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
#if (CURRENT_PLATFORM == PLATFORM_KC705) || \
	(CURRENT_PLATFORM == PLATFORM_AC701) || (CURRENT_PLATFORM == PLATFORM_VC707)
	DDRAudioWr();
#endif

	Xil_Out32((CFA_BASEADDR + 0x04), 0x040); // sample frequency
#if (CURRENT_PLATFORM == PLATFORM_KC705) || \
	(CURRENT_PLATFORM == PLATFORM_AC701) || (CURRENT_PLATFORM == PLATFORM_VC707)
	Xil_Out32((CFA_BASEADDR + 0x00),
		  0xc03); // clock ratio, data enable & signal enable
#elif (CURRENT_PLATFORM == PLATFORM_ZC702) || \
		(CURRENT_PLATFORM == PLATFORM_ZC706) || \
		(CURRENT_PLATFORM == PLATFORM_ZED)
	Xil_Out32((CFA_BASEADDR + 0x00),
		  0x103); // clock ratio, data enable & signal enable
#endif
}

/***************************************************************************//**
 * @brief AudioClick.
*******************************************************************************/
void AudioClick(void)
{
#if (CURRENT_PLATFORM == PLATFORM_KC705) || \
	(CURRENT_PLATFORM == PLATFORM_AC701) || (CURRENT_PLATFORM == PLATFORM_VC707)
	/* Generating audio clicks. */
	Xil_Out32((AUDIO_BASEADDR+0x1c), 0x00); // status
	Xil_Out32((AUDIO_BASEADDR+0x5c), 0x00); // status
	Xil_DCacheFlush();
	Xil_Out32((ADMA_BASEADDR + 0x00), 0); // clear dma operations
	Xil_Out32((ADMA_BASEADDR + 0x08), AUDIO_BASEADDR); // head descr.
	Xil_Out32((ADMA_BASEADDR + 0x00), 1); // enable dma operations
	Xil_Out32((ADMA_BASEADDR + 0x10), (AUDIO_BASEADDR+0x40)); // tail descr.
#elif (CURRENT_PLATFORM == PLATFORM_ZC702) || \
		(CURRENT_PLATFORM == PLATFORM_ZC706) || \
		(CURRENT_PLATFORM == PLATFORM_ZED)
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

/***************************************************************************//**
 * @brief CLKGEN_LookupFilter.
*******************************************************************************/
static unsigned long CLKGEN_LookupFilter(unsigned long m)
{
	if (m < 47)
	{
		return clkgen_filter_table[m];
	}
	return 0x08008090;
}

/***************************************************************************//**
 * @brief CLKGEN_LookupLock.
*******************************************************************************/
static unsigned long CLKGEN_LookupLock(unsigned long m)
{
	if (m < 36)
	{
		return clkgen_lock_table[m];
	}
	return 0x1f1f00fa;
}

/***************************************************************************//**
 * @brief CLKGEN_CalcParams.
*******************************************************************************/
void CLKGEN_CalcParams(unsigned long fin,
					   unsigned long fout,
					   unsigned long *best_d,
					   unsigned long *best_m,
					   unsigned long *best_dout)
{
	const unsigned long fpfd_min = 10000;
	const unsigned long fpfd_max = 300000;
	const unsigned long fvco_min = 600000;
	const unsigned long	fvco_max = 1200000;
	unsigned long		d		 = 0;
	unsigned long		d_min	 = 0;
	unsigned long		d_max	 = 0;
	unsigned long		_d_min	 = 0;
	unsigned long		_d_max	 = 0;
	unsigned long		m		 = 0;
	unsigned long		m_min	 = 0;
	unsigned long		m_max	 = 0;
	unsigned long		dout	 = 0;
	unsigned long		fvco	 = 0;
	long				f		 = 0;
	long				best_f	 = 0;

	fin /= 1000;
	fout /= 1000;

	best_f = 0x7fffffff;
	*best_d = 0;
	*best_m = 0;
	*best_dout = 0;

	d_min = MAX(DIV_ROUND_UP(fin, fpfd_max), 1);
	d_max = MIN(fin / fpfd_min, 80);

	m_min = MAX(DIV_ROUND_UP(fvco_min, fin) * d_min, 1);
	m_max = MIN(fvco_max * d_max / fin, 64);

	for(m = m_min; m <= m_max; m++)
	{
		_d_min = MAX(d_min, DIV_ROUND_UP(fin * m, fvco_max));
		_d_max = MIN(d_max, fin * m / fvco_min);

		for (d = _d_min; d <= _d_max; d++)
		{
			fvco = fin * m / d;

			dout = DIV_ROUND_CLOSEST(fvco, fout);
			dout = CLAMP(dout, 1, 128);
			f = fvco / dout;
			if (ABS(f - fout) < ABS(best_f - fout))
			{
				best_f = f;
				*best_d = d;
				*best_m = m;
				*best_dout = dout;
				if (best_f == fout)
				{
					return;
				}
			}
		}
	}
}

/***************************************************************************//**
 * @brief CLKGEN_CalcClkParams.
*******************************************************************************/
void CLKGEN_CalcClkParams(unsigned long divider,
						  unsigned long *low,
						  unsigned long *high,
						  unsigned long *edge,
						  unsigned long *nocount)
{
	if (divider == 1)
	{
		*nocount = 1;
	}
	else
	{
		*nocount = 0;
	}
	*high = divider / 2;
	*edge = divider % 2;
	*low = divider - *high;
}

/***************************************************************************//**
 * @brief CLKGEN_Write.
*******************************************************************************/
void CLKGEN_Write(unsigned long reg,
				  unsigned long val)
{
	Xil_Out32(CF_CLKGEN_BASEADDR + reg, val);
}

/***************************************************************************//**
 * @brief CLKGEN_Read.
*******************************************************************************/
static void CLKGEN_Read(unsigned long reg,
						unsigned long *val)
{
	*val = Xil_In32(CF_CLKGEN_BASEADDR + reg);
}

/***************************************************************************//**
 * @brief CLKGEN_MMCMRead.
*******************************************************************************/
static void CLKGEN_MMCMRead(unsigned long reg,
							unsigned long *val)
{
	unsigned long timeout = 1000000;
	unsigned long reg_val;

	do {
		CLKGEN_Read(AXI_CLKGEN_V2_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & AXI_CLKGEN_V2_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	reg_val = AXI_CLKGEN_V2_DRP_CNTRL_SEL | AXI_CLKGEN_V2_DRP_CNTRL_READ;
	reg_val |= (reg << 16);

	CLKGEN_Write(AXI_CLKGEN_V2_REG_DRP_CNTRL, 0x00);
	CLKGEN_Write(AXI_CLKGEN_V2_REG_DRP_CNTRL, reg_val);
	do {
		CLKGEN_Read(AXI_CLKGEN_V2_REG_DRP_STATUS, val);
	} while ((*val & AXI_CLKGEN_V2_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	*val &= 0xffff;
}

/***************************************************************************//**
 * @brief CLKGEN_MMCMWrite.
*******************************************************************************/
void CLKGEN_MMCMWrite(unsigned long reg,
					  unsigned long val,
					  unsigned long mask)
{
	unsigned long timeout = 1000000;
	unsigned long reg_val;

	do {
		CLKGEN_Read(AXI_CLKGEN_V2_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & AXI_CLKGEN_V2_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	if (mask != 0xffff) {
		CLKGEN_MMCMRead(reg, &reg_val);
		reg_val &= ~mask;
	} else {
		reg_val = 0;
	}

	reg_val |= AXI_CLKGEN_V2_DRP_CNTRL_SEL | (reg << 16) | (val & mask);

	Xil_Out32(CF_CLKGEN_BASEADDR + AXI_CLKGEN_V2_REG_DRP_CNTRL, 0x00);
	Xil_Out32(CF_CLKGEN_BASEADDR + AXI_CLKGEN_V2_REG_DRP_CNTRL, reg_val);
}

/***************************************************************************//**
 * @brief CLKGEN_MMCMEnable.
*******************************************************************************/
static void CLKGEN_MMCMEnable(char enable)
{
        unsigned long val = AXI_CLKGEN_V2_RESET_ENABLE;

        if (enable)
                val |= AXI_CLKGEN_V2_RESET_MMCM_ENABLE;

        CLKGEN_Write(AXI_CLKGEN_V2_REG_RESET, val);
}

/***************************************************************************//**
 * @brief CLKGEN_SetRate.
*******************************************************************************/
int CLKGEN_SetRate(unsigned long rate,
				   unsigned long parent_rate)
{
	unsigned long d		  = 0;
	unsigned long m		  = 0;
	unsigned long dout	  = 0;
	unsigned long nocount = 0;
	unsigned long high	  = 0;
	unsigned long edge	  = 0;
	unsigned long low	  = 0;
	unsigned long filter  = 0;
	unsigned long lock	  = 0;

	if (parent_rate == 0 || rate == 0)
	{
		return 0;
	}

	CLKGEN_CalcParams(parent_rate, rate, &d, &m, &dout);

	if (d == 0 || dout == 0 || m == 0)
	{
		return 0;
	}

	filter = CLKGEN_LookupFilter(m - 1);
	lock = CLKGEN_LookupLock(m - 1);

	CLKGEN_MMCMEnable(0);

	CLKGEN_CalcClkParams(dout, &low, &high, &edge, &nocount);
	CLKGEN_MMCMWrite(MMCM_REG_CLKOUT0_1, (high << 6) | low, 0xefff);
	CLKGEN_MMCMWrite(MMCM_REG_CLKOUT0_2, (edge << 7) | (nocount << 6), 0x03ff);


	CLKGEN_CalcClkParams(d, &low, &high, &edge, &nocount);
	CLKGEN_MMCMWrite(MMCM_REG_CLK_DIV, (edge << 13) | (nocount << 12) | (high << 6) | low, 0x3fff);

	CLKGEN_CalcClkParams(m, &low, &high, &edge, &nocount);
	CLKGEN_MMCMWrite(MMCM_REG_CLK_FB1, (high << 6) | low, 0xefff);
	CLKGEN_MMCMWrite(MMCM_REG_CLK_FB2, (edge << 7) | (nocount << 6), 0x03ff);

	CLKGEN_MMCMWrite(MMCM_REG_LOCK1, lock & 0x3ff, 0x3ff);
	CLKGEN_MMCMWrite(MMCM_REG_LOCK2, (((lock >> 16) & 0x1f) << 10) | 0x1, 0x7fff);
	CLKGEN_MMCMWrite(MMCM_REG_LOCK3, (((lock >> 24) & 0x1f) << 10) | 0x3e9, 0x7fff);
	CLKGEN_MMCMWrite(MMCM_REG_FILTER1, filter >> 16, 0x9900);
	CLKGEN_MMCMWrite(MMCM_REG_FILTER2, filter, 0x9900);
	CLKGEN_MMCMEnable(1);

	return 0;
}

/***************************************************************************//**
 * @brief CLKGEN_GetRate.
*******************************************************************************/
unsigned long CLKGEN_GetRate(unsigned long parent_rate)
{
	unsigned long d, m, dout;
	unsigned long reg;
	unsigned long long tmp;

	CLKGEN_MMCMRead(MMCM_REG_CLKOUT0_1, &reg);
	dout = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	CLKGEN_MMCMRead(MMCM_REG_CLK_DIV, &reg);
	d = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	CLKGEN_MMCMRead(MMCM_REG_CLK_FB1, &reg);
	m = (reg & 0x3f) + ((reg >> 6) & 0x3f);

	if (d == 0 || dout == 0)
		return 0;

	tmp = (unsigned long long)(parent_rate / d) * m;
	tmp = tmp / dout;

	if (tmp > 0xffffffff)
	{
		return 0xffffffff;
	}

	return (unsigned long)tmp;
}

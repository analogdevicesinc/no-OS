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

/***************************************************************************//**
 * @brief DDRVideoWr.
*******************************************************************************/
void DDRVideoWr(void)
{
	u32 n    = 0;
	u32 d    = 0;
	u32 dcnt = 0;

	for (n = 0; n < IMG_LENGTH; n++)
	{
		for (d = 0; d < ((IMG_DATA[n]>>24) & 0xff); d++)
		{
			Xil_Out32((VIDEO_BASEADDR+(dcnt*4)), (IMG_DATA[n] & 0xffffff));
			dcnt = dcnt + 1;
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
}

/***************************************************************************//**
 * @brief InitHdmiVideoPcore.
*******************************************************************************/
void InitHdmiVideoPcore(unsigned short hActive,
						unsigned short vActive,
						unsigned short hBlanking,
						unsigned short vBlanking,
						unsigned short hWidth,
						unsigned short vWidth,
						unsigned short hOffset,
						unsigned short vOffset)
{
	unsigned short hCount	  = 0;
	unsigned short vCount	  = 0;
	unsigned short hBackPorch = 0;
	unsigned short vBackPorch = 0;
	unsigned short hDeMin	  = 0;
	unsigned short hDeMax	  = 0;
	unsigned short vDeMin	  = 0;
	unsigned short vDeMax	  = 0;

	DDRVideoWr();

	hCount = hActive + hBlanking;
	vCount = vActive + vBlanking;
	hBackPorch = hBlanking - hOffset - hWidth;
	vBackPorch = vBlanking - vOffset - vWidth;
	hDeMin = hWidth + hBackPorch;
	hDeMax = hDeMin + hActive;
	vDeMin = vWidth + vBackPorch;
	vDeMax = vDeMin + vActive;

	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_HTIMING1), ((hWidth << 16) | hCount));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_HTIMING2), ((hDeMin << 16) | hDeMax));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_VTIMING1), ((vWidth << 16) | vCount));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_VTIMING2), ((vDeMin << 16) | vDeMax));
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_CTRL), 0x00000000); // disable
	Xil_Out32((CFV_BASEADDR + AXI_HDMI_REG_CTRL), 0x00000001); // enable

	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_DMA_CTRL), 0x00000003); // enable circular mode
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_START_1), VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_START_2), VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_START_3), VIDEO_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_FRMDLY_STRIDE), (hActive*4)); // h offset
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_H_SIZE), (hActive*4)); // h size
	Xil_Out32((VDMA_BASEADDR + AXI_VDMA_REG_V_SIZE), vActive); // v size
}

/***************************************************************************//**
 * @brief InitHdmiAudioPcore.
*******************************************************************************/
void InitHdmiAudioPcore(void)
{
	DDRAudioWr();

	Xil_Out32((CFA_BASEADDR + 0x04), 0x040); // sample frequency
	Xil_Out32((CFA_BASEADDR + 0x00), 0xc03); // clock ratio, data enable & signal enable
}

/***************************************************************************//**
 * @brief AudioClick.
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
	Xil_Out32(XPAR_AXI_CLKGEN_0_BASEADDR + reg, val);
}

/***************************************************************************//**
 * @brief CLKGEN_Read.
*******************************************************************************/
static void CLKGEN_Read(unsigned long reg,
						unsigned long *val)
{
	*val = Xil_In32(XPAR_AXI_CLKGEN_0_BASEADDR + reg);
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

	CLKGEN_Write(AXI_CLKGEN_REG_UPDATE_ENABLE, 0);

	CLKGEN_CalcClkParams(dout, &low, &high, &edge, &nocount);
	CLKGEN_Write(AXI_CLKGEN_REG_CLK_OUT1, (high << 6) | low);
	CLKGEN_Write(AXI_CLKGEN_REG_CLK_OUT2, (edge << 7) | (nocount << 6));

	CLKGEN_CalcClkParams(d, &low, &high, &edge, &nocount);
	CLKGEN_Write(AXI_CLKGEN_REG_CLK_DIV, (edge << 13) | (nocount << 12) | (high << 6) | low);

	CLKGEN_CalcClkParams(m, &low, &high, &edge, &nocount);
	CLKGEN_Write(AXI_CLKGEN_REG_CLK_FB1, (high << 6) | low);
	CLKGEN_Write(AXI_CLKGEN_REG_CLK_FB2, (edge << 7) | (nocount << 6));

	CLKGEN_Write(AXI_CLKGEN_REG_LOCK1, lock & 0x3ff);
	CLKGEN_Write(AXI_CLKGEN_REG_LOCK2, (((lock >> 16) & 0x1f) << 10) | 0x1);
	CLKGEN_Write(AXI_CLKGEN_REG_LOCK3, (((lock >> 24) & 0x1f) << 10) | 0x3e9);
	CLKGEN_Write(AXI_CLKGEN_REG_FILTER1, filter >> 16);
	CLKGEN_Write(AXI_CLKGEN_REG_FILTER2, filter);
	CLKGEN_Write(AXI_CLKGEN_REG_UPDATE_ENABLE, 1);

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

	CLKGEN_Read(AXI_CLKGEN_REG_CLK_OUT1, &reg);
	dout = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	CLKGEN_Read(AXI_CLKGEN_REG_CLK_DIV, &reg);
	d = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	CLKGEN_Read(AXI_CLKGEN_REG_CLK_FB1, &reg);
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

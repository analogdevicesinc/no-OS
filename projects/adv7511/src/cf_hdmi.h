/***************************************************************************//**
 *   @file   cf_hdmi.h
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

#ifndef CF_HDMI_H_
#define CF_HDMI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xparameters.h"
#include "app_config.h"
#include "clk_axi_clkgen.h"

/******************************************************************************/
/************************** Macros Definitions ********************************/
/******************************************************************************/
#ifdef XPAR_AXI_HDMI_TX_0_BASEADDR
#define CFV_BASEADDR        XPAR_AXI_HDMI_TX_0_BASEADDR
#else
#define CFV_BASEADDR		XPAR_AXI_HDMI_CORE_BASEADDR
#endif
#ifdef XPAR_AXI_SPDIF_TX_0_BASEADDR
#define CFA_BASEADDR        XPAR_AXI_SPDIF_TX_0_BASEADDR
#else
#define CFA_BASEADDR        XPAR_AXI_SPDIF_TX_CORE_BASEADDR
#endif

#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
#ifdef XPAR_DDR3_SDRAM_S_AXI_BASEADDR
#define DDR_BASEADDR        XPAR_DDR3_SDRAM_S_AXI_BASEADDR
#else
#define DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR
#endif
#elif defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
#define DDR_BASEADDR        XPAR_DDR_MEM_BASEADDR
#endif

#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
#ifdef XPAR_RS232_UART_1_BASEADDR
#define UART_BASEADDR       XPAR_RS232_UART_1_BASEADDR
#else
#define UART_BASEADDR       XPAR_AXI_UART_BASEADDR
#endif
#elif defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
#define UART_BASEADDR       XPS_UART1_BASEADDR
#endif

#ifdef XPAR_AXI_VDMA_0_BASEADDR
#define VDMA_BASEADDR       XPAR_AXI_VDMA_0_BASEADDR
#else
#define VDMA_BASEADDR       XPAR_AXI_HDMI_DMA_BASEADDR
#endif

#ifdef XPAR_AXI_DMA_0_BASEADDR
#define ADMA_BASEADDR       XPAR_AXI_DMA_0_BASEADDR
#else
#if defined(PLATFORM_AC701) || defined(PLATFORM_VC707)
#define ADMA_BASEADDR       XPAR_AXIDMA_1_BASEADDR
#elif defined(PLATFORM_KC705)
#define ADMA_BASEADDR       XPAR_AXIDMA_0_BASEADDR
#endif
#endif
#if defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
#define ADMA_DEVICE_ID		XPAR_XDMAPS_1_DEVICE_ID
#endif

#if defined(PLATFORM_KC705) || defined(PLATFORM_AC701) || \
	defined(PLATFORM_VC707)
#define IIC_BASEADDR        XPAR_AXI_IIC_0_BASEADDR
#elif defined(PLATFORM_ZC702) || defined(PLATFORM_ZC706) || \
		defined(PLATFORM_ZED)
#define IIC_BASEADDR        XPS_I2C0_BASEADDR
#endif

#define VIDEO_BASEADDR		DDR_BASEADDR + 0x2000000
#define AUDIO_BASEADDR		DDR_BASEADDR + 0x1000000
#define A_SAMPLE_FREQ       48000
#define A_FREQ              1400
#define AUDIO_LENGTH		(A_SAMPLE_FREQ/A_FREQ)

#define AXI_HDMI_REG_RESET			0x040
#define AXI_HDMI_REG_CTRL			0x044
#define AXI_HDMI_REG_SOURCE_SEL		0x048
#define AXI_HDMI_REG_COLORPATTERN	0x04c
#define AXI_HDMI_REG_STATUS			0x05c
#define AXI_HDMI_REG_VDMA_STATUS	0x060
#define AXI_HDMI_REG_TPM_STATUS		0x064
#define AXI_HDMI_REG_HTIMING1		0x400
#define AXI_HDMI_REG_HTIMING2		0x404
#define AXI_HDMI_REG_HTIMING3		0x408
#define AXI_HDMI_REG_VTIMING1		0x440
#define AXI_HDMI_REG_VTIMING2		0x444
#define AXI_HDMI_REG_VTIMING3		0x448

#define AXI_VDMA_REG_DMA_CTRL		0x00
#define AXI_VDMA_REG_V_SIZE			0x50
#define AXI_VDMA_REG_H_SIZE			0x54
#define AXI_VDMA_REG_FRMDLY_STRIDE	0x58
#define AXI_VDMA_REG_START_1		0x5C
#define AXI_VDMA_REG_START_2		0x60
#define AXI_VDMA_REG_START_3		0x64

#define AXI_CLKGEN_V2_REG_RESET			0x40
#define AXI_CLKGEN_V2_REG_DRP_CNTRL		0x70
#define AXI_CLKGEN_V2_REG_DRP_STATUS	0x74

#define AXI_CLKGEN_V2_RESET_MMCM_ENABLE	(1 << 1)
#define AXI_CLKGEN_V2_RESET_ENABLE		(1 << 0)

#define AXI_CLKGEN_V2_DRP_CNTRL_SEL		(1 << 29)
#define AXI_CLKGEN_V2_DRP_CNTRL_READ	(1 << 28)
#define AXI_CLKGEN_V2_DRP_STATUS_BUSY	(1 << 16)

#define MMCM_REG_CLKOUT0_1		0x08
#define MMCM_REG_CLKOUT0_2		0x09
#define MMCM_REG_CLK_FB1		0x14
#define MMCM_REG_CLK_FB2		0x15
#define MMCM_REG_CLK_DIV		0x16
#define MMCM_REG_LOCK1			0x18
#define MMCM_REG_LOCK2			0x19
#define MMCM_REG_LOCK3			0x1a
#define MMCM_REG_FILTER1		0x4e
#define MMCM_REG_FILTER2		0x4f

enum videoResolution {
	RESOLUTION_640x480,
	RESOLUTION_800x600,
	RESOLUTION_1024x768,
	RESOLUTION_1280x720,
	RESOLUTION_1360x768,
	RESOLUTION_1600x900,
	RESOLUTION_1920x1080
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void InitHdmiVideoPcore(unsigned short horizontalActiveTime,
			unsigned short horizontalBlankingTime,
			unsigned short horizontalSyncOffset,
			unsigned short horizontalSyncPulseWidth,
			unsigned short verticalActiveTime,
			unsigned short verticalBlankingTime,
			unsigned short verticalSyncOffset,
			unsigned short verticalSyncPulseWidth);
void SetVideoResolution(struct axi_clkgen *clkgen, unsigned char resolution);
void InitHdmiAudioPcore(void);
void AudioClick(void);

#endif /* CF_HDMI_H_ */

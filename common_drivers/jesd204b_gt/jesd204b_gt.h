/***************************************************************************//**
* @file jesd204b_gt.h
* @brief Header file of JESD204B GT Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014-2015(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
*******************************************************************************/
#ifndef JESD204B_GT_H_
#define JESD204B_GT_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define JESD204B_GT_REG_VERSION						0x0000
#define JESD204B_GT_VERSION(x)						(((x) & 0xffffffff) << 0)
#define JESD204B_GT_VERSION_IS(x,y,z)				((x) << 16 | (y) << 8 | (z))
#define JESD204B_GT_VERSION_MAJOR(x)				((x) >> 16)

#define JESD204B_GT_REG_ID							0x0004
#define JESD204B_GT_REG_SCRATCH						0x0008
#define JESD204B_GT_REG_DRP_RSTN(c)					(0x014 + (c) * 0x400)
#define JESD204B_GT_DRP_RSTN						(1 << 1)
#define JESD204B_GT_REG_RX_OFFSET					0x0000
#define JESD204B_GT_REG_TX_OFFSET					0x0040

#define JESD204B_GT_REG_LPM_CPLL_PD(c)				(0x010 + (c) * 0x400)
#define JESD204B_GT_LPM_DFE(x)						(x << 1)
#define JESD204B_GT_CPLL_PD							(1 << 0)

#define JESD204B_GT_REG_GT_RSTN(c)					(0x020 + (c) * 0x400)
#define JESD204B_GT_GT_RSTN							(1 << 0)

#define JESD204B_GT_REG_RSTN(c)						(0x024 + (c) * 0x400)
#define JESD204B_GT_RSTN							(1 << 0)

#define JESD204B_GT_REG_CLK_SEL(c)					(0x028 + (c) * 0x400)
#define JESD204B_GT_SYS_CLK_SEL(x)					(((x) & 0x3) << 4)
#define JESD204B_GT_TO_SYS_CLK_SEL(x)				(((x) >> 4) & 0x3)
#define JESD204B_GT_OUT_CLK_SEL(x)					(((x) & 0x7) << 0)
#define JESD204B_GT_TO_OUT_CLK_SEL(x)				(((x) >> 0) & 0x7)

#define JESD204B_GT_REG_SYSREF_CTL(c)				(0x02C + (c) * 0x400)
#define JESD204B_GT_SYSREF_EXTERNAL					(1 << 1)
#define JESD204B_GT_SYSREF_ON						(1 << 0)
#define JESD204B_GT_SYSREF_OFF						(0 << 0)

#define JESD204B_GT_REG_SYNC_CTL(c)					(0x030 + (c) * 0x400)
#define JESD204B_GT_SYNC							(1 << 0)

#define JESD204B_GT_REG_STATUS(c)					(0x034 + (c) * 0x400)
#define JESD204B_GT_STATUS							(1 << 16)
#define JESD204B_GT_STATUS_PLL_LOCKED				0x000ff
#define JESD204B_GT_STATUS_RST_DONE					0x0ffff
#define JESD204B_GT_STATUS_SYNC						0x1ffff
#define JESD204B_GT_RST_DONE(x)						(((x) & 0xFF) << 8)
#define JESD204B_GT_TO_RST_DONE(x)					(((x) >> 8) & 0xFF)
#define JESD204B_GT_PLL_LOCKED(x)					(((x) & 0xFF) << 0)
#define JESD204B_GT_TO_PLL_LOCKED(x)				(((x) >> 0) & 0xFF)

#define JESD204B_GT_REG_USER_READY(c)				(0x038 + (c) * 0x400)
#define JESD204B_GT_USER_READY						(1 << 0)

#define JESD204B_GT_REG_PLL_RSTN(c)					(0x03c + (c) * 0x400)
#define JESD204B_GT_PLL_RSTN						(1 << 0)

#define JESD204B_GT_REG_RX_LANESEL(c)				(0x008C + (c) * 0x400)
#define JESD204B_GT_RX_LANESEL(x)					(((x) & 0xFF) << 0)
#define JESD204B_GT_TO_RX_LANESEL(x)				(((x) >> 0) & 0xFF)

#define JESD204B_GT_REG_DRP_CNTRL(c)				(0x0090 + (c) * 0x400)
#define JESD204B_GT_DRP_RWN							(1 << 28)
#define JESD204B_GT_DRP_ADDRESS(x)					(((x) & 0xFFF) << 16)
#define JESD204B_GT_TO_DRP_ADDRESS(x)				(((x) >> 16) & 0xFFF)
#define JESD204B_GT_DRP_WDATA(x)					(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_DRP_WDATA(x)					(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_DRP_STATUS(c)				(0x0094 + (c) * 0x400)
#define JESD204B_GT_DRP_STATUS						(1 << 16)
#define JESD204B_GT_DRP_RDATA(x)					(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_DRP_RDATA(x)					(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_CNTRL(c)			(0x00A0 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_INIT					(1 << 2)
#define JESD204B_GT_EYESCAN_STOP					(1 << 1)
#define JESD204B_GT_EYESCAN_START					(1 << 0)

#define JESD204B_GT_REG_EYESCAN_PRESCALE(c)			(0x00A4 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_PRESCALE(x)				(((x) & 0x1F) << 0)
#define JESD204B_GT_TO_EYESCAN_PRESCALE(x)			(((x) >> 0) & 0x1F)

#define JESD204B_GT_REG_EYESCAN_VOFFSET(c)			(0x00A8 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_VOFFSET_STEP(x)			(((x) & 0xFF) << 16)
#define JESD204B_GT_TO_EYESCAN_VOFFSET_STEP(x)		(((x) >> 16) & 0xFF)
#define JESD204B_GT_EYESCAN_VOFFSET_MAX(x)			(((x) & 0xFF) << 8)
#define JESD204B_GT_TO_EYESCAN_VOFFSET_MAX(x)		(((x) >> 8) & 0xFF)
#define JESD204B_GT_EYESCAN_VOFFSET_MIN(x)			(((x) & 0xFF) << 0)
#define JESD204B_GT_TO_EYESCAN_VOFFSET_MIN(x)		(((x) >> 0) & 0xFF)

#define JESD204B_GT_REG_EYESCAN_HOFFSET_1(c)		(0x00AC + (c) * 0x400)
#define JESD204B_GT_EYESCAN_HOFFSET_MAX(x)			(((x) & 0xFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_HOFFSET_MAX(x)		(((x) >> 16) & 0xFFF)
#define JESD204B_GT_EYESCAN_HOFFSET_MIN(x)			(((x) & 0xFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_HOFFSET_MIN(x)		(((x) >> 0) & 0xFFF)

#define JESD204B_GT_REG_EYESCAN_HOFFSET_2(c)		(0x00B0 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_HOFFSET_STEP(x)			(((x) & 0xFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_HOFFSET_STEP(x)		(((x) >> 0) & 0xFFF)

#define JESD204B_GT_REG_EYESCAN_DMA_STARTADDR(c)	(0x00B4 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_DMA_STARTADDR(x)		(((x) & 0xFFFFFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_DMA_STARTADDR(x)		(((x) >> 0) & 0xFFFFFFFF)

#define JESD204B_GT_REG_EYESCAN_SDATA_1_0(c)		(0x00B8 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_SDATA1(x)				(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_SDATA1(x)			(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_SDATA0(x)				(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_SDATA0(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_SDATA_3_2(c)		(0x00BC + (c) * 0x400)
#define JESD204B_GT_EYESCAN_SDATA3(x)				(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_SDATA3(x)			(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_SDATA2(x)				(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_SDATA2(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_SDATA_4(c)			(0x00C0 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_SDATA4(x)				(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_SDATA4(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_QDATA_1_0(c)		(0x00C4 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_QDATA1(x)				(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_QDATA1(x)			(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_QDATA0(x)				(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_QDATA0(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_QDATA_3_2(c)		(0x00C8 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_QDATA3(x)				(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_QDATA3(x)			(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_QDATA2(x)				(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_QDATA2(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_QDATA_4(c)			(0x00CC + (c) * 0x400)
#define JESD204B_GT_EYESCAN_QDATA4(x)				(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_QDATA4(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_STATUS(c)			(0x00E0 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_DMAERR					(1 << 1)
#define JESD204B_GT_EYESCAN_STATUS					(1 << 0)

#define JESD204B_GT_REG_EYESCAN_RATE(c)				(0x00E4 + (c) * 0x400)
#define JESD204B_GT_EYESCAN_RATE					(1 << 1)

#define JESD204B_GT_RX				0
#define JESD204B_GT_TX				1
#define JESD204B_GT_CPLL			0
#define JESD204B_GT_QPLL			1
#define JESD204B_GT_DFE				0
#define JESD204B_GT_LPM				1
#define JESD204B_GT_SYSREF_INT		0
#define JESD204B_GT_SYSREF_EXT		1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct
{
	uint32_t gt_core_addr;
	uint8_t	 tx_or_rx;			// transmit (0x1) or receive (0x0)
	uint8_t	 first_lane;		// first lane index
	uint8_t	 last_lane;			// last lane index
	uint8_t	 qpll_or_cpll;		// qpll (0x1) or cpll (0x0)
	uint8_t	 lpm_or_dfe;		// lpm (0x1) or dfe (0x0) -- receive only
	uint32_t ref_clk;			// reference clock frequency in MHz.
	uint32_t lane_rate;			// lane rate in Mbps
	uint32_t sysref_int_or_ext;	// internal (0x1) or external (0x0) sysref

	// the following parameters are internally set and you should not modify

	uint32_t sys_clk_sel;		// sys clock select
	uint32_t out_clk_sel;		// out clock select
	uint32_t gth_or_gtx;		// gth (0x1) or gtx (0x0)
}jesd204b_gt_link;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t jesd204b_gt_read(jesd204b_gt_link link_param,
						 uint32_t reg_addr,
						 uint32_t *reg_data);
int32_t jesd204b_gt_write(jesd204b_gt_link link_param,
						  uint32_t reg_addr,
						  uint32_t reg_data);
int32_t jesd204b_gt_txrx_read(jesd204b_gt_link link_param,
							  uint32_t reg_addr,
							  uint32_t *reg_data);
int32_t jesd204b_gt_txrx_write(jesd204b_gt_link link_param,
							   uint32_t reg_addr,
							   uint32_t reg_data);
int32_t jesd204b_gt_txrx_status(jesd204b_gt_link link_param,
								uint8_t first_lane,
								uint8_t last_lane,
								uint32_t status);
int32_t jesd204b_gt_initialize(jesd204b_gt_link link_param);
int32_t jesd204b_gt_setup(jesd204b_gt_link link_param);
int32_t jesd204b_gt_en_sync_sysref(jesd204b_gt_link link_param);
#endif

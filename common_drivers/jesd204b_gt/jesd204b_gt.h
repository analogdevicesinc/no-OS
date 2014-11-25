/***************************************************************************//**
* @file jesd204b_gt.h
* @brief Header file of JESD204B GT Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
#define JESD204B_GT_REG_VERSION				0x0000

#define JESD204B_GT_REG_ID					0x0004

#define JESD204B_GT_REG_SCRATCH				0x0008

#define JESD204B_GT_REG_CPLL_PD				0x010
#define JESD204B_GT_CPLL_PD 				(1 << 0)

#define JESD204B_GT_REG_RSTN_1				0x014
#define JESD204B_GT_DRP_RSTN				(1 << 1)
#define JESD204B_GT_PLL_RSTN				(1 << 0)

#define JESD204B_GT_REG_RX_GT_RSTN			0x020
#define JESD204B_GT_RX_GT_RSTN				(1 << 0)

#define JESD204B_GT_REG_RX_RSTN				0x024
#define JESD204B_GT_RX_RSTN					(1 << 0)

#define JESD204B_GT_REG_RX_CLK_SEL			0x028
#define JESD204B_GT_RX_SYS_CLK_SEL(x)		(((x) & 0x3) << 4)
#define JESD204B_GT_TO_RX_SYS_CLK_SEL(x)	(((x) >> 4) & 0x3)
#define JESD204B_GT_RX_OUT_CLK_SEL(x)		(((x) & 0x7) << 0)
#define JESD204B_GT_TO_RX_OUT_CLK_SEL(x)	(((x) >> 0) & 0x7)

#define JESD204B_GT_REG_RX_SYSREF_CTL		0x02c
#define JESD204B_GT_RX_SYSREF_SEL			(1 << 1)
#define JESD204B_GT_RX_SYSREF				(1 << 0)

#define JESD204B_GT_REG_RX_SYNC_CTL			0x030
#define JESD204B_GT_RX_SYNC					(1 << 0)

#define JESD204B_GT_REG_RX_STATUS			0x034
#define JESD204B_GT_RX_STATUS				(1 << 16)
#define JESD204B_GT_RX_RST_DONE(x)			(((x) & 0xFF) << 8)
#define JESD204B_GT_TO_RX_RST_DONE(x)		(((x) >> 8) & 0xFF)
#define JESD204B_GT_RX_PLL_LOCKED(x)		(((x) & 0xFF) << 0)
#define JESD204B_GT_TO_RX_PLL_LOCKED(x)		(((x) >> 0) & 0xFF)

#define JESD204B_GT_REG_TX_GT_RSTN			0x060
#define JESD204B_GT_TX_GT_RSTN				(1 << 0)

#define JESD204B_GT_REG_TX_RSTN				0x064
#define JESD204B_GT_TX_RSTN					(1 << 0)

#define JESD204B_GT_REG_TX_CLK_SEL			0x068
#define JESD204B_GT_TX_SYS_CLK_SEL(x)		(((x) & 0x3) << 4)
#define JESD204B_GT_TO_TX_SYS_CLK_SEL(x)	(((x) >> 4) & 0x3)
#define JESD204B_GT_TX_OUT_CLK_SEL(x)		(((x) & 0x7) << 0)
#define JESD204B_GT_TO_TX_OUT_CLK_SEL(x)	(((x) >> 0) & 0x7)

#define JESD204B_GT_REG_TX_SYSREF_CTL		0x006C
#define JESD204B_GT_TX_SYSREF_SEL			(1 << 1)
#define JESD204B_GT_TX_SYSREF				(1 << 0)

#define JESD204B_GT_REG_TX_SYNC_CTL			0x0070
#define JESD204B_GT_TX_SYNC					(1 << 0)

#define JESD204B_GT_REG_TX_STATUS			0x074
#define JESD204B_GT_TX_STATUS				(1 << 16)
#define JESD204B_GT_TX_RST_DONE(x)			(((x) & 0xFF) << 8)
#define JESD204B_GT_TO_TX_RST_DONE(x)		(((x) >> 8) & 0xFF)
#define JESD204B_GT_TX_PLL_LOCKED(x)		(((x) & 0xFF) << 0)
#define JESD204B_GT_TO_TX_PLL_LOCKED(x)		(((x) >> 0) & 0xFF)

#define JESD204B_GT_REG_RX_LANESEL			0x008C
#define JESD204B_GT_RX_LANESEL(x)			(((x) & 0xFF) << 0)
#define JESD204B_GT_TO_RX_LANESEL(x)		(((x) >> 0) & 0xFF)

#define JESD204B_GT_REG_DRP_CNTRL			0x0090
#define JESD204B_GT_DRP_RWN					(1 << 28)
#define JESD204B_GT_DRP_ADDRESS(x)			(((x) & 0xFFF) << 16)
#define JESD204B_GT_TO_DRP_ADDRESS(x)		(((x) >> 16) & 0xFFF)
#define JESD204B_GT_DRP_WDATA(x)			(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_DRP_WDATA(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_DRP_STATUS			0x0094
#define JESD204B_GT_DRP_STATUS				(1 << 16)
#define JESD204B_GT_DRP_RDATA(x)			(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_DRP_RDATA(x)			(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_CNTRL		0x00A0
#define JESD204B_GT_EYESCAN_INIT			(1 << 2)
#define JESD204B_GT_EYESCAN_STOP			(1 << 1)
#define JESD204B_GT_EYESCAN_START			(1 << 0)

#define JESD204B_GT_REG_EYESCAN_PRESCALE	0x00A4
#define JESD204B_GT_EYESCAN_PRESCALE(x)		(((x) & 0x1F) << 0)
#define JESD204B_GT_TO_EYESCAN_PRESCALE(x)	(((x) >> 0) & 0x1F)

#define JESD204B_GT_REG_EYESCAN_VOFFSET			0x00A8
#define JESD204B_GT_EYESCAN_VOFFSET_STEP(x)		(((x) & 0xFF) << 16)
#define JESD204B_GT_TO_EYESCAN_VOFFSET_STEP(x)	(((x) >> 16) & 0xFF)
#define JESD204B_GT_EYESCAN_VOFFSET_MAX(x)		(((x) & 0xFF) << 8)
#define JESD204B_GT_TO_EYESCAN_VOFFSET_MAX(x)	(((x) >> 8) & 0xFF)
#define JESD204B_GT_EYESCAN_VOFFSET_MIN(x)		(((x) & 0xFF) << 0)
#define JESD204B_GT_TO_EYESCAN_VOFFSET_MIN(x)	(((x) >> 0) & 0xFF)

#define JESD204B_GT_REG_EYESCAN_HOFFSET_1		0x00AC
#define JESD204B_GT_EYESCAN_HOFFSET_MAX(x)		(((x) & 0xFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_HOFFSET_MAX(x)	(((x) >> 16) & 0xFFF)
#define JESD204B_GT_EYESCAN_HOFFSET_MIN(x)		(((x) & 0xFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_HOFFSET_MIN(x)	(((x) >> 0) & 0xFFF)

#define JESD204B_GT_REG_EYESCAN_HOFFSET_2		0x00B0
#define JESD204B_GT_EYESCAN_HOFFSET_STEP(x)		(((x) & 0xFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_HOFFSET_STEP(x)	(((x) >> 0) & 0xFFF)

#define JESD204B_GT_REG_EYESCAN_DMA_STARTADDR	0x00B4
#define JESD204B_GT_EYESCAN_DMA_STARTADDR(x)	(((x) & 0xFFFFFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_DMA_STARTADDR(x)	(((x) >> 0) & 0xFFFFFFFF)

#define JESD204B_GT_REG_EYESCAN_SDATA_1_0	0x00B8
#define JESD204B_GT_EYESCAN_SDATA1(x)		(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_SDATA1(x)	(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_SDATA0(x)		(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_SDATA0(x)	(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_SDATA_3_2	0x00BC
#define JESD204B_GT_EYESCAN_SDATA3(x)		(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_SDATA3(x)	(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_SDATA2(x)		(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_SDATA2(x)	(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_SDATA_4		0x00C0
#define JESD204B_GT_EYESCAN_SDATA4(x)		(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_SDATA4(x)	(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_QDATA_1_0	0x00C4
#define JESD204B_GT_EYESCAN_QDATA1(x)		(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_QDATA1(x)	(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_QDATA0(x)		(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_QDATA0(x)	(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_QDATA_3_2	0x00C8
#define JESD204B_GT_EYESCAN_QDATA3(x)		(((x) & 0xFFFF) << 16)
#define JESD204B_GT_TO_EYESCAN_QDATA3(x)	(((x) >> 16) & 0xFFFF)
#define JESD204B_GT_EYESCAN_QDATA2(x)		(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_QDATA2(x)	(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_QDATA_4		0x00CC
#define JESD204B_GT_EYESCAN_QDATA4(x)		(((x) & 0xFFFF) << 0)
#define JESD204B_GT_TO_EYESCAN_QDATA4(x)	(((x) >> 0) & 0xFFFF)

#define JESD204B_GT_REG_EYESCAN_STATUS		0x00E0
#define JESD204B_GT_EYESCAN_DMAERR			(1 << 1)
#define JESD204B_GT_EYESCAN_STATUS			(1 << 0)

#define JESD204B_GT_REG_EYESCAN_RATE		0x00E4
#define JESD204B_GT_EYESCAN_RATE			(1 << 1)

#define JESD204B_GT_RX		0
#define JESD204B_GT_TX		1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct
{
	uint8_t		use_cpll;
	uint32_t	rx_sys_clk_sel;
	uint32_t	rx_out_clk_sel;
	uint32_t	tx_sys_clk_sel;
	uint32_t	tx_out_clk_sel;
}jesd204b_gt_state;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t jesd204b_gt_read(uint32_t reg_addr, uint32_t *reg_data);
int32_t jesd204b_gt_write(uint32_t reg_addr, uint32_t reg_data);
int32_t jesd204b_gt_clk_enable(uint32_t num);
int32_t jesd204b_gt_clk_synchronize(uint32_t num);
int32_t jesd204b_gt_setup(uint32_t baseaddr, jesd204b_gt_state setup_param);

#endif

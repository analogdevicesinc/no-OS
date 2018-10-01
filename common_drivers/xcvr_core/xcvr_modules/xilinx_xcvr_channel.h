/*******************************************************************************
 * @file xilinx_xcvr_channel.h
 * @brief part of xcvr dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 *******************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
 ******************************************************************************/

#ifndef XILINX_XCVR_CHANNEL_H
#define XILINX_XCVR_CHANNEL_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"
#include "xcvr_core.h"

#ifdef XILINX

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	refclk_div_M;
	uint32_t	fb_div_N1;
	uint32_t	fb_div_N2;
	uint32_t	out_div;
} xcvr_cpll;

/******************************************************************************/
/******************************* Definitions **********************************/
/******************************************************************************/

#define CPLL_CFG0_ADDR			0x5c
#define CPLL_CFG0_MASK			0xff00

#define CPLL_CFG1_ADDR			0x5d
#define CPLL_CFG1_MASK			0xffff

#define CPLL_REFCLK_DIV_M_ADDR	0x5e
#define CPLL_REFCLK_DIV_M_MASK	0x1f00
#define CPLL_FB_DIV_45_N1_MASK	0x0080
#define CPLL_FBDIV_N2_MASK		0x007f

#define OUT_DIV_ADDR			0x88
#define OUT_DIV_TX_OFFSET		0x4
#define OUT_DIV_RX_OFFSET		0x0

#define RXCDR_CFG0_ADDR			0xa8
#define RXCDR_CFG0_MASK			0xffff

#define RXCDR_CFG1_ADDR			0xa9
#define RXCDR_CFG1_MASK			0xffff

#define RXCDR_CFG2_ADDR			0xaa
#define RXCDR_CFG2_MASK			0xffff

#define RXCDR_CFG3_ADDR			0xab
#define RXCDR_CFG3_MASK			0xffff

#define RXCDR_CFG4_ADDR			0xac
#define RXCDR_CFG4_MASK			0x00ff

#define RX_DFE_LPM_CFG_ADDR		0x29
#define RX_DFE_LPM_CFG_MASK		0xffff

#define RX_CLK25_DIV			0x11
#define RX_CLK25_DIV_OFFSET		6
#define RX_CLK25_DIV_MASK		0x07c0

#define TX_CLK25_DIV			0x6a
#define TX_CLK25_DIV_MASK		0x1f

#define ENC_8B10B				810

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t xilinx_xcvr_calc_cpll_config(xcvr_core *core,
				     uint32_t refclk_khz,
				     uint32_t lane_rate_khz,
				     struct xilinx_xcvr_cpll_config *conf,
				     uint32_t *out_div);

int32_t xilinx_xcvr_gth34_cpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_cpll_config *conf);

int32_t xilinx_xcvr_gtx2_cpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_cpll_config *conf);

int32_t xilinx_xcvr_cpll_read_config(xcvr_core *core,
				     uint32_t drp_port,
				     struct xilinx_xcvr_cpll_config *conf);

int32_t xilinx_xcvr_gtx2_cpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_cpll_config *conf);

int32_t xilinx_xcvr_gth34_cpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_cpll_config *conf);

int32_t xilinx_xcvr_cpll_write_config(xcvr_core *core,
				      uint32_t drp_port,
				      const struct xilinx_xcvr_cpll_config *conf);

uint32_t xilinx_xcvr_cpll_calc_lane_rate(xcvr_core *core,
		uint32_t refclk_hz,
		const struct xilinx_xcvr_cpll_config *conf,
		uint32_t out_div);

int32_t xilinx_xcvr_gth3_configure_cdr(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t out_div);

int32_t xilinx_xcvr_gtx2_configure_cdr(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t lane_rate,
				       uint32_t out_div,
				       uint8_t lpm_enable);

int32_t xilinx_xcvr_configure_cdr(xcvr_core *core,
				  uint32_t drp_port,
				  uint32_t lane_rate,
				  uint32_t out_div,
				  uint8_t lpm_enable);

int32_t xilinx_xcvr_configure_lpm_dfe_mode(xcvr_core *core,
		uint32_t drp_port,
		uint8_t lpm);

int32_t xilinx_xcvr_gth34_read_out_div(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t *rx_out_div,
				       uint32_t *tx_out_div);

int32_t xilinx_xcvr_gtx2_read_out_div(xcvr_core *core,
				      uint32_t drp_port,
				      uint32_t *rx_out_div,
				      uint32_t *tx_out_div);

int32_t xilinx_xcvr_read_out_div(xcvr_core *core,
				 uint32_t drp_port,
				 uint32_t *rx_out_div,
				 uint32_t *tx_out_div);

int32_t xilinx_xcvr_gtx2_write_out_div(xcvr_core *core,
				       uint32_t drp_port,
				       int32_t rx_out_div,
				       int32_t tx_out_div);

int32_t xilinx_xcvr_gth34_write_out_div(xcvr_core *core,
					uint32_t drp_port,
					int32_t rx_out_div,
					int32_t tx_out_div);

int32_t xilinx_xcvr_write_out_div(xcvr_core *core,
				  uint32_t drp_port,
				  int32_t rx_out_div,
				  int32_t tx_out_div);

int32_t xilinx_xcvr_write_rx_clk25_div(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t div);

int32_t xilinx_xcvr_write_tx_clk25_div(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t div);

#endif
#endif

/******************************************************************************/
/******************************************************************************/

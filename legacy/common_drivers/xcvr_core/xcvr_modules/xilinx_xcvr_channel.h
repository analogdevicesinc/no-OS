/*******************************************************************************
 * @file xilinx_xcvr_channel.h
 * @brief part of xcvr dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 *******************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

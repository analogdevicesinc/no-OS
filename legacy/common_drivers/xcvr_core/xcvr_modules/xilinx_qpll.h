/*******************************************************************************
 * @file xilinx_qpll.h
 * @brief part of xcvr dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 *******************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef XILINX_XCVR_QPLL_H
#define XILINX_XCVR_QPLL_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"
#include "xcvr_core.h"

#ifdef XILINX

/******************************************************************************/
/******************************* Definitions **********************************/
/******************************************************************************/

#define QPLL_CFG0_ADDR			0x32
#define QPLL_CFG0_LOWBAND_MASK		0x0040

#define QPLL_CFG1_ADDR			0x33
#define QPLL_REFCLK_DIV_M_MASK		0xf800
#define QPLL_REFCLK_DIV_M_OFFSET	11
#define QPLL_REFCLK_DIV_M(x)		((x) << 11)

#define QPLL_FBDIV_N_ADDR		0x36
#define QPLL_FBDIV_N_MASK		0x03ff

#define QPLL_FBDIV_RATIO_ADDR		0x37
#define QPLL_FBDIV_RATIO_MASK		0x0040

#define QPLL_FBDIV(x)			(0x14 + (x) * 0x80)
#define QPLL_REFCLK_DIV(x)		(0x18 + (x) * 0x80)

#define QPLL0_FBDIV_DIV 0x14
#define QPLL0_REFCLK_DIV 0x18
#define QPLL1_FBDIV 0x94
#define QPLL1_REFCLK_DIV 0x98

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct {
	uint32_t refclk_div_M;
	uint32_t fb_div;
	uint32_t band;
	uint32_t out_div;
} xcvr_qpll;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t xilinx_xcvr_calc_qpll_config(xcvr_core *core,
				     uint32_t refclk_khz,
				     uint32_t lane_rate_khz,
				     struct xilinx_xcvr_qpll_config *conf,
				     uint32_t *out_div);

int32_t xilinx_xcvr_gth34_qpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_qpll_config *conf);

int32_t xilinx_xcvr_gtx2_qpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_qpll_config *conf);

int32_t xilinx_xcvr_qpll_read_config(xcvr_core *core,
				     uint32_t drp_port,
				     struct xilinx_xcvr_qpll_config *conf);

int32_t xilinx_xcvr_gtx2_qpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_qpll_config *conf);

int32_t xilinx_xcvr_gth34_qpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_qpll_config *conf);

int32_t xilinx_xcvr_qpll_write_config(xcvr_core *core,
				      uint32_t drp_port,
				      const struct xilinx_xcvr_qpll_config *conf);

uint32_t xilinx_xcvr_qpll_calc_lane_rate(xcvr_core *core,
		uint32_t refclk_hz,
		const struct xilinx_xcvr_qpll_config *conf,
		uint32_t out_div);

#endif
#endif

/******************************************************************************/
/******************************************************************************/

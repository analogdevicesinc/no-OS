/*******************************************************************************
 * @file xilinx_qpll.h
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

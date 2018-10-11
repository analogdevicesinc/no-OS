/***************************************************************************//**
 *   @file   xilinx_transceiver.h
 *   @brief  Driver for the Xilinx High-speed transceiver dynamic
 *           reconfiguration.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef XILINX_TRANSCEIVER_H_
#define XILINX_TRANSCEIVER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/************************ Macros and Types Declarations ***********************/
/******************************************************************************/
enum xilinx_xcvr_type {
	XILINX_XCVR_TYPE_S7_GTX2,
	XILINX_XCVR_TYPE_US_GTH3,
	XILINX_XCVR_TYPE_US_GTH4,
};

enum xilinx_xcvr_refclk_ppm {
	PM_200,
	PM_700,
	PM_1250,
};

struct xilinx_xcvr {
	enum xilinx_xcvr_type type;
	enum xilinx_xcvr_refclk_ppm refclk_ppm;
	uint32_t encoding;
	struct adxcvr *ad_xcvr;
};

struct xilinx_xcvr_cpll_config {
	uint32_t refclk_div;
	uint32_t fb_div_N1;
	uint32_t fb_div_N2;
};

struct xilinx_xcvr_qpll_config {
	uint32_t refclk_div;
	uint32_t fb_div;
	uint32_t band;
};

#define ENC_8B10B		810

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t xilinx_xcvr_configure_cdr(struct xilinx_xcvr *xcvr,
				  uint32_t drp_port, uint32_t lane_rate, uint32_t out_div,
				  bool lpm_enable);
int32_t xilinx_xcvr_configure_lpm_dfe_mode(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, bool lpm);
int32_t xilinx_xcvr_calc_cpll_config(struct xilinx_xcvr *xcvr,
				     uint32_t refclk_hz, uint32_t lane_rate_khz,
				     struct xilinx_xcvr_cpll_config *conf, uint32_t *out_div);
int32_t xilinx_xcvr_cpll_read_config(struct xilinx_xcvr *xcvr,
				     uint32_t drp_port, struct xilinx_xcvr_cpll_config *conf);
int32_t xilinx_xcvr_cpll_write_config(struct xilinx_xcvr *xcvr,
				      uint32_t drp_port, const struct xilinx_xcvr_cpll_config *conf);
int32_t xilinx_xcvr_cpll_calc_lane_rate(struct xilinx_xcvr *xcvr,
					uint32_t refclk_khz, const struct xilinx_xcvr_cpll_config *conf,
					uint32_t out_div);
int32_t xilinx_xcvr_calc_qpll_config(struct xilinx_xcvr *xcvr,
				     uint32_t refclk_khz, uint32_t lane_rate_khz,
				     struct xilinx_xcvr_qpll_config *conf, uint32_t *out_div);
int32_t xilinx_xcvr_qpll_read_config(struct xilinx_xcvr *xcvr,
				     uint32_t drp_port, struct xilinx_xcvr_qpll_config *conf);
int32_t xilinx_xcvr_qpll_write_config(struct xilinx_xcvr *xcvr,
				      uint32_t drp_port, const struct xilinx_xcvr_qpll_config *conf);
int32_t xilinx_xcvr_qpll_calc_lane_rate(struct xilinx_xcvr *xcvr,
					uint32_t refclk_hz, const struct xilinx_xcvr_qpll_config *conf,
					uint32_t out_div);
int32_t xilinx_xcvr_read_out_div(struct xilinx_xcvr *xcvr, uint32_t drp_port,
				 uint32_t *rx_out_div, uint32_t *tx_out_div);
int32_t xilinx_xcvr_write_out_div(struct xilinx_xcvr *xcvr, uint32_t drp_port,
				  int32_t rx_out_div, int32_t tx_out_div);
int32_t xilinx_xcvr_write_rx_clk25_div(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, uint32_t div);
int32_t xilinx_xcvr_write_tx_clk25_div(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, uint32_t div);
#endif

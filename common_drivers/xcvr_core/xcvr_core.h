/***************************************************************************//**
 * @file adxcvr.h
 * @brief Header file of XCVR Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
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
 *******************************************************************************/
#ifndef XCVR_CORE_H_
#define XCVR_CORE_H_

#include "platform_drivers.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	PM_200,
	PM_700,
	PM_1250,
} refclk_ppm;

typedef struct {
	uint32_t		base_address;
	uint32_t		tx_lane_pll_base_address;
	uint32_t		mmcm_lpll_base_address;
	uint32_t		mmcm_present;
	uint32_t		reconfig_bypass;
	uint8_t 		rx_tx_n;
	uint32_t		no_of_lanes;
	uint32_t		*lane_base_addresses;
	uint32_t		lane_rate_kbps;
	uint32_t		ref_clock_khz;

	// below parameters are auto computed and is for temporary internal use only-

	refclk_ppm		ppm;
	uint16_t		encoding;
	uint8_t			gth_enable;
	uint8_t			lpm_enable;
	uint8_t			cpll_enable;
	uint32_t		sys_clk_sel;
	uint32_t		out_clk_sel;
} xcvr_core;

/******************************************************************************/
/************************ XCVR Common Core Definitions ************************/
/******************************************************************************/

#define PCORE_VER(major, minor, letter)		((major << 16) | (minor << 8) | letter)
#define PCORE_VER_MAJOR(version)		(version >> 16)
#define PCORE_VER_MINOR(version)		((version >> 8) & 0xff)
#define PCORE_VER_LETTER(version)		(version & 0xff)

#define XCVR_REG_VERSION			0x0000
#define XCVR_VERSION(x)				(((x) & 0xffffffff) << 0)
#define XCVR_VERSION_IS(x, y, z)		((x) << 16 | (y) << 8 | (z))
#define XCVR_VERSION_MAJOR(x)			((x) >> 16)

#define XCVR_REG_ID				0x0004

#define XCVR_REG_SCRATCH			0x0008

#define XCVR_REG_RESETN				0x0010
#define XCVR_RESETN				(1 << 0)

#define XCVR_REG_STATUS				0x0014
#define XCVR_STATUS				(1 << 0)

#define XCVR_REG_CONTROL			0x0020
#define XCVR_LPM_DFE_N				(1 << 12)
#define XCVR_RATE(x)				(((x) & 0x7) << 8)
#define XCVR_SYSCLK_SEL(x)			(((x) & 0x3) << 4)
#define XCVR_OUTCLK_SEL(x)			(((x) & 0x7) << 0)

#define XCVR_REG_CM_SEL				0x0040

#define XCVR_REG_CM_CONTROL			0x0044
#define XCVR_CM_WR				(1 << 28)
#define XCVR_CM_ADDR(x)				(((x) & 0xFFF) << 16)
#define XCVR_CM_WDATA(x)			(((x) & 0xFFFF) << 0)

#define XCVR_REG_CM_STATUS			0x0048
#define XCVR_CM_BUSY				(1 << 16)
#define XCVR_CM_RDATA(x)			(((x) & 0xFFFF) << 0)

#define XCVR_REG_CH_SEL				0x0060

#define XCVR_REG_CH_CONTROL			0x0064
#define XCVR_CH_WR				(1 << 28)
#define XCVR_CH_ADDR(x)				(((x) & 0xFFF) << 16)
#define XCVR_CH_WDATA(x)			(((x) & 0xFFFF) << 0)

#define XCVR_REG_CH_STATUS			0x0068
#define XCVR_CH_BUSY				(1 << 16)
#define XCVR_CH_RDATA(x)			(((x) & 0xFFFF) << 0)

#define XCVR_BROADCAST				0xff
#define ENC_8B10B				810

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t xcvr_read(xcvr_core core, uint32_t reg_addr, uint32_t *reg_data);
int32_t xcvr_write(xcvr_core core, uint32_t reg_addr, uint32_t reg_data);

#ifdef XILINX

	int32_t xcvr_drp_read(xcvr_core core, uint8_t drp_sel, uint32_t drp_addr, uint32_t *drp_data);
	int32_t xcvr_drp_write(xcvr_core core, uint8_t drp_sel, uint32_t drp_addr, uint32_t drp_data);

	int32_t xcvr_reconfig(xcvr_core core, uint32_t lane_rate, uint32_t ref_clk);
#endif

int32_t xcvr_setup(xcvr_core *core);
int32_t xcvr_status(xcvr_core core);

#endif

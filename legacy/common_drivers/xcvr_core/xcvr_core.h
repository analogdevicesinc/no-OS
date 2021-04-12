/***************************************************************************//**
 * @file adxcvr.h
 * @brief Header file of XCVR Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
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
#ifndef XCVR_CORE_H_
#define XCVR_CORE_H_

#include "platform_drivers.h"

#ifdef ALTERA
#include "altera_pll_common.h"
#endif

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
#ifdef ALTERA
typedef enum {
	PM_100,
	PM_300,
	PM_500,
	PM_1000,
} xilinx_xcvr_refclk_ppm;

struct fpga_dev {
	xcvr_pll		link_pll;
	xcvr_pll		atx_pll;
	xcvr_pll		channel_pll[8];	// max 8
};
#endif

#ifdef XILINX
typedef enum {
	PM_200,
	PM_700,
	PM_1250,
} xilinx_xcvr_refclk_ppm;

typedef enum {
	XILINX_XCVR_TYPE_S7_GTX2,
	XILINX_XCVR_TYPE_US_GTH3,
	XILINX_XCVR_TYPE_US_GTH4,
} xilinx_xcvr_type;

struct xilinx_xcvr_qpll_config {
	uint32_t refclk_div;
	uint32_t fb_div;
	uint32_t band;
};

struct xilinx_xcvr_cpll_config {
	uint32_t refclk_div;
	uint32_t fb_div_N1;
	uint32_t fb_div_N2;
};

struct fpga_dev {
	uint32_t			   sys_clk_sel;
	uint32_t			   out_clk_sel;
	uint32_t			   out_div;
	uint8_t				   lpm_enable;
	uint8_t				   cpll_enable;
	xilinx_xcvr_type	   type;
};
#endif

typedef struct {
	uint8_t				   num_lanes;
	uint8_t				   rx_tx_n;
	uint32_t			   base_address;
	xilinx_xcvr_refclk_ppm ppm;
	uint16_t			   encoding;
	uint32_t			   lane_rate_kbps;
	uint32_t			   ref_rate_khz;
	uint8_t				   reconfig_bypass;
	struct fpga_dev		   dev;
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

#define XCVR_RESETN					(1 << 0)

#define XCVR_REG_STATUS				0x0014
#define XCVR_STATUS					(1 << 0)

#define XCVR_REG_CONTROL			0x0020
#define XCVR_LPM_DFE_N				(1 << 12)
#define XCVR_RATE(x)				(((x) & 0x7) << 8)
#define XCVR_SYSCLK_SEL(x)			(((x) & 0x3) << 4)
#define XCVR_OUTCLK_SEL(x)			(((x) & 0x7) << 0)

#define XCVR_REG_SYNTH				0x24
#define XCVR_QPLL_ENABLE_MASK		(0x01 << XCVR_QPLL_ENABLE_OFFSET)
#define XCVR_GT_TYPE_MASK			(0x0f << XCVR_GT_TYPE_OFFSET)
#define XCVR_TX_OR_RXN_MASK			0x100
#define XCVR_NUM_OF_LANES_MASK			0xff
#define XCVR_QPLL_ENABLE_OFFSET			20
#define XCVR_GT_TYPE_OFFSET			16
#define XCVR_TX_OR_RXN_OFFSET			 8
#define XCVR_NUM_OF_LANES_OFFSET		 0

#define XCVR_REG_CM_SEL				0x0040

#define XCVR_REG_CM_CONTROL			0x0044
#define XCVR_CM_WR				(1 << 28)
#define XCVR_CM_ADDR(x)				(((x) & 0xFFF) << 16)
#define XCVR_CM_WDATA(x)			(((x) & 0xFFFF) << 0)

#define XCVR_REG_DRP_SEL(x)			(0x0040 + (x))

#define XCVR_REG_DRP_CTRL(x)		(0x0044 + (x))
#define XCVR_DRP_CTRL_WR			(1 << 28)
#define XCVR_DRP_CTRL_ADDR(x)		(((x) & 0xFFF) << 16)
#define XCVR_DRP_CTRL_WDATA(x)		(((x) & 0xFFFF) << 0)

#define XCVR_REG_DRP_STATUS(x)		(0x0048 + (x))
#define XCVR_DRP_STATUS_BUSY		(1 << 16)
#define XCVR_DRP_STATUS_RDATA(x)	(((x) & 0xFFFF) << 0)

#define XCVR_DRP_PORT_ADDR_COMMON	0x00
#define XCVR_DRP_PORT_ADDR_CHANNEL	0x20

#define XCVR_DRP_PORT_COMMON		0x00
#define XCVR_DRP_PORT_CHANNEL(x)	(0x1 + x)
#define XCVR_DRP_PORT_CHANNEL_BCAST	0xff

#ifdef ALTERA
#define XCVR_REG_STATUS2			0x0018
#define XCVR_STATUS2_XCVR(x)			BIT(x)
#define XCVR_REG_SYNTH_CONF			0x0024
# endif

// GT types

#define XILINX_GTH	0
#define XILINX_GTX	1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t xcvr_read(xcvr_core *core, uint32_t reg_addr, uint32_t *reg_data);
int32_t xcvr_write(xcvr_core *core, uint32_t reg_addr, uint32_t reg_data);

#ifdef XILINX

int32_t xcvr_drp_wait_idle(xcvr_core *core, uint32_t drp_addr);
int32_t xilinx_xcvr_drp_read(xcvr_core *core,
			     uint32_t drp_port,uint32_t reg);
int32_t xilinx_xcvr_drp_write(xcvr_core *core, uint32_t drp_port, uint32_t reg,
			      uint32_t val);
int32_t xcvr_drp_update(xcvr_core *core, uint32_t drp_port, uint32_t reg,
			uint32_t mask, uint32_t val);

#endif

int32_t xcvr_setup(xcvr_core *core);
int32_t xcvr_status(xcvr_core *core);
int32_t xcvr_getconfig(xcvr_core *core);
int32_t xcvr_reset(xcvr_core *core);
void xcvr_finalize_lane_rate_change(xcvr_core *core);

#endif

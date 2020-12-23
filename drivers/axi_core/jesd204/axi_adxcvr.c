/***************************************************************************//**
 *   @file   axi_adxcvr.c
 *   @brief  Driver for the ADI AXI-ADXCVR Module.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "axi_io.h"
#include "util.h"
#include "error.h"
#include "delay.h"
#include "xilinx_transceiver.h"
#include "axi_adxcvr.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADXCVR_REG_RESETN		0x0010
#define ADXCVR_RESETN			(1 << 0)

#define ADXCVR_REG_STATUS		0x0014
#define ADXCVR_STATUS			(1 << 0)

#define ADXCVR_REG_CONTROL		0x0020
#define ADXCVR_LPM_DFE_N		(1 << 12)
#define ADXCVR_RATE(x)			(((x) & 0x7) << 8)
#define ADXCVR_SYSCLK_SEL(x)	(((x) & 0x3) << 4)
#define ADXCVR_OUTCLK_SEL(x)	(((x) & 0x7) << 0)

#define ADXCVR_REG_SYNTH		0x24

#define ADXCVR_REG_DRP_SEL(x)		(0x0040 + (x))

#define ADXCVR_REG_DRP_CTRL(x)		(0x0044 + (x))
#define ADXCVR_DRP_CTRL_WR			(1 << 28)
#define ADXCVR_DRP_CTRL_ADDR(x)		(((x) & 0xFFF) << 16)
#define ADXCVR_DRP_CTRL_WDATA(x)	(((x) & 0xFFFF) << 0)

#define ADXCVR_REG_DRP_STATUS(x)	(0x0048 + (x))
#define ADXCVR_DRP_STATUS_BUSY		(1 << 16)
#define ADXCVR_DRP_STATUS_RDATA(x)	(((x) & 0xFFFF) << 0)

#define ADXCVR_DRP_PORT_ADDR_COMMON		0x00
#define ADXCVR_DRP_PORT_ADDR_CHANNEL	0x20

#define ADXCVR_DRP_PORT_COMMON(x)		(x)
#define ADXCVR_DRP_PORT_CHANNEL(x)		(0x100 + (x))

#define ADXCVR_BROADCAST				0xff

/**
 * @brief adxcvr_write
 */
int32_t adxcvr_write(struct adxcvr *xcvr,
		     uint32_t reg_addr,
		     uint32_t reg_val)
{
	axi_io_write(xcvr->base, reg_addr, reg_val);

	return SUCCESS;
}

/**
 * @brief adxcvr_read
 */
int32_t adxcvr_read(struct adxcvr *xcvr,
		    uint32_t reg_addr,
		    uint32_t *reg_val)
{
	axi_io_read(xcvr->base, reg_addr, reg_val);

	return SUCCESS;
}

/**
 * @brief adxcvr_drp_wait_idle
 */
int32_t adxcvr_drp_wait_idle(struct adxcvr *xcvr,
			     uint32_t drp_addr)
{
	uint32_t val;
	int32_t timeout = 20;

	do {
		adxcvr_read(xcvr, ADXCVR_REG_DRP_STATUS(drp_addr), &val);
		if (!(val & ADXCVR_DRP_STATUS_BUSY))
			return ADXCVR_DRP_STATUS_RDATA(val);

		mdelay(1);
	} while (timeout--);

	printf("%s: %s: Timeout!", xcvr->name, __func__);

	return FAILURE;
}

/**
 * @brief adxcvr_drp_read
 */
int32_t adxcvr_drp_read(struct adxcvr *xcvr,
			uint32_t drp_port,
			uint32_t reg,
			uint32_t *val)
{
	uint32_t drp_sel, drp_addr;
	int32_t ret;

	if (drp_port < ADXCVR_DRP_PORT_CHANNEL(0))
		drp_addr = ADXCVR_DRP_PORT_ADDR_COMMON;
	else
		drp_addr = ADXCVR_DRP_PORT_ADDR_CHANNEL;

	drp_sel = drp_port & 0xFF;

	adxcvr_write(xcvr, ADXCVR_REG_DRP_SEL(drp_addr), drp_sel);
	adxcvr_write(xcvr, ADXCVR_REG_DRP_CTRL(drp_addr), ADXCVR_DRP_CTRL_ADDR(reg));

	ret = adxcvr_drp_wait_idle(xcvr, drp_addr);
	if (ret < 0)
		return ret;

	*val = ret & 0xffff;

	return SUCCESS;
}

/**
 * @brief adxcvr_drp_write
 */
int32_t adxcvr_drp_write(struct adxcvr *xcvr,
			 uint32_t drp_port,
			 uint32_t reg,
			 uint32_t val)
{
	uint32_t drp_sel, drp_addr;
	int32_t ret;

	if (drp_port < ADXCVR_DRP_PORT_CHANNEL(0))
		drp_addr = ADXCVR_DRP_PORT_ADDR_COMMON;
	else
		drp_addr = ADXCVR_DRP_PORT_ADDR_CHANNEL;

	drp_sel = drp_port & 0xFF;

	adxcvr_write(xcvr, ADXCVR_REG_DRP_SEL(drp_addr), drp_sel);
	adxcvr_write(xcvr, ADXCVR_REG_DRP_CTRL(drp_addr), (ADXCVR_DRP_CTRL_WR |
			ADXCVR_DRP_CTRL_ADDR(reg) | ADXCVR_DRP_CTRL_WDATA(val)));

	ret = adxcvr_drp_wait_idle(xcvr, drp_addr);
	if (ret < 0)
		return ret;

	return SUCCESS;
}

/**
 * @brief adxcvr_clk_set_rate
 */
int32_t adxcvr_clk_set_rate(struct adxcvr *xcvr,
			    uint32_t rate,
			    uint32_t parent_rate)
{
	struct xilinx_xcvr_cpll_config cpll_conf;
	struct xilinx_xcvr_qpll_config qpll_conf;
	uint32_t out_div, clk25_div;
	uint32_t i;
	int32_t ret;

	clk25_div = DIV_ROUND_CLOSEST(parent_rate, 25000);

	if (xcvr->cpll_enable)
		ret = xilinx_xcvr_calc_cpll_config(&xcvr->xlx_xcvr, parent_rate, rate,
						   &cpll_conf, &out_div);
	else
		ret = xilinx_xcvr_calc_qpll_config(&xcvr->xlx_xcvr, parent_rate, rate,
						   &qpll_conf, &out_div);
	if (ret < 0)
		return ret;

	for (i = 0; i < xcvr->num_lanes; i++) {

		if (xcvr->cpll_enable)
			ret = xilinx_xcvr_cpll_write_config(&xcvr->xlx_xcvr,
							    ADXCVR_DRP_PORT_CHANNEL(i),
							    &cpll_conf);
		else if (i % 4 == 0)
			ret = xilinx_xcvr_qpll_write_config(&xcvr->xlx_xcvr,
							    ADXCVR_DRP_PORT_COMMON(i),
							    &qpll_conf);
		if (ret < 0)
			return ret;

		ret = xilinx_xcvr_write_out_div(&xcvr->xlx_xcvr,
						ADXCVR_DRP_PORT_CHANNEL(i),
						xcvr->tx_enable ? -1 : (int32_t)out_div,
						xcvr->tx_enable ? (int32_t)out_div : -1);
		if (ret < 0)
			return ret;

		if (!xcvr->tx_enable) {
			ret = xilinx_xcvr_configure_cdr(&xcvr->xlx_xcvr,
							ADXCVR_DRP_PORT_CHANNEL(i),
							rate, out_div,
							xcvr->lpm_enable);
			if (ret < 0)
				return ret;

			ret = xilinx_xcvr_write_rx_clk25_div(&xcvr->xlx_xcvr,
							     ADXCVR_DRP_PORT_CHANNEL(i),
							     clk25_div);
		} else {
			ret = xilinx_xcvr_write_tx_clk25_div(&xcvr->xlx_xcvr,
							     ADXCVR_DRP_PORT_CHANNEL(i),
							     clk25_div);
		}

		if (ret < 0)
			return ret;
	}


	xcvr->lane_rate_khz = rate;

	return SUCCESS;
}

/**
 * @brief adxcvr_status_error
 */
int32_t adxcvr_status_error(struct adxcvr *xcvr)
{
	int32_t timeout = 100;
	uint32_t status;

	do {
		mdelay(1);
		adxcvr_read(xcvr, ADXCVR_REG_STATUS, &status);
	} while ((timeout--) && (status == 0));

	if (!status)
		return FAILURE;

	printf("%s: OK (%"PRId32" kHz)\n", xcvr->name, xcvr->lane_rate_khz);

	return SUCCESS;
}

/**
 * @brief adxcvr_clk_enable
 */
int32_t adxcvr_clk_enable(struct adxcvr *xcvr)
{
	adxcvr_write(xcvr, ADXCVR_REG_RESETN, ADXCVR_RESETN);
	mdelay(100);

	return adxcvr_status_error(xcvr);
}

/**
 * @brief adxcvr_clk_disable
 */
int32_t adxcvr_clk_disable(struct adxcvr *xcvr)
{
	adxcvr_write(xcvr, ADXCVR_REG_RESETN, 0);

	return SUCCESS;
}

/**
 * @brief adxcvr_get_info
 */
static void adxcvr_get_info(struct adxcvr *xcvr)
{
	uint32_t reg_value;

	adxcvr_read(xcvr, AXI_REG_FPGA_INFO, &reg_value);
	xcvr->xlx_xcvr.tech = AXI_INFO_FPGA_TECH(reg_value);
	xcvr->xlx_xcvr.family = AXI_INFO_FPGA_FAMILY(reg_value);
	xcvr->xlx_xcvr.speed_grade = AXI_INFO_FPGA_SPEED_GRADE(reg_value);
	xcvr->xlx_xcvr.dev_package = AXI_INFO_FPGA_DEV_PACKAGE(reg_value);

	adxcvr_read(xcvr, AXI_REG_FPGA_VOLTAGE, &reg_value);
	xcvr->xlx_xcvr.voltage = AXI_INFO_FPGA_VOLTAGE(reg_value);
}

/**
 * @brief adxcvr_init
 */
int32_t adxcvr_init(struct adxcvr **ad_xcvr,
		    const struct adxcvr_init *init)
{
	struct adxcvr *xcvr;
	uint32_t synth_conf, xcvr_type;
	uint32_t i;
	int32_t ret;

	xcvr = (struct adxcvr *)malloc(sizeof(*xcvr));
	if (!xcvr)
		return FAILURE;

	xcvr->base = init->base;
	xcvr->name = init->name;
	xcvr->sys_clk_sel = init->sys_clk_sel;
	xcvr->out_clk_sel = init->out_clk_sel;
	xcvr->cpll_enable = init->cpll_enable;
	xcvr->lpm_enable = init->lpm_enable;

	xcvr->lane_rate_khz = init->lane_rate_khz;
	xcvr->ref_rate_khz = init->ref_rate_khz;

	adxcvr_read(xcvr, ADXCVR_REG_SYNTH, &synth_conf);
	xcvr->tx_enable = (synth_conf >> 8) & 1;
	xcvr->num_lanes = synth_conf & 0xff;

	xcvr_type = (synth_conf >> 16) & 0xf;

	adxcvr_read(xcvr, AXI_REG_VERSION, &xcvr->xlx_xcvr.version);
	if (AXI_PCORE_VER_MAJOR(xcvr->xlx_xcvr.version) > 0x10)
		adxcvr_get_info(xcvr);

	/* Ensure compliance with legacy xcvr type */
	if (AXI_PCORE_VER_MAJOR(xcvr->xlx_xcvr.version) <= 0x10) {
		switch (xcvr_type) {
		case XILINX_XCVR_LEGACY_TYPE_S7_GTX2:
			xcvr->xlx_xcvr.type = XILINX_XCVR_TYPE_S7_GTX2;
			break;
		case XILINX_XCVR_LEGACY_TYPE_US_GTH3:
			xcvr->xlx_xcvr.type = XILINX_XCVR_TYPE_US_GTH3;
			break;
		case XILINX_XCVR_LEGACY_TYPE_US_GTH4:
			xcvr->xlx_xcvr.type = XILINX_XCVR_TYPE_US_GTH4;
			break;
		case XILINX_XCVR_LEGACY_TYPE_US_GTY4:
			xcvr->xlx_xcvr.type = XILINX_XCVR_TYPE_US_GTY4;
			break;
		default:
			goto err;
		}
	} else
		xcvr->xlx_xcvr.type = xcvr_type;

	switch (xcvr->xlx_xcvr.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		break;
	default:
		printf("Unknown transceiver type: %d\n", xcvr->xlx_xcvr.type);
		goto err;
	}

	xcvr->xlx_xcvr.encoding = ENC_8B10B;
	xcvr->xlx_xcvr.refclk_ppm = PM_200; /* TODO use clock accuracy */

	adxcvr_write(xcvr, ADXCVR_REG_RESETN, 0);

	adxcvr_write(xcvr, ADXCVR_REG_CONTROL,
		     ((xcvr->lpm_enable ? ADXCVR_LPM_DFE_N : 0) |
		      ADXCVR_SYSCLK_SEL(xcvr->sys_clk_sel) |
		      ADXCVR_OUTCLK_SEL(xcvr->out_clk_sel)));

	xcvr->xlx_xcvr.ad_xcvr = xcvr;

	if (!xcvr->tx_enable) {
		for (i = 0; i < xcvr->num_lanes; i++) {
			xilinx_xcvr_configure_lpm_dfe_mode(&xcvr->xlx_xcvr,
							   ADXCVR_DRP_PORT_CHANNEL(i),
							   xcvr->lpm_enable);
		}
	}

	if (xcvr->lane_rate_khz && xcvr->ref_rate_khz) {
		ret = adxcvr_clk_set_rate(xcvr, xcvr->lane_rate_khz, xcvr->ref_rate_khz);
		if (ret)
			goto err;
	}

	*ad_xcvr = xcvr;

	return SUCCESS;

err:
	free(xcvr);

	return FAILURE;
}

/**
 * @brief adxcvr_remove
 */
int32_t adxcvr_remove(struct adxcvr *xcvr)
{
	free(xcvr);

	return SUCCESS;
}

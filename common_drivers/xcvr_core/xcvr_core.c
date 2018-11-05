/***************************************************************************//**
 * @file xcvr.c
 * @brief Implementation of xcvr Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xcvr_core.h"

#ifdef ALTERA
#include "altera_pll_common.h"
#include "altera_a10_fpll.h"
#include "altera_a10_atx_pll.h"
#include "altera_a10_xcvr_channel.h"
#endif

#ifdef XILINX
#include "xilinx_xcvr_channel.h"
#include "xilinx_qpll.h"
#endif

/***************************************************************************//**
 * @brief xcvr_read
 ******************************************************************************/
int32_t xcvr_read(xcvr_core *core,
		  uint32_t reg_addr,
		  uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core->base_address + reg_addr));

	return 0;
}

/***************************************************************************//**
 * @brief xcvr_write
 ******************************************************************************/
int32_t xcvr_write(xcvr_core *core,
		   uint32_t reg_addr,
		   uint32_t reg_data)
{
	ad_reg_write((core->base_address + reg_addr), reg_data);

	return 0;
}

#ifdef XILINX

/***************************************************************************//**
 * @brief xcvr_drp_wait_idle
 ******************************************************************************/
int32_t xcvr_drp_wait_idle(xcvr_core *core,
			   uint32_t drp_addr)
{
	uint32_t val;
	int32_t timeout = 20;

	do {
		xcvr_read(core, XCVR_REG_DRP_STATUS(drp_addr), &val);
		if (!(val & XCVR_DRP_STATUS_BUSY))
			return XCVR_DRP_STATUS_RDATA(val);

		mdelay(1);
	} while (timeout--);

	printf("%s: Timeout!", __func__);

	return -1;
}

/***************************************************************************//**
 * @brief xilinx_xcvr_drp_read Dynamic reconfiguration port read access for
 * 		  Xilinx devices
 ******************************************************************************/
int32_t xilinx_xcvr_drp_read(xcvr_core *core,
			     uint32_t drp_port,
			     uint32_t reg)
{
	uint32_t drp_sel = XCVR_DRP_PORT_CHANNEL_BCAST;
	uint32_t drp_addr;
	int32_t ret;

	if (drp_port == XCVR_DRP_PORT_COMMON) {
		drp_addr = XCVR_DRP_PORT_ADDR_COMMON;
	} else {
		drp_addr = XCVR_DRP_PORT_ADDR_CHANNEL;
		if (drp_port != XCVR_DRP_PORT_CHANNEL_BCAST)
			drp_sel = drp_port - 1;
	}

	xcvr_write(core, XCVR_REG_DRP_SEL(drp_addr), drp_sel);
	xcvr_write(core, XCVR_REG_DRP_CTRL(drp_addr), XCVR_DRP_CTRL_ADDR(reg));

	ret = xcvr_drp_wait_idle(core, drp_addr);
	if (ret < 0)
		return ret;

	return ret & 0xffff;
}

/***************************************************************************//**
 * @brief xilinx_xcvr_drp_write Dynamic reconfiguration port write access for
 * 		  Xilinx devices
 ******************************************************************************/
int32_t xilinx_xcvr_drp_write(xcvr_core *core,
			      uint32_t drp_port,
			      uint32_t reg,
			      uint32_t val)
{
	uint32_t drp_sel = XCVR_DRP_PORT_CHANNEL_BCAST;
	uint32_t drp_addr;
	int32_t ret;

	if (drp_port == XCVR_DRP_PORT_COMMON) {
		drp_addr = XCVR_DRP_PORT_ADDR_COMMON;
	} else {
		drp_addr = XCVR_DRP_PORT_ADDR_CHANNEL;
		if (drp_port != XCVR_DRP_PORT_CHANNEL_BCAST)
			drp_sel = drp_port - 1;
	}

	xcvr_write(core, XCVR_REG_DRP_SEL(drp_addr), drp_sel);
	xcvr_write(core, XCVR_REG_DRP_CTRL(drp_addr), (XCVR_DRP_CTRL_WR |
			XCVR_DRP_CTRL_ADDR(reg) | XCVR_DRP_CTRL_WDATA(val)));

	ret = xcvr_drp_wait_idle(core, drp_addr);
	if (ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
 * @brief xcvr_drp_update Dynamic reconfiguration port access for Xilinx devices
 ******************************************************************************/
int32_t xcvr_drp_update(xcvr_core *core,
			uint32_t drp_port,
			uint32_t reg,
			uint32_t mask,
			uint32_t val)
{
	int32_t ret;

	ret = xilinx_xcvr_drp_read(core, drp_port, reg);
	if (ret < 0)
		return ret;

	val |= ret & ~mask;

	return xilinx_xcvr_drp_write(core, drp_port, reg, val);
}

#endif

/*******************************************************************************
 * @brief xcvr_setup
 ******************************************************************************/
int32_t xcvr_setup(xcvr_core *core)
{
	int32_t ret = 0;

#ifdef ALTERA

	xcvr_write(core, XCVR_REG_RESETN, 0); // enter reset state

	uint32_t link_clock=0;
	uint64_t lane_rate=0;

	link_clock = (uint32_t)(core->lane_rate_kbps / 40);
	link_clock = fpll_round_rate(link_clock, core->ref_clock_khz);
	ret |= fpll_set_rate(&(core->dev.link_pll),
			     link_clock,
			     core->ref_clock_khz);

	core->link_clk_khz = link_clock;

	if (core->rx_tx_n) {
		lane_rate = altera_a10_cdr_pll_round_rate(core->lane_rate_kbps,
				core->ref_clock_khz);
		ret |= altera_a10_cdr_pll_set_rate(core,
						   lane_rate,
						   core->ref_clock_khz);
	} else {
		lane_rate = atx_pll_round_rate(core->lane_rate_kbps,
					       core->ref_clock_khz);
		ret |= atx_pll_set_rate(core, lane_rate, core->ref_clock_khz);
		altera_a10_calib_tx_pll(core);
	}

	xcvr_finalize_lane_rate_change(core); // bring out of reset - print  status

#endif
#ifdef XILINX
	uint32_t status;
	uint32_t timeout;
	uint16_t local_sys_clk_sel;
	uint32_t out_div;
	uint32_t rx_out_div;
	uint32_t tx_out_div;
	uint32_t ln_rate_kbps;
	struct xilinx_xcvr_cpll_config cpll_config;
	struct xilinx_xcvr_qpll_config qpll_config;

	xcvr_write(core, XCVR_REG_RESETN, 0);  // enter reset state

	if (core->reconfig_bypass == 0) {
		if (core->lane_rate_kbps > 12500000) {
			printf("ERROR: Max qpll lane_rate: %d", 12500000);
			return -1;
		}
		if (core->lane_rate_kbps < 1250000) {
			printf("ERROR: Min cpll lane_rate: %d", 1250000);
			return -1;
		}

		if (core->dev.cpll_enable == 0) {
			printf("\nQPLL ENABLE\n");
			ret |= xilinx_xcvr_calc_qpll_config(core, core->ref_rate_khz,
							    core->lane_rate_kbps, &qpll_config,
							    &core->dev.out_div);
			out_div = core->dev.out_div;

			ln_rate_kbps = xilinx_xcvr_qpll_calc_lane_rate(core,
					core->ref_rate_khz, &qpll_config, out_div);

			xilinx_xcvr_qpll_write_config(core,
						      XCVR_DRP_PORT_COMMON,
						      &qpll_config);
			local_sys_clk_sel = 0x3;
		} else {
			printf("\nCPLL ENABLE\n");
			ret |= xilinx_xcvr_calc_cpll_config(core, core->ref_rate_khz,
							    core->lane_rate_kbps, &cpll_config,
							    &core->dev.out_div);
			out_div = core->dev.out_div;

			ln_rate_kbps = xilinx_xcvr_cpll_calc_lane_rate(core,
					core->ref_rate_khz, &cpll_config, out_div);

			xilinx_xcvr_cpll_write_config(core,
						      XCVR_DRP_PORT_CHANNEL_BCAST,
						      &cpll_config);
			local_sys_clk_sel = 0;
		}


		if ((ln_rate_kbps == 0) || (ln_rate_kbps != core->lane_rate_kbps)) {
			printf("%s: Faild to set line rate!",__func__);
			printf("Desired rate: %lu, obtained rate: %lu\n",
			       core->lane_rate_kbps, ln_rate_kbps);
			return -1;
		}

		core->dev.sys_clk_sel = local_sys_clk_sel;

		xilinx_xcvr_read_out_div(core,
					 XCVR_DRP_PORT_COMMON,
					 &rx_out_div,
					 &tx_out_div);

		if (core->rx_tx_n) {
			xilinx_xcvr_configure_lpm_dfe_mode(core,
							   XCVR_DRP_PORT_COMMON,
							   core->dev.lpm_enable);
			xilinx_xcvr_configure_cdr(core,
						  XCVR_DRP_PORT_COMMON,
						  core->lane_rate_kbps,
						  out_div,
						  core->dev.lpm_enable);
			rx_out_div = out_div;
		} else {
			tx_out_div = out_div;
		}

		core->dev.out_div = out_div;
		xilinx_xcvr_write_out_div(core,
					  XCVR_DRP_PORT_COMMON,
					  rx_out_div,
					  tx_out_div);

		xcvr_write(core, XCVR_REG_CONTROL,
			   (core->dev.lpm_enable ? XCVR_LPM_DFE_N : 0) |
			   XCVR_SYSCLK_SEL(core->dev.sys_clk_sel) |
			   XCVR_OUTCLK_SEL(core->dev.out_clk_sel));

	}

	xcvr_write(core, XCVR_REG_RESETN, XCVR_RESETN);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout = timeout - 1;
		xcvr_read(core, XCVR_REG_STATUS, &status);
		if (status == 1)
			break;
	}

	if (status == 0) {
		printf("%s ERROR: XCVR initialization failed!\n", __func__);
		return(-1);
	}
#endif

	return(0);
}

/*******************************************************************************
 * @brief xcvr_getconfig
 ******************************************************************************/
int32_t xcvr_getconfig(xcvr_core *core)
{
	uint32_t regbuf;

	xcvr_read(core, XCVR_REG_SYNTH, &regbuf);
	core->num_lanes = (regbuf & XCVR_NUM_OF_LANES_MASK) >>
			  XCVR_NUM_OF_LANES_OFFSET;
	core->rx_tx_n = ((regbuf & XCVR_TX_OR_RXN_MASK) >> XCVR_TX_OR_RXN_OFFSET);

#ifdef XILINX
	core->dev.type = (regbuf & XCVR_GT_TYPE_MASK) >> XCVR_GT_TYPE_OFFSET;
	core->dev.cpll_enable = ((regbuf & XCVR_QPLL_ENABLE_MASK) >>
				 XCVR_QPLL_ENABLE_OFFSET) ? 0 : 1;

	xcvr_read(core, XCVR_REG_CONTROL, &regbuf);
	core->dev.lpm_enable = (regbuf & (0x1 << 12)) >> 12;
	core->dev.out_div = (regbuf & (0x7 << 8)) >> 8;
	core->dev.sys_clk_sel = (regbuf & (0x3 << 4)) >> 4;
	core->dev.out_clk_sel = regbuf & 0x7;
#endif

#ifdef ALTERA
	core->refclk_ppm = PM_1000;

	core->dev.link_pll.type = fpll_type;
	core->dev.atx_pll.type = atx_type;

	for (int i=1; i < core->lanes_per_link; i++) {
		core->dev.channel_pll[i].type = core->dev.channel_pll[0].type;
		core->dev.channel_pll[i].base_address =
			core->dev.channel_pll[0].base_address + 0x1000 * i;
		core->dev.channel_pll[i].initial_recalc =
			core->dev.channel_pll[0].initial_recalc;
	}
#endif

	return 0;
}

/*******************************************************************************
 * @brief xcvr_reset
 ******************************************************************************/
int32_t xcvr_reset(xcvr_core *core)
{
	uint32_t status;
	int32_t timeout;

	xcvr_write(core, XCVR_REG_RESETN, 0);

	xcvr_write(core, XCVR_REG_RESETN, XCVR_RESETN);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout = timeout - 1;
		xcvr_read(core, XCVR_REG_STATUS, &status);
		if (status == 1)
			break;
	}

	if (status == 0) {
		printf("%s ERROR: XCVR initialization failed!\n", __func__);
		return(-1);
	}

	return(0);
}

#ifdef ALTERA

/*******************************************************************************
 * @brief xcvr_finalize_lane_rate_change
 ******************************************************************************/
void xcvr_finalize_lane_rate_change(xcvr_core *core)
{
	uint32_t status;
	int timeout = 1000;
	uint32_t i;

	mdelay(100);

	xcvr_write(core, XCVR_REG_RESETN, 1);
	do {
		xcvr_read(core, XCVR_REG_STATUS, &status);
		if (status == XCVR_STATUS)
			break;
		mdelay(1);
	} while (timeout--);

	if (timeout < 0) {
		xcvr_read(core, XCVR_REG_STATUS2, &status);
		printf("Link activation error:\n");
		printf( "\tLink PLL %s locked\n", \
			((status & XCVR_STATUS2_XCVR(core->lanes_per_link)) ? "" : "not"));
		for (i = 0; i < core->lanes_per_link; i++) {
			printf("\tLane %d transceiver %s ready\n", i, \
			       ((status & XCVR_STATUS2_XCVR(i)) ?"" : "not"));
		}
	}
}
#endif

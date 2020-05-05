/***************************************************************************//**
 *   @file   app_clock.c
 *   @brief  Application clocks initialization.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "spi.h"
#include "spi_extra.h"
#include "hmc7044.h"
#include "error.h"
#include "app_parameters.h"
#include "app_clock.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct hmc7044_dev* hmc7044_dev;

struct clk_hw hmc7044_hw;

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * @brief Application clock setup.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t app_clock_init(struct clk dev_refclk[MULTIDEVICE_INSTANCE_COUNT])
{
	int32_t ret;

	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PL,
		.device_id = 0,
	};

	struct spi_init_param clkchip_spi_init_param = {
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_0,
		.chip_select = CLK_CS,
		.extra = &xil_spi_param
	};

	struct hmc7044_chan_spec chan_spec[6] = {
		{
			.num = 2,		// DEV_REFCLK
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 3,		// DEV_SYSREF
			.divider = 1536,	// 1.953125 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 6,		// CORE_CLK_TX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 8,		// CORE_CLK_RX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 12,		// FPGA_REFCLK
			.divider = 6,		// 500 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 13,		// FPGA_SYSREF
			.divider = 1536,	// 1.953125 MHz
			.driver_mode = 2,	// LVDS
		}
	};

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &clkchip_spi_init_param,
		.clkin_freq = {122880000, 30720000, 0, 0},
		.vcxo_freq = 122880000,
		.pll2_freq = 3000000000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 1024,
		.in_buf_mode = {0x07, 0x07, 0x00, 0x00, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
		.gpo_ctrl = {0x37, 0x33, 0x00, 0x00},
		.num_channels = 6,
		.pll1_ref_prio_ctrl = 0xe4,
		.sync_pin_mode = 0x1,
		.high_performance_mode_clock_dist_en = false,
		.high_performance_mode_pll_vco_en = false,
		.pulse_gen_mode = 0x0,
		.channels = chan_spec
	};

	ret = hmc7044_init(&hmc7044_dev, &hmc7044_param);
	if (ret)
		return ret;

	hmc7044_hw.dev = hmc7044_dev;
	hmc7044_hw.dev_clk_recalc_rate = hmc7044_clk_recalc_rate;
	hmc7044_hw.dev_clk_round_rate = hmc7044_clk_round_rate;
	hmc7044_hw.dev_clk_set_rate = hmc7044_clk_set_rate;

	dev_refclk[0].hw = &hmc7044_hw;
	dev_refclk[0].hw_ch_num = 0;
	dev_refclk[0].name = "dev_refclk";

	return SUCCESS;
}

/**
 * @brief Application clocking remove.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t app_clock_remove(void)
{
	return hmc7044_remove(hmc7044_dev);
}

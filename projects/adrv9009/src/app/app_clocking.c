/***************************************************************************//**
 *   @file   app_clocking.c
 *   @brief  Clock setup and initialization routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
// stdlibs
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "app_config.h"

// clock chips
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
#include "hmc7044.h"
#else
#include "ad9528.h"
#endif

// platform drivers
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"

// platform specific
#ifdef ALTERA_PLATFORM
#include "clk_altera_a10_fpll.h"
#include "altera_spi.h"
#include "altera_gpio.h"
#else
#include "xil_cache.h"
#include "xilinx_spi.h"
#if !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
#include "clk_axi_clkgen.h"
#include "xilinx_gpio.h"
#endif
#endif

// hal
#include "parameters.h"
#include "adi_hal.h"

// devices
#include "app_talise.h"

// header
#include "app_clocking.h"

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
struct hmc7044_dev* clkchip_device;
struct hmc7044_dev * car_clkchip_device;
#else
struct ad9528_dev* clkchip_device;
#endif

#ifdef ALTERA_PLATFORM
struct altera_a10_fpll *rx_device_clk_pll;
struct altera_a10_fpll *tx_device_clk_pll;
struct altera_a10_fpll *rx_os_device_clk_pll;
#elif !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
struct axi_clkgen *rx_clkgen;
struct axi_clkgen *tx_clkgen;
struct axi_clkgen *rx_os_clkgen;
#endif

adiHalErr_t clocking_init(uint32_t rx_div40_rate_hz,
			  uint32_t tx_div40_rate_hz,
			  uint32_t rx_os_div40_rate_hz,
			  uint32_t device_clock_khz,
			  uint32_t lmfc_rate_hz)
{
	int32_t status;
	uint64_t dev_clk, fmc_clk;
	uint64_t rate_dev = device_clock_khz * 1000;
	uint64_t rate_fmc = device_clock_khz * 1000;
	uint32_t n;
	int ret;

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	struct hmc7044_init_param hmc7044_param = {
		.spi_init = NULL,
		.clkin_freq = {30720000, 30720000, 0, 0},
		.vcxo_freq = 122880000,
		.pll2_freq = 2949120000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 3840,
		.in_buf_mode = {0x09, 0x09, 0x00, 0x00, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x11},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 10,
		.pll1_ref_prio_ctrl = 0xE5,
		.sync_pin_mode = 0x1,
		.high_performance_mode_clock_dist_en = true,
		.pulse_gen_mode = 0x0,
	};

	struct hmc7044_chan_spec chan_spec[10] = {
		/* DEV_REFCLK_A */
		{
			.disable = 0, .num = 0, .divider = 12, .driver_mode = 2,
			.coarse_delay = 15
		},
		/* DEV_SYSREF_A */
		{
			.disable = 0, .num = 1, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.output_control0_rb4_enable = true,
			.force_mute_enable = true,
			.driver_impedance = 1
		},
		/* DEV_REFCLK_B */
		{
			.disable = 0, .num = 2,  .divider = 12, .driver_mode = 2,
			.coarse_delay = 15
		},
		/* DEV_SYSREF_B */
		{
			.disable = 0, .num = 3,  .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.output_control0_rb4_enable = true,
			.force_mute_enable = true,
			.driver_impedance = 1
		},
		/* JESD_REFCLK_TX_OBS_AB */
		{
			.disable = 0, .num = 4,  .divider = 12, .driver_mode = 1
		},
		/* JESD_REFCLK_RX_AB */
		{
			.disable = 0, .num = 5,  .divider = 12, .driver_mode = 1
		},
#if defined(ZU11EG)
		/* CORE_CLK_TX_OBS_AB */
		{
			.disable = 0, .num = 6,
			.divider = hmc7044_param.pll2_freq / tx_div40_rate_hz,
			.driver_mode = 0,
			.driver_impedance = 1
		},
		/* CORE_CLK_RX_AB */
		{
			.disable = 0, .num = 7,
			.divider = hmc7044_param.pll2_freq / rx_div40_rate_hz,
			.driver_mode = 0,
			.driver_impedance = 1
		},
		/* FPGA_SYSREF_TX_OBS_AB */
		{
			.disable = 0, .num = 8, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.output_control0_rb4_enable = true,
			.force_mute_enable = true
		},
		/* FPGA_SYSREF_RX_AB */
		{
			.disable = 0, .num = 9, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.output_control0_rb4_enable = true,
			.force_mute_enable = true
		}
#elif defined(FMCOMMS8_ZCU102)
		/* FPGA_SYSREF_TX_OBS_AB */
		{
			.disable = 0, .num = 6, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.output_control0_rb4_enable = true,
			.force_mute_enable = true
		},
		/* FPGA_SYSREF_RX_AB */
		{
			.disable = 0, .num = 7, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.output_control0_rb4_enable = true,
			.force_mute_enable = true
		},
		/* CORE_CLK_TX_OBS_AB */
		{
			.disable = 0, .num = 8, .divider = 24, .driver_mode = 0,
			.driver_impedance = 1
		},
		/* CORE_CLK_RX_AB */
		{
			.disable = 0, .num = 9, .divider = 12, .driver_mode = 0,
			.driver_impedance = 1
		}
#endif
	};
	hmc7044_param.channels = chan_spec;

	struct hmc7044_chan_spec car_chan_spec[2] = {
		/* REFCLK_OUT2 */
		{
			.disable = 0, .num = 2, .divider = 24, .driver_mode = 1,
		},
		/* SYNC_OUT1 */
		{
			.disable = 0, .num = 5, .divider = 3840, .driver_mode = 3,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.driver_impedance = 3
		}
	};

#if defined(ZU11EG)
	car_chan_spec[0].divider = 0x60;
#endif

	struct hmc7044_init_param hmc7044_car_param = {
		.spi_init = NULL,
		.clkin_freq = {122880000, 30720000, 0, 38400000},
		.vcxo_freq = 122880000,
		.pll2_freq = 2949120000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 3840,
		.in_buf_mode = {0x07, 0x07, 0x00, 0x11, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x11},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 2,
		.pll1_ref_prio_ctrl = 0xB1,
		.sync_pin_mode = 0x1,
		.pulse_gen_mode = 0x1,
		.channels = car_chan_spec
	};
#else
	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param;
	struct ad9528_platform_data ad9528_pdata;

	// ad9528 defaults
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 14;
	ad9528_param.pdata->channels = &ad9528_channels[0];

	status = ad9528_init(&ad9528_param);
	if(status) {
		printf("error: ad9528_init() failed with %d\n", status);
		goto error_0;
	}

	// ad9528 channel defaults
	for(unsigned int ch = 0; ch < ad9528_param.pdata->num_channels; ch++) {
		ad9528_channels[ch].channel_num = ch;
		ad9528_channels[ch].output_dis = 1;
	}

	// ad9528 channel specifics

	// adrv9009 device clock
	ad9528_channels[13].output_dis = 0;
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].divider_phase = 0;
	ad9528_channels[13].signal_source = SOURCE_VCO;

	// fpga device clock
	ad9528_channels[1].output_dis = 0;
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].divider_phase = 0;
	ad9528_channels[1].signal_source = SOURCE_VCO;

	// adrv9009 sysref
	ad9528_channels[12].output_dis = 0;
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].divider_phase = 0;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;

	// fpga sysref
	ad9528_channels[3].output_dis = 0;
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].divider_phase = 0;
	ad9528_channels[3].signal_source = SOURCE_SYSREF_VCO;

	// ad9528 settings
	ad9528_param.pdata->spi3wire = 0;
	ad9528_param.pdata->vcxo_freq = 122880000;
	ad9528_param.pdata->refa_en = 1;
	ad9528_param.pdata->refa_diff_rcv_en = 1;
	ad9528_param.pdata->refa_r_div = 1;
	ad9528_param.pdata->osc_in_cmos_neg_inp_en = 1;
	ad9528_param.pdata->pll1_feedback_div = 4;
	ad9528_param.pdata->pll1_feedback_src_vcxo = 0; /* VCO */
	ad9528_param.pdata->pll1_charge_pump_current_nA = 5000;
	ad9528_param.pdata->pll1_bypass_en = 0;
	ad9528_param.pdata->pll2_vco_div_m1 = 3;
	ad9528_param.pdata->pll2_n2_div = 10;
	ad9528_param.pdata->pll2_r1_div = 1;
	ad9528_param.pdata->pll2_charge_pump_current_nA = 805000;
	ad9528_param.pdata->pll2_bypass_en = false;
	ad9528_param.pdata->sysref_src = SYSREF_SRC_INTERNAL;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_CONTINUOUS;
	ad9528_param.pdata->sysref_req_en = true;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_4_PULSES;
	ad9528_param.pdata->sysref_req_trigger_mode = SYSREF_LEVEL_HIGH;
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	ad9528_param.pdata->stat0_pin_func_sel = 0x1; /* PLL1 & PLL2 Locked */
	ad9528_param.pdata->stat1_pin_func_sel = 0x7; /* REFA Correct */
#endif

#ifdef ALTERA_PLATFORM
	struct altera_spi_init_param ad9528_spi_param = {
		.type = NIOS_II_SPI,
		.device_id = 0,
		.base_address = SPI_BASEADDR
	};
	struct altera_gpio_init_param ad9528_gpio_param = {
		.type = NIOS_II_GPIO,
		.device_id = 0,
		.base_address = GPIO_BASEADDR
	};
	struct altera_a10_fpll_init rx_device_clk_pll_init = {
		"rx_device_clk_pll",
		RX_A10_FPLL_BASEADDR,
		device_clock_khz * 1000
	};
	struct altera_a10_fpll_init tx_device_clk_pll_init = {
		"tx_device_clk_pll",
		TX_A10_FPLL_BASEADDR,
		device_clock_khz * 1000
	};
	struct altera_a10_fpll_init rx_os_device_clk_pll_init = {
		"rx_os_device_clk_pll",
		RX_OS_A10_FPLL_BASEADDR,
		device_clock_khz * 1000
	};
	struct altera_a10_fpll *rx_device_clk_pll;
	struct altera_a10_fpll *tx_device_clk_pll;
	struct altera_a10_fpll *rx_os_device_clk_pll;
#else
	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
		.flags = SPI_CS_DECODE
#endif
	};
#if !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID,
	};
	struct axi_clkgen_init rx_clkgen_init = {
		"rx_clkgen",
		RX_CLKGEN_BASEADDR,
		device_clock_khz * 1000
	};
	struct axi_clkgen_init tx_clkgen_init = {
		"tx_clkgen",
		TX_CLKGEN_BASEADDR,
		device_clock_khz * 1000
	};
	struct axi_clkgen_init rx_os_clkgen_init = {
		"rx_os_clkgen",
		RX_OS_CLKGEN_BASEADDR,
		device_clock_khz * 1000
	};
#endif
#endif

	// clock chip spi settings
	struct no_os_spi_init_param clkchip_spi_init_param = {
		.device_id = 0,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = CLK_CS,
#ifndef ALTERA_PLATFORM
		.platform_ops = &xil_spi_ops,
#else
		.platform_ops = &altera_spi_ops,
#endif
		.extra = &xil_spi_param
	};

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	// clock chip spi settings
	struct no_os_spi_init_param car_clkchip_spi_init_param = {
		.device_id = 0,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = CAR_CLK_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};
	hmc7044_car_param.spi_init = &car_clkchip_spi_init_param;

	hmc7044_param.spi_init = &clkchip_spi_init_param;
#else
	ad9528_param.spi_init = clkchip_spi_init_param;
#endif

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	// reset pin not needed, hmc7044_init performs a soft reset over SPI.
#else
	struct no_os_gpio_init_param clkchip_gpio_init_param = {
		.number = CLK_RESETB_GPIO,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};
	ad9528_param.gpio_resetb = &clkchip_gpio_init_param;
#endif

	/** < Insert User System Clock(s) Initialization Code Here >
	* System Clock should provide a device clock and SYSREF signal
	* to the Talise device.
	**/
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	status = hmc7044_init(&car_clkchip_device, &hmc7044_car_param);
	if (status != 0) {
		printf("hmc7044_init() error: %d\n", status);
		goto error_1;
	}
	status = hmc7044_init(&clkchip_device, &hmc7044_param);
	if (status != 0) {
		printf("hmc7044_init() error: %d\n", status);
		goto error_1;
	}
	status = hmc7044_clk_round_rate(clkchip_device, device_clock_khz * 1000,
					&dev_clk);
	if (status != 0) {
		printf("hmc7044_clk_round_rate() error: %d\n", status);
		goto error_1;
	}
	status = hmc7044_clk_round_rate(clkchip_device, device_clock_khz * 1000,
					&fmc_clk);
	if (status != 0) {
		printf("hmc7044_clk_round_rate() error: %d\n", status);
		goto error_1;
	}
#else
	status = ad9528_setup(&clkchip_device, ad9528_param);
	if(status < 0) {
		printf("error: ad9528_setup() failed with %d\n", status);
		goto error_1;
	}

	dev_clk = ad9528_clk_round_rate(clkchip_device, DEV_CLK,
					device_clock_khz * 1000);

	fmc_clk = ad9528_clk_round_rate(clkchip_device, FMC_CLK,
					device_clock_khz * 1000);
#endif
	if (dev_clk > 0 && fmc_clk > 0 && fmc_clk == dev_clk &&
	    (dev_clk / 1000) == device_clock_khz) {
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
		ret = hmc7044_clk_set_rate(clkchip_device, DEV_REFCLK_A, dev_clk);
		if (ret != 0) {
			printf("hmc7044_clk_set_rate() error: %d\n", status);
			goto error_1;
		}
		ret = hmc7044_clk_set_rate(clkchip_device, DEV_REFCLK_B, dev_clk);
		if (ret != 0) {
			printf("hmc7044_clk_set_rate() error: %d\n", status);
			goto error_1;
		}
		ret = hmc7044_clk_set_rate(clkchip_device, JESD_REFCLK_TX_OBS_AB, fmc_clk);
		if (ret != 0) {
			printf("hmc7044_clk_set_rate() error: %d\n", status);
			goto error_1;
		}
		ret = hmc7044_clk_set_rate(clkchip_device, JESD_REFCLK_RX_AB, fmc_clk);
		if (ret != 0) {
			printf("hmc7044_clk_set_rate() error: %d\n", status);
			goto error_1;
		}
#else
		ad9528_clk_set_rate(clkchip_device, DEV_CLK, dev_clk);
		ad9528_clk_set_rate(clkchip_device, FMC_CLK, fmc_clk);
#endif
	} else {
		printf("Requesting device clock %u failed got %u\n",
		       device_clock_khz * 1000, dev_clk);
		goto error_1;
	}

	/* If the current rate is not OK, change it */
	if (!(adrv9009_check_sysref_rate(lmfc_rate_hz, rate_dev) &&
	      (rate_fmc == rate_dev))) {
		/*
		* Try to find a rate that integer divides the LMFC. Starting with a low
		* rate is a good idea and then slowly go up in case the clock generator
		* can't generate such slow rates.
		*/
		for (n = 64; n > 0; n--) {
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
			hmc7044_clk_round_rate(clkchip_device, lmfc_rate_hz / n,
					       &rate_dev);
#else
			rate_dev = ad9528_clk_round_rate(clkchip_device, DEV_SYSREF, lmfc_rate_hz / n);
#endif
			if (adrv9009_check_sysref_rate(lmfc_rate_hz, rate_dev))
				break;
		}

		if (n == 0) {
			printf("Could not find suitable SYSREF rate for LMFC of %u\n", lmfc_rate_hz);
			goto error_1;
		}

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
		ret = hmc7044_clk_set_rate(clkchip_device, JESD_REFCLK_TX_OBS_AB, rate_fmc);
		if (ret)
			printf("Failed to set JESD_REFCLK_TX_OBS_AB rate to %u Hz: %d\n",
			       rate_fmc, ret);
		ret = hmc7044_clk_set_rate(clkchip_device, JESD_REFCLK_RX_AB, rate_fmc);
		if (ret)
			printf("Failed to set JESD_REFCLK_RX_AB rate to %u Hz: %d\n",
			       rate_fmc, ret);
#else
		ret = ad9528_clk_set_rate(clkchip_device, FMC_SYSREF, rate_fmc);
		if (ret)
			printf("Failed to set FMC SYSREF rate to %u Hz: %d\n",
			       rate_fmc, ret);
#endif

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
		ret = hmc7044_clk_set_rate(clkchip_device, DEV_SYSREF_A, rate_dev);
		if (ret)
			printf("Failed to set DEV SYSREF A rate to %u Hz: %d\n",
			       rate_dev, ret);
		ret = hmc7044_clk_set_rate(clkchip_device, DEV_SYSREF_B, rate_dev);
		if (ret)
			printf("Failed to set DEV SYSREF B rate to %u Hz: %d\n",
			       rate_dev, ret);
#else
		ret = ad9528_clk_set_rate(clkchip_device, DEV_SYSREF, rate_dev);
		if (ret)
			printf("Failed to set DEV SYSREF rate to %u Hz: %d\n",
			       rate_fmc, ret);
#endif
	}

#ifdef ALTERA_PLATFORM
#ifndef ADRV9008_2
	/* Initialize A10 FPLLs */
	status = altera_a10_fpll_init(&rx_device_clk_pll,
				      &rx_device_clk_pll_init);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_device_clk_pll_init.name);
		goto error_1;
	}
#endif
#ifndef ADRV9008_1
	status = altera_a10_fpll_init(&tx_device_clk_pll,
				      &tx_device_clk_pll_init);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       tx_device_clk_pll_init.name);
		goto error_2;
	}
	status = altera_a10_fpll_init(&rx_os_device_clk_pll,
				      &rx_os_device_clk_pll_init);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_os_device_clk_pll_init.name);
		goto error_3;
	}

#endif
#ifndef ADRV9008_2
	altera_a10_fpll_disable(rx_device_clk_pll);
	status = altera_a10_fpll_set_rate(rx_device_clk_pll,
					  rx_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       rx_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(rx_device_clk_pll);
#endif
#ifndef ADRV9008_1
	altera_a10_fpll_disable(tx_device_clk_pll);
	status = altera_a10_fpll_set_rate(tx_device_clk_pll,
					  tx_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       tx_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(tx_device_clk_pll);
	altera_a10_fpll_disable(rx_os_device_clk_pll);
	status = altera_a10_fpll_set_rate(rx_os_device_clk_pll,
					  rx_os_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       rx_os_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(rx_os_device_clk_pll);
#endif
#else
#if !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
	/* Initialize CLKGEN */
#ifndef ADRV9008_2
	status = axi_clkgen_init(&rx_clkgen, &rx_clkgen_init);
	if (status != 0) {
		printf("error: %s: axi_clkgen_init() failed\n", rx_clkgen_init.name);
		goto error_1;
	}
#endif
#ifndef ADRV9008_1
	status = axi_clkgen_init(&tx_clkgen, &tx_clkgen_init);
	if (status != 0) {
		printf("error: %s: axi_clkgen_init() failed\n", tx_clkgen_init.name);
		goto error_2;
	}
	status = axi_clkgen_init(&rx_os_clkgen, &rx_os_clkgen_init);
	if (status != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_os_clkgen_init.name);
		goto error_3;
	}

#endif
#ifndef ADRV9008_2
	status = axi_clkgen_set_rate(rx_clkgen, rx_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_clkgen->name);
		goto error_4;
	}
#endif
#ifndef ADRV9008_1
	status = axi_clkgen_set_rate(tx_clkgen, tx_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", tx_clkgen->name);
		goto error_4;
	}
	status = axi_clkgen_set_rate(rx_os_clkgen, rx_os_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_os_clkgen->name);
		goto error_4;
	}
#endif
#endif
#endif

	return ADIHAL_OK;

#if !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
error_4:
#endif
#ifdef ALTERA_PLATFORM
#ifndef ADRV9008_1
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#endif
#elif !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
#ifndef ADRV9008_1
	axi_clkgen_remove(rx_os_clkgen);
#endif
#endif

#if !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
error_3:
#endif
#ifdef ALTERA_PLATFORM
#ifndef ADRV9008_1
	altera_a10_fpll_remove(tx_device_clk_pll);
#endif
#elif !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
#ifndef ADRV9008_1
	axi_clkgen_remove(tx_clkgen);
#endif
#endif

#if !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
error_2:
#endif
#ifdef ALTERA_PLATFORM
#ifndef ADRV9008_2
	altera_a10_fpll_remove(rx_device_clk_pll);
#endif
#elif !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
#ifndef ADRV9008_2
	axi_clkgen_remove(rx_clkgen);
#endif
#endif
error_1:
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	hmc7044_remove(clkchip_device);
#else
	ad9528_remove(clkchip_device);
error_0:
#endif
	return ADIHAL_ERR;
}

void clocking_deinit(void)
{
#ifdef ALTERA_PLATFORM
#ifndef ADRV9008_2
	altera_a10_fpll_remove(rx_device_clk_pll);
#endif
#ifndef ADRV9008_1
	altera_a10_fpll_remove(tx_device_clk_pll);
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#endif
#elif !defined(ZU11EG) && !defined(FMCOMMS8_ZCU102)
#ifndef ADRV9008_1
	axi_clkgen_remove(rx_os_clkgen);
	axi_clkgen_remove(tx_clkgen);
#endif
#ifndef ADRV9008_2
	axi_clkgen_remove(rx_clkgen);
#endif
#endif

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	hmc7044_remove(clkchip_device);
#else
	ad9528_remove(clkchip_device);
#endif
}

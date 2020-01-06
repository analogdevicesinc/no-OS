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

// clock chips
#include "ad9528.h"

// platform drivers
#include "spi.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "error.h"
#include "delay.h"
#include "util.h"

// platform specific
#ifdef ALTERA_PLATFORM
#include "clk_altera_a10_fpll.h"
#else
#include "xil_cache.h"
#if !defined(ZU11EG)
#include "clk_axi_clkgen.h"
#endif
#endif

// hal
#include "parameters.h"
#include "adi_hal.h"

// devices
#include "ad9528.h"
#include "app_talise.h"

// header
#include "app_clocking.h"

struct ad9528_dev* ad9528_device;

#ifdef ALTERA_PLATFORM
struct altera_a10_fpll *rx_device_clk_pll;
struct altera_a10_fpll *tx_device_clk_pll;
struct altera_a10_fpll *rx_os_device_clk_pll;
#elif !defined(ZU11EG)
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
	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param;
	struct ad9528_platform_data ad9528_pdata;
	uint32_t dev_clk, fmc_clk;
	uint32_t rate_dev = device_clock_khz * 1000;
	uint32_t rate_fmc = device_clock_khz * 1000;
	uint32_t n;
	int ret;

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
	struct xil_spi_init_param ad9528_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = 0
	};
	struct xil_gpio_init_param ad9528_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID,
	};
#if !defined(ZU11EG)
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

	// ad9528 spi settings
	struct spi_init_param ad9528_spi_init_param = {
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_0,
		.chip_select = CLK_CS,
		.extra = &ad9528_spi_param
	};

	ad9528_param.spi_init = ad9528_spi_init_param;

	struct gpio_init_param ad9528_gpio_init_param = {
		.number = CLK_RESETB_GPIO,
		.extra = &ad9528_gpio_param
	};

	ad9528_param.gpio_resetb = ad9528_gpio_init_param;

	/** < Insert User System Clock(s) Initialization Code Here >
	* System Clock should provide a device clock and SYSREF signal
	* to the Talise device.
	**/
	status = ad9528_setup(&ad9528_device, ad9528_param);
	if(status < 0) {
		printf("error: ad9528_setup() failed with %d\n", status);
		goto error_1;
	}

	dev_clk = ad9528_clk_round_rate(ad9528_device, DEV_CLK,
					device_clock_khz * 1000);

	fmc_clk = ad9528_clk_round_rate(ad9528_device, FMC_CLK,
					device_clock_khz * 1000);

	if (dev_clk > 0 && fmc_clk > 0 && fmc_clk == dev_clk &&
	    (dev_clk / 1000) == device_clock_khz) {
		ad9528_clk_set_rate(ad9528_device, DEV_CLK, dev_clk);
		ad9528_clk_set_rate(ad9528_device, FMC_CLK, fmc_clk);
	} else {
		printf("Requesting device clock %u failed got %u",
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
			rate_dev = ad9528_clk_round_rate(ad9528_device, DEV_SYSREF, lmfc_rate_hz / n);
			if (adrv9009_check_sysref_rate(lmfc_rate_hz, rate_dev))
				break;
		}

		if (n == 0) {
			printf("Could not find suitable SYSREF rate for LMFC of %u\n", lmfc_rate_hz);
			goto error_1;
		}

		ret = ad9528_clk_set_rate(ad9528_device, FMC_SYSREF, rate_fmc);
		if (ret)
			printf("Failed to set FMC SYSREF rate to %u Hz: %d\n",
			       rate_fmc, ret);

		ret = ad9528_clk_set_rate(ad9528_device, DEV_SYSREF, rate_dev);
		if (ret)
			printf("Failed to set DEV SYSREF rate to %u Hz: %d\n",
			       rate_fmc, ret);
	}

#ifdef ALTERA_PLATFORM
	/* Initialize A10 FPLLs */
	status = altera_a10_fpll_init(&rx_device_clk_pll,
				      &rx_device_clk_pll_init);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_os_device_clk_pll_init.name);
		goto error_1;
	}
	status = altera_a10_fpll_init(&tx_device_clk_pll,
				      &tx_device_clk_pll_init);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_os_device_clk_pll_init.name);
		goto error_2;
	}
	status = altera_a10_fpll_init(&rx_os_device_clk_pll,
				      &rx_os_device_clk_pll_init);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_os_device_clk_pll_init.name);
		goto error_3;
	}

	altera_a10_fpll_disable(rx_device_clk_pll);
	status = altera_a10_fpll_set_rate(rx_device_clk_pll,
					  rx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       rx_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(rx_device_clk_pll);
	altera_a10_fpll_disable(tx_device_clk_pll);
	status = altera_a10_fpll_set_rate(tx_device_clk_pll,
					  tx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       tx_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(tx_device_clk_pll);
	altera_a10_fpll_disable(rx_os_device_clk_pll);
	status = altera_a10_fpll_set_rate(rx_os_device_clk_pll,
					  rx_os_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       rx_os_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(rx_os_device_clk_pll);
#else
#if !defined(ZU11EG)
	/* Initialize CLKGEN */
	status = axi_clkgen_init(&rx_clkgen, &rx_clkgen_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_init() failed\n", rx_clkgen_init.name);
		goto error_1;
	}
	status = axi_clkgen_init(&tx_clkgen, &tx_clkgen_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_init() failed\n", tx_clkgen_init.name);
		goto error_2;
	}
	status = axi_clkgen_init(&rx_os_clkgen, &rx_os_clkgen_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_os_clkgen_init.name);
		goto error_3;
	}

	status = axi_clkgen_set_rate(rx_clkgen, rx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_clkgen->name);
		goto error_4;
	}
	status = axi_clkgen_set_rate(tx_clkgen, tx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", tx_clkgen->name);
		goto error_4;
	}
	status = axi_clkgen_set_rate(rx_os_clkgen, rx_os_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_os_clkgen->name);
		goto error_4;
	}
#endif
#endif

	return ADIHAL_OK;

#if !defined(ZU11EG)
error_4:
#endif
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#elif !defined(ZU11EG)
	axi_clkgen_remove(rx_os_clkgen);
#endif

#if !defined(ZU11EG)
error_3:
#endif
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(tx_device_clk_pll);
#elif !defined(ZU11EG)
	axi_clkgen_remove(tx_clkgen);
#endif

#if !defined(ZU11EG)
error_2:
#endif
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_device_clk_pll);
#elif !defined(ZU11EG)
	axi_clkgen_remove(rx_clkgen);
#endif
error_1:
	ad9528_remove(ad9528_device);
error_0:
	return ADIHAL_ERR;
}

void clocking_deinit(void)
{
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_device_clk_pll);
	altera_a10_fpll_remove(tx_device_clk_pll);
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#elif !defined(ZU11EG)
	axi_clkgen_remove(rx_os_clkgen);
	axi_clkgen_remove(tx_clkgen);
	axi_clkgen_remove(rx_clkgen);
#endif
	ad9528_remove(ad9528_device);
}

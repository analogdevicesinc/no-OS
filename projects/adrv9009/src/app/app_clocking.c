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
#include "talise_config_ad9528.h"
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
#include "clk_axi_clkgen.h"
#endif

// hal
#include "parameters.h"
#include "adi_hal.h"

// header
#include "app_clocking.h"

ad9528Device_t clockAD9528_ = {
	NULL,
	NULL,
	&clockSpiSettings,
	&clockPll1Settings,
	&clockPll2Settings,
	&clockOutputSettings,
	&clockSysrefSettings,
	NULL,
	NULL
};

#ifdef ALTERA_PLATFORM
struct altera_a10_fpll *rx_device_clk_pll;
struct altera_a10_fpll *tx_device_clk_pll;
struct altera_a10_fpll *rx_os_device_clk_pll;
#else
struct axi_clkgen *rx_clkgen;
struct axi_clkgen *tx_clkgen;
struct axi_clkgen *rx_os_clkgen;
#endif

adiHalErr_t clocking_init(uint32_t rx_div40_rate_hz,
			  uint32_t tx_div40_rate_hz,
			  uint32_t rx_os_div40_rate_hz)
{
	adiHalErr_t ad9528Error;
	int32_t status;
	ad9528Device_t *clockAD9528_device = &clockAD9528_;

#ifdef ALTERA_PLATFORM
	struct altera_spi_init_param ad9528_spi_param = {
		.type = NIOS_II_SPI,
		.device_id = 0,
		.base_address = SPI_BASEADDR
	};
	clockAD9528_.extra_spi = &ad9528_spi_param;
	struct altera_gpio_init_param ad9528_gpio_param = {
		.type = NIOS_II_GPIO,
		.device_id = 0,
		.base_address = GPIO_BASEADDR
	};
	clockAD9528_.extra_gpio = &ad9528_gpio_param;
	struct altera_a10_fpll_init rx_device_clk_pll_init = {
		"rx_device_clk_pll",
		RX_A10_FPLL_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct altera_a10_fpll_init tx_device_clk_pll_init = {
		"tx_device_clk_pll",
		TX_A10_FPLL_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct altera_a10_fpll_init rx_os_device_clk_pll_init = {
		"rx_os_device_clk_pll",
		RX_OS_A10_FPLL_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
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
	clockAD9528_.extra_spi = &ad9528_spi_param;
	struct xil_gpio_init_param ad9528_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID,
	};
	clockAD9528_.extra_gpio = &ad9528_gpio_param;
	struct axi_clkgen_init rx_clkgen_init = {
		"rx_clkgen",
		RX_CLKGEN_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct axi_clkgen_init tx_clkgen_init = {
		"tx_clkgen",
		TX_CLKGEN_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct axi_clkgen_init rx_os_clkgen_init = {
		"rx_os_clkgen",
		RX_OS_CLKGEN_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
#endif

	/** < Insert User System Clock(s) Initialization Code Here >
	* System Clock should provide a device clock and SYSREF signal
	* to the Talise device.
	**/
	/* Init the AD9528 data structure */
	ad9528Error = AD9528_initDeviceDataStruct(clockAD9528_device,
			clockAD9528_device->pll1Settings->vcxo_Frequency_Hz,
			clockAD9528_device->pll1Settings->refA_Frequency_Hz,
			clockAD9528_device->outputSettings->outFrequency_Hz[1]);
	if (ad9528Error != ADIHAL_OK) {
		printf("error: AD9528_initDeviceDataStruct() failed\n");
		goto error_0;
	}

	/* Perform a hard reset on the AD9528 DUT */
	ad9528Error = AD9528_resetDevice(clockAD9528_device);
	if (ad9528Error != ADIHAL_OK) {
		printf("error: AD9528_resetDevice() failed\n");
		goto error_1;
	}

	/* Initialize the AD9528 by writing all SPI registers */
	ad9528Error = AD9528_initialize(clockAD9528_device);
	if (ad9528Error == ADIHAL_ERR) {
		printf("error: AD9528_initialize() failed\n");
		goto error_1;
	}

	if (ad9528Error == ADIHAL_WARNING)
		printf("warning: AD9528_initialize() issues. "
		       "Possible cause: REF_CLK not connected\n");

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

	return ADIHAL_OK;

error_4:
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#else
	axi_clkgen_remove(rx_os_clkgen);
#endif
error_3:
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(tx_device_clk_pll);
#else
	axi_clkgen_remove(tx_clkgen);
#endif
error_2:
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_device_clk_pll);
#else
	axi_clkgen_remove(rx_clkgen);
#endif
error_1:
	AD9528_remove(&clockAD9528_);
error_0:
	return ADIHAL_ERR;
}

void clocking_deinit(void)
{
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_device_clk_pll);
	altera_a10_fpll_remove(tx_device_clk_pll);
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#else
	axi_clkgen_remove(rx_os_clkgen);
	axi_clkgen_remove(tx_clkgen);
	axi_clkgen_remove(rx_clkgen);
#endif
	AD9528_remove(&clockAD9528_);
}
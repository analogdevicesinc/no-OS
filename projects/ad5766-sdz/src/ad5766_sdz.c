/***************************************************************************//**
* @file ad5766_sdz.c
* @brief Implementation of Main Function.
* @author DBogdan (dragos.bogdan@analog.com)
* @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
* Copyright 2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xil_cache.h>
#include <xparameters.h>
#include "spi_engine.h"
#include "ad5766_core.h"
#include "ad5766.h"
#include "parameters.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_error.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"

#include "no_os_print_log.h"

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	struct spi_engine_init_param spi_eng_init_param  = {
		.ref_clk_hz = 100000000,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD5766_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 24,
	};

	struct xil_gpio_init_param gpio_extra_param = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS,
	};

	struct no_os_gpio_init_param ad5766_gpio_reset_param = {
		.number = GPIO_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};

	struct no_os_spi_init_param ad5766_spi_init = {
		.chip_select = SPI_AD5766_CS,
		.max_speed_hz = 50000000,
		.mode = NO_OS_SPI_MODE_1,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct ad5766_init_param default_init_param = {
		.spi_init = ad5766_spi_init,
		.gpio_reset = ad5766_gpio_reset_param,
		.daisy_chain_en = AD5766_DISABLE,
		.clr = AD5766_ZERO,
		.span = AD5766_M_10V_TO_P_10V,
		.pwr_dac_setting = 0,
		.pwr_dither_setting = 0,
		.dither_signal_setting = 0,
		.inv_dither_setting = 0,
		.dither_scale_setting = 0
	};
	struct ad5766_dev *dev;

	ad5766_core_init_param default_core_init_param = {
		.core_baseaddr = AD5766_CORE_BASEADDR,
		.dma_baseaddr = AD5766_DMA_BASEADDR,
		.dma_source_addr = AD5766_DDR_BASEADDR,
		.rate_hz = 800000,
		.spi_clk_hz = 50000000,
	};
	ad5766_core *core;

	ad5766_init(&dev, default_init_param);

	ad5766_core_setup(dev->spi_desc->extra, &core, default_core_init_param);

	pr_info("Done\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

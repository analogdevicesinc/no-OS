/***************************************************************************//**
* @file ad7616_sdz.c
* @brief Implementation of Main Function.
* @author DBogdan (dragos.bogdan@analog.com)
* @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "spi_engine.h"
#include <xil_cache.h>
#include <xparameters.h>
#include "no_os_error.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "ad7616.h"
#include "parameters.h"
#include "no_os_pwm.h"
#include "axi_pwm_extra.h"
#include "clk_axi_clkgen.h"
#include "xil_io.h"

#include "no_os_print_log.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#define AD7616_SDZ_SAMPLE_NO 1000

struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = AD7616_DMA_BASEADDR,
};

struct spi_engine_init_param spi_eng_init_param  = {
	.ref_clk_hz = 100000000,
	.type = SPI_ENGINE,
	.spi_engine_baseaddr = AD7616_SPI_ENGINE_BASEADDR,
	.cs_delay = 1,
	.data_width = 16,
};

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = AD7616_RX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

struct axi_pwm_init_param axi_pwm_init_params = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = 100000000,
};

struct no_os_pwm_init_param trigger_pwm_init = {
	.period_ns = 1000,	/* 1MHz */
	.duty_cycle_ns = AD7616_TRIGGER_PULSE_WIDTH_NS,  /* pulse_width = 50 */
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.extra = &axi_pwm_init_params,
	.platform_ops = &axi_pwm_ops,
};

struct no_os_spi_init_param ad7616_spi_init = {
	.chip_select = SPI_AD7616_CS,
	.max_speed_hz = 50000000,
	.mode = NO_OS_SPI_MODE_2,
	.platform_ops = &spi_eng_platform_ops,
	.extra = (void*)&spi_eng_init_param,
};

struct xil_gpio_init_param xil_gpio_param = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS,
};
struct no_os_gpio_init_param ad7616_gpio_reset = {
	.number = GPIO_ADC_RESET_N,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

struct ad7616_init_param init_param = {
	/* SPI */
	.spi_param = &ad7616_spi_init,
	.offload_init_param = &spi_engine_offload_init_param,
	.trigger_pwm_init = &trigger_pwm_init,
	.clkgen_init = &clkgen_init,
	.axi_clkgen_rate = 100000000,
	.reg_access_speed = 1000000,
	/* GPIO */
	.gpio_hw_rngsel0_param = NULL,
	.gpio_hw_rngsel1_param = NULL,
	.gpio_os0_param = NULL,
	.gpio_os1_param = NULL,
	.gpio_os2_param = NULL,
	.gpio_reset_param = &ad7616_gpio_reset,
	/* AXI Core */
	.core_baseaddr = AD7616_SPI_ENGINE_BASEADDR,
	/* Device Settings */
	.mode = AD7616_SW,
	.va = {
		AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V,
		AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V
	},
	.vb = {
		AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V,
		AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V
	},
	.osr = AD7616_OSR_0,
	.dcache_invalidate_range =
	(void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	struct ad7616_dev	*dev;
	struct ad7616_conversion_result buf[AD7616_SDZ_SAMPLE_NO] __attribute__ ((
				aligned));
	uint32_t i;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	pr_info("AD7616 Reference Design.\n");

	ad7616_setup(&dev, &init_param);

	if(dev->interface == AD7616_PARALLEL)
		ad7616_read_data_parallel(dev, buf, AD7616_SDZ_SAMPLE_NO);
	else
		ad7616_read_data_serial(dev, buf, AD7616_SDZ_SAMPLE_NO * 2);

	for (i = 0; i < AD7616_SDZ_SAMPLE_NO; i++) {
		pr_info("%u\t%u\n", buf[i].channel_a, buf[i].channel_b);
	}

	pr_info("Capture done. \n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

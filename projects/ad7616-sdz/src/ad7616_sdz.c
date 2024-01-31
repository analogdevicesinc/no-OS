/***************************************************************************//**
* @file ad7616_sdz.c
* @brief Implementation of Main Function.
* @author DBogdan (dragos.bogdan@analog.com)
* @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
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

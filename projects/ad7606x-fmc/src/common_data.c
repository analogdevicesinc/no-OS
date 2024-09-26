/***************************************************************************//**
* @file common_data.c
* @brief Definitions of initialization parameters common for all project variants
* @author Alexandru Ardelean (aardelean@baylibre.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xil_cache.h>

#include "parameters.h"
#include "common_data.h"

#include "xilinx_gpio.h"
#include "axi_pwm_extra.h"
#include "no_os_pwm.h"
#include "spi_engine.h"
#include "ad7606.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#define AD7606X_FMC_SAMPLE_NO 1000

#define AD7606X_REF_CLK_HZ	(100000000)

static struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = AD7606X_DMA_BASEADDR,
};

static struct spi_engine_init_param spi_eng_init_param  = {
	.ref_clk_hz = AD7606X_REF_CLK_HZ,
	.type = SPI_ENGINE,
	.spi_engine_baseaddr = AD7606X_CORE_BASEADDR,
	.cs_delay = 1,
	.data_width = 16,
};

static struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = AD7606X_RX_CLKGEN_BASEADDR,
	.parent_rate = AD7606X_REF_CLK_HZ,
};

static struct axi_pwm_init_param axi_pwm_init_params = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = AD7606X_REF_CLK_HZ,
};

static struct no_os_pwm_init_param trigger_pwm_init = {
	.period_ns = 1000,	/* 1MHz */
	.duty_cycle_ns = AD7606X_TRIGGER_PULSE_WIDTH_NS,  /* pulse_width = 120 */
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.extra = &axi_pwm_init_params,
	.platform_ops = &axi_pwm_ops,
};

static struct xil_gpio_init_param xil_gpio_param = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS,
};

static struct no_os_gpio_init_param ad7606x_gpio_reset = {
	.number = GPIO_ADC_RESET,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

static struct no_os_gpio_init_param ad7606x_gpio_stby = {
	.number = GPIO_ADC_STBY,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

static struct no_os_gpio_init_param ad7606x_gpio_range = {
	.number = GPIO_ADC_RANGE,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

static struct no_os_gpio_init_param ad7606x_gpio_os0 = {
	.number = GPIO_ADC_OS0,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

static struct no_os_gpio_init_param ad7606x_gpio_os1 = {
	.number = GPIO_ADC_OS1,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

static struct no_os_gpio_init_param ad7606x_gpio_os2 = {
	.number = GPIO_ADC_OS2,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

static struct no_os_gpio_init_param ad7606x_gpio_ser_par = {
	.number = GPIO_ADC_SER_PAR,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

static struct ad7606_axi_init_param ad7606x_axi_init_param = {
	.offload_init_param = &spi_engine_offload_init_param,
	.clkgen_init = &clkgen_init,
	.trigger_pwm_init = &trigger_pwm_init,
	.axi_clkgen_rate = AD7606X_REF_CLK_HZ,
	.reg_access_speed = 1000000,
	.core_baseaddr = AD7606X_CORE_BASEADDR,
	.rx_dma_baseaddr = AD7606X_DMA_BASEADDR,
	.dcache_invalidate_range = (void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
};

struct ad7606_init_param ad7606x_init_param_parallel = {
	.axi_init = &ad7606x_axi_init_param,
	.gpio_reset = &ad7606x_gpio_reset,
	.gpio_convst = NULL, // Driven by HDL
	.gpio_busy = NULL,   // Driver by HDL
	.gpio_stby_n = &ad7606x_gpio_stby,
	.gpio_range = &ad7606x_gpio_range,
	.gpio_os0 = &ad7606x_gpio_os0,
	.gpio_os1 = &ad7606x_gpio_os1,
	.gpio_os2 = &ad7606x_gpio_os2,
	.gpio_par_ser = &ad7606x_gpio_ser_par,
	.device_id = ID_AD7606C_18,
	.oversampling = {
	},
	.sw_mode = true,
	.parallel_interface = true,
	.config = {
		.dout_format = AD7606_8_DOUT,
	},
	.digital_diag_enable = { // CRC checks
	},
	.offset_ch = {
	},
	.phase_ch = {
	},
	.gain_ch = {
	},
	.range_ch = { // in HW mode there are only 2 ranges supported, none are differential
		{  -5000,  5000, false },
		{ -10000, 10000, false },
		{  -5000,  5000, false },
		{ -10000, 10000, false },
		{  -5000,  5000, false },
		{ -10000, 10000, false },
		{  -5000,  5000, false },
		{ -10000, 10000, false },
	}
};

struct ad7606_init_param ad7606x_init_param_serial = {
	.spi_init = {
		.chip_select = SPI_AD7606X_CS,
		.max_speed_hz = 50000000,
		.mode = NO_OS_SPI_MODE_2,
		.platform_ops = &spi_eng_platform_ops,
		.extra = &spi_eng_init_param,
	},
	.axi_init = &ad7606x_axi_init_param,
	.gpio_reset = &ad7606x_gpio_reset,
	.gpio_convst = NULL, // Driven by HDL
	.gpio_busy = NULL,   // Driver by HDL
	.gpio_stby_n = &ad7606x_gpio_stby,
	.gpio_range = &ad7606x_gpio_range,
	.gpio_os0 = &ad7606x_gpio_os0,
	.gpio_os1 = &ad7606x_gpio_os1,
	.gpio_os2 = &ad7606x_gpio_os2,
	.gpio_par_ser = &ad7606x_gpio_ser_par,
	.device_id = ID_AD7606C_18,
	.oversampling = {
	},
	.sw_mode = true,
	.config = {
		.dout_format = AD7606_8_DOUT,
	},
	.digital_diag_enable = { // CRC checks
	},
	.offset_ch = {
	},
	.phase_ch = {
	},
	.gain_ch = {
	},
	.range_ch = { // in HW mode there are only 2 ranges supported, none are differential
		{  -5000,  5000, false },
		{ -10000, 10000, false },
		{  -5000,  5000, false },
		{ -10000, 10000, false },
		{  -5000,  5000, false },
		{ -10000, 10000, false },
		{  -5000,  5000, false },
		{ -10000, 10000, false },
	}
};

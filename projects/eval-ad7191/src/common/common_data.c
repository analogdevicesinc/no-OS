/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-ad7191 examples.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "common_data.h"

struct no_os_uart_init_param ad7191_uart_ip = {
	.device_id = 0,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

static struct no_os_gpio_init_param ad7191_gpio_pga1_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_PGA1_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_pga2_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_PGA2_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_odr1_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_ODR1_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_odr2_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_ODR2_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_chan_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_CHAN_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_temp_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_TEMP_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_clksel_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_CLKSEL_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_pdown_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_PDOWN_PIN,
	.platform_ops = GPIO_OPS,
};

static struct no_os_gpio_init_param ad7191_gpio_rdy_ip = {
	.port = GPIO_CHIP,
	.number = GPIO_RDY_PIN,
	.platform_ops = GPIO_OPS,
};

struct ad7191_init_param ad7191_ip = {
	.spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED_HZ,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
	},
	.gpio_pdown = &ad7191_gpio_pdown_ip,
	.gpio_rdy = &ad7191_gpio_rdy_ip,
	.gpio_clksel = &ad7191_gpio_clksel_ip,
	.gpio_temp = &ad7191_gpio_temp_ip,
	.gpio_chan = &ad7191_gpio_chan_ip,
	.gpio_pga1 = &ad7191_gpio_pga1_ip,
	.gpio_pga2 = &ad7191_gpio_pga2_ip,
	.gpio_odr1 = &ad7191_gpio_odr1_ip,
	.gpio_odr2 = &ad7191_gpio_odr2_ip,
	.vref_mv = AD7191_VREF_MV,
	.clock_sel = AD7191_INT_CLK,
	.channel = AD7191_CH_AIN1_AIN2,
	.gain = AD7191_PGA_GAIN_1,
	.odr = AD7191_ODR_120,
};

/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by lt8722 examples.
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
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
#include "common_data.h"

struct no_os_uart_init_param lt8722_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param lt8722_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.extra = SPI_EXTRA,
	.max_speed_hz = SPI_BAUDRATE,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0
};

struct no_os_gpio_init_param lt8722_swen_ip = {
	.port = GPIO_SWEN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_SWEN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param lt8722_en_ip = {
	.port = GPIO_EN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_EN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct lt8722_init_param lt8722_ip = {
	.spi = &lt8722_spi_ip,
	.gpio_swen = &lt8722_swen_ip,
	.gpio_en = &lt8722_en_ip,
	.start_up_sequence = true
};

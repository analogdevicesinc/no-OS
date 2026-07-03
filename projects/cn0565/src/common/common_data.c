/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by cn0565 examples.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

volatile uint32_t ucInterrupted = 0; /* Flag to indicate interrupt occurred */

void ad5940_int_callback(void *ctx)
{
	ucInterrupted = 1;
}

uint32_t GetMCUIntFlag(void)
{
	return ucInterrupted;
}

uint32_t ClrMCUIntFlag(void)
{
	ucInterrupted = 0;
	return 1;
}

struct no_os_uart_init_param cn0565_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_i2c_init_param cn0565_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_BAUDRATE,
	.slave_address = 0x70,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct ad5940_init_param cn0565_ad5940_ip = {
	.spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = SPI_CS,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	},
	.reset_gpio_init = {
		.port = RESET_PORT,
		.number = RESET_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_RESET_EXTRA,
	},
	.gp0_gpio_init = {
		.port = GP0_PORT,
		.number = GP0_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_GP0_EXTRA,
	},
};

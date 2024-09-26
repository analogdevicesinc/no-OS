/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max14919 example.
 *   @author Radu Sabau (radu.sabau@analog.com)
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

struct no_os_uart_init_param max14919_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_gpio_init_param max14919_fault_ip = {
	.port = GPIO_FAULT_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_FAULT_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param max14919_in_ip[4] = {
	{
		.port = GPIO_IN_PORT_NUM,
		.pull = NO_OS_PULL_NONE,
		.number = GPIO_IN_PIN_NUM(0),
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	{
		.port = GPIO_IN_PORT_NUM,
		.pull = NO_OS_PULL_NONE,
		.number = GPIO_IN_PIN_NUM(1),
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	{
		.port = GPIO_IN_PORT_NUM,
		.pull = NO_OS_PULL_NONE,
		.number = GPIO_IN_PIN_NUM(2),
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	{
		.port = GPIO_IN_PORT_NUM,
		.pull = NO_OS_PULL_NONE,
		.number = GPIO_IN_PIN_NUM(3),
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
};

struct no_os_gpio_init_param max14919_in_rush_ip = {
	.port = GPIO_IN_RUSH_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_IN_RUSH_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param max14919_rev_ip = {
	.port = GPIO_REV_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_REV_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct max14919_init_param max14919_ip = {
	.in_param = {
		[0] = &max14919_in_ip[0],
		[1] = &max14919_in_ip[1],
		[2] = &max14919_in_ip[2],
		[3] = &max14919_in_ip[3]
	},
	.fault_param = &max14919_fault_ip,
	.inrush_param = &max14919_in_rush_ip,
	.rev_param = &max14919_rev_ip,
};

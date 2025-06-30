/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of the basic example for RTC.
 *   @author Francis Roi Manabat (francisroi.manabat@analog.com)
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
#include "maxim_gpio.h"
#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_irq.h"
#include "no_os_rtc.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "maxim_rtc.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct no_os_uart_init_param uart_ip = {
	.device_id = 2,
	.asynchronous_rx = false,
	.baud_rate = 115200,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = &max_uart_ops,
};

struct no_os_rtc_init_param rtc_ip = {
	.freq = 1,
	.load = 0,
	.platform_ops = &max_rtc_ops,
	.extra = NULL
};

struct no_os_gpio_init_param led_ip = {
	.port = 0,
	.number = 14,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra_ip,
};
struct no_os_gpio_init_param button_ip = {
	.port = 4,
	.number = 0,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra_ip,
};

struct max_gpio_init_param gpio_extra_ip = {
	.vssel = 1,
};

struct no_os_irq_init_param rtc_irq_ip = {
	.irq_ctrl_id = 19,
	.platform_ops = &max_irq_ops,
	.extra = NULL,
};

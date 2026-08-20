/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Common data implementation for LTC4284 project
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include "parameters.h"

/* CN2 FTDI USB-to-UART Bridge */
struct no_os_uart_init_param ltc4284_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.size = UART_SIZE,
	.parity = UART_PARITY,
	.stop = UART_STOP,
	.platform_ops = &max_uart_ops,
	.extra = &ltc4284_uart_extra_ip
};

struct no_os_i2c_init_param ltc4284_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_MAX_SPEED,
	.slave_address = LTC4284_I2C_ADDR_6,
	.platform_ops = &max_i2c_ops,
	.extra = &ltc4284_i2c_extra_ip
};

/* DC2470A board-specific values:
 *   RSENSE  = 5 mΩ ‖ (6 × 2 mΩ) = 312 µΩ  (datasheet p.49 worked example)
 *   Bus → RTNS pin  divider: RRT(390k) / RRB(10k) = 40:1
 *   Bus → DRAIN pin divider: RDT(390k) / RDB(10k) = 40:1
 *   V_ILIM at ship: 18 mV (CONFIG_1 ILIM code 0011, DC2470A altered default)
 * These are board-specific — a future cleanup should move the dividers out
 * of the driver's device struct into a project-level configuration layer.
 */
struct ltc4284_init_param ltc4284_ip = {
	.i2c_init      = &ltc4284_i2c_ip,
	.rsense_uohm   = 312,
	.vpwr_divider  = 40,
	.drain_divider = 40,
	.vsense_mv     = 18,
	.alert_gpio    = NULL,
};

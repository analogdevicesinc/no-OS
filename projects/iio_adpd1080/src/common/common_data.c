/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by iio_adpd1080 examples.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

struct no_os_uart_init_param adpd1080_uart_ip = {
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

struct adpd188_iio_init_param adpd1080_iio_ip = {
	.drv_init_param = {
		.device = ADPD1080,
		.phy_opt = ADPD188_I2C,
		.phy_init.i2c_phy = {
			.slave_address = I2C_SLAVE_ADDR,
			.max_speed_hz = I2C_BAUDRATE,
			.platform_ops = I2C_OPS,
			.extra = I2C_EXTRA,
		},
		.gpio0_init = {
			.number = 0,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.gpio1_init = {
			.number = 0,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
	},
};

/* Calibration resources used to trim the 32kHz sample clock. */
struct no_os_timer_init_param adpd1080_cal_timer_ip = {
	.id = TIMER_DEVICE_ID,
	.ticks_count = TIMER_TICKS,
	.freq_hz = TIMER_FREQ_HZ,
	.platform_ops = TIMER_OPS,
	.extra = TIMER_EXTRA,
};

struct no_os_gpio_init_param adpd1080_sync_gpio_ip = {
	.number = GPIO_SYNC_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_irq_init_param adpd1080_cal_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

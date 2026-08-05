/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by iio_adpd1080 examples.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

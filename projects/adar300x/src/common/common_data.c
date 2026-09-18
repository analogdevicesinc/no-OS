/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Common data definitions for ADAR300X project.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param adar300x_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

static struct no_os_gpio_init_param adar300x_gpio_rstb_ip = {
	.number = GPIO_RSTB,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

static struct no_os_gpio_init_param adar300x_gpio_mux_sel_ip = {
	.number = GPIO_MUX_SEL,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

static struct no_os_gpio_init_param adar300x_gpio_update_ip = {
	.number = GPIO_UPDATE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

static struct no_os_gpio_init_param adar300x_gpio_reset_ip = {
	.number = GPIO_BEAM_RESET,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

static struct no_os_gpio_init_param adar300x_gpio_mute_ip = {
	.number = GPIO_MUTE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct adar300x_init_param adar300x_ip = {
	.spi_init = {
		.device_id = SPI_DEVICE_ID,
		.chip_select = SPI_CS,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	},
	.spi_desc_shared = NULL,
	.gpio_rstb = &adar300x_gpio_rstb_ip,
	.gpio_mux_sel = &adar300x_gpio_mux_sel_ip,
	.gpio_update = &adar300x_gpio_update_ip,
	.gpio_reset = &adar300x_gpio_reset_ip,
	.gpio_mute = &adar300x_gpio_mute_ip,
	.type = ADAR3000,
	.dev_addr = 0,
};

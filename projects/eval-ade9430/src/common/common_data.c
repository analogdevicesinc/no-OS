/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by EVAL-ADE9430 example projects.
 *   @author RaduE (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

static struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEV_ID,
	.irq_id = UART_IRQ,
	.asynchronous_rx = false,
	.baud_rate = UART_BAUD,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_uart_ops,
	.extra = &uart_extra_ip,
};

static struct max_spi_init_param spi_extra_ip = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct no_os_spi_init_param ade9430_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = SPI_CS,
	.extra = &spi_extra_ip,
};

#ifdef HW_RESET_ENABLED
static struct max_gpio_init_param gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_gpio_init_param gpio_reset_ip = {
	.port = GPIO_RESET_PORT,
	.number = GPIO_RESET_NUMBER,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra_ip,
};
#endif

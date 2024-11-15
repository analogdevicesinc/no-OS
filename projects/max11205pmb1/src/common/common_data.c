/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max11205pmb1 examples.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param max11205_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param max11205_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_1,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

struct no_os_gpio_init_param max11205_gpio_rdy_ip = {
	.port = GPIO_SYNC_PORT_NUM,
	.number = GPIO_SYNC_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_irq_init_param max11205_gpio_irq_ip = {
	.platform_ops = GPIO_IRQ_OPS,
	.irq_ctrl_id = GPIO_CTRL_IRQ_ID,
	.extra = GPIO_IRQ_EXTRA,
};

struct max11205_init_param max11205_ip = {
	.gpio_rdy = &max11205_gpio_rdy_ip,
	.vref_mv = MAX11205_VREF_MV,
};

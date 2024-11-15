/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-ad7616 examples.
 *   @author Esteban Blanc (eblanc@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "main.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param ad7616_uart_ip = {
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

struct no_os_spi_init_param ad7616_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_2,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

//PJ0
struct no_os_gpio_init_param ad7616_gpio_busy_ip = {
	.port = GPIO_BUSY_PORT,
	.number = GPIO_BUSY_NB,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
};

//PJ3
struct no_os_gpio_init_param ad7616_gpio_reset_ip = {
	.port = GPIO_RESET_PORT,
	.number = GPIO_RESET_NB,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
};

//PJ5
struct no_os_gpio_init_param ad7616_gpio_convst_ip = {
	.port = GPIO_CONVST_PORT,
	.number = GPIO_CONVST_NB,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
};

struct ad7616_init_param ad7616_ip = {
	.gpio_reset_param = &ad7616_gpio_reset_ip,
	.gpio_convst_param = &ad7616_gpio_convst_ip,
	.gpio_busy_param = &ad7616_gpio_busy_ip,
	.mode = AD7616_SW,
	.osr = AD7616_OSR_0,
};

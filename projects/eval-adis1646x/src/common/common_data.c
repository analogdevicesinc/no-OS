/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adis1646x examples.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "common_data.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

struct no_os_uart_init_param adis1646x_uart_ip = {
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

struct no_os_spi_init_param adis1646x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

/* Initialization for Sync pin */
struct no_os_gpio_init_param adis1646x_gpio_reset_ip = {
	.port = GPIO_RESET_PORT_NUM,
	.number = GPIO_RESET_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct adis_init_param adis1646x_ip = {
	.info = &adis1646x_chip_info,
	.gpio_reset = &adis1646x_gpio_reset_ip,
	.sync_mode = ADIS_SYNC_OUTPUT,
	.dev_id = ADIS16465_1,
};

#ifdef IIO_TRIGGER_EXAMPLE
/* GPIO trigger */
struct no_os_irq_init_param adis1646x_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

const struct iio_hw_trig_cb_info gpio_cb_info = {
	.event = NO_OS_EVT_GPIO,
	.peripheral = NO_OS_GPIO_IRQ,
	.handle = ADIS1646X_GPIO_CB_HANDLE,
};

struct iio_hw_trig_init_param adis1646x_gpio_trig_ip = {
	.irq_id = ADIS1646X_GPIO_TRIG_IRQ_ID,
	.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
	.cb_info = gpio_cb_info,
	.name = ADIS1646X_GPIO_TRIG_NAME,
};
#endif

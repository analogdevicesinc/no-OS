/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adis16607 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_gpio.h"

struct no_os_uart_init_param adis16607_uart_ip = {
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

struct no_os_spi_init_param adis16607_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

/* Initialization for reset pin */
struct no_os_gpio_init_param adis16607_reset_ip = {
	.port = GPIO_RESET_PORT_NUM,
	.number = GPIO_RESET_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct adis_init_param adis16607_ip = {
	.info = &adis16607_chip_info,
	.spi_init = &adis16607_spi_ip,
	.i2c_init = NULL,
	.gpio_reset = &adis16607_reset_ip,
	.sync_mode = ADIS_SYNC_DEFAULT,
	.ext_clk = 0,
	.dev_id = ADIS16607_3,
	.duplex_type = ADIS_SPI_FULL_DUPLEX,
	.comm_type = ADIS_SPI_COMM,
	.use_fifo = true,
};

#ifdef IIO_TRIGGER_EXAMPLE
/* GPIO trigger */
struct no_os_irq_init_param adis16607_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

const struct iio_hw_trig_cb_info gpio_cb_info = {
	.event = NO_OS_EVT_GPIO,
	.peripheral = NO_OS_GPIO_IRQ,
	.handle = ADIS16607_GPIO_CB_HANDLE,
};

struct iio_hw_trig_init_param adis16607_gpio_trig_ip = {
	.irq_id = ADIS16607_GPIO_TRIG_IRQ_ID,
	.irq_trig_lvl = NO_OS_IRQ_EDGE_FALLING,
	.cb_info = gpio_cb_info,
	.name = ADIS16607_GPIO_TRIG_NAME,
};
#endif

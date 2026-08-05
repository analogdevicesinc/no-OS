/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Common data implementation for MAX30009 project
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "parameters.h"

struct no_os_uart_init_param max30009_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

const struct no_os_spi_init_param max30009_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

const struct no_os_i2c_init_param max30009_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.slave_address = MAX30009_I2C_ADDRESS,
	.max_speed_hz = I2C_SPEED,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct no_os_gpio_init_param max30009_gpio_ip = {
	.port = GPIO_INTB_PORT_NUM,
	.number = GPIO_INTB_PIN_NUM,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

const struct no_os_gpio_init_param max30009_gpio_csb_ip = {
	.port = GPIO_CSB_PORT_NUM,
	.number = GPIO_CSB_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = &max30009_gpio_csb_extra,
};

struct max30009_init_param max30009_init_params = {
	.use_i2c = false,
	.spi_init = max30009_spi_ip,
	.i2c_init = max30009_i2c_ip,
	.int_gpio_init = &max30009_gpio_ip,
	.csb_gpio_init = max30009_gpio_csb_ip,
};

struct no_os_irq_init_param max30009_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

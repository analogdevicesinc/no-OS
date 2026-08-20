/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data for the MAXM86161 project.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "parameters.h"

struct no_os_uart_init_param maxm86161_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_gpio_init_param maxm86161_gpio_intb_ip = {
	.port = GPIO_INTB_PORT_NUM,
	.number = GPIO_INTB_PIN_NUM,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

struct no_os_irq_init_param maxm86161_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

const struct no_os_i2c_init_param maxm86161_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 400000,
	.slave_address = MAXM86161_I2C_ADDR,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct maxm86161_init_param maxm86161_ip = {
	.i2c_init = maxm86161_i2c_ip,
	.gpio_intb_init = &maxm86161_gpio_intb_ip,
	.irq_init = &maxm86161_gpio_irq_ip,
	.irq_handle = MAXM86161_GPIO_CB_HANDLE,
};

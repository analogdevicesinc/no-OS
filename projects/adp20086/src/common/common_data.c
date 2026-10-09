/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Source file of common data for the adp20086 examples.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param adp20086_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_i2c_init_param adp20086_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_MAX_SPEED,
	.platform_ops = I2C_OPS,
	.slave_address = ADP20086_6P8KOHM_ADDR,
	.extra = I2C_EXTRA,
};

struct no_os_gpio_init_param adp20086_en_ip = {
	.port = EN_PORT,
	.number = EN_PIN,
	.pull = NO_OS_PULL_DOWN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

/* INTB is open drain and active low, so it idles high through the pull-up. */
struct no_os_gpio_init_param adp20086_intb_ip = {
	.port = INTB_PORT,
	.number = INTB_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_irq_init_param adp20086_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA
};

struct adp20086_init_param adp20086_ip = {
	.i2c_ip = &adp20086_i2c_ip,
	.en_gpio_ip = &adp20086_en_ip,			/* comment out if J3 != GPIO_EN (pos 1-3) */
	.intb_gpio_ip = &adp20086_intb_ip,		/* comment out if INTB is not wired */
	/* .irq_ctrl is filled in at runtime by the examples. */
};

/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to ADuCM3029 platform used by cn0565 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <stdint.h>
#include <stddef.h>
#include "platform_init.h"
#include "aducm3029_spi.h"
#include "aducm3029_gpio.h"
#include "aducm3029_uart.h"
#include "aducm3029_i2c.h"
#include "aducm3029_irq.h"
#include "aducm3029_gpio_irq.h"
#include "aducm3029_uart_stdio.h"

#define SPI_DEVICE_ID		0
#define SPI_CS			1
#define UART_DEVICE_ID		0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE		230400
#define I2C_DEVICE_ID		0
#define I2C_BAUDRATE		100000
#define RESET_PIN		13 // 0.13
#define RESET_PORT		3
#define GP0_PIN			15 // 0.15
#define GP0_PORT		6
#define INT_IRQn		GP0_PIN
#define UART_OPS		&aducm_uart_ops
#define IRQ_OPS			&aducm_irq_ops
#define GPIO_IRQ_OPS		&aducm_gpio_irq_ops
#define GPIO_OPS		&aducm_gpio_ops
#define SPI_OPS			&aducm_spi_ops
#define I2C_OPS			&aducm_i2c_ops

#define GPIO_IRQ_CTRL_ID	ADUCM_GPIO_B_GROUP_SOFT_CTRL

#define UART_EXTRA		NULL
#define SPI_EXTRA		&cn0565_spi_extra_ip
#define I2C_EXTRA		NULL
#define GPIO_RESET_EXTRA	NULL
#define GPIO_GP0_EXTRA		NULL
#define GPIO_IRQ_EXTRA		NULL

extern struct aducm_spi_init_param cn0565_spi_extra_ip;

#endif // __PARAMETERS_H__

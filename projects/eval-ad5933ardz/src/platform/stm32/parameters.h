/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-ad5933ardz
 *           project.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "stm32_i2c.h"
#include "stm32_irq.h"
#include "stm32_gpio.h"

#define I2C_DEVICE_ID		1
#define I2C_TIMING			0x00000E14 /* (Unused) */
#define I2C_MAX_SPEED       400000
#define I2C_OPS				&stm32_i2c_ops
#define I2C_EXTRA			&i2c_extra_ip

extern UART_HandleTypeDef huart5;

#define UART_DEVICE_ID		5
#define UART_BAUDRATE		115200
#define UART_EXTRA			&uart_extra_ip
#define UART_OPS			&stm32_uart_ops
#define UART_IRQ_ID			UART5_IRQn

extern struct stm32_uart_init_param uart_extra_ip;
extern struct stm32_i2c_init_param i2c_extra_ip;

#endif /* __PARAMETERS_H__ */

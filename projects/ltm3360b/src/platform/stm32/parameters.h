/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of STM32 platform data used by LTM3360B project.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio.h"
#include "stm32_i2c.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions ********************/
/******************************************************************************/

/* UART Configuration */
#define UART_DEVICE_ID      1
#define UART_IRQ_ID         UART5_IRQn
#define UART_BAUDRATE       115200
#define UART_EXTRA          &ltm3360b_uart_extra_ip
#define UART_OPS            &stm32_uart_ops

/* I2C Configuration */
#define I2C_DEVICE_ID       1
#define I2C_BAUDRATE        100000
#define I2C_OPS             &stm32_i2c_ops

/******************************************************************************/
/********************** Variables and User defined data types ****************/
/******************************************************************************/

extern struct stm32_uart_init_param ltm3360b_uart_extra_ip;

extern UART_HandleTypeDef huart5;


#endif /* __PARAMETERS_H__ */

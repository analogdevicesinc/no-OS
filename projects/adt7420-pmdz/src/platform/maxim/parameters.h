/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to MAXIM platform used by adt7420-pmdz
 *           project.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_i2c.h"
#include "maxim_irq.h"
#include "maxim_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#endif

#if (TARGET_NUM == 32650) || (TARGET_NUM == 78000)
#define I2C_DEVICE_ID    1
#elif (TARGET_NUM == 32655)
#define I2C_DEVICE_ID    2
#elif (TARGET_NUM == 32665) || (TARGET_NUM == 32660) || (TARGET_NUM == 32690)
#define I2C_DEVICE_ID    0
#endif

#if (TARGET_NUM == 32690)
#define UART_IRQ_ID     UART2_IRQn
#define UART_DEVICE_ID  2
#else
#define UART_IRQ_ID     UART0_IRQn
#define UART_DEVICE_ID  0
#endif
#define UART_BAUDRATE   57600
#define UART_OPS        &max_uart_ops

#define I2C_OPS         &max_i2c_ops

extern struct max_uart_init_param xuip;
extern struct max_i2c_init_param adt7420_i2c_extra;

#endif /* __PARAMETERS_H__ */

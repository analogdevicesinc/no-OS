/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to MAXIM platform used by eval-ltc4306-pmdz
 *           project.
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#if (TARGET_NUM == 32650) || (TARGET_NUM == 78000)
#define I2C_DEVICE_ID    1
#elif (TARGET_NUM == 32655)
#define I2C_DEVICE_ID    2
#elif (TARGET_NUM == 32665) || (TARGET_NUM == 32660) || (TARGET_NUM == 32690)
#define I2C_DEVICE_ID    0
#endif

#define I2C_OPS         &max_i2c_ops
#define I2C_EXTRA       &max_i2c_extra

#if (TARGET_NUM == 32690)
#define UART_IRQ_ID     UART2_IRQn
#define UART_DEVICE_ID  2
#elif (TARGET_NUM == 32650) || (TARGET_NUM == 32655)
#define UART_IRQ_ID     UART0_IRQn
#define UART_DEVICE_ID  0
#else
#define UART_IRQ_ID     UART1_IRQn
#define UART_DEVICE_ID  1
#endif

#define UART_BAUDRATE   57600
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &xuip

#define MAX_DUT         MAX5380M

extern struct max_uart_init_param xuip;
extern struct max_i2c_init_param max_i2c_extra;

#endif /* __PARAMETERS_H__ */

/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by adt75
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "maxim_i2c.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#define UART_IRQ_ID     UART0_IRQn
#endif

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adt75_uart_extra_ip
#define UART_OPS        &max_uart_ops

#define I2C_DEVICE_ID   1
#define I2C_OPS         &max_i2c_ops
#define I2C_EXTRA       &adt75_i2c_extra

extern struct max_uart_init_param adt75_uart_extra_ip;
extern struct max_i2c_init_param adt75_i2c_extra;

#endif /* __PARAMETERS_H__ */

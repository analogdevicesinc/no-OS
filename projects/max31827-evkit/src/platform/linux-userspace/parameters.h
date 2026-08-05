/*******************************************************************************
 *   @file   parameters.h
 *   @brief  Definitions of linux-userspace platform parameters for max31827-evkit.
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#ifdef CONFIG_FTD2XX
#include "ftd2xx_uart.h"
#include "ftd2xx_i2c.h"
#else
#include "linux_uart.h"
#include "linux_i2c.h"
#endif

#define UART_IRQ_ID	0
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	0
#define UART_EXTRA	NULL

#define I2C_DEVICE_ID	0

#ifdef CONFIG_FTD2XX
extern struct ftd2xx_i2c_init max31827_i2c_extra;
#define I2C_EXTRA	&max31827_i2c_extra
#define I2C_OPS		&ftd2xx_i2c_ops
#define UART_OPS	&ftd2xx_uart_ops
#else
#define I2C_EXTRA	NULL
#define I2C_OPS		&linux_i2c_ops
#define UART_OPS	&linux_uart_ops
#endif

#endif /* __PARAMETERS_H__ */

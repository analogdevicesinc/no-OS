/*******************************************************************************
 *   @file   parameters.h
 *   @brief  Definition of linux platform data used by max31827 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "ftd2xx_uart.h"
#include "ftd2xx_i2c.h"

#define UART_IRQ_ID	0
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	0
#define UART_EXTRA      NULL

#define I2C_DEVICE_ID   0

#define I2C_OPS         &ftd2xx_i2c_ops
#define UART_OPS        &ftd2xx_uart_ops
#define I2C_EXTRA       &max31827_i2c_extra

extern struct ftd2xx_i2c_init max31827_i2c_extra;

#endif /* __PARAMETERS_H__ */

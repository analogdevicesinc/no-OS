/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to aducm3029 platform used by eval-ltc4306-pmdz
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
#include "stdio.h"
#include "platform_init.h"
#include "aducm3029_i2c.h"
#include "aducm3029_irq.h"
#include "aducm3029_uart.h"
#include "aducm3029_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_IRQ_ID             ADUCM_UART_INT_ID

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_EXTRA      NULL
#define UART_OPS        &aducm_uart_ops

#define I2C_DEVICE_ID   0
#define I2C_OPS         &aducm_i2c_ops
#define I2C_EXTRA       NULL

#define MAX_DUT         MAX5380M

#endif /* __PARAMETERS_H__ */

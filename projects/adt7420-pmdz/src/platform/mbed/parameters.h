/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by adt7420-pmdz
 *           project.
 *   @author CMinajigi (chandrakant.minajigi@analog.com)
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
#include <PinNames.h>
#include "mbed_uart.h"
#include "mbed_i2c.h"
#include "no_os_uart.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_TX_PIN	    CONSOLE_TX
#define	UART_RX_PIN	    CONSOLE_RX
#define UART_DEVICE_ID  0
#define UART_IRQ_ID     0
#define UART_BAUDRATE   115200
#define UART_OPS        &mbed_uart_ops

/* I2C Pin mapping for Arduino interface */
#define I2C_SCL         ARDUINO_UNO_D15
#define I2C_SDA         ARDUINO_UNO_D14
#define I2C_DEVICE_ID   0
#define I2C_OPS         &mbed_i2c_ops

extern struct mbed_uart_init_param xuip;
extern struct mbed_i2c_init_param adt7420_i2c_extra;

#endif /* __PARAMETERS_H__ */

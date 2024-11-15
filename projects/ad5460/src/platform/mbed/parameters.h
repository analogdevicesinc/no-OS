/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by ad5460
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "mbed_spi.h"
#include "no_os_uart.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_TX_PIN	    CONSOLE_TX
#define	UART_RX_PIN	    CONSOLE_RX
#define UART_DEVICE_ID  0
#define UART_IRQ_ID     0
#define UART_BAUDRATE   115200
#define UART_EXTRA	&ad5460_uart_extra_ip
#define UART_OPS        &mbed_uart_ops

/* I2C Pin mapping for Arduino interface */
#define I2C_SCL         ARDUINO_UNO_D15
#define I2C_SDA         ARDUINO_UNO_D14
#define I2C_DEVICE_ID   0
#define I2C_OPS         &mbed_i2c_ops

#define SPI_BAUDRATE    1000000
#define SPI_OPS         &mbed_spi_ops
#define SPI_EXTRA       &ad5460_spi_extra
#define SPI_DEVICE_ID   0
#define SPI_CS          ARDUINO_UNO_D10

extern struct mbed_uart_init_param ad5460_uart_extra_ip;
extern struct mbed_i2c_init_param ad5460_i2c_extra;
extern struct mbed_spi_init_param ad5460_spi_extra;

#endif /* __PARAMETERS_H__ */

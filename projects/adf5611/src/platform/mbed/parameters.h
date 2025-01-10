/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by ADF5611 project.
 *   @author Josemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <PinNames.h>
#include "mbed_uart.h"
#include "mbed_spi.h"
#include "no_os_uart.h"
#include "mbed_i2c.h"
#include "no_os_i2c.h"

#define UART_TX_PIN	CONSOLE_TX
#define	UART_RX_PIN	CONSOLE_RX
#define UART_DEVICE_ID  5
#define UART_IRQ_ID     53
#define UART_BAUDRATE   230400
#define UART_EXTRA	&adf5611_uart_extra_ip
#define UART_OPS        &mbed_uart_ops

#define SPI_BAUDRATE    4000000
#define SPI_OPS         &mbed_spi_ops
#define SPI_EXTRA       &adf5611_spi_extra
#define SPI_DEVICE_ID   5
#define SPI_CS          ARDUINO_UNO_D10

#define I2C_OPS		&mbed_i2c_ops
#define I2C_EXTRA	&adf5611_i2c_extra_param
#define I2C_DEVICE_ID	0

extern struct mbed_uart_init_param adf5611_uart_extra_ip;
extern struct mbed_spi_init_param adf5611_spi_extra;
extern struct mbed_i2c_init_param adf5611_i2c_extra_param;

#endif /* __PARAMETERS_H__ */

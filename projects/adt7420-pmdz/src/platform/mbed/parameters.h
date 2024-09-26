/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by adt7420-pmdz
 *           project.
 *   @author CMinajigi (chandrakant.minajigi@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

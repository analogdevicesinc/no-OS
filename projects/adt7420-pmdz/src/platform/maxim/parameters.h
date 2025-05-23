/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to MAXIM platform used by adt7420-pmdz
 *           project.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_i2c.h"
#include "maxim_irq.h"
#include "maxim_gpio.h"

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

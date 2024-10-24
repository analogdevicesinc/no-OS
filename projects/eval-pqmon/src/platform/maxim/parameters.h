/*******************************************************************************
 *   @file   parameters.h
 *   @brief  Parameters header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_timer.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_usb_uart.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define INTC_DEVICE_ID 0
#define UART_DEVICE_ID 1
#define UART_BAUDRATE 115200
#define UART_IRQ_ID USB_IRQn
#define UART_OPS &max_usb_uart_ops
#define UART_EXTRA &iio_demo_usb_uart_extra_ip

#define SPI_CS 0
#define SPI_DEVICE_ID 2
#define SPI_BAUDRATE 15000000
#define SPI_OPS &max_spi_ops

#define I2C_EXTRA &vddioh_i2c_extra
#define SPI_PQM_DEVICE_ID 1
#define SPI_PQM_BAUDRATE 6000000
#define SPI_PQM_EXTRA &spi_extra_ip
#define UART_STDIO_EXT &uart_stdio_extra_ip

#define TIMER_DEVICE_ID 0
#define TIMER_FREQUENCY 1000
#define TIMER_TICK_PERIOD 16384
#define TIMER_OPS &max_timer_ops
#define TIMER_IRQ_ID TMR0_IRQn
#define TIMER_IRQ_OPS &max_irq_ops
#define TIMER_IRQ_EXTRA NULL
#define TIMER_CB_HANDLE MXC_TMR0
#define TIMER_TRIG_IRQ_ID TMR0_IRQn

#define RESET_GPIO_PIN_NUM 27
#define RESET_GPIO_PORT_NUM 1
#define GPIO_OPS &max_gpio_ops
#define RESET_GPIO_EXTRA &max_reset_gpio_extra_ip

#define INTR_GPIO_PIN_NUM 30
#define INTR_GPIO_PORT_NUM 0
#define INTR_GPIO_EXTRA &max_reset_gpio_extra_ip
#define INTR_GPIO_IRQ_ID 0
#define INTR_OPS &max_gpio_irq_ops

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

extern struct max_i2c_init_param vddioh_i2c_extra;
extern struct max_spi_init_param spi_extra_ip;
extern struct max_uart_init_param uart_stdio_extra_ip;
extern struct max_uart_init_param iio_demo_uart_extra_ip;
extern struct max_usb_uart_init_param iio_demo_usb_uart_extra_ip;
extern struct max_gpio_init_param max_reset_gpio_extra_ip;
extern struct max_gpio_init_param max_intr_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */

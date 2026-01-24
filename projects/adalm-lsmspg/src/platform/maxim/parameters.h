/***************************************************************************//**
 *   @file   parameters.h
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_uart.h"
#include "no_os_irq.h"
#include "no_os_timer.h"
#include "no_os_i2c.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_usb_uart.h"
#include "maxim_irq.h"
#include "maxim_timer.h"
#include "maxim_i2c.h"

#define INTC_DEVICE_ID 		0

#define AD5592R_SPI_DEVICE_ID 	2
#define AD5592R_SPI_SPEED 	1000000
#define AD5592R_SPI_CS 		0
#define AD5592R_SPI_MODE 	NO_OS_SPI_MODE_2 /* CPOL = 1, CPHA = 0 */
#define AD5592R_SPI_BIT_ORDER 	NO_OS_SPI_BIT_ORDER_MSB_FIRST
#define AD5592R_SPI_LANES 	NO_OS_SPI_SINGLE_LANE
#define AD5592R_SPI_OPS 	&max_spi_ops
#define AD5592R_SPI_EXTRA 	&ad5592r_spi_extra
extern struct max_spi_init_param ad5592r_spi_extra;

#define AD5592R_SPI_SS_PORT 	0
#define AD5592R_SPI_SS_NUMBER 	2
#define AD5592R_SPI_SS_PULL 	NO_OS_PULL_NONE
#define AD5592R_SPI_SS_OPS 	&max_gpio_ops
#define AD5592R_SPI_SS_EXTRA 	&ad5592r_spi_ss_extra
extern struct max_gpio_init_param ad5592r_spi_ss_extra;

#define UART_DEVICE_ID 		0
#define UART_IRQ_ID 		USB_IRQn
#define UART_ASYNC_RX 		true
#define UART_BAUD_RATE 		115200
#define UART_SIZE  		NO_OS_UART_CS_8
#define UART_PARITY 		NO_OS_UART_PAR_NO
#define UART_STOP_BITS 		NO_OS_UART_STOP_1_BIT
#define UART_PLATFORM_OPS 	&max_usb_uart_ops
#define UART_EXTRA 		&uart_extra
extern struct max_usb_uart_init_param uart_extra;

#define TIMER_IRQ_ID 		0
#define TIMER_IRQ_OPS 		&max_irq_ops
#define TIMER_IRQ_EXTRA 	NULL

#define TIMER_ID 		0
#define TIMER_FREQ_HZ 		1000000
#define TIMER_TICKS_COUNT 	16000
#define TIMER_OPS 		&max_timer_ops

#define LED_GPIO_PORT 		0
#define LED_GPIO_NUMBER 	21
#define LED_GPIO_PULL 		NO_OS_PULL_NONE
#define LED_GPIO_OPS 		&max_gpio_ops
#define LED_GPIO_EXTRA 		&ad5592r_led_extra
extern struct max_gpio_init_param ad5592r_led_extra;

#define AD5593R_I2C_DEVICE_ID 	0
#define AD5593R_I2C_SPEED_HZ 	100000  /* fast mode 100 Khz */
#define AD5593R_I2C_SLAVE_ADDR 	0b10000 /* 0x10 */
#define AD5593R_I2C_OPS 	&max_i2c_ops
#define AD5593R_I2C_EXTRA 	&ad5593r_i2c_extra
extern struct max_i2c_init_param ad5593r_i2c_extra;

#define LM75_I2C_DEVICE_ID 	0
#define LM75_I2C_SPEED_HZ 	100000
#define LM75_I2C_SLAVE_ADDR 	0b1001000 /* 0x48 */
#define LM75_I2C_OPS 		&max_i2c_ops
#define LM75_I2C_EXTRA 		&lm75_i2c_extra
extern struct max_i2c_init_param lm75_i2c_extra;

#endif /* __PARAMETERS_H__ */

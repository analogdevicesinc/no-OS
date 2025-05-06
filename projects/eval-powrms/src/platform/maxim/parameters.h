/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by ssd1306
 *           project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "maxim_irq.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_timer.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_timer.h"

// Used for debug purposes
#define UART_DEVICE_ID	        0
#define UART_BAUDRATE	        57600
#define UART_IRQ_ID    	        UART0_IRQn
#define UART_USB_IRQ_ID         USB_IRQn
#define UART_EXTRA_A            &demo_uart0_map_A_extra_ip
#define UART_EXTRA_B            &demo_uart0_map_B_extra_ip
#define UART_OPS                &max_uart_ops

#define I2C_DEVICE_ID           0
#define I2C_SPEED               400000
#define I2C_OPS                 &max_i2c_ops
#define I2C_EXTRA_ADC           &oled_display_i2c_maxim_extra_param
#define I2C_EXTRA_DISPLAY       &oled_display_i2c_maxim_extra_param

#define GPIO_ADC_RESET		    0
#define GPIO_OPS                &max_gpio_ops
#define GPIO_EXTRA              &adc_gpio_extra_param

#define M24512_I2C_DEVICE_ID    0
#define M24512_I2C_SPEED        400000
#define M24512_I2C_ADDR         0x50

#define M24512_WC_PORT          0
#define M24512_WC_NR            5

extern struct max_uart_init_param demo_uart0_map_A_extra_ip;
extern struct max_uart_init_param demo_uart0_map_B_extra_ip;
extern struct max_i2c_init_param oled_display_i2c_maxim_extra_param;
extern struct max_gpio_init_param adc_gpio_extra_param;

#endif /* __PARAMETERS_H__ */

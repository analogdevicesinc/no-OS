/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by EVAL-POWRMS
 *           project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

/* AD7091R5 RESETB shares the EN_ADC_VDRIVE net (P0.20): the driver pulses this
 * pin during ad7091r5_init() to hardware-reset the part. Must match
 * en_adc_vdrive_ip in common_data.c. */
#define GPIO_ADC_RESET		    2
#define GPIO_OPS                &max_gpio_ops
#define GPIO_EXTRA              &adc_gpio_extra_param

#define M24512_I2C_DEVICE_ID    0
#define M24512_I2C_ADDR         0x50

#define M24512_WC_PORT          0
#define M24512_WC_NR            5

extern struct max_uart_init_param demo_uart0_map_A_extra_ip;
extern struct max_uart_init_param demo_uart0_map_B_extra_ip;
extern struct max_i2c_init_param oled_display_i2c_maxim_extra_param;
extern struct max_gpio_init_param adc_gpio_extra_param;

#endif /* __PARAMETERS_H__ */

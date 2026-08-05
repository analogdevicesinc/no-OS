/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by ssd1306
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
#include "maxim_uart_stdio.h"
#include "maxim_timer.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_timer.h"

// Used for debug purposes
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	115200
#define UART_IRQ_ID    	UART0_IRQn
#define UART_EXTRA      &demo_uart_extra_ip
#define UART_OPS        &max_uart_ops

#define I2C_EXTRA      &oled_display_i2c_maxim_extra_param
#define I2C_OPS        &max_i2c_ops

extern struct max_uart_init_param demo_uart_extra_ip;
extern struct max_i2c_init_param oled_display_i2c_maxim_extra_param;

#endif /* __PARAMETERS_H__ */

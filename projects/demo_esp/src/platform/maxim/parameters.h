/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to maxim platform used by demo_esp
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "stdio.h"
#include "maxim_timer.h"
#include "maxim_irq.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_CONFIG_BAUDRATE	115200
#define UART_CONFIG_IRQ_ID	UART2_IRQn
#define UART_DEVICE_ID		2
#define UART_EXTRA		&uart_extra_ip

#define GPIO_PORT		3
#define GPIO_NR			9
#define GPIO_EXTRA		&gpio_extra_ip

#define TIMER_ID		1
#define TIMER_FREQ		32000

#define IRQ_OPS			&max_irq_ops
#define UART_OPS		&max_uart_ops
#define TIMER_OPS		&max_timer_ops
#define GPIO_OPS		&max_gpio_ops
#define TIMER_EXTRA		NULL

#define WIFI_SW_RESET		false

extern struct max_uart_init_param uart_extra_ip;
extern struct max_gpio_init_param gpio_extra_ip;

#endif /* __PARAMETERS_H__ */

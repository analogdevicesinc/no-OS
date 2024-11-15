/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to aducm3029 platform used by demo_esp
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
#include "platform_init.h"
#include "aducm3029_timer.h"
#include "aducm3029_irq.h"
#include "aducm3029_uart.h"
#include "aducm3029_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_CONFIG_BAUDRATE	115200
#define UART_CONFIG_IRQ_ID	ADUCM_UART_INT_ID
#define UART_DEVICE_ID		0
#define UART_EXTRA		NULL

#define GPIO_PORT		NULL
#define GPIO_NR			NULL
#define GPIO_EXTRA		NULL

#define TIMER_ID		1
#define TIMER_FREQ		1000

#define IRQ_OPS			&aducm_irq_ops
#define UART_OPS		&aducm_uart_ops
#define TIMER_OPS		&aducm_timer_ops
#define GPIO_OPS		&aducm_gpio_ops
#define TIMER_EXTRA		&timer_extra_ip

#define WIFI_SW_RESET		true

extern struct aducm_timer_init_param timer_extra_ip;

#endif /* __PARAMETERS_H__ */

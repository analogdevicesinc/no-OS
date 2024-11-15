/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by iio_demo
 *           project.
 *   @author Robert Budai(robert.budai@analog.com)
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
#include "maxim_irq.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_timer.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_timer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX_SIZE_BASE_ADDR	(SAMPLES_PER_CHANNEL * DEMO_CHANNELS * \
					sizeof(uint16_t))

#define SAMPLES_PER_CHANNEL_PLATFORM 2000

#define INTC_DEVICE_ID	0
#define UART_IRQ_ID    	UART0_IRQn
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600
#define UART_EXTRA      &iio_demo_uart_extra_ip
#define UART_OPS        &max_uart_ops
#define GPIO_EXTRA      &gpio_extra_ip
#define GPIO_OPS        &max_gpio_ops
#define GPIO_PORT	2

extern struct max_uart_init_param iio_demo_uart_extra_ip;
extern struct max_gpio_init_param gpio_extra_ip;

#endif /* __PARAMETERS_H__ */

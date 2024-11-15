/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by MAX25603 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_pwm.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"


#if (TARGET_NUM == 32690)
#define	UART_IRQ_ID		UART0_IRQn
#define UART_DEVICE_ID		0
#define UART_BAUDRATE		57600
#endif

#define UART_EXTRA		&max25603_uart_extra
#define UART_OPS		&max_uart_ops

#if (TARGET_NUM == 32690)
#define PWM_EN1_ID		3
#define PWM_EN1_PERIOD_NS	200000000
#define PWM_EN1_DUTY_NS		100000000

#define PWM_EN2_ID		2
#define PWM_EN2_PERIOD_NS	200000000
#define PWM_EN2_DUTY_NS		100000000

#define PWM_LB_ID		1
#define PWM_LB_PERIOD_NS	200000000
#define PWM_LB_DUTY_NS		100000000

#define PWM_HB_ID		0
#define PWM_HB_PERIOD_NS	200000000
#define PWM_HB_DUTY_NS		100000000
#endif

#define PWM_EXTRA		&max25603_pwm_extra
#define PWM_OPS			&max_pwm_ops

#if (TARGET_NUM == 32690)
#define GPIO_SHUNT_PORT_NUM	1
#define GPIO_SHUNT_PIN_NUM	6

#define GPIO_FLT_PORT_NUM	2
#define GPIO_FLT_PIN_NUM	21
#endif

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&max25603_gpio_extra

extern struct max_uart_init_param max25603_uart_extra;
extern struct max_pwm_init_param max25603_pwm_extra;
extern struct max_gpio_init_param max25603_gpio_extra;

#endif /* __PARAMETERS_H__ */

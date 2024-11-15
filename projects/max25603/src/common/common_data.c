/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Common data used by the MAX25603 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"


struct no_os_uart_init_param max25603_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_gpio_init_param max25603_shunt_ip = {
	.port = GPIO_SHUNT_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_SHUNT_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_pwm_init_param max25603_en1_ip = {
	.id = PWM_EN1_ID,
	.period_ns = PWM_EN1_PERIOD_NS,
	.duty_cycle_ns = PWM_EN1_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};

struct no_os_pwm_init_param max25603_en2_ip = {
	.id = PWM_EN2_ID,
	.period_ns = PWM_EN2_PERIOD_NS,
	.duty_cycle_ns = PWM_EN2_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};

struct no_os_pwm_init_param max25603_hb_ip = {
	.id = PWM_HB_ID,
	.period_ns = PWM_HB_PERIOD_NS,
	.duty_cycle_ns = PWM_HB_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};

struct no_os_pwm_init_param max25603_lb_ip = {
	.id = PWM_LB_ID,
	.period_ns = PWM_LB_PERIOD_NS,
	.duty_cycle_ns = PWM_LB_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};

struct max25603_init_param max25603_ip = {
	.en1_param = &max25603_en1_ip,
	.en2_param = &max25603_en2_ip,
	.flt_param = NULL,
	.hb_param = &max25603_hb_ip,
	.lb_param = &max25603_lb_ip,
	.shunt_param = &max25603_shunt_ip,
};

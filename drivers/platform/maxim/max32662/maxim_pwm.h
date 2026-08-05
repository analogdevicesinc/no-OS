/***************************************************************************//**
 *   @file   maxim_pwm.h
 *   @brief  Maxim platform specific implementation PWM API.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef MAXIM_PWM_H_
#define MAXIM_PWM_H_

#include "tmr.h"
#include "gpio.h"
#include "no_os_pwm.h"
#include "no_os_util.h"

/**
 * Possible clock sources:
 *			- MXC_TMR_APB_CLK
 *			- MXC_TMR_ERTCO_CLK
*			- MXC_TMR_INRO_CLK
 */
#define NO_OS_MXC_PWM_CLOCK_SOURCE		MXC_TMR_APB_CLK

#define MAX_PWM_TMR_MAX_VAL		NO_OS_GENMASK(15, 0)
#define MAX_PWM_PRESCALER_VAL(n)	((n - 1) * 16)
#define MAX_PWM_PRESCALER_TRUE(n)	NO_OS_BIT((n) / 16)
#define MAX_PWM_GET_PRESCALER(n)	NO_OS_BIT((n) - 1)

struct max_pwm_init_param {
	mxc_gpio_vssel_t vssel;
};

struct max_pwm_extra {
	mxc_gpio_vssel_t vssel;
	mxc_tmr_cfg_t tmr_cfg;
};

/**
 * @brief Maxim specific PWM ops
 */
extern const struct no_os_pwm_platform_ops max_pwm_ops;

#endif // MAXIM_PWM_H_

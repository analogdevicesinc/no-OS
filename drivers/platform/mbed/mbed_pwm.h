/***************************************************************************//**
 *   @file   mbed_pwm.h
 *   @brief  Header containing extra types required for Mbed PWM interface
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MBED_PWM_H
#define MBED_PWM_H

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/

struct mbed_pwm_init_param {
	uint16_t pwm_pin;	// PWM pin number
};

struct mbed_pwm_desc {
	void *pwm_obj;		// Mbed PWM instance/object
};

/**
 * @brief Mbed specific PWM platform ops structure
 */
extern const struct no_os_pwm_platform_ops mbed_pwm_ops;

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /* MBED_PWM_H */

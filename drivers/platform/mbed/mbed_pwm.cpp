/***************************************************************************//**
 *   @file   mbed_pwm.cpp
 *   @brief  Implementation of PWM Mbed platform driver interfaces
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <mbed.h>

// Platform drivers needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif //  _cplusplus

#include "no_os_error.h"
#include "no_os_pwm.h"
#include "no_os_gpio.h"
#include "mbed_pwm.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief	Initialize the PWM interface.
 * @param	desc[in, out] - The PWM descriptor.
 * @param	param[in] - The structure that contains the PWM init parameters.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_init(struct no_os_pwm_desc **desc,
		      const struct no_os_pwm_init_param *param)
{
	struct no_os_pwm_desc *pwm_desc;
	mbed::PwmOut *pwm;
	struct mbed_pwm_desc *mbed_pwm_desc;

	if (!desc || !param)
		return -EINVAL;

	/* Allocate memory for general PWM descriptor */
	pwm_desc = (struct no_os_pwm_desc *)calloc(1, sizeof(*pwm_desc));
	if (!pwm_desc)
		return -ENOMEM;

	pwm_desc->id = param->id; 	// PWM Id
	pwm_desc->period_ns = param->period_ns; 	// PWM period

	/* Create and initialize Mbed PWM object */
	pwm = new PwmOut((PinName)((struct mbed_pwm_init_param *)(
					   param->extra))->pwm_pin);
	if (!pwm)
		goto err_pwm;

	/* Allocate memory for Mbed specific PWM descriptor for future use */
	mbed_pwm_desc = (struct mbed_pwm_desc *)calloc(1, sizeof(*mbed_pwm_desc));
	if (!mbed_pwm_desc)
		goto err_new_mbed_pwm_desc;

	mbed_pwm_desc->pwm_obj = (mbed::PwmOut *)pwm;
	pwm_desc->extra = mbed_pwm_desc;

	/* Configure Mbed PWM parameters */
	pwm->period_us(param->period_ns / 1000); 			// Period in usec
	pwm->pulsewidth_us(param->duty_cycle_ns / 1000);	// Duty cycle in usec
	pwm->suspend();

	*desc = pwm_desc;

	return 0;

err_new_mbed_pwm_desc:
	free(pwm);
err_pwm:
	free(pwm_desc);

	return -ENOMEM;
}

/**
 * @brief	Enable the PWM interface.
 * @param	desc[in] - Pointer where the configured instance is stored.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_enable(struct no_os_pwm_desc *desc)
{
	mbed::PwmOut *pwm;

	if (!desc || !desc->extra)
		return -EINVAL;

	pwm = (mbed::PwmOut *)(((struct mbed_pwm_desc *)desc->extra)->pwm_obj);
	if (!pwm)
		return -ENOMEM;

	pwm->resume();

	return 0;
}

/**
 * @brief	Disable the PWM interface.
 * @param	desc[in] - Pointer where the configured instance is stored.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_disable(struct no_os_pwm_desc *desc)
{
	mbed::PwmOut *pwm;

	if (!desc || !desc->extra)
		return -EINVAL;

	pwm = (mbed::PwmOut *)(((struct mbed_pwm_desc *)desc->extra)->pwm_obj);
	if (!pwm)
		return -ENOMEM;

	pwm->suspend();

	return 0;
}

/**
 * @brief	Set the PWM period.
 * @param	desc[in] - Pointer where the configured instance is stored.
 * @param	period_ns[in] - PWM period.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_set_period(struct no_os_pwm_desc *desc,
			    uint32_t period_ns)
{
	mbed::PwmOut *pwm;

	if (!desc || !desc->extra)
		return -EINVAL;

	pwm = (mbed::PwmOut *)(((struct mbed_pwm_desc *)desc->extra)->pwm_obj);
	if (!pwm)
		return -ENOMEM;

	pwm->period_us(period_ns / 1000);

	return 0;
}

/**
 * @brief	Get the PWM period.
 * @param	desc[in] - Pointer where the configured instance is stored.
 * @param	period_ns[in] - PWM period.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_get_period(struct no_os_pwm_desc *desc,
			    uint32_t *period_ns)
{
	mbed::PwmOut *pwm;

	if (!desc || !desc->extra)
		return -EINVAL;

	pwm = (mbed::PwmOut *)(((struct mbed_pwm_desc *)desc->extra)->pwm_obj);
	if (!pwm)
		return -ENOMEM;

	*period_ns = pwm->read_period_us() * 1000;

	return 0;
}

/**
 * @brief	Set the PWM duty cycle.
 * @param	desc[in] - Pointer where the configured instance is stored.
 * @param	duty_cycle_ns[in] - PWM duty cycle.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_set_duty_cycle(struct no_os_pwm_desc *desc,
				uint32_t duty_cycle_ns)
{
	mbed::PwmOut *pwm;

	if (!desc || !desc->extra)
		return -EINVAL;

	pwm = (mbed::PwmOut *)(((struct mbed_pwm_desc *)desc->extra)->pwm_obj);
	if (!pwm)
		return -ENOMEM;

	pwm->pulsewidth_us(duty_cycle_ns / 1000);

	return 0;
}

/**
 * @brief	Get the PWM duty cycle.
 * @param	desc[in] - Pointer where the configured instance is stored.
 * @param	duty_cycle_ns[in] - PWM duty cycle.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_get_duty_cycle(struct no_os_pwm_desc *desc,
				uint32_t *duty_cycle_ns)
{
	mbed::PwmOut *pwm;

	if (!desc || !desc->extra)
		return -EINVAL;

	pwm = (mbed::PwmOut *)(((struct mbed_pwm_desc *)desc->extra)->pwm_obj);
	if (!pwm)
		return -ENOMEM;

	*duty_cycle_ns = pwm->read_pulsewitdth_us() * 1000;

	return 0;
}

/**
 * @brief	Remove the memory allocated for PWM device descriptors
 * @param	desc[in, out] - Pointer where the configured instance is stored
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_pwm_remove(struct no_os_pwm_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	if (((struct mbed_pwm_desc *)desc->extra)->pwm_obj)
		delete((mbed::PwmOut *)((struct mbed_pwm_desc *)desc->extra)->pwm_obj);

	free(desc->extra);
	free(desc);

	return 0;
}

/**
* @brief Mbed platform specific PWM platform ops structure
*/
const struct no_os_pwm_platform_ops mbed_pwm_ops = {
	.pwm_ops_init = &mbed_pwm_init,
	.pwm_ops_enable = &mbed_pwm_enable,
	.pwm_ops_disable = &mbed_pwm_disable,
	.pwm_ops_set_period = &mbed_pwm_set_period,
	.pwm_ops_get_period = &mbed_pwm_get_period,
	.pwm_ops_set_duty_cycle = &mbed_pwm_set_duty_cycle,
	.pwm_ops_get_duty_cycle = &mbed_pwm_get_duty_cycle,
	.pwm_ops_remove = &mbed_pwm_remove
};

#ifdef __cplusplus  // Closing extern c
}
#endif //  _cplusplus

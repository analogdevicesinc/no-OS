/***************************************************************************//**
 *   @file   no_os_pwm.c
 *   @brief  Implementation of the PWM Interface
 *   @author Pratyush Mallick (pratyush.mallick@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <inttypes.h>
#include "no_os_pwm.h"
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_mutex.h"

/**
 * @brief - PWM mutex
*/
static void *pwm_mutex_table[PWM_MAX_NUMBER + 1];

/**
 * @brief Initialize the PWM peripheral.
 * @param desc - The PWM descriptor.
 * @param param - The structure that contains the PWM parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_init(struct no_os_pwm_desc **desc,
		       const struct no_os_pwm_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->pwm_ops_init)
		return -ENOSYS;

	ret = param->platform_ops->pwm_ops_init(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	no_os_mutex_init(&pwm_mutex_table[param->id]);
	(*desc)->mutex = pwm_mutex_table[param->id];

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_pwm_init().
 * @param desc - The PWM descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_remove(struct no_os_pwm_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_remove)
		return -ENOSYS;

	no_os_mutex_remove(desc->mutex);
	pwm_mutex_table[desc->id] = NULL;

	return desc->platform_ops->pwm_ops_remove(desc);
}

/**
 * @brief Enable PWM signal generation.
 * @param desc - The PWM descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_enable(struct no_os_pwm_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_enable)
		return -ENOSYS;

	no_os_mutex_lock(desc->mutex);
	return desc->platform_ops->pwm_ops_enable(desc);
}

/**
 * @brief Disable PWM signal generation.
 * @param desc - The PWM descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_disable(struct no_os_pwm_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_disable)
		return -ENOSYS;

	no_os_mutex_unlock(desc->mutex);
	return desc->platform_ops->pwm_ops_disable(desc);
}

/**
 * @brief Set the PWM period value.
 * @param desc - The PWM descriptor.
 * @param period_ns - The period value in nanoseconds.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_set_period(struct no_os_pwm_desc *desc,
			     uint32_t period_ns)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_set_period)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_set_period(desc, period_ns);
}

/**
 * @brief Get the PWM period value.
 * @param desc - The PWM descriptor.
 * @param period_ns - The period value in nanoseconds.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_get_period(struct no_os_pwm_desc *desc,
			     uint32_t *period_ns)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_get_period)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_get_period(desc, period_ns);
}

/**
 * @brief Set the PWM duty cycle.
 * @param desc - The PWM descriptor.
 * @param duty_cycle_ns - Duty cycle in nanoseconds.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_set_duty_cycle(struct no_os_pwm_desc *desc,
				 uint32_t duty_cycle_ns)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_set_duty_cycle)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_set_duty_cycle(desc, duty_cycle_ns);
}

/**
 * @brief Get the PWM duty cycle.
 * @param desc - The PWM descriptor.
 * @param duty_cycle_ns - Duty cycle in nanoseconds.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_get_duty_cycle(struct no_os_pwm_desc *desc,
				 uint32_t *duty_cycle_ns)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_get_duty_cycle)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_get_duty_cycle(desc, duty_cycle_ns);
}

/**
 * @brief Set the PWM phase value.
 * @param desc - The PWM descriptor.
 * @param phase_ns - Phase value in nanoseconds.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_set_phase(struct no_os_pwm_desc *desc,
			    uint32_t phase_ns)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_set_phase)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_set_phase(desc, phase_ns);
}

/**
 * @brief Get the PWM phase value.
 * @param desc - The PWM descriptor.
 * @param phase_ns - Phase value in nanoseconds.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_get_phase(struct no_os_pwm_desc *desc,
			    uint32_t *phase_ns)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_get_phase)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_get_phase(desc, phase_ns);
}

/**
 * @brief Set the PWM polarity.
 * @param desc - The PWM descriptor.
 * @param polarity - Polarity value.
 *                   Example: NO_OS_PWM_POLARITY_HIGH
 *                            NO_OS_PWM_POLARITY_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_set_polarity(struct no_os_pwm_desc *desc,
			       enum no_os_pwm_polarity polarity)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_set_polarity)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_set_polarity(desc, polarity);
}

/**
 * @brief Get the PWM polarity.
 * @param desc - The PWM descriptor.
 * @param polarity - Polarity value.
 *                   Example: NO_OS_PWM_POLARITY_HIGH
 *                            NO_OS_PWM_POLARITY_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_pwm_get_polarity(struct no_os_pwm_desc *desc,
			       enum no_os_pwm_polarity *polarity)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->pwm_ops_get_polarity)
		return -ENOSYS;

	return desc->platform_ops->pwm_ops_get_polarity(desc, polarity);
}

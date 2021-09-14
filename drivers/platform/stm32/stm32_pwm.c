/***************************************************************************//**
 *   @file   stm32/stm32_pwm.c
 *   @brief  Implementation of stm32 pwm driver.
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include "util.h"
#include "gpio.h"
#include "stm32_gpio.h"
#include "pwm.h"
#include "stm32_pwm.h"

/******************************************************************************/
/********************** Macros and Constants Definition ***********************/
/******************************************************************************/

/* For 16-bit pwm timer */
#define	PWM_DEFAULT_PERIOD	0xffff

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the PWM Timer
 * @param desc - STM32 PWM descriptor
 * @param param - The structure that contains the PWM init parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 * @note PWM Timer period is dependent upon the timer clock and prescaler value. Prescaler
 * 		 value is used as 0 for timer frequency generation in below function
 */
static int32_t _init_pwm_timer(struct stm32_pwm_desc *desc,
			       const struct pwm_init_param *param)
{
	uint32_t period;
	uint32_t timer_frequency_hz;
	float pwm_frequency;
	struct stm32_pwm_init_param *sparam = param->extra;

	/* Formula for timer period count calculation:
	 * timer frequency = timer clock / (prescaler + 1)
	 * pwm frequency = 1 / pwm period (sec)
	 * timer period count = (timer frequency / pwm frequency) - 1
	 * Example:
	 * Timer clock = 216Mhz, pwm period = 1000ns, prescaler = 1
	 * timer frequency = 216 / (0+1) = 216Mhz
	 * pwm frequency = 1 / 1000ns = 1Mhz
	 * timer period count = (216 / 1) - 1 = 215
	 **/
	if (sparam->get_timer_clock) {
		/* Timer period is determined based on the timer peripheral frequency */
		timer_frequency_hz = sparam->get_timer_clock();	// prescaler = 0
		pwm_frequency = (1.0 / param->period_ns);
		period = (uint32_t)(timer_frequency_hz / (pwm_frequency * 1000000000)) - 1;

		if (period >= PWM_DEFAULT_PERIOD)
			period = PWM_DEFAULT_PERIOD - 1;
	} else {
		period = PWM_DEFAULT_PERIOD - 1;
	}

	desc->htimer.Instance = sparam->base;
	desc->htimer.Init.Prescaler = 0;
	desc->htimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	desc->htimer.Init.Period = period;
	desc->htimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	desc->htimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	desc->htimer.Init.RepetitionCounter = 0;
	desc->htimer.State = HAL_TIM_STATE_RESET;
	if (HAL_TIM_Base_Init(&desc->htimer) != HAL_OK)
		return -EIO;

	if (HAL_TIM_PWM_Init(&desc->htimer) != HAL_OK)
		return -EIO;

	/* Store the timer period for calculating PWM duty cycle */
	desc->timer_period = period;
	desc->timer_chn = ((struct stm32_pwm_init_param *)param->extra)->timer_chn;

	return SUCCESS;
}

/**
 * @brief Initialize the PWM
 * @param desc - STM32 PWM descriptor
 * @param param - The structure that contains the PWM init parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
static int32_t _init_pwm(struct stm32_pwm_desc *desc,
			 const struct pwm_init_param *param)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	uint32_t pwm_pulse_width;
	float duty_cycle_percentage;
	uint32_t period = desc->timer_period;
	uint32_t timer_chn = ((struct stm32_pwm_init_param *)(param->extra))->timer_chn;

	/* Calculate the percentage duty cycle */
	duty_cycle_percentage = ((float)param->duty_cycle_ns / param->period_ns) * 100;
	pwm_pulse_width = (uint32_t)((period + 1) * duty_cycle_percentage) / (100 - 1);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pwm_pulse_width;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&desc->htimer, &sConfigOC, timer_chn) != HAL_OK)
		return -EIO;

	return SUCCESS;
}

/**
 * @brief Initialize the PWM GPIO
 * @param desc - STM32 PWM descriptor
 * @param param - The structure that contains the PWM init parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
static int32_t _init_pwm_gpio(struct stm32_pwm_desc *desc,
			      const struct pwm_init_param *param)
{
	int32_t ret;
	struct stm32_pwm_init_param *sparam = param->extra;
	struct gpio_init_param *pwm_gpio_params = sparam->gpio;

	ret = gpio_get(&desc->gpio, pwm_gpio_params);
	if (ret)
		return ret;

	return SUCCESS;
}

/**
 * @brief Initialize the PWM
 * @param desc - PWM descriptor
 * @param param - The structure that contains the PWM init parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_pwm_init(struct pwm_desc **desc,
		       const struct pwm_init_param *param)
{
	struct pwm_desc *descriptor;
	struct stm32_pwm_desc *extra;
	int32_t ret;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct pwm_desc *)calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	extra = (struct stm32_pwm_desc *)calloc(1, sizeof(*extra));
	if (!extra) {
		ret = -ENOMEM;
		goto error_desc;
	}

	/* Initialize the PWM timer */
	ret = _init_pwm_timer(extra, param);
	if (ret)
		goto error_extra;

	/* Initialize the PWM */
	ret = _init_pwm(extra, param);
	if (ret)
		goto error_extra;

	/* Initialize the GPIO attached to timer channel */
	ret = _init_pwm_gpio(extra, param);
	if (ret)
		goto error_extra;

	descriptor->extra = extra;
	*desc = descriptor;

	return SUCCESS;
error_extra:
	free(extra);
error_desc:
	free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by stm32_pwm_init()
 * @param desc - PWM descriptor
 * @return SUCCESS in case of success, FAILURE otherwise
 */
int32_t stm32_pwm_remove(struct pwm_desc *desc)
{
	struct stm32_pwm_desc *extra;
	int32_t ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	extra = desc->extra;

	if (HAL_TIM_Base_DeInit(&extra->htimer) != HAL_OK)
		return -EIO;

	if (HAL_TIM_PWM_DeInit(&extra->htimer) != HAL_OK)
		return -EIO;

	ret = gpio_remove(extra->gpio);
	if (ret)
		return ret;

	free(desc->extra);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Enable the PWM
 * @param desc - PWM descriptor
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_pwm_enable(struct pwm_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (HAL_TIM_PWM_Start(&((struct stm32_pwm_desc *)desc->extra)->htimer,
			      ((struct stm32_pwm_desc *)desc->extra)->timer_chn) != HAL_OK)
		return -EIO;

	return SUCCESS;
}

/**
 * @brief Disable the PWM
 * @param desc - PWM descriptor
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_pwm_disable(struct pwm_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (HAL_TIM_PWM_Stop(&((struct stm32_pwm_desc *)desc->extra)->htimer,
			     ((struct stm32_pwm_desc *)desc->extra)->timer_chn) != HAL_OK)
		return -EIO;

	return SUCCESS;
}

/***************************************************************************//**
 *   @file   stm32/stm32_pwm.h
 *   @brief  Implementation of stm32 pwm driver.
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "stm32_gpio.h"
#include "stm32_pwm.h"
#include "stm32_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definition ***********************/
/******************************************************************************/

/* For 16-bit pwm timer */
#define	PWM_DEFAULT_PERIOD	             0xffff
#define NO_OS_CHN_TO_STM32_CHN(x)        (0x4*(x-1))
#define FREQUENCY_HZ_TO_TIME_NS_FACTOR   1000000000.0

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief Calculate the period in ticks.
 * @param desc - STM32 PWM descriptor.
 * @param timer_clk_hz - timer clock frequency.
 * @param period_ns - timer period.
 * @return return period value in ticks.
 */
static uint32_t _compute_period_ticks(struct stm32_pwm_desc *desc,
				      uint32_t timer_clk_hz, uint32_t period_ns)
{
	float pwm_frequency;
	uint32_t period;

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
	/* Timer period is determined based on the timer peripheral frequency */
	pwm_frequency = (1.0 / period_ns);
	period = (uint32_t)(timer_clk_hz / (pwm_frequency *
					    FREQUENCY_HZ_TO_TIME_NS_FACTOR)) - 1;

	return period;
}

/**
 * @brief Initialize the PWM Timer.
 * @param desc - STM32 PWM descriptor.
 * @param param - The structure containing PWM init parameters.
 * @return 0 in case of success, negative error code otherwise.
 * @note PWM Timer period is dependent upon the timer clock and prescaler value.
 *       CounterMode is UP for timer frequency generation in below function.
 */
static int32_t stm32_init_timer(struct stm32_pwm_desc *desc,
				const struct no_os_pwm_init_param *param)
{
	int32_t ret;
	uint32_t period;
	TIM_TypeDef *base = NULL;
	uint32_t timer_frequency_hz;
	struct stm32_pwm_init_param *sparam = param->extra;

	if (sparam->get_timer_clock) {
		timer_frequency_hz = sparam->get_timer_clock();
		timer_frequency_hz *= sparam->clock_divider;
		timer_frequency_hz /= (sparam->prescaler + 1);
		period = _compute_period_ticks(desc, timer_frequency_hz, param->period_ns);
	} else {
		period = PWM_DEFAULT_PERIOD - 1;
	}

	switch (param->id) {
#if defined(TIM1)
	case 1:
		base = TIM1;
		break;
#endif
#if defined(TIM2)
	case 2:
		base = TIM2;
		break;
#endif
#if defined(TIM3)
	case 3:
		base = TIM3;
		break;
#endif
		break;
#if defined(TIM4)
	case 4:
		base = TIM4;
		break;
#endif
#if defined(TIM5)
	case 5:
		base = TIM5;
		break;
#endif
#if defined(TIM6)
	case 6:
		base = TIM6;
		break;
#endif
#if defined(TIM7)
	case 7:
		base = TIM7;
		break;
#endif
#if defined(TIM8)
	case 8:
		base = TIM8;
		break;
#endif
#if defined(TIM9)
	case 9:
		base = TIM9;
		break;
#endif
#if defined(TIM10)
	case 10:
		base = TIM10;
		break;
#endif
#if defined(TIM11)
	case 11:
		base = TIM11;
		break;
#endif
#if defined(TIM12)
	case 12:
		base = TIM12;
		break;
#endif
	default:
		ret = -EINVAL;
		goto error;
	};

	desc->htimer.Instance = base;
	desc->htimer.Init.Prescaler = sparam->prescaler;
	desc->htimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	desc->htimer.Init.Period = period;
	desc->htimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	desc->htimer.Init.AutoReloadPreload = sparam->timer_autoreload ?
					      TIM_AUTORELOAD_PRELOAD_ENABLE : TIM_MASTERSLAVEMODE_DISABLE;
	desc->htimer.Init.RepetitionCounter = 0;
	desc->htimer.State = HAL_TIM_STATE_RESET;
	if (HAL_TIM_Base_Init(&desc->htimer) != HAL_OK)
		return -EIO;

	if (HAL_TIM_PWM_Init(&desc->htimer) != HAL_OK)
		return -EIO;

	/* Store the timer specific configuration for later use.*/
	desc->prescaler = sparam->prescaler;
	desc->get_timer_clock = sparam->get_timer_clock;
	desc->clock_divider = sparam->clock_divider;
	desc->timer_autoreload = sparam->timer_autoreload;

	if (param->pwm_callback) {
		struct no_os_irq_init_param nvic_tim_cplt = {
			.platform_ops = &stm32_irq_ops
		};

		ret = no_os_irq_ctrl_init(&desc->nvic_tim, &nvic_tim_cplt);
		if (ret < 0)
			goto error;

		sparam->timer_callback.callback = &param->pwm_callback;
		sparam->timer_callback.ctx = desc;
		sparam->timer_callback.event = NO_OS_EVT_TIM_PWM_PULSE_FINISHED;
		sparam->timer_callback.peripheral = NO_OS_TIM_IRQ;
		sparam->timer_callback.handle = &desc->htimer;

		ret = no_os_irq_register_callback(desc->nvic_tim, param->irq_id,
						  &sparam->timer_callback);
		if (ret < 0)
			goto error_register;

		ret = no_os_irq_enable(desc->nvic_tim, param->irq_id);
		if (ret < 0)
			goto error_enable;
	}

	return 0;
error_enable:
	no_os_irq_unregister_callback(desc->nvic_tim, param->irq_id,
				      &sparam->timer_callback);
error_register:
	no_os_irq_ctrl_remove(desc->nvic_tim);
error:
	return ret;
}

/**
 * @brief Initialize the PWM.
 * @param desc - STM32 PWM descriptor.
 * @param param - The structure containing PWM init parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t stm32_init_pwm(struct stm32_pwm_desc *desc,
			      const struct no_os_pwm_init_param *param)
{
	uint32_t pwm_pulse_width;
	float duty_cycle_percentage;
	uint32_t ocmode;
	uint32_t chn;
	TIM_OC_InitTypeDef sConfigOC = {0};
	uint32_t period = desc->htimer.Init.Period;
	struct stm32_pwm_init_param *sparam = param->extra;

	switch (sparam->mode) {
	case TIM_OC_TOGGLE:
		ocmode = TIM_OCMODE_TOGGLE;
		break;
	case TIM_OC_PWM1:
		ocmode = TIM_OCMODE_PWM1;
		break;
	case TIM_OC_PWM2:
		ocmode = TIM_OCMODE_PWM2;
		break;
	default:
		return -EINVAL;
	}

	switch (sparam->timer_chn) {
#if defined(TIM_CHANNEL_1)
	case 1:
		chn = TIM_CHANNEL_1;
		break;
#endif
#if defined(TIM_CHANNEL_2)
	case 2:
		chn = TIM_CHANNEL_2;
		break;
#endif
#if defined(TIM_CHANNEL_3)
	case 3:
		chn = TIM_CHANNEL_3;
		break;
#endif
#if defined(TIM_CHANNEL_4)
	case 4:
		chn = TIM_CHANNEL_4;
		break;
#endif
#if defined(TIM_CHANNEL_5)
	case 5:
		chn = TIM_CHANNEL_5;
		break;
#endif
#if defined(TIM_CHANNEL_6)
	case 6:
		chn = TIM_CHANNEL_6;
		break;
#endif
	default:
		return -EINVAL;
	};

	/* Calculate the percentage duty cycle */
	duty_cycle_percentage = ((float)param->duty_cycle_ns / param->period_ns) * 100;
	pwm_pulse_width = (uint32_t)((period + 1) * duty_cycle_percentage) / (100);

	sConfigOC.OCMode = ocmode;
	sConfigOC.Pulse = pwm_pulse_width;
	switch (param->polarity) {
	case NO_OS_PWM_POLARITY_HIGH:
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		break;
	case NO_OS_PWM_POLARITY_LOW:
		sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
		break;
	default:
		return -EINVAL;
	};

	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&desc->htimer, &sConfigOC, chn) != HAL_OK)
		return -EIO;

	desc->mode = sparam->mode;
	desc->timer_chn = sparam->timer_chn;
	desc->complementary_channel = sparam->complementary_channel;

	return 0;
}

/**
 * @brief Initialize the PWM.
 * @param desc - PWM descriptor.
 * @param param - The structure containing PWM init parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t stm32_pwm_init(struct no_os_pwm_desc **desc,
		       const struct no_os_pwm_init_param *param)
{
	struct no_os_pwm_desc *descriptor;
	struct stm32_pwm_desc *extra;
	int32_t ret;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct no_os_pwm_desc *)calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	extra = (struct stm32_pwm_desc *)calloc(1, sizeof(*extra));
	if (!extra) {
		ret = -ENOMEM;
		goto error_desc;
	}

	/* Initialize the PWM timer */
	ret = stm32_init_timer(extra, param);
	if (ret)
		goto error_extra;

	/* Initialize the PWM */
	ret = stm32_init_pwm(extra, param);
	if (ret)
		goto error_extra;

	/* Initialize the GPIO if timer output is required*/
	if (param->pwm_gpio) {
		ret = no_os_gpio_get(&descriptor->pwm_gpio, param->pwm_gpio);
		if (ret)
			goto error_extra;
	}

	descriptor->id = param->id;
	descriptor->extra = extra;
	descriptor->duty_cycle_ns = param->duty_cycle_ns;
	descriptor->period_ns = param->period_ns;
	descriptor->phase_ns = param->phase_ns;
	descriptor->polarity = param->polarity;
	descriptor->irq_id = param->irq_id;
	*desc = descriptor;

	return 0;
error_extra:
	free(extra);
error_desc:
	free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by stm32_pwm_init()
 * @param desc - PWM descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t stm32_pwm_remove(struct no_os_pwm_desc *desc)
{
	int32_t ret;
	struct stm32_pwm_desc *extra;

	if (!desc || !desc->extra)
		return -EINVAL;

	extra = desc->extra;

	if (HAL_TIM_Base_DeInit(&extra->htimer) != HAL_OK)
		return -EIO;

	if (HAL_TIM_PWM_DeInit(&extra->htimer) != HAL_OK)
		return -EIO;

	ret = no_os_gpio_remove(extra->gpio);
	if (ret)
		return ret;

	if (desc->irq_id) {
		ret = no_os_irq_unregister_callback(extra->nvic_tim, desc->irq_id,
						    &extra->timer_callback);
		if (ret)
			return ret;

		ret = no_os_irq_disable(extra->nvic_tim, desc->irq_id);
		if (ret)
			return ret;

		ret = no_os_irq_ctrl_remove(extra->nvic_tim);
		if (ret)
			return ret;
	}

	free(desc->extra);
	free(desc);

	return 0;
}

/**
 * @brief Enable the PWM
 * @param desc - PWM descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t stm32_pwm_enable(struct no_os_pwm_desc *desc)
{
	int32_t ret;
	uint32_t chn_num;
	struct stm32_pwm_desc *sparam;

	if (!desc)
		return -EINVAL;

	sparam = desc->extra;
	chn_num = NO_OS_CHN_TO_STM32_CHN(sparam->timer_chn);

	if (desc->irq_id) {
		if (sparam->complementary_channel) {
			ret = HAL_TIMEx_PWMN_Start_IT(&sparam->htimer, chn_num);
		} else {
			ret = HAL_TIM_PWM_Start_IT(&sparam->htimer, chn_num);
		}
	} else {
		if (sparam->complementary_channel)
			ret = HAL_TIMEx_PWMN_Start(&sparam->htimer, chn_num);
		else
			ret = HAL_TIM_PWM_Start(&sparam->htimer, chn_num);
	}

	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Disable the PWM
 * @param desc - PWM descriptor
 * @return 0 in case of success, negative error otherwise.
 */
int32_t stm32_pwm_disable(struct no_os_pwm_desc *desc)
{
	int32_t ret;
	uint32_t chn_num;
	struct stm32_pwm_desc *sparam;

	if (!desc)
		return -EINVAL;

	sparam = desc->extra;
	chn_num = NO_OS_CHN_TO_STM32_CHN(sparam->timer_chn);

	if (sparam->complementary_channel)
		ret = HAL_TIMEx_PWMN_Stop(&sparam->htimer, chn_num);
	else
		ret = HAL_TIM_PWM_Stop(&sparam->htimer, chn_num);

	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief	Set the PWM period.
 * @param	desc - Pointer where the configured instance is stored.
 * @param	period_ns - PWM period.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t stm32_pwm_set_period(struct no_os_pwm_desc *desc,
			     uint32_t period_ns)
{
	int32_t ret;
	struct no_os_pwm_init_param param;
	struct stm32_pwm_init_param sparam;

	param.id = desc->id;
	param.duty_cycle_ns = desc->duty_cycle_ns;
	param.period_ns = period_ns;
	param.phase_ns = desc->phase_ns;
	param.pwm_callback = desc->pwm_callback;
	param.irq_id = desc->irq_id;
	sparam.clock_divider = ((struct stm32_pwm_desc *)desc->extra)->clock_divider;
	sparam.prescaler = ((struct stm32_pwm_desc *)desc->extra)->prescaler;
	sparam.timer_autoreload = ((struct stm32_pwm_desc *)
				   desc->extra)->timer_autoreload;
	sparam.get_timer_clock = ((struct stm32_pwm_desc *)
				  desc->extra)->get_timer_clock;
	sparam.timer_chn = ((struct stm32_pwm_desc *)desc->extra)->timer_chn;
	sparam.timer_callback = ((struct stm32_pwm_desc*)desc->extra)->timer_callback;
	param.extra = &sparam;

	if (!desc || !desc->extra)
		return -EINVAL;

	ret = stm32_init_timer(desc->extra, &param);
	if (ret != HAL_OK)
		return -EIO;

	desc->period_ns = period_ns;

	return 0;
}

/**
 * @brief	Get the PWM period.
 * @param	desc - Pointer where the configured instance is stored.
 * @param	period_ns - PWM period.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t stm32_pwm_get_period(struct no_os_pwm_desc *desc,
			     uint32_t *period_ns)
{
	uint32_t timer_hz;

	if (!desc || !desc->extra || !period_ns)
		return -EINVAL;

	struct stm32_pwm_desc *sparam = desc->extra;
	timer_hz = sparam->get_timer_clock() * sparam->clock_divider;
	timer_hz /= (sparam->prescaler + 1);

	*period_ns = (uint32_t)((sparam->htimer.Init.Period + 1) *
				(FREQUENCY_HZ_TO_TIME_NS_FACTOR / timer_hz));

	return 0;
}

/**
 * @brief	Set the PWM duty cycle.
 * @param	desc - Pointer where the configured instance is stored.
 * @param	duty_cycle_ns - PWM duty cycle.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t stm32_pwm_set_duty_cycle(struct no_os_pwm_desc *desc,
				 uint32_t duty_cycle_ns)
{
	int32_t ret;
	struct no_os_pwm_init_param param;
	struct stm32_pwm_init_param sparam;

	if (!desc || !desc->extra)
		return -EINVAL;

	param.duty_cycle_ns = duty_cycle_ns;
	param.period_ns = desc->period_ns;
	param.phase_ns = desc->phase_ns;
	sparam.mode = ((struct stm32_pwm_desc *)desc->extra)->mode;
	sparam.timer_chn = ((struct stm32_pwm_desc *)desc->extra)->timer_chn;
	sparam.complementary_channel = ((struct stm32_pwm_desc*)
					desc->extra)->complementary_channel;
	param.extra = &sparam;

	ret = stm32_init_pwm(desc->extra, &param);
	if (ret != HAL_OK)
		return -EIO;

	desc->duty_cycle_ns = param.duty_cycle_ns;

	return 0;
}

/**
 * @brief	Get the PWM duty cycle.
 * @param	desc - Pointer where the configured instance is stored.
 * @param	duty_cycle_ns - PWM duty cycle.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t stm32_pwm_get_duty_cycle(struct no_os_pwm_desc *desc,
				 uint32_t *duty_cycle_ns)
{
	*duty_cycle_ns = desc->duty_cycle_ns;

	return 0;
}

/**
* @brief STM32 platform specific PWM platform ops structure
*/
const struct no_os_pwm_platform_ops stm32_pwm_ops = {
	.pwm_ops_init = &stm32_pwm_init,
	.pwm_ops_enable = &stm32_pwm_enable,
	.pwm_ops_disable = &stm32_pwm_disable,
	.pwm_ops_set_period = &stm32_pwm_set_period,
	.pwm_ops_get_period = &stm32_pwm_get_period,
	.pwm_ops_set_duty_cycle = &stm32_pwm_set_duty_cycle,
	.pwm_ops_get_duty_cycle = &stm32_pwm_get_duty_cycle,
	.pwm_ops_remove = &stm32_pwm_remove
};

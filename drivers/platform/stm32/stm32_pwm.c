/***************************************************************************//**
 *   @file   stm32/stm32_pwm.c
 *   @brief  Implementation of stm32 pwm driver.
********************************************************************************
 * Copyright 2023-25(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "stm32_gpio.h"
#include "stm32_pwm.h"
#include "stm32_irq.h"

/* For 16-bit pwm timer */
#define	PWM_DEFAULT_PERIOD	             0xffff
#define NO_OS_CHN_TO_STM32_CHN(x)        (0x4*(x-1))
#define FREQUENCY_HZ_TO_TIME_NS_FACTOR   1000000000.0

/**
 * @brief Calculate the period in ticks.
 * @param get_timer_clock - timer source clock function.
 * @param clock_divider - timer clock divider.
 * @param prescaler - timer prescaler.
 * @param period_ns - timer period.
 * @return return period value in ticks.
 */
static uint32_t _compute_period_ticks(uint32_t (*get_timer_clock)(void),
				      uint32_t clock_divider,
				      uint32_t prescaler,
				      uint32_t period_ns)
{
	double pwm_frequency;
	uint32_t timer_frequency_hz;
	uint32_t period;

	if (get_timer_clock) {
		timer_frequency_hz = get_timer_clock();
		timer_frequency_hz *= clock_divider;
		timer_frequency_hz /= (prescaler + 1);

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
		period = (uint32_t)(timer_frequency_hz / (pwm_frequency *
				    FREQUENCY_HZ_TO_TIME_NS_FACTOR)) - 1;
	} else {
		period = PWM_DEFAULT_PERIOD - 1;
	}

	return period;
}

#ifdef HAL_TIM_MODULE_ENABLED
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
	struct stm32_pwm_init_param *sparam = param->extra;
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_HandleTypeDef *htimer;

	/* Compute period ticks */
	period = _compute_period_ticks(sparam->get_timer_clock, sparam->clock_divider,
				       sparam->prescaler, param->period_ns);

	htimer = (TIM_HandleTypeDef *)sparam->htimer;

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

	htimer->Instance = base;
	htimer->Init.Prescaler = sparam->prescaler;
	htimer->Init.CounterMode = TIM_COUNTERMODE_UP;
	htimer->Init.Period = period;
	htimer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htimer->Init.AutoReloadPreload = sparam->timer_autoreload ?
					 TIM_AUTORELOAD_PRELOAD_ENABLE : TIM_MASTERSLAVEMODE_DISABLE;
	htimer->Init.RepetitionCounter = sparam->repetitions;
	htimer->State = HAL_TIM_STATE_RESET;
	if (HAL_TIM_Base_Init(htimer) != HAL_OK)
		return -EIO;

	/* If slave mode enabled but trigger source is not provided */
	if (sparam->slave_mode != STM32_PWM_SM_DISABLE &&
	    sparam->trigger_source == PWM_TS_NONE) {
		return -EINVAL;
	}

	if (sparam->slave_mode == STM32_PWM_SM_DISABLE) {
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
	} else {
		switch (sparam->trigger_source) {
		case PWM_TS_ITR0:
			sSlaveConfig.InputTrigger = TIM_TS_ITR0;
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ITR0;
			break;
		case PWM_TS_ITR1:
			sSlaveConfig.InputTrigger = TIM_TS_ITR1;
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ITR1;
			break;
		case PWM_TS_ITR2:
			sSlaveConfig.InputTrigger = TIM_TS_ITR2;
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ITR2;
			break;
		case PWM_TS_ITR3:
			sSlaveConfig.InputTrigger = TIM_TS_ITR3;
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ITR3;
			break;
		case PWM_TS_ETR:
			sSlaveConfig.InputTrigger = TIM_TS_ETRF;
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE1;
			break;
		default:
			break;
		}

		switch (sparam->slave_mode) {
		case STM32_PWM_SM_EXTERNAL1:
			sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
			break;
		case STM32_PWM_SM_TRIGGER:
			sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			break;
		case STM32_PWM_SM_GATED:
			sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			break;
		default:
			return -EINVAL;
		}

		if (sparam->trigger_source == PWM_TS_ETR) {
			switch (sparam->trigger_polarity) {
			case PWM_TRIG_POL_RISING:
				sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
				break;
			case PWM_TRIG_POL_FALLING:
				sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_INVERTED;
				break;
			default:
				return -EINVAL;
			}
		}
	}

	if (HAL_TIM_ConfigClockSource(htimer, &sClockSourceConfig) != HAL_OK)
		return -EIO;

	if (HAL_TIM_SlaveConfigSynchro(htimer, &sSlaveConfig) != HAL_OK)
		return -EIO;

	if (HAL_TIM_PWM_Init(htimer) != HAL_OK)
		return -EIO;

	if (sparam->onepulse_enable) {
		if (HAL_TIM_OnePulse_Init(htimer, TIM_OPMODE_SINGLE) != HAL_OK)
			return -EIO;
	} else {
		if (HAL_TIM_OnePulse_Init(htimer, TIM_OPMODE_REPETITIVE) != HAL_OK)
			return -EIO;
	}

	switch (sparam->trigger_output) {
	case PWM_TRGO_RESET:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		break;
	case PWM_TRGO_ENABLE:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
		break;
	case PWM_TRGO_UPDATE:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
		break;
	case PWM_TRGO_OC1:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1;
		break;
	case PWM_TRGO_OC1REF:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
		break;
	case PWM_TRGO_OC2REF:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;
		break;
	case PWM_TRGO_OC3REF:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC3REF;
		break;
	case PWM_TRGO_OC4REF:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC4REF;
		break;
	default:
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	}
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(htimer,
			&sMasterConfig) != HAL_OK)
		return -EIO;

	/* Store the timer specific configuration for later use.*/
	desc->htimer = htimer;
	desc->pwm_timer = STM32_PWM_TIMER_TIM;
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

		sparam->timer_callback.callback = param->pwm_callback;
		sparam->timer_callback.ctx = desc;
		sparam->timer_callback.event = NO_OS_EVT_TIM_PWM_PULSE_FINISHED;
		sparam->timer_callback.peripheral = NO_OS_TIM_IRQ;
		sparam->timer_callback.handle = htimer;

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
#endif

#ifdef HAL_LPTIM_MODULE_ENABLED
/**
 * @brief Initialize the PWM LPTimer.
 * @param desc - STM32 PWM descriptor.
 * @param param - The structure containing PWM init parameters.
 * @return 0 in case of success, negative error code otherwise.
 * @note PWM Timer period is dependent upon the timer clock and prescaler value.
 *       CounterMode is UP for timer frequency generation in below function.
 */
static int32_t stm32_init_lptimer(struct stm32_pwm_desc *desc,
				  const struct no_os_pwm_init_param *param)
{
	int32_t ret;
	uint32_t period;
	LPTIM_TypeDef *base = NULL;
	struct stm32_pwm_init_param *sparam = param->extra;
	LPTIM_HandleTypeDef *hlptimer;
	uint32_t computed_prescaler = 0;

	/* Compute period ticks */
	period = _compute_period_ticks(sparam->get_timer_clock, sparam->clock_divider,
				       sparam->prescaler - 1, param->period_ns);
	hlptimer = (LPTIM_HandleTypeDef *) sparam->htimer;

	switch (param->id) {
#if defined(LPTIM1)
	case 1:
		base = LPTIM1;
		break;
#endif
#if defined(LPTIM2)
	case 2:
		base = LPTIM2;
		break;
#endif
#if defined(LPTIM3)
	case 3:
		base = LPTIM3;
		break;
#endif
		break;
#if defined(LPTIM4)
	case 4:
		base = LPTIM4;
		break;
#endif
#if defined(LPTIM5)
	case 5:
		base = LPTIM5;
		break;
#endif
#if defined(LPTIM6)
	case 6:
		base = LPTIM6;
		break;
#endif
#if defined(LPTIM7)
	case 7:
		base = LPTIM7;
		break;
#endif
	default:
		ret = -EINVAL;
		goto error;
	};

	switch (sparam->prescaler) {
	case 1:
		computed_prescaler = LPTIM_PRESCALER_DIV1;
		break;
	case 2:
		computed_prescaler = LPTIM_PRESCALER_DIV2;
		break;
	case 3 ... 4:
		computed_prescaler = LPTIM_PRESCALER_DIV4;
		break;
	case 5 ... 8:
		computed_prescaler = LPTIM_PRESCALER_DIV8;
		break;
	case 9 ... 16:
		computed_prescaler = LPTIM_PRESCALER_DIV16;
		break;
	case 17 ... 32:
		computed_prescaler = LPTIM_PRESCALER_DIV32;
		break;
	case 33 ... 64:
		computed_prescaler = LPTIM_PRESCALER_DIV64;
		break;
	default:
		computed_prescaler = LPTIM_PRESCALER_DIV128;
		break;
	}

	hlptimer->Instance = base;
	hlptimer->Init.Period = period;
	hlptimer->Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
	hlptimer->Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
	hlptimer->Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
	hlptimer->Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
	hlptimer->Init.RepetitionCounter = sparam->repetitions;

	hlptimer->Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
	hlptimer->Init.Clock.Prescaler = computed_prescaler;
	hlptimer->State = HAL_LPTIM_STATE_RESET;

	switch (sparam->slave_mode) {
	case STM32_PWM_SM_DISABLE:
		hlptimer->Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
		break;
	case STM32_PWM_SM_TRIGGER:
		switch (sparam->trigger_source) {
		case PWM_TS_ITR0:
			hlptimer->Init.Trigger.Source = LPTIM_TRIGSOURCE_0;
			break;
		case PWM_TS_ITR1:
			hlptimer->Init.Trigger.Source = LPTIM_TRIGSOURCE_1;
			break;
		case PWM_TS_ITR2:
			hlptimer->Init.Trigger.Source = LPTIM_TRIGSOURCE_2;
			break;
		case PWM_TS_ITR3:
			hlptimer->Init.Trigger.Source = LPTIM_TRIGSOURCE_3;
			break;
		default:
			return -EINVAL;
		}
		break;
	case STM32_PWM_SM_EXTERNAL1:
		/* Not supported */
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	hlptimer->Init.Trigger.ActiveEdge = LPTIM_ACTIVEEDGE_RISING;
	hlptimer->Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION;

	if (HAL_LPTIM_Init(hlptimer) != HAL_OK)
		return -EIO;

	/* Store the timer specific configuration for later use.*/
	desc->htimer = hlptimer;
	desc->pwm_timer = STM32_PWM_TIMER_LPTIM;
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

		sparam->timer_callback.callback = param->pwm_callback;
		sparam->timer_callback.ctx = desc;
		sparam->timer_callback.event = NO_OS_EVT_LPTIM_PWM_PULSE_FINISHED;
		sparam->timer_callback.peripheral = NO_OS_LPTIM_IRQ;
		sparam->timer_callback.handle = hlptimer;

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
#endif

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
	uint32_t chn;
	struct stm32_pwm_init_param *sparam = param->extra;
	uint32_t period;
#ifdef HAL_LPTIM_MODULE_ENABLED
	LPTIM_OC_ConfigTypeDef sLpTimerConfigOC = {0};
	LPTIM_HandleTypeDef *hlptimer;
#endif
#ifdef HAL_TIM_MODULE_ENABLED
	TIM_OC_InitTypeDef sTimerConfigOC = {0};
	TIM_HandleTypeDef *htimer;
#endif

	switch (sparam->pwm_timer) {
#ifdef HAL_TIM_MODULE_ENABLED
	case STM32_PWM_TIMER_TIM:
		htimer = (TIM_HandleTypeDef *) sparam->htimer;
		period = htimer->Init.Period;
		switch (sparam->mode) {
		case TIM_OC_TOGGLE:
			sTimerConfigOC.OCMode = TIM_OCMODE_TOGGLE;
			break;
		case TIM_OC_PWM1:
			sTimerConfigOC.OCMode = TIM_OCMODE_PWM1;
			break;
		case TIM_OC_PWM2:
			sTimerConfigOC.OCMode = TIM_OCMODE_PWM2;
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

		sTimerConfigOC.Pulse = pwm_pulse_width;
		switch (param->polarity) {
		case NO_OS_PWM_POLARITY_HIGH:
			sTimerConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
			break;
		case NO_OS_PWM_POLARITY_LOW:
			sTimerConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
			break;
		default:
			return -EINVAL;
		};

		sTimerConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sTimerConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sTimerConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		if (HAL_TIM_PWM_ConfigChannel(htimer, &sTimerConfigOC, chn) != HAL_OK)
			return -EIO;
		break;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	case STM32_PWM_TIMER_LPTIM:
		hlptimer = (LPTIM_HandleTypeDef *) sparam->htimer;
		period = hlptimer->Init.Period;
		sparam->complementary_channel = false;
		sparam->dma_enable = false;

		switch (sparam->timer_chn) {
#if defined(LPTIM_CHANNEL_1)
		case 1:
			chn = LPTIM_CHANNEL_1;
			break;
#endif
#if defined(LPTIM_CHANNEL_2)
		case 2:
			chn = LPTIM_CHANNEL_2;
			break;
#endif
		default:
			return -EINVAL;
		};

		/* Calculate the percentage duty cycle */
		duty_cycle_percentage = ((float)param->duty_cycle_ns / param->period_ns) * 100;
		pwm_pulse_width = (uint32_t)((period + 1) * duty_cycle_percentage) / (100);

		sLpTimerConfigOC.Pulse = pwm_pulse_width;
		switch (param->polarity) {
		case NO_OS_PWM_POLARITY_HIGH:
			sLpTimerConfigOC.OCPolarity = LPTIM_OCPOLARITY_HIGH;
			break;
		case NO_OS_PWM_POLARITY_LOW:
			sLpTimerConfigOC.OCPolarity = LPTIM_OCPOLARITY_LOW;
			break;
		default:
			return -EINVAL;
		};

		if (HAL_LPTIM_OC_ConfigChannel(hlptimer, &sLpTimerConfigOC, chn) != HAL_OK)
			return -EIO;

		break;
#endif
	default:
		return -EINVAL;
	}

	desc->mode = sparam->mode;
	desc->timer_chn = sparam->timer_chn;
	desc->complementary_channel = sparam->complementary_channel;
	desc->dma_enable = sparam->dma_enable;
	desc->repetitions = sparam->repetitions;

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
	struct stm32_pwm_init_param *sparam;
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

	sparam = (struct stm32_pwm_init_param *)param->extra;

	/* Initialize the PWM timer */
	switch (sparam->pwm_timer) {
#ifdef HAL_TIM_MODULE_ENABLED
	case STM32_PWM_TIMER_TIM:
		ret = stm32_init_timer(extra, param);
		if (ret)
			goto error_extra;
		break;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	case STM32_PWM_TIMER_LPTIM:
		ret = stm32_init_lptimer(extra, param);
		if (ret)
			goto error_extra;
		break;
#endif
	default:
		return -EINVAL;
	}

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
#ifdef HAL_TIM_MODULE_ENABLED
	TIM_HandleTypeDef *htimer;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	LPTIM_HandleTypeDef *hlptimer;
#endif

	if (!desc || !desc->extra)
		return -EINVAL;

	extra = desc->extra;
	switch (extra->pwm_timer) {
#ifdef HAL_TIM_MODULE_ENABLED
	case STM32_PWM_TIMER_TIM:
		htimer = (TIM_HandleTypeDef *) extra->htimer;

		if (HAL_TIM_PWM_DeInit(htimer) != HAL_OK)
			return -EIO;

		if (HAL_TIM_Base_DeInit(htimer) != HAL_OK)
			return -EIO;
		break;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	case STM32_PWM_TIMER_LPTIM:
		hlptimer = (LPTIM_HandleTypeDef *) extra->htimer;

		if (HAL_LPTIM_DeInit(hlptimer) != HAL_OK)
			return -EIO;
		break;
#endif
	default:
		return -EINVAL;
	}

	ret = no_os_gpio_remove(desc->pwm_gpio);
	if (ret)
		return ret;

	if (desc->pwm_callback) {
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

#ifdef HAL_TIM_MODULE_ENABLED
	TIM_HandleTypeDef *htimer;
	const uint32_t timer_map[] = {
		[1] = TIM_DMA_CC1,
		[2] = TIM_DMA_CC2,
		[3] = TIM_DMA_CC3,
		[4] = TIM_DMA_CC4,
	};

#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	LPTIM_HandleTypeDef *hlptimer;
	const uint32_t lptimer_map[] = {
		[1] = LPTIM_DMA_CC1,
		[2] = LPTIM_DMA_CC2,
	};
#endif

	if (!desc)
		return -EINVAL;

	sparam = desc->extra;
	chn_num = NO_OS_CHN_TO_STM32_CHN(sparam->timer_chn);

	switch (sparam->pwm_timer) {
#ifdef HAL_TIM_MODULE_ENABLED
	case STM32_PWM_TIMER_TIM:
		htimer = (TIM_HandleTypeDef *) sparam->htimer;
		/* set counter to 0 to start from known state */
		__HAL_TIM_SET_COUNTER(htimer, 0);

		if (sparam->dma_enable) {
			/*sw trigger to reload repetition counter from repetition register */
			htimer->Instance->RCR = sparam->repetitions;
			htimer->Instance->EGR |= (1 << 0);
			__HAL_TIM_ENABLE_DMA(htimer, timer_map[sparam->timer_chn]);
		}

		if (desc->irq_id) {
			if (sparam->complementary_channel) {
				ret = HAL_TIMEx_PWMN_Start_IT(htimer, chn_num);
			} else {
				ret = HAL_TIM_PWM_Start_IT(htimer, chn_num);
			}
		} else {
			if (sparam->complementary_channel)
				ret = HAL_TIMEx_PWMN_Start(htimer, chn_num);
			else
				ret = HAL_TIM_PWM_Start(htimer, chn_num);
		}
		if (ret != HAL_OK) {
			return -EIO;
		}

		break;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	case STM32_PWM_TIMER_LPTIM:
		hlptimer = (LPTIM_HandleTypeDef *) sparam->htimer;

		/* set counter to 0 to start from known state */
		__HAL_LPTIM_RESET_COUNTER(hlptimer);

		if (sparam->dma_enable) {
			/*sw trigger to reload repetition counter from repetition register */
			hlptimer->Instance->RCR = sparam->repetitions;
			hlptimer->Instance->CR |= (1 << 3);
			__HAL_LPTIM_ENABLE_DMA(hlptimer, lptimer_map[sparam->timer_chn]);
		}

		if (desc->irq_id) {
			if (sparam->onepulse_enable)
				ret = HAL_LPTIM_OnePulse_Start_IT(hlptimer, chn_num);
			else
				ret = HAL_LPTIM_PWM_Start_IT(hlptimer, chn_num);
		} else {
			if (sparam->onepulse_enable)
				ret = HAL_LPTIM_OnePulse_Start(hlptimer, chn_num);
			else
				ret = HAL_LPTIM_PWM_Start(hlptimer, chn_num);
		}
		if (ret != HAL_OK) {
			return -EIO;
		}

		break;
#endif
	default:
		return -EINVAL;
	}

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

#ifdef HAL_TIM_MODULE_ENABLED
	TIM_HandleTypeDef *htimer;
	const uint32_t timer_map[] = {
		[1] = TIM_DMA_CC1,
		[2] = TIM_DMA_CC2,
		[3] = TIM_DMA_CC3,
		[4] = TIM_DMA_CC4,
	};

#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	LPTIM_HandleTypeDef *hlptimer;
	const uint32_t lptimer_map[] = {
		[1] = LPTIM_DMA_CC1,
		[2] = LPTIM_DMA_CC2,
	};
#endif

	if (!desc)
		return -EINVAL;

	sparam = desc->extra;
	chn_num = NO_OS_CHN_TO_STM32_CHN(sparam->timer_chn);

	switch (sparam->pwm_timer) {
#ifdef HAL_TIM_MODULE_ENABLED
	case STM32_PWM_TIMER_TIM:
		htimer = (TIM_HandleTypeDef *) sparam->htimer;

		if (sparam->dma_enable)
			__HAL_TIM_DISABLE_DMA(htimer,  timer_map[sparam->timer_chn]);

		if (desc->irq_id) {
			if (sparam->complementary_channel)
				ret = HAL_TIMEx_PWMN_Stop_IT(htimer, chn_num);
			else
				ret = HAL_TIM_PWM_Stop_IT(htimer, chn_num);
		} else {
			if (sparam->complementary_channel)
				ret = HAL_TIMEx_PWMN_Stop(htimer, chn_num);
			else
				ret = HAL_TIM_PWM_Stop(htimer, chn_num);
		}

		if (ret != HAL_OK) {
			return -EIO;
		}

		break;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	case STM32_PWM_TIMER_LPTIM:
		hlptimer = (LPTIM_HandleTypeDef *) sparam->htimer;

		if (sparam->dma_enable)
			__HAL_LPTIM_DISABLE_DMA(hlptimer,  lptimer_map[sparam->timer_chn]);

		if (desc->irq_id) {
			if (sparam->onepulse_enable)
				ret = HAL_LPTIM_OnePulse_Stop_IT(hlptimer, chn_num);
			else
				ret = HAL_LPTIM_PWM_Stop_IT(hlptimer, chn_num);
		} else {
			if (sparam->onepulse_enable)
				ret = HAL_LPTIM_OnePulse_Stop(hlptimer, chn_num);
			else
				ret = HAL_LPTIM_PWM_Stop(hlptimer, chn_num);
		}

		if (ret != HAL_OK) {
			return -EIO;
		}
		break;
#endif
	default:
		return -EINVAL;
	}

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
	struct stm32_pwm_desc *sdesc;
	uint32_t period;

#ifdef HAL_TIM_MODULE_ENABLED
	TIM_HandleTypeDef *htimer;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	LPTIM_HandleTypeDef *hlptimer;
#endif

	if (!desc || !desc->extra)
		return -EINVAL;

	sdesc = desc->extra;

	switch (sdesc->pwm_timer) {
#ifdef HAL_TIM_MODULE_ENABLED
	case STM32_PWM_TIMER_TIM:
		htimer = (TIM_HandleTypeDef *) sdesc->htimer;
		/* Compute period ticks */
		period = _compute_period_ticks(sdesc->get_timer_clock, sdesc->clock_divider,
					       sdesc->prescaler, period_ns);

		htimer->Init.AutoReloadPreload = sdesc->timer_autoreload ?
						 TIM_AUTORELOAD_PRELOAD_ENABLE : TIM_MASTERSLAVEMODE_DISABLE;
		htimer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htimer->Init.CounterMode = TIM_COUNTERMODE_UP;
		htimer->Init.Prescaler = sdesc->prescaler;
		htimer->Init.Period = period;
		htimer->Init.RepetitionCounter = sdesc->repetitions;

		TIM_Base_SetConfig(htimer->Instance, &htimer->Init);
		break;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	case STM32_PWM_TIMER_LPTIM:
		hlptimer = (LPTIM_HandleTypeDef *) sdesc->htimer;
		/* Compute period ticks */
		period = _compute_period_ticks(sdesc->get_timer_clock, sdesc->clock_divider,
					       sdesc->prescaler - 1, period_ns);
		hlptimer->Init.Period = period;

		/* Change the LPTIM state */
		hlptimer->State = HAL_LPTIM_STATE_READY;
		if (HAL_LPTIM_Init(hlptimer) != HAL_OK)
			return -EIO;
		break;
#endif
	default:
		return -EINVAL;
	}

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
	uint32_t period;

	if (!desc || !desc->extra || !period_ns)
		return -EINVAL;

	struct stm32_pwm_desc *sparam = desc->extra;

	timer_hz = sparam->get_timer_clock() * sparam->clock_divider;

	switch (sparam->pwm_timer) {
#ifdef HAL_TIM_MODULE_ENABLED
	case STM32_PWM_TIMER_TIM:
		timer_hz /= (sparam->prescaler + 1);
		period = ((TIM_HandleTypeDef *)sparam->htimer)->Init.Period;
		*period_ns = (uint32_t)((period + 1) *
					(FREQUENCY_HZ_TO_TIME_NS_FACTOR / timer_hz));
		break;
#endif
#ifdef HAL_LPTIM_MODULE_ENABLED
	case STM32_PWM_TIMER_LPTIM:
		timer_hz /= sparam->prescaler;
		period = ((LPTIM_HandleTypeDef *)sparam->htimer)->Init.Period;
		*period_ns = (uint32_t)((period + 1) *
					(FREQUENCY_HZ_TO_TIME_NS_FACTOR / timer_hz));
		break;
#endif
	default:
		return -EINVAL;
	}

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
	struct no_os_pwm_init_param param = {0};
	struct stm32_pwm_init_param sparam = {0};

	if (!desc || !desc->extra)
		return -EINVAL;

	param.duty_cycle_ns = duty_cycle_ns;
	param.period_ns = desc->period_ns;
	param.phase_ns = desc->phase_ns;
	sparam.htimer = ((struct stm32_pwm_desc *)desc->extra)->htimer;
	sparam.pwm_timer = ((struct stm32_pwm_desc *)desc->extra)->pwm_timer;
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

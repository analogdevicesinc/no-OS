/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <errno.h>
#include "capi_alloc.h"
#include "stm32_capi_timer_priv.h"

#ifdef HAL_TIM_MODULE_ENABLED

#define NSEC_PER_SEC			1000000000ULL
#define STM32_TIMER_DEFAULT_FREQ_HZ	1000000
#define STM32_TIMER_DEFAULT_PERIOD	0xFFFFFFFFU
#define STM32_TIMER_INVALID_CHANNEL	0xFFFFFFFFU

#define MAX_TIMER_INSTANCES		16

/* Lookup table for mapping HAL handles to private handles */
static struct {
	TIM_HandleTypeDef *htim;
	struct stm32_capi_timer_priv *priv;
} timer_handle_lookup[MAX_TIMER_INSTANCES];
static uint8_t timer_handle_count = 0;

/**
 * @brief Find the private handle associated with a HAL timer handle.
 * @param htim - Pointer to the HAL timer handle.
 * @return Pointer to the private handle, or NULL if not found.
 */
static struct stm32_capi_timer_priv *find_priv_from_htim(
	TIM_HandleTypeDef *htim)
{
	for (uint8_t i = 0; i < timer_handle_count; i++) {
		if (timer_handle_lookup[i].htim == htim)
			return timer_handle_lookup[i].priv;
	}
	return NULL;
}

/**
 * @brief Register a mapping between a HAL timer handle and a private handle.
 * @param htim - Pointer to the HAL timer handle.
 * @param priv - Pointer to the private handle.
 * @return 0 on success, negative error code otherwise.
 */
static int register_timer_handle(TIM_HandleTypeDef *htim,
				 struct stm32_capi_timer_priv *priv)
{
	if (timer_handle_count >= MAX_TIMER_INSTANCES)
		return -ENOMEM;

	timer_handle_lookup[timer_handle_count].htim = htim;
	timer_handle_lookup[timer_handle_count].priv = priv;
	timer_handle_count++;
	return 0;
}

/**
 * @brief Unregister the mapping for a HAL timer handle.
 * @param htim - Pointer to the HAL timer handle.
 */
static void unregister_timer_handle(TIM_HandleTypeDef *htim)
{
	for (uint8_t i = 0; i < timer_handle_count; i++) {
		if (timer_handle_lookup[i].htim == htim) {
			if (i < timer_handle_count - 1)
				timer_handle_lookup[i] =
					timer_handle_lookup[timer_handle_count - 1];
			timer_handle_count--;
			break;
		}
	}
}

/**
 * @brief Get timer base address from identifier.
 * @param identifier - Timer peripheral identifier or base address.
 * @param clk_freq - Pointer to store the clock frequency for the timer.
 * @return Pointer to the timer peripheral, or NULL if invalid.
 */
static TIM_TypeDef *get_timer_base_from_identifier(uint64_t identifier,
		uint32_t *clk_freq)
{
	uint32_t apb2_freq = HAL_RCC_GetPCLK2Freq();
	*clk_freq = HAL_RCC_GetPCLK1Freq();

	if (identifier >= PERIPH_BASE)
		return (TIM_TypeDef *)identifier;

	switch (identifier) {
#if defined(TIM1)
	case 1:
		*clk_freq = apb2_freq;
		return TIM1;
#endif
#if defined(TIM2)
	case 2:
		return TIM2;
#endif
#if defined(TIM3)
	case 3:
		return TIM3;
#endif
#if defined(TIM4)
	case 4:
		return TIM4;
#endif
#if defined(TIM5)
	case 5:
		return TIM5;
#endif
#if defined(TIM6)
	case 6:
		return TIM6;
#endif
#if defined(TIM7)
	case 7:
		return TIM7;
#endif
#if defined(TIM8)
	case 8:
		*clk_freq = apb2_freq;
		return TIM8;
#endif
#if defined(TIM9)
	case 9:
		return TIM9;
#endif
#if defined(TIM10)
	case 10:
		return TIM10;
#endif
#if defined(TIM11)
	case 11:
		return TIM11;
#endif
#if defined(TIM12)
	case 12:
		return TIM12;
#endif
#if defined(TIM13)
	case 13:
		return TIM13;
#endif
#if defined(TIM14)
	case 14:
		return TIM14;
#endif
#if defined(TIM15)
	case 15:
		return TIM15;
#endif
#if defined(TIM16)
	case 16:
		return TIM16;
#endif
#if defined(TIM17)
	case 17:
		return TIM17;
#endif
	default:
		return NULL;
	}
}

/**
 * @brief Map CAPI channel index to HAL timer channel constant.
 * @param chan - CAPI channel index (0-3).
 * @return HAL timer channel constant, or STM32_TIMER_INVALID_CHANNEL if invalid.
 */
static uint32_t get_hal_channel(uint32_t chan)
{
	switch (chan) {
	case 0:
		return TIM_CHANNEL_1;
	case 1:
		return TIM_CHANNEL_2;
	case 2:
		return TIM_CHANNEL_3;
	case 3:
		return TIM_CHANNEL_4;
	default:
		return STM32_TIMER_INVALID_CHANNEL;
	}
}

/**
 * @brief Map CAPI channel index to HAL timer interrupt flag.
 * @param chan - CAPI channel index (0-3).
 * @return HAL timer interrupt flag, or 0 if invalid.
 */
static uint32_t get_hal_it_flag(uint32_t chan)
{
	switch (chan) {
	case 0:
		return TIM_IT_CC1;
	case 1:
		return TIM_IT_CC2;
	case 2:
		return TIM_IT_CC3;
	case 3:
		return TIM_IT_CC4;
	default:
		return 0;
	}
}

/**
 * @brief Initialize the STM32 timer.
 * @param handle - Pointer to a pointer of the timer handle.
 * @param config - Pointer to the timer configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_init(struct capi_timer_handle **handle,
				 const struct capi_timer_config *config)
{
	struct capi_timer_handle *tim_handle;
	struct stm32_capi_timer_priv *priv;
	struct stm32_capi_timer_extra_config *extra;
	TIM_TypeDef *base;
	uint32_t src_clk_freq;
	uint32_t prescaler;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	base = get_timer_base_from_identifier(config->identifier, &src_clk_freq);
	if (!base)
		return -EINVAL;

	if (*handle == NULL) {
		tim_handle = capi_calloc(1, sizeof(*tim_handle));
		if (!tim_handle)
			return -ENOMEM;

		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(tim_handle);
			return -ENOMEM;
		}

		tim_handle->priv = priv;
		tim_handle->init_allocated = true;
	} else {
		tim_handle = *handle;
		tim_handle->init_allocated = false;

		if (!tim_handle->priv)
			return -EINVAL;

		priv = tim_handle->priv;
	}

	tim_handle->ops = config->ops;

	extra = (struct stm32_capi_timer_extra_config *)config->extra;

	if (extra && extra->htim) {
		priv->htim = *extra->htim;
	} else {
		priv->htim.Instance = base;
	}

	if (extra && extra->get_input_clock)
		priv->input_clock_hz = extra->get_input_clock();
	else if (config->input_clock_hz)
		priv->input_clock_hz = config->input_clock_hz;
	else
		priv->input_clock_hz = src_clk_freq;

	if (extra)
		priv->irq_num = extra->irq_num;

	priv->output_freq_hz = config->output_freq_hz;
	if (priv->output_freq_hz == 0)
		priv->output_freq_hz = STM32_TIMER_DEFAULT_FREQ_HZ;

	if (priv->input_clock_hz < priv->output_freq_hz) {
		ret = -EINVAL;
		goto error;
	}

	prescaler = (priv->input_clock_hz / priv->output_freq_hz) - 1;

	priv->htim.Init.Prescaler = prescaler;
	priv->htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	priv->htim.Init.Period = STM32_TIMER_DEFAULT_PERIOD;
	priv->htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	priv->htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	ret = HAL_TIM_Base_Init(&priv->htim);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	priv->direction = CAPI_TIMER_COUNT_UP;
	priv->counter_min = 0;
	priv->counter_max = priv->htim.Init.Period;
	priv->rollover = true;

	ret = register_timer_handle(&priv->htim, priv);
	if (ret)
		goto error_deinit;

	*handle = tim_handle;
	return 0;

error_deinit:
	HAL_TIM_Base_DeInit(&priv->htim);
error:
	if (tim_handle->init_allocated) {
		capi_free(priv);
		capi_free(tim_handle);
	}
	return ret;
}

/**
 * @brief Deinitialize the STM32 timer.
 * @param handle - Pointer to the timer handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_deinit(struct capi_timer_handle *handle)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle)
		return -EINVAL;

	priv = handle->priv;
	if (priv) {
		HAL_TIM_Base_Stop_IT(&priv->htim);
		HAL_TIM_Base_DeInit(&priv->htim);
		unregister_timer_handle(&priv->htim);
	}

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Start the timer counter.
 * @param handle - Pointer to the timer handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_start(struct capi_timer_handle *handle)
{
	struct stm32_capi_timer_priv *priv;
	HAL_StatusTypeDef ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	if (priv->irq_num)
		ret = HAL_TIM_Base_Start_IT(&priv->htim);
	else
		ret = HAL_TIM_Base_Start(&priv->htim);

	return (ret == HAL_OK) ? 0 : -EIO;
}

/**
 * @brief Stop the timer counter.
 * @param handle - Pointer to the timer handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_stop(struct capi_timer_handle *handle)
{
	struct stm32_capi_timer_priv *priv;
	HAL_StatusTypeDef ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	if (priv->irq_num)
		ret = HAL_TIM_Base_Stop_IT(&priv->htim);
	else
		ret = HAL_TIM_Base_Stop(&priv->htim);

	return (ret == HAL_OK) ? 0 : -EIO;
}

/**
 * @brief Configure the timer counter.
 * @param handle - Pointer to the timer handle.
 * @param config - Pointer to the counter configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_counter_config(struct capi_timer_handle *handle,
		const struct capi_timer_counter_config *config)
{
	struct stm32_capi_timer_priv *priv;
	HAL_StatusTypeDef ret;

	if (!handle || !handle->priv || !config)
		return -EINVAL;

	priv = handle->priv;

	priv->direction = config->direction;
	priv->counter_min = config->min;
	priv->counter_max = config->max;
	priv->rollover = config->rollover;

	if (config->direction == CAPI_TIMER_COUNT_DOWN)
		priv->htim.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	else
		priv->htim.Init.CounterMode = TIM_COUNTERMODE_UP;

	priv->htim.Init.Period = config->max - config->min;

	ret = HAL_TIM_Base_Init(&priv->htim);
	if (ret != HAL_OK)
		return -EIO;

	__HAL_TIM_SET_COUNTER(&priv->htim, config->min);

	return 0;
}

/**
 * @brief Get the current counter value.
 * @param handle - Pointer to the timer handle.
 * @param counter - Pointer to store the counter value.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_counter_get(struct capi_timer_handle *handle,
					uint32_t *counter)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv || !counter)
		return -EINVAL;

	priv = handle->priv;
	*counter = __HAL_TIM_GET_COUNTER(&priv->htim) + priv->counter_min;

	return 0;
}

/**
 * @brief Enable timer event IRQ.
 * @param handle - Pointer to the timer handle.
 * @param event - Event type identifier.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_event_irq_enable(struct capi_timer_handle *handle,
		uint32_t event)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	switch (event) {
	case CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW:
		__HAL_TIM_ENABLE_IT(&priv->htim, TIM_IT_UPDATE);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Disable timer event IRQ.
 * @param handle - Pointer to the timer handle.
 * @param event - Event type identifier.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_event_irq_disable(struct capi_timer_handle *handle,
		uint32_t event)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	switch (event) {
	case CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW:
		__HAL_TIM_DISABLE_IT(&priv->htim, TIM_IT_UPDATE);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Register a global timer event callback.
 * @param handle - Pointer to the timer handle.
 * @param callback - Callback function.
 * @param callback_arg - Callback argument.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_register_event_callback(
	struct capi_timer_handle *handle,
	capi_timer_event_callback callback,
	void *callback_arg)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	priv->event_callback = callback;
	priv->event_callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Initialize a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_init(struct capi_timer_handle *handle,
		uint32_t chan)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	priv->channels[chan].enabled = false;
	priv->channels[chan].mode = CAPI_TIMER_COMPARE_MODE;
	priv->channels[chan].callback = NULL;
	priv->channels[chan].callback_arg = NULL;

	return 0;
}

/**
 * @brief Deinitialize a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_deinit(struct capi_timer_handle *handle,
		uint32_t chan)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t hal_chan;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	hal_chan = get_hal_channel(chan);

	switch (priv->channels[chan].mode) {
	case CAPI_TIMER_COMPARE_MODE:
		HAL_TIM_OC_Stop(&priv->htim, hal_chan);
		break;
	case CAPI_TIMER_CAPTURE_MODE:
		HAL_TIM_IC_Stop(&priv->htim, hal_chan);
		break;
	case CAPI_TIMER_PWM_MODE:
		HAL_TIM_PWM_Stop(&priv->htim, hal_chan);
		break;
	default:
		break;
	}

	priv->channels[chan].enabled = false;
	priv->channels[chan].callback = NULL;
	priv->channels[chan].callback_arg = NULL;

	return 0;
}

/**
 * @brief Configure a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @param ch_config - Pointer to the channel configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_config(
	struct capi_timer_handle *handle,
	uint32_t chan,
	const struct capi_timer_channel_config *ch_config)
{
	struct stm32_capi_timer_priv *priv;
	TIM_OC_InitTypeDef oc_config = {0};
	TIM_IC_InitTypeDef ic_config = {0};
	uint32_t hal_chan;
	HAL_StatusTypeDef ret;

	if (!handle || !handle->priv || !ch_config)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	hal_chan = get_hal_channel(chan);
	priv->channels[chan].mode = ch_config->mode;

	switch (ch_config->mode) {
	case CAPI_TIMER_COMPARE_MODE:
		switch (ch_config->config.compare.polarity) {
		case CAPI_TIMER_ON_COMPARE_TOGGLE:
			oc_config.OCMode = TIM_OCMODE_TOGGLE;
			break;
		case CAPI_TIMER_ON_COMPARE_ACTIVE:
			oc_config.OCMode = TIM_OCMODE_ACTIVE;
			break;
		case CAPI_TIMER_ON_COMPARE_INACTIVE:
			oc_config.OCMode = TIM_OCMODE_INACTIVE;
			break;
		case CAPI_TIMER_ON_COMPARE_PULSE:
			oc_config.OCMode = TIM_OCMODE_PWM1;
			break;
		case CAPI_TIMER_ON_COMPARE_KEEP:
		default:
			oc_config.OCMode = TIM_OCMODE_TIMING;
			break;
		}
		oc_config.Pulse = ch_config->config.compare.match_value;
		oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
		oc_config.OCFastMode = TIM_OCFAST_DISABLE;

		ret = HAL_TIM_OC_ConfigChannel(&priv->htim, &oc_config, hal_chan);
		if (ret != HAL_OK)
			return -EIO;
		break;

	case CAPI_TIMER_CAPTURE_MODE:
		switch (ch_config->config.capture.edge) {
		case CAPI_TIMER_CAPTURE_RISING:
			ic_config.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
			break;
		case CAPI_TIMER_CAPTURE_FALLING:
			ic_config.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
			break;
		case CAPI_TIMER_CAPTURE_BOTH:
			ic_config.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
			break;
		default:
			ic_config.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
			break;
		}
		ic_config.ICSelection = TIM_ICSELECTION_DIRECTTI;
		ic_config.ICPrescaler = TIM_ICPSC_DIV1;
		ic_config.ICFilter = 0;

		ret = HAL_TIM_IC_ConfigChannel(&priv->htim, &ic_config, hal_chan);
		if (ret != HAL_OK)
			return -EIO;
		break;

	case CAPI_TIMER_PWM_MODE:
		oc_config.OCMode = TIM_OCMODE_PWM1;
		if (ch_config->config.pwm.period_ns > 0) {
			uint64_t period_ticks = (priv->output_freq_hz *
						 ch_config->config.pwm.period_ns) / NSEC_PER_SEC;
			uint64_t active_ticks = (priv->output_freq_hz *
						 ch_config->config.pwm.active_ns) / NSEC_PER_SEC;

			priv->htim.Init.Period = (uint32_t)period_ticks;
			HAL_TIM_Base_Init(&priv->htim);
			oc_config.Pulse = (uint32_t)active_ticks;
		} else {
			oc_config.Pulse = 0;
		}

		if (ch_config->config.pwm.inverted_polarity)
			oc_config.OCPolarity = TIM_OCPOLARITY_LOW;
		else
			oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
		oc_config.OCFastMode = TIM_OCFAST_DISABLE;

		ret = HAL_TIM_PWM_ConfigChannel(&priv->htim, &oc_config, hal_chan);
		if (ret != HAL_OK)
			return -EIO;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Enable a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_enable(struct capi_timer_handle *handle,
		uint32_t chan)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t hal_chan;
	HAL_StatusTypeDef ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	hal_chan = get_hal_channel(chan);

	switch (priv->channels[chan].mode) {
	case CAPI_TIMER_COMPARE_MODE:
		ret = HAL_TIM_OC_Start(&priv->htim, hal_chan);
		break;
	case CAPI_TIMER_CAPTURE_MODE:
		ret = HAL_TIM_IC_Start(&priv->htim, hal_chan);
		break;
	case CAPI_TIMER_PWM_MODE:
		ret = HAL_TIM_PWM_Start(&priv->htim, hal_chan);
		break;
	default:
		return -EINVAL;
	}

	if (ret != HAL_OK)
		return -EIO;

	priv->channels[chan].enabled = true;
	return 0;
}

/**
 * @brief Disable a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_disable(struct capi_timer_handle *handle,
		uint32_t chan)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t hal_chan;
	HAL_StatusTypeDef ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	hal_chan = get_hal_channel(chan);

	switch (priv->channels[chan].mode) {
	case CAPI_TIMER_COMPARE_MODE:
		ret = HAL_TIM_OC_Stop(&priv->htim, hal_chan);
		break;
	case CAPI_TIMER_CAPTURE_MODE:
		ret = HAL_TIM_IC_Stop(&priv->htim, hal_chan);
		break;
	case CAPI_TIMER_PWM_MODE:
		ret = HAL_TIM_PWM_Stop(&priv->htim, hal_chan);
		break;
	default:
		return -EINVAL;
	}

	if (ret != HAL_OK)
		return -EIO;

	priv->channels[chan].enabled = false;
	return 0;
}

/**
 * @brief Set compare value for a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @param compare - Compare value to set.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_compare_set(struct capi_timer_handle
		*handle,
		uint32_t chan, uint32_t compare)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t hal_chan;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	hal_chan = get_hal_channel(chan);

	__HAL_TIM_SET_COMPARE(&priv->htim, hal_chan, compare);

	return 0;
}

/**
 * @brief Get compare value from a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @param compare - Pointer to store compare value.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_compare_get(struct capi_timer_handle
		*handle,
		uint32_t chan, uint32_t *compare)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t hal_chan;

	if (!handle || !handle->priv || !compare)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	hal_chan = get_hal_channel(chan);

	*compare = __HAL_TIM_GET_COMPARE(&priv->htim, hal_chan);

	return 0;
}

/**
 * @brief Get capture value from a timer channel.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @param capture - Pointer to store capture value.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_capture_get(struct capi_timer_handle
		*handle,
		uint32_t chan, uint32_t *capture)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t hal_chan;

	if (!handle || !handle->priv || !capture)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	hal_chan = get_hal_channel(chan);

	*capture = HAL_TIM_ReadCapturedValue(&priv->htim, hal_chan);

	return 0;
}

/**
 * @brief Enable channel IRQ.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @param event - Event type identifier.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_irq_enable(struct capi_timer_handle *handle,
		uint32_t chan, uint32_t event)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t it_flag;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	it_flag = get_hal_it_flag(chan);

	__HAL_TIM_ENABLE_IT(&priv->htim, it_flag);

	return 0;
}

/**
 * @brief Disable channel IRQ.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @param event - Event type identifier.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_irq_disable(struct capi_timer_handle
		*handle,
		uint32_t chan, uint32_t event)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t it_flag;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	it_flag = get_hal_it_flag(chan);

	__HAL_TIM_DISABLE_IT(&priv->htim, it_flag);

	return 0;
}

/**
 * @brief Register a channel-specific callback.
 * @param handle - Pointer to the timer handle.
 * @param chan - Channel number (0-3).
 * @param callback - Callback function.
 * @param callback_arg - Callback argument.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_channel_register_callback(
	struct capi_timer_handle *handle,
	uint32_t chan,
	capi_timer_channel_callback callback,
	void *callback_arg)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan >= STM32_TIMER_MAX_CHANNELS)
		return -EINVAL;

	priv = handle->priv;
	priv->channels[chan].callback = callback;
	priv->channels[chan].callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Check if any timer IRQ is pending.
 * @param handle - Pointer to the timer handle.
 * @param pending - Pointer to store pending status.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_is_irq_pending(struct capi_timer_handle *handle,
		bool *pending)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t sr;

	if (!handle || !handle->priv || !pending)
		return -EINVAL;

	priv = handle->priv;
	sr = priv->htim.Instance->SR;

	*pending = (sr & (TIM_SR_UIF | TIM_SR_CC1IF | TIM_SR_CC2IF |
			  TIM_SR_CC3IF | TIM_SR_CC4IF)) != 0;

	return 0;
}

/**
 * @brief Timer interrupt service routine.
 * @param handle - Pointer to the timer handle.
 */
static void stm32_capi_timer_isr(struct capi_timer_handle *handle)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t sr;
	uint32_t i;

	if (!handle || !handle->priv)
		return;

	priv = handle->priv;
	sr = priv->htim.Instance->SR;

	if (sr & TIM_SR_UIF) {
		__HAL_TIM_CLEAR_FLAG(&priv->htim, TIM_FLAG_UPDATE);
		if (priv->event_callback) {
			priv->event_callback(CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW,
					     priv->event_callback_arg, 0);
		}
	}

	for (i = 0; i < STM32_TIMER_MAX_CHANNELS; i++) {
		uint32_t cc_flag = TIM_SR_CC1IF << i;
		if (sr & cc_flag) {
			__HAL_TIM_CLEAR_FLAG(&priv->htim, TIM_FLAG_CC1 << i);
			if (priv->channels[i].callback) {
				uint32_t event =
					(priv->channels[i].mode == CAPI_TIMER_CAPTURE_MODE) ?
					CAPI_TIMER_CHANNEL_EVENT_CAPTURE :
					CAPI_TIMER_CHANNEL_EVENT_COMPARE;
				priv->channels[i].callback(event, i,
							   priv->channels[i].callback_arg, 0);
			}
		}
	}
}

/**
 * @brief Convert nanoseconds to timer ticks.
 * @param handle - Pointer to the timer handle.
 * @param duration_ns - Duration in nanoseconds.
 * @param ticks - Pointer to store tick count.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_nsec_to_ticks(const struct capi_timer_handle
		*handle,
		uint64_t duration_ns, uint32_t *ticks)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv || !ticks)
		return -EINVAL;

	priv = handle->priv;

	*ticks = (uint32_t)((priv->output_freq_hz * duration_ns) / NSEC_PER_SEC);

	return 0;
}

/**
 * @brief Convert timer ticks to nanoseconds.
 * @param handle - Pointer to the timer handle.
 * @param ticks - Tick count.
 * @param duration_ns - Pointer to store duration in nanoseconds.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_timer_ticks_to_nsec(const struct capi_timer_handle
		*handle,
		uint64_t ticks, uint32_t *duration_ns)
{
	struct stm32_capi_timer_priv *priv;

	if (!handle || !handle->priv || !duration_ns)
		return -EINVAL;

	priv = handle->priv;

	if (priv->output_freq_hz == 0)
		return -EINVAL;

	*duration_ns = (uint32_t)((ticks * NSEC_PER_SEC) / priv->output_freq_hz);

	return 0;
}

const struct capi_timer_ops stm32_capi_timer_ops = {
	.init = stm32_capi_timer_init,
	.deinit = stm32_capi_timer_deinit,
	.start = stm32_capi_timer_start,
	.stop = stm32_capi_timer_stop,
	.counter_config = stm32_capi_timer_counter_config,
	.counter_get = stm32_capi_timer_counter_get,
	.event_irq_enable = stm32_capi_timer_event_irq_enable,
	.event_irq_disable = stm32_capi_timer_event_irq_disable,
	.register_event_callback = stm32_capi_timer_register_event_callback,
	.channel_init = stm32_capi_timer_channel_init,
	.channel_deinit = stm32_capi_timer_channel_deinit,
	.channel_config = stm32_capi_timer_channel_config,
	.channel_enable = stm32_capi_timer_channel_enable,
	.channel_disable = stm32_capi_timer_channel_disable,
	.channel_compare_set = stm32_capi_timer_channel_compare_set,
	.channel_compare_get = stm32_capi_timer_channel_compare_get,
	.channel_capture_get = stm32_capi_timer_channel_capture_get,
	.channel_irq_enable = stm32_capi_timer_channel_irq_enable,
	.channel_irq_disable = stm32_capi_timer_channel_irq_disable,
	.channel_register_callback = stm32_capi_timer_channel_register_callback,
	.is_irq_pending = stm32_capi_timer_is_irq_pending,
	.isr = stm32_capi_timer_isr,
	.nsec_to_ticks = stm32_capi_timer_nsec_to_ticks,
	.ticks_to_nsec = stm32_capi_timer_ticks_to_nsec,
};

/**
 * @brief HAL timer period elapsed callback (overflow).
 * @param htim - Pointer to the HAL timer handle.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	struct stm32_capi_timer_priv *priv;

	priv = find_priv_from_htim(htim);
	if (!priv)
		return;

	if (priv->event_callback) {
		priv->event_callback(CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW,
				     priv->event_callback_arg, 0);
	}
}

/**
 * @brief HAL timer output compare delay elapsed callback.
 * @param htim - Pointer to the HAL timer handle.
 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t chan;

	priv = find_priv_from_htim(htim);
	if (!priv)
		return;

	for (chan = 0; chan < STM32_TIMER_MAX_CHANNELS; chan++) {
		if (priv->channels[chan].callback &&
		    priv->channels[chan].mode == CAPI_TIMER_COMPARE_MODE) {
			priv->channels[chan].callback(CAPI_TIMER_CHANNEL_EVENT_COMPARE,
						      chan,
						      priv->channels[chan].callback_arg,
						      0);
		}
	}
}

/**
 * @brief HAL timer input capture callback.
 * @param htim - Pointer to the HAL timer handle.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t chan;

	priv = find_priv_from_htim(htim);
	if (!priv)
		return;

	for (chan = 0; chan < STM32_TIMER_MAX_CHANNELS; chan++) {
		uint32_t hal_chan = get_hal_channel(chan);
		if (htim->Channel == (1 << (hal_chan / 4))) {
			if (priv->channels[chan].callback &&
			    priv->channels[chan].mode == CAPI_TIMER_CAPTURE_MODE) {
				priv->channels[chan].callback(
					CAPI_TIMER_CHANNEL_EVENT_CAPTURE,
					chan,
					priv->channels[chan].callback_arg,
					0);
			}
			break;
		}
	}
}

/**
 * @brief HAL timer PWM pulse finished callback.
 * @param htim - Pointer to the HAL timer handle.
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	struct stm32_capi_timer_priv *priv;
	uint32_t chan;

	priv = find_priv_from_htim(htim);
	if (!priv)
		return;

	for (chan = 0; chan < STM32_TIMER_MAX_CHANNELS; chan++) {
		if (priv->channels[chan].callback &&
		    priv->channels[chan].mode == CAPI_TIMER_PWM_MODE) {
			priv->channels[chan].callback(CAPI_TIMER_CHANNEL_EVENT_COMPARE,
						      chan,
						      priv->channels[chan].callback_arg,
						      0);
		}
	}
}

#endif /* HAL_TIM_MODULE_ENABLED */

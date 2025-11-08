/***************************************************************************//**
 *   @file   maxim_timer.c
 *   @brief  Source file of the timer driver for the maxim platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "maxim_timer.h"
#include "capi_timer.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "tmr.h"
#include "mxc_sys.h"

#define SOURCE_CLOCK_FREQ       8000000
#define MAX_TIMER_INSTANCES     4
#define MAX_TIMER_CHANNELS      4

/**
 * @brief Convert clock division value to prescaler enum
 * @param div Clock division value (should be power of 2)
 * @param prescaler Output prescaler enum value
 * @return 0 on success, negative error code otherwise
 */
static int get_prescaler_from_div(uint32_t div, mxc_tmr_pres_t *prescaler)
{
	if (div > 4096)
		return -EINVAL;

	if (div < 2)
		*prescaler = TMR_PRES_1;
	else if (div < 4)
		*prescaler = TMR_PRES_2;
	else if (div < 8)
		*prescaler = TMR_PRES_4;
	else if (div < 16)
		*prescaler = TMR_PRES_8;
	else if (div < 32)
		*prescaler = TMR_PRES_16;
	else if (div < 64)
		*prescaler = TMR_PRES_32;
	else if (div < 128)
		*prescaler = TMR_PRES_64;
	else if (div < 256)
		*prescaler = TMR_PRES_128;
	else if (div < 512)
		*prescaler = TMR_PRES_256;
	else if (div < 1024)
		*prescaler = TMR_PRES_512;
	else if (div < 2048)
		*prescaler = TMR_PRES_1024;
	else if (div < 4096)
		*prescaler = TMR_PRES_2048;
	else
		*prescaler = TMR_PRES_4096;

	return 0;
}

/**
 * @brief Calculate optimal prescaler for desired frequency
 * @param input_freq_hz Input clock frequency
 * @param output_freq_hz Desired output frequency
 * @param prescaler Output prescaler value
 * @return 0 on success, negative error code otherwise
 */
static int calculate_prescaler(uint32_t input_freq_hz, uint32_t output_freq_hz,
			       mxc_tmr_pres_t *prescaler)
{
	uint32_t div;

	if (!input_freq_hz || !output_freq_hz || output_freq_hz > input_freq_hz)
		return -EINVAL;

	div = input_freq_hz / output_freq_hz;
	return get_prescaler_from_div(div, prescaler);
}

/**
 * @brief Timer interrupt handler
 */
static void maxim_timer_irq_handler(void *arg)
{
	struct capi_timer_handle *handle = (struct capi_timer_handle *)arg;
	struct maxim_capi_timer_handle *priv;
	uint32_t flags;

	if (!handle || !handle->priv)
		return;

	priv = (struct maxim_capi_timer_handle *)handle->priv;
	flags = MXC_TMR_GetFlags(priv->regs);

	/* Handle timer overflow */
	if (flags & MXC_F_TMR_INTFL_IRQ_A) {
		MXC_TMR_ClearFlags(priv->regs);
		/* Timer overflow - call callbacks for channels that have overflow enabled */
		for (int chan = 0; chan < MAX_TIMER_CHANNELS; chan++) {
			if (priv->callbacks[chan]) {
				priv->callbacks[chan](CAPI_TIMER_EVENT_COUNTER_OVERFLOW,
						     chan, priv->callback_args[chan], 0);
			}
		}
	}

	/* Handle compare/capture events for other channels */
	/* Note: MAX32655 timer implementation would need specific channel interrupt handling */
}

/**
 * @brief Initialize timer CAPI handle
 * @param handle Double pointer to timer handle (allocated if *handle is NULL)
 * @param config Timer configuration
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_init(struct capi_timer_handle **handle,
				 const struct capi_timer_config *config)
{
	struct capi_timer_handle *timer_handle;
	struct maxim_capi_timer_handle *priv;
	uint32_t input_freq;
	mxc_tmr_pres_t prescaler;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MAX_TIMER_INSTANCES)
		return -EINVAL;

	/* Handle dynamic vs static allocation */
	if (!*handle) {
		/* Dynamic allocation */
		timer_handle = no_os_calloc(1, sizeof(struct capi_timer_handle));
		if (!timer_handle)
			return -ENOMEM;

		priv = no_os_calloc(1, sizeof(struct maxim_capi_timer_handle));
		if (!priv) {
			no_os_free(timer_handle);
			return -ENOMEM;
		}

		timer_handle->init_allocated = true;
		timer_handle->priv = priv;
	} else {
		/* Static allocation */
		timer_handle = *handle;
		priv = (struct maxim_capi_timer_handle *)timer_handle->priv;
		if (!priv)
			return -EINVAL;

		/* Clear the private structure */
		memset(priv, 0, sizeof(struct maxim_capi_timer_handle));
		timer_handle->init_allocated = false;
	}

	/* Set up the handle */
	timer_handle->ops = &maxim_capi_timer_ops;

	/* Initialize private data */
	priv->instance = config->identifier;
	priv->regs = MXC_TMR_GET_TMR(config->identifier);

	/* Determine input clock frequency */
	if (config->input_clock_external) {
		input_freq = config->input_clock_hz;
		priv->config.clock = MXC_TMR_EXT_CLK;
	} else {
		input_freq = SOURCE_CLOCK_FREQ;
		priv->config.clock = MXC_TMR_8M_CLK;
	}

	priv->input_clock_hz = input_freq;
	priv->output_freq_hz = config->output_freq_hz;

	/* Calculate prescaler for desired frequency */
	ret = calculate_prescaler(input_freq, config->output_freq_hz, &prescaler);
	if (ret)
		goto cleanup;

	/* Configure timer */
	priv->config.bitMode = TMR_BIT_MODE_32;
	priv->config.mode = TMR_MODE_CONTINUOUS;
	priv->config.pres = prescaler;
	priv->config.cmp_cnt = 0xFFFFFFFF;  /* Maximum count for continuous mode */
	priv->config.pol = 1;

	/* Initialize timer hardware */
	MXC_TMR_Shutdown(priv->regs);
	ret = MXC_TMR_Init(priv->regs, &priv->config, 0);
	if (ret) {
		ret = -EIO;
		goto cleanup;
	}

	*handle = timer_handle;
	return 0;

cleanup:
	if (timer_handle->init_allocated) {
		no_os_free(priv);
		no_os_free(timer_handle);
	}
	return ret;
}

/**
 * @brief Deinitialize timer CAPI handle
 * @param handle Timer handle to deinitialize
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_deinit(struct capi_timer_handle *handle)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Stop and shutdown timer */
	MXC_TMR_Stop(priv->regs);
	MXC_TMR_Shutdown(priv->regs);

	/* Free dynamically allocated memory */
	if (handle->init_allocated) {
		no_os_free(priv);
		no_os_free(handle);
	}

	return 0;
}

/**
 * @brief Start timer counting
 * @param handle Timer handle
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_start(struct capi_timer_handle *handle)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	MXC_TMR_Start(priv->regs);
	return 0;
}

/**
 * @brief Stop timer counting
 * @param handle Timer handle
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_stop(struct capi_timer_handle *handle)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	MXC_TMR_Stop(priv->regs);
	return 0;
}

/**
 * @brief Set timer counter value
 * @param handle Timer handle
 * @param counter Counter value to set
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_counter_set(struct capi_timer_handle *handle,
					uint32_t counter)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	MXC_TMR_SetCount(priv->regs, counter);
	return 0;
}

/**
 * @brief Get timer counter value
 * @param handle Timer handle
 * @param counter Pointer to store counter value
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_counter_get(struct capi_timer_handle *handle,
					uint32_t *counter)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || !counter)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	*counter = MXC_TMR_GetCount(priv->regs);
	return 0;
}

/**
 * @brief Initialize timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_channel_init(struct capi_timer_handle *handle,
					 uint32_t chan)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Clear channel configuration */
	memset(&priv->channels[chan], 0, sizeof(struct capi_timer_channel_config));
	priv->callbacks[chan] = NULL;
	priv->callback_args[chan] = NULL;
	priv->channel_enabled[chan] = false;

	return 0;
}

/**
 * @brief Configure timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @param ch_config Channel configuration
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_channel_config(struct capi_timer_handle *handle,
					   uint32_t chan, void *ch_config)
{
	struct maxim_capi_timer_handle *priv;
	struct capi_timer_channel_config *config;

	if (!handle || !handle->priv || !ch_config || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;
	config = (struct capi_timer_channel_config *)ch_config;

	/* Copy channel configuration */
	memcpy(&priv->channels[chan], config, sizeof(struct capi_timer_channel_config));

	/* Configure channel based on mode */
	switch (config->mode) {
	case CAPI_TIMER_COMPARE_MODE:
		/* Configure compare mode */
		if (config->compare.use_pin) {
			/* Configure GPIO pin for output compare */
			/* Note: Actual GPIO configuration would be done here */
		}
		break;

	case CAPI_TIMER_CAPTURE_MODE:
		/* Configure capture mode */
		if (config->capture.use_pin) {
			/* Configure GPIO pin for input capture */
			/* Note: Actual GPIO configuration would be done here */
		}
		break;

	case CAPI_TIMER_PWM_MODE:
		/* Configure PWM mode */
		/* Note: MAX32655 PWM configuration would be done here */
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Deinitialize timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_channel_deinit(struct capi_timer_handle *handle,
					    uint32_t chan)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Disable channel */
	priv->channel_enabled[chan] = false;
	priv->callbacks[chan] = NULL;
	priv->callback_args[chan] = NULL;

	return 0;
}

/**
 * @brief Enable timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_channel_enable(struct capi_timer_handle *handle,
					   uint32_t chan)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	priv->channel_enabled[chan] = true;
	return 0;
}

/**
 * @brief Disable timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_channel_disable(struct capi_timer_handle *handle,
					    uint32_t chan)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	priv->channel_enabled[chan] = false;
	return 0;
}

/**
 * @brief Enable timer channel interrupt
 * @param handle Timer handle
 * @param chan Channel number
 * @param event Event type
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_irq_enable(struct capi_timer_handle *handle,
				       uint32_t chan, enum capi_timer_event event)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Enable appropriate interrupt based on event type */
	switch (event) {
	case CAPI_TIMER_EVENT_COUNTER_OVERFLOW:
		MXC_TMR_EnableInt(priv->regs);
		break;
	case CAPI_TIMER_EVENT_COMPARE:
		/* Enable compare interrupt for channel */
		break;
	case CAPI_TIMER_EVENT_CAPTURE:
		/* Enable capture interrupt for channel */
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Disable timer channel interrupt
 * @param handle Timer handle
 * @param chan Channel number
 * @param event Event type
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_irq_disable(struct capi_timer_handle *handle,
					uint32_t chan, enum capi_timer_event event)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Disable appropriate interrupt based on event type */
	switch (event) {
	case CAPI_TIMER_EVENT_COUNTER_OVERFLOW:
		MXC_TMR_DisableInt(priv->regs);
		break;
	case CAPI_TIMER_EVENT_COMPARE:
		/* Disable compare interrupt for channel */
		break;
	case CAPI_TIMER_EVENT_CAPTURE:
		/* Disable capture interrupt for channel */
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Register callback for timer channel events
 * @param handle Timer handle
 * @param chan Channel number
 * @param callback Callback function
 * @param callback_arg Callback argument
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_channel_register_callback(struct capi_timer_handle *handle,
						      uint32_t chan,
						      capi_timer_callback *const callback,
						      void *const callback_arg)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	priv->callbacks[chan] = *callback;
	priv->callback_args[chan] = callback_arg;

	return 0;
}

/**
 * @brief Set compare value for timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @param compare Compare value
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_compare_set(struct capi_timer_handle *handle,
					uint32_t chan, uint32_t compare)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Set compare value - MAX32655 specific implementation would go here */
	if (chan == 0) {
		/* For channel 0, use main compare register */
		priv->config.cmp_cnt = compare;
		/* Would need to update the hardware register */
	}

	return 0;
}

/**
 * @brief Get compare value for timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @param compare Pointer to store compare value
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_compare_get(struct capi_timer_handle *handle,
					uint32_t chan, uint32_t *compare)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || !compare || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Get compare value - MAX32655 specific implementation would go here */
	if (chan == 0) {
		*compare = priv->config.cmp_cnt;
	} else {
		*compare = 0;  /* Other channels not implemented yet */
	}

	return 0;
}

/**
 * @brief Get capture value for timer channel
 * @param handle Timer handle
 * @param chan Channel number
 * @param capture Pointer to store capture value
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_timer_capture_get(struct capi_timer_handle *handle,
					uint32_t chan, uint32_t *capture)
{
	struct maxim_capi_timer_handle *priv;

	if (!handle || !handle->priv || !capture || chan >= MAX_TIMER_CHANNELS)
		return -EINVAL;

	priv = (struct maxim_capi_timer_handle *)handle->priv;

	/* Get capture value - MAX32655 specific implementation would go here */
	*capture = MXC_TMR_GetCount(priv->regs);  /* For now, return current count */

	return 0;
}

/**
 * @brief MAX32655 Timer CAPI operations structure
 */
const struct capi_timer_ops maxim_capi_timer_ops = {
	.init = maxim_capi_timer_init,
	.deinit = maxim_capi_timer_deinit,
	.start = maxim_capi_timer_start,
	.stop = maxim_capi_timer_stop,
	.counter_set = maxim_capi_timer_counter_set,
	.counter_get = maxim_capi_timer_counter_get,
	.channel_init = maxim_capi_timer_channel_init,
	.channel_config = maxim_capi_timer_channel_config,
	.channel_deinit = maxim_capi_timer_channel_deinit,
	.channel_enable = maxim_capi_timer_channel_enable,
	.channel_disable = maxim_capi_timer_channel_disable,
	.irq_enable = maxim_capi_timer_irq_enable,
	.irq_disable = maxim_capi_timer_irq_disable,
	.channel_register_callback = maxim_capi_timer_channel_register_callback,
	.compare_set = maxim_capi_timer_compare_set,
	.compare_get = maxim_capi_timer_compare_get,
	.capture_get = maxim_capi_timer_capture_get,
};
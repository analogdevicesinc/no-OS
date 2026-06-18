/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx PS TTC (XTtcPs) driver for CAPI
 *
 * One channel per TTC instance (three TTC units per TTC block on Zynq-7000).
 * capi_irq_init() must be called before IRQ-driven async ops will work.
 */

#include <capi_alloc.h>
#include <string.h>
#include <errno.h>
#include <capi_timer.h>
#include <capi_irq.h>
#include <xilinx_capi_timer_priv.h>
#include <xttcps.h>
#include "xinterrupt_wrap.h"

#define CAPI_TIMER_NSEC_PER_SEC	1000000000ULL

static int capi_timer_ps_ttc_init(struct capi_timer_handle **handle,
				  const struct capi_timer_config *config);
static int capi_timer_ps_ttc_deinit(struct capi_timer_handle *handle);
static int capi_timer_ps_ttc_start(struct capi_timer_handle *handle);
static int capi_timer_ps_ttc_stop(struct capi_timer_handle *handle);
static int capi_timer_ps_ttc_counter_config(struct capi_timer_handle *handle,
		const struct capi_timer_counter_config *config);
static int capi_timer_ps_ttc_counter_get(struct capi_timer_handle *handle,
		uint32_t *counter);
static int capi_timer_ps_ttc_event_irq_enable(struct capi_timer_handle *handle,
		uint32_t event);
static int capi_timer_ps_ttc_event_irq_disable(struct capi_timer_handle *handle,
		uint32_t event);
static int capi_timer_ps_ttc_register_event_callback(struct capi_timer_handle
		*handle,
		capi_timer_event_callback callback,
		void *callback_arg);
static int capi_timer_ps_ttc_channel_init(struct capi_timer_handle *handle,
		uint32_t chan);
static int capi_timer_ps_ttc_channel_deinit(struct capi_timer_handle *handle,
		uint32_t chan);
static int capi_timer_ps_ttc_channel_config(struct capi_timer_handle *handle,
		uint32_t chan,
		const struct capi_timer_channel_config *ch_config);
static int capi_timer_ps_ttc_channel_enable(struct capi_timer_handle *handle,
		uint32_t chan);
static int capi_timer_ps_ttc_channel_disable(struct capi_timer_handle *handle,
		uint32_t chan);
static int capi_timer_ps_ttc_channel_compare_set(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t compare);
static int capi_timer_ps_ttc_channel_compare_get(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t *compare);
static int capi_timer_ps_ttc_channel_capture_get(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t *capture);
static int capi_timer_ps_ttc_channel_irq_enable(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t event);
static int capi_timer_ps_ttc_channel_irq_disable(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t event);
static int capi_timer_ps_ttc_channel_register_callback(struct capi_timer_handle
		*handle,
		uint32_t chan,
		capi_timer_channel_callback callback,
		void *callback_arg);
static int capi_timer_ps_ttc_is_irq_pending(struct capi_timer_handle *handle,
		bool *pending);
static void capi_timer_ps_ttc_isr(struct capi_timer_handle *handle);
static int capi_timer_ps_ttc_nsec_to_ticks(const struct capi_timer_handle
		*handle,
		uint64_t duration_ns, uint32_t *ticks);
static int capi_timer_ps_ttc_ticks_to_nsec(const struct capi_timer_handle
		*handle, uint64_t ticks,
		uint32_t *duration_ns);

#define PS_TTC_NUM_CHANNELS 1
#define PS_TTC_PRESCALER 0U
#define PS_TTC_CLOCK_DIVISOR (2U << PS_TTC_PRESCALER)

struct capi_timer_ops capi_timer_xilinx_ps_ttc_ops = {
	.init = capi_timer_ps_ttc_init,
	.deinit = capi_timer_ps_ttc_deinit,
	.start = capi_timer_ps_ttc_start,
	.stop = capi_timer_ps_ttc_stop,
	.counter_config = capi_timer_ps_ttc_counter_config,
	.counter_get = capi_timer_ps_ttc_counter_get,
	.event_irq_enable = capi_timer_ps_ttc_event_irq_enable,
	.event_irq_disable = capi_timer_ps_ttc_event_irq_disable,
	.register_event_callback = capi_timer_ps_ttc_register_event_callback,
	.channel_init = capi_timer_ps_ttc_channel_init,
	.channel_deinit = capi_timer_ps_ttc_channel_deinit,
	.channel_config = capi_timer_ps_ttc_channel_config,
	.channel_enable = capi_timer_ps_ttc_channel_enable,
	.channel_disable = capi_timer_ps_ttc_channel_disable,
	.channel_compare_set = capi_timer_ps_ttc_channel_compare_set,
	.channel_compare_get = capi_timer_ps_ttc_channel_compare_get,
	.channel_capture_get = capi_timer_ps_ttc_channel_capture_get,
	.channel_irq_enable = capi_timer_ps_ttc_channel_irq_enable,
	.channel_irq_disable = capi_timer_ps_ttc_channel_irq_disable,
	.channel_register_callback = capi_timer_ps_ttc_channel_register_callback,
	.is_irq_pending = capi_timer_ps_ttc_is_irq_pending,
	.isr = capi_timer_ps_ttc_isr,
	.nsec_to_ticks = capi_timer_ps_ttc_nsec_to_ticks,
	.ticks_to_nsec = capi_timer_ps_ttc_ticks_to_nsec,
};

static XTtcPs *inst(struct capi_timer_xilinx_handle *xh)
{
	return (XTtcPs *)xh->instance;
}

static void xilinx_timer_free_allocated_handle(struct capi_timer_handle
		**handle)
{
	if (handle == NULL || *handle == NULL)
		return;

	capi_free((*handle)->priv);
	capi_free(*handle);
	*handle = NULL;
}

static void xilinx_timer_clear_app_handle(struct capi_timer_handle *handle)
{
	if (handle != NULL)
		handle->ops = NULL;
}

static int check_timer_config(const struct capi_timer_config *config)
{
	if (config->input_clock_identifier != 0)
		return -ENOTSUP;

	return 0;
}

static int check_compare_config(const struct capi_timer_compare_config *config)
{
	if (config->generate_pulse_on_match || config->output_identifier != 0 ||
	    config->polarity != CAPI_TIMER_ON_COMPARE_KEEP || config->stop_enabled ||
	    config->extra != NULL)
		return -ENOTSUP;

	return 0;
}

static int check_pwm_config(const struct capi_timer_pwm_config *config)
{
	if (config->output_identifier != 0 || config->offset_ns != 0
	    || config->extra != NULL)
		return -ENOTSUP;

	return 0;
}

static void irq_handler(void *ref)
{
	capi_timer_ps_ttc_isr((struct capi_timer_handle *)ref);
}

/**
 * @brief Initialize the CAPI timer backend instance.
 * @note PS: XTtcPs_LookupConfig()/XTtcPs_CfgInitialize().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_init(struct capi_timer_handle **handle,
				  const struct capi_timer_config *config)
{
	if (!handle || !config)
		return -EINVAL;
	if (check_timer_config(config) != 0)
		return -ENOTSUP;

	bool alloc_handle = (*handle == NULL);
	struct capi_timer_handle *h = *handle;
	struct capi_timer_xilinx_handle *xh;

	if (alloc_handle) {
		h = capi_malloc(sizeof(*h));
		if (!h)
			return -ENOMEM;
		memset(h, 0, sizeof(*h));

		xh = capi_malloc(sizeof(*xh));
		if (!xh) {
			capi_free(h);
			return -ENOMEM;
		}
		h->priv = xh;
		*handle = h;
	} else {
		xh = h->priv;
		if (!xh)
			return -EINVAL;
	}

	memset(xh, 0, sizeof(*xh));
	h->init_allocated = alloc_handle;
	h->ops = config->ops ? config->ops : &capi_timer_xilinx_ps_ttc_ops;
	h->priv = xh;
	xh->input_clock_hz = (uint32_t)config->input_clock_hz;

	XTtcPs *xi = capi_malloc(sizeof(XTtcPs));
	if (!xi) {
		if (alloc_handle)
			xilinx_timer_free_allocated_handle(handle);
		else
			xilinx_timer_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(xi, 0, sizeof(XTtcPs));

	XTtcPs_Config *cfg = XTtcPs_LookupConfig((u32)config->identifier);
	if (!cfg) {
		capi_free(xi);
		if (alloc_handle)
			xilinx_timer_free_allocated_handle(handle);
		else
			xilinx_timer_clear_app_handle(h);
		return -ENODEV;
	}

	int status = XTtcPs_CfgInitialize(xi, cfg, cfg->BaseAddress);
	if (status != XST_SUCCESS) {
		capi_free(xi);
		if (alloc_handle)
			xilinx_timer_free_allocated_handle(handle);
		else
			xilinx_timer_clear_app_handle(h);
		return -EIO;
	}

	XTtcPs_Stop(xi);
	XTtcPs_SetPrescaler(xi, PS_TTC_PRESCALER);

	xh->instance = xi;
	xh->input_clock_hz = cfg->InputClockHz / PS_TTC_CLOCK_DIVISOR;
	xh->num_channels = PS_TTC_NUM_CHANNELS;

	const struct capi_timer_xilinx_config *xcfg =
		(const struct capi_timer_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, irq_handler, h);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
			if (ret == 0) {
				xh->irq_id = irq_id;
				xh->use_irq = true;
			}
		}
	}

	return 0;
}

/**
 * @brief Deinitialize the CAPI timer backend instance.
 * @note PS: XTtcPs_Stop() then capi_free.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_deinit(struct capi_timer_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;
	if (!xh)
		return -EINVAL;

	if (xh->use_irq) {
		capi_irq_disable(xh->irq_id);
		xh->use_irq = false;
	}

	if (xh->instance) {
		XTtcPs_Stop(inst(xh));
		capi_free(xh->instance);
		xh->instance = NULL;
	}

	if (handle->init_allocated) {
		capi_free(xh);
		capi_free(handle);
	} else {
		handle->ops = NULL;
	}

	return 0;
}

/**
 * @brief Start the timer counter.
 * @note PS: XTtcPs_Start().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_start(struct capi_timer_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;
	XTtcPs_Start(inst(xh));

	return 0;
}

/**
 * @brief Stop the timer counter.
 * @note PS: XTtcPs_Stop().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_stop(struct capi_timer_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;
	XTtcPs_Stop(inst(xh));

	return 0;
}

/**
 * @brief Configure the timer counter.
 * @note PS: XTtcPs_SetInterval()/XTtcPs_ResetCounterValue().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_counter_config(struct capi_timer_handle *handle,
		const struct capi_timer_counter_config *config)
{
	if (!handle || !config)
		return -EINVAL;
	if (config->extra != NULL)
		return -ENOTSUP;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (config->direction != CAPI_TIMER_COUNT_UP)
		return -ENOTSUP;
	if (config->min != 0)
		return -ENOTSUP;
	if (!config->rollover)
		return -ENOTSUP;
	if ((uint64_t)config->max > (uint64_t)XTTCPS_MAX_INTERVAL_COUNT)
		return -EINVAL;
	XTtcPs_SetInterval(inst(xh), (XInterval)config->max);
	XTtcPs_ResetCounterValue(inst(xh));

	return 0;
}

/**
 * @brief Read the timer counter value.
 * @note PS: XTtcPs_GetCounterValue().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_counter_get(struct capi_timer_handle *handle,
		uint32_t *counter)
{
	if (!handle || !counter)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	*counter = XTtcPs_GetCounterValue(inst(xh));

	return 0;
}

/**
 * @brief Enable timer global event interrupts.
 * @note PS: XTtcPs_EnableInterrupts().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_event_irq_enable(struct capi_timer_handle *handle,
		uint32_t event)
{
	if (!handle)
		return -EINVAL;

	if (event != CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;
	if (!xh->use_irq)
		return -ENOTSUP;

	XTtcPs_EnableInterrupts(inst(xh), XTTCPS_IXR_CNT_OVR_MASK);
	xh->event_irq_enabled = true;

	return 0;
}

/**
 * @brief Disable timer global event interrupts.
 * @note PS: XTtcPs_DisableInterrupts().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_event_irq_disable(struct capi_timer_handle *handle,
		uint32_t event)
{
	if (!handle)
		return -EINVAL;

	if (event != CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	xh->event_irq_enabled = false;
	XTtcPs_DisableInterrupts(inst(xh), XTTCPS_IXR_CNT_OVR_MASK);

	return 0;
}

/**
 * @brief Register the timer global event callback.
 * @note PS: Stores overflow callback.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_register_event_callback(struct capi_timer_handle
		*handle,
		capi_timer_event_callback callback,
		void *callback_arg)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	xh->event_callback = callback;
	xh->event_callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Initialize the CAPI timer backend instance.
 * @note PS: Marks channel initialized.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_init(struct capi_timer_handle *handle,
		uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	xh->channels[0].initialized = true;
	xh->channels[0].enabled = false;
	xh->channels[0].mode = CAPI_TIMER_COMPARE_MODE;

	return 0;
}

/**
 * @brief Deinitialize the CAPI timer backend instance.
 * @note PS: Disables and clears channel state.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_deinit(struct capi_timer_handle *handle,
		uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	capi_timer_ps_ttc_channel_disable(handle, 0);
	memset(&xh->channels[0], 0, sizeof(struct capi_timer_xilinx_channel));

	return 0;
}

/**
 * @brief Configure a timer channel.
 * @note PS: XTtcPs_SetOptions()/XTtcPs_SetInterval().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_config(struct capi_timer_handle *handle,
		uint32_t chan,
		const struct capi_timer_channel_config *ch_config)
{
	if (!handle || !ch_config)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;
	if (ch_config->extra != NULL)
		return -ENOTSUP;

	u32 options = XTTCPS_OPTION_WAVE_DISABLE;

	switch (ch_config->mode) {
	case CAPI_TIMER_COMPARE_MODE:
		if (check_compare_config(&ch_config->config.compare) != 0)
			return -ENOTSUP;
		if ((uint64_t)ch_config->config.compare.match_value >
		    (uint64_t)XTTCPS_MAX_INTERVAL_COUNT)
			return -EINVAL;
		options |= XTTCPS_OPTION_INTERVAL_MODE;
		break;

	case CAPI_TIMER_PWM_MODE: {
		if (check_pwm_config(&ch_config->config.pwm) != 0)
			return -ENOTSUP;
		if (ch_config->config.pwm.period_ns == 0)
			return -EINVAL;
		if (ch_config->config.pwm.active_ns > ch_config->config.pwm.period_ns)
			return -EINVAL;
		if (xh->input_clock_hz > 0) {
			uint32_t period_ticks;
			uint32_t duty_ticks;
			int ret = capi_timer_ps_ttc_nsec_to_ticks(handle,
					ch_config->config.pwm.period_ns, &period_ticks);
			if (ret != 0)
				return -EINVAL;
			ret = capi_timer_ps_ttc_nsec_to_ticks(handle,
							      ch_config->config.pwm.active_ns, &duty_ticks);
			if (ret != 0)
				return -EINVAL;
			if (period_ticks == 0 ||
			    (uint64_t)period_ticks > (uint64_t)XTTCPS_MAX_INTERVAL_COUNT ||
			    (uint64_t)duty_ticks > (uint64_t)XTTCPS_MAX_INTERVAL_COUNT)
				return -EINVAL;
			options = XTTCPS_OPTION_MATCH_MODE;
			if (ch_config->config.pwm.inverted_polarity)
				options |= XTTCPS_OPTION_WAVE_POLARITY;
			XTtcPs_SetOptions(inst(xh), options);
			XTtcPs_SetInterval(inst(xh), (XInterval)period_ticks);
			XTtcPs_SetMatchValue(inst(xh), 0, (XMatchRegValue)duty_ticks);
			xh->channels[0].mode = CAPI_TIMER_PWM_MODE;
			return 0;
		}
		options = XTTCPS_OPTION_MATCH_MODE;
		if (ch_config->config.pwm.inverted_polarity)
			options |= XTTCPS_OPTION_WAVE_POLARITY;
		break;
	}

	case CAPI_TIMER_CAPTURE_MODE:
		return -ENOTSUP;

	default:
		return -EINVAL;
	}

	XTtcPs_SetOptions(inst(xh), options);

	if (ch_config->mode == CAPI_TIMER_COMPARE_MODE)
		XTtcPs_SetInterval(inst(xh), (XInterval)ch_config->config.compare.match_value);

	xh->channels[0].mode = ch_config->mode;

	return 0;
}

/**
 * @brief Enable a timer channel.
 * @note PS: XTtcPs_Start().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_enable(struct capi_timer_handle *handle,
		uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	xh->channels[0].enabled = true;
	XTtcPs_Start(inst(xh));

	return 0;
}

/**
 * @brief Disable a timer channel.
 * @note PS: XTtcPs_Stop().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_disable(struct capi_timer_handle *handle,
		uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	xh->channels[0].enabled = false;
	XTtcPs_Stop(inst(xh));

	return 0;
}

/**
 * @brief Set a timer channel compare value.
 * @note PS: XTtcPs_SetOptions()/XTtcPs_SetInterval().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_compare_set(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t compare)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;
	if ((uint64_t)compare > (uint64_t)XTTCPS_MAX_INTERVAL_COUNT)
		return -EINVAL;

	u32 options = XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE;
	XTtcPs_SetOptions(inst(xh), options);
	XTtcPs_SetInterval(inst(xh), (XInterval)compare);

	return 0;
}

/**
 * @brief Get a timer channel compare value.
 * @note PS: XTtcPs_GetInterval().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_compare_get(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t *compare)
{
	if (!handle || !compare)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	*compare = (uint32_t)XTtcPs_GetInterval(inst(xh));

	return 0;
}

/**
 * @brief Get a timer channel capture value.
 * @note PS: Returns -ENOTSUP.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_capture_get(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t *capture)
{
	(void)handle;
	(void)chan;
	(void)capture;
	return -ENOTSUP;
}

/**
 * @brief Enable timer channel interrupts.
 * @note PS: XTtcPs_EnableInterrupts().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_irq_enable(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t event)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	if (event != CAPI_TIMER_CHANNEL_EVENT_COMPARE)
		return -EINVAL;

	if (!xh->use_irq)
		return -ENOTSUP;

	xh->channels[0].use_irq = true;
	XTtcPs_EnableInterrupts(inst(xh), XTTCPS_IXR_INTERVAL_MASK);

	return 0;
}

/**
 * @brief Disable timer channel interrupts.
 * @note PS: XTtcPs_DisableInterrupts().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_irq_disable(struct capi_timer_handle
		*handle, uint32_t chan,
		uint32_t event)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	if (event != CAPI_TIMER_CHANNEL_EVENT_COMPARE)
		return -EINVAL;

	xh->channels[0].use_irq = false;
	XTtcPs_DisableInterrupts(inst(xh), XTTCPS_IXR_INTERVAL_MASK);

	return 0;
}

/**
 * @brief Register a timer channel callback.
 * @note PS: Stores channel callback.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_channel_register_callback(struct capi_timer_handle
		*handle,
		uint32_t chan,
		capi_timer_channel_callback callback,
		void *callback_arg)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	xh->channels[0].callback = callback;
	xh->channels[0].callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Check whether a timer interrupt is pending.
 * @note PS: XTtcPs_GetInterruptStatus().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_is_irq_pending(struct capi_timer_handle *handle,
		bool *pending)
{
	if (!handle || !pending)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	u32 status = XTtcPs_GetInterruptStatus(inst(xh));
	*pending = (status != 0);

	return 0;
}

/**
 * @brief Dispatch the timer interrupt handler.
 * @note PS: XTtcPs_Get/ClearInterruptStatus().
 *
 */
static void capi_timer_ps_ttc_isr(struct capi_timer_handle *handle)
{
	if (!handle)
		return;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	uint32_t status = XTtcPs_GetInterruptStatus(inst(xh));
	XTtcPs_ClearInterruptStatus(inst(xh), status);

	if ((status & XTTCPS_IXR_INTERVAL_MASK) && xh->channels[0].use_irq &&
	    xh->channels[0].callback) {
		xh->channels[0].callback(CAPI_TIMER_CHANNEL_EVENT_COMPARE, 0,
					 xh->channels[0].callback_arg, 0);
	}

	if ((status & XTTCPS_IXR_CNT_OVR_MASK) && xh->event_irq_enabled &&
	    xh->event_callback) {
		xh->event_callback(CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW,
				   xh->event_callback_arg,
				   0);
	}
}

/**
 * @brief Convert nanoseconds to timer ticks.
 * @note PS: Converts ns using input clock.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_nsec_to_ticks(const struct capi_timer_handle
		*handle,
		uint64_t duration_ns, uint32_t *ticks)
{
	if (!handle || !ticks)
		return -EINVAL;

	const struct capi_timer_xilinx_handle *xh = handle->priv;

	if (xh->input_clock_hz == 0)
		return -EINVAL;

	uint64_t whole = duration_ns / CAPI_TIMER_NSEC_PER_SEC;
	uint64_t remainder = duration_ns % CAPI_TIMER_NSEC_PER_SEC;

	if (whole > UINT32_MAX / xh->input_clock_hz)
		return -EOVERFLOW;

	uint64_t result = whole * xh->input_clock_hz;
	uint64_t fraction = (remainder * xh->input_clock_hz) /
			    CAPI_TIMER_NSEC_PER_SEC;

	if (fraction > UINT32_MAX - result)
		return -EOVERFLOW;

	result += fraction;
	*ticks = (uint32_t)result;
	return 0;
}

/**
 * @brief Convert timer ticks to nanoseconds.
 * @note PS: Converts ticks using input clock.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_ps_ttc_ticks_to_nsec(const struct capi_timer_handle
		*handle, uint64_t ticks,
		uint32_t *duration_ns)
{
	if (!handle || !duration_ns)
		return -EINVAL;

	const struct capi_timer_xilinx_handle *xh = handle->priv;

	if (xh->input_clock_hz == 0)
		return -EINVAL;

	uint64_t whole = ticks / xh->input_clock_hz;
	uint64_t remainder = ticks % xh->input_clock_hz;

	if (whole > UINT32_MAX / CAPI_TIMER_NSEC_PER_SEC)
		return -EOVERFLOW;

	uint64_t result = whole * CAPI_TIMER_NSEC_PER_SEC;
	uint64_t fraction = (remainder * CAPI_TIMER_NSEC_PER_SEC) /
			    xh->input_clock_hz;

	if (fraction > UINT32_MAX - result)
		return -EOVERFLOW;

	result += fraction;
	*duration_ns = (uint32_t)result;
	return 0;
}

/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx AXI Timer (XTmrCtr) driver for CAPI
 *
 * Two channels per instance.
 * capi_irq_init() must be called before IRQ-driven async ops will work.
 */

#include <capi_alloc.h>
#include <string.h>
#include <errno.h>
#include <capi_timer.h>
#include <capi_irq.h>
#include <xilinx_capi_timer_priv.h>
#include <xtmrctr.h>
#include <xtmrctr_l.h>
#include "xinterrupt_wrap.h"

#define CAPI_TIMER_NSEC_PER_SEC	1000000000ULL

static int capi_timer_pl_init(struct capi_timer_handle **handle,
			      const struct capi_timer_config *config);
static int capi_timer_pl_deinit(struct capi_timer_handle *handle);
static int capi_timer_pl_start(struct capi_timer_handle *handle);
static int capi_timer_pl_stop(struct capi_timer_handle *handle);
static int capi_timer_pl_counter_config(struct capi_timer_handle *handle,
					const struct capi_timer_counter_config *config);
static int capi_timer_pl_counter_get(struct capi_timer_handle *handle,
				     uint32_t *counter);
static int capi_timer_pl_event_irq_enable(struct capi_timer_handle *handle,
		uint32_t event);
static int capi_timer_pl_event_irq_disable(struct capi_timer_handle *handle,
		uint32_t event);
static int capi_timer_pl_register_event_callback(struct capi_timer_handle
		*handle,
		capi_timer_event_callback callback,
		void *callback_arg);
static int capi_timer_pl_channel_init(struct capi_timer_handle *handle,
				      uint32_t chan);
static int capi_timer_pl_channel_deinit(struct capi_timer_handle *handle,
					uint32_t chan);
static int capi_timer_pl_channel_config(struct capi_timer_handle *handle,
					uint32_t chan,
					const struct capi_timer_channel_config *ch_config);
static int capi_timer_pl_channel_enable(struct capi_timer_handle *handle,
					uint32_t chan);
static int capi_timer_pl_channel_disable(struct capi_timer_handle *handle,
		uint32_t chan);
static int capi_timer_pl_channel_compare_set(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t compare);
static int capi_timer_pl_channel_compare_get(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t *compare);
static int capi_timer_pl_channel_capture_get(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t *capture);
static int capi_timer_pl_channel_irq_enable(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t event);
static int capi_timer_pl_channel_irq_disable(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t event);
static int capi_timer_pl_channel_register_callback(struct capi_timer_handle
		*handle, uint32_t chan,
		capi_timer_channel_callback callback,
		void *callback_arg);
static int capi_timer_pl_is_irq_pending(struct capi_timer_handle *handle,
					bool *pending);
static void capi_timer_pl_isr(struct capi_timer_handle *handle);
static int capi_timer_pl_nsec_to_ticks(const struct capi_timer_handle *handle,
				       uint64_t duration_ns,
				       uint32_t *ticks);
static int capi_timer_pl_ticks_to_nsec(const struct capi_timer_handle *handle,
				       uint64_t ticks,
				       uint32_t *duration_ns);

#define PL_TIMER_NUM_CHANNELS 2

struct capi_timer_ops capi_timer_xilinx_pl_ops = {
	.init = capi_timer_pl_init,
	.deinit = capi_timer_pl_deinit,
	.start = capi_timer_pl_start,
	.stop = capi_timer_pl_stop,
	.counter_config = capi_timer_pl_counter_config,
	.counter_get = capi_timer_pl_counter_get,
	.event_irq_enable = capi_timer_pl_event_irq_enable,
	.event_irq_disable = capi_timer_pl_event_irq_disable,
	.register_event_callback = capi_timer_pl_register_event_callback,
	.channel_init = capi_timer_pl_channel_init,
	.channel_deinit = capi_timer_pl_channel_deinit,
	.channel_config = capi_timer_pl_channel_config,
	.channel_enable = capi_timer_pl_channel_enable,
	.channel_disable = capi_timer_pl_channel_disable,
	.channel_compare_set = capi_timer_pl_channel_compare_set,
	.channel_compare_get = capi_timer_pl_channel_compare_get,
	.channel_capture_get = capi_timer_pl_channel_capture_get,
	.channel_irq_enable = capi_timer_pl_channel_irq_enable,
	.channel_irq_disable = capi_timer_pl_channel_irq_disable,
	.channel_register_callback = capi_timer_pl_channel_register_callback,
	.is_irq_pending = capi_timer_pl_is_irq_pending,
	.isr = capi_timer_pl_isr,
	.nsec_to_ticks = capi_timer_pl_nsec_to_ticks,
	.ticks_to_nsec = capi_timer_pl_ticks_to_nsec,
};

static XTmrCtr *inst(struct capi_timer_xilinx_handle *xh)
{
	return (XTmrCtr *)xh->instance;
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

static int check_capture_config(const struct capi_timer_capture_config *config)
{
	if (config->input_identifier != 0 || config->edge != CAPI_TIMER_CAPTURE_BOTH ||
	    config->extra != NULL)
		return -ENOTSUP;

	return 0;
}

static int check_pwm_config(const struct capi_timer_pwm_config *config)
{
	if (config->output_identifier != 0 || config->inverted_polarity
	    || config->offset_ns != 0 ||
	    config->extra != NULL)
		return -ENOTSUP;

	return 0;
}

static void axi_timer_isr_handler(void *ref, u8 timer_number)
{
	struct capi_timer_xilinx_handle *xh = (struct capi_timer_xilinx_handle *)ref;

	if (timer_number < xh->num_channels && xh->channels[timer_number].use_irq &&
	    xh->channels[timer_number].callback) {
		xh->channels[timer_number].callback(CAPI_TIMER_CHANNEL_EVENT_COMPARE,
						    timer_number,
						    xh->channels[timer_number].callback_arg, 0);
	}

	if (timer_number == 0 && xh->event_irq_enabled && xh->event_callback) {
		xh->event_callback(CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW,
				   xh->event_callback_arg,
				   0);
	}
}

static void irq_handler(void *ref)
{
	capi_timer_pl_isr((struct capi_timer_handle *)ref);
}

/**
 * @brief Initialize the CAPI timer backend instance.
 * @note PL: XTmrCtr_CfgInitialize()/XTmrCtr_InitHw().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_init(struct capi_timer_handle **handle,
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
	h->ops = config->ops ? config->ops : &capi_timer_xilinx_pl_ops;
	h->priv = xh;
	xh->input_clock_hz = (uint32_t)config->input_clock_hz;

	XTmrCtr *xi = capi_malloc(sizeof(XTmrCtr));
	if (!xi) {
		if (alloc_handle)
			xilinx_timer_free_allocated_handle(handle);
		else
			xilinx_timer_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(xi, 0, sizeof(XTmrCtr));

	XTmrCtr_Config *cfg = XTmrCtr_LookupConfig((UINTPTR)config->identifier);
	if (!cfg) {
		capi_free(xi);
		if (alloc_handle)
			xilinx_timer_free_allocated_handle(handle);
		else
			xilinx_timer_clear_app_handle(h);
		return -ENODEV;
	}

	XTmrCtr_CfgInitialize(xi, cfg, cfg->BaseAddress);

	int status = XTmrCtr_InitHw(xi);
	if (status != XST_SUCCESS) {
		capi_free(xi);
		if (alloc_handle)
			xilinx_timer_free_allocated_handle(handle);
		else
			xilinx_timer_clear_app_handle(h);
		return -EIO;
	}

	for (u8 n = 0; n < 2; n++) {
		u32 csr = XTmrCtr_ReadReg(cfg->BaseAddress, n, XTC_TCSR_OFFSET);
		if (csr & XTC_CSR_INT_OCCURED_MASK)
			XTmrCtr_WriteReg(cfg->BaseAddress, n, XTC_TCSR_OFFSET,
					 csr | XTC_CSR_INT_OCCURED_MASK);
	}

	XTmrCtr_SetHandler(xi, axi_timer_isr_handler, xh);

	xh->instance = xi;
	if (cfg->SysClockFreqHz)
		xh->input_clock_hz = cfg->SysClockFreqHz;
	else if (xh->input_clock_hz != 0)
		xi->Config.SysClockFreqHz = xh->input_clock_hz;
	xh->num_channels = PL_TIMER_NUM_CHANNELS;

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
 * @note PL: XTmrCtr_PwmDisable()/XTmrCtr_Stop() then capi_free.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_deinit(struct capi_timer_handle *handle)
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
		XTmrCtr_PwmDisable(inst(xh));
		XTmrCtr_Stop(inst(xh), 0);
		XTmrCtr_Stop(inst(xh), 1);
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
 * @note PL: XTmrCtr_PwmEnable() for PWM, XTmrCtr_Start() otherwise.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_start(struct capi_timer_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (xh->channels[0].mode == CAPI_TIMER_PWM_MODE && xh->channels[0].enabled) {
		XTmrCtr_PwmEnable(inst(xh));
		return 0;
	}

	for (uint8_t i = 0; i < xh->num_channels; i++) {
		if (xh->channels[i].initialized && xh->channels[i].enabled)
			XTmrCtr_Start(inst(xh), i);
	}

	return 0;
}

/**
 * @brief Stop the timer counter.
 * @note PL: XTmrCtr_PwmDisable() for PWM, XTmrCtr_Stop() otherwise.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_stop(struct capi_timer_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (xh->channels[0].mode == CAPI_TIMER_PWM_MODE) {
		XTmrCtr_PwmDisable(inst(xh));
		return 0;
	}

	for (uint8_t i = 0; i < xh->num_channels; i++)
		XTmrCtr_Stop(inst(xh), i);

	return 0;
}

/**
 * @brief Configure the timer counter.
 * @note PL: XTmrCtr_SetResetValue()/XTmrCtr_Reset().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_counter_config(struct capi_timer_handle *handle,
					const struct capi_timer_counter_config *config)
{
	if (!handle || !config)
		return -EINVAL;
	if (config->extra != NULL)
		return -ENOTSUP;

	if (config->min != 0)
		return -ENOTSUP;
	if (!config->rollover)
		return -ENOTSUP;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	/*
	 * Map the simple "counter" API onto AXI-timer channel 0. XTmrCtr counts UP
	 * by default and DOWN only when XTC_DOWN_COUNT_OPTION is set; rollover==true
	 * maps to auto-reload. Up: load 0 and run to 2^32. Down: load max, run to 0.
	 */
	u32 options = XTC_AUTO_RELOAD_OPTION;
	u32 reset_value;

	switch (config->direction) {
	case CAPI_TIMER_COUNT_UP:
		reset_value = 0;
		break;
	case CAPI_TIMER_COUNT_DOWN:
		options |= XTC_DOWN_COUNT_OPTION;
		reset_value = config->max;
		break;
	default:
		return -ENOTSUP;
	}

	XTmrCtr_SetOptions(inst(xh), 0, options);
	XTmrCtr_SetResetValue(inst(xh), 0, reset_value);
	XTmrCtr_Reset(inst(xh), 0);

	/*
	 * The counter API bypasses channel_init()/channel_enable(), but
	 * capi_timer_pl_start() only starts channels flagged initialized+enabled.
	 * Flag channel 0 here so start() actually runs the timer (matches how the
	 * PS TTC/SCU backends make the counter API self-contained).
	 */
	xh->channels[0].initialized = true;
	xh->channels[0].enabled = true;
	xh->channels[0].mode = CAPI_TIMER_COMPARE_MODE;

	return 0;
}

/**
 * @brief Read the timer counter value.
 * @note PL: XTmrCtr_GetValue().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_counter_get(struct capi_timer_handle *handle,
				     uint32_t *counter)
{
	if (!handle || !counter)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	*counter = XTmrCtr_GetValue(inst(xh), 0);

	return 0;
}

/**
 * @brief Enable timer global event interrupts.
 * @note PL: Enables XTC_INT_MODE_OPTION.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_event_irq_enable(struct capi_timer_handle *handle,
		uint32_t event)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (event != CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW)
		return -EINVAL;

	if (!xh->use_irq)
		return -ENOTSUP;

	u32 options = XTmrCtr_GetOptions(inst(xh), 0);
	options |= XTC_INT_MODE_OPTION;
	XTmrCtr_SetOptions(inst(xh), 0, options);
	xh->event_irq_enabled = true;

	return 0;
}

/**
 * @brief Disable timer global event interrupts.
 * @note PL: Clears XTC_INT_MODE_OPTION.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_event_irq_disable(struct capi_timer_handle *handle,
		uint32_t event)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (event != CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW)
		return -EINVAL;

	xh->event_irq_enabled = false;
	if (!xh->channels[0].use_irq) {
		u32 options = XTmrCtr_GetOptions(inst(xh), 0);
		options &= ~XTC_INT_MODE_OPTION;
		XTmrCtr_SetOptions(inst(xh), 0, options);
	}

	return 0;
}

/**
 * @brief Register the timer global event callback.
 * @note PL: Stores overflow callback.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_register_event_callback(struct capi_timer_handle
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
 * @note PL: Marks channel initialized.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_init(struct capi_timer_handle *handle,
				      uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	xh->channels[chan].initialized = true;
	xh->channels[chan].enabled = false;
	xh->channels[chan].mode = CAPI_TIMER_COMPARE_MODE;

	return 0;
}

/**
 * @brief Deinitialize the CAPI timer backend instance.
 * @note PL: Disables and clears channel state.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_deinit(struct capi_timer_handle *handle,
					uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	if (xh->channels[chan].mode == CAPI_TIMER_PWM_MODE) {
		if (chan != 0)
			return -EINVAL;
		capi_timer_pl_channel_disable(handle, chan);
		memset(&xh->channels[0], 0, sizeof(struct capi_timer_xilinx_channel));
		memset(&xh->channels[1], 0, sizeof(struct capi_timer_xilinx_channel));
		return 0;
	}

	capi_timer_pl_channel_disable(handle, chan);
	memset(&xh->channels[chan], 0, sizeof(struct capi_timer_xilinx_channel));

	return 0;
}

/**
 * @brief Configure a timer channel.
 * @note PL: XTmrCtr_PwmConfigure() for PWM, XTmrCtr_SetOptions()/SetResetValue()
 * otherwise.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_config(struct capi_timer_handle *handle,
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

	u32 options = 0;

	switch (ch_config->mode) {
	case CAPI_TIMER_COMPARE_MODE:
		if (check_compare_config(&ch_config->config.compare) != 0)
			return -ENOTSUP;
		options = XTC_DOWN_COUNT_OPTION;
		break;

	case CAPI_TIMER_CAPTURE_MODE:
		if (check_capture_config(&ch_config->config.capture) != 0)
			return -ENOTSUP;
		options = XTC_CAPTURE_MODE_OPTION | XTC_EXT_COMPARE_OPTION;
		break;

	case CAPI_TIMER_PWM_MODE: {
		if (check_pwm_config(&ch_config->config.pwm) != 0)
			return -ENOTSUP;
		if (chan != 0)
			return -EINVAL;

		if (ch_config->config.pwm.active_ns > ch_config->config.pwm.period_ns)
			return -EINVAL;
		if (ch_config->config.pwm.period_ns > UINT32_MAX ||
		    ch_config->config.pwm.active_ns > UINT32_MAX)
			return -EINVAL;
		if (inst(xh)->Config.SysClockFreqHz == 0)
			return -EINVAL;

		uint64_t clock_period_ns =
			(CAPI_TIMER_NSEC_PER_SEC +
			 (inst(xh)->Config.SysClockFreqHz / 2U)) /
			inst(xh)->Config.SysClockFreqHz;
		/*
		 * XTmrCtr_PwmConfigure() uses a period-load formula with a
		 * documented two-clock adjustment; shorter periods would
		 * underflow that path.
		 */
		if (clock_period_ns == 0 ||
		    ch_config->config.pwm.period_ns < (2ULL * clock_period_ns) ||
		    ch_config->config.pwm.active_ns < (2ULL * clock_period_ns))
			return -EINVAL;

		/* Match embeddedsw PWM load math: load = rounded_period_ticks - 2. */
		uint64_t period_load =
			((ch_config->config.pwm.period_ns + (clock_period_ns / 2ULL)) /
			 clock_period_ns) -
			2ULL;
		if (period_load > XTC_MAX_LOAD_VALUE)
			return -EINVAL;

		XTmrCtr_PwmDisable(inst(xh));
		(void)XTmrCtr_PwmConfigure(inst(xh), (u32)ch_config->config.pwm.period_ns,
					   (u32)ch_config->config.pwm.active_ns);

		xh->channels[0].mode = CAPI_TIMER_PWM_MODE;
		xh->channels[1].mode = CAPI_TIMER_PWM_MODE;
		return 0;
	}

	default:
		return -EINVAL;
	}

	XTmrCtr_SetOptions(inst(xh), (u8)chan, options);

	if (ch_config->mode == CAPI_TIMER_COMPARE_MODE) {
		XTmrCtr_SetResetValue(inst(xh), (u8)chan,
				      ch_config->config.compare.match_value);
	}

	xh->channels[chan].mode = ch_config->mode;

	return 0;
}

/**
 * @brief Enable a timer channel.
 * @note PL: XTmrCtr_PwmEnable() for PWM, XTmrCtr_Start() otherwise.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_enable(struct capi_timer_handle *handle,
					uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	if (xh->channels[chan].mode == CAPI_TIMER_PWM_MODE) {
		if (chan != 0)
			return -EINVAL;
		xh->channels[0].enabled = true;
		xh->channels[1].enabled = true;
		XTmrCtr_PwmEnable(inst(xh));
		return 0;
	}

	xh->channels[chan].enabled = true;
	XTmrCtr_Start(inst(xh), (u8)chan);

	return 0;
}

/**
 * @brief Disable a timer channel.
 * @note PL: XTmrCtr_PwmDisable() for PWM, XTmrCtr_Stop() otherwise.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_disable(struct capi_timer_handle *handle,
		uint32_t chan)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	if (xh->channels[chan].mode == CAPI_TIMER_PWM_MODE) {
		if (chan != 0)
			return -EINVAL;
		xh->channels[0].enabled = false;
		xh->channels[1].enabled = false;
		XTmrCtr_PwmDisable(inst(xh));
		return 0;
	}

	xh->channels[chan].enabled = false;
	XTmrCtr_Stop(inst(xh), (u8)chan);

	return 0;
}

/**
 * @brief Set a timer channel compare value.
 * @note PL: XTmrCtr_SetResetValue()/SetOptions().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_compare_set(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t compare)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	XTmrCtr_SetResetValue(inst(xh), (u8)chan, compare);

	u32 options = XTmrCtr_GetOptions(inst(xh), (u8)chan);
	options |= XTC_AUTO_RELOAD_OPTION | XTC_DOWN_COUNT_OPTION;
	XTmrCtr_SetOptions(inst(xh), (u8)chan, options);

	return 0;
}

/**
 * @brief Get a timer channel compare value.
 * @note PL: XTmrCtr_ReadReg(XTC_TLR_OFFSET).
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_compare_get(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t *compare)
{
	if (!handle || !compare)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	*compare = XTmrCtr_ReadReg(inst(xh)->BaseAddress, (u8)chan, XTC_TLR_OFFSET);

	return 0;
}

/**
 * @brief Get a timer channel capture value.
 * @note PL: XTmrCtr_GetCaptureValue().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_capture_get(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t *capture)
{
	if (!handle || !capture)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	*capture = XTmrCtr_GetCaptureValue(inst(xh), (u8)chan);

	return 0;
}

/**
 * @brief Enable timer channel interrupts.
 * @note PL: Enables XTC_INT_MODE_OPTION.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_irq_enable(struct capi_timer_handle *handle,
		uint32_t chan,
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

	xh->channels[chan].use_irq = true;

	u32 options = XTmrCtr_GetOptions(inst(xh), (u8)chan);
	options |= XTC_INT_MODE_OPTION;
	XTmrCtr_SetOptions(inst(xh), (u8)chan, options);

	return 0;
}

/**
 * @brief Disable timer channel interrupts.
 * @note PL: Clears XTC_INT_MODE_OPTION.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_irq_disable(struct capi_timer_handle *handle,
		uint32_t chan,
		uint32_t event)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	if (event != CAPI_TIMER_CHANNEL_EVENT_COMPARE)
		return -EINVAL;

	xh->channels[chan].use_irq = false;

	if (chan != 0 || !xh->event_irq_enabled) {
		u32 options = XTmrCtr_GetOptions(inst(xh), (u8)chan);
		options &= ~XTC_INT_MODE_OPTION;
		XTmrCtr_SetOptions(inst(xh), (u8)chan, options);
	}

	return 0;
}

/**
 * @brief Register a timer channel callback.
 * @note PL: Stores channel callback.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_channel_register_callback(struct capi_timer_handle
		*handle, uint32_t chan,
		capi_timer_channel_callback callback,
		void *callback_arg)
{
	if (!handle)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	if (chan >= xh->num_channels)
		return -EINVAL;

	if (xh->use_irq)
		capi_irq_disable(xh->irq_id);
	xh->channels[chan].callback = callback;
	xh->channels[chan].callback_arg = callback_arg;
	if (xh->use_irq)
		capi_irq_enable(xh->irq_id);

	return 0;
}

/**
 * @brief Check whether a timer interrupt is pending.
 * @note PL: XTmrCtr_IsExpired().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_is_irq_pending(struct capi_timer_handle *handle,
					bool *pending)
{
	if (!handle || !pending)
		return -EINVAL;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	*pending = (XTmrCtr_IsExpired(inst(xh), 0) != 0) ||
		   (XTmrCtr_IsExpired(inst(xh), 1) != 0);

	return 0;
}

/**
 * @brief Dispatch the timer interrupt handler.
 * @note PL: XTmrCtr_InterruptHandler().
 *
 */
static void capi_timer_pl_isr(struct capi_timer_handle *handle)
{
	if (!handle)
		return;

	struct capi_timer_xilinx_handle *xh = handle->priv;

	XTmrCtr_InterruptHandler(inst(xh));
}

/**
 * @brief Convert nanoseconds to timer ticks.
 * @note PL: Converts ns using input clock.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_nsec_to_ticks(const struct capi_timer_handle *handle,
				       uint64_t duration_ns,
				       uint32_t *ticks)
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
 * @note PL: Converts ticks using input clock.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_timer_pl_ticks_to_nsec(const struct capi_timer_handle *handle,
				       uint64_t ticks,
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

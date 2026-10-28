/*******************************************************************************
 *   @file   maxim_capi_rtc.c
 *   @brief  Implementation of RTC functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <string.h>
#include "capi_alloc.h"
#include "capi_irq.h"
#include "capi_rtc.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_rtc.h"
#include "maxim_capi_rtc_priv.h"
#include "max32657.h"
#include "lp.h"
#include "rtc.h"

/** Static variables **********************************************************/

static struct capi_rtc_handle *rtc = NULL;

/** Forward declarations ******************************************************/

static void max_capi_rtc_isr(struct capi_rtc_handle *handle);

/** Helper functions **********************************************************/

/**
 * @brief Convert an MSDK error code to a negative errno value
 * @param ret MSDK error code
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_rtc_err(int ret)
{
	switch (ret) {
	case E_NO_ERROR:
		return 0;
	case E_BUSY:
		return -EBUSY;
	case E_BAD_PARAM:
	case E_NULL_PTR:
		return -EINVAL;
	default:
		return -EIO;
	}
}

/**
 * @brief Check that a handle is the one currently owning the RTC
 * @param handle The RTC handle
 * @return true if the handle is valid, false otherwise
 */
static inline bool _max_capi_rtc_valid(struct capi_rtc_handle *handle)
{
	return handle && handle == rtc;
}

/**
 * @brief Wait until pending writes are synchronized to the RTC clock domain
 */
static inline void _max_capi_rtc_wait_busy(void)
{
	while (MXC_RTC->ctrl & MXC_F_RTC_CTRL_BUSY);
}

/**
 * @brief Mask the RTC IRQ at the NVIC. The ISR, and any callback calling the
 * 	  RTC API from it, updates CTRL with read-modify-write accesses, so
 * 	  thread-context CTRL updates and callback (de)registration are done
 * 	  with the IRQ masked. The NVIC is accessed directly since
 * 	  capi_irq_enable() resets the IRQ priority.
 * @return true if the IRQ was enabled and must be unmasked, false otherwise
 */
static inline bool _max_capi_rtc_irq_mask(void)
{
	bool enabled = NVIC_GetEnableIRQ(RTC_IRQn);

	if (enabled)
		NVIC_DisableIRQ(RTC_IRQn);

	return enabled;
}

/**
 * @brief Restore the RTC IRQ masked by _max_capi_rtc_irq_mask()
 * @param enabled Return value of _max_capi_rtc_irq_mask()
 */
static inline void _max_capi_rtc_irq_unmask(bool enabled)
{
	if (enabled)
		NVIC_EnableIRQ(RTC_IRQn);
}

/**
 * @brief Convert a square-wave frequency to its MSDK equivalent
 * @param freq Square-wave frequency, other than MAX_CAPI_RTC_SQWAVE_DISABLED
 * @param msdk_freq Where to store the MSDK frequency
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_rtc_sqwave_freq(enum max_capi_rtc_sqwave_freq freq,
				     mxc_rtc_freq_sel_t *msdk_freq)
{
	switch (freq) {
	case MAX_CAPI_RTC_SQWAVE_1HZ:
		*msdk_freq = MXC_RTC_F_1HZ;
		return 0;
	case MAX_CAPI_RTC_SQWAVE_512HZ:
		*msdk_freq = MXC_RTC_F_512HZ;
		return 0;
	case MAX_CAPI_RTC_SQWAVE_4KHZ:
		*msdk_freq = MXC_RTC_F_4KHZ;
		return 0;
	case MAX_CAPI_RTC_SQWAVE_32KHZ:
		*msdk_freq = MXC_RTC_F_32KHZ;
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Start the RTC counter, along with the square-wave output if one is
 * 	  configured. The caller must mask the RTC IRQ.
 * @param rtc_priv The private RTC data
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_rtc_run(struct max_capi_rtc_priv *rtc_priv)
{
	mxc_rtc_freq_sel_t msdk_freq;
	int ret;

	_max_capi_rtc_wait_busy();
	if (rtc_priv->sqwave == MAX_CAPI_RTC_SQWAVE_DISABLED)
		return _max_capi_rtc_err(MXC_RTC_Start());

	ret = _max_capi_rtc_sqwave_freq(rtc_priv->sqwave, &msdk_freq);
	if (ret)
		return ret;

	/* Also sets CTRL.EN, starting the counter */
	return _max_capi_rtc_err(MXC_RTC_SquareWaveStart(msdk_freq));
}

/**
 * @brief Disable all alarms and interrupts, clear the alarm flags and stop the
 * 	  square-wave output. The counter itself is left untouched.
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_rtc_quiesce(void)
{
	int ret;

	ret = MXC_RTC_DisableInt(MXC_F_RTC_CTRL_TOD_ALARM_IE |
				 MXC_F_RTC_CTRL_SSEC_ALARM_IE |
				 MXC_F_RTC_CTRL_RDY_IE);
	if (ret)
		return _max_capi_rtc_err(ret);

	MXC_RTC_ClearFlags(MXC_F_RTC_CTRL_TOD_ALARM_IF |
			   MXC_F_RTC_CTRL_SSEC_ALARM_IF);

	_max_capi_rtc_wait_busy();

	return _max_capi_rtc_err(MXC_RTC_SquareWaveStop());
}

/**
 * @brief Load a new seconds count into the RTC, preserving the run state,
 * 	  alarm, interrupt and square-wave configuration.
 * @param sec Seconds count
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_rtc_write_count(uint32_t sec)
{
	bool running;
	int ret;

	_max_capi_rtc_wait_busy();
	running = MXC_RTC->ctrl & MXC_F_RTC_CTRL_EN;
	if (running) {
		ret = MXC_RTC_Stop();
		if (ret)
			return _max_capi_rtc_err(ret);
	}

	_max_capi_rtc_wait_busy();
	MXC_RTC->ctrl |= MXC_F_RTC_CTRL_WR_EN;
	_max_capi_rtc_wait_busy();
	MXC_RTC->ssec = 0;
	_max_capi_rtc_wait_busy();
	MXC_RTC->sec = sec;
	_max_capi_rtc_wait_busy();
	MXC_RTC->ctrl &= ~MXC_F_RTC_CTRL_WR_EN;

	if (running) {
		_max_capi_rtc_wait_busy();
		return _max_capi_rtc_err(MXC_RTC_Start());
	}

	return 0;
}

/**
 * @brief Connect and enable the RTC IRQ. The first call goes through
 * 	  capi_irq; later calls only re-enable the IRQ at the NVIC, so that
 * 	  the priority set after the first connection is preserved.
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_rtc_irq_connect(struct capi_rtc_handle *handle)
{
	struct max_capi_rtc_priv *rtc_priv = handle->priv;
	int ret;

	if (rtc_priv->irq_connected) {
		NVIC_EnableIRQ(RTC_IRQn);
		return 0;
	}

	ret = capi_irq_connect(RTC_IRQn, (capi_isr_callback_t)max_capi_rtc_isr,
			       handle);
	if (ret)
		return ret;

	ret = capi_irq_enable(RTC_IRQn);
	if (ret)
		return ret;

	rtc_priv->irq_connected = true;

	return 0;
}

/**
 * @brief Disable the RTC IRQ at the NVIC if no event, alarm callback or
 * 	  wake-up requires it anymore. The IRQ stays connected and enabled in
 * 	  capi_irq, since capi_irq_enable() would reset its priority on the
 * 	  next connection.
 * @param handle The RTC handle
 */
static void _max_capi_rtc_irq_release(struct capi_rtc_handle *handle)
{
	struct max_capi_rtc_priv *rtc_priv = handle->priv;
	int i;

	if (!rtc_priv->irq_connected || rtc_priv->irq_events ||
	    rtc_priv->wakeup_enabled)
		return;

	for (i = 0; i < MAX_CAPI_RTC_NUM_ALARMS; i++)
		if (rtc_priv->alarm_callback[i])
			return;

	NVIC_DisableIRQ(RTC_IRQn);
}

/**
 * @brief Check whether the ISR latched an alarm that no callback consumed
 * @param rtc_priv The private RTC data
 * @return true if any alarm is latched, false otherwise
 */
static bool _max_capi_rtc_alarm_latched(struct max_capi_rtc_priv *rtc_priv)
{
	int i;

	for (i = 0; i < MAX_CAPI_RTC_NUM_ALARMS; i++)
		if (rtc_priv->alarm_pending[i])
			return true;

	return false;
}

/** RTC functions implementations *********************************************/

/**
 * @brief Initialize the RTC. All alarms and interrupts are disabled. Unless
 * 	  CAPI_RTC_INIT_FLAG_PRESERVE_COUNT is set, the counter is also
 * 	  stopped, loaded with initial_count, and the trim is reset to 0;
 * 	  otherwise the counter value, run state and trim are kept.
 * 	  The square-wave output selected through the optional
 * 	  struct max_capi_rtc_extra in config->extra is only driven while the
 * 	  counter runs, so it is enabled here only if the counter is
 * 	  preserved running, and by capi_rtc_start() otherwise.
 * @param handle A pointer to the RTC handle pointer
 * @param config A pointer to the configuration struct
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_init(struct capi_rtc_handle **handle,
			     const struct capi_rtc_config *config)
{
	const struct max_capi_rtc_extra *extra;
	enum max_capi_rtc_sqwave_freq sqwave;
	struct capi_rtc_handle *rtc_handle;
	struct max_capi_rtc_priv *rtc_priv;
	mxc_rtc_freq_sel_t msdk_freq;
	bool irq_enabled;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier != 0)
		return -EINVAL;

	if (config->tick.tick_freq_hz != 0 &&
	    config->tick.tick_freq_hz != MAX_CAPI_RTC_TICK_FREQ_HZ)
		return -EINVAL;

	extra = config->extra;
	sqwave = extra ? extra->sqwave : MAX_CAPI_RTC_SQWAVE_DISABLED;
	if (sqwave != MAX_CAPI_RTC_SQWAVE_DISABLED &&
	    _max_capi_rtc_sqwave_freq(sqwave, &msdk_freq))
		return -EINVAL;

	if (rtc != NULL)
		return -EBUSY;

	if (*handle == NULL) {
		rtc_handle = capi_calloc(1, sizeof(*rtc_handle));
		if (!rtc_handle)
			return -ENOMEM;

		rtc_priv = capi_calloc(1, sizeof(*rtc_priv));
		if (!rtc_priv) {
			capi_free(rtc_handle);
			return -ENOMEM;
		}

		rtc_handle->priv = rtc_priv;
		rtc_handle->init_allocated = true;
	} else {
		rtc_handle = *handle;

		if (!rtc_handle->priv)
			return -EINVAL;

		rtc_priv = rtc_handle->priv;
		memset(rtc_priv, 0, sizeof(*rtc_priv));

		rtc_handle->init_allocated = false;
	}

	ret = _max_capi_rtc_quiesce();
	if (ret)
		goto free_handle;

	if (!(config->flags & CAPI_RTC_INIT_FLAG_PRESERVE_COUNT)) {
		_max_capi_rtc_wait_busy();
		ret = MXC_RTC_Init(config->initial_count, 0);
		if (ret) {
			ret = _max_capi_rtc_err(ret);
			goto free_handle;
		}

		_max_capi_rtc_wait_busy();
		ret = MXC_RTC_Trim(0);
		if (ret) {
			ret = _max_capi_rtc_err(ret);
			goto free_handle;
		}
	}

	rtc_priv->sqwave = sqwave;

	/* Only reachable with CAPI_RTC_INIT_FLAG_PRESERVE_COUNT */
	_max_capi_rtc_wait_busy();
	if (sqwave != MAX_CAPI_RTC_SQWAVE_DISABLED &&
	    (MXC_RTC->ctrl & MXC_F_RTC_CTRL_EN)) {
		irq_enabled = _max_capi_rtc_irq_mask();
		ret = _max_capi_rtc_run(rtc_priv);
		_max_capi_rtc_irq_unmask(irq_enabled);
		if (ret)
			goto free_handle;
	}

	rtc_priv->callback = config->callback;
	rtc_priv->arg = config->arg;

	rtc_handle->ops = config->ops;

	rtc = rtc_handle;
	*handle = rtc_handle;

	return 0;

free_handle:
	if (rtc_handle->init_allocated) {
		capi_free(rtc_priv);
		capi_free(rtc_handle);
	}

	return ret;
}

/**
 * @brief Deinitialize the RTC. Disables all alarms, interrupts, the alarm
 * 	  wake-up and the square-wave output, and stops the counter. May be
 * 	  called from an RTC callback.
 * @param handle The RTC handle pointer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_deinit(struct capi_rtc_handle *handle)
{
	struct max_capi_rtc_priv *rtc_priv;
	int stop_ret;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	/* Disabled first so the ISR cannot race the CTRL updates below */
	if (rtc_priv->irq_connected)
		capi_irq_disable(RTC_IRQn);

	MXC_LP_DisableRTCAlarmWakeup();

	ret = _max_capi_rtc_quiesce();

	_max_capi_rtc_wait_busy();
	stop_ret = _max_capi_rtc_err(MXC_RTC_Stop());
	if (!ret)
		ret = stop_ret;

	if (handle->init_allocated) {
		capi_free(rtc_priv);
		capi_free(handle);
	}

	rtc = NULL;

	return ret;
}

/**
 * @brief Start the RTC counter, and the square-wave output if configured
 * @param handle The RTC handle pointer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_start(struct capi_rtc_handle *handle)
{
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	irq_enabled = _max_capi_rtc_irq_mask();
	ret = _max_capi_rtc_run(handle->priv);
	_max_capi_rtc_irq_unmask(irq_enabled);

	return ret;
}

/**
 * @brief Stop the RTC counter, and the square-wave output if configured
 * @param handle The RTC handle pointer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_stop(struct capi_rtc_handle *handle)
{
	struct max_capi_rtc_priv *rtc_priv;
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	irq_enabled = _max_capi_rtc_irq_mask();
	_max_capi_rtc_wait_busy();
	ret = MXC_RTC_Stop();
	if (!ret && rtc_priv->sqwave != MAX_CAPI_RTC_SQWAVE_DISABLED) {
		_max_capi_rtc_wait_busy();
		ret = MXC_RTC_SquareWaveStop();
	}
	_max_capi_rtc_irq_unmask(irq_enabled);

	return _max_capi_rtc_err(ret);
}

/**
 * @brief Get the current value of the RTC (seconds). Blocks until any
 * 	  pending register write is synchronized to the RTC clock domain.
 * 	  SEC is read until two consecutive reads match, so that a value
 * 	  sampled while the counter updates is never returned.
 * @param handle The RTC handle pointer
 * @param count The pointer to store the value to
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_get_count(struct capi_rtc_handle *handle,
				  uint32_t *count)
{
	uint32_t sec;

	if (!_max_capi_rtc_valid(handle) || !count)
		return -EINVAL;

	_max_capi_rtc_wait_busy();
	do {
		sec = MXC_RTC->sec;
	} while (sec != MXC_RTC->sec);

	*count = sec;

	return 0;
}

/**
 * @brief Set the current value of the RTC (seconds). The sub-second counter
 * 	  is reset to 0. Blocks until the value is synchronized to the RTC
 * 	  clock domain.
 * @param handle The RTC handle pointer
 * @param count The value to set
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_set_count(struct capi_rtc_handle *handle,
				  uint32_t count)
{
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	irq_enabled = _max_capi_rtc_irq_mask();
	ret = _max_capi_rtc_write_count(count);
	_max_capi_rtc_irq_unmask(irq_enabled);

	return ret;
}

/**
 * @brief Set the tick-rate configuration of the RTC. The MAX32657 RTC counter
 * 	  has a fixed 1 Hz tick, so only 0 (default) and 1 Hz are accepted.
 * @param handle The RTC handle pointer
 * @param config The tick-rate configuration pointer to set
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_set_tick_config(struct capi_rtc_handle *handle,
					const struct capi_rtc_tick_config *config)
{
	if (!_max_capi_rtc_valid(handle) || !config)
		return -EINVAL;

	if (config->tick_freq_hz != 0 &&
	    config->tick_freq_hz != MAX_CAPI_RTC_TICK_FREQ_HZ)
		return -EINVAL;

	return 0;
}

/**
 * @brief Get the current tick-rate configuration of the RTC
 * @param handle The RTC handle pointer
 * @param config The pointer to store the tick-rate configuration to
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_get_tick_config(struct capi_rtc_handle *handle,
					struct capi_rtc_tick_config *config)
{
	if (!_max_capi_rtc_valid(handle) || !config)
		return -EINVAL;

	config->tick_freq_hz = MAX_CAPI_RTC_TICK_FREQ_HZ;
	config->extra = NULL;

	return 0;
}

/**
 * @brief Get the tick frequency of the RTC
 * @param handle The RTC handle pointer
 * @param freq_hz The tick frequency in Hz
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_get_tick_freq(struct capi_rtc_handle *handle,
				      uint32_t *freq_hz)
{
	if (!_max_capi_rtc_valid(handle) || !freq_hz)
		return -EINVAL;

	*freq_hz = MAX_CAPI_RTC_TICK_FREQ_HZ;

	return 0;
}

/**
 * @brief Set and enable an alarm for the RTC. The alarm is described by a
 * 	  struct max_capi_rtc_alarm_extra passed through config->extra; the
 * 	  time and mask fields are ignored.
 * @param handle The RTC handle pointer
 * @param id Alarm channel index (enum max_capi_rtc_alarm_id)
 * @param config The alarm configuration pointer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_alarm_set(struct capi_rtc_handle *handle, uint16_t id,
				  const struct capi_rtc_alarm_config *config)
{
	const struct max_capi_rtc_alarm_extra *alarm_extra;
	struct max_capi_rtc_priv *rtc_priv;
	uint32_t alarm_ie;
	uint32_t alarm_if;
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle) || !config || !config->extra)
		return -EINVAL;

	if (id >= MAX_CAPI_RTC_NUM_ALARMS)
		return -EINVAL;

	rtc_priv = handle->priv;
	alarm_extra = config->extra;

	if (id == MAX_CAPI_RTC_ALARM_SSEC && alarm_extra->value == 0)
		return -EINVAL;

	alarm_ie = id == MAX_CAPI_RTC_ALARM_TOD ? MXC_F_RTC_CTRL_TOD_ALARM_IE :
		   MXC_F_RTC_CTRL_SSEC_ALARM_IE;
	alarm_if = id == MAX_CAPI_RTC_ALARM_TOD ? MXC_F_RTC_CTRL_TOD_ALARM_IF :
		   MXC_F_RTC_CTRL_SSEC_ALARM_IF;

	irq_enabled = _max_capi_rtc_irq_mask();

	/* The alarm registers can only be written while the alarm is disabled */
	ret = MXC_RTC_DisableInt(alarm_ie);
	if (ret)
		goto unmask;

	if (id == MAX_CAPI_RTC_ALARM_TOD)
		ret = MXC_RTC_SetTimeofdayAlarm(alarm_extra->value &
						MXC_F_RTC_TODA_TOD_ALARM);
	else
		/* The SSEC alarm counts up from the register value until it overflows */
		ret = MXC_RTC_SetSubsecondAlarm(0U - alarm_extra->value);
	if (ret)
		goto unmask;

	MXC_RTC_ClearFlags(alarm_if);
	rtc_priv->alarm_pending[id] = false;

	ret = MXC_RTC_EnableInt(alarm_ie);

unmask:
	_max_capi_rtc_irq_unmask(irq_enabled);

	return _max_capi_rtc_err(ret);
}

/**
 * @brief Disable an alarm for the RTC
 * @param handle The RTC handle pointer
 * @param id Alarm channel index (enum max_capi_rtc_alarm_id)
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_alarm_disable(struct capi_rtc_handle *handle,
				      uint16_t id)
{
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	if (id >= MAX_CAPI_RTC_NUM_ALARMS)
		return -EINVAL;

	irq_enabled = _max_capi_rtc_irq_mask();
	ret = MXC_RTC_DisableInt(id == MAX_CAPI_RTC_ALARM_TOD ?
				 MXC_F_RTC_CTRL_TOD_ALARM_IE :
				 MXC_F_RTC_CTRL_SSEC_ALARM_IE);
	_max_capi_rtc_irq_unmask(irq_enabled);

	return _max_capi_rtc_err(ret);
}

/**
 * @brief Get the alarm configuration for an RTC. The alarm value and state
 * 	  are returned through the struct max_capi_rtc_alarm_extra pointed to
 * 	  by config->extra; time and mask are zeroed.
 * @param handle The RTC handle pointer
 * @param id Alarm channel index (enum max_capi_rtc_alarm_id)
 * @param config The pointer to store the alarm configuration to
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_alarm_get(struct capi_rtc_handle *handle, uint16_t id,
				  struct capi_rtc_alarm_config *config)
{
	struct max_capi_rtc_alarm_extra *alarm_extra;

	if (!_max_capi_rtc_valid(handle) || !config || !config->extra)
		return -EINVAL;

	if (id >= MAX_CAPI_RTC_NUM_ALARMS)
		return -EINVAL;

	alarm_extra = config->extra;

	memset(&config->time, 0, sizeof(config->time));
	config->mask = 0;

	if (id == MAX_CAPI_RTC_ALARM_TOD)
		alarm_extra->value = MXC_RTC->toda & MXC_F_RTC_TODA_TOD_ALARM;
	else
		alarm_extra->value = 0U - MXC_RTC->sseca;

	alarm_extra->enabled = MXC_RTC->ctrl & (id == MAX_CAPI_RTC_ALARM_TOD ?
						MXC_F_RTC_CTRL_TOD_ALARM_IE :
						MXC_F_RTC_CTRL_SSEC_ALARM_IE);

	return 0;
}

/**
 * @brief Register a per-alarm callback. A registered per-alarm callback takes
 * 	  precedence over the global callback for that alarm, and is invoked
 * 	  regardless of whether CAPI_RTC_EVENT_ALARM is enabled through
 * 	  capi_rtc_irq_enable(), which only gates the global callback.
 * @param handle The RTC handle pointer
 * @param id Alarm channel index (enum max_capi_rtc_alarm_id)
 * @param callback The callback function to register. NULL to unregister.
 * @param arg The argument to pass to the callback function
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_alarm_set_callback(struct capi_rtc_handle *handle,
		uint16_t id,
		capi_rtc_event_callback callback,
		void *arg)
{
	struct max_capi_rtc_priv *rtc_priv;
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	if (id >= MAX_CAPI_RTC_NUM_ALARMS)
		return -EINVAL;

	rtc_priv = handle->priv;

	if (callback) {
		ret = _max_capi_rtc_irq_connect(handle);
		if (ret)
			return ret;
	}

	/* Masked so that the ISR never sees a new callback with a stale arg */
	irq_enabled = _max_capi_rtc_irq_mask();
	rtc_priv->alarm_callback[id] = callback;
	rtc_priv->alarm_arg[id] = arg;
	_max_capi_rtc_irq_unmask(irq_enabled);

	if (!callback)
		_max_capi_rtc_irq_release(handle);

	return 0;
}

/**
 * @brief Set the clock trim in parts-per-billion for the RTC. The trim
 * 	  register has a resolution of 1 ppm, so the value is rounded to the
 * 	  nearest ppm.
 * @param handle The RTC handle pointer
 * @param cal_ppb The clock trim value
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_set_calibration(struct capi_rtc_handle *handle,
					int32_t cal_ppb)
{
	bool irq_enabled;
	int32_t trim;
	int32_t rem;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	trim = cal_ppb / 1000;
	rem = cal_ppb % 1000;
	if (rem >= 500)
		trim++;
	else if (rem <= -500)
		trim--;

	if (trim < INT8_MIN || trim > INT8_MAX)
		return -ERANGE;

	irq_enabled = _max_capi_rtc_irq_mask();
	_max_capi_rtc_wait_busy();
	ret = MXC_RTC_Trim((int8_t)trim);
	_max_capi_rtc_irq_unmask(irq_enabled);

	return _max_capi_rtc_err(ret);
}

/**
 * @brief Get the clock trim in parts-per-billion for the RTC
 * @param handle The RTC handle pointer
 * @param cal_ppb The pointer to store the clock trim to
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_get_calibration(struct capi_rtc_handle *handle,
					int32_t *cal_ppb)
{
	int8_t trim;

	if (!_max_capi_rtc_valid(handle) || !cal_ppb)
		return -EINVAL;

	trim = (int8_t)((MXC_RTC->trim & MXC_F_RTC_TRIM_TRIM) >>
			MXC_F_RTC_TRIM_TRIM_POS);
	*cal_ppb = (int32_t)trim * 1000;

	return 0;
}

/**
 * @brief Enable the IRQ for an RTC event. Supported events are
 * 	  CAPI_RTC_EVENT_ALARM (alarms themselves are enabled by alarm_set) and
 * 	  MAX_CAPI_RTC_EVENT_READY. CAPI_RTC_EVENT_ALARM only gates delivery
 * 	  to the global callback; per-alarm callbacks are always invoked.
 * @param handle The RTC handle pointer
 * @param event The event to enable
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_irq_enable(struct capi_rtc_handle *handle,
				   uint32_t event)
{
	struct max_capi_rtc_priv *rtc_priv;
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	switch (event) {
	case CAPI_RTC_EVENT_ALARM:
		break;
	case MAX_CAPI_RTC_EVENT_READY:
		irq_enabled = _max_capi_rtc_irq_mask();
		ret = MXC_RTC_EnableInt(MXC_F_RTC_CTRL_RDY_IE);
		_max_capi_rtc_irq_unmask(irq_enabled);
		if (ret)
			return _max_capi_rtc_err(ret);
		break;
	default:
		return -ENOTSUP;
	}

	ret = _max_capi_rtc_irq_connect(handle);
	if (ret) {
		if (event == MAX_CAPI_RTC_EVENT_READY) {
			irq_enabled = _max_capi_rtc_irq_mask();
			MXC_RTC_DisableInt(MXC_F_RTC_CTRL_RDY_IE);
			_max_capi_rtc_irq_unmask(irq_enabled);
		}
		return ret;
	}

	rtc_priv->irq_events |= (1U << event);

	return 0;
}

/**
 * @brief Disable the IRQ for an RTC event. Disabling CAPI_RTC_EVENT_ALARM
 * 	  stops delivery to the global callback only; alarms with a per-alarm
 * 	  callback keep invoking it until it is unregistered.
 * @param handle The RTC handle pointer
 * @param event The event to disable
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_irq_disable(struct capi_rtc_handle *handle,
				    uint32_t event)
{
	struct max_capi_rtc_priv *rtc_priv;
	bool irq_enabled;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	switch (event) {
	case CAPI_RTC_EVENT_ALARM:
		break;
	case MAX_CAPI_RTC_EVENT_READY:
		irq_enabled = _max_capi_rtc_irq_mask();
		ret = MXC_RTC_DisableInt(MXC_F_RTC_CTRL_RDY_IE);
		_max_capi_rtc_irq_unmask(irq_enabled);
		if (ret)
			return _max_capi_rtc_err(ret);
		break;
	default:
		return -ENOTSUP;
	}

	rtc_priv->irq_events &= ~(1U << event);
	_max_capi_rtc_irq_release(handle);

	return 0;
}

/**
 * @brief Get the status of the RTC as a bitmask (CAPI_RTC_STATUS_*)
 * @param handle The RTC handle pointer
 * @param status The pointer to store the status bitmask to
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_get_status(struct capi_rtc_handle *handle,
				   uint32_t *status)
{
	uint32_t ctrl;

	if (!_max_capi_rtc_valid(handle) || !status)
		return -EINVAL;

	ctrl = MXC_RTC->ctrl;
	*status = 0;

	if (ctrl & MXC_F_RTC_CTRL_EN)
		*status |= CAPI_RTC_STATUS_RUNNING;
	if (ctrl & (MXC_F_RTC_CTRL_TOD_ALARM_IE | MXC_F_RTC_CTRL_SSEC_ALARM_IE))
		*status |= CAPI_RTC_STATUS_ALARM_ACTIVE;
	if ((ctrl & (MXC_F_RTC_CTRL_TOD_ALARM_IF | MXC_F_RTC_CTRL_SSEC_ALARM_IF)) ||
	    _max_capi_rtc_alarm_latched(handle->priv))
		*status |= CAPI_RTC_STATUS_ALARM_PENDING;

	return 0;
}

/**
 * @brief Clear the status flags of the RTC given a bitmask. Only
 * 	  CAPI_RTC_STATUS_ALARM_PENDING can be cleared; other bits are ignored.
 * @param handle The RTC handle pointer
 * @param status The status bitmask to clear
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_clear_status(struct capi_rtc_handle *handle,
				     uint32_t status)
{
	struct max_capi_rtc_priv *rtc_priv;
	bool irq_enabled;
	int i;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	if (status & CAPI_RTC_STATUS_ALARM_PENDING) {
		irq_enabled = _max_capi_rtc_irq_mask();
		MXC_RTC_ClearFlags(MXC_F_RTC_CTRL_TOD_ALARM_IF |
				   MXC_F_RTC_CTRL_SSEC_ALARM_IF);
		for (i = 0; i < MAX_CAPI_RTC_NUM_ALARMS; i++)
			rtc_priv->alarm_pending[i] = false;
		_max_capi_rtc_irq_unmask(irq_enabled);
	}

	return 0;
}

/**
 * @brief Check whether an interrupt is pending for a given event
 * @param handle The RTC handle pointer
 * @param event The event to check
 * @param pending The pointer to store the pending status to
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_is_irq_pending(struct capi_rtc_handle *handle,
				       uint32_t event, bool *pending)
{
	if (!_max_capi_rtc_valid(handle) || !pending)
		return -EINVAL;

	switch (event) {
	case CAPI_RTC_EVENT_ALARM:
		*pending = (MXC_RTC_GetFlags() & (MXC_F_RTC_CTRL_TOD_ALARM_IF |
						  MXC_F_RTC_CTRL_SSEC_ALARM_IF)) ||
			   _max_capi_rtc_alarm_latched(handle->priv);
		return 0;
	case MAX_CAPI_RTC_EVENT_READY:
		*pending = (MXC_RTC->ctrl & MXC_F_RTC_CTRL_RDY_IE) &&
			   (MXC_RTC_GetFlags() & MXC_F_RTC_CTRL_RDY);
		return 0;
	default:
		return -ENOTSUP;
	}
}

/**
 * @brief Register the global callback of the RTC
 * @param handle The RTC handle pointer
 * @param callback The callback function to register
 * @param arg The argument passed to the callback function
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_register_callback(struct capi_rtc_handle *handle,
		capi_rtc_event_callback callback,
		void *arg)
{
	struct max_capi_rtc_priv *rtc_priv;
	bool irq_enabled;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	/* Masked so that the ISR never sees a new callback with a stale arg */
	irq_enabled = _max_capi_rtc_irq_mask();
	rtc_priv->callback = callback;
	rtc_priv->arg = arg;
	_max_capi_rtc_irq_unmask(irq_enabled);

	return 0;
}

/**
 * @brief Enable the RTC alarms to wake up the device from low power modes.
 * 	  The RTC IRQ is kept enabled at the NVIC until wake-up is disabled,
 * 	  so that an alarm also resumes the core from WFI. An alarm consumed
 * 	  by no callback is latched (see get_status() and is_irq_pending()).
 * @param handle The RTC handle pointer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_wakeup_enable(struct capi_rtc_handle *handle)
{
	struct max_capi_rtc_priv *rtc_priv;
	int ret;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	ret = _max_capi_rtc_irq_connect(handle);
	if (ret)
		return ret;

	MXC_LP_EnableRTCAlarmWakeup();
	rtc_priv->wakeup_enabled = true;

	return 0;
}

/**
 * @brief Disable the RTC alarms from waking up the device
 * @param handle The RTC handle pointer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_rtc_wakeup_disable(struct capi_rtc_handle *handle)
{
	struct max_capi_rtc_priv *rtc_priv;

	if (!_max_capi_rtc_valid(handle))
		return -EINVAL;

	rtc_priv = handle->priv;

	MXC_LP_DisableRTCAlarmWakeup();
	rtc_priv->wakeup_enabled = false;
	_max_capi_rtc_irq_release(handle);

	return 0;
}

/**
 * @brief Dispatch an alarm event to the per-alarm or the global callback.
 * 	  If neither consumes it, the alarm is latched in software so that it
 * 	  remains visible to get_status() and is_irq_pending().
 * @param rtc_priv The private RTC data
 * @param id Alarm channel index, passed as event_extra
 */
static void _max_capi_rtc_dispatch_alarm(struct max_capi_rtc_priv *rtc_priv,
		uint16_t id)
{
	if (rtc_priv->alarm_callback[id])
		rtc_priv->alarm_callback[id](CAPI_RTC_EVENT_ALARM,
					     rtc_priv->alarm_arg[id], id);
	else if (rtc_priv->callback &&
		 (rtc_priv->irq_events & (1U << CAPI_RTC_EVENT_ALARM)))
		rtc_priv->callback(CAPI_RTC_EVENT_ALARM, rtc_priv->arg, id);
	else
		rtc_priv->alarm_pending[id] = true;
}

/**
 * @brief The interrupt service routine for the RTC
 * @param handle The RTC handle pointer
 */
static void max_capi_rtc_isr(struct capi_rtc_handle *handle)
{
	struct max_capi_rtc_priv *rtc_priv;
	uint32_t alarm_if;
	uint32_t flags;
	uint16_t id;

	if (!_max_capi_rtc_valid(handle))
		return;

	rtc_priv = handle->priv;
	flags = MXC_RTC_GetFlags();

	for (id = 0; id < MAX_CAPI_RTC_NUM_ALARMS; id++) {
		alarm_if = id == MAX_CAPI_RTC_ALARM_TOD ?
			   MXC_F_RTC_CTRL_TOD_ALARM_IF :
			   MXC_F_RTC_CTRL_SSEC_ALARM_IF;
		if (!(flags & alarm_if))
			continue;

		MXC_RTC_ClearFlags(alarm_if);
		_max_capi_rtc_dispatch_alarm(rtc_priv, id);

		/* The callback deinitialized the RTC, freeing the handle */
		if (!_max_capi_rtc_valid(handle))
			return;
	}

	if ((flags & MXC_F_RTC_CTRL_RDY) &&
	    (MXC_RTC->ctrl & MXC_F_RTC_CTRL_RDY_IE)) {
		MXC_RTC_ClearFlags(MXC_F_RTC_CTRL_RDY);

		if (rtc_priv->callback &&
		    (rtc_priv->irq_events & (1U << MAX_CAPI_RTC_EVENT_READY)))
			rtc_priv->callback(MAX_CAPI_RTC_EVENT_READY,
					   rtc_priv->arg, 0);
	}
}

const struct capi_rtc_ops max_capi_rtc_ops = {
	.init = max_capi_rtc_init,
	.deinit = max_capi_rtc_deinit,
	.start = max_capi_rtc_start,
	.stop = max_capi_rtc_stop,
	.get_count = max_capi_rtc_get_count,
	.set_count = max_capi_rtc_set_count,
	.set_tick_config = max_capi_rtc_set_tick_config,
	.get_tick_config = max_capi_rtc_get_tick_config,
	.get_tick_freq = max_capi_rtc_get_tick_freq,
	.alarm_set = max_capi_rtc_alarm_set,
	.alarm_disable = max_capi_rtc_alarm_disable,
	.alarm_get = max_capi_rtc_alarm_get,
	.alarm_set_callback = max_capi_rtc_alarm_set_callback,
	.set_calibration = max_capi_rtc_set_calibration,
	.get_calibration = max_capi_rtc_get_calibration,
	.irq_enable = max_capi_rtc_irq_enable,
	.irq_disable = max_capi_rtc_irq_disable,
	.get_status = max_capi_rtc_get_status,
	.clear_status = max_capi_rtc_clear_status,
	.is_irq_pending = max_capi_rtc_is_irq_pending,
	.register_callback = max_capi_rtc_register_callback,
	.wakeup_enable = max_capi_rtc_wakeup_enable,
	.wakeup_disable = max_capi_rtc_wakeup_disable,
	.isr = max_capi_rtc_isr,
};

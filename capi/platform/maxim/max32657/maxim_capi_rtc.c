/*******************************************************************************
 *   @file   maxim_capi_rtc.c
 *   @brief  Implementation of RTC functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include "maxim_capi_irq.h"
#include "maxim_capi_rtc.h"
#include "rtc.h"

/** Static variables **********************************************************/

static struct capi_rtc_handle *rtc = NULL;

/** Forward declarations ******************************************************/

void max_capi_rtc_isr(void *handle);

/** Function implementations **************************************************/

/**
 * @brief Initialize the RTC peripheral
 * @param handle Pointer to RTC handle pointer
 * @param config RTC configuration
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_init(struct capi_rtc_handle **handle,
		      struct capi_rtc_config *config)
{
	int ret;
	struct capi_rtc_handle *rtc_handle;
	struct max_capi_rtc_priv *rtc_priv;
	const struct max_capi_rtc_extra *rtc_extra;
	uint32_t actual_freq;

	if (!handle || !config)
		return -EINVAL;

	if (rtc != NULL) {
		*handle = rtc;
		return 0;
	}

	if (config->initial_subsec >= MXC_RTC_MAX_SSEC)
		return -EINVAL;

	if (*handle == NULL) {
		rtc_handle = capi_calloc(1, sizeof(*rtc_handle));
		if (!rtc_handle)
			return -ENOMEM;
		rtc_handle->init_allocated = true;
	} else {
		rtc_handle = *handle;
		rtc_handle->init_allocated = false;
	}

	rtc_priv = capi_calloc(1, sizeof(*rtc_priv));
	if (!rtc_priv) {
		ret = -ENOMEM;
		goto free_handle;
	}

	ret = MXC_RTC_Init(config->initial_sec, config->initial_subsec);
	if (ret) {
		ret = -EIO;
		goto free_priv;
	}

	rtc_priv->freq = config->freq ? config->freq : 32768;
	rtc_priv->measure_freq = false;
	rtc_priv->callback = NULL;
	rtc_priv->event_ctx = NULL;
	rtc_priv->events_enabled = 0;
	rtc_priv->is_running = false;

	if (config->extra) {
		rtc_extra = config->extra;
		rtc_priv->measure_freq = rtc_extra->measure_freq;
	}

	if (rtc_priv->measure_freq) {
		actual_freq = MXC_RTC_TrimCrystal();
		if (actual_freq > 0)
			rtc_priv->freq = actual_freq;
	}

	rtc_handle->ops = config->ops;
	rtc_handle->priv = rtc_priv;

	struct capi_irq_config irq_config = {
		.irq_ctrl_id = 0,
	};

	ret = capi_irq_init(&irq_config);
	if (ret)
		goto free_priv;

	ret = capi_irq_connect(RTC_IRQn, max_capi_rtc_isr, rtc_handle);
	if (ret)
		goto free_priv;

	ret = capi_irq_set_priority(RTC_IRQn, config->irq_priority);
	if (ret)
		goto disconnect_irq;

	ret = capi_irq_enable(RTC_IRQn);
	if (ret)
		goto disconnect_irq;

	rtc = rtc_handle;
	*handle = rtc_handle;

	return 0;

disconnect_irq:
	capi_irq_connect(RTC_IRQn, NULL, NULL);
free_priv:
	capi_free(rtc_priv);
free_handle:
	if (rtc_handle->init_allocated)
		capi_free(rtc_handle);

	rtc = NULL;

	return ret;
}

/**
 * @brief Deinitialize the RTC peripheral
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_deinit(struct capi_rtc_handle *handle)
{
	int ret;
	struct max_capi_rtc_priv *rtc_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (handle != rtc)
		return -EINVAL;

	rtc_priv = handle->priv;

	MXC_RTC_DisableInt(MXC_RTC_INT_EN_LONG |
			   MXC_RTC_INT_EN_SHORT |
			   MXC_RTC_INT_EN_READY);
	capi_irq_disable(RTC_IRQn);
	capi_irq_connect(RTC_IRQn, NULL, NULL);

	ret = MXC_RTC_Stop();

	capi_free(rtc_priv);

	if (handle->init_allocated)
		capi_free(handle);

	rtc = NULL;

	return ret;
}

/**
 * @brief Start the RTC
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_start(struct capi_rtc_handle *handle)
{
	int ret;
	struct max_capi_rtc_priv *rtc_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	rtc_priv = handle->priv;

	ret = MXC_RTC_Start();
	if (ret != E_NO_ERROR)
		return -EIO;

	rtc_priv->is_running = true;

	return 0;
}

/**
 * @brief Stop the RTC
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_stop(struct capi_rtc_handle *handle)
{
	int ret;
	struct max_capi_rtc_priv *rtc_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	rtc_priv = handle->priv;

	ret = MXC_RTC_Stop();
	if (ret != E_NO_ERROR)
		return -EIO;

	rtc_priv->is_running = false;

	return 0;
}

/**
 * @brief Get the current time
 * @param handle The RTC handle
 * @param time Where to store the current time
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_get_time(struct capi_rtc_handle *handle,
			  struct capi_rtc_time *time)
{
	int ret;
	uint32_t sec, subsec;

	if (!handle || !time)
		return -EINVAL;

	ret = MXC_RTC_GetTime(&sec, &subsec);
	if (ret != E_NO_ERROR)
		return (ret == E_BUSY) ? -EBUSY : -EIO;

	time->sec = sec;
	time->subsec = subsec;
	time->subsec_resolution = MXC_RTC_MAX_SSEC;

	return 0;
}

/**
 * @brief Set the current time
 * @param handle The RTC handle
 * @param time The time struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_set_time(struct capi_rtc_handle *handle,
			  const struct capi_rtc_time *time)
{
	int ret;
	const struct max_capi_rtc_priv *rtc_priv;

	if (!handle || !handle->priv || !time)
		return -EINVAL;

	if (time->subsec >= MXC_RTC_MAX_SSEC)
		return -EINVAL;

	rtc_priv = handle->priv;

	ret = MXC_RTC_Stop();
	if (ret != E_NO_ERROR)
		return -EIO;

	ret = MXC_RTC_Init(time->sec, time->subsec);
	if (ret != E_NO_ERROR)
		return -EIO;

	if (rtc_priv->is_running) {
		ret = MXC_RTC_Start();
		if (ret != E_NO_ERROR)
			return -EIO;
	}

	return 0;
}

/**
 * @brief Get the current datetime - not supported since the MAX32657 does not
 * 	  have calendar-related functions
 * @param handle The RTC handle
 * @param datetime Where to store the datetime
 * @return -ENOSYS
 */
int max_capi_rtc_get_datetime(struct capi_rtc_handle *handle,
			      struct capi_rtc_datetime *datetime)
{
	return -ENOSYS;
}

/**
 * @brief Set the current datetime - not supported since the MAX32657 does not
 * 	  have calendar-related functions
 * @param handle The RTC handle
 * @param datetime The datetime struct
 * @return -ENOSYS
 */
int max_capi_rtc_set_datetime(struct capi_rtc_handle *handle,
			      const struct capi_rtc_datetime *datetime)
{
	return -ENOSYS;
}

/**
 * @brief Set an alarm
 * @param handle The RTC handle
 * @param type Alarm type
 * @param alarm_value Pointer to alarm value
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_set_alarm(struct capi_rtc_handle *handle,
			   enum capi_rtc_alarm_type type,
			   const void *alarm_value)
{
	int ret;

	if (!handle || !alarm_value)
		return -EINVAL;

	switch (type) {
	case CAPI_RTC_ALARM_TIME:
		const struct capi_rtc_time *alarm = alarm_value;

		if (alarm->sec > MXC_F_RTC_TODA_TOD_ALARM)
			return -EINVAL;

		ret = MXC_RTC_SetTimeofdayAlarm(alarm->sec);
		if (ret != E_NO_ERROR)
			return -EIO;

		return 0;

	case CAPI_RTC_ALARM_SUBSEC:
		const uint32_t *subsec = alarm_value;

		ret = MXC_RTC_SetSubsecondAlarm(subsec);
		if (ret != E_NO_ERROR)
			return -EIO;

		return 0;

	case CAPI_RTC_ALARM_DATETIME:
		return -ENOTSUP;

	default:
		return -EINVAL;
	}
}

/**
 * @brief Disable an alarm
 * @param handle The RTC handle
 * @param type Alarm type
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_disable_alarm(struct capi_rtc_handle *handle,
			       enum capi_rtc_alarm_type type)
{
	int ret;

	if (!handle)
		return -EINVAL;

	switch (type) {
	case CAPI_RTC_ALARM_TIME:
		ret = MXC_RTC_DisableInt(MXC_RTC_INT_EN_LONG);
		break;
	case CAPI_RTC_ALARM_SUBSEC:
		ret = MXC_RTC_DisableInt(MXC_RTC_INT_EN_SHORT);
		break;
	case CAPI_RTC_ALARM_DATETIME:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Enable square wave output
 * @param handle The RTC handle
 * @param freq The square wave frequency
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_sqwave_enable(struct capi_rtc_handle *handle,
			       enum capi_rtc_sqwave_freq freq)
{
	int ret;
	mxc_rtc_freq_sel_t msdk_freq;

	if (!handle)
		return -EINVAL;

	switch (freq) {
	case CAPI_RTC_SQWAVE_1HZ:
		msdk_freq = MXC_RTC_F_1HZ;
		break;
	case CAPI_RTC_SQWAVE_512HZ:
		msdk_freq = MXC_RTC_F_512HZ;
		break;
	case CAPI_RTC_SQWAVE_4KHZ:
		msdk_freq = MXC_RTC_F_4KHZ;
		break;
	case CAPI_RTC_SQWAVE_32KHZ:
		msdk_freq = MXC_RTC_F_32KHZ;
		break;
	default:
		return -EINVAL;
	}

	ret = MXC_RTC_SquareWaveStart(msdk_freq);
	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Disable square wave output
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_sqwave_disable(struct capi_rtc_handle *handle)
{
	int ret;

	if (!handle)
		return -EINVAL;

	ret = MXC_RTC_SquareWaveStop();
	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Trim/calibrate the RTC frequency
 * @param handle The RTC handle
 * @param trim Trim value
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_trim(struct capi_rtc_handle *handle, int8_t trim)
{
	int ret;
	struct max_capi_rtc_priv *rtc_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	rtc_priv = handle->priv;

	ret = MXC_RTC_Trim(trim);
	if (ret != E_NO_ERROR)
		return -EIO;

	if (rtc_priv->measure_freq) {
		uint32_t actual_freq = MXC_RTC_TrimCrystal();
		if (actual_freq > 0)
			rtc_priv->freq = actual_freq;
	}

	return 0;
}

/**
 * @brief Register event callback
 * @param handle The RTC handle
 * @param callback Callback function
 * @param event_ctx Context pointer for callback
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_register_callback(struct capi_rtc_handle *handle,
				   capi_rtc_event_callback_t callback,
				   void *event_ctx)
{
	struct max_capi_rtc_priv *rtc_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	rtc_priv = handle->priv;
	rtc_priv->callback = callback;
	rtc_priv->event_ctx = event_ctx;

	return 0;
}

/**
 * @brief Enable RTC events
 * @param handle The RTC handle
 * @param events_mask Bitmask of events to enable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_enable_events(struct capi_rtc_handle *handle,
			       uint32_t events_mask)
{
	int ret;
	struct max_capi_rtc_priv *rtc_priv;
	uint32_t msdk_mask = 0;

	if (!handle || !handle->priv)
		return -EINVAL;

	rtc_priv = handle->priv;

	if (events_mask & (1 << CAPI_RTC_EVENT_ALARM))
		msdk_mask |= MXC_RTC_INT_EN_LONG;
	if (events_mask & (1 << CAPI_RTC_EVENT_SUBSEC_ALARM))
		msdk_mask |= MXC_RTC_INT_EN_SHORT;
	if (events_mask & (1 << CAPI_RTC_EVENT_READY))
		msdk_mask |= MXC_RTC_INT_EN_READY;

	ret = MXC_RTC_EnableInt(msdk_mask);
	if (ret != E_NO_ERROR)
		return -EIO;

	rtc_priv->events_enabled |= events_mask;

	return 0;
}

/**
 * @brief Disable RTC events
 * @param handle The RTC handle
 * @param events_mask Bitmask of events to disable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_rtc_disable_events(struct capi_rtc_handle *handle,
				uint32_t events_mask)
{
	int ret;
	struct max_capi_rtc_priv *rtc_priv;
	uint32_t msdk_mask = 0;

	if (!handle || !handle->priv)
		return -EINVAL;

	rtc_priv = handle->priv;

	if (events_mask & (1 << CAPI_RTC_EVENT_ALARM))
		msdk_mask |= MXC_RTC_INT_EN_LONG;
	if (events_mask & (1 << CAPI_RTC_EVENT_SUBSEC_ALARM))
		msdk_mask |= MXC_RTC_INT_EN_SHORT;
	if (events_mask & (1 << CAPI_RTC_EVENT_READY))
		msdk_mask |= MXC_RTC_INT_EN_READY;

	ret = MXC_RTC_DisableInt(msdk_mask);
	if (ret != E_NO_ERROR)
		return -EIO;

	rtc_priv->events_enabled &= ~events_mask;

	return 0;
}

/**
 * @brief RTC interrupt handler
 * @param handle The RTC handle
 */
void max_capi_rtc_isr(void *handle)
{
	struct capi_rtc_handle *rtc_handle = (struct capi_rtc_handle *)handle;
	struct max_capi_rtc_priv *rtc_priv;
	int flags;

	if (!rtc_handle || !rtc_handle->priv)
		return;

	rtc_priv = rtc_handle->priv;

	flags = MXC_RTC_GetFlags();

	if (flags & MXC_RTC_INT_FL_LONG) {
		MXC_RTC_ClearFlags(MXC_RTC_INT_FL_LONG);

		if (rtc_priv->callback
		    && (rtc_priv->events_enabled & (1 << CAPI_RTC_EVENT_ALARM)))
			rtc_priv->callback(rtc_handle, CAPI_RTC_EVENT_ALARM, rtc_priv->event_ctx);
	}

	if (flags & MXC_RTC_INT_FL_SHORT) {
		MXC_RTC_ClearFlags(MXC_RTC_INT_FL_SHORT);

		if (rtc_priv->callback
		    && (rtc_priv->events_enabled & (1 << CAPI_RTC_EVENT_SUBSEC_ALARM)))
			rtc_priv->callback(rtc_handle, CAPI_RTC_EVENT_SUBSEC_ALARM,
					   rtc_priv->event_ctx);
	}

	if (flags & MXC_RTC_INT_FL_READY) {
		MXC_RTC_ClearFlags(MXC_RTC_INT_FL_READY);

		if (rtc_priv->callback
		    && (rtc_priv->events_enabled & (1 << CAPI_RTC_EVENT_READY)))
			rtc_priv->callback(rtc_handle, CAPI_RTC_EVENT_READY, rtc_priv->event_ctx);
	}
}

struct capi_rtc_ops max_capi_rtc_ops = {
	.init = max_capi_rtc_init,
	.deinit = max_capi_rtc_deinit,
	.start = max_capi_rtc_start,
	.stop = max_capi_rtc_stop,
	.get_time = max_capi_rtc_get_time,
	.set_time = max_capi_rtc_set_time,
	.get_datetime = max_capi_rtc_get_datetime,
	.set_datetime = max_capi_rtc_set_datetime,
	.set_alarm = max_capi_rtc_set_alarm,
	.disable_alarm = max_capi_rtc_disable_alarm,
	.sqwave_enable = max_capi_rtc_sqwave_enable,
	.sqwave_disable = max_capi_rtc_sqwave_disable,
	.trim = max_capi_rtc_trim,
	.register_callback = max_capi_rtc_register_callback,
	.enable_events = max_capi_rtc_enable_events,
	.disable_events = max_capi_rtc_disable_events,
	.isr = max_capi_rtc_isr,
};

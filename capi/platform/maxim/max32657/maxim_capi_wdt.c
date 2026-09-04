/*******************************************************************************
 *   @file   maxim_capi_wdt.c
 *   @brief  Implementation of WDT functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <string.h>
#include "maxim_capi_irq.h"
#include "maxim_capi_wdt.h"
#include "maxim_capi_wdt_priv.h"
#include "capi_wdt.h"
#include "capi_irq.h"
#include "capi_alloc.h"
#include "max32657.h"

/** Static variables **********************************************************/

static struct capi_wdt_handle *wdt[MXC_CFG_WDT_INSTANCES] = {NULL};

/** Forward declarations ******************************************************/

void max_capi_wdt_isr(void *handle);

/** Helper functions **********************************************************/

/**
 * @brief Get clock frequency for a given source
 * @param clock_source Clock source
 * @return Clock frequency in Hz on success, 0 on error
 */
static uint32_t _max_capi_wdt_get_clock_freq(enum max_capi_wdt_clock
		clock_source)
{
	switch (clock_source) {
	case MAX_CAPI_WDT_CLOCK_PCLK:
		return PeripheralClock;
	case MAX_CAPI_WDT_CLOCK_IBRO:
		return IBRO_FREQ;
	default:
		return 0;
	}
}

/**
 * @brief Convert microseconds to period (enum max_capi_wdt_period)
 * @param timeout_us Timeout in microseconds
 * @param clock_freq_hz Clock frequency in Hz
 * @param period Period
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_wdt_us_to_period(uint64_t timeout_us,
				      uint32_t clock_freq_hz,
				      enum max_capi_wdt_period *period)
{
	uint64_t required_ticks;
	int power;

	if (!period)
		return -EINVAL;

	required_ticks = (timeout_us * clock_freq_hz) / 1000000ULL;

	for (power = 16; power <= 31; power++) {
		if ((1ULL << power) >= required_ticks) {
			*period = (enum max_capi_wdt_period)(31 - power);
			return 0;
		}
	}

	return -ERANGE;
}

/** WDT functions implementations *********************************************/

/**
 * @brief Initialize the WDT peripheral. Does not start the WDT.
 * 	  WDT starts with the first feed.
 * @param handle The WDT handle
 * @param config The config struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_init(struct capi_wdt_handle **handle,
		      const struct capi_wdt_config *config)
{
	int ret;
	struct capi_wdt_handle *wdt_handle;
	struct max_capi_wdt_priv *wdt_priv;
	struct max_capi_wdt_extra *wdt_extra;
	enum max_capi_wdt_clock clock_source;
	mxc_wdt_cfg_t wdt_config;
	uint32_t rst_flags;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MXC_CFG_WDT_INSTANCES)
		return -EINVAL;

	if (wdt[config->identifier] != NULL) {
		*handle = wdt[config->identifier];
		return 0;
	}

	if (*handle == NULL) {
		wdt_handle = capi_calloc(1, sizeof(*wdt_handle));
		if (!wdt_handle)
			return -ENOMEM;

		wdt_priv = capi_calloc(1, sizeof(*wdt_priv));
		if (!wdt_priv) {
			capi_free(wdt_handle);
			return -ENOMEM;
		}

		wdt_handle->priv = wdt_priv;
		wdt_handle->init_allocated = true;
	} else {
		wdt_handle = *handle;

		if (!wdt_handle->priv)
			return -EINVAL;

		wdt_priv = wdt_handle->priv;

		wdt_handle->init_allocated = false;
	}

	clock_source = MAX_CAPI_WDT_CLOCK_PCLK;
	if (config->extra) {
		wdt_extra = config->extra;
		clock_source = wdt_extra->clock_source;
	}

	wdt_priv->id = config->identifier;
	wdt_priv->callback = config->callback;
	wdt_priv->clock_source = clock_source;
	wdt_priv->clock_freq_hz = _max_capi_wdt_get_clock_freq(clock_source);
	if (wdt_priv->clock_freq_hz == 0) {
		ret = -EINVAL;
		goto free_handle;
	}

	rst_flags = MXC_WDT_GetResetFlag(MXC_WDT);

	memset(&wdt_config, 0, sizeof(wdt_config));
	wdt_config.upperIntPeriod = MXC_WDT_PERIOD_2_31;
	wdt_config.upperResetPeriod = MXC_WDT_PERIOD_2_31;
	ret = MXC_WDT_Init(MXC_WDT, &wdt_config);
	MXC_WDT_SetIntPeriod(MXC_WDT, &wdt_config);
	MXC_WDT_SetResetPeriod(MXC_WDT, &wdt_config);
	MXC_WDT_DisableInt(MXC_WDT);
	MXC_WDT_DisableReset(MXC_WDT);
	MXC_WDT_Disable(MXC_WDT);
	MXC_WDT_ClearResetFlag(MXC_WDT);
	if (ret)
		goto free_handle;

	/** Execute the WDT feed sequence and disable the WDT */
	__disable_irq();
	MXC_WDT_Disable(MXC_WDT);

	/** Set WDTn_CTRL.clk_rdy_ie = 1 to generate a
	 * WDT-enabled interrupt event */
	MXC_WDT->ctrl |= MXC_F_WDT_CTRL_CLKRDY_IE;

	/** Re-enable global interrupts */
	__enable_irq();

	/** Configure WDTn_CLKSEL.source to select the clock source */
	ret = MXC_WDT_SetClockSource(MXC_WDT, (mxc_wdt_clock_t)clock_source);
	if (ret)
		goto free_handle;

	/** Restore reset flags */
	MXC_WDT->ctrl |= rst_flags;

	wdt_priv->configured = false;
	wdt_priv->enabled = false;
	wdt_priv->irq_connected = false;

	wdt_handle->ops = config->ops;

	wdt[config->identifier] = wdt_handle;
	*handle = wdt_handle;

	return 0;

free_handle:
	if (wdt_handle->init_allocated) {
		capi_free(wdt_priv);
		capi_free(wdt_handle);
	}

	wdt[config->identifier] = NULL;

	return ret;
}

/**
 * @brief Deinitialize the WDT peripheral
 * @param handle The WDT handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_deinit(struct capi_wdt_handle *handle)
{
	const struct max_capi_wdt_priv *wdt_priv;
	uint8_t id;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	wdt_priv = handle->priv;
	id = wdt_priv->id;

	ret = MXC_WDT_Shutdown(MXC_WDT);

	if (wdt_priv->irq_connected)
		capi_irq_disable(WDT_IRQn);

	if (handle->init_allocated) {
		capi_free(handle->priv);
		capi_free(handle);
	}

	wdt[id] = NULL;

	return ret;
}

/**
 * @brief Get available channels for WDT. In the case of MAX32657, it is single
 * 	  channel, and will return 1 as per CAPI.
 * @param handle The WDT handle
 * @param channels Where to store the number of channels.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_get_chan_count(struct capi_wdt_handle *handle, int *channels)
{
	if (!handle || !channels)
		return -EINVAL;

	*channels = 1;

	return 0;
}

/**
 * @brief Set up a WDT channel. Does not start the WDT.
 * 	  WDT starts with the first feed.
 * @param handle The WDT handle
 * @param chan_id The channel ID - must be 0
 * @param chan_config The configuration struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_setup_chan(struct capi_wdt_handle *handle, int chan_id,
			    const struct capi_wdt_chan_config *chan_config)
{
	int ret;
	struct max_capi_wdt_priv *wdt_priv;
	struct max_capi_wdt_chan_extra *wdt_chan_extra;
	enum max_capi_wdt_period late_period;
	enum max_capi_wdt_mode mode = MAX_CAPI_WDT_MODE_COMPATIBILITY;
	mxc_wdt_cfg_t wdt_config;

	if (!handle || !handle->priv || !chan_config)
		return -EINVAL;

	if (chan_id != 0)
		return -EINVAL;

	wdt_priv = handle->priv;

	/**
	 * Configure the standard thresholds
	 * If using the optional windowed WDT feature...
	 */
	memset(&wdt_config, 0, sizeof(wdt_config));
	if (chan_config->extra) {
		wdt_chan_extra = chan_config->extra;
		wdt_config.upperIntPeriod = (mxc_wdt_period_t)wdt_chan_extra->late_interrupt;
		wdt_config.upperResetPeriod = (mxc_wdt_period_t)wdt_chan_extra->late_reset;
		wdt_config.lowerIntPeriod = (mxc_wdt_period_t)wdt_chan_extra->early_interrupt;
		wdt_config.lowerResetPeriod = (mxc_wdt_period_t)wdt_chan_extra->early_reset;
		mode = wdt_chan_extra->mode;
	} else if (chan_config->timeout_us > 0) {
		ret = _max_capi_wdt_us_to_period(chan_config->timeout_us,
						 wdt_priv->clock_freq_hz,
						 &late_period);
		if (ret)
			return ret;

		wdt_config.upperIntPeriod = (mxc_wdt_period_t)late_period;
		wdt_config.upperResetPeriod = (mxc_wdt_period_t)late_period;
	} else {
		return -EINVAL;
	}

	MXC_WDT_SetIntPeriod(MXC_WDT, &wdt_config);
	MXC_WDT_SetResetPeriod(MXC_WDT, &wdt_config);
	if (mode == MAX_CAPI_WDT_MODE_WINDOWED)
		MXC_WDT->ctrl |= MXC_F_WDT_CTRL_WIN_EN;
	else
		MXC_WDT->ctrl &= ~MXC_F_WDT_CTRL_WIN_EN;

	if (chan_config->irq_enabled) {
		if (!wdt_priv->irq_connected) {
			ret = capi_irq_connect(WDT_IRQn, max_capi_wdt_isr, handle);
			if (ret)
				return ret;

			ret = capi_irq_enable(WDT_IRQn);
			if (ret)
				return ret;

			wdt_priv->irq_connected = true;
		}

		/**
		 * Set WDTn_CTRL.wdt_int_en to generate an interrupt when a
		 * WDT late interrupt event occurs...
		 */
		MXC_WDT_EnableInt(MXC_WDT);
		MXC_WDT_DisableReset(MXC_WDT);
	} else {
		if (wdt_priv->irq_connected) {
			capi_irq_disable(WDT_IRQn);
			wdt_priv->irq_connected = false;
		}

		/**
		 * Set WDTn_CTRL.wdt_rst_en to generate an interrupt when a
		 * WDT late reset event occurs...
		 */
		MXC_WDT_EnableReset(MXC_WDT);
		MXC_WDT_DisableInt(MXC_WDT);
	}

	wdt_priv->configured = true;
	wdt_priv->enabled = false;

	return 0;
}

/**
 * @brief Disable a channel. Feed is required to restart.
 * @param handle The WDT handle
 * @param chan_id The channel ID - must be 0
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_disable_chan(struct capi_wdt_handle *handle, int chan_id)
{
	struct max_capi_wdt_priv *wdt_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan_id != 0)
		return -EINVAL;

	wdt_priv = handle->priv;

	if (wdt_priv->enabled) {
		MXC_WDT_Disable(MXC_WDT);
		wdt_priv->enabled = false;
	}

	return 0;
}

/**
 * @brief Feed (restart) a channel
 * @param handle The WDT handle
 * @param chan_id The channel ID - must be 0
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_feed_chan(struct capi_wdt_handle *handle, int chan_id)
{
	struct max_capi_wdt_priv *wdt_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	if (chan_id != 0)
		return -EINVAL;

	wdt_priv = handle->priv;

	if (!wdt_priv->configured)
		return -EAGAIN;

	if (wdt_priv->enabled) {
		MXC_WDT_ResetTimer(MXC_WDT);
	} else {
		/** Execute the WDT feed sequence and enable the WDT */
		__disable_irq();
		MXC_WDT_Enable(MXC_WDT);

		/** Verify the peripheral is enabled before proceeding */
		while (!(MXC_WDT->ctrl & MXC_F_WDT_CTRL_CLKRDY));

		/**
		 * Set WDTn_CTRL.clkrdy_ie = 1 to generate WDT-enabled
		 * event interrupt
		 */
		MXC_WDT->ctrl |= MXC_F_WDT_CTRL_CLKRDY_IE;

		/** Re-enable global interrupts */
		__enable_irq();

		wdt_priv->enabled = true;
	}

	return 0;
}

/**
 * @brief The ISR for the WDT peripheral
 * @param handle The WDT handle
 */
void max_capi_wdt_isr(void *handle)
{
	struct capi_wdt_handle *wdt_handle = handle;
	struct max_capi_wdt_priv *wdt_priv;
	uint32_t flags;
	bool windowed_mode;

	if (!wdt_handle || !wdt_handle->priv)
		return;

	wdt_priv = wdt_handle->priv;

	windowed_mode = MXC_WDT->ctrl & MXC_F_WDT_CTRL_WIN_EN;
	flags = 0;
	if (windowed_mode) {
		if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_INT_EARLY)
			flags |= MAX_CAPI_WDT_FLAG_INT_EARLY;
		if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_RST_EARLY)
			flags |= MAX_CAPI_WDT_FLAG_RST_EARLY;
	}
	if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_INT_LATE)
		flags |= MAX_CAPI_WDT_FLAG_INT_LATE;
	if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_RST_LATE)
		flags |= MAX_CAPI_WDT_FLAG_RST_LATE;

	if (flags && wdt_priv->callback)
		wdt_priv->callback(0, handle, flags);

	/* Acknowledge the interrupt; reset-cause flags are left for the app */
	MXC_WDT_ClearIntFlag(MXC_WDT);
}

struct capi_wdt_ops max_capi_wdt_ops = {
	.init = max_capi_wdt_init,
	.deinit = max_capi_wdt_deinit,
	.get_chan_count = max_capi_wdt_get_chan_count,
	.setup_chan = max_capi_wdt_setup_chan,
	.disable_chan = max_capi_wdt_disable_chan,
	.feed_chan = max_capi_wdt_feed_chan,
	.isr = max_capi_wdt_isr,
};

/** Platform-specific functions ***********************************************/

/**
 * @brief Get the interrupt and reset flags from the WDT peripheral
 * @param handle The WDT handle
 * @param flags Where to store the flags
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_get_flags(struct capi_wdt_handle *handle, uint32_t *flags)
{
	if (!handle || !flags)
		return -EINVAL;

	*flags = 0;

	if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_INT_LATE)
		*flags |= MAX_CAPI_WDT_FLAG_INT_LATE;

	if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_INT_EARLY)
		*flags |= MAX_CAPI_WDT_FLAG_INT_EARLY;

	if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_RST_LATE)
		*flags |= MAX_CAPI_WDT_FLAG_RST_LATE;

	if (MXC_WDT->ctrl & MXC_F_WDT_CTRL_RST_EARLY)
		*flags |= MAX_CAPI_WDT_FLAG_RST_EARLY;

	return 0;
}

/**
 * @brief Clear the interrupt and reset flags from the WDT peripheral
 * @param handle The WDT handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_clear_flags(struct capi_wdt_handle *handle)
{
	if (!handle)
		return -EINVAL;

	MXC_WDT_ClearIntFlag(MXC_WDT);
	MXC_WDT_ClearResetFlag(MXC_WDT);

	return 0;
}

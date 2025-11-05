/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The Watchdog Timer API
 */

#ifndef _CAPI_WDT_H
#define _CAPI_WDT_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief User callback. Used for interrupt mode only.
 *
 * @param[in] chan_id Channel identifier (for multi-channel WDT support).
 * @param[in] arg Pointer to user specific data.
 * @param[in] extra_flags optional, platform/driver specific extra flags
 */
typedef void (*capi_wdt_callback_t)(int chan_id, void *arg, uint32_t extra_flags);

/**
 * @brief WDT configuration. Common WDT config for all channels.
 * WDT can support multiple channels (multiple reset and IRQ signals).
 * Channel numbers starts from Zero.
 */
struct capi_wdt_config {
	/** ops - optional. if NULL, API selects driver from static mapping based on
	 * identifier */
	const struct capi_wdt_ops *ops;
	/** WDT controller identifier:
	 * - Base address for internal controllers.
	 * - Device specific address for external controllers
	 */
	uint64_t identifier;
	/** Frequency of peripheral clock in Hz, if zero, driver sets to default of platform */
	uint32_t clk_freq_hz;
	/** User callback. Must be provided if any channel is set to irq mode */
	capi_wdt_callback_t callback;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief WDT channel configuration.
 * WDT can support multiple channels (multiple reset and IRQ signals).
 * Channel numbers starts from Zero.
 */
struct capi_wdt_chan_config {
	/** true : WDT raises interrupt on expiration instead of resetting system */
	bool irq_mode;
	/** Timeout in usecs */
	uint64_t timeout_us;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief WDT handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_wdt_handle {
	const struct capi_wdt_ops *ops; /**< set and used by capi thin layer */
	bool init_allocated;            /**< If true, the driver is owner of handle memory. */
	void *lock;                     /**< set and used by capi thin layer if mux is available */
	void *priv;                     /**< set and used by user optionally */
};

/**
 * @brief Initialize an instance of the WDT.
 * Note, init doesn't start the WDT. WDT starts with first feed.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the controller will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the WDT.
 *
 * @return int 0 for success or error code.
 */
int capi_wdt_init(struct capi_wdt_handle **handle, const struct capi_wdt_config *config);

/**
 * @brief Deinitialize the WDT, disable all channels, and bring to default settings.
 *
 * @param [in] handle Points to the WDT context.
 *
 * @return int 0 for success or error code.
 */
int capi_wdt_deinit(struct capi_wdt_handle *handle);

/**
 * @brief Get available channels for WDT. WDT can support multiple channels (multiple reset and IRQ
 * signals). Channel numbers starts from Zero.
 *
 * @param [in] handle Points to a pointer of the context structure.
 * @param [out] channels returns number of available channels. Single channel WDT shall return 1.
 *
 * @return int 0 for success or error code.
 */
int capi_wdt_get_chan_count(struct capi_wdt_handle *handle, int *channels);

/**
 * @brief Setup WDT channel with timing and other options
 * Note, setup doesn't start the WDT. WDT starts with first feed.
 *
 * @param [in] handle Points to a pointer of the context structure.
 * @param [in] chan_id WDT channel id for multi-channel case. Channels starts from 0.
 * @param [in] chan_config Points to the configuration for the WDT.
 *
 * @return int 0 for success or error code.
 */
int capi_wdt_setup_chan(struct capi_wdt_handle *handle, int chan_id,
			const struct capi_wdt_chan_config *chan_config);

/**
 * @brief Setup single channel WDT with timing and other options
 * Note, setup doesn't start the WDT. WDT starts with first feed.
 *
 * @param [in] handle Points to a pointer of the context structure.
 * @param [in] chan_config Points to the configuration for the WDT.
 *
 * @return int 0 for success or error code.
 */
static inline int capi_wdt_setup(struct capi_wdt_handle *handle,
				 const struct capi_wdt_chan_config *chan_config)
{
	return capi_wdt_setup_chan(handle, 0, chan_config);
}

/**
 * @brief Stops the WDT for specific channel. Feed is required to restart.
 *
 * @param [in] handle Points to a pointer of the context structure.
 * @param [in] chan_id WDT channel id for multi-channel case. Channels starts from 0.
 *
 * @return int 0 for success or error code.
 */
int capi_wdt_disable_chan(struct capi_wdt_handle *handle, int chan_id);

/**
 * @brief Stops the single channel WDT. Feed is required to restart.
 *
 * @param [in] handle Points to a pointer of the context structure.
 *
 * @return int 0 for success or error code.
 */
static inline int capi_wdt_disable(struct capi_wdt_handle *handle)
{
	return capi_wdt_disable_chan(handle, 0);
}

/**
 * @brief Feed (restart) the WDT for specific channel.
 *
 * @param [in] handle Points to a pointer of the context structure.
 * @param [in] chan_id WDT channel id for multi-channel case. Channels starts from 0.
 *
 * @return int 0 for success or error code.
 */
int capi_wdt_feed_chan(struct capi_wdt_handle *handle, int chan_id);

/**
 * @brief Feed (restart) the single channel WDT.
 *
 * @param [in] handle Points to a pointer of the context structure.
 *
 * @return int 0 for success or error code.
 */
static inline int capi_wdt_feed(struct capi_wdt_handle *handle)
{
	return capi_wdt_feed_chan(handle, 0);
}

/**
 * @brief WDT Driver Interrupt handler. If interrupt vectors are managed and implemented by user,
 * then user shall call this function in the relevant interrupt vector function.
 *
 * @param [in] handle Points to the WDT context
 */
void capi_wdt_isr(struct capi_wdt_handle *handle);

/**
 * @brief Structure holding WDT function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_wdt_ops {
	/** See capi_wdt_init() */
	int (*init)(struct capi_wdt_handle **handle, const struct capi_wdt_config *config);
	/** See capi_wdt_deinit() */
	int (*deinit)(struct capi_wdt_handle *handle);
	/** See capi_wdt_get_chan_count() */
	int (*get_chan_count)(struct capi_wdt_handle *handle, int *channels);
	/** See capi_wdt_setup_chan() */
	int (*setup_chan)(struct capi_wdt_handle *handle, int chan_id,
			  const struct capi_wdt_chan_config *chan_config);
	/** See capi_wdt_disable_chan() */
	int (*disable_chan)(struct capi_wdt_handle *handle, int chan_id);
	/** See capi_wdt_feed_chan() */
	int (*feed_chan)(struct capi_wdt_handle *handle, int chan_id);
	/** See capi_wdt_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_WDT_H */
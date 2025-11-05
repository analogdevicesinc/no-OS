/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The MIXER API
 *
 * The MIXER baremetal driver configures and controls the MIXER block.
 * The block takes in 4 channel of signed 24 bits input signals, multiply
 * each by the desired volume setting set through their respected registers,
 * adds the results together and outputs as a signed 24 bits results. Each
 * channel input and its volume settings are independent of each other and
 * can be enabled and controlled through their individual registers
 */

#ifndef CAPI_MIXER_H
#define CAPI_MIXER_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief MIXER channel types
 */
enum capi_mixer_chan {
	CAPI_MIXER_CHAN_0,  /**< MIXER channel 0 */
	CAPI_MIXER_CHAN_1,  /**< MIXER channel 1 */
	CAPI_MIXER_CHAN_2,  /**< MIXER channel 2 */
	CAPI_MIXER_CHAN_3,  /**< MIXER channel 3 */
	CAPI_MIXER_CHAN_MAX /**< Max MIXER channel */
};

/**
 * @brief MIXER gain modes
 */
enum capi_mixer_gain_mode {
	CAPI_MIXER_GAIN_MODE_0,  /**<  Hard volume switch mode */
	CAPI_MIXER_GAIN_MODE_1,  /**<  Sample rate ramping mode */
	CAPI_MIXER_GAIN_MODE_2,  /**<  Zero crossing sample rate mode */
	CAPI_MIXER_GAIN_MODE_3,  /**<  Zero crossing ramping mode */
	CAPI_MIXER_GAIN_MODE_MAX /**<  Max gain mode */
};

/**
 * @brief MIXER configuration
 */
struct capi_mixer_config {
	/**< Base address */
	uint64_t identifier;
	/** Platform specific MIXER platform ops structure */
	const struct capi_mixer_ops *ops;
	/** This is intended to store MIXER specific configurations. */
	void *extra;
};

/**
 * @brief MIXER handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_mixer_handle {
	/** MIXER ID */
	uint32_t mixer_id;
	/** MIXER port count */
	uint32_t channel_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_mixer_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Initialize handle
 *
 * @param[in] handle Pointer to the MIXER handle pointer
 * @param[in] config Pointer to MIXER configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_mixer_init(struct capi_mixer_handle **handle, const struct capi_mixer_config *config);

/**
 * @brief Deinitialize handle
 *
 * @param[in] handle Pointer to the MIXER handle
 *
 * @return int 0 for success or error code
 */
int capi_mixer_deinit(const struct capi_mixer_handle *handle);

/**
 * @brief Set the MIXER update gain mode
 *
 * @param handle Pointer to the MIXER handle
 * @param gain_mode 0 = hard switch, 1 = zero crossing, 2 = zc ramp
 *
 * @return int 0 for success or error code
 */
int capi_mixer_set_gain_mode(const struct capi_mixer_handle *handle,
			     enum capi_mixer_gain_mode gain_mode);

/**
 * @brief Select MIXER reference channel for update timing
 *
 * @param handle Pointer to the MIXER handle
 * @param ref_chan Channel index
 *
 * @return int 0 for success or error code
 */
int capi_mixer_set_reference(const struct capi_mixer_handle *handle, enum capi_mixer_chan ref_chan);

/**
 * @brief Enable MIXER channel
 *
 * @param[in] handle Pointer to the MIXER handle
 * @param[in] chan Required channel
 *
 * @return int 0 for success or error code
 */
int capi_mixer_channel_enable(const struct capi_mixer_handle *handle, enum capi_mixer_chan chan);

/**
 * @brief Disable MIXER channel
 *
 * @param[in] handle Pointer to the MIXER handle
 * @param[in] chan Required channel
 *
 * @return int 0 for success or error code
 */
int capi_mixer_channel_disable(const struct capi_mixer_handle *handle, enum capi_mixer_chan chan);

/**
 * @brief Set MIXER channel volume
 *
 * @param[in] handle Pointer to the MIXER handle
 * @param[in] chan Required channel
 * @param[in] gain_db Gain in dB
 *
 * @return int 0 for success or error code
 */
int capi_mixer_channel_set_channel_gain(const struct capi_mixer_handle *handle,
					enum capi_mixer_chan chan, float gain_db);

/**
 * @brief Structure holding MIXER function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_mixer_ops {
	/** See capi_mixer_init() */
	int (*init)(struct capi_mixer_handle **handle, const struct capi_mixer_config *config);
	/** See capi_mixer_deinit() */
	int (*deinit)(const struct capi_mixer_handle *handle);
	/** See capi_mixer_set_gain_mode() */
	int (*set_gain_mode)(const struct capi_mixer_handle *handle,
			     enum capi_mixer_gain_mode gain_mode);
	/** See capi_mixer_set_reference() */
	int (*set_reference)(const struct capi_mixer_handle *handle, enum capi_mixer_chan ref_chan);
	/** See capi_mixer_channel_enable() */
	int (*channel_enable)(const struct capi_mixer_handle *handle, enum capi_mixer_chan chan);
	/** See capi_mixer_channel_disable() */
	int (*channel_disable)(const struct capi_mixer_handle *handle, enum capi_mixer_chan chan);
	/** See capi_mixer_channel_set_channel_gain() */
	int (*channel_set_channel_gain)(const struct capi_mixer_handle *handle,
					enum capi_mixer_chan chan, float gain_db);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_MIXER_H */

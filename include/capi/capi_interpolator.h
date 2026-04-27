/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The Interpolator API
 *
 * The Interpolator driver configures and controls the Interpolator interfaces. The interpolation
 * filter channels convert lower sample rate input data (slow) to higher sample rate output data
 * (fast) and are each enabled individually. The input and output sample rate settings for the
 * interpolation channels are grouped in pairs (channel pairs 0/1, 2/3, 4/5, and 6/7). The input
 * and the output sample rate is set.
 *
 * The selected input sample rate must be lower than the output sample rate.
 */

#ifndef CAPI_INTERPOLATOR_H
#define CAPI_INTERPOLATOR_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief Interpolator channels
 */
enum capi_interpolator_chan {
	CAPI_INTERPOLATOR_CHAN_0,
	CAPI_INTERPOLATOR_CHAN_1,
	CAPI_INTERPOLATOR_CHAN_2,
	CAPI_INTERPOLATOR_CHAN_3,
	CAPI_INTERPOLATOR_CHAN_4,
	CAPI_INTERPOLATOR_CHAN_5,
	CAPI_INTERPOLATOR_CHAN_6,
	CAPI_INTERPOLATOR_CHAN_7,
	CAPI_INTERPOLATOR_CHAN_MAX
};

/**
 * @brief Interpolator channel pairs
 *
 * The sampling rate of adjacent channels(channel pair) is tied together.
 * CH0 and CH1 share the same frequency, as do CH2 & CH3, CH4 & CH5 and CH6 & CH7
 */
enum capi_interpolator_chan_pair {
	CAPI_INTERPOLATOR_CHAN_PAIR_0, /**< Channel 0 and 1 */
	CAPI_INTERPOLATOR_CHAN_PAIR_1, /**< Channel 2 and 3 */
	CAPI_INTERPOLATOR_CHAN_PAIR_2, /**< Channel 4 and 5 */
	CAPI_INTERPOLATOR_CHAN_PAIR_3, /**< Channel 6 and 7 */
	CAPI_INTERPOLATOR_CHAN_PAIR_MAX
};

/**
 * @brief Interpolator input sample rates
 */
enum capi_interpolator_in_sample_rate {
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS8K,   /**<   8 kHz Sample Rate.  */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS12K,  /**<  12 kHz Sample Rate.  */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS16K,  /**<  16 kHz Sample Rate.  */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS24K,  /**<  24 kHz Sample Rate.  */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS48K,  /**<  48 kHz Sample Rate.  */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS96K,  /**<  96 kHz Sample Rate.  */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS192K, /**<  192 kHz Sample Rate. */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_FS384K, /**<  384 kHz Sample Rate. */
	CAPI_INTERPOLATOR_IN_SAMPLE_RATE_MAX     /**<  Invalid Sample Rate. */
};

/**
 * @brief Interpolator output sample rates
 */
enum capi_interpolator_out_sample_rate {
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_FS16K,  /**<  16  kHz Sample Rate. */
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_FS24K,  /**<  24  kHz Sample Rate. */
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_FS48K,  /**<  48  kHz Sample Rate. */
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_FS96K,  /**<  96  kHz Sample Rate. */
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_FS192K, /**<  192 kHz Sample Rate. */
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_FS384K, /**<  384 kHz Sample Rate. */
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_FS768K, /**<  768 kHz Sample Rate. */
	CAPI_INTERPOLATOR_OUT_SAMPLE_RATE_MAX     /**<  Invalid Sample Rate. */
};

/**
 * @brief Interpolator configuration
 */
struct capi_interpolator_config {
	/** Base address */
	uint64_t identifier;
	/** Platform specific Interpolator platform ops structure */
	const struct capi_interpolator_ops *ops;
	/** This is intended to store Interpolator specific configurations */
	void *extra;
};

/**
 * @brief Interpolator handle
 *
 * Drivers may need own handle type to handle internals
 * Driver developer shall declare this as the first field of private handle structure
 */
struct capi_interpolator_handle {
	/** Interpolator ID */
	uint32_t interpolator_id;
	/** Interpolator channel pair count */
	uint32_t channel_pair_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_interpolator_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Interpolator channel pair configuration structure
 *
 * The sampling rate of adjacent channels(channel pair) is tied together.
 * For example, CH0 and CH1 share the same frequency, as do CH2 & CH3,
 * CH4 & CH5, CH6 & CH7 Changing one will affect the other*
 */
struct capi_interpolator_chan_pair_config {
	/** Input sample rate for the channel pair */
	enum capi_interpolator_in_sample_rate input_sample_rate;
	/** Output sample rate for the channel pair */
	enum capi_interpolator_out_sample_rate output_sample_rate;
};

/**
 * @brief Initialize handle base address
 *
 * @param[in] handle Pointer to the Interpolator handle pointer
 * @param[in] config Pointer to Interpolator configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_interpolator_init(struct capi_interpolator_handle **handle,
			   const struct capi_interpolator_config *config);

/**
 * @brief Deinitialize handle
 *
 * @param[in] handle Pointer to the Interpolator handle
 *
 * @return int 0 for success or error code
 */
int capi_interpolator_deinit(const struct capi_interpolator_handle *handle);

/**
 * @brief Set the configuration for a channel pair
 *
 * @param[in] handle Pointer to the Interpolator handle
 * @param[in] chan_pair_id Required channel pair
 * @param[in] chan_pair_config Pointer to config structure for the channel pair
 *
 * @return int 0 for success or error code
 */
int capi_interpolator_set_channel_pair_config(
	const struct capi_interpolator_handle *handle,
	enum capi_interpolator_chan_pair chan_pair_id,
	const struct capi_interpolator_chan_pair_config *chan_pair_config);

/**
 * @brief Get the configuration for a channel pair
 *
 * @param[in] handle Pointer to the Interpolator handle
 * @param[in] chan_pair_id Required channel pair
 * @param[out] chan_pair_config Pointer to config structure for the channel pair
 *
 * @return int 0 for success or error code
 */
int capi_interpolator_get_channel_pair_config(
	const struct capi_interpolator_handle *handle,
	enum capi_interpolator_chan_pair chan_pair_id,
	struct capi_interpolator_chan_pair_config *chan_pair_config);

/**
 * @brief Enable Interpolator channel
 *
 * @param[in] handle Pointer to the Interpolator handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_interpolator_channel_enable(const struct capi_interpolator_handle *handle,
				     enum capi_interpolator_chan chan_id);

/**
 * @brief Disable Interpolator channel
 *
 * @param[in] handle Pointer to the Interpolator handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_interpolator_channel_disable(const struct capi_interpolator_handle *handle,
				      enum capi_interpolator_chan chan_id);

/**
 * @brief Get channel status
 *
 * @param[in] handle Pointer to the Interpolator handle
 * @param[in] chan_id Required channel id
 * @param[out] status Pointer to status
 *
 * @return int 0 for success or error code
 */
int capi_interpolator_get_channel_status(const struct capi_interpolator_handle *handle,
					 enum capi_interpolator_chan chan_id, bool *status);

/**
 * @brief Structure holding Interpolator function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */

struct capi_interpolator_ops {
	/** See capi_interpolator_init() */
	int (*init)(struct capi_interpolator_handle **handle,
		    const struct capi_interpolator_config *config);
	/** See capi_interpolator_deinit() */
	int (*deinit)(const struct capi_interpolator_handle *handle);
	/** See capi_interpolator_set_channel_pair_config() */
	int (*set_channel_pair_config)(
		const struct capi_interpolator_handle *handle,
		enum capi_interpolator_chan_pair chan_pair_id,
		const struct capi_interpolator_chan_pair_config *chan_pair_config);
	/** See capi_interpolator_get_channel_pair_config() */
	int (*get_channel_pair_config)(const struct capi_interpolator_handle *handle,
				       enum capi_interpolator_chan_pair chan_pair_id,
				       struct capi_interpolator_chan_pair_config *chan_pair_config);

	/** See capi_interpolator_channel_enable() */
	int (*channel_enable)(const struct capi_interpolator_handle *handle,
			      enum capi_interpolator_chan chan_id);
	/** See capi_interpolator_channel_disable() */
	int (*channel_disable)(const struct capi_interpolator_handle *handle,
			       enum capi_interpolator_chan chan_id);
	/** See capi_interpolator_get_channel_status() */
	int (*get_channel_status)(const struct capi_interpolator_handle *handle,
				  enum capi_interpolator_chan chan_id, bool *status);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_INTERPOLATOR_H */

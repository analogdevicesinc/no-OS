/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The Decimator API
 *
 * The Decimator driver configures and controls the Decimator interfaces.The decimation filter
 * channels convert higher sample rate input data (fast) to lower sample rate output data (slow)
 * and can each be enabled individually.The input and output sample rate settings for channels
 * are grouped in pairs (channel pairs 0/1, 2/3, 4/5, and 6/7) so the input and output sample rate
 * is set for each pair NOT individually for each channel.
 *
 * The selected input sample rate must be higher than the selected output sample rate
 */

#ifndef CAPI_DECIMATOR_H
#define CAPI_DECIMATOR_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief Decimator channels
 */
enum capi_decimator_chan {
	CAPI_DECIMATOR_CHAN_0,
	CAPI_DECIMATOR_CHAN_1,
	CAPI_DECIMATOR_CHAN_2,
	CAPI_DECIMATOR_CHAN_3,
	CAPI_DECIMATOR_CHAN_4,
	CAPI_DECIMATOR_CHAN_5,
	CAPI_DECIMATOR_CHAN_6,
	CAPI_DECIMATOR_CHAN_7,
	CAPI_DECIMATOR_CHAN_MAX
};

/**
 * @brief Decimator channel pairs
 *
 * The sampling rate of adjacent channels(channel pair) is tied together.
 * CH0 and CH1 share the same frequency, as do CH2 & CH3, CH4 & CH5 and CH6 & CH7
 */
enum capi_decimator_chan_pair {
	CAPI_DECIMATOR_CHAN_PAIR_0, /**< Channel 0 and 1 */
	CAPI_DECIMATOR_CHAN_PAIR_1, /**< Channel 2 and 3 */
	CAPI_DECIMATOR_CHAN_PAIR_2, /**< Channel 4 and 5 */
	CAPI_DECIMATOR_CHAN_PAIR_3, /**< Channel 6 and 7 */
	CAPI_DECIMATOR_CHAN_PAIR_MAX
};

/**
 * @brief Decimator input sample rates
 */
enum capi_decimator_in_sample_rate {
	CAPI_DECIMATOR_IN_SAMPLE_RATE_FS16K,  /**<  16 kHz Sample Rate.  */
	CAPI_DECIMATOR_IN_SAMPLE_RATE_FS24K,  /**<  24 kHz Sample Rate.  */
	CAPI_DECIMATOR_IN_SAMPLE_RATE_FS48K,  /**<  48 kHz Sample Rate.  */
	CAPI_DECIMATOR_IN_SAMPLE_RATE_FS96K,  /**<  96 kHz Sample Rate.  */
	CAPI_DECIMATOR_IN_SAMPLE_RATE_FS192K, /**<  192 kHz Sample Rate. */
	CAPI_DECIMATOR_IN_SAMPLE_RATE_FS384K, /**<  384 kHz Sample Rate. */
	CAPI_DECIMATOR_IN_SAMPLE_RATE_FS768K, /**<  768 kHz Sample Rate. */
	CAPI_DECIMATOR_IN_SAMPLE_RATE_MAX     /**<  Invalid Sample Rate. */
};

/**
 * @brief Decimator output sample rates
 */
enum capi_decimator_out_sample_rate {
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS8K,   /**<  8   kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS12K,  /**<  12  kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS16K,  /**<  16  kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS24K,  /**<  24  kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS48K,  /**<  48  kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS96K,  /**<  96  kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS192K, /**<  192 kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_FS384K, /**<  384 kHz Sample Rate. */
	CAPI_DECIMATOR_OUT_SAMPLE_RATE_MAX     /**<  Invalid Sample Rate. */
};

/**
 * @brief Decimator configuration
 */
struct capi_decimator_config {
	/** Base address */
	uint64_t identifier;
	/** Platform specific Decimator platform ops structure */
	const struct capi_decimator_ops *ops;
	/** This is intended to store Decimator specific configurations */
	void *extra;
};

/**
 * @brief Decimator handle
 *
 * Drivers may need own handle type to handle internals
 * Driver developer shall declare this as the first field of private handle structure
 */
struct capi_decimator_handle {
	/** Decimator ID */
	uint32_t decimator_id;
	/** Decimator channel pair count */
	uint32_t channel_pair_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_decimator_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Decimator channel pair configuration structure
 *
 * The sampling rate of adjacent channels(channel pair) is tied together.
 * For example, CH0 and CH1 share the same frequency, as do CH2 & CH3,
 * CH4 & CH5, CH6 & CH7 Changing one will affect the other
 */
struct capi_decimator_chan_pair_config {
	/** Input sample rate for the channel pair */
	enum capi_decimator_in_sample_rate input_sample_rate;
	/** Output sample rate for the channel pair */
	enum capi_decimator_out_sample_rate output_sample_rate;
};

/**
 * @brief Initialize handle base address
 *
 * @param[in] handle Pointer to the Decimator handle pointer
 * @param[in] config Pointer to Decimator configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_decimator_init(struct capi_decimator_handle **handle,
			const struct capi_decimator_config *config);

/**
 * @brief Deinitialize handle
 *
 * @param[in] handle Pointer to the Decimator handle
 *
 * @return int 0 for success or error code
 */
int capi_decimator_deinit(const struct capi_decimator_handle *handle);

/**
 * @brief Set the configuration for a channel pair
 *
 * @param[in] handle Pointer to the Decimator handle
 * @param[in] chan_pair_id Required channel pair
 * @param[in] chan_pair_config Pointer to config structure for the channel pair
 *
 * @return int 0 for success or error code
 */
int capi_decimator_set_channel_pair_config(
	const struct capi_decimator_handle *handle, enum capi_decimator_chan_pair chan_pair_id,
	const struct capi_decimator_chan_pair_config *chan_pair_config);

/**
 * @brief Get the configuration for a channel pair
 *
 * @param[in] handle Pointer to the Decimator handle
 * @param[in] chan_pair_id Required channel pair
 * @param[out] chan_pair_config Pointer to config structure for the channel pair
 *
 * @return int 0 for success or error code
 */
int capi_decimator_get_channel_pair_config(
	const struct capi_decimator_handle *handle, enum capi_decimator_chan_pair chan_pair_id,
	struct capi_decimator_chan_pair_config *chan_pair_config);

/**
 * @brief Enable Decimator channel
 *
 * @param[in] handle Pointer to the Decimator handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_decimator_channel_enable(const struct capi_decimator_handle *handle,
				  enum capi_decimator_chan chan_id);

/**
 * @brief Disable Decimator channel
 *
 * @param[in] handle Pointer to the Decimator handle
 * @param[in] chan_id Required channel id
 *
 * @return int 0 for success or error code
 */
int capi_decimator_channel_disable(const struct capi_decimator_handle *handle,
				   enum capi_decimator_chan chan_id);

/**
 * @brief Get channel status
 *
 * @param[in] handle Pointer to the Decimator handle
 * @param[in] chan_id Required channel id
 * @param[out] status Pointer to channel status
 *
 * @return int 0 for success or error code
 */
int capi_decimator_get_channel_status(const struct capi_decimator_handle *handle,
				      enum capi_decimator_chan chan_id, bool *status);

/**
 * @brief Structure holding decimator function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_decimator_ops {
	/** See capi_decimator_init() */
	int (*init)(struct capi_decimator_handle **handle,
		    const struct capi_decimator_config *config);
	/** See capi_decimator_deinit() */
	int (*deinit)(const struct capi_decimator_handle *handle);
	/** See capi_decimator_set_channel_pair_config() */
	int (*set_channel_pair_config)(
		const struct capi_decimator_handle *handle,
		enum capi_decimator_chan_pair chan_pair_id,
		const struct capi_decimator_chan_pair_config *chan_pair_config);
	/** See capi_decimator_get_channel_pair_config() */
	int (*get_channel_pair_config)(const struct capi_decimator_handle *handle,
				       enum capi_decimator_chan_pair chan_pair_id,
				       struct capi_decimator_chan_pair_config *chan_pair_config);

	/** See capi_decimator_channel_enable() */
	int (*channel_enable)(const struct capi_decimator_handle *handle,
			      enum capi_decimator_chan chan_id);
	/** See capi_decimator_channel_disable() */
	int (*channel_disable)(const struct capi_decimator_handle *handle,
			       enum capi_decimator_chan chan_id);
	/** See capi_decimator_get_channel_status() */
	int (*get_channel_status)(const struct capi_decimator_handle *handle,
				  enum capi_decimator_chan chan_id, bool *status);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_DECIMATOR_H */

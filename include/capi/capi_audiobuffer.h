/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Audiobuffer Common API
 *
 * The audio buffer driver sets up the DFG control signals, and the DFG input and
 * output stream addresses. DFG memory associated with the streams holds the audio
 * buffer data. The driver configures the audio buffer to read and write data to
 * the streams, and configures the format conversion between audio fabric format (PCM 1.23)
 * and memory formats (1.15, 1.23, 9.23, 1.31, and single precision floating-point).
 * Only non-interleaved data transfer to the DFG memory is supported.
 */

#ifndef CAPI_AUDIOBUFFER_H
#define CAPI_AUDIOBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief AudioBuffer input channel data type
 */
enum capi_audiobuffer_in_chan_data_sel {
	CAPI_AUDIOBUFFER_IN_CHAN_DATA_SEL_AUDIO_DATA,    /**< Audio data type */
	CAPI_AUDIOBUFFER_IN_CHAN_DATA_SEL_REALTIME_DATA, /**< Real time data type */
	CAPI_AUDIOBUFFER_IN_CHAN_DATA_SEL_MAX            /**< Max data type */
};

/**
 * @brief AudioBuffer input channel data format
 */
enum capi_audiobuffer_in_chan_format {
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_1D31,  /**< Source format is 1.31 */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_1D15,  /**< Source format is 1.15 */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_9D23,  /**< Source format is 9.23 */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_1D23,  /**< Source format is 1.23 */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_FLOAT, /**< Source format is floating point */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_MAX,   /**< Max source format */
};

/**
 * @brief AudioBuffer input channel format conversion rounding mode
 */
enum capi_audiobuffer_in_chan_format_round {
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_ROUND_DOWN,         /**< Number is rounded down */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_ROUND_HALF_TO_EVEN, /**< Round half to even */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_ROUND_TO_ZERO,      /**< Round half to zero */
	CAPI_AUDIOBUFFER_IN_CHAN_FORMAT_ROUND_MAX,          /**< Max rounding format */
};

/**
 * @brief AudioBuffer output channel data type
 */
enum capi_audiobuffer_out_chan_data_sel {
	CAPI_AUDIOBUFFER_OUT_CHAN_DATA_SEL_AUDIO_DATA,    /**< Audio data */
	CAPI_AUDIOBUFFER_OUT_CHAN_DATA_SEL_REALTIME_DATA, /**< Real time data */
	CAPI_AUDIOBUFFER_OUT_CHAN_DATA_SEL_MAX            /**< Max data type */
};

/**
 * @brief AudioBuffer output channel data format
 */
enum capi_audiobuffer_out_chan_format {
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_1D31,  /**< Destination format is 1.31 */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_1D15,  /**< Destination format is 1.15 */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_9D23,  /**< Destination format is 9.23 */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_1D23,  /**< Destination format is 1.23 */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_FLOAT, /**< Destination format is floating point */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_MAX,   /**< Max destination format */
};

/**
 * @brief AudioBuffer output channel format conversion rounding mode
 */
enum capi_audiobuffer_out_chan_format_round {
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_ROUND_DOWN,         /**< Number is rounded down */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_ROUND_HALF_TO_EVEN, /**< Round half to even */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_ROUND_TO_ZERO,      /**< Round half to zero */
	CAPI_AUDIOBUFFER_OUT_CHAN_FORMAT_ROUND_MAX           /**< Max rounding format */
};

/**
 * @brief AudioBuffer output channel frame-sync
 */
enum capi_audiobuffer_out_chan_fs_sel {
	CAPI_AUDIOBUFFER_OUT_CHAN_FS_SEL_FS,  /**< Select 1 frame-sync */
	CAPI_AUDIOBUFFER_OUT_CHAN_FS_SEL_2FS, /**< Select 2 frame-syncs */
	CAPI_AUDIOBUFFER_OUT_CHAN_FS_SEL_4FS, /**< Select 4 frame-syncs */
	CAPI_AUDIOBUFFER_OUT_CHAN_FS_SEL_MAX  /**< MAX frame-sync */
};

/**
 * @brief AudioBuffer input channel configuration
 */
struct capi_audiobuffer_in_chan_config {
	uint8_t delay_comp;                               /**< Delay compensation value */
	enum capi_audiobuffer_in_chan_data_sel data_type; /**< Data type */
	enum capi_audiobuffer_in_chan_format format;      /**< Data format */
	enum capi_audiobuffer_in_chan_format_round
		format_round; /**< Format conversion rounding mode */
};

/**
 * @brief AudioBuffer output channel configuration
 */
struct capi_audiobuffer_out_chan_config {
	enum capi_audiobuffer_out_chan_data_sel data_type; /**< Data type */
	enum capi_audiobuffer_out_chan_format format;      /**< Data format */
	enum capi_audiobuffer_out_chan_format_round
		format_round;                     /**< Format conversion rounding mode */
	enum capi_audiobuffer_out_chan_fs_sel fs; /**< Frame-sync */
};

/**
 * @brief AudioBuffer configuration
 */
struct capi_audiobuffer_config {
	/** Base address */
	uint64_t identifier;
	/** Audiobuffer enable */
	bool audiobuff_en;
	/** Delay compensation enable */
	bool delay_comp_en;
	/** Platform specific AudioBuffer platform ops structure */
	const struct capi_audiobuffer_ops *ops;
	/** This is intended to store AudioBuffer specific configurations. */
	void *extra;
};

/**
 * @brief AudioBuffer handle
 *
 * Drivers may need own handle type to handle internals
 * Driver developer shall declare this as the first field of private handle structure
 */
struct capi_audiobuffer_handle {
	/** Audiobuffer ID */
	uint32_t audiobuffer_id;
	/** Audiobuffer channel_count */
	uint32_t chan_count;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_audiobuffer_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Initialize Audiobuffer with settings from the configuration structure
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle pointer
 * @param[in] config Pointer to Audiobuffer Configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_init(struct capi_audiobuffer_handle **handle,
			  const struct capi_audiobuffer_config *config);
/**
 * @brief De-initialize Audiobuffer
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_deinit(const struct capi_audiobuffer_handle *handle);

/**
 * @brief Set input channel configuration
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Input channel number
 * @param[in] config Pointer to input channel configuration
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_set_input_channel_config(const struct capi_audiobuffer_handle *handle,
					      uint8_t chan,
					      const struct capi_audiobuffer_in_chan_config *config);

/**
 * @brief Get input channel configuration
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Input channel number
 * @param[in] config Pointer to input channel configuration
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_get_input_channel_config(const struct capi_audiobuffer_handle *handle,
					      uint8_t chan,
					      struct capi_audiobuffer_in_chan_config *config);

/**
 * @brief Enable input channel
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Input channel number
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_enable_input_channel(const struct capi_audiobuffer_handle *handle,
					  uint8_t chan);

/**
 * @brief Disable input channel
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Input channel number
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_disable_input_channel(const struct capi_audiobuffer_handle *handle,
					   uint8_t chan);

/**
 * @brief Get enable status of input channel
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Input channel number
 * @param[out] status Pointer to enable status of input channel (true:enabled,false:disabled)
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_get_input_channel_status(const struct capi_audiobuffer_handle *handle,
					      uint8_t chan, bool *status);

/**
 * @brief Set output channel configuration
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Output channel number
 * @param[in] config Pointer to output channel configuration
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_set_output_channel_config(
	const struct capi_audiobuffer_handle *handle, uint8_t chan,
	const struct capi_audiobuffer_out_chan_config *config);
/**
 * @brief Get output channel configuration
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Output channel number
 * @param[in] config Pointer to output channel configuration
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_get_output_channel_config(const struct capi_audiobuffer_handle *handle,
					       uint8_t chan,
					       struct capi_audiobuffer_out_chan_config *config);

/**
 * @brief Enable output channel
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Output channel number
 * @param[in] enable Enable/disable flag
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_enable_output_channel(const struct capi_audiobuffer_handle *handle,
					   uint8_t chan, bool enable);

/**
 * @brief Disable output channel
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Output channel number
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_disable_output_channel(const struct capi_audiobuffer_handle *handle,
					    uint8_t chan);

/**
 * @brief Get enable status of output channel
 *
 * @param[in] handle Pointer to Audiobuffer Driver handle
 * @param[in] chan Output channel number
 * @param[out] status Pointer to enable status of output channel (true:enabled,false:disabled)
 *
 * @return int 0 for success or error code
 */
int capi_audiobuffer_get_output_channel_status(const struct capi_audiobuffer_handle *handle,
					       uint8_t chan, bool *status);

/**
 * @brief Structure holding AudiBuffer function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_audiobuffer_ops {
	/** See capi_audiobuffer_init() */
	int (*init)(struct capi_audiobuffer_handle **handle,
		    const struct capi_audiobuffer_config *config);
	/** See capi_audiobuffer_deinit() */
	int (*deinit)(const struct capi_audiobuffer_handle *handle);
	/** See capi_audiobuffer_set_input_channel_config() */
	int (*set_input_channel_config)(const struct capi_audiobuffer_handle *handle, uint8_t chan,
					const struct capi_audiobuffer_in_chan_config *config);
	/** See capi_audiobuffer_get_input_channel_config() */
	int (*get_input_channel_config)(const struct capi_audiobuffer_handle *handle, uint8_t chan,
					struct capi_audiobuffer_in_chan_config *config);
	/** See capi_audiobuffer_enable_input_channel() */
	int (*enable_input_channel)(const struct capi_audiobuffer_handle *handle, uint8_t chan);
	/** See capi_audiobuffer_disable_input_channel() */
	int (*disable_input_channel)(const struct capi_audiobuffer_handle *handle, uint8_t chan);
	/** See capi_audiobuffer_get_input_channel_status() */
	int (*get_input_channel_status)(const struct capi_audiobuffer_handle *handle, uint8_t chan,
					bool *status);
	/** See capi_audiobuffer_set_output_channel_config() */
	int (*set_output_channel_config)(const struct capi_audiobuffer_handle *handle, uint8_t chan,
					 const struct capi_audiobuffer_out_chan_config *config);
	/** See capi_audiobuffer_get_output_channel_config() */
	int (*get_output_channel_config)(const struct capi_audiobuffer_handle *handle, uint8_t chan,
					 struct capi_audiobuffer_out_chan_config *config);
	/** See capi_audiobuffer_enable_output_channel() */
	int (*enable_output_channel)(const struct capi_audiobuffer_handle *handle, uint8_t chan,
				     bool enable);
	/** See capi_audiobuffer_disable_output_channel() */
	int (*disable_output_channel)(const struct capi_audiobuffer_handle *handle, uint8_t chan);
	/** See capi_audiobuffer_get_output_channel_status() */
	int (*get_output_channel_status)(const struct capi_audiobuffer_handle *handle, uint8_t chan,
					 bool *status);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_AUDIOBUFFER_H */

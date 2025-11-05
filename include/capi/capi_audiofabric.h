/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief AudioFabric Common API
 *
 * Audio Fabric block is responsible to route various audio channels to audio
 * peripherals. The Audio Fabric is a passive wiring matrix through which RSS
 * audio components exchange audio samples. An audio signal chain may be configured
 * in the RSS from a source interface, through an arbitrary number of processing
 * elements, to sink interfaces
 *
 * Samples routed through the Audio Fabric use the PCM 1.23(24bit signed) format
 * One source can be listened by more than one sink
 * One sink can only listen one source
 */

#ifndef CAPI_AUDIOFABRIC_H
#define CAPI_AUDIOFABRIC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @typedef capi_audiofabric_component
 * @brief Type definition for audiofabric component identifier.
 */
typedef uint32_t capi_audiofabric_component;

/**
 * @brief AudioFabric configuration
 */
struct capi_audiofabric_config {
	/** Base address */
	uint64_t identifier;
	/** Platform specific AudioFabric platform ops structure */
	const struct capi_audiofabric_ops *ops;
	/** This is intended to store AudioFabric specific configurations. */
	void *extra;
};

/**
 * @brief AudioFabric handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_audiofabric_handle {
	/** AudioFabric ID */
	uint32_t audiofabric_id;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_audiofabric_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Source channel configuration
 */
struct capi_audiofabric_src_chan_config {
	/** Source audio component */
	capi_audiofabric_component src;
	/** Source channel */
	uint8_t src_chan_id;
};

/**
 * @brief Sink channel configuration
 */
struct capi_audiofabric_sink_chan_config {
	/** Sink audio component */
	capi_audiofabric_component sink;
	/** Sink channel */
	uint8_t sink_chan_id;
};

/**
 * @brief Initialize AudioFabric with settings from the configuration structure
 *
 * @param[in] handle Pointer to AudioFabric Driver handle pointer
 * @param[in] config Pointer to AudioFabric Configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_audiofabric_init(struct capi_audiofabric_handle **handle,
			  const struct capi_audiofabric_config *config);

/**
 * @brief Deinit AudioFabric
 *
 * @param[in] handle Pointer to AudioFabric Driver handle
 *
 * @return int 0 for success or error code
 */
int capi_audiofabric_deinit(const struct capi_audiofabric_handle *handle);

/**
 * @brief Set route
 *
 * @param[in] handle Pointer to AudioFabric Driver handle
 * @param[in] src_config Pointer to audio source configuration
 * @param[in] sink_config Pointer to audio sink configuration
 *
 * @return int 0 for success or error code
 */
int capi_audiofabric_set_route(const struct capi_audiofabric_handle *handle,
			       const struct capi_audiofabric_src_chan_config *src_config,
			       const struct capi_audiofabric_sink_chan_config *sink_config);

/**
 * @brief Structure holding AudiFabric function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_audiofabric_ops {
	/** See capi_audiofabric_init() */
	int (*init)(struct capi_audiofabric_handle **handle, uint32_t base_addr);
	/** See capi_audiofabric_deinit() */
	int (*deinit)(const struct capi_audiofabric_handle *handle);
	/** See capi_audiofabric_set_route() */
	int (*set_route)(const struct capi_audiofabric_handle *handle,
			 const struct capi_audiofabric_src_chan_config *src_config,
			 const struct capi_audiofabric_sink_chan_config *sink_config);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_AUDIOFABRIC_H */

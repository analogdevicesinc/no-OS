/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The Sync Source API
 *
 *  The Sync Source is responsible for generating various synchronization
 *  signals, pulses and bit clock signals. Syncsource block receives the
 *  reference clock signal from external/internal source and orchestrates
 *  the generation of clocks and synchronization pulses
 */

#ifndef CAPI_SYNCSOURCE_H
#define CAPI_SYNCSOURCE_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief Sync Source reference clock type
 */
enum capi_syncsource_ref_clk_type {
	CAPI_SYNCSOURCE_REF_CLK_TYPE_INTERNAL, /**< Internal reference clock type */
	CAPI_SYNCSOURCE_REF_CLK_TYPE_EXTERNAL, /**< External reference clock type */
	CAPI_SYNCSOURCE_REF_CLK_TYPE_MAX,      /**< Max reference clock type */
};

/**
 * @brief Sync Source input reference clock
 *
 * Should be defined by the driver
 */
typedef uint32_t capi_syncsource_ref_clk;

/**
 * @brief Sync Source output sync signal
 *
 * Should be defined by the driver
 */
typedef uint32_t capi_syncsource_sync_signal;

/**
 * @brief Sync Source output bit clock
 *
 * Should be defined by the driver
 */
typedef uint32_t capi_syncsource_bit_clk;

/**
 * @brief Sync Source configuration
 */
struct capi_syncsource_config {
	/**< Base address */
	uint64_t identifier;
	/** Platform specific Sync Source platform ops structure */
	const struct capi_syncsource_ops *ops;
	/** Reference clk type */
	enum capi_syncsource_ref_clk_type ref_clk_type;
	/** This is intended to store Sync Source specific configurations. */
	void *extra;
};

/**
 * @brief Sync Source handle
 *
 * Drivers may need own handle type to handle internals
 * Driver developer shall declare this as the first field of private handle structure
 */
struct capi_syncsource_handle {
	/** Sync Source ID */
	uint32_t syncsource_id;
	/** Reference clk type */
	enum capi_syncsource_ref_clk_type ref_clk_type;
	/** Reference clk */
	capi_syncsource_ref_clk ref_clk;
	/** If true, the driver is owner of handle memory */
	bool init_allocated;
	/** set and used by capi thin layer */
	const struct capi_syncsource_ops *ops;
	/** set and used by capi thin layer if locking is available */
	void *lock;
	/** set and used by user optionally */
	void *priv;
};

/**
 * @brief Initialize Sync Source handle
 *
 * @param[in] handle Pointer to the Sync Source handle pointer
 * @param[in] config Pointer to Sync Source Configuration structure
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_init(struct capi_syncsource_handle **handle,
			 const struct capi_syncsource_config *config);

/**
 * @brief Deinitialize Sync Source handle
 *
 * @param[in] handle Pointer to Sync Source handle
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_deinit(const struct capi_syncsource_handle *handle);

/**
 * @brief Set reference clock
 *
 * @param [in] handle Pointer to Sync Source handle
 * @param [in] ref_clk Reference clock
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_set_ref_clk(const struct capi_syncsource_handle *handle,
				capi_syncsource_ref_clk ref_clk);

/**
 * @brief Get reference clock
 *
 * @param [in] handle Pointer to Sync Source handle
 * @param [out] ref_clk Pointer to reference clock
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_get_ref_clk(const struct capi_syncsource_handle *handle,
				capi_syncsource_ref_clk *ref_clk);

/**
 * @brief Initialize reference clock
 *
 * @param [in] handle Pointer to Sync Source handle
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_init_ref_clk(const struct capi_syncsource_handle *handle);

/**
 * @brief Enable reference clock detection
 *
 * @param [in] handle Pointer to Sync Source handle
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_enable_ref_clk_detection(const struct capi_syncsource_handle *handle);

/**
 * @brief Disable reference clock detection
 *
 * @param [in] handle Pointer to Sync Source handle
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_disable_ref_clk_detection(const struct capi_syncsource_handle *handle);

/**
 * @brief Get reference clock lock status
 *
 * @param [in] handle Pointer to Sync Source handle
 * @param [out] status : True locked, False : unlocked
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_get_ref_clk_lock_status(const struct capi_syncsource_handle *handle,
					    bool *status);

/**
 * @brief Enable sync signal and bit clk generation
 *
 * @param [in] handle Pointer to Sync Source handle
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_enable_synchronization(const struct capi_syncsource_handle *handle);

/**
 * @brief Disable sync signal and bit clk generation
 *
 * @param [in] handle Pointer to Sync Source handle
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_disable_synchronization(const struct capi_syncsource_handle *handle);

/**
 * @brief Enable sync signal
 *
 * @param [in] handle Pointer to Sync Source handle
 * @param [in] sync_signal Sync signal
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_enable_sync_signal(const struct capi_syncsource_handle *handle,
				       capi_syncsource_sync_signal sync_signal);

/**
 * @brief Disable sync signal
 *
 * @param [in] handle Pointer to Sync Source handle
 * @param [in] sync_signal Sync signal component
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_disable_sync_signal(const struct capi_syncsource_handle *handle,
					capi_syncsource_sync_signal sync_signal);

/**
 * @brief Enable bit clock
 *
 * @param [in] handle Pointer to Sync Source handle
 * @param [in] bit_clk Bit clock
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_enable_bit_clk(const struct capi_syncsource_handle *handle,
				   capi_syncsource_bit_clk bit_clk);

/**
 * @brief Disable bit clock
 *
 * @param [in] handle Pointer to Sync Source handle
 * @param [in] bit_clk Bit clock
 *
 * @return int 0 for success or error code
 */
int capi_syncsource_disable_bit_clk(const struct capi_syncsource_handle *handle,
				    capi_syncsource_bit_clk bit_clk);

#if defined(__cplusplus)
}
#endif

/**
 * @brief Structure holding Sync Source function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_syncsource_ops {
	/** See capi_syncsource_init() */
	int (*init)(struct capi_syncsource_handle **handle,
		    const struct capi_syncsource_config *config);
	/** See capi_syncsource_deinit() */
	int (*deinit)(const struct capi_syncsource_handle *handle);
	/** See capi_syncsource_set_ref_clk() */
	int (*set_ref_clk)(const struct capi_syncsource_handle *handle,
			   capi_syncsource_ref_clk ref_clk);
	/** See capi_syncsource_get_ref_clk() */
	int (*get_ref_clk)(const struct capi_syncsource_handle *handle,
			   capi_syncsource_ref_clk *ref_clk);
	/** See capi_syncsource_init_ref_clk() */
	int (*init_ref_clk)(const struct capi_syncsource_handle *handle);
	/** See capi_syncsource_enable_ref_clk_detection() */
	int (*enable_ref_clk_detection)(const struct capi_syncsource_handle *handle);
	/** See capi_syncsource_disable_ref_clk_detection() */
	int (*disable_ref_clk_detection)(const struct capi_syncsource_handle *handle);
	/** See capi_syncsource_get_ref_clk_lock_status() */
	int (*get_ref_clk_lock_status)(const struct capi_syncsource_handle *handle, bool *status);
	/** See capi_syncsource_enable_synchronization() */
	int (*enable_synchronization)(const struct capi_syncsource_handle *handle);
	/** See capi_syncsource_disable_synchronization() */
	int (*disable_synchronization)(const struct capi_syncsource_handle *handle);
	/** See capi_syncsource_enable_sync_signal() */
	int (*enable_sync_signal)(const struct capi_syncsource_handle *handle,
				  capi_syncsource_sync_signal sync_signal);
	/** See capi_syncsource_disable_sync_signal() */
	int (*disable_sync_signal)(const struct capi_syncsource_handle *handle,
				   capi_syncsource_sync_signal sync_signal);
	/** See capi_syncsource_enable_bit_clk() */
	int (*enable_bit_clk)(const struct capi_syncsource_handle *handle,
			      capi_syncsource_bit_clk bit_clk);
	/** See capi_syncsource_disable_bit_clk() */
	int (*disable_bit_clk)(const struct capi_syncsource_handle *handle,
			       capi_syncsource_bit_clk bit_clk);
};

#endif /* CAPI_SYNCSOURCE_H */

/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API TRU
 */

#ifndef _CAPI_TRU_H_
#define _CAPI_TRU_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief TRU controller configuration
 */
struct capi_tru_config {
	/** ops - optional. if NULL, API selects driver from static mapping based on
	 * identifier */
	const struct capi_tru_ops *ops;
	/** TRU controller identifier */
	uint64_t identifier;
	/** Optional platform specific extra configuration */
	void *extra;
};

/**
 * @brief TRU controller handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_tru_handle {
	/** Set if init() has allocated the required memory. */
	bool init_allocated;
	const struct capi_tru_ops *ops; /**< set and used by capi thin layer */
	void *lock;                     /**< set and used by capi thin layer if mux is available */
	void *priv;                     /**< set and used by user optionally */
};

/**
 * @typedef route_id_t
 * @brief Type definition for TRU route identifier.
 */
typedef uint32_t route_id_t;

/**
 * @brief  TRU Driver Call Back type
 * @param[in] arg Pointer to user specific data.
 */
typedef void (*capi_tru_callback_t)(void *arg);

/**
 * @brief Initialize an instance of the TRU controller.
 *
 * @param [in out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the driver will allocate
 *	               the context structure and be the owner. If the pointer
 *	               is not NULL, the application has allocated the structure and is
 *	               the owner.
 * @param [in] config Points to the configuration for the TRU controller.
 *
 * @return int 0 for success or error code.
 */
int capi_tru_init(struct capi_tru_handle **handle, const struct capi_tru_config *config);

/**
 * @brief Deinitialize the TRU controller, disable, and bring to default settings.
 *
 * @param [in] handle Points to the TRU controller context.
 *
 * @return int 0 for success or error code.
 */
int capi_tru_deinit(struct capi_tru_handle *handle);

/**
 * @brief Connect TRU receiver to a generator route.
 *
 * @param [in] handle Points to the TRU controller context.
 * @param [in] receiver_route_id Receiver route ID
 * @param [in] generator_route_id Generator route ID
 * @param [in] lock Lock this route configuration
 *
 * @return int 0 for success or error code.
 */
int capi_tru_route_connect(struct capi_tru_handle *handle, route_id_t receiver_route_id,
			   route_id_t generator_route_id, bool lock);

/**
 * @brief Disconnect a TRU receiver if it is connected to a generator route.
 *
 * @param [in] handle Points to the TRU controller context.
 * @param [in] receiver_route_id Receiver route ID
 *
 * @return int 0 for success or error code.
 */
int capi_tru_route_disconnect(struct capi_tru_handle *handle, route_id_t receiver_route_id);

/**
 * @brief Get the generator route ID for a receiver route ID.
 *
 * @param [in] handle Points to the TRU controller context.
 * @param [in] receiver_route_id Receiver route ID
 * @param [out] generator_route_id Generator route ID
 * @param [out] locked Lock status of this route configuration
 *
 * @return int 0 for success or error code.
 */
int capi_tru_get_route(struct capi_tru_handle *handle, route_id_t receiver_route_id,
		       route_id_t *generator_route_id, bool *locked);

/**
 * @brief Trigger a generator route from Software (mostly for debug purposes).
 *
 * @param [in] handle Points to the TRU controller context.
 * @param [in] generator_route_id Generator route ID
 *
 * @return int 0 for success or error code.
 */
int capi_tru_trigger(struct capi_tru_handle *handle, route_id_t generator_route_id);

/**
 * @brief Enables/disables TRU software trigger
 *
 * @param [in] handle Points to the TRU controller context.
 * @param [in] enabled Enables/disables TRU software trigger
 *
 * @return int 0 for success or error code.
 */
int capi_tru_software_trigger_set_enabled(struct capi_tru_handle *handle, bool enabled);

/**
 * @brief Get TRU software trigger status
 *
 * @param [in] handle Points to the TRU controller context.
 * @param [out] enabled Whether TRU software trigger is enabled or not
 *
 * @return int 0 for success or error code.
 */
int capi_tru_software_trigger_get_enabled(struct capi_tru_handle *handle, bool *enabled);

/**
 * @brief Register callback to get TRU interrupt events
 *
 * @param [in] handle Points to the TRU controller context.
 * @param [in] callback User callback function
 * @param [in] callback_arg User callback arg
 *
 * @return int 0 for success or error code.
 */
int capi_tru_register_callback(struct capi_tru_handle *handle, capi_tru_callback_t const callback,
			       void *callback_arg);

/**
 * @brief TRU Driver Interrupt handler. If interrupt vectors are managed and implemented by user,
 * then user shall call this function in the relevant interrupt vector function.
 *
 * @param [in] handle Points to the TRU controller context
 */
void capi_tru_isr(struct capi_tru_handle *handle);

/**
 * @brief Structure holding TRU function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_tru_ops {
	/** See capi_tru_init() */
	int (*init)(struct capi_tru_handle **handle, const struct capi_tru_config *config);
	/** See capi_tru_deinit() */
	int (*deinit)(struct capi_tru_handle *handle);
	/** See capi_tru_route_connect() */
	int (*route_connect)(struct capi_tru_handle *handle, route_id_t receiver_route_id,
			     route_id_t generator_route_id, bool lock);
	/** See capi_tru_route_disconnect() */
	int (*route_disconnect)(struct capi_tru_handle *handle, route_id_t receiver_route_id);
	/** See capi_tru_get_route() */
	int (*get_route)(struct capi_tru_handle *handle, route_id_t receiver_route_id,
			 route_id_t *generator_route_id, bool *locked);
	/** See capi_tru_trigger() */
	int (*trigger)(struct capi_tru_handle *handle, route_id_t generator_route_id);
	/** See capi_tru_software_trigger_set_enabled() */
	int (*software_trigger_set_enabled)(struct capi_tru_handle *handle, bool enabled);
	/** See capi_tru_software_trigger_get_enabled() */
	int (*software_trigger_get_enabled)(struct capi_tru_handle *handle, bool *enabled);
	/** See capi_tru_register_callback() */
	int (*register_callback)(struct capi_tru_handle *handle, capi_tru_callback_t const callback,
				 void *callback_arg);
	/** See capi_tru_isr() */
	int (*isr)(struct capi_tru_handle *handle);
};

#if defined(__cplusplus)
}
#endif

#endif

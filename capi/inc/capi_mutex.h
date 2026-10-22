/* SPDX-License-Identifier: BSD-3-Clause */

/**
 * @file
 * @brief Common API MUTEX
 */

#ifndef _CAPI_MUTEX_H_
#define _CAPI_MUTEX_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MUTEX configuration
 */
struct capi_mutex_config {
	/** Platform ops (mandatory: init returns -EINVAL when NULL). */
	const struct capi_mutex_ops *ops;
	/** If true, the mutex supports recursive locking. */
	bool recursive;
	/** Optional platform specific extra configuration. */
	void *extra;
};

/**
 * @brief MUTEX handle
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_mutex {
	/** Function pointers that implement the Mutex CAPI. */
	const struct capi_mutex_ops *ops;
	/** If true, the driver is owner of handle memory. */
	bool init_allocated;
	/** Implementation specifics. */
	void *priv;
};

/**
 * @struct capi_mutex_ops
 * @brief Structure that contains the mutex operations function pointers.
 */
struct capi_mutex_ops {
	/** See capi_mutex_init() */
	int (*init)(struct capi_mutex **handle, const struct capi_mutex_config *config);
	/** See capi_mutex_deinit() */
	int (*deinit)(struct capi_mutex *handle);
	/** See capi_mutex_lock() */
	int (*lock)(struct capi_mutex *handle);
	/** See capi_mutex_trylock() */
	int (*trylock)(struct capi_mutex *handle);
	/** See capi_mutex_unlock() */
	int (*unlock)(struct capi_mutex *handle);
};

/**
 * @brief Initialize a mutex, allocates the handle and configures the mutex.
 * @param[in] handle Double pointer to a mutex handle structure. Allocated by implementation when
 * *handle is NULL.
 * @param[in] config Pointer to a mutex configuration structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_mutex_init(struct capi_mutex **handle, const struct capi_mutex_config *config);

/**
 * @brief Deallocate resources allocated by capi_mutex_init.
 * @param[in] handle Pointer to a mutex handle structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_mutex_deinit(struct capi_mutex *handle);

/**
 * @brief Lock the mutex (blocking).
 * @param[in] handle Pointer to a mutex handle structure.
 * @return int 0 for success, negative error code otherwise.
 */
int capi_mutex_lock(struct capi_mutex *handle);

/**
 * @brief Try to lock the mutex (non-blocking).
 * @param[in] handle Pointer to a mutex handle structure.
 * @return int 0 for success, -EBUSY if already locked, negative error code otherwise.
 */
int capi_mutex_trylock(struct capi_mutex *handle);

/**
 * @brief Unlock the mutex.
 * @param[in] handle Pointer to a mutex handle structure.
 * @return int 0 for success, -EPERM if not locked, negative error code otherwise.
 */
int capi_mutex_unlock(struct capi_mutex *handle);

#ifdef __cplusplus
}
#endif

#endif

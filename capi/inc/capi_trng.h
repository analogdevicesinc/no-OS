/*
 * Copyright (c) 2024-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The True Random Number Generator API
 */

#ifndef CAPI_TRNG_H_
#define CAPI_TRNG_H_

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

enum capi_trng_async_event {
	CAPI_TRNG_EVENT_GENERATION_COMPLETE,
	CAPI_TRNG_EVENT_ERROR,
};

/**
 * @brief TRNG callback function type
 * @param event Event type
 * @param arg User-provided context pointer
 * @param event_extra Optional platform-specific event information
 */
typedef void (*capi_trng_callback_t)(enum capi_trng_async_event event,
				     void *arg, int event_extra);

struct capi_trng_handle {
	/** If true, driver is owner of handle memory */
	bool init_allocated;
	/** Pointer to function implementations */
	const struct capi_trng_ops *ops;
	/** Optional lock for thread safety */
	void *lock;
	/** Optional user-defined data */
	void *priv;
	/** Optional platform-specific data */
	void *extra;
};

struct capi_trng_config {
	/** Unique identifier for the TRNG controller */
	uint64_t identifier;
	/** Pointer to platform-specific function implementations */
	const struct capi_trng_ops *ops;
	/** IRQ priority for async operations */
	uint32_t irq_priority;
	/** Optional platform-specific configuration */
	void *extra;
};

/**
 * @brief Initialize the TRNG controller
 * @param handle Pointer to handle pointer (can point to NULL for driver allocation)
 * @param config TRNG configuration
 * @return 0 on succes, negative error code otherwise
 */
int capi_trng_init(struct capi_trng_handle **handle,
		   const struct capi_trng_config *config);

/**
 * @brief Deinitialize the TRNG controller
 * @param handle The TRNG handle
 * @return 0 on success, negative error code otherwise
 */
int capi_trng_deinit(struct capi_trng_handle *handle);

/**
 * @brief Generate single 32-bit random number (blocking)
 * @param handle The TRNG handle
 * @param value Pointer to store the generated value
 * @return 0 on success, negative error code otherwise
 */
int capi_trng_generate_u32(struct capi_trng_handle *handle, uint32_t *value);

/**
 * @brief Fill buffer with cryptographically-secure random bytes (blocking)
 * @param handle The TRNG handle
 * @param buffer Pointer to buffer to fill
 * @param length Number of bytes to generate
 * @return 0 on success, negative error code otherwise
 */
int capi_trng_fill_buffer(struct capi_trng_handle *handle, uint8_t *buffer,
			  uint32_t length);

/**
 * @brief Fill buffer with cryptographically-secure random bytes (non-blocking)
 * @param handle The TRNG handle
 * @param buffer Pointer to buffer to fill
 * @param length Number of bytes to generate
 * @return 0 on success, negative error code otherwise
 */
int capi_trng_fill_buffer_async(struct capi_trng_handle *handle,
				uint8_t *buffer, uint32_t length);

/**
 * @brief Register callback for async events
 * @param handle The TRNG handle
 * @param callback User callback function
 * @param callback_arg User context pointer passed to callback
 * @return 0 on success, negative error code otherwise
 */
int capi_trng_register_callback(struct capi_trng_handle *handle,
				capi_trng_callback_t callback,
				void *callback_arg);

/**
 * @brief Perform entropy source health test
 * @param handle The TRNG handle
 * @return 0 on success, negative error code otherwise
 */
int capi_trng_health_test(struct capi_trng_handle *handle);

/**
 * @brief TRNG driver interrupt handler. If interrupt vectors are managed and
 * 	  implemented by the user, then the user shall call this function in the
 * 	  relevant interrupt vector function.
 * @param handle The TRNG handle
 */
void capi_trng_isr(void *handle);

struct capi_trng_ops {
	/** See capi_trng_init() */
	int (*init)(struct capi_trng_handle **handle,
		    const struct capi_trng_config *config);
	/** See capi_trng_deinit() */
	int (*deinit)(struct capi_trng_handle *handle);
	/** See capi_trng_generate_u32() */
	int (*generate_u32)(struct capi_trng_handle *handle, uint32_t *value);
	/** See capi_trng_fill_buffer() */
	int (*fill_buffer)(struct capi_trng_handle *handle, uint8_t *buffer,
			   uint32_t length);
	/** See capi_trng_fill_buffer_async() */
	int (*fill_buffer_async)(struct capi_trng_handle *handle,
				 uint8_t *buffer, uint32_t length);
	/** See capi_trng_register_callback() */
	int (*register_callback)(struct capi_trng_handle *handle,
				 capi_trng_callback_t callback,
				 void *callback_arg);
	/** See capi_trng_health_test() */
	int (*health_test)(struct capi_trng_handle *handle);
	/** See capi_trng_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* CAPI_TRNG_H_ */

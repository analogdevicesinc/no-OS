/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL Semaphore API
 */

#ifndef _CAPI_SEM_H_
#define _CAPI_SEM_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

enum capi_sem_type {
	CAPI_SEM_TYPE_BINARY,
	CAPI_SEM_TYPE_COUNTING,
	CAPI_SEM_TYPE_OWNED,
};

struct capi_sem_config {
	uint64_t identifier;
	enum capi_sem_type type;
	uint32_t initial_value;
	void *extra;
	const struct capi_sem_ops *ops;
};

struct capi_sem_handle {
	const struct capi_sem_ops *ops;
	bool init_allocated;
	enum capi_sem_type type;
	uint32_t owner_tag;
	void *lock;
	void *priv;
};

int capi_sem_init(struct capi_sem_handle **handle,
		  const struct capi_sem_config *config);
int capi_sem_attach(struct capi_sem_handle **handle,
		    const struct capi_sem_config *config);
int capi_sem_detach(struct capi_sem_handle *handle);
int capi_sem_deinit(struct capi_sem_handle *handle);
int capi_sem_try_acquire(struct capi_sem_handle *handle);
int capi_sem_acquire(struct capi_sem_handle *handle);
int capi_sem_acquire_timeout(struct capi_sem_handle *handle,
			     uint32_t timeout_us);
int capi_sem_release(struct capi_sem_handle *handle);
int capi_sem_get_value(struct capi_sem_handle *handle, uint32_t *value);
int capi_sem_set_value(struct capi_sem_handle *handle, uint32_t value);

struct capi_sem_ops {
	int (*init)(struct capi_sem_handle **handle,
		    const struct capi_sem_config *config);
	int (*attach)(struct capi_sem_handle **handle,
		      const struct capi_sem_config *config);
	int (*detach)(struct capi_sem_handle *handle);
	int (*deinit)(struct capi_sem_handle *handle);
	int (*try_acquire)(struct capi_sem_handle *handle);
	int (*acquire)(struct capi_sem_handle *handle);
	int (*acquire_timeout)(struct capi_sem_handle *handle, uint32_t timeout_us);
	int (*release)(struct capi_sem_handle *handle);
	int (*get_value)(struct capi_sem_handle *handle, uint32_t *value);
	int (*set_value)(struct capi_sem_handle *handle, uint32_t value);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_SEM_H_ */

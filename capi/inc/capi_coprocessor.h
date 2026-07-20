/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL coprocessor lifecycle API.
 */

#ifndef _CAPI_COPROCESSOR_H_
#define _CAPI_COPROCESSOR_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

struct capi_coprocessor_config {
	uint32_t identifier;
	void *extra;
	const struct capi_coprocessor_ops *ops;
};

struct capi_coprocessor_handle {
	const struct capi_coprocessor_ops *ops;
	bool init_allocated;
	void *lock;
	void *priv;
};

int capi_coprocessor_init(struct capi_coprocessor_handle **handle,
			  const struct capi_coprocessor_config *config);
int capi_coprocessor_deinit(struct capi_coprocessor_handle *handle);
int capi_coprocessor_boot(struct capi_coprocessor_handle *handle);
int capi_coprocessor_halt(struct capi_coprocessor_handle *handle);

struct capi_coprocessor_ops {
	int (*init)(struct capi_coprocessor_handle **handle,
		    const struct capi_coprocessor_config *config);
	int (*deinit)(struct capi_coprocessor_handle *handle);
	int (*boot)(struct capi_coprocessor_handle *handle);
	int (*halt)(struct capi_coprocessor_handle *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_COPROCESSOR_H_ */

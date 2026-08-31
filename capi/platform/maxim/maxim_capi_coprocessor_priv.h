/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MAXIM_CAPI_COPROCESSOR_PRIV_H_
#define _MAXIM_CAPI_COPROCESSOR_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "maxim_capi_coprocessor.h"

/**
 * @brief MAX78000 coprocessor private data.
 *
 * The hardware boot state lives in a single register (urvbootaddr), so the
 * remaining boot_config fields are shadowed here to make
 * capi_coprocessor_get_boot_config() a faithful read-back of what was
 * requested.
 */
struct maxim_capi_coprocessor_priv {
	/** Instance identifier; only MAXIM_CAPI_COPROCESSOR_RV32 is valid. */
	uint32_t identifier;
	/** Shadow of the last applied boot configuration. */
	struct capi_coprocessor_boot_config boot_config;
	/** Platform options taken from capi_coprocessor_config.extra. */
	struct maxim_capi_coprocessor_config options;
	/**
	 * Last mode requested by the caller. The register bits alone cannot
	 * distinguish PARKED from OFF, nor RUN from LOW_POWER, so the intent is
	 * remembered and re-derived from hardware where the bits do disagree.
	 */
	enum capi_coprocessor_mode mode;
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _MAXIM_CAPI_COPROCESSOR_PRIV_H_ */

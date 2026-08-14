/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_CAPI_COPROCESSOR_H_
#define MAXIM_CAPI_COPROCESSOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "capi_coprocessor.h"

/**
 * @brief Coprocessor instance identifiers accepted in
 *        capi_coprocessor_config.identifier.
 *
 * The MAX78000 has exactly one coprocessor: the RV32 core, referred to as
 * CPU1 in the register map.
 */
#define MAXIM_CAPI_COPROCESSOR_RV32 0

/**
 * @brief Number of independently gateable resources of each class.
 *
 * The RV32 core has one instruction cache (ICC1), sits in one clock domain
 * (CPU1), shares one accelerator with the host (the CNN), and reaches the host
 * over one data lane (the semaphore block).
 */
#define MAXIM_CAPI_COPROCESSOR_NUM_DATA_LANES    1
#define MAXIM_CAPI_COPROCESSOR_NUM_CACHES        1
#define MAXIM_CAPI_COPROCESSOR_NUM_ACCELERATORS  1
#define MAXIM_CAPI_COPROCESSOR_NUM_POWER_DOMAINS 1

/**
 * @brief Boot fields the RV32 core honours.
 *
 * urvbootaddr is a single reset-vector register, so vector_base and entry_pc
 * both map onto it and the vector mode is always
 * CAPI_COPROCESSOR_VECTOR_ENTRY. The core has no mechanism for preloading a
 * stack pointer or argument registers before release from reset, so
 * STACK_POINTER and ARGS are deliberately absent and requests to set them
 * report -ENOTSUP rather than being silently dropped.
 */
#define MAXIM_CAPI_COPROCESSOR_BOOT_CAPS \
	((uint32_t)CAPI_COPROCESSOR_BOOT_CAP_VECTOR_BASE | \
	 (uint32_t)CAPI_COPROCESSOR_BOOT_CAP_ENTRY_PC | \
	 (uint32_t)CAPI_COPROCESSOR_BOOT_CAP_VECTOR_MODE)

/**
 * @brief MAX78000-specific coprocessor configuration.
 *
 * Passed through capi_coprocessor_config.extra. May be left NULL, in which
 * case the defaults below apply.
 */
struct maxim_capi_coprocessor_config {
	/**
	 * Leave the CNN accelerator clock untouched by
	 * capi_coprocessor_set_mode(). Defaults to false, i.e. entering
	 * CAPI_COPROCESSOR_MODE_OFF also gates the CNN clock.
	 */
	bool keep_accelerator_on;
};

/** @brief MAX78000 coprocessor operations for CAPI. */
extern const struct capi_coprocessor_ops maxim_capi_coprocessor_ops;

#endif /* MAXIM_CAPI_COPROCESSOR_H_ */

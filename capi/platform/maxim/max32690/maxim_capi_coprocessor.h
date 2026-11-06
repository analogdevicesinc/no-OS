/***************************************************************************//**
 *   @file   maxim_capi_coprocessor.h
 *   @brief  MAX32690 CAPI coprocessor driver interface.
 *   @author Christopher de Guzman (christopher.deguzman@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAX32690_CAPI_COPROCESSOR_H_
#define MAX32690_CAPI_COPROCESSOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "capi_coprocessor.h"

/**
 * @brief Coprocessor instance identifiers accepted in
 *        capi_coprocessor_config.identifier.
 *
 * The MAX32690 has exactly one coprocessor: the RV32 core, referred to as
 * CPU1 in the register map.
 */
#define MAXIM_CAPI_COPROCESSOR_RV32 0

/**
 * @brief Number of independently gateable resources of each class.
 *
 * The RV32 core has one instruction cache (ICC1), sits in one clock domain
 * (CPU1), and reaches the host over one data lane (the semaphore block). The
 * MAX32690 has no accelerator shared with the RV32 core.
 */
#define MAXIM_CAPI_COPROCESSOR_NUM_DATA_LANES    1
#define MAXIM_CAPI_COPROCESSOR_NUM_CACHES        1
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
 * @brief MAX32690-specific coprocessor configuration.
 *
 * Passed through capi_coprocessor_config.extra. May be left NULL, in which
 * case the defaults below apply.
 */
struct maxim_capi_coprocessor_config {
	/**
	 * Configure the RV32 JTAG pins (gpio_cfg_rv_jtag) before CPU1 boots.
	 * Intended for an attached debug probe; defaults to false.
	 */
	bool enable_debug;
};

/** @brief MAX32690 coprocessor operations for CAPI. */
extern const struct capi_coprocessor_ops maxim_capi_coprocessor_ops;

#endif /* MAX32690_CAPI_COPROCESSOR_H_ */

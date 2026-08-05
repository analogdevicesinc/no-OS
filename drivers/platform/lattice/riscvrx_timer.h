/***************************************************************************//**
 *   @file   riscvrx_timer.h
 *   @brief  Timer control module header for Lattice RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef LATTICE_TIMER_H_
#define LATTICE_TIMER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "no_os_timer.h"
#include "plic.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum latt_timer_source {
	LATT_TIMER_CLINT = 0,
};

/**
 * @struct latt_timer_desc
 * @brief Lattice specific timer descriptor.
 */
struct latt_timer_desc {
	/** Timer source type. */
	enum latt_timer_source type;
	/** Privilege mode used by CLINT handler installation. */
	priv_e mode;
	/** User callback function. */
	void (*callback)(void *);
	/** User callback context pointer. */
	void *callback_ctx;
	/** Timer running state. */
	bool running;
};

/**
 * @struct latt_timer_init_param
 * @brief Lattice specific timer initialization parameters.
 */
struct latt_timer_init_param {
	/** Timer source type. */
	enum latt_timer_source type;
	/** Privilege mode used by CLINT handler installation. */
	priv_e mode;
};

/**
 * @brief Register timer callback and context for Lattice timer backend.
 * @param desc - Pointer to no-OS timer descriptor.
 * @param handler - Timer callback handler.
 * @param ctx - Callback context passed to handler.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t lattice_timer_set_callback(struct no_os_timer_desc *desc,
				   void (*handler)(void *),
				   void *ctx);

/**
 * @brief Lattice platform specific timer platform ops structure.
 */
extern const struct no_os_timer_platform_ops latt_timer_ops;

#endif /* LATTICE_TIMER_H_ */

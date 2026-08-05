/***************************************************************************//**
 *   @file   riscvrx_timer.h
 *   @brief  Timer control module header for Lattice RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

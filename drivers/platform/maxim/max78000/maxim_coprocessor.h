/***************************************************************************//**
 *   @file   maxim_coprocessor.h
 *   @brief  Maxim MAX78000 coprocessor (RISC-V CPU1) platform driver header.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 ******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef _MAXIM_COPROCESSOR_H_
#define _MAXIM_COPROCESSOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_coprocessor.h"

/**
 * @struct max_coprocessor_init_param
 * @brief Maxim-specific coprocessor initialization parameters.
 */
struct max_coprocessor_init_param {
	/**
	 * RISC-V boot address (written to FCR->urvbootaddr).
	 * If 0, the driver uses the linker-supplied __FlashStart_ symbol.
	 */
	uint32_t boot_addr;
	/**
	 * If true, configure the RISC-V JTAG debug pins during boot so a debugger
	 * can attach to the coprocessor.
	 */
	bool enable_debug;
};

/** Platform ops table for MAX78000 coprocessor driver */
extern const struct no_os_coprocessor_platform_ops max_coprocessor_ops;

#endif /* _MAXIM_COPROCESSOR_H_ */

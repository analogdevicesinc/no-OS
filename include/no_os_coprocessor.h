/***************************************************************************//**
 *   @file   no_os_coprocessor.h
 *   @brief  Header file of coprocessor interface.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Generic coprocessor lifecycle management API. Supports auxiliary processor
 * cores (e.g., RISC-V on a dual-core Arm+RISC-V SoC) with boot, halt, and
 * lifecycle control. Platform-agnostic; implementations must provide ops.
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

#ifndef _NO_OS_COPROCESSOR_H_
#define _NO_OS_COPROCESSOR_H_

#include <stdint.h>

/* Forward declaration of the ops struct */
struct no_os_coprocessor_platform_ops;

/**
 * @struct no_os_coprocessor_init_param
 * @brief Coprocessor initialization parameters.
 * @details Passed by caller to no_os_coprocessor_init(). Platform driver
 * must set platform_ops and may set extra to point to platform-specific
 * initialization data.
 */
struct no_os_coprocessor_init_param {
	/** Coprocessor identifier (0-based index, board-specific meaning) */
	uint32_t id;
	/** Pointer to platform-specific ops table (required) */
	const struct no_os_coprocessor_platform_ops *platform_ops;
	/** Platform-specific extra initialization parameters */
	void *extra;
};

/**
 * @struct no_os_coprocessor_desc
 * @brief Coprocessor descriptor.
 * @details Allocated by no_os_coprocessor_init(). Passed to all subsequent
 * operations. Holds the ops table and platform-specific runtime state.
 */
struct no_os_coprocessor_desc {
	/** Coprocessor identifier (copied from init_param) */
	uint32_t id;
	/** Pointer to platform-specific ops table (copied from init_param) */
	const struct no_os_coprocessor_platform_ops *platform_ops;
	/** Platform-specific runtime state */
	void *extra;
};

/**
 * @struct no_os_coprocessor_platform_ops
 * @brief Platform-specific coprocessor operation vtable.
 * @details Each platform driver must define a const instance of this struct
 * with all function pointers set. The generic API dispatches through these.
 */
struct no_os_coprocessor_platform_ops {
	/**
	 * Initialize coprocessor descriptor and resources.
	 * @param desc Output parameter: pointer to allocated descriptor.
	 * @param param Initialization parameters from caller.
	 * @return 0 on success, negative error code on failure.
	 */
	int (*init)(struct no_os_coprocessor_desc **desc,
		    const struct no_os_coprocessor_init_param *param);

	/**
	 * Release coprocessor from reset and enable clocking.
	 * Coprocessor begins executing from reset vector after this call.
	 * @param desc Coprocessor descriptor.
	 * @return 0 on success, negative error code on failure.
	 */
	int (*boot)(struct no_os_coprocessor_desc *desc);

	/**
	 * Halt coprocessor: assert reset and/or disable clocking.
	 * @param desc Coprocessor descriptor.
	 * @return 0 on success, negative error code on failure.
	 */
	int (*halt)(struct no_os_coprocessor_desc *desc);

	/**
	 * Shut down and free coprocessor resources.
	 * @param desc Coprocessor descriptor.
	 * @return 0 on success, negative error code on failure.
	 */
	int (*remove)(struct no_os_coprocessor_desc *desc);
};

/**
 * @brief Initialize a coprocessor.
 * @param desc Output: pointer to coprocessor descriptor (allocated by this function).
 * @param param Input: initialization parameters.
 * @return 0 on success, negative error code on failure.
 */
int no_os_coprocessor_init(struct no_os_coprocessor_desc **desc,
			   const struct no_os_coprocessor_init_param *param);

/**
 * @brief Boot (release from reset) a coprocessor.
 * Coprocessor begins execution after this call.
 * @param desc Coprocessor descriptor from no_os_coprocessor_init().
 * @return 0 on success, negative error code on failure.
 */
int no_os_coprocessor_boot(struct no_os_coprocessor_desc *desc);

/**
 * @brief Halt (assert reset/disable clock) a coprocessor.
 * @param desc Coprocessor descriptor from no_os_coprocessor_init().
 * @return 0 on success, negative error code on failure.
 */
int no_os_coprocessor_halt(struct no_os_coprocessor_desc *desc);

/**
 * @brief Free coprocessor resources and remove descriptor.
 * @param desc Coprocessor descriptor from no_os_coprocessor_init().
 * @return 0 on success, negative error code on failure.
 */
int no_os_coprocessor_remove(struct no_os_coprocessor_desc *desc);

#endif /* _NO_OS_COPROCESSOR_H_ */

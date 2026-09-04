/***************************************************************************//**
 *   @file   no_os_coprocessor.c
 *   @brief  Implementation of coprocessor interface.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Generic dispatch layer for platform-specific coprocessor drivers.
 * Follows the no-OS platform abstraction pattern: thin guards + dispatch.
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

#include "no_os_coprocessor.h"
#include <errno.h>

/**
 * @brief Initialize a coprocessor.
 * Allocates and initializes descriptor via platform ops.
 * @param desc Output: pointer to coprocessor descriptor.
 * @param param Input: initialization parameters with platform_ops.
 * @return 0 on success, -EINVAL if param or platform_ops is NULL,
 *         -ENOSYS if init op is not implemented, or platform-specific error.
 */
int no_os_coprocessor_init(struct no_os_coprocessor_desc **desc,
			   const struct no_os_coprocessor_init_param *param)
{
	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	return param->platform_ops->init(desc, param);
}

/**
 * @brief Boot (release from reset) a coprocessor.
 * @param desc Coprocessor descriptor from no_os_coprocessor_init().
 * @return 0 on success, -EINVAL if desc or platform_ops is NULL,
 *         -ENOSYS if boot op is not implemented, or platform-specific error.
 */
int no_os_coprocessor_boot(struct no_os_coprocessor_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->boot)
		return -ENOSYS;

	return desc->platform_ops->boot(desc);
}

/**
 * @brief Halt (assert reset/disable clock) a coprocessor.
 * @param desc Coprocessor descriptor from no_os_coprocessor_init().
 * @return 0 on success, -EINVAL if desc or platform_ops is NULL,
 *         -ENOSYS if halt op is not implemented, or platform-specific error.
 */
int no_os_coprocessor_halt(struct no_os_coprocessor_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->halt)
		return -ENOSYS;

	return desc->platform_ops->halt(desc);
}

/**
 * @brief Free coprocessor resources and remove descriptor.
 * @param desc Coprocessor descriptor from no_os_coprocessor_init().
 * @return 0 on success, -EINVAL if desc or platform_ops is NULL,
 *         -ENOSYS if remove op is not implemented, or platform-specific error.
 */
int no_os_coprocessor_remove(struct no_os_coprocessor_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	return desc->platform_ops->remove(desc);
}

/***************************************************************************//**
 *   @file   maxim_trng.c
 *   @brief  MAX32650 implementation of true random number generator
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "trng.h"
#include "maxim_trng.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/**
 * @brief Initialize the device.
 * @param desc - The device structure.
 * @param param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_trng_init(struct no_os_trng_desc **desc,
		  struct no_os_trng_init_param *param)
{
	int ret;
	struct no_os_trng_desc *descriptor;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	NO_OS_UNUSED_PARAM(param);

	ret = MXC_TRNG_Init();
	if (ret)
		goto error;

	*desc = descriptor;

	return 0;

error:
	no_os_free(desc);

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_trng_remove(struct no_os_trng_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc);

	return MXC_TRNG_Shutdown();
}

/**
 * @brief Fill buffer with random numbers.
 * @param desc - The device structure.
 * @param buff - Buffer to be filled.
 * @param len - Length of the buffer.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_trng_fill_buffer(struct no_os_trng_desc *desc, uint8_t *buff,
			 uint32_t len)
{
	NO_OS_UNUSED_PARAM(desc);

	return MXC_TRNG_Random(buff, len);
}

/**
 * @brief MAX32650 platform specific TRNG platform ops structure
 */
const struct no_os_trng_platform_ops max_trng_ops = {
	.init = &max_trng_init,
	.fill_buffer = &max_trng_fill_buffer,
	.remove = &max_trng_remove
};

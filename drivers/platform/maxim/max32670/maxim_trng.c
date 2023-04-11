/***************************************************************************//**
 *   @file   maxim_trng.c
 *   @brief  MAX32670 implementation of true random number generator
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "trng.h"
#include "maxim_trng.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
 * @brief MAX32670 platform specific TRNG platform ops structure
 */
const struct no_os_trng_platform_ops max_trng_ops = {
	.init = &max_trng_init,
	.fill_buffer = &max_trng_fill_buffer,
	.remove = &max_trng_remove
};

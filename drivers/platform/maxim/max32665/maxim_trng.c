/***************************************************************************//**
 *   @file   maxim_trng.c
 *   @brief  MAX32665 implementation of true random number generator
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
 * @brief MAX32665 platform specific TRNG platform ops structure
 */
const struct no_os_trng_platform_ops max_trng_ops = {
	.init = &max_trng_init,
	.fill_buffer = &max_trng_fill_buffer,
	.remove = &max_trng_remove
};

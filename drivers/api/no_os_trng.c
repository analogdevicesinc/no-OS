/***************************************************************************//**
 *   @file   no_os_trng.c
 *   @brief  Implementation of the TRNG Interface
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <inttypes.h>
#include "no_os_trng.h"
#include <stdlib.h>
#include "no_os_error.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the TRNG.
 * @param desc - The TRNG descriptor.
 * @param param - The structure that contains the TRNG parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_trng_init(struct no_os_trng_desc **desc,
		    const struct no_os_trng_init_param *param)
{
	int ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	ret = param->platform_ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_trng_init().
 * @param desc - The TRNG descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_trng_remove(struct no_os_trng_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Fill buffer with rng data.
 * @param desc - The TRNG descriptor.
 * @param buff - Buffer to be filled
 * @param len - Size of the buffer
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_trng_fill_buffer(struct no_os_trng_desc *desc, uint8_t *buff,
			   uint32_t len)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->fill_buffer)
		return -ENOSYS;

	return desc->platform_ops->fill_buffer(desc, buff, len);
}

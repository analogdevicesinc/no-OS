/***************************************************************************//**
 *   @file   generic_trng.c
 *   @brief  Generic implementation of true random number generator
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_trng.h"
#include "no_os_util.h"
#include "no_os_error.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* Initialize descriptor */
int generic_trng_init(struct no_os_trng_desc **desc,
		      struct no_os_trng_init_param *param)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(param);

	return -1;
}

/* Free resources allocated in descriptor */
int generic_trng_remove(struct no_os_trng_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);
}

/* Fill buffer with random numbers */
int generic_trng_fill_buffer(struct no_os_trng_desc *desc, uint8_t *buff,
			     uint32_t len)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(buff);
	NO_OS_UNUSED_PARAM(len);

	return -1;
}

/**
 * @brief Generic TRNG platform ops structure
 */
const struct no_os_trng_platform_ops aducm_trng_ops = {
	.init = &generic_trng_init,
	.fill_buffer = &generic_trng_fill_buffer,
	.remove = &generic_trng_remove
};

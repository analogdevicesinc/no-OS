/***************************************************************************//**
 *   @file   no_os_trng.h
 *   @brief  Header file of true random number generator
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NO_OS_TRNG_H_
#define _NO_OS_TRNG_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_trng_platform_ops
 * @brief Structure holding TRNG function pointers that point to the platform
 * specific function
 */
struct no_os_trng_platform_ops;

/**
 * @struct no_os_trng_desc
 * @brief TRNG Descriptor
 */
struct no_os_trng_desc {
	/** Platform ops */
	const struct no_os_trng_platform_ops *platform_ops;
	/** Platform specific parameters */
	void *extra;
};

/**
 * @struct no_os_trng_init_param
 * @brief Init parameter for TRNG
 */
struct no_os_trng_init_param {
	/** Device id */
	uint32_t	dev_id;
	/** Platform specific parameter */
	void		*extra;
	/** Platform ops */
	const struct no_os_trng_platform_ops *platform_ops;
};

/**
 * @struct no_os_trng_platform_ops
 * @brief Structure holding TRNG function pointers that point to the platform
 * specific function
 */
struct no_os_trng_platform_ops {
	/** TRNG initialization function pointer */
	int (*init)(struct no_os_trng_desc **,
		    const struct no_os_trng_init_param *);
	/** Fill buffer with random numbers */
	int (*fill_buffer)(struct no_os_trng_desc *, uint8_t *, uint32_t);
	/** TRNG remove function pointer */
	int (*remove)(struct no_os_trng_desc *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize descriptor */
int no_os_trng_init(struct no_os_trng_desc **desc,
		    const struct no_os_trng_init_param *param);

/* Free resources allocated in descriptor */
int no_os_trng_remove(struct no_os_trng_desc *desc);

/* Fill buffer with random numbers */
int no_os_trng_fill_buffer(struct no_os_trng_desc *desc, uint8_t *buff,
			   uint32_t len);

#endif // _NO_OS_TRNG_H_

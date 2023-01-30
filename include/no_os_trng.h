/***************************************************************************//**
 *   @file   no_os_trng.h
 *   @brief  Header file of true random number generator
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
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

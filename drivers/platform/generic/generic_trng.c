/***************************************************************************//**
 *   @file   generic_trng.c
 *   @brief  Generic implementation of true random number generator
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "no_os_trng.h"
#include "no_os_util.h"
#include "no_os_error.h"

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

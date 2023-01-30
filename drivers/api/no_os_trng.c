/***************************************************************************//**
 *   @file   no_os_trng.c
 *   @brief  Implementation of the TRNG Interface
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
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

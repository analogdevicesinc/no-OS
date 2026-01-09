/***************************************************************************//**
 *   @file   no_os_net.c
 *   @brief  Implementation of the network interface API
 *   @author alisa.roman@analog.com
********************************************************************************
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

#include "no_os_net.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/**
 * @brief Initialize the network device
 *
 * Allocates the network descriptor and calls the backend init function.
 * The backend is responsible for setting desc->net_if and desc->extra.
 *
 * @param desc - Pointer to network descriptor to be allocated
 * @param param - Initialization parameters
 * @return 0 on success, negative error code otherwise
 */
int no_os_net_init(struct no_os_net_desc **desc,
		   struct no_os_net_init_param *param)
{
	struct no_os_net_desc *ldesc;
	int ret;

	if (!desc || !param || !param->platform_ops || !param->platform_ops->init)
		return -EINVAL;

	ldesc = (struct no_os_net_desc *)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	ldesc->platform_ops = param->platform_ops;

	ret = param->platform_ops->init(ldesc, param);
	if (ret) {
		no_os_free(ldesc);
		return ret;
	}

	*desc = ldesc;

	return 0;
}

/**
 * @brief Remove the network device
 *
 * Calls the backend remove function to free backend-specific resources,
 * then frees the network descriptor.
 *
 * @param desc - Network descriptor to be freed
 * @return 0 on success, negative error code otherwise
 */
int no_os_net_remove(struct no_os_net_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	if (desc->platform_ops && desc->platform_ops->remove) {
		ret = desc->platform_ops->remove(desc);
		if (ret)
			return ret;
	}

	no_os_free(desc);

	return 0;
}

/**
 * @brief Execute network processing step
 *
 * @param desc - Network descriptor
 * @param arg - Optional backend-specific argument
 * @return 0 on success, negative error code otherwise
 */
int no_os_net_step(struct no_os_net_desc *desc, void *arg)
{
	if (!desc)
		return -EINVAL;

	if (!desc->platform_ops || !desc->platform_ops->step)
		return 0;

	return desc->platform_ops->step(desc, arg);
}

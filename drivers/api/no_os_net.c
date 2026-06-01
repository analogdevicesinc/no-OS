/***************************************************************************//**
 *   @file   no_os_net.c
 *   @brief  Implementation of the Network Interface
 *   @author Alisa-Dariana Roman <alisa.roman@analog.com>
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

#include <inttypes.h>
#include <stdlib.h>

#include "no_os_error.h"
#include "no_os_net.h"
#include "no_os_util.h"

/**
 * @brief Initialize the network device.
 * @param desc - The network device descriptor.
 * @param param - The structure that contains the network device parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_net_init(struct no_os_net_desc **desc,
		       const struct no_os_net_init_param *param)
{
	int32_t ret;

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
 * @brief Free the resources allocated by no_os_net_init().
 * @param desc - The network device descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_net_remove(struct no_os_net_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Process pending network events (timers, RX polling).
 * @param desc - The network device descriptor.
 * @return 0 in case of success or if step is not needed, error code otherwise.
 */
int32_t no_os_net_step(struct no_os_net_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->step)
		return 0;

	return desc->platform_ops->step(desc);
}

/**
 * @brief Get the current IP/netmask/gateway configuration.
 * @param desc - The network device descriptor.
 * @param ip - Output structure for the current IP configuration.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_net_get_ip(struct no_os_net_desc *desc,
			 struct no_os_net_ip_config *ip)
{
	if (!desc || !ip || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->get_ip)
		return -ENOSYS;

	return desc->platform_ops->get_ip(desc, ip);
}

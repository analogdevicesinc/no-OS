/***************************************************************************//**
 *   @file   no_os_net.c
 *   @brief  Implementation of the network interface.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include <stdio.h>
#include "no_os_net.h"
#include "no_os_error.h"

/**
 * @brief Initialize a network interface.
 * @param desc - The network interface descriptor.
 * @param param - The structure that contains the network interface parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_net_init(struct no_os_net_desc **desc,
		       const struct no_os_net_init_param *param)
{
	int32_t ret;

	if (!desc || !param || !param->platform_ops)
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
 * @param desc - The network interface descriptor.
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
 * @brief Service the network interface.
 *
 * Must be called periodically from the application loop to drive the stack
 * timers and poll the underlying MAC.
 * @param desc - The network interface descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_net_step(struct no_os_net_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->step)
		return -ENOSYS;

	return desc->platform_ops->step(desc);
}

/**
 * @brief Get the current IP address as a numeric string.
 * @param desc - The network interface descriptor.
 * @param buf - Destination buffer for the address string.
 * @param size - Size of the destination buffer.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_net_get_ip(struct no_os_net_desc *desc, char *buf, uint32_t size)
{
	if (!desc || !desc->platform_ops || !buf)
		return -EINVAL;

	if (!desc->platform_ops->get_ip)
		return -ENOSYS;

	return desc->platform_ops->get_ip(desc, buf, size);
}

/**
 * @brief Resolve a hostname to an address.
 * @param desc - The network interface descriptor.
 * @param host - Hostname to resolve.
 * @param addr - Destination for the resolved address.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_net_resolve(struct no_os_net_desc *desc, const char *host,
			  struct no_os_sockaddr *addr)
{
	if (!desc || !desc->platform_ops || !host || !addr)
		return -EINVAL;

	if (!desc->platform_ops->resolve)
		return -ENOSYS;

	return desc->platform_ops->resolve(desc, host, addr);
}

/**
 * @brief Parse a dotted-decimal IPv4 string into 4 bytes.
 * @param str - Source string, e.g. "192.168.0.1".
 * @param ip - Destination array of 4 bytes.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int32_t no_os_net_ipv4_to_bytes(const char *str, uint8_t ip[4])
{
	if (!str || !ip)
		return -EINVAL;

	if (sscanf(str, "%hhu.%hhu.%hhu.%hhu",
		   &ip[0], &ip[1], &ip[2], &ip[3]) != 4)
		return -EINVAL;

	return 0;
}

/**
 * @brief Format 4 bytes as a dotted-decimal IPv4 string.
 * @param ip - Source array of 4 bytes.
 * @param buf - Destination buffer.
 * @param size - Size of the destination buffer.
 * @return 0 in case of success, -EINVAL if the buffer is too small.
 */
int32_t no_os_net_ipv4_to_str(const uint8_t ip[4], char *buf, uint32_t size)
{
	int ret;

	if (!ip || !buf)
		return -EINVAL;

	ret = snprintf(buf, size, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	if (ret < 0 || (uint32_t)ret >= size)
		return -EINVAL;

	return 0;
}

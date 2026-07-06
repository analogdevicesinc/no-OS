/***************************************************************************//**
 *   @file   linux_net.h
 *   @brief  Linux adapter for the no_os_net / no_os_socket interfaces.
 *   @author Ramona Nechita (ramona.nechita@analog.com)
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
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
#ifndef _LINUX_NET_H_
#define _LINUX_NET_H_

#if defined(NO_OS_NET) && defined(LINUX_PLATFORM)

#include <stdint.h>
#include "no_os_net.h"
#include "no_os_socket.h"

/**
 * @struct linux_net_desc
 * @brief Linux backend state, stored in no_os_net_desc::extra.
 *
 * The host kernel owns the whole TCP/IP stack, so there is no device, no socket
 * pool and no netdev sublayer. Each no_os_socket_desc carries its file
 * descriptor directly in no_os_socket_desc::id, so this descriptor holds no
 * per-connection state and exists only for interface symmetry.
 */
struct linux_net_desc {
	/** Placeholder; the kernel holds all networking state. */
	uint8_t				reserved;
};

/** Network platform operations implementing the Linux adapter. */
extern const struct no_os_net_platform_ops linux_net_ops;

#endif /* NO_OS_NET && LINUX_PLATFORM */
#endif /* _LINUX_NET_H_ */

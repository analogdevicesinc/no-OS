/***************************************************************************//**
 *   @file   echo_lwip_example.c
 *   @brief  TCP echo server over Ethernet (lwIP / XEmacPs) for Xilinx boards.
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
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

#include <string.h>
#include <stdio.h>
#include "no_os_error.h"
#include "lwip_socket.h"
#include "lwip_xemacps.h"
#include "tcpecho_raw.h"
#include "parameters.h"
#include "echo_lwip_example.h"

/* MAC address - must be unique on the local network */
static uint8_t mac_addr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};

/***************************************************************************//**
 * @brief Run the TCP echo server on the board.
 *
 * Initialises the XEmacPs MAC, brings up the lwIP stack, starts the
 * tcpecho_raw server on port 7, and polls indefinitely.
 *
 * @return 0 on success, negative error code on failure.
*******************************************************************************/
int echo_lwip_example_main(void)
{
	struct lwip_network_desc *lwip_desc;
	struct xemacps_init_param gem_ip = {
		.device_id = GEM_DEVICE_ID,
	};
	struct lwip_network_param lwip_param = {
		.platform_ops = &xemacps_lwip_ops,
		.mac_param    = &gem_ip,
	};
	int ret;

	memcpy(gem_ip.hwaddr, mac_addr, sizeof(mac_addr));
	memcpy(lwip_param.hwaddr, mac_addr, sizeof(mac_addr));

	ret = no_os_lwip_init(&lwip_desc, &lwip_param);
	if (ret)
		return ret;

	tcpecho_raw_init();

	printf("echo: listening on port 7\n");

	while (1)
		no_os_lwip_step(lwip_desc, lwip_desc);

	no_os_lwip_remove(lwip_desc);

	return 0;
}

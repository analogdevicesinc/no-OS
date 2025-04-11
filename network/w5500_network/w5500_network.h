/***************************************************************************//**
*   @file   w5500_network.h
*   @brief  W5500 Network Interface
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

#ifndef W5500_NETWORK_H
#define W5500_NETWORK_H

#ifdef NO_OS_W5500_NETWORKING

#include "network_interface.h"
#include "w5500.h"

enum w5500_socket_role {
	W5500_ROLE_UNUSED,
	W5500_ROLE_SERVER,
	W5500_ROLE_CLIENT
};

struct w5500_socket_map {
	uint8_t                 physical_id;
	uint32_t                sock_id;
	uint8_t                 in_use;
	enum w5500_socket_role  role;
	uint16_t                local_port;
	uint16_t                remote_port;
	uint8_t                 remote_ip[4];
};


struct w5500_network_dev {
	struct                  w5500_dev *mac_dev;
	struct                  network_interface net_if;
	struct                  w5500_socket_map sockets[W5500_MAX_SOCK_NUMBER + 1];
	uint32_t                next_virtual_id;
	uint8_t                 ip[4];
	uint8_t                 netmask[4];
	uint8_t                 gateway[4];
};

struct w5500_network_init_param {
	void                    *mac_dev;
	struct w5500_init_param *w5500_ip;
};

/** Initialize the device */
int w5500_network_init(struct w5500_network_dev **net_dev,
		       struct w5500_network_init_param *init_param);

/** Free a device descriptor and release resources */
int w5500_network_remove(struct w5500_network_dev *dev);

#endif /* NO_OS_W5500_NETWORKING */
#endif /* W5500_NETWORK_H */

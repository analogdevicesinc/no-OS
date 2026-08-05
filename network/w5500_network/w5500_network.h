/***************************************************************************//**
*   @file   w5500_network.h
*   @brief  W5500 Network Interface
*   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
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

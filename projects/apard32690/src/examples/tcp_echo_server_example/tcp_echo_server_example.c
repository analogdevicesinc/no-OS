/***************************************************************************//**
 *   @file   tcp_echo_server_example.c
 *   @brief  Implementation of the TCP echo server example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "common_data.h"

#include "lwip_socket.h"
#include "tcp_socket.h"
#include "no_os_error.h"
#include "adin1110.h"
#include "network_interface.h"

/***************************************************************************//**
 * @brief TCP echo example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int tcp_echo_server_example_main()
{
	bool connected = false;
	struct tcp_socket_desc *server_socket;
	struct tcp_socket_desc *client_socket;
	struct lwip_network_desc *lwip_desc;
	struct tcp_socket_init_param tcp_ip = {
		.max_buff_size = 0
	};

	uint8_t read_byte;
	uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x60};
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	memcpy(lwip_ip.hwaddr, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);

	ret = no_os_lwip_init(&lwip_desc, &lwip_ip);
	if (ret)
		return ret;

	tcp_ip.net = &lwip_desc->no_os_net;

	ret = socket_init(&server_socket, &tcp_ip);
	if (ret)
		return ret;

	ret = socket_bind(server_socket, 10000);
	if (ret)
		return ret;

	ret = socket_listen(server_socket, MAX_BACKLOG);
	if (ret)
		return ret;

	while(1) {
		ret = socket_accept(server_socket, &client_socket);
		if (ret && ret != -EAGAIN)
			return ret;

		if (!ret) {
			connected = true;
		}

		no_os_lwip_step(server_socket->net->net, NULL);

		if (connected) {
			ret = socket_recv(client_socket, &read_byte, 1);
			if (ret > 0)
				socket_send(client_socket, &read_byte, ret);
		}
	}

	return 0;
}

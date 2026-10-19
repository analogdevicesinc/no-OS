/***************************************************************************//**
 *   @file   communication_and_forward_example.c
 *   @brief  Implementation of the APARDPFWD communication and forward
 *           example.
 *   @author Monica Constandachi (monica.constandachi@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
****************************************/

#include <stdio.h>
#include <string.h>
#include "common_data.h"

#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "tcp_socket.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "adin1110.h"
#include "network_interface.h"

#define SERVER_PORT		10000

/***************************************************************************//**
 * @brief Configure the output port of the AD-APARDPFWD-SL, remove the
 *        broadcast filter so unmatched frames are forwarded between T1L
 *        ports by the ADIN2111 switch, then open a TCP socket to
 *        communicate with the AD-APARD32690-SL.
 * @return ret - Result of the example execution.
*******************************************************************************/
int example_main()
{
	struct lwip_network_param lwip_ip = {
		.platform_ops = &adin1110_lwip_ops,
		.mac_param = &adin1110_ip,
	};

	struct tcp_socket_desc *server_socket;
	struct tcp_socket_desc *client_socket;
	struct lwip_network_desc *lwip_desc;
	struct tcp_socket_init_param tcp_ip = {
		.max_buff_size = 0
	};

	struct no_os_uart_desc *uart_desc;
	struct adin1110_desc *adin_desc;

	uint32_t device_id;
	uint8_t read_byte;
	bool connected = false;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		pr_err("UART initialization failed (%d)\n", ret);
		return ret;
	}

	no_os_uart_stdio(uart_desc);

	ret = spi_cfg_0(adin1110_spi_cfg_0);
	if (ret) {
		pr_err("ADIN2111 SPI configuration failed (%d)\n", ret);
		goto remove_uart;
	}

	/* Enable Port 2 on PFWD shield (LOW = enabled) */
	ret = port2_cfg(port2_cfg_0, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("AD-APARDPFWD output port configuration failed (%d)\n", ret);
		goto remove_uart;
	}

	pr_info("AD-APARDPFWD COMMUNICATION AND FORWARD EXAMPLE.\n");

	memcpy(lwip_ip.hwaddr, adin1110_ip.mac_address, NETIF_MAX_HWADDR_LEN);

	ret = no_os_lwip_init(&lwip_desc, &lwip_ip);
	if (ret) {
		pr_err("LWIP initialization failed (%d)\n", ret);
		goto remove_uart;
	}

	adin_desc = (struct adin1110_desc *)lwip_desc->mac_desc;

	if (adin_desc) {
		printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
		       adin_desc->mac_address[0],
		       adin_desc->mac_address[1],
		       adin_desc->mac_address[2],
		       adin_desc->mac_address[3],
		       adin_desc->mac_address[4],
		       adin_desc->mac_address[5]);
	} else {
		pr_err("MAC address is NULL (%d)\n", ret);
		goto remove_lwip;
	}

	/*
	 * Remove broadcast filter so unmatched broadcasts are forwarded
	 * between T1L ports by the ADIN2111 switch.
	 */
	ret = adin1110_broadcast_filter(adin_desc, false);
	if (ret) {
		pr_err("Error disabling the broadcast filter (%d)\n", ret);
		goto remove_lwip;
	}

	/*
	 * Enable FWD_UNK2HOST so the host still receives unmatched frames
	 * (including broadcasts for ARP) while the switch forwards them.
	 */
	ret = adin1110_set_promisc(adin_desc, 0, true);
	if (ret) {
		pr_err("Error enabling promiscuous mode (%d)\n", ret);
		goto remove_lwip;
	}

	ret = adin1110_reg_read(adin_desc, ADIN1110_PHY_ID_REG, &device_id);
	if (ret) {
		pr_err("Error reading the ADIN1110's device id (%d)\n", ret);
		goto remove_lwip;
	}

	pr_info("Got device id 0x%X\n", device_id);

	tcp_ip.net = &lwip_desc->no_os_net;

	ret = socket_init(&server_socket, &tcp_ip);
	if (ret) {
		pr_err("Socket initialization failed (%d)\n", ret);
		goto remove_lwip;
	}

	ret = socket_bind(server_socket, SERVER_PORT);
	if (ret) {
		pr_err("Socket bind failed (%d)\n", ret);
		goto remove_server_socket;
	}

	ret = socket_listen(server_socket, MAX_BACKLOG);
	if (ret) {
		pr_err("Socket listen failed (%d)\n", ret);
		goto remove_server_socket;
	}

	pr_info("Command server listening on port %d\n", SERVER_PORT);

	while (1) {
		no_os_lwip_step(server_socket->net->net, NULL);

		if (connected) {
			ret = socket_recv(client_socket, &read_byte, 1);
			if (ret > 0) {
				ret = socket_send(client_socket, &read_byte, ret);
				if (ret < 0) {
					pr_err("Socket send failed (%d), closing\n", ret);
					socket_remove(client_socket);
					connected = false;
					continue;
				}
				pr_info("%c", read_byte);
			} else if (ret < 0 && ret != -EAGAIN) {
				pr_err("Socket recv failed (%d), closing\n", ret);
				socket_remove(client_socket);
				connected = false;
			}
		} else {
			ret = socket_accept(server_socket, &client_socket);
			if (ret && ret != -EAGAIN) {
				pr_err("Socket accept failed (%d)\n", ret);
				goto remove_server_socket;
			}

			if (!ret) {
				connected = true;
				pr_info("Client connected\n");
			}
		}
	}

remove_server_socket:
	socket_remove(server_socket);

remove_lwip:
	no_os_lwip_remove(lwip_desc);

remove_uart:
	no_os_uart_remove(uart_desc);

	return 0;
}

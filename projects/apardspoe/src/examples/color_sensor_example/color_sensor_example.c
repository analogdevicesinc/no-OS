/***************************************************************************//**
 *   @file   color_sensor_example.c
 *   @brief  APARDSPOE color sensor example for AD-APARD32690-SL with
 *           AD-APARDSPOE-SL hat chained with an AD-APARDPFWD-SL. Reads RGBC
 *           data from a TCS34725 color sensor and exposes it over a TCP
 *           command server on the ADIN1110 link.
 *   @author Monica Constandachi (monica.constandachi@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "common_data.h"

#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "tcp_socket.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "adin1110.h"
#include "tcs34725.h"
#include "network_interface.h"

#define SERVER_PORT		10000
#define CMD_BUF_SIZE		64
#define RESP_BUF_SIZE		64
#define COLOR_PRINT_INTERVAL_MS	200

static int process_command(const char *cmd, char *resp,
			   struct tcs34725_desc *color_desc)
{
	uint16_t r, g, b, c;
	int ret;

	if (strcmp(cmd, "COLOR_READ") == 0) {
		ret = tcs34725_get_rgbc(color_desc, &r, &g, &b, &c);
		if (ret)
			return sprintf(resp, "ERR:READ_FAILED\n");

		return sprintf(resp, "R:%u G:%u B:%u C:%u\n", r, g, b, c);
	}

	return sprintf(resp, "ERR:UNKNOWN_CMD\n");
}

/***************************************************************************//**
 * @brief Configure the output port of the AD-APARDPFWD-SL, initialize the
 *        TCS34725 color sensor, then open a TCP socket to accept commands.
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
	struct tcs34725_desc *color_desc;

	char cmd_buf[CMD_BUF_SIZE];
	char resp_buf[RESP_BUF_SIZE];
	int cmd_idx = 0;

	uint32_t device_id;
	uint8_t color_id;
	uint8_t read_byte;
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
		pr_err("AD-APARDSPOE output port configuration failed (%d)\n", ret);
		goto remove_uart;
	}

	pr_info("AD-APARDSPOE COLOR SENSOR COMMAND SERVER.\n");

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
		ret = -ENOMEM;
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

	/* Initialize the TCS34725 color sensor over I2C */
	ret = tcs34725_init(&color_desc, &tcs34725_ip);
	if (ret) {
		pr_err("TCS34725 initialization failed (%d)\n", ret);
		goto remove_lwip;
	}

	printf("---- I2C debug ----\n");
	printf("bus (device_id) = %u\n", color_desc->comm_desc->device_id);
	printf("slave_address   = 0x%02x (7-bit)\n",
	       color_desc->comm_desc->slave_address);
	printf("max_speed_hz    = %lu\n",
	       (unsigned long)color_desc->comm_desc->max_speed_hz);

	/* Wait for TCS34725 power-on (~2.4ms typ) before first I2C access. */
	no_os_mdelay(10);

	ret = tcs34725_reg_read(color_desc, TCS34725_ID, &color_id);
	if (ret) {
		pr_err("Error reading the TCS34725's device id (%d)\n", ret);
		goto remove_color;
	}

	pr_info("Got TCS34725 device id 0x%X\n", color_id);

	if (color_id != 0x44) {
		printf("Unexpected device id: 0x%X (expected 0x44 for TCS34725)\n",
		       color_id);
		ret = -ENODEV;
		goto remove_color;
	}
	printf("Device id matches TCS34725 (0x44)\n");

	ret = tcs34725_reg_write(color_desc, TCS34725_ENABLE, TCS34725_ENABLE_PON);
	if (ret) {
		pr_err("Error powering on the TCS34725 (%d)\n", ret);
		goto remove_color;
	}

	/* PON requires a 2.4 ms warm-up before AEN can be asserted. */
	no_os_mdelay(3);

	ret = tcs34725_reg_write(color_desc, TCS34725_ENABLE,
				 TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	if (ret) {
		pr_err("Error enabling the TCS34725's RGBC engine (%d)\n", ret);
		goto remove_color;
	}

	pr_info("TCS34725 color sensor initialized\n");

	tcp_ip.net = &lwip_desc->no_os_net;

	ret = socket_init(&server_socket, &tcp_ip);
	if (ret) {
		pr_err("Socket initialization failed (%d)\n", ret);
		goto remove_color;
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

	bool connected = false;

	struct no_os_time now;
	uint64_t last_print_us = 0;
	uint64_t now_us;
	uint16_t r, g, b, c;

	while (1) {
		no_os_lwip_step(server_socket->net->net, NULL);

		now = no_os_get_time();
		now_us = (uint64_t)now.s * 1000000 + now.us;

		if (now_us - last_print_us >= COLOR_PRINT_INTERVAL_MS * 1000) {
			last_print_us = now_us;

			ret = tcs34725_get_rgbc(color_desc, &r, &g, &b, &c);
			if (!ret)
				printf("R:%u, G:%u, B:%u, C:%u\n", r, g, b, c);
		}

		if (connected) {
			ret = socket_recv(client_socket, &read_byte, 1);
			if (ret > 0) {
				if (read_byte == '\n' || read_byte == '\r') {
					if (cmd_idx > 0) {
						cmd_buf[cmd_idx] = '\0';
						pr_info("CMD: %s\n", cmd_buf);

						int resp_len = process_command(
								       cmd_buf, resp_buf,
								       color_desc);
						ret = socket_send(client_socket,
								  (uint8_t *)resp_buf,
								  resp_len);
						if (ret < 0) {
							pr_err("Socket send failed (%d), closing\n",
							       ret);
							socket_remove(client_socket);
							connected = false;
							cmd_idx = 0;
							continue;
						}

						pr_info("RSP: %s", resp_buf);
						cmd_idx = 0;
					}
				} else if (cmd_idx < CMD_BUF_SIZE - 1) {
					cmd_buf[cmd_idx++] = (char)read_byte;
				} else {
					pr_err("Command too long, discarding\n");
					cmd_idx = 0;
				}
			} else if (ret < 0 && ret != -EAGAIN) {
				pr_err("Socket recv failed (%d), closing\n",
				       ret);
				socket_remove(client_socket);
				connected = false;
				cmd_idx = 0;
			}
		} else {
			ret = socket_accept(server_socket, &client_socket);
			if (ret && ret != -EAGAIN) {
				pr_err("Socket accept failed (%d)\n", ret);
				goto remove_server_socket;
			}

			if (!ret) {
				connected = true;
				cmd_idx = 0;
				pr_info("Client connected\n");
			}
		}
	}

remove_server_socket:
	socket_remove(server_socket);

remove_color:
	tcs34725_remove(color_desc);

remove_lwip:
	no_os_lwip_remove(lwip_desc);

remove_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}

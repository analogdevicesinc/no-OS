/***************************************************************************//**
 *   @file   apard_servo_control.c
 *   @brief  Implementation of the APARD servo control example.
 *           Drives two hobby servos via PWM (50 Hz) and exposes
 *           SERVO1_ON / SERVO1_OFF / SERVO2_ON / SERVO2_OFF /
 *           SERVO_STATUS commands over a TCP socket on the ADIN1110
 *           link.
 *   @author Tudor Gansca (tudor.gansca@analog.com)
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
#include "no_os_pwm.h"
#include "maxim_pwm.h"
#include "adin1110.h"
#include "network_interface.h"

#define SERVER_PORT		10000
#define CMD_BUF_SIZE		64
#define RESP_BUF_SIZE		64

#define PWM_PERIOD_NS		20000000U	/* 50 Hz */
#define SERVO_PULSE_OFF_NS	1500000U	/* 1.5 ms — initial position */
#define SERVO_PULSE_ON_NS	2000000U	/* 2.0 ms — ~45° from initial */

static struct max_pwm_init_param servo_pwm_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

static struct no_os_pwm_init_param servo_pwm_ip = {
	.id = 1,
	.period_ns = PWM_PERIOD_NS,
	.duty_cycle_ns = SERVO_PULSE_OFF_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = &max_pwm_ops,
	.extra = &servo_pwm_extra,
};

static struct max_pwm_init_param servo2_pwm_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

static struct no_os_pwm_init_param servo2_pwm_ip = {
	.id = 2,
	.period_ns = PWM_PERIOD_NS,
	.duty_cycle_ns = SERVO_PULSE_OFF_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = &max_pwm_ops,
	.extra = &servo2_pwm_extra,
};

static struct no_os_pwm_desc *servo_desc;
static struct no_os_pwm_desc *servo2_desc;
static bool servo_on;
static bool servo2_on;

static int process_command(const char *cmd, char *resp)
{
	int ret;

	if (strcmp(cmd, "SERVO1_ON") == 0) {
		ret = no_os_pwm_set_duty_cycle(servo_desc, SERVO_PULSE_ON_NS);
		if (ret)
			return sprintf(resp, "ERR:SET_DUTY_FAILED\n");

		servo_on = true;
		return sprintf(resp, "OK\n");
	}

	if (strcmp(cmd, "SERVO1_OFF") == 0) {
		ret = no_os_pwm_set_duty_cycle(servo_desc, SERVO_PULSE_OFF_NS);
		if (ret)
			return sprintf(resp, "ERR:SET_DUTY_FAILED\n");

		servo_on = false;
		return sprintf(resp, "OK\n");
	}

	if (strcmp(cmd, "SERVO2_ON") == 0) {
		ret = no_os_pwm_set_duty_cycle(servo2_desc, SERVO_PULSE_ON_NS);
		if (ret)
			return sprintf(resp, "ERR:SET_DUTY_FAILED\n");

		servo2_on = true;
		return sprintf(resp, "OK\n");
	}

	if (strcmp(cmd, "SERVO2_OFF") == 0) {
		ret = no_os_pwm_set_duty_cycle(servo2_desc, SERVO_PULSE_OFF_NS);
		if (ret)
			return sprintf(resp, "ERR:SET_DUTY_FAILED\n");

		servo2_on = false;
		return sprintf(resp, "OK\n");
	}

	if (strcmp(cmd, "SERVO_STATUS") == 0)
		return sprintf(resp, "SERVO1:%s SERVO2:%s\n",
			       servo_on ? "ON" : "OFF",
			       servo2_on ? "ON" : "OFF");

	return sprintf(resp, "ERR:UNKNOWN_CMD\n");
}

/***************************************************************************//**
 * @brief Configure the output port of the AD-APARDPFWD-SL, initialize the
 *        servo PWM, then open a TCP socket to accept commands.
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

	char cmd_buf[CMD_BUF_SIZE];
	char resp_buf[RESP_BUF_SIZE];
	int cmd_idx = 0;

	uint32_t device_id;
	uint8_t read_byte;
	bool connected;
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

	pr_info("AD-APARDPFWD SERVO COMMAND SERVER.\n");

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
		ret = -ENODEV;
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

	/* Initialize servo 1 PWM at the OFF (initial) position */
	ret = no_os_pwm_init(&servo_desc, &servo_pwm_ip);
	if (ret) {
		pr_err("Servo 1 PWM init failed (%d)\n", ret);
		goto remove_lwip;
	}

	servo_on = false;

	pr_info("Servo 1 PWM initialized on TMR%lu (50 Hz, 1.5 ms initial)\n",
		(unsigned long)servo_pwm_ip.id);

	/* Initialize servo 2 PWM at the OFF (initial) position */
	ret = no_os_pwm_init(&servo2_desc, &servo2_pwm_ip);
	if (ret) {
		pr_err("Servo 2 PWM init failed (%d)\n", ret);
		goto remove_pwm;
	}

	servo2_on = false;

	pr_info("Servo 2 PWM initialized on TMR%lu (50 Hz, 1.5 ms initial)\n",
		(unsigned long)servo2_pwm_ip.id);

	tcp_ip.net = &lwip_desc->no_os_net;

	ret = socket_init(&server_socket, &tcp_ip);
	if (ret) {
		pr_err("Socket initialization failed (%d)\n", ret);
		goto remove_pwm2;
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

	connected = false;
	cmd_idx = 0;

	while (1) {
		no_os_lwip_step(server_socket->net->net, NULL);

		if (connected) {
			ret = socket_recv(client_socket, &read_byte, 1);
			if (ret > 0) {
				if (read_byte == '\n' || read_byte == '\r') {
					if (cmd_idx > 0) {
						cmd_buf[cmd_idx] = '\0';
						pr_info("CMD: %s\n", cmd_buf);

						int resp_len = process_command(
								       cmd_buf, resp_buf);
						socket_send(client_socket,
							    (uint8_t *)resp_buf,
							    resp_len);

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

remove_pwm2:
	no_os_pwm_remove(servo2_desc);

remove_pwm:
	no_os_pwm_remove(servo_desc);

remove_lwip:
	no_os_lwip_remove(lwip_desc);

remove_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}

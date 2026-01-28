/***************************************************************************//**
 *   @file   mqtt_freertos.c
 *   @brief  Implementation of the mqtt freertos example
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "common_data.h"

#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"

#include "mqtt_client.h"
#include "mqtt_noos_support.h"
#include "mbedtls/debug.h"

#include "tcp_socket.h"
#include "network_interface.h"

#if (CONNECTION_TYPE == 1)
#include "certs.h"
#endif

#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_timer.h"
#include "maxim_trng.h"
#include "maxim_timer.h"

const struct max_spi_init_param spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIO,
};

const struct no_os_gpio_init_param adin1110_rst_gpio_ip = {
	.port = ADIN1110_RESET_PORT,
	.number = ADIN1110_RESET_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param){
		.vssel = 0
	},
};

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = ADIN1110_SPI_DEVICE_ID,
	.max_speed_hz = ADIN1110_SPI_CLK_SPEED,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = ADIN1110_SPI_CHIP_SEL,
	.extra = &spi_extra,
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_rst_gpio_ip,
	.mac_address = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80},
	.append_crc = false,
	.oa_tc6_spi = true,
	.oa_tc6_prote = true,
};

struct lwip_network_param lwip_ip = {
	.platform_ops = &adin1110_lwip_ops,
	.mac_param = &adin1110_ip,
};

/***************************************************************************//**
 * @brief MQTT Example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
static void message_handler(struct mqtt_message_data *msg)
{
	msg->message.payload[msg->message.len] = 0;
	printf("Topic:%s -- Payload: %s\n", msg->topic, msg->message.payload);
}

int mqtt_freertos_main()
{

	int ret;
	uint8_t send_buff[256];
	uint8_t read_buff[256];
	char val_buff[32];
	uint32_t msg_len;

	struct no_os_uart_desc *uart_desc;
	struct lwip_network_desc *lwip_desc;
	struct tcp_socket_desc *tcp_socket;

	uint8_t adin1110_mac_address[6] = {0x00, 0xe0, 0x22, 0x03, 0x25, 0x60};

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;
	no_os_uart_stdio(uart_desc);

	printf("Starting MQTT Example\n");

	struct no_os_timer_init_param timer_param = {
		.id = 0,                      // Timer 0
		.freq_hz = 32000,             // 32 kHz clock
		.ticks_count = 0,        // Free mode
		.platform_ops = &max_timer_ops,
		.extra = NULL                 // No additional parameters needed
	};

	uint32_t connect_timeout = 5000;

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	memcpy(lwip_ip.hwaddr, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);

	ret = no_os_lwip_init(&lwip_desc, &lwip_ip);
	if (ret){
		printf("LWIP init error: %d (%s)\n", ret, strerror(-ret));
		return ret;
	}

	struct tcp_socket_init_param tcp_ip = {
		.net = &lwip_desc->no_os_net,
		.max_buff_size = 0
	};

	#if (CONNECTION_TYPE == 1)
	char ca_cert[] = CA_CERT;
	char client_cert[] = DEVICE_CERT;
	char pki[] = DEVICE_PRIVATE_KEY;
	#endif

	struct no_os_trng_init_param trng_ip = {
		.platform_ops = &max_trng_ops
	};

	#if (CONNECTION_TYPE == 1)
	struct secure_init_param secure_params = {
		.trng_init_param = &trng_ip,
		.ca_cert = ca_cert,
		.ca_cert_len = NO_OS_ARRAY_SIZE(ca_cert),
		.cli_cert = client_cert,
		.cli_cert_len = NO_OS_ARRAY_SIZE(client_cert),
		.cli_pk = pki, 
		.cli_pk_len = NO_OS_ARRAY_SIZE(pki),
		.cert_verify_mode = MBEDTLS_SSL_VERIFY_NONE
	};
	tcp_ip.secure_init_param = &secure_params;
	#endif

	ret = socket_init(&tcp_socket, &tcp_ip);
	if (ret){
		printf("Socket init error: %d (%s)\n", ret, strerror(-ret));
		return ret;
	}

	struct socket_address ip_addr = {
		.port = MQTT_SERVER_PORT,
		.addr = MQTT_SERVER_IP
	};

	struct mqtt_desc *mqtt;
	struct mqtt_init_param	mqtt_init_param = {
		.sock = tcp_socket,
		.timer_init_param = &timer_param,
		.command_timeout_ms = 50000,
		.send_buff = send_buff,
		.read_buff = read_buff,
		.send_buff_size = 256,
		.read_buff_size = 256,
		.message_handler = message_handler
	};

	ret = mqtt_init(&mqtt, &mqtt_init_param);
	if (ret) {
		printf("MQTT init error: %d (%s)\n", ret, strerror(-ret));
		return ret;
	}

	struct mqtt_connect_config conn_config = {
		.version = MQTT_VERSION_3_1_1,
		.keep_alive_ms = 1000,
		.client_name = "maxim",
		.username = NULL,
		.password = NULL
	};

	ret = socket_connect(tcp_socket, &ip_addr);
	if (ret) {
		printf("Couldn't connect to the remote TCP socket: %d (%s)\n", ret,
		       strerror(-ret));
		return ret;
	}

	while (connect_timeout--) {
		no_os_lwip_step(tcp_socket->net->net, NULL);
		no_os_mdelay(1);
	}

	ret = mqtt_connect(mqtt, &conn_config, NULL);
	if (ret) {
		socket_disconnect(tcp_socket);
		printf("Couldn't connect to the MQTT broker: %d (%s)\n", ret, strerror(-ret));
		return ret;
	}

	struct mqtt_message test_msg = {
		.qos = 0,
		.payload = "Sensor value: 28",
		.len = strlen(test_msg.payload),
		.retained = false
	};

	while (1) {
		no_os_lwip_step(tcp_socket->net->net, NULL);
		ret = mqtt_publish(mqtt, "Sensor/Temperature", &test_msg);
		printf("MQTT Message: %s\n", test_msg.payload);
		no_os_mdelay(1000);
	}

	return 0;
}

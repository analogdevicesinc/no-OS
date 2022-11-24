/***************************************************************************//**
 *   @file   iio_app.c
 *   @brief  C file of iio_app
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifdef IIO_SUPPORT

#include <stdlib.h>
#include <stdio.h>
#include "iio.h"
#include "iio_app.h"
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_delay.h"
#include "mqtt_client.h"

#if defined(ADUCM_PLATFORM)
#include "aducm3029_uart.h"
#include "aducm3029_irq.h"
#include "aducm3029_timer.h"
#endif
#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
#include "no_os_irq.h"
#include "no_os_error.h"
#endif
#if defined(XILINX_PLATFORM)
#include "xilinx_uart.h"
#include "xilinx_irq.h"
#endif
#if defined(STM32_PLATFORM)
#include <errno.h>
#include "stm32_uart.h"
#include "stm32_irq.h"
#include "no_os_irq.h"
#include "no_os_error.h"
#endif

#if defined(MAXIM_PLATFORM)
#include "maxim_irq.h"
#include "maxim_uart.h"
#include "no_os_irq.h"
#include "no_os_error.h"
#endif

#ifdef NO_OS_NETWORKING
/* Fix: Use static buffers instead of calloc for new connections */
#warning "iio may not work with WIFI on aducm3029."
#ifdef ADUCM_PLATFORM
#include "wifi.h"
#endif
#include "tcp_socket.h"
#endif

#ifdef LINUX_PLATFORM
#include "linux_socket.h"
#include "tcp_socket.h"
#include "no_os_error.h"
#endif

#ifdef PICO_PLATFORM
#include "no_os_error.h"
#endif

// The default baudrate iio_app will use to print messages to console.
#define UART_BAUDRATE_DEFAULT	115200
#define PRINT_ERR_AND_RET(msg, ret) do {\
	printf("%s - Code: %d (-0x%x) \n", msg, ret, ret);\
	return ret;\
} while (0)


static inline uint32_t _calc_uart_xfer_time(uint32_t len, uint32_t baudrate)
{
	uint32_t ms = 1000ul * len * 8 / UART_BAUDRATE_DEFAULT;
	ms += ms / 10; // overhead
	return ms;
}

static int32_t print_uart_hello_message(struct no_os_uart_desc **uart_desc,
					struct no_os_uart_init_param *uart_init_par)
{
#if defined(LINUX_PLATFORM) || defined(NO_OS_NETWORKING)
	return 0;
#else
	const char *uart_data_size[] = { "5", "6", "7", "8", "9" };
	const char *uart_parity[] = { "none", "mark", "space", "odd", "even" };
	const char *uart_stop[] = { "1", "2" };
	char message[512];
	uint32_t msglen = sprintf(message,
				  "Running TinyIIOD server...\n"
				  "If successful, you may connect an IIO client application by:\n"
				  "1. Disconnecting the serial terminal you use to view this message.\n"
				  "2. Connecting the IIO client application using the serial backend configured as shown:\n"
				  "\tBaudrate: %d\n"
				  "\tData size: %s bits\n"
				  "\tParity: %s\n"
				  "\tStop bits: %s\n"
				  "\tFlow control: none\n",
				  UART_BAUDRATE,
				  uart_data_size[uart_init_par->size],
				  uart_parity[uart_init_par->parity],
				  uart_stop[uart_init_par->stop]);

	return iio_print_uart_info_message(uart_desc, uart_init_par, message,
					   msglen);
#endif
}

static int32_t print_uart_error_message(struct no_os_uart_desc **uart_desc,
					struct no_os_uart_init_param *uart_init_par,
					int32_t status)
{
	char message[512];
	uint32_t msglen = sprintf(message,
				  "TinyIIOD server failed with code %d.\n",
				  (int)status);
#if defined(LINUX_PLATFORM) || defined(NO_OS_NETWORKING)
	(void)msglen;
	printf("%s", message);
	return 0;
#else
	return iio_print_uart_info_message(uart_desc, uart_init_par, message,
					   msglen);
#endif
}

//void mqtt_message_handler(struct mqtt_message_data *msg)
//{
//	char	buff[101];
//	int32_t	len;
//
//	/* Message.payload don't have at the end '\0' so we have to add it. */
//	len = msg->message.len > 100 ? 100 : msg->message.len;
//	memcpy(buff, msg->message.payload, len);
//	buff[len] = 0;
//
//	printf("Topic:%s -- Payload: %s\n", msg->topic, buff);
//}


//int32_t read_and_send(struct mqtt_desc *mqtt)
//{
//	struct mqtt_message	msg;
//	uint8_t			buff[100];
//	uint32_t		len;
//
//	/* Serialize data */
//	len = sprintf(buff, "X:adsd -- Y: dsadsad -- Z: dasdad -- Temp: dasda");
//	/* Send data to mqtt broker */
//	msg = (struct mqtt_message) {
//		.qos = MQTT_QOS0,
//		.payload = buff,
//		.len = len,
//		.retained = false
//	};
//	return mqtt_publish(mqtt, MQTT_PUBLISH_TOPIC, &msg);
//}

#if defined(NO_OS_NETWORKING) || defined(LINUX_PLATFORM)
static int32_t network_setup(struct iio_init_param *iio_init_param,
			     struct no_os_uart_desc *uart_desc,
			     void *irq_desc)
{
	static struct tcp_socket_init_param socket_param;

#ifdef ADUCM_PLATFORM
	int32_t status;
	static struct wifi_desc *wifi;
	struct wifi_init_param wifi_param = {
		.irq_desc = irq_desc,
		.uart_desc = uart_desc,
		.uart_irq_conf = uart_desc,
		.uart_irq_id = UART_IRQ_ID
	};
	status = wifi_init(&wifi, &wifi_param);
	if (status < 0)
		return status;

	status = wifi_connect(wifi, WIFI_SSID, WIFI_PWD);
	if (status < 0)
		return status;

	char buff[100];
	wifi_get_ip(wifi, buff, 100);
	printf("Tinyiiod ip is: %s\n", buff);

	wifi_get_network_interface(wifi, &socket_param.net);
#endif

	socket_param.max_buff_size = 0;
	iio_init_param->phy_type = USE_NETWORK;
	iio_init_param->tcp_socket_init_param = &socket_param;

	static struct tcp_socket_desc	*sock;
	status = socket_init(&sock, &socket_param);
		if (NO_OS_IS_ERR_VALUE(status))
			PRINT_ERR_AND_RET("Error socket_init", status);

	struct socket_address		mqtt_broker_addr;
	/* Connect socket to mqtt borker server */
	mqtt_broker_addr = (struct socket_address) {
		.addr = SERVER_ADDR,
		.port = SERVER_PORT
	};
	status = socket_connect(sock, &mqtt_broker_addr);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error socket_connect", status);

	printf("Connection with \"%s\" established\n", SERVER_ADDR);

	static uint8_t			send_buff[BUFF_LEN];
	static uint8_t			read_buff[BUFF_LEN];
	struct mqtt_init_param		mqtt_init_param;
	/* Initialize mqtt descriptor */
	mqtt_init_param = (struct mqtt_init_param) {
		.timer_id = TIMER_ID,
		.extra_timer_init_param = &aducm3029_timer_ops,
		.sock = sock,
		.command_timeout_ms = MQTT_CONFIG_CMD_TIMEOUT,
		.send_buff = send_buff,
		.read_buff = read_buff,
		.send_buff_size = BUFF_LEN,
		.read_buff_size = BUFF_LEN,
//		.message_handler = mqtt_message_handler
	};
	struct mqtt_desc	*mqtt;

	status = mqtt_init(&mqtt, &mqtt_init_param);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_init", status);

	struct mqtt_connect_config	conn_config;
	/* Mqtt configuration */
	/* Connect to mqtt broker */
	conn_config = (struct mqtt_connect_config) {
		.version = MQTT_CONFIG_VERSION,
		.keep_alive_ms = MQTT_CONFIG_KEEP_ALIVE,
		.client_name = MQTT_CONFIG_CLIENT_NAME,
		.username = MQTT_CONFIG_CLI_USER,
		.password = MQTT_CONFIG_CLI_PASS
	};


	status = mqtt_connect(mqtt, &conn_config, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_connect", status);

	printf("Connected to mqtt broker\n");

	/* Subscribe for a topic */
	status = mqtt_subscribe(mqtt, MQTT_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_subscribe", status);
	printf("Subscribed to topic: %s\n", MQTT_SUBSCRIBE_TOPIC);

	while (true) {

//		status = read_and_send(mqtt);
//		if (NO_OS_IS_ERR_VALUE(status))
//			PRINT_ERR_AND_RET("Error read_and_send", status);
//		printf("Data sent to broker\n");

//		/* Dispatch new mqtt mesages if any during SCAN_SENSOR_TIME */
//		status = mqtt_yield(mqtt, SCAN_SENSOR_TIME);
//		if (NO_OS_IS_ERR_VALUE(status))
//			PRINT_ERR_AND_RET("Error mqtt_yield", status);
	}


	return 0;
}
#endif

static int32_t uart_setup(struct no_os_uart_desc **uart_desc,
			  struct no_os_uart_init_param **uart_init_par,
			  void *irq_desc)
{
	static struct no_os_uart_init_param luart_par = {
		.device_id = UART_DEVICE_ID,
		/* TODO: remove this ifdef when asynchrounous rx is implemented on every platform. */
#if defined(STM32_PLATFORM) || defined(MAXIM_PLATFORM) || defined(ADUCM_PLATFORM) || defined(PICO_PLATFORM)
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
#endif
		.baud_rate = UART_BAUDRATE_DEFAULT,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
	};
	*uart_init_par = &luart_par;

	return no_os_uart_init(uart_desc, &luart_par);
}

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB)) || (defined(STM32_PLATFORM)) || defined(MAXIM_PLATFORM)
static int32_t irq_setup(struct no_os_irq_ctrl_desc **irq_desc)
{
	int32_t status;

	void *platform_irq_init_par = NULL;
	const struct no_os_irq_platform_ops *platform_irq_ops = &aducm_irq_ops;

	struct no_os_irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.platform_ops = platform_irq_ops,
		.extra = platform_irq_init_par
	};

	status = no_os_irq_ctrl_init(irq_desc, &irq_init_param);
	if (status < 0)
		return status;

	return no_os_irq_global_enable(*irq_desc);
}
#endif


/**
 * @brief IIO Application API with trigger initialization.
 * @param devices  - IIO devices to be used.
 * @param nb_devs  - Number of devices to be used.
 * @param trigs    - IIO triggers to be used.
 * @param nb_trigs - Number of triggers to be used.
 * @param irq_desc - IRQ descriptor to be used
 * @param iio_desc - IIO descriptor to be returned.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t iio_app_run_with_trigs(struct iio_app_device *devices, uint32_t nb_devs,
			       struct iio_trigger_init *trigs, int32_t nb_trigs,
			       void *irq_desc, struct iio_desc **iio_desc)
{
	int32_t			status;
	struct iio_init_param	iio_init_param;
	struct no_os_uart_desc	*uart_desc;
	struct no_os_uart_init_param	*uart_init_par;
	struct iio_device_init	*iio_init_devs;
	uint32_t		i;
	struct iio_data_buffer *buff;

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB)) || defined(STM32_PLATFORM)
	/* Only one irq controller can exist and be initialized in
	 * any of the iio_devices. */
	for (i = 0; i < nb_devs; i++) {
		if (devices[i].dev_descriptor->irq_desc) {
			irq_desc = (struct no_os_irq_ctrl_desc *)devices[i].dev_descriptor->irq_desc;
			break;
		}
	}

	if (!irq_desc) {
		status = irq_setup((struct no_os_irq_ctrl_desc **)&irq_desc);
		if (status < 0)
			return status;
	}
#endif

	status = uart_setup(&uart_desc, &uart_init_par, irq_desc);
	if (status < 0)
		return status;

#if !defined(NO_OS_NETWORKING) || (defined(NO_OS_NETWORKING) && defined(XILINX_PLATFORM))
	status = print_uart_hello_message(&uart_desc, uart_init_par);
	if (status < 0)
		return status;
#endif

#if defined(NO_OS_NETWORKING) || defined(LINUX_PLATFORM)
	status = network_setup(&iio_init_param, uart_desc, irq_desc);
	if(status < 0)
		goto error;
#else
	iio_init_param.phy_type = USE_UART;
	iio_init_param.uart_desc = uart_desc;
#endif


error:
	status = print_uart_error_message(&uart_desc, uart_init_par, status);
	return status;
}

int32_t iio_app_run(struct iio_app_device *devices, uint32_t len)
{
	struct iio_desc	*iio_desc;
	void *irq_desc = NULL;

	return iio_app_run_with_trigs(devices, len, NULL, 0, irq_desc, &iio_desc);
}

#endif

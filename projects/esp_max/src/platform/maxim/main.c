/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of iio_demo project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_error.h"

#include <stdlib.h>
#include <stdio.h>
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_timer.h"
#include "mqtt_client.h"
#include "ade9430.h"
#include "nhd_c12832a1z.h"

#include "az_iot_hub_client.h"
#include "az_iot_provisioning_client.h"
#include "iot_sample_common.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_irq.h"
#include "maxim_timer.h"
#include "no_os_irq.h"
#include "no_os_error.h"

#include "wifi.h"
#include "tcp_socket.h"
#include "mbedtls/config.h"

// The default baudrate iio_app will use to print messages to console.
#define PRINT_ERR_AND_RET(msg, ret) do {\
	printf("%s - Code: %d (-0x%x) \n", msg, ret, ret);\
	return ret;\
} while (0)

az_iot_hub_client my_client;
static az_span my_iothub_hostname = AZ_SPAN_LITERAL_FROM_STR(SERVER_ADDR);
static az_span my_device_id = AZ_SPAN_LITERAL_FROM_STR(AZ_IOT_PROVISIONING_REGISTRATION_ID);
static az_span my_request_id = AZ_SPAN_LITERAL_FROM_STR("1");

// Make sure to size the buffer to fit the client id (16 is an example)
static char my_mqtt_client_id[64];
static size_t my_mqtt_client_id_length;

static const char my_ca_cert[] = CA_CERT;
static const char my_cli_cert[] = DEVICE_CERT;
static const char my_cli_pk[] = DEVICE_PRIVATE_KEY;

// Make sure to size the buffer to fit the username (128 is an example)
static char my_mqtt_user_name[256];
static size_t my_mqtt_user_name_length;

static char telemetry_topic[128];
static char reported_topic[128];

static az_span custom_registration_payload_property
    = AZ_SPAN_LITERAL_FROM_STR(AZ_IOT_PROVISIONING_CUSTOM_PAYLOAD);

static iot_sample_environment_variables env_vars;
static az_iot_provisioning_client provisioning_client;
static char iot_hub_endpoint_buffer[128];
static char iot_hub_device_id_buffer[128];
static az_span device_iot_hub_endpoint;
static az_span device_id;
static char mqtt_client_username_buffer[256];
static char mqtt_client_id_buffer[CLIENT_ID_BUFFER_LENGTH];
static char mqtt_endpoint_buffer[PROVISIONING_ENDPOINT_BUFFER_LENGTH];

static uint8_t mqtt_payload[MQTT_PAYLOAD_BUFFER_LENGTH];
static size_t mqtt_payload_length;

static char register_topic_buffer[REGISTER_TOPIC_BUFFER_LENGTH];

static uint8_t			send_buff[BUFF_LEN];
static uint8_t			read_buff[BUFF_LEN];

int32_t read_and_send(struct mqtt_desc *mqtt, struct ade9430_dev *ade9430_dev, struct nhd_c12832a1z_device *nhd_c12832a1z_dev)
{
	struct mqtt_message	msg;
	uint8_t			buff[100];
	uint32_t		len;
	int			ret;

	ret = ade9430_read_temp(ade9430_dev);
	if (ret)
		return ret;

	ret = ade9430_read_watt(ade9430_dev);
	if (ret)
		return ret;

	/* Serialize data */
	len = sprintf(buff, "Temp: %.2f\nAIRMS: %.2f\nAVRMS: %.2f\nAWATT: %.2f", ade9430_dev->temp_deg, ade9430_dev->airms, ade9430_dev->avrms, ade9430_dev->awatt);

	ret = nhd_c12832a1z_print_string(nhd_c12832a1z_dev, buff, len);
	if (ret)
		return ret;

	//Get the topic to send a telemetry message
	az_iot_hub_client_telemetry_get_publish_topic(&my_client, NULL, telemetry_topic, sizeof(telemetry_topic), NULL);

	//Send the telemetry message with the MQTT client
	/* Send data to mqtt broker */
	msg = (struct mqtt_message) {
		.qos = AZ_HUB_CLIENT_DEFAULT_MQTT_TELEMETRY_QOS,
		.payload = buff,
		.len = len,
		.retained = false
	};
	return mqtt_publish(mqtt, telemetry_topic, &msg);
}

/***************************************************************************//**
 * @brief Main function execution for aducm3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/

void mqtt_message_handler(struct mqtt_message_data *msg)
{
	char	buff[101];
	int32_t	len;

	/* Message.payload don't have at the end '\0' so we have to add it. */
	len = msg->message.len > 100 ? 100 : msg->message.len;
	memcpy(buff, msg->message.payload, len);
	buff[len] = 0;

	printf("Topic:%s -- Payload: %s\n", msg->topic, buff);
}

/* create_and_configure_mqtt_client_for_provisioning */
void create_and_configure_mqtt_client_for_provisioning(void)
{
	iot_sample_create_mqtt_endpoint(SAMPLE_TYPE, &env_vars, mqtt_endpoint_buffer, sizeof(mqtt_endpoint_buffer));
	az_iot_provisioning_client_init(&provisioning_client, az_span_create_from_str(mqtt_endpoint_buffer),
							az_span_create_from_str(AZ_IOT_PROVISIONING_ID_SCOPE), 
							az_span_create_from_str(AZ_IOT_PROVISIONING_REGISTRATION_ID),
							NULL);

	// Get the MQTT client id used for the MQTT connection.
	az_iot_provisioning_client_get_client_id(
	&provisioning_client, mqtt_client_id_buffer, sizeof(mqtt_client_id_buffer), NULL);

	az_iot_provisioning_client_get_user_name(&provisioning_client, mqtt_client_username_buffer,
							sizeof(mqtt_client_username_buffer), NULL);
	
	az_iot_provisioning_client_register_get_publish_topic(&provisioning_client, register_topic_buffer, sizeof(register_topic_buffer), NULL);

	az_iot_provisioning_client_get_request_payload(
				&provisioning_client,
				custom_registration_payload_property,
				NULL,
				mqtt_payload,
				sizeof(mqtt_payload),
				&mqtt_payload_length);
}

/* create_and_configure_mqtt_client_for_iot_hub */
void create_and_configure_mqtt_client_for_iot_hub(void)
{
	az_iot_hub_client_options options = az_iot_hub_client_options_default();
 
	// Initialize the hub client with hostname, device id, and default connection options.
	az_iot_hub_client_init(&my_client, my_iothub_hostname, my_device_id, &options);
	
	// Get the MQTT client id used for the MQTT connection.
	az_iot_hub_client_get_client_id(
	&my_client, my_mqtt_client_id, sizeof(my_mqtt_client_id),  &my_mqtt_client_id_length);
	
	// Get the MQTT user name to connect.
	az_iot_hub_client_get_user_name(
	&my_client, my_mqtt_user_name, sizeof(my_mqtt_user_name), &my_mqtt_user_name_length);

	az_iot_hub_client_properties_get_reported_publish_topic(&my_client, my_request_id, reported_topic, sizeof(reported_topic), NULL);
}

int main()
{
	int ret = -EINVAL;
	int i = 0;
	int status;

	struct max_spi_init_param spi_extra_ip  = {
		.numSlaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	};

	struct no_os_spi_init_param spi_ip = {
		.device_id = 1,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &spi_extra_ip,
	};

	struct ade9430_init_param ade9430_ip = {
		.spi_init = &spi_ip,
	};

	struct ade9430_dev *ade9430_device;

	ret = ade9430_init(&ade9430_device, ade9430_ip);
	if (ret)
		return ret;

	ret = ade9430_read_temp(ade9430_device);
	if (ret)
		return ret;

	struct no_os_spi_init_param spi_lcd_ip = {
		.device_id = 1,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = &max_spi_ops,
		.chip_select = 2,
		.extra = &spi_extra_ip,
	};

	struct max_gpio_init_param gpio_dc_extra_ip = {
		.direction = NO_OS_GPIO_OUT,
	};

	struct no_os_gpio_init_param gpio_dc_ip = {
		.port = 2,
		.number = 15,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_dc_extra_ip,
	};

	struct max_gpio_init_param gpio_rst_extra_ip = {
		.direction = NO_OS_GPIO_OUT,
	};

	struct no_os_gpio_init_param gpio_rst_ip = {
		.port = 1,
		.number = 25,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_rst_extra_ip,
	};

	struct nhd_c12832a1z_init_param nhd_c12832a1z_ip = {
		.spi_ip = &spi_lcd_ip,
		.dc_pin_ip = &gpio_dc_ip,
		.reset_pin_ip = &gpio_rst_ip
	};

	struct nhd_c12832a1z_dev *nhd_c12832a1z_device;

	ret = nhd_c12832a1z_init(&nhd_c12832a1z_device, nhd_c12832a1z_ip);
	if (ret)
		return ret;

	struct max_gpio_init_param gpio_extra_ip = {
		.direction = NO_OS_GPIO_OUT,
	};

	struct no_os_gpio_init_param gpio_reset_ip = {
		.port = 2,
		.number = 13,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip
	};

	struct no_os_gpio_desc *rst_gpio;

	status = no_os_gpio_get(&rst_gpio, &gpio_reset_ip);
	if (status < 0)
		return status;
	
	status = no_os_gpio_set_value(rst_gpio, NO_OS_GPIO_LOW);
	if (status < 0)
		return status;
	
	no_os_mdelay(500);

	status = no_os_gpio_set_value(rst_gpio, NO_OS_GPIO_HIGH);
	if (status < 0)
		return status;
	
	no_os_mdelay(5000);

	const struct no_os_irq_platform_ops *platform_irq_ops = &max_irq_ops;

	struct no_os_irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.platform_ops = platform_irq_ops,
		.extra = NULL
	};

	struct max_uart_init_param ip = {
		.flow = UART_FLOW_DIS
	};

	struct no_os_irq_ctrl_desc *irq_desc;
	status = no_os_irq_ctrl_init(&irq_desc, &irq_init_param);
	if (status < 0)
		return status;

	status = no_os_irq_global_enable(irq_desc);
	if (status < 0)
		return status;

	struct no_os_uart_init_param luart_par = {
		.device_id = UART_DEVICE_ID,
		/* TODO: remove this ifdef when asynchrounous rx is implemented on every platform. */
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &ip
	};
	struct no_os_uart_desc *uart_desc;

	status = no_os_uart_init(&uart_desc, &luart_par);
	if (status < 0)
		return status;

	struct no_os_uart_init_param luart0_par = {
		.device_id = 0,
		/* TODO: remove this ifdef when asynchrounous rx is implemented on every platform. */
		.irq_id = UART0_IRQn,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &ip
	};
	struct no_os_uart_desc *uart0_desc;

	ret = no_os_uart_init(&uart0_desc, &luart0_par);
	if (ret)
		return ret;

	maxim_uart_stdio(uart0_desc);

	static struct tcp_socket_init_param socket_param;

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

	wifi_get_network_interface(wifi, &socket_param.net);

	struct secure_init_param sip = {
		.ca_cert = my_ca_cert,
		.ca_cert_len = NO_OS_ARRAY_SIZE(my_ca_cert),
		.cli_cert = my_cli_cert,
		.cli_cert_len = NO_OS_ARRAY_SIZE(my_cli_cert),
		.cli_pk = my_cli_pk,
		.cli_pk_len = NO_OS_ARRAY_SIZE(my_cli_pk)
	};

	socket_param.max_buff_size = 0;
	socket_param.secure_init_param = &sip;

	static struct tcp_socket_desc	*sock;
	status = socket_init(&sock, &socket_param);
		if (NO_OS_IS_ERR_VALUE(status))
			PRINT_ERR_AND_RET("Error socket_init", status);

	struct socket_address		mqtt_broker_addr;
	/* Connect socket to mqtt borker server */
	mqtt_broker_addr = (struct socket_address) {
		.addr = DPS_SERVER_ADDR,
		.port = SERVER_PORT
	};
	status = socket_connect(sock, &mqtt_broker_addr);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error socket_connect", status);

	printf("Connection with \"%s\" established\n", SERVER_ADDR);

	struct mqtt_init_param		mqtt_init_param;
	/* Initialize mqtt descriptor */
	mqtt_init_param = (struct mqtt_init_param) {
		.timer_id = TIMER_ID,
		.extra_timer_init_param = &max_timer_ops,
		.sock = sock,
		.command_timeout_ms = MQTT_CONFIG_CMD_TIMEOUT,
		.send_buff = send_buff,
		.read_buff = read_buff,
		.send_buff_size = BUFF_LEN,
		.read_buff_size = BUFF_LEN,
		.message_handler = mqtt_message_handler
	};
	struct mqtt_desc	*mqtt;

	status = mqtt_init(&mqtt, &mqtt_init_param);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_init", status);

	struct mqtt_connect_config	conn_config;

	create_and_configure_mqtt_client_for_provisioning();

	conn_config = (struct mqtt_connect_config) {
		.version = MQTT_CONFIG_VERSION,
		.keep_alive_ms = MQTT_CONFIG_KEEP_ALIVE,
		.client_name = mqtt_client_id_buffer,
		.username = mqtt_client_username_buffer,
		.password = MQTT_CONFIG_CLI_PASS
	};

	status = mqtt_connect(mqtt, &conn_config, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_connect", status);

	status = mqtt_subscribe(mqtt, AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, 1, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_subscribe", status);
	printf("Subscribed to topic: %s\n", AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC);

	struct mqtt_message	msg;
	uint32_t		len;

	msg = (struct mqtt_message) {
		.qos = 1,
		.payload = mqtt_payload,
		.len = (int)mqtt_payload_length,
		.retained = false
	};

	ret = mqtt_publish(mqtt, register_topic_buffer, &msg);
	if (ret)
		return ret;

	status = mqtt_disconnect(mqtt);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_connect", status);
	
	status = socket_disconnect(sock);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error socket_connect", status);

	status = socket_init(&sock, &socket_param);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error socket_init", status);

	/* Connect socket to mqtt borker server */
	mqtt_broker_addr = (struct socket_address) {
		.addr = SERVER_ADDR,
		.port = SERVER_PORT
	};

	status = socket_connect(sock, &mqtt_broker_addr);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error socket_connect", status);

	mqtt_init_param = (struct mqtt_init_param) {
		.timer_id = TIMER_ID,
		.extra_timer_init_param = &max_timer_ops,
		.sock = sock,
		.command_timeout_ms = MQTT_CONFIG_CMD_TIMEOUT,
		.send_buff = send_buff,
		.read_buff = read_buff,
		.send_buff_size = BUFF_LEN,
		.read_buff_size = BUFF_LEN,
		.message_handler = mqtt_message_handler
	};

	status = mqtt_init(&mqtt, &mqtt_init_param);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_init", status);

	create_and_configure_mqtt_client_for_iot_hub();

	conn_config = (struct mqtt_connect_config) {
		.version = MQTT_CONFIG_VERSION,
		.keep_alive_ms = MQTT_CONFIG_KEEP_ALIVE,
		.client_name = my_mqtt_client_id,
		.username = my_mqtt_user_name,
		.password = MQTT_CONFIG_CLI_PASS
	};

	status = mqtt_connect(mqtt, &conn_config, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_connect", status);

	printf("Connected to mqtt broker\n");

	//Subscribe to twin response topic
	status = mqtt_subscribe(mqtt, AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_subscribe", status);
	printf("Subscribed to topic: %s\n", AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC);

	uint8_t			twin_buff[100];
	len = sprintf(twin_buff, "{\"key\":\"value\"}");

	msg = (struct mqtt_message) {
		.qos = AZ_HUB_CLIENT_DEFAULT_MQTT_TELEMETRY_QOS,
		.payload = twin_buff,
		.len = len,
		.retained = false
	};

	ret = mqtt_publish(mqtt, reported_topic, &msg);
	if (ret)
		return ret;

	status = mqtt_yield(mqtt, SCAN_SENSOR_TIME);
		if (NO_OS_IS_ERR_VALUE(status))
			PRINT_ERR_AND_RET("Error mqtt_yield", status);

	while (true) {
		status = read_and_send(mqtt, ade9430_device, nhd_c12832a1z_device);
		if (NO_OS_IS_ERR_VALUE(status))
			PRINT_ERR_AND_RET("Error read_and_send", status);
		printf("Data sent to broker\n");

		/* Dispatch new mqtt mesages if any during SCAN_SENSOR_TIME */
		status = mqtt_yield(mqtt, SCAN_SENSOR_TIME);
		if (NO_OS_IS_ERR_VALUE(status))
			PRINT_ERR_AND_RET("Error mqtt_yield", status);
		i++;
	}


	return 0;
}

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
static az_span my_iothub_hostname = AZ_SPAN_LITERAL_FROM_STR("iot-hub-ylr4pd6jbvxtc.azure-devices.net");
static az_span my_device_id = AZ_SPAN_LITERAL_FROM_STR("EnergyMonitoringDevice1");

// Make sure to size the buffer to fit the client id (16 is an example)
static char my_mqtt_client_id[64];
static size_t my_mqtt_client_id_length;

#define CA_CRT_PEM							\
"-----BEGIN CERTIFICATE-----\r\n"					\
"MIIDHzCCAgegAwIBAgIULv9GOV/gHojd0KXRx9GZ0PUqyBwwDQYJKoZIhvcNAQEL\r\n"	\
"BQAwHzEdMBsGA1UEAwwUQW5hbG9nIERldmljZXMsIEluYy4wHhcNMjMwMTEyMDkz\r\n"	\
"NjExWhcNMzIxMDExMDkzNjExWjAfMR0wGwYDVQQDDBRBbmFsb2cgRGV2aWNlcywg\r\n"	\
"SW5jLjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANrAoQnTHwOW2TXr\r\n"	\
"v4hWXSeU/7upsbIdIPB5TgWFN5lX+SY0tx0ljcGKymrnf6a/tAzdljGqRXH2H4Ln\r\n"	\
"sW2sj7qbntppe1023SHA5S8d4WGqbmoh7Lf+FrYY9oAScT/nGNMXZ5hWgpJkloLN\r\n"	\
"AFYCS7eJ08aR0geUt2Qc7YM77Rogmp0x95Z2yIV2QvKB41U6p151x63D5/tWvICJ\r\n"	\
"q9PPKxeGuS2wa0h2zP82BKLyHDZdxSUkV2O/MbjxY/48amvZHrYRk+t8HfClIOt8\r\n"	\
"ECobTnuLO8uEcz/H8mwaQGWDd5B3IwoUulI8gsCcQI1/iXcxZOEFgv8zwW/PeV4D\r\n"	\
"h9vJkQMCAwEAAaNTMFEwHQYDVR0OBBYEFBsV5sXaaKZFFpNHB3rtmiBgMEsZMB8G\r\n"	\
"A1UdIwQYMBaAFBsV5sXaaKZFFpNHB3rtmiBgMEsZMA8GA1UdEwEB/wQFMAMBAf8w\r\n"	\
"DQYJKoZIhvcNAQELBQADggEBANexrLjMSe9r3inDDTme9LgWQpLeJ374ZgIMesgn\r\n"	\
"hjrMYWloJa2Uqvj/7qc0aE9nkaGmnQwlMQ8ozBFizkyNoUFl5JdT+JzpQZv+6B3P\r\n"	\
"DfQ+AKjfvcVNwsSgMIs6zZs4P47i745p5uvjdtVGW3rExQmI8/2398cBjeByyNT9\r\n"	\
"N+TCK8ZwHS4fHYDJTSLUuWvX54Xl7S3Nmgq5frca4Bi7+i2wqjQWW6/QTbK7ZSdG\r\n"	\
"m7qpW8nDxClkgO+5IQmqaENkohMwAlF99FBK0iJlSSSIsuo0SFXtqINFa7emFwGa\r\n"	\
"oAMxi2Zfx52/MhR5WfVvQx4YIX2fIQP+OPLBTMMYNdesZ+U=\r\n"			\
"-----END CERTIFICATE-----\r\n"

static const char my_ca_cert[] = CA_CRT_PEM;

static const char my_cli_cert[]=
"-----BEGIN CERTIFICATE-----\n"
"MIICyDCCAbACFB2GazFUJ6ofN6kFRe4qTnUFCdYtMA0GCSqGSIb3DQEBCwUAMB8x\n"
"HTAbBgNVBAMMFEFuYWxvZyBEZXZpY2VzLCBJbmMuMB4XDTIyMTIyMDExNDEwN1oX\n"
"DTIzMDExOTExNDEwN1owIjEgMB4GA1UEAwwXRW5lcmd5TW9uaXRvcmluZ0Rldmlj\n"
"ZTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDy9QcbYm3/8ZG4zXfK\n"
"VHd0H/0GN5xpLV+61jBnWvvnn2raX1xuw9vH6ExHs5NR7i/c0TSO69SnQ/3DjD7l\n"
"+4lOUmkdJ9odYYQo3SEFhGAC0+0W/XSef97XQIudsYuALxFEHvr8XW/qG9RXX3vM\n"
"xzTdfHQHlP+4F8W6mMVsey0KjwnK1yAhaTf4Wl73mw/uXE6F4Cve+JygIs4R+Xg+\n"
"R3oej3ojEDtHOqueiEndWk5OFxXSKHbVDlmwu6MZLCf70vfLeuEwFE6APxTMUfi1\n"
"U4zKWfij3zns2X/mBJ7aDS41UllE330jnT+FWPbfJrCd4x810vfWxW6aumq1IP+b\n"
"3L+VAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAHz54zcNB1NFpS7SLQj4uGuQ8MWM\n"
"ztESbadGtANthFiN68ESvolDaFWkrPZUnvqEIBMEWmaviOQqdq/UYJH+r/E/C5xX\n"
"Mg1M0jVLi0rrUNHewJXnbvjq6MBkz9T4McX7a+ruCVdkjwsS2o5iDVPiArj+dPyJ\n"
"JOGIpr12GSAgHUJq716TmqPa0muS5z/Ahh3CmGJGK/qyeBjYCbSC46U5UvpGD+YJ\n"
"+WeXLosSLTGdB/pf6+AMiDsX3FwLBRyCn149jJcgYH1hMGOL1nC2yA7ZkMijpQjt\n"
"n0Xs1T8QbrjRM0HLjsHD7xRyhW61I51UdgV0RlVYJ08438DgLgUJ1/WYed8=\n"
"-----END CERTIFICATE-----\n";

static const char my_cli_pk[]= \
"-----BEGIN PRIVATE KEY-----\n"
"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDy9QcbYm3/8ZG4\n"
"zXfKVHd0H/0GN5xpLV+61jBnWvvnn2raX1xuw9vH6ExHs5NR7i/c0TSO69SnQ/3D\n"
"jD7l+4lOUmkdJ9odYYQo3SEFhGAC0+0W/XSef97XQIudsYuALxFEHvr8XW/qG9RX\n"
"X3vMxzTdfHQHlP+4F8W6mMVsey0KjwnK1yAhaTf4Wl73mw/uXE6F4Cve+JygIs4R\n"
"+Xg+R3oej3ojEDtHOqueiEndWk5OFxXSKHbVDlmwu6MZLCf70vfLeuEwFE6APxTM\n"
"Ufi1U4zKWfij3zns2X/mBJ7aDS41UllE330jnT+FWPbfJrCd4x810vfWxW6aumq1\n"
"IP+b3L+VAgMBAAECggEBAJRxVVhL+X2JsUlCcNB0b0/KQ6RK0gwJJxZWy9g6ZPMO\n"
"GhE2dplOnbg0nHaqp3ZFm4TFZeF1Acb46dFSnROTbj8mZwbjVIbjEAaPpR9j0+2m\n"
"YpPLZDd7VUIbM7KzydIV7+y5KQyMYladjk2cD1380LYK1jHTvbJTBAKQWP+A+vIZ\n"
"ZlbuuzPOss4hrRTPDuCBsOlFKx89qm7SP+hJh+qhkT2qtG8t7IkuMaghlfft7vXX\n"
"te1ebmReP2djiaKHstcDQiA7125MfrGP7c7WstVY3/oy5FIJakV3S++ODHNItUT/\n"
"f3s7deIBzkPX+MuHzb1Fs/yJgdhKWW7aUSpfSiPC2wECgYEA+cJCyZusILG8Df9G\n"
"v5afpg/gpR1nzRxQqs4/94KqSa+Llw3ogBxV+t5aKbCxkzBPiEOK2eCnSlo0U/iT\n"
"GoIps7kYTFoH120HfhgB8IRzthH03+Zw1oG25YUNltYlhWkRzz2VoXrH1qpx4FmV\n"
"q8XSGJjxccow2O5zygQm3r6k/WECgYEA+QdBbdDsoKL/k3UF695c95C4uYDfk/b4\n"
"7XSI2UDXM+QTnSdSl/I+f3thZjJjW6p90HMVedm2gR2B17nS2htl35XbWkxdA6Mz\n"
"6QGxmHyzW8vMo5VcGgFQ9Ux0lShgqv/AY5mjOqQ+yDGrvcyD7h2KHpLFH0jkuEqT\n"
"RTLv2D4R2rUCgYEAiRhvHMeM1m15cp17slL102vHvhrEDbzLMtcAGdbproTjtMj3\n"
"pMrpD0naaCQLdWYBYiThJPPCIHgfXTCHXvev+G1TvyV0RHYetMJs3h92fKyio/yU\n"
"V8rgIGDGstNdyTuBgSam6frBkz4AQEwschumVcYuUm3QHmPVv0BELeSsIEECgYBj\n"
"2+z4xFae873Yu4xsUPamr3njqTH+e4LGdXyK4d1qCYNz+Vi9ZX56UFd2S65cC7n3\n"
"5K4v9oWgsNxoTTOoEmR/i/Ax/X0vADrIrM0ygI7LuKWO003jf7OfKdF2HoLlDzlu\n"
"tyUlVsd2L7TWnzQoP+Aq8l/pc6mD3SPBE/Ppmro+IQKBgBjdhDgRLtqSRz9RJZoq\n"
"mCnGyQf02YopKvhD828lVHjqqDoYVbYZVlYTV3h1edx3d2i5jAicjn25M/BNTYzO\n"
"RM5rhN+zLoBF6V728kraaX3A/HJbOFnqH7J/ed0FWvtcCCD+4CMr71kytx/tS+Is\n"
"HjKbpJjgh0Tf/HOK00ksJ2j5\n"
"-----END PRIVATE KEY-----\n";

// Make sure to size the buffer to fit the username (128 is an example)
static char my_mqtt_user_name[128];
static size_t my_mqtt_user_name_length;

static char telemetry_topic[128];

int32_t read_and_send(struct mqtt_desc *mqtt, struct ade9430_dev *dev)
{
	struct mqtt_message	msg;
	uint8_t			buff[100];
	uint32_t		len;
	int			ret, temp;

	ret = ade9430_read_temp(dev, &temp);
	if (ret)
		return ret;

	ret = ade9430_read_watt(dev);
	if (ret)
		return ret;

	/* Serialize data */
	len = sprintf(buff, "ADE9430 Temp: %d, AWATT_ACC: %u, AWATTHR: %llu", temp, dev->awatt_acc, dev->awatthr);

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

int main()
{
	int ret = -EINVAL;
	int i = 0;
	int status;

	az_iot_hub_client_options options = az_iot_hub_client_options_default();
 
	// Initialize the hub client with hostname, device id, and default connection options.
	az_iot_hub_client_init(&my_client, my_iothub_hostname, my_device_id, &options);
	
	// Get the MQTT client id used for the MQTT connection.
	az_iot_hub_client_get_client_id(
	&my_client, my_mqtt_client_id, sizeof(my_mqtt_client_id),  &my_mqtt_client_id_length);
	
	// Get the MQTT user name to connect.
	az_iot_hub_client_get_user_name(
	&my_client, my_mqtt_user_name, sizeof(my_mqtt_user_name), &my_mqtt_user_name_length);
	
	// At this point you are free to use my_mqtt_client_id and my_mqtt_user_name to connect using
	// your MQTT client.

	struct max_spi_init_param spi_extra_ip  = {
		.numSlaves = 1,
		.polarity = SPI_SS_POL_LOW
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

	ret = nhd_c12832a1z_print_ascii(nhd_c12832a1z_device, 0x00, 0, 0);
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
	
	no_os_mdelay(2000);

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

	char buff[100];
	wifi_get_ip(wifi, buff, 100);
	printf("Tinyiiod ip is: %s\n", buff);
	wifi_get_network_interface(wifi, &socket_param.net);

	struct secure_init_param sip = {
		.ca_cert = my_ca_cert,
		.ca_cert_len = NO_OS_ARRAY_SIZE(my_ca_cert),
		// .cli_cert = my_cli_cert,
		// .cli_cert_len = NO_OS_ARRAY_SIZE(my_cli_cert),
		// .cli_pk = my_cli_pk,
		// .cli_pk_len = NO_OS_ARRAY_SIZE(my_cli_pk)
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
	/* Mqtt configuration */
	/* Connect to mqtt broker */
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

	//Subscribe to c2d messages
	status = mqtt_subscribe(mqtt, AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_subscribe", status);
	printf("Subscribed to topic: %s\n", AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC);

	//Subscribe to device methods
	status = mqtt_subscribe(mqtt, AZ_IOT_HUB_CLIENT_METHODS_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_subscribe", status);
	printf("Subscribed to topic: %s\n", AZ_IOT_HUB_CLIENT_METHODS_SUBSCRIBE_TOPIC);

	//Subscribe to twin patch topic
	status = mqtt_subscribe(mqtt, AZ_IOT_HUB_CLIENT_TWIN_PATCH_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_subscribe", status);
	printf("Subscribed to topic: %s\n", AZ_IOT_HUB_CLIENT_TWIN_PATCH_SUBSCRIBE_TOPIC);

	//Subscribe to twin response topic
	status = mqtt_subscribe(mqtt, AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (NO_OS_IS_ERR_VALUE(status))
		PRINT_ERR_AND_RET("Error mqtt_subscribe", status);
	printf("Subscribed to topic: %s\n", AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC);

	while (true) {
		status = read_and_send(mqtt, ade9430_device);
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

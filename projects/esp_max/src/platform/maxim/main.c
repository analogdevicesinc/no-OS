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
static az_span my_iothub_hostname = AZ_SPAN_LITERAL_FROM_STR("iot-hub-ylr4pd6jbvxtc.azure-devices.net");
static az_span my_device_id = AZ_SPAN_LITERAL_FROM_STR("EnergyMonitoringDevice1");
static az_span my_request_id = AZ_SPAN_LITERAL_FROM_STR("1");

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
"MIICyDCCAbACFBhDPi10huvZNJYgKltRGCBE2l9lMA0GCSqGSIb3DQEBCwUAMB8x\r\n" \
"HTAbBgNVBAMMFEFuYWxvZyBEZXZpY2VzLCBJbmMuMB4XDTIzMDExMjEwMzUxN1oX\r\n" \
"DTIzMDIxMTEwMzUxN1owIjEgMB4GA1UEAwwXRW5lcmd5TW9uaXRvcmluZ0Rldmlj\r\n" \
"ZTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQD5e81cT1BLOsZNHz7s\r\n" \
"wn4/WXm5gFO/kK8JjlvC2G2QLlhzUJDQ+FraTcbWGz0sRlbX5kareyxLpYNcKKxF\r\n" \
"LCcglI8fRlPLM2v88vsE8xiVav6uIyPh7koolmn9RQ2bEZlwcXJRoo53W9XbEWMK\r\n" \
"gADsL4gzj+IK3Wd7JAdmqsJEiSg6u2ePjuBF/1XB+4LvkvbqrP+NmmDzqouu06hj\r\n" \
"PLsf3uWzOtUO1DsWjMkUW00V6ZVV0Ljr73dxdezuQGyH5GIKy2bjO4C+3NNFl517\r\n" \
"+3VmEiIIR1n8sfRCtRaeO/o4VJW5rpaMHbhI9xTYnk6vTc4BMuHP3eFxtj/aU09K\r\n" \
"xhyXAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAF7UDpmL7PswJcYf7mGTjesOaTDr\r\n" \
"cGgFtotuXlidLz2S2exX5nUKuoNmWzEl+vUDmI/d8VqWy61I6gDk35pqXgrcX9Rm\r\n" \
"naqYluhHcjQwQfsHwkDsGDz9uwLAe0JK6wlbCZDcgSXdvU6CxUDtrv9WBXl7kcgU\r\n" \
"Lph5a/t9L/LFp24Sh5wsK6iUTtCW4MFGwqWhFdeglTDJrM7YdPLsf72feBkH9vfR\r\n" \
"fxm/2d6dxkqcJg952k49qq5iCB41fAemS6TwizwNJfcGPT31dE4WS0wCI1wx6jHy\r\n" \
"Zfm7ChOD+OGOwx627UdIXtfBOzv45SMQIAGvj55GRBvQZ9Hoo3j0GbXNA3M=\r\n"     \
"-----END CERTIFICATE-----\r\n";

static const char my_cli_pk[]= \
"-----BEGIN PRIVATE KEY-----\n"
"MIIEwAIBADANBgkqhkiG9w0BAQEFAASCBKowggSmAgEAAoIBAQD5e81cT1BLOsZN\r\n"
"Hz7swn4/WXm5gFO/kK8JjlvC2G2QLlhzUJDQ+FraTcbWGz0sRlbX5kareyxLpYNc\r\n"
"KKxFLCcglI8fRlPLM2v88vsE8xiVav6uIyPh7koolmn9RQ2bEZlwcXJRoo53W9Xb\r\n"
"EWMKgADsL4gzj+IK3Wd7JAdmqsJEiSg6u2ePjuBF/1XB+4LvkvbqrP+NmmDzqouu\r\n"
"06hjPLsf3uWzOtUO1DsWjMkUW00V6ZVV0Ljr73dxdezuQGyH5GIKy2bjO4C+3NNF\r\n"
"l517+3VmEiIIR1n8sfRCtRaeO/o4VJW5rpaMHbhI9xTYnk6vTc4BMuHP3eFxtj/a\r\n"
"U09KxhyXAgMBAAECggEBAOmntF7m/Xlns39ZazKMuXaVUzaT5Fu+ouCdTn1++6YU\r\n"
"ccyWQPsmDtj0DOxLmJBV1FHiHn/OHV8/xXZTtXwXbRHLcPvGfwYJw/KkkspA7t5J\r\n"
"1fq7eIjKBvRaYyvMN4DrC7zO01V3nFxXS0Qf1ZVo6vRJVBOOhxLL53Xh+XiDJuzN\r\n"
"ethkGaDEHRbnMk3s3I/5OB5SmshIubteQbLfGj6m27BGdUmWEdbcoNz0xWJX3pir\r\n"
"YyraXKM6RctfBTL/KNXf3iz52OHbyhwKtpmVs4yvoL3jvxMzd5qQsMluE6rOopfd\r\n"
"eBsRFnId5kMCrd/ngzPaki2keXagkQJXmxvIbf46HYkCgYEA/cd4CI+j5AQWAS/6\r\n"
"EyDmTW8PL+D5jgteBsVoNzvN6TDqIPBhMEx66XwRhbkWgjd4VOfb6QCm6UPWXwHu\r\n"
"GsBzz2xLhtwD2S1+BfYklS63bT+jHRHQuUEkID3u/aWJeiQ207NOvxEk3/0ZsByc\r\n"
"UxEjI7a0NLJJPkwMaxeK1eJfT00CgYEA+6q1ye1fFJXxlWFIE1/j7/eesNo/E6V0\r\n"
"Eykr24kiNZCrlMJV7o/YyLG6GEvvjt6eHRKUfVBTZj6FHP6IsvrGeGXeQcSj4gsU\r\n"
"U0vaVIv2+oJqJNPzrYY8w05DlqFK5VGUx8YyhhT/oAQOZ7bPLpeIJ1oNqvJBD1vu\r\n"
"b1EeNdaG8XMCgYEAmhBANKJPEpvQaAQiQv6UEao8hAqGNc9804uASxfJV6nUWQ7V\r\n"
"Uab8b0ZctNu6aG8YZCMmA/vbWH1QZPkc/aycIe5So5pp06ykdyYI63Ai9vXNJlas\r\n"
"aeq77b7rmZynKzlU94xBYAkeY8SZlaqo1FzxAjtmhx89C87LMzBlnpcA2XkCgYEA\r\n"
"yjbhuO2NL2pH4kJ3enHbIa7yiKHkevG/scGsJssqfKXQ6Ee6amSUyLXdlLY3N7Hy\r\n"
"k+OhAovvZ0EnDiQLTNypaoZXrrrE/9TtMPzK4pJQEfuGNC9GJhCtdDTHpBnXpykF\r\n"
"BfxgDlKDD10TAgzmrkY9tXjrcIcU9bVlgXZJcvL8HLcCgYEA7eIu1N8T0bH0Bd/w\r\n"
"QRzNo1wzYLR21AZ0aNqgATQmTHdwXDtEoL14wJisr/L0w+rV+AEA+XczvR81NzIe\r\n"
"Rb7SPxgKJ9ZviuxC1FXTsxlAMlEyhJDQOc1EYreidKZvuk8Mq3NWXHyMQdRiz9Uw\r\n"
"gtV47HSZkKlslkvgtRxCe6nuZ6U=\r\n"
"-----END PRIVATE KEY-----\r\n";

#define AZ_IOT_PROVISIONING_ID_SCOPE "0ne0090DBBD"
#define AZ_IOT_PROVISIONING_REGISTRATION_ID "EnergyMonitoringDevice1"

#define SAMPLE_TYPE PAHO_IOT_PROVISIONING
#define SAMPLE_NAME PAHO_IOT_PNP_WITH_PROVISIONING_SAMPLE

#define QUERY_TOPIC_BUFFER_LENGTH 256
#define REGISTER_TOPIC_BUFFER_LENGTH 128
#define PROVISIONING_ENDPOINT_BUFFER_LENGTH 256
#define MQTT_PAYLOAD_BUFFER_LENGTH 256

#define CLIENT_ID_BUFFER_LENGTH 128

// Make sure to size the buffer to fit the username (128 is an example)
static char my_mqtt_user_name[256];
static size_t my_mqtt_user_name_length;

static char telemetry_topic[128];
static char reported_topic[128];

static az_span const model_id = AZ_SPAN_LITERAL_FROM_STR("dtmi:com:example:Thermostat;1");
static az_span custom_registration_payload_property
    = AZ_SPAN_LITERAL_FROM_STR("{\"modelId\":\"dtmi:com:example:Thermostat;1\"}");

static iot_sample_environment_variables env_vars;
static az_iot_provisioning_client provisioning_client;
static char iot_hub_endpoint_buffer[128];
static char iot_hub_device_id_buffer[128];
static az_span device_iot_hub_endpoint;
static az_span device_id;
static char mqtt_client_username_buffer[256];
char mqtt_client_id_buffer[CLIENT_ID_BUFFER_LENGTH];

uint8_t mqtt_payload[MQTT_PAYLOAD_BUFFER_LENGTH];
size_t mqtt_payload_length;

int32_t read_and_send(struct mqtt_desc *mqtt, struct ade9430_dev *ade9430_dev, struct nhd_c12832a1z_device *nhd_c12832a1z_dev)
{
	struct mqtt_message	msg;
	uint8_t			buff[100];
	uint32_t		len;
	int			ret, temp;

	ret = ade9430_read_temp(ade9430_dev, &temp);
	if (ret)
		return ret;

	ret = ade9430_read_watt(ade9430_dev);
	if (ret)
		return ret;

	/* Serialize data */
	len = sprintf(buff, "ADE9430 Temp: %d; AWATT_ACC: %u; AWATTHR: %llu", temp, ade9430_dev->awatt_acc, ade9430_dev->awatthr);

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

int main()
{
	int ret = -EINVAL;
	int i = 0;
	int status;
	putenv("AZ_IOT_PROVISIONING_ID_SCOPE=0ne0090DBBD");
	putenv("AZ_IOT_PROVISIONING_REGISTRATION_ID=EnergyMonitoringDevice1");
	/* create_and_configure_mqtt_client_for_provisioning */
	iot_sample_read_environment_variables(SAMPLE_TYPE, SAMPLE_NAME, &env_vars);
	char mqtt_endpoint_buffer[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
	iot_sample_create_mqtt_endpoint(
	SAMPLE_TYPE, &env_vars, mqtt_endpoint_buffer, sizeof(mqtt_endpoint_buffer));
	int rc = az_iot_provisioning_client_init(&provisioning_client, az_span_create_from_str(mqtt_endpoint_buffer),
							env_vars.provisioning_id_scope, env_vars.provisioning_registration_id,
							NULL);
	if (az_result_failed(rc))
	{
		IOT_SAMPLE_LOG_ERROR(
			"Failed to initialize provisioning client: az_result return code 0x%08x.", rc);
		exit(rc);
	}

	// Get the MQTT client id used for the MQTT connection.
	rc = az_iot_provisioning_client_get_client_id(
	&provisioning_client, mqtt_client_id_buffer, sizeof(mqtt_client_id_buffer), NULL);
	if (az_result_failed(rc))
	{
		IOT_SAMPLE_LOG_ERROR("Failed to get MQTT client id: az_result return code 0x%08x.", rc);
		exit(rc);
	}

	rc = az_iot_provisioning_client_get_user_name(&provisioning_client, mqtt_client_username_buffer, sizeof(mqtt_client_username_buffer), NULL);
	if (az_result_failed(rc))
	{
		IOT_SAMPLE_LOG_ERROR("Failed to get MQTT client username: az_result return code 0x%08x.", rc);
		exit(rc);
	}

	/* create_and_configure_mqtt_client_for_iot_hub */

	az_iot_hub_client_options options = az_iot_hub_client_options_default();
 
	// Initialize the hub client with hostname, device id, and default connection options.
	az_iot_hub_client_init(&my_client, my_iothub_hostname, my_device_id, &options);
	
	// Get the MQTT client id used for the MQTT connection.
	az_iot_hub_client_get_client_id(
	&my_client, my_mqtt_client_id, sizeof(my_mqtt_client_id),  &my_mqtt_client_id_length);
	
	// Get the MQTT user name to connect.
	az_iot_hub_client_get_user_name(
	&my_client, my_mqtt_user_name, sizeof(my_mqtt_user_name), &my_mqtt_user_name_length);

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

	int			temp;

	ret = ade9430_read_temp(ade9430_device, &temp);
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

	char buff[100];
	wifi_get_ip(wifi, buff, 100);
	printf("Tinyiiod ip is: %s\n", buff);
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

	char register_topic_buffer[REGISTER_TOPIC_BUFFER_LENGTH];
	rc = az_iot_provisioning_client_register_get_publish_topic(&provisioning_client, register_topic_buffer, sizeof(register_topic_buffer), NULL);
	if (az_result_failed(rc))
	{
		IOT_SAMPLE_LOG_ERROR("Failed to get the Register topic: az_result return code 0x%08x.", rc);
		exit(rc);
	}

	rc = az_iot_provisioning_client_get_request_payload(
				&provisioning_client,
				custom_registration_payload_property,
				NULL,
				mqtt_payload,
				sizeof(mqtt_payload),
				&mqtt_payload_length);
	if (az_result_failed(rc))
	{
		IOT_SAMPLE_LOG_ERROR(
			"Failed to initialize provisioning client: az_result return code 0x%08x.", rc);
		exit(rc);
	}

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
	// At this point you are free to use my_mqtt_client_id and my_mqtt_user_name to connect using
	// your MQTT client.

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

	az_iot_hub_client_properties_get_reported_publish_topic(&my_client, my_request_id, reported_topic, sizeof(reported_topic), NULL);
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

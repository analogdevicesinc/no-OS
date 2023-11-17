/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for EVAL-ADE9430 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_timer.h"
#include "no_os_i2c.h"
#include "mqtt_client.h"
#include "ade9430.h"
#include "pcf85263.h"
#include "nhd_c12832a1z.h"

#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_irq.h"
#include "maxim_i2c.h"
#include "maxim_spi.h"
#include "maxim_timer.h"
#include "maxim_trng.h"
#include "no_os_irq.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

#include "wifi.h"
#include "tcp_socket.h"

#ifndef DISABLE_SECURE_SOCKET
#include "az_iot_hub_client.h"
#include "az_iot_provisioning_client.h"
#endif

#include "parameters.h"

#ifndef DISABLE_SECURE_SOCKET

static az_iot_hub_client my_client;

static char mqtt_hub_user_name[256];
static char mqtt_hub_client_id[64];

static char reported_topic[128];

static char mqtt_dps_user_name[256];
static char mqtt_dps_client_id[CLIENT_ID_BUFFER_LENGTH];

static uint8_t mqtt_payload[MQTT_PAYLOAD_BUFFER_LENGTH];
static size_t mqtt_payload_length;

static char register_topic_buffer[REGISTER_TOPIC_BUFFER_LENGTH];
#endif

#ifndef DISABLE_SECURE_SOCKET
/* create_and_configure_mqtt_client_for_provisioning */
int create_and_configure_mqtt_client_for_provisioning(void)
{
	int ret;
	char mqtt_endpoint_buffer[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
	az_iot_provisioning_client provisioning_client;
	az_span custom_registration_payload_property
		= AZ_SPAN_LITERAL_FROM_STR(AZ_IOT_PROVISIONING_CUSTOM_PAYLOAD);

	iot_sample_create_mqtt_endpoint(SAMPLE_TYPE,
					mqtt_endpoint_buffer,
					sizeof(mqtt_endpoint_buffer));

	ret = az_iot_provisioning_client_init(&provisioning_client,
					      az_span_create_from_str(mqtt_endpoint_buffer),
					      az_span_create_from_str(AZ_IOT_PROVISIONING_ID_SCOPE),
					      az_span_create_from_str(AZ_IOT_PROVISIONING_REGISTRATION_ID),
					      NULL);
	if (ret != AZ_OK)
		return ret;

	// Get the MQTT client id used for the MQTT connection.
	ret = az_iot_provisioning_client_get_client_id(&provisioning_client,
			mqtt_dps_client_id,
			sizeof(mqtt_dps_client_id),
			NULL);
	if (ret != AZ_OK)
		return ret;

	ret = az_iot_provisioning_client_get_user_name(&provisioning_client,
			mqtt_dps_user_name,
			sizeof(mqtt_dps_user_name),
			NULL);
	if (ret != AZ_OK)
		return ret;

	ret = az_iot_provisioning_client_register_get_publish_topic(
		      &provisioning_client,
		      register_topic_buffer,
		      sizeof(register_topic_buffer),
		      NULL);
	if (ret != AZ_OK)
		return ret;

	return az_iot_provisioning_client_get_request_payload(
		       &provisioning_client,
		       custom_registration_payload_property,
		       NULL,
		       mqtt_payload,
		       sizeof(mqtt_payload),
		       &mqtt_payload_length);
}

/* create_and_configure_mqtt_client_for_iot_hub */
int create_and_configure_mqtt_client_for_iot_hub(void)
{
	int ret;
	size_t mqtt_hub_user_name_length, mqtt_hub_client_id_length;
	az_span my_iothub_hostname = AZ_SPAN_LITERAL_FROM_STR(SERVER_ADDR);
	az_span my_device_id = AZ_SPAN_LITERAL_FROM_STR(
				       AZ_IOT_PROVISIONING_REGISTRATION_ID);
	az_span my_request_id = AZ_SPAN_LITERAL_FROM_STR("1");

	az_iot_hub_client_options options = az_iot_hub_client_options_default();

	// Initialize the hub client with hostname, device id, and default connection options.
	ret = az_iot_hub_client_init(&my_client, my_iothub_hostname,
				     my_device_id, &options);
	if (ret != AZ_OK)
		return ret;

	// Get the MQTT client id used for the MQTT connection.
	ret = az_iot_hub_client_get_client_id(&my_client, mqtt_hub_client_id,
					      sizeof(mqtt_hub_client_id),
					      &mqtt_hub_client_id_length);
	if (ret != AZ_OK)
		return ret;

	// Get the MQTT user name to connect.
	ret = az_iot_hub_client_get_user_name(&my_client, mqtt_hub_user_name,
					      sizeof(mqtt_hub_user_name),
					      &mqtt_hub_user_name_length);
	if (ret != AZ_OK)
		return ret;

	return az_iot_hub_client_properties_get_reported_publish_topic(&my_client,
			my_request_id,
			reported_topic,
			sizeof(reported_topic),
			NULL);
}
#endif

int read_and_send(struct mqtt_desc *mqtt, struct ade9430_dev *ade9430_dev,
		  struct nhd_c12832a1z_dev *nhd_c12832a1z_dev,
		  struct pcf85263_dev *pcf85263_dev)
{
	struct mqtt_message	msg;
	struct pcf85263_date	ts;
	char			buff[100], telemetry_topic[128];
	uint32_t		len;
	int			ret;

	ret = ade9430_read_temp(ade9430_dev);
	if (ret) {
		pr_err("Error reading ADE9430 temperature!\n");
		return ret;
	}

	ret = ade9430_read_data_ph(ade9430_dev, ADE9430_PHASE_A);
	if (ret) {
		pr_err("Error reading ADE9430 phase data!\n");
		return ret;
	}

	/* Serialize data */
	len = sprintf(buff, "Temp: %d; AIRMS: %d; AVRMS: %d; AWATT: %d",
		      ade9430_dev->temp_deg,
		      ade9430_dev->irms_val,
		      ade9430_dev->vrms_val,
		      ade9430_dev->watt_val);

	ret = nhd_c12832a1z_print_string(nhd_c12832a1z_dev, buff);
	if (ret) {
		pr_err("Error printing LCD data!\n");
		return ret;
	}

	ret = pcf85263_read_ts(pcf85263_dev, &ts);
	if (ret) {
		pr_err("Error reading timestamp!\n");
		return ret;
	}
	len = sprintf(buff,
		      "Temp: %d;AIRMS: %d;AVRMS: %d;AWATT: %d;timestamp:20%02d-%02d-%02dT%02d:%02d:%02d+02:00",
		      ade9430_dev->temp_deg,
		      ade9430_dev->irms_val,
		      ade9430_dev->vrms_val,
		      ade9430_dev->watt_val,
		      ts.year,
		      ts.mon,
		      ts.day,
		      ts.hr,
		      ts.min,
		      ts.sec);

	/* Get the topic to send a telemetry message */
#ifndef DISABLE_SECURE_SOCKET
	ret = az_iot_hub_client_telemetry_get_publish_topic(&my_client, NULL,
			telemetry_topic,
			sizeof(telemetry_topic), NULL);
	if (ret != AZ_OK) {
		pr_err("Error getting telemetry topic!\n");
		return ret;
	}
#else
	sprintf(telemetry_topic, MQTT_PUBLISH_TOPIC);
#endif

	/* Send the telemetry message with the MQTT client */
	msg = (struct mqtt_message) {
#ifndef DISABLE_SECURE_SOCKET
		.qos = AZ_HUB_CLIENT_DEFAULT_MQTT_TELEMETRY_QOS,
#else
		.qos = MQTT_QOS0,
#endif
		.payload = buff,
		.len = len,
		.retained = false
	};
	return mqtt_publish(mqtt, telemetry_topic, &msg);
}

void mqtt_message_handler(struct mqtt_message_data *msg)
{
	char	buff[101];
	int32_t	len;

	/* Message.payload don't have at the end '\0' so we have to add it. */
	len = msg->message.len > 100 ? 100 : msg->message.len;
	memcpy(buff, msg->message.payload, len);
	buff[len] = 0;

	pr_info("Topic:%s -- Payload: %s\n", msg->topic, buff);
}

/***************************************************************************//**
 * @brief Main function execution for maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;
	uint32_t len;
	uint8_t send_buff[BUFF_LEN];
	uint8_t read_buff[BUFF_LEN];

	struct max_i2c_init_param i2c_extra = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	};

	/* I2C Initialization Parameters */
	struct no_os_i2c_init_param i2c_ip = {
		.device_id = 1,
		.max_speed_hz = 100000,
		.slave_address = 0x51,
		.platform_ops = &max_i2c_ops,
		.extra = &i2c_extra
	};

	/* RTC Initialization Parameters */
	struct pcf85263_init_param pcf85263_ip = {
		.i2c_init = &i2c_ip,
		.battery_en = 1,
	};

#ifdef RTC_SET_DEFAULT
	struct pcf85263_date pcf85263_date = {
		.sec = RTC_SEC_DEFAULT,
		.min = RTC_MIN_DEFAULT,
		.hr = RTC_HR_DEFAULT,
		.day = RTC_DAY_DEFAULT,
		.mon = RTC_MON_DEFAULT,
		.year = RTC_YEAR_DEFAULT,
	};
#endif
	struct pcf85263_dev *pcf85263_device;

	/* Platform SPI Initialization Parameters */
	struct max_spi_init_param spi_extra_ip  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	};

	/* Energy Meter SPI Initialization Parameters */
	struct no_os_spi_init_param spi_egy_ip = {
		.device_id = 1,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &spi_extra_ip,
	};

	/* ADE9430 Initialization Parameters */
	struct ade9430_init_param ade9430_ip = {
		.spi_init = &spi_egy_ip,
		.temp_en = true,
	};
	struct ade9430_dev *ade9430_device;

	struct no_os_spi_init_param spi_lcd_ip = {
		.device_id = 1,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = &max_spi_ops,
		.chip_select = 2,
		.extra = &spi_extra_ip,
	};

	struct max_gpio_init_param gpio_extra_ip = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct no_os_gpio_init_param gpio_dc_ip = {
		.port = 2,
		.number = 15,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip,
	};

	struct no_os_gpio_init_param gpio_lcd_rst_ip = {
		.port = 1,
		.number = 25,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip,
	};

	struct no_os_gpio_init_param gpio_wifi_rst_ip = {
		.port = 2,
		.number = 13,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip
	};
	struct no_os_gpio_desc *wifi_rst_gpio;

	struct no_os_gpio_init_param green_gpio_led_ip = {
		.port = 1,
		.number = 18,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip
	};
	struct no_os_gpio_desc *green_led_gpio;

	struct no_os_gpio_init_param red_gpio_led_ip = {
		.port = 1,
		.number = 14,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra_ip
	};
	struct no_os_gpio_desc *red_led_gpio;

	/* Display Initialization parameters */
	struct nhd_c12832a1z_init_param nhd_c12832a1z_ip = {
		.spi_ip = &spi_lcd_ip,
		.dc_pin_ip = &gpio_dc_ip,
		.reset_pin_ip = &gpio_lcd_rst_ip
	};
	struct nhd_c12832a1z_dev *nhd_c12832a1z_device;

	struct no_os_irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.platform_ops = &max_irq_ops,
		.extra = NULL
	};
	struct no_os_irq_ctrl_desc *irq_desc;

	struct max_uart_init_param ip = {
		.flow = UART_FLOW_DIS
	};

	struct no_os_uart_init_param luart_par = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &ip
	};
	struct no_os_uart_desc *uart_desc;

	/* Configure Red Led */
	ret = no_os_gpio_get(&red_led_gpio, &red_gpio_led_ip);
	if (ret) {
		pr_err("Error getting red LED gpio!\n");
		return ret;
	}

	ret = no_os_gpio_direction_output(red_led_gpio, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("Error setting red LED gpio low!\n");
		goto error_red_gpio_led;
	}

	ret = pcf85263_init(&pcf85263_device, pcf85263_ip);
	if (ret) {
		pr_err("Error RTC initialization!\n");
		goto error_red_gpio_led;
	}

#ifdef RTC_SET_DEFAULT
	ret = pcf85263_set_date(pcf85263_device, pcf85263_date);
	if (ret) {
		pr_err("Error setting RTC initial date and time!\n");
		goto error_pcf;
	}
#endif

	/* Configure Green Led */
	ret = no_os_gpio_get(&green_led_gpio, &green_gpio_led_ip);
	if (ret) {
		pr_err("Error getting LED gpio!\n");
		goto error_pcf;
	}

	ret = no_os_gpio_direction_output(green_led_gpio, NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("Error setting LED gpio low!\n");
		goto error_green_gpio_led;
	}

	/* Hardware Reset for the Wifi Module */
	ret = no_os_gpio_get(&wifi_rst_gpio, &gpio_wifi_rst_ip);
	if (ret) {
		pr_err("Error getting wifi reset gpio!\n");
		goto error_green_gpio_led;
	}

	ret = no_os_gpio_direction_output(wifi_rst_gpio, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("Error setting wifi reset gpio low!\n");
		goto error_gpio_rst;
	}

	no_os_mdelay(500);

	ret = no_os_gpio_set_value(wifi_rst_gpio, NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("Error setting wifi reset gpio high!\n");
		goto error_gpio_rst;
	}

	/* Allow the wifi module to bring up after reset */
	no_os_mdelay(3000);

	ret = ade9430_init(&ade9430_device, ade9430_ip);
	if (ret) {
		pr_err("Error ADE9430 initialization!\n");
		goto error_ade9430;
	}

	ret = ade9430_set_egy_model(ade9430_device, ADE9430_EGY_NR_SAMPLES,
				    ADE9430_SAMPLES_NR);
	if (ret) {
		pr_err("Error setting the energy use model!\n");
		goto error_ade9430;
	}

	ret = nhd_c12832a1z_init(&nhd_c12832a1z_device, nhd_c12832a1z_ip);
	if (ret) {
		pr_err("Error LCD initialization!\n");
		goto error_nhd;
	}

	ret = no_os_irq_ctrl_init(&irq_desc, &irq_init_param);
	if (ret) {
		pr_err("Error irq controller initialization!\n");
		goto error_nhd;
	}

	ret = no_os_irq_global_enable(irq_desc);
	if (ret) {
		pr_err("Error irq global enable!\n");
		goto error_irq;
	}

	ret = no_os_uart_init(&uart_desc, &luart_par);
	if (ret) {
		pr_err("Error no_os_uart_init!\n");
		goto error_irq;
	}

	static struct wifi_desc *wifi;
	struct wifi_init_param wifi_param = {
		.irq_desc = irq_desc,
		.uart_desc = uart_desc,
		.uart_irq_conf = uart_desc,
		.uart_irq_id = UART_IRQ_ID,
		.sw_reset_en = false
	};
	ret = wifi_init(&wifi, &wifi_param);
	if (ret) {
		pr_err("Error wifi_init!\n");
		goto error_uart;
	}

	ret = wifi_connect(wifi, WIFI_SSID, WIFI_PWD);
	if (ret) {
		pr_err("Error wifi connection!\n");
		goto error_wifi;
	}

	struct tcp_socket_init_param socket_param;
	struct tcp_socket_desc	*sock;
	socket_param.max_buff_size = 0;

	wifi_get_network_interface(wifi, &socket_param.net);
	if (ret) {
		pr_err("Error getting wifi network interface!\n");
		goto error_wifi;
	}

	struct no_os_timer_init_param timer_init_param = {
		.id = TIMER_ID,
		.freq_hz = 32000,
		.ticks_count = 0,
		.platform_ops = &max_timer_ops,
		.extra = NULL
	};

	struct mqtt_connect_config conn_config = {
		.version = MQTT_CONFIG_VERSION,
		.keep_alive_ms = MQTT_CONFIG_KEEP_ALIVE,
		.password = MQTT_CONFIG_CLI_PASS
	};

	struct socket_address mqtt_broker_addr = {
		.port = SERVER_PORT
	};

	/* Initialize mqtt descriptor */
	struct mqtt_init_param  mqtt_init_param = {
		.timer_init_param = &timer_init_param,
		.command_timeout_ms = MQTT_CONFIG_CMD_TIMEOUT,
		.send_buff = send_buff,
		.read_buff = read_buff,
		.send_buff_size = BUFF_LEN,
		.read_buff_size = BUFF_LEN,
		.message_handler = mqtt_message_handler
	};
	struct mqtt_desc		*mqtt;

#ifndef DISABLE_SECURE_SOCKET
	struct mqtt_message	msg;
	char my_ca_cert[] = CA_CERT;
	char my_cli_cert[] = DEVICE_CERT;
	char my_cli_pk[] = DEVICE_PRIVATE_KEY;

	struct no_os_trng_init_param trng_ip = {
		.platform_ops = &max_trng_ops
	};

	struct secure_init_param sip = {
		.trng_init_param = &trng_ip,
		.ca_cert = my_ca_cert,
		.ca_cert_len = NO_OS_ARRAY_SIZE(my_ca_cert),
		.cli_cert = my_cli_cert,
		.cli_cert_len = NO_OS_ARRAY_SIZE(my_cli_cert),
		.cli_pk = my_cli_pk,
		.cli_pk_len = NO_OS_ARRAY_SIZE(my_cli_pk),
		.cert_verify_mode = MBEDTLS_SSL_VERIFY_NONE
	};

	socket_param.secure_init_param = &sip;

#ifdef CONFIG_DPS
	mqtt_broker_addr.addr = DPS_SERVER_ADDR;
	sip.hostname = DPS_SERVER_ADDR;

	ret = socket_init(&sock, &socket_param);
	if (ret) {
		pr_err("Error Azure DPS socket initalization!\n");
		goto error_wifi;
	}

	ret = socket_connect(sock, &mqtt_broker_addr);
	if (ret) {
		pr_err("Error Azure DPS socket connect!\n");
		goto error_sock;
	}

	mqtt_init_param.sock = sock;

	ret = create_and_configure_mqtt_client_for_provisioning();
	if (ret != AZ_OK)
		goto error_sock;

	pr_info("Connection with \"%s\" established\n", mqtt_broker_addr.addr);

	ret = mqtt_init(&mqtt, &mqtt_init_param);
	if (ret) {
		pr_err("Error Azure DPS mqtt init!\n");
		goto error_sock;
	}

	conn_config.client_name = mqtt_dps_client_id;
	conn_config.username = mqtt_dps_user_name;

	ret = mqtt_connect(mqtt, &conn_config, NULL);
	if (ret) {
		pr_err("Error Azure DPS mqtt connect!\n");
		goto error_mqtt;
	}

	ret = mqtt_subscribe(mqtt, AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC,
			     1, NULL);
	if (ret) {
		pr_err("Error Azure DPS MQTT subscribe!\n");
		goto error_mqtt;
	}
	pr_info("Subscribed to topic: %s\n",
		AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC);

	msg = (struct mqtt_message) {
		.qos = MQTT_QOS1,
		.payload = mqtt_payload,
		.len = (int)mqtt_payload_length,
		.retained = false
	};

	ret = mqtt_publish(mqtt, register_topic_buffer, &msg);
	if (ret) {
		pr_err("Error Azure DPS mqtt publish!\n");
		goto error_mqtt;
	}

	ret = mqtt_disconnect(mqtt);
	if (ret) {
		pr_err("Error Azure DPS mqtt disconnect!\n");
		goto error_mqtt;
	}

	ret = mqtt_remove(mqtt);
	if (ret) {
		pr_err("Error Azure DPS mqtt remove!\n");
		goto error_sock;
	}

	ret = socket_disconnect(sock);
	if (ret) {
		pr_err("Error Azure DPS socket disconnect!\n");
		goto error_sock;
	}

	ret = socket_remove(sock);
	if (ret) {
		pr_err("Error Azure DPS socket remove!\n");
		goto error_wifi;
	}
#endif
	mqtt_broker_addr.addr = SERVER_ADDR;
	sip.hostname = SERVER_ADDR;

	ret = socket_init(&sock, &socket_param);
	if (ret) {
		pr_err("Error Azure Hub socket init!\n");
		goto error_wifi;;
	}

	ret = socket_connect(sock, &mqtt_broker_addr);
	if (ret) {
		pr_err("Error Azure Hub socket connect!\n");
		goto error_sock;
	}

	mqtt_init_param.sock = sock;

	ret = mqtt_init(&mqtt, &mqtt_init_param);
	if (ret) {
		pr_err("Error Azure Hub mqtt_init!\n");
		goto error_sock;
	}

	ret = create_and_configure_mqtt_client_for_iot_hub();
	if (ret != AZ_OK)
		goto error_mqtt;

	conn_config.client_name = mqtt_hub_client_id;
	conn_config.username = mqtt_hub_user_name;

	ret = mqtt_connect(mqtt, &conn_config, NULL);
	if (ret) {
		pr_err("Error Azure Hub mqtt connect!\n");
		goto error_mqtt;
	}

	//Subscribe to twin response topic
	ret = mqtt_subscribe(mqtt, AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC,
			     MQTT_QOS0, NULL);
	if (ret) {
		pr_err("Error Azure Hub mqtt subscribe!\n");
		goto error_mqtt;
	}

	pr_info("Subscribed to topic: %s\n",
		AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC);

	char twin_buff[100];
	len = sprintf(twin_buff, "{\"key\":\"value\"}");

	msg = (struct mqtt_message) {
		.qos = AZ_HUB_CLIENT_DEFAULT_MQTT_TELEMETRY_QOS,
		.payload = twin_buff,
		.len = len,
		.retained = false
	};

	ret = mqtt_publish(mqtt, reported_topic, &msg);
	if (ret)
		goto error_mqtt;

	ret = mqtt_yield(mqtt, SCAN_SENSOR_TIME);
	if (ret) {
		pr_err("Error Azure Hub mqtt yield!\n");
		goto error_mqtt;
	}
#else
	mqtt_broker_addr.addr = SERVER_ADDR;

	ret = socket_init(&sock, &socket_param);
	if (ret) {
		pr_err("Error socket_init!\n");
		goto error_wifi;
	}

	ret = socket_connect(sock, &mqtt_broker_addr);
	if (ret) {
		pr_err("Error socket_connect!\n");
		goto error_sock;
	}

	mqtt_init_param.sock = sock;

	ret = mqtt_init(&mqtt, &mqtt_init_param);
	if (ret) {
		pr_err("Error mqtt_init!\n");
		goto error_sock;
	}

	conn_config.client_name = MQTT_CONFIG_CLIENT_NAME;
	conn_config.username = MQTT_CONFIG_CLI_USER;

	ret = mqtt_connect(mqtt, &conn_config, NULL);
	if (ret) {
		pr_err("Error mqtt_connect!\n");
		goto error_mqtt;
	}

	pr_info("Connected to mqtt broker\n");

	ret = mqtt_subscribe(mqtt, MQTT_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (ret) {
		pr_err("Error mqtt_subscribe!\n");
		goto error_mqtt;
	}

	pr_info("Subscribed to topic: %s\n", MQTT_SUBSCRIBE_TOPIC);
#endif

	ret = no_os_gpio_set_value(red_led_gpio, NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("Error setting red LED gpio high!\n");
		goto error_mqtt;
	}

	while (true) {
		ret = no_os_gpio_set_value(green_led_gpio, NO_OS_GPIO_LOW);
		if (ret) {
			pr_err("Error setting green LED gpio high!\n");
			goto error_mqtt;
		}

		ret = read_and_send(mqtt, ade9430_device, nhd_c12832a1z_device,
				    pcf85263_device);
		if (ret) {
			pr_err("Error read_and_send!\n");
			goto error_mqtt;
		}

		ret = no_os_gpio_set_value(green_led_gpio, NO_OS_GPIO_HIGH);
		if (ret) {
			pr_err("Error setting green LED gpio high!\n");
			goto error_mqtt;
		}

		pr_info("Data sent to broker\n");

		no_os_mdelay(1000);

		/* Dispatch new mqtt mesages if any during SCAN_SENSOR_TIME */
		ret = mqtt_yield(mqtt, SCAN_SENSOR_TIME);
		if (ret) {
			pr_err("Error mqtt_yield!\n");
			goto error_mqtt;
		}
	}

	return 0;

error_mqtt:
	mqtt_remove(mqtt);
error_sock:
	socket_remove(sock);
error_wifi:
	wifi_remove(wifi);
error_uart:
	no_os_uart_remove(uart_desc);
error_irq:
	no_os_irq_ctrl_remove(irq_desc);
error_nhd:
	nhd_c12832a1z_remove(nhd_c12832a1z_device);
error_ade9430:
	ade9430_remove(ade9430_device);
error_gpio_rst:
	no_os_gpio_remove(wifi_rst_gpio);
error_green_gpio_led:
	no_os_gpio_remove(green_led_gpio);
error_pcf:
	pcf85263_remove(pcf85263_device);

	while (true) {
		ret = no_os_gpio_set_value(red_led_gpio, NO_OS_GPIO_HIGH);
		if (ret) {
			pr_err("Error setting LED gpio high!\n");
			goto error_red_gpio_led;
		}

		no_os_mdelay(500);

		ret = no_os_gpio_set_value(red_led_gpio, NO_OS_GPIO_LOW);
		if (ret) {
			pr_err("Error setting LED gpio low!\n");
			goto error_red_gpio_led;
		}

		no_os_mdelay(500);
	}

error_red_gpio_led:
	no_os_gpio_remove(green_led_gpio);

	return ret;
}

/***************************************************************************//**
 *   @file   swiot1l_mqtt.c
 *   @brief  Source file for the swiot1l mqtt example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

// TODO why?? is this an absolute path
#include "iio_adt75.h"
#include "swiot1l_mqtt.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "mqtt_client.h"
#include "mqtt_noos_support.h"
#include "no_os_timer.h"
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "maxim_trng.h"
#include "mbedtls/debug.h"

#include "MXQ_API.h"
#include "USS_API.h"

static void message_handler(struct mqtt_message_data *msg)
{
	msg->message.payload[msg->message.len] = 0;
	printf("Topic:%s -- Payload: %s\n", msg->topic, msg->message.payload);
}

int swiot1l_mqtt()
{

	uint8_t adin1110_mac_address[6] = {0x00, 0xe0, 0x22, 0x03, 0x25, 0x60};
	uint8_t send_buff[256];
	uint8_t read_buff[256];
	struct ad74413r_decimal val;
	char val_buff[32];
	uint32_t msg_len;
	int32_t adt75_val;
	int ret;

	struct ad74413r_desc *ad74413r;
	struct lwip_network_param lwip_ip = {
		.platform_ops = &adin1110_lwip_ops,
		.mac_param = &adin1110_ip,
	};

	struct adt75_desc *adt75;

	struct lwip_network_desc *lwip_desc;
	struct tcp_socket_desc *tcp_socket;
	struct no_os_timer_init_param adc_demo_tip = {
		.id = 0,
		.freq_hz = 32000,
		.ticks_count = 0,
		.platform_ops = &max_timer_ops,
		.extra = NULL,
	};
	uint32_t connect_timeout = 5000;

    ret = adt75_init(&adt75, &adt75_ip);
    if (ret) {
        printf("Failed to initialise ADT75 - %d\r\n", ret);
        return ret;
    }

    printf("ADT75 Sensor Initialised - %d\r\n", ret);

	struct no_os_gpio_desc *ad74413r_ldac_gpio;
	struct no_os_gpio_desc *ad74413r_reset_gpio;
	struct no_os_gpio_desc *ad74413r_irq_gpio;
	struct no_os_gpio_desc *max14906_en_gpio;
	struct no_os_gpio_desc *max14906_d1_gpio;
	struct no_os_gpio_desc *max14906_d2_gpio;
	struct no_os_gpio_desc *max14906_d3_gpio;
	struct no_os_gpio_desc *max14906_d4_gpio;
	struct no_os_gpio_desc *max14906_synch_gpio;
	struct no_os_gpio_desc *adin1110_swpd_gpio;
	struct no_os_gpio_desc *adin1110_tx2p4_gpio;
	struct no_os_gpio_desc *adin1110_mssel_gpio;
	struct no_os_gpio_desc *adin1110_cfg0_gpio;
	struct no_os_gpio_desc *adin1110_cfg1_gpio;
	struct no_os_gpio_desc *adin1110_int_gpio;
	struct no_os_gpio_desc *swiot_led1_gpio;
	struct no_os_gpio_desc *swiot_led2_gpio;

	no_os_gpio_get(&swiot_led1_gpio, &swiot_led1_ip);
	no_os_gpio_get(&swiot_led2_gpio, &swiot_led2_ip);
	no_os_gpio_get(&max14906_d1_gpio, &max14906_d1_ip);
	no_os_gpio_get(&max14906_d2_gpio, &max14906_d2_ip);
	no_os_gpio_get(&max14906_d3_gpio, &max14906_d3_ip);
	no_os_gpio_get(&max14906_d4_gpio, &max14906_d4_ip);
	no_os_gpio_direction_output(max14906_d1_gpio, 0);
	no_os_gpio_direction_output(max14906_d2_gpio, 0);
	no_os_gpio_direction_output(max14906_d3_gpio, 0);
	no_os_gpio_direction_output(max14906_d4_gpio, 0);
	no_os_gpio_get(&max14906_en_gpio, &max14906_en_ip);
	no_os_gpio_direction_output(max14906_en_gpio, 0);
	no_os_gpio_get(&adin1110_cfg0_gpio, &adin1110_cfg0_ip);
	no_os_gpio_get(&ad74413r_ldac_gpio, &ad74413r_ldac_ip);
	no_os_gpio_get(&ad74413r_reset_gpio, &ad74413r_reset_ip);
	no_os_gpio_get(&ad74413r_irq_gpio, &ad74413r_irq_ip);
	no_os_gpio_get(&max14906_synch_gpio, &max14906_synch_ip);
	no_os_gpio_get(&adin1110_swpd_gpio, &adin1110_swpd_ip);
	no_os_gpio_get(&adin1110_tx2p4_gpio, &adin1110_tx2p4_ip);
	no_os_gpio_get(&adin1110_mssel_gpio, &adin1110_mssel_ip);
	no_os_gpio_get(&adin1110_cfg1_gpio, &adin1110_cfg1_ip);
	no_os_gpio_get(&adin1110_int_gpio, &adin1110_int_ip);
	no_os_gpio_direction_output(ad74413r_ldac_gpio, 0);
	no_os_gpio_direction_output(ad74413r_reset_gpio, 1);
	no_os_gpio_direction_output(max14906_synch_gpio, 1);
	no_os_gpio_direction_output(adin1110_swpd_gpio, 1);
	no_os_gpio_direction_output(adin1110_tx2p4_gpio, 0);
	no_os_gpio_direction_output(adin1110_mssel_gpio, 1);
	no_os_gpio_direction_output(adin1110_cfg1_gpio, 1);
	no_os_gpio_direction_output(adin1110_cfg0_gpio, 1);
	no_os_gpio_direction_output(swiot_led1_gpio, 1);
	no_os_gpio_direction_output(swiot_led2_gpio, 1);
	no_os_gpio_direction_input(adin1110_int_gpio);
	no_os_gpio_direction_input(ad74413r_irq_gpio);

	ret = ad74413r_init(&ad74413r, &ad74413r_ip);
	if (ret)
		goto free_gpio;

	ad74413r_set_channel_function(ad74413r, 0, AD74413R_VOLTAGE_IN);
	ad74413r_set_channel_function(ad74413r, 1, AD74413R_CURRENT_OUT);
	ad74413r_set_channel_function(ad74413r, 2, AD74413R_RESISTANCE);
	ad74413r_set_channel_function(ad74413r, 3, AD74413R_VOLTAGE_OUT);

	ad74413r_set_adc_rejection(ad74413r, 0, AD74413R_REJECTION_NONE);
	ad74413r_set_adc_rejection(ad74413r, 1, AD74413R_REJECTION_NONE);
	ad74413r_set_adc_rejection(ad74413r, 2, AD74413R_REJECTION_NONE);
	ad74413r_set_adc_rejection(ad74413r, 3, AD74413R_REJECTION_NONE);

	ad74413r_set_channel_dac_code(ad74413r, 1, 1000);
	ad74413r_set_channel_dac_code(ad74413r, 3, 3000);

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	memcpy(lwip_ip.hwaddr, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);

	ret = no_os_lwip_init(&lwip_desc, &lwip_ip);
	if (ret) {
		pr_err("LWIP init error: %d (%s)\n", ret, strerror(-ret));
		goto free_ad74413r;
	}
	struct tcp_socket_init_param tcp_ip = {
		.net = &lwip_desc->no_os_net,
		.max_buff_size = 0,

	};
	

	//char my_ca_cert[] = CA_CERT;
	//char my_cli_cert[] = DEVICE_CERT;
	//char my_cli_pk[] = DEVICE_PRIVATE_KEY;

	USS_Module_Init();
    if(USS_Ping(4) == USS_OK )
        printf("\nSuccessfully initialize.\n");
    else
        printf("\n initialize failed. \n");

    // enable MAXQ106x
    MXQ_Module_Init();

    // reset the MAXQ106x
    MXQ_Reset();

    printf("MQTT client example\r\n\r\n");


	mxq_u1 my_cli_cert[1024];
    mxq_length certlen=1024;
    printf("Device certificate\r\n\r\n");
    MXQ_ReadObject(0x1002,0,(mxq_u1*)&my_cli_cert,&certlen);
    //print_buffer(my_cli_cert,certlen);

	mxq_u1 my_ca_cert[1024];
    mxq_length ca_certlen=1024;
    printf("Device certificate\r\n\r\n");
    MXQ_ReadObject(0x1002,0,(mxq_u1*)&my_ca_cert,&ca_certlen);
    //print_buffer(my_ca_cert,ca_certlen);

	mxq_u1 my_cli_pk[1024];
    mxq_length my_cli_pk_len=1024;
    printf("Device certificate\r\n\r\n");
    MXQ_ReadObject(0x1002,0,(mxq_u1*)&my_cli_pk,&my_cli_pk_len);
    //print_buffer(my_cli_pk,my_cli_pk_len);


	struct no_os_trng_init_param trng_ip = {
		.platform_ops = &max_trng_ops
	};

	struct secure_init_param secure_params = {
		.trng_init_param = &trng_ip,
		.ca_cert = my_ca_cert,
		.ca_cert_len = NO_OS_ARRAY_SIZE(my_ca_cert),
		.cli_cert = my_cli_cert,
		.cli_cert_len = NO_OS_ARRAY_SIZE(my_cli_cert),
		.cli_pk = my_cli_pk,
		.cli_pk_len = NO_OS_ARRAY_SIZE(my_cli_pk),
		.cert_verify_mode = MBEDTLS_SSL_VERIFY_NONE
	};


	tcp_ip.secure_init_param = &secure_params;

	ret = socket_init(&tcp_socket, &tcp_ip);
	if (ret) {
		pr_err("Socket init error: %d (%s)\n", ret, strerror(-ret));
		goto free_ad74413r;
	}

	/* The default settings are 192.168.97.1:1883 */
	struct socket_address ip_addr = {
		.port = 8883,
		.addr = SWIOT1L_MQTT_SERVER_IP
	};

	struct mqtt_desc *mqtt;
	struct mqtt_init_param	mqtt_init_param = {
		.timer_init_param = &adc_demo_tip,
		.sock = tcp_socket,
		.command_timeout_ms = 20000,
		.send_buff = send_buff,
		.read_buff = read_buff,
		.send_buff_size = 256,
		.read_buff_size = 256,
		.message_handler = message_handler
	};

	ret = mqtt_init(&mqtt, &mqtt_init_param);
	if (ret) {
		pr_err("MQTT init error: %d (%s)\n", ret, strerror(-ret));
		goto free_socket;
	}

	struct mqtt_connect_config conn_config = {
		.version = MQTT_VERSION_3_1_1,
		.keep_alive_ms = 1000,
		.client_name = "maxim",
		.username = NULL,
		.password = NULL
	};
	//ret is not zero
	ret = socket_connect(tcp_socket, &ip_addr);
	if (ret) {
		pr_err("Couldn't connect to the remote TCP socket: %d (%s)\n", ret,
		       strerror(-ret));
		goto free_mqtt;
	}

	while (connect_timeout--) {
		no_os_lwip_step(tcp_socket->net->net, NULL);
		no_os_mdelay(1);
	}

	ret = mqtt_connect(mqtt, &conn_config, NULL);
	if (ret) {
		socket_disconnect(tcp_socket);
		pr_err("Couldn't connect to the MQTT broker: %d (%s)\n", ret, strerror(-ret));
		goto free_mqtt;
	}

	struct mqtt_message test_msg = {
		.qos = 0,
		.payload = val_buff,
		.retained = false
	};

	while (1) {
		no_os_lwip_step(tcp_socket->net->net, NULL);

		ad74413r_adc_get_value(ad74413r, 0, &val);
		memset(val_buff, 0, sizeof(val_buff));
		if (val.integer == 0 && val.decimal < 0)
			msg_len = snprintf(val_buff, sizeof(val_buff), "-%lld mV", val.integer,
					   abs(val.decimal));
		else
			msg_len = snprintf(val_buff, sizeof(val_buff), "%lld mV", val.integer,
					   abs(val.decimal));
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel0", &test_msg);
		if (ret) {
			pr_err("Error publishing MQTT message: %d (%s)\n", ret, strerror(-ret));
			goto free_mqtt;
		}

		ad74413r_adc_get_value(ad74413r, 1, &val);
		memset(val_buff, 0, sizeof(val_buff));
		if (val.integer == 0 && val.decimal < 0)
			msg_len = snprintf(val_buff, sizeof(val_buff), "-%lld mV",
					   val.integer / 1000,
					   abs(val.decimal));
		else
			msg_len = snprintf(val_buff, sizeof(val_buff), "%lld mV", val.integer,
					   abs(val.decimal));
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel1", &test_msg);
		if (ret) {
			pr_err("Error publishing MQTT message: %d (%s)\n", ret, strerror(-ret));
			goto free_mqtt;
		}

		ad74413r_adc_get_value(ad74413r, 2, &val);
		memset(val_buff, 0, sizeof(val_buff));
		msg_len = snprintf(val_buff, sizeof(val_buff), "%lld Ω",
				   val.integer / 1000,
				   abs(val.decimal));
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel2", &test_msg);
		if (ret) {
			pr_err("Error publishing MQTT message: %d (%s)\n", ret, strerror(-ret));
			goto free_mqtt;
		}

		ad74413r_adc_get_value(ad74413r, 3, &val);
		memset(val_buff, 0, sizeof(val_buff));

		if (val.integer == 0 && val.decimal < 0)
			msg_len = snprintf(val_buff, sizeof(val_buff), "-%lld"".%02lu mA",
					   val.integer,
					   abs(val.decimal / 1000000));
		else
			msg_len = snprintf(val_buff, sizeof(val_buff), "%lld"".%02lu mA",
					   val.integer,
					   abs(val.decimal / 1000000));
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel3", &test_msg);
		if (ret) {
			pr_err("Error publishing MQTT message: %d (%s)\n", ret, strerror(-ret));
			goto free_mqtt;
		}

		ret = adt75_get_single_temp(adt75, &adt75_val);
		memset(val_buff, 0, sizeof(val_buff));

		if (!ret) {
			msg_len = snprintf(val_buff, sizeof(val_buff), "%.03f", ((double)adt75_val / 1000));
			printf("Temperature reading: \e[96m%.03f\e[0m degrees C\r\n\r\n", ((double) adt75_val / 1000));
		} else {
			msg_len = snprintf(val_buff, sizeof(val_buff), "Null");
			printf("No Valid Data - %d\r\n", ret);
		}

		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "adt75/temperature", &test_msg);

		no_os_mdelay(1000);
	}
	

	printf("\nsubscribing \n");
	ret = mqtt_subscribe(mqtt, "test", MQTT_QOS0, NULL);
	if (ret) {
		// pr_err("Error mqtt_subscribe!\n");
		goto free_mqtt;
	}

	pr_info("Subscribed to topic: %s\n", "test");

	ret = mqtt_yield(mqtt, 50000);
	if (ret) {
		pr_err("Error mqtt_yield!\n");
		goto free_mqtt;
	}

	// ret = mqtt_subscribe(mqtt, "test", MQTT_QOS0, NULL);
	// if (ret) {
	// 	pr_err("Error mqtt_subscribe!\n");
	// 	goto free_mqtt;
	// }
	// printf("Subscribed!!");
	// ret = mqtt_yield(mqtt, 50000);
	// if (ret) {
	// 	pr_err("Error mqtt_yield!\n");
	// 	goto free_mqtt;
	// }





	return 0;

free_mqtt:
	mqtt_remove(mqtt);
free_socket:
	socket_remove(tcp_socket);
free_lwip:
	no_os_lwip_remove(lwip_desc);
free_ad74413r:
	ad74413r_remove(ad74413r);
free_gpio:
	no_os_gpio_remove(adin1110_int_gpio);
	no_os_gpio_remove(adin1110_cfg1_gpio);
	no_os_gpio_remove(adin1110_mssel_gpio);
	no_os_gpio_remove(adin1110_tx2p4_gpio);
	no_os_gpio_remove(adin1110_swpd_gpio);
	no_os_gpio_remove(max14906_synch_gpio);
	no_os_gpio_remove(ad74413r_irq_gpio);
	no_os_gpio_remove(ad74413r_reset_gpio);
	no_os_gpio_remove(ad74413r_ldac_gpio);
	no_os_gpio_remove(adin1110_cfg0_gpio);
	no_os_gpio_remove(max14906_en_gpio);
	no_os_gpio_remove(max14906_d4_gpio);
	no_os_gpio_remove(max14906_d3_gpio);
	no_os_gpio_remove(max14906_d2_gpio);
	no_os_gpio_remove(max14906_d1_gpio);
	no_os_gpio_remove(swiot_led2_gpio);
	no_os_gpio_remove(swiot_led1_gpio);

	return ret;
}



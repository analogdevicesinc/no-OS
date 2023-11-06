#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "swiot1l_mqtt.h"
#include "iio_ad74413r.h"
#include "iio_max14906.h"
#include "iio_adt75.h"
#include "swiot.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "mqtt_client.h"
#include "mqtt_noos_support.h"
#include "no_os_timer.h"

#include "lwip_adin1110.h"

static void message_handler(struct mqtt_message_data *msg)
{
	msg->message.payload[msg->message.len] = 0;
	printf("Topic:%s -- Payload: %s\n", msg->topic, msg->message.payload);
}

int swiot1l_mqtt()
{
	uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x60};
	uint8_t send_buff[1000];
	uint8_t read_buff[1000];
	struct ad74413r_decimal val;
	uint8_t val_buff[50];
	uint32_t msg_len;
	int ret;

	struct ad74413r_desc *ad74413r;
	struct lwip_network_param lwip_ip = {
		.platform_ops = &adin1110_lwip_ops,
		.mac_param = &adin1110_ip,
	};
	struct lwip_network_desc *lwip_desc;
	struct tcp_socket_desc *tcp_socket;
	// struct connection *client_socket;
	struct no_os_timer_init_param adc_demo_tip = {
		.id = 0,
		.freq_hz = 32000,
		.ticks_count = 0,
		.platform_ops = &max_timer_ops,
		.extra = NULL,
	};
	struct no_os_timer_desc *timer;
	uint32_t connect_timeout = 5000;

	struct no_os_gpio_desc *ad74413r_ldac_gpio;
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
	no_os_gpio_get(&ad74413r_irq_gpio, &ad74413r_irq_ip);
	no_os_gpio_get(&max14906_synch_gpio, &max14906_synch_ip);
	no_os_gpio_get(&adin1110_swpd_gpio, &adin1110_swpd_ip);
	no_os_gpio_get(&adin1110_tx2p4_gpio, &adin1110_tx2p4_ip);
	no_os_gpio_get(&adin1110_mssel_gpio, &adin1110_mssel_ip);
	no_os_gpio_get(&adin1110_cfg1_gpio, &adin1110_cfg1_ip);
	no_os_gpio_get(&adin1110_int_gpio, &adin1110_int_ip);
	no_os_gpio_direction_output(ad74413r_ldac_gpio, 0);
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

	const mxc_gpio_cfg_t gpio_cfg = { MXC_GPIO1, (MXC_GPIO_PIN_24), MXC_GPIO_FUNC_OUT,
                                           MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_DRVSTR_3 };
	MXC_GPIO_Init(1);
	MXC_GPIO_Config(&gpio_cfg);

	ret = ad74413r_init(&ad74413r, &ad74413r_ip);
	if (ret)
		return ret;

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
	if (ret)
		return ret;

	struct tcp_socket_init_param tcp_ip = {
		.net = &lwip_desc->no_os_net,
		.max_buff_size = 0
	};

	ret = socket_init(&tcp_socket, &tcp_ip);
	if (ret)
		return ret;

	struct socket_address ip_addr = {
		.addr = "169.254.97.30",
		.port = 1883
	};

	struct mqtt_desc *mqtt;
  	struct mqtt_init_param	mqtt_init_param = {
  		.timer_init_param = &adc_demo_tip,
		.sock = tcp_socket,
  		.command_timeout_ms = 20000,
  		.send_buff = send_buff,
  		.read_buff = read_buff,
  		.send_buff_size = 700,
  		.read_buff_size = 700,
  		.message_handler = message_handler
  	};

	ret = mqtt_init(&mqtt, &mqtt_init_param);

 	struct mqtt_connect_config conn_config = {
  		.version = MQTT_VERSION_3_1_1,
  		.keep_alive_ms = 72000,
  		.client_name = (int8_t *)"maxim",
  		.username = NULL,
  		.password = NULL
  	};

	ret = socket_connect(tcp_socket, &ip_addr);
	if (ret)
		return ret;

	while (connect_timeout--) {
		no_os_lwip_step(tcp_socket->net->net, NULL);
		no_os_mdelay(1);
	}

  	ret = mqtt_connect(mqtt, &conn_config, NULL);
	if (ret) {
		socket_disconnect(tcp_socket);
		return 0;
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
			msg_len = snprintf(val_buff, sizeof(val_buff), "-%lld.%d", val.integer, abs(val.decimal));
		else
			msg_len = snprintf(val_buff, sizeof(val_buff), "%lld.%d", val.integer, val.decimal);
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel0", &test_msg);
		if (ret)
			return ret;

		ad74413r_adc_get_value(ad74413r, 1, &val);
		memset(val_buff, 0, sizeof(val_buff));
		if (val.integer == 0 && val.decimal < 0)
			msg_len = snprintf(val_buff, sizeof(val_buff), "-%lld.%d", val.integer, abs(val.decimal));
		else
			msg_len = snprintf(val_buff, sizeof(val_buff), "%lld.%d", val.integer, val.decimal);
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel1", &test_msg);

		ad74413r_adc_get_value(ad74413r, 2, &val);
		memset(val_buff, 0, sizeof(val_buff));
		if (val.integer == 0 && val.decimal < 0)
			msg_len = snprintf(val_buff, sizeof(val_buff), "-%lld.%d", val.integer, abs(val.decimal));
		else
			msg_len = snprintf(val_buff, sizeof(val_buff), "%lld.%d", val.integer, val.decimal);
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel2", &test_msg);

		ad74413r_adc_get_value(ad74413r, 3, &val);
		memset(val_buff, 0, sizeof(val_buff));

		if (val.integer == 0 && val.decimal < 0)
			msg_len = snprintf(val_buff, sizeof(val_buff), "-%lld.%d", val.integer, abs(val.decimal));
		else
			msg_len = snprintf(val_buff, sizeof(val_buff), "%lld.%d", val.integer, val.decimal);
		test_msg.len = msg_len;
		ret = mqtt_publish(mqtt, "ad74413r/channel3", &test_msg);

		no_os_mdelay(1000);
		// mqtt_publish(mqtt, "ad74413r/channel1", &test_msg);
		// mqtt_publish(mqtt, "ad74413r/channel2", &test_msg);
		// mqtt_publish(mqtt, "ad74413r/channel3", &test_msg);
	}
	

	while(1) {
		no_os_lwip_step(tcp_socket->net->net, NULL);
	}

	return 0;
}


#include <stdio.h>
#include "no_os_init.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "no_os_delay.h"

#include "ad717x.h"
#include "ad411x_regs.h"

#include "no_os_gpio.h"
#include "maxim_gpio.h"
#include "maxim_irq.h"
#include "maxim_timer.h"

#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"
#include "iio_app.h"
#include "iio_ad717x.h"

#include "no_os_print_log.h"


#include "wifi.h"

#include "mqtt_client.h"
#include "parameters.h"


/* Connect to local mqtt broker */
#define LOCAL			0
/* Connect to online broker */
#define TEST_MOSQUITTO		1
#define MQTT_ECLIPSE		2

#if SERVER_TO_USE==TEST_MOSQUITTO
#define SERVER_ADDR		"test.mosquitto.org"
#elif SERVER_TO_USE==MQTT_ECLIPSE
#define SERVER_ADDR		"mqtt.eclipse.org"
#else //local
#define SERVER_ADDR		"10.42.0.1"
#endif

#define SERVER_PORT		1883

/* Maximum buffer size for a message received from mqtt broker */
#define BUFF_LEN		200

/* Wifi network parameters */
#define WIFI_SSID		"workshop"
#define WIFI_PASS		"analog2023"

/* Mqtt configuration */
#define MQTT_PUBLISH_TOPIC	"sensor"
#define MQTT_SUBSCRIBE_TOPIC	"esp_messages"
#define MQTT_CONFIG_CMD_TIMEOUT	20000
#define MQTT_CONFIG_VERSION	MQTT_VERSION_3_1
#define MQTT_CONFIG_KEEP_ALIVE	7200
#define MQTT_CONFIG_CLIENT_NAME	"esp-client"
#define MQTT_CONFIG_CLI_USER	NULL
#define MQTT_CONFIG_CLI_PASS	NULL
#define MQTT_PAYLOAD_BUFF_LEN	100

/* Time between each scan mq*/
#define SCAN_SENSOR_TIME	500

struct max_uart_init_param uart_extra_ip = {
	.flow = UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};



#define UART_CONFIG_BAUDRATE	115200
#define UART_CONFIG_IRQ_ID	UART2_IRQn
#define UART_DEVICE_ID		2
#define UART_EXTRA		&uart_extra_ip

#define GPIO_PORT		3
#define GPIO_NR			9
#define GPIO_EXTRA		&gpio_extra_ip

#define TIMER_ID		1
#define TIMER_FREQ		32000

#define IRQ_OPS			&max_irq_ops
#define UART_OPS		&max_uart_ops
#define TIMER_OPS		&max_timer_ops
#define GPIO_OPS		&max_gpio_ops
#define TIMER_EXTRA		NULL


struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_CONFIG_BAUDRATE,
	.irq_id = UART_CONFIG_IRQ_ID,
	.asynchronous_rx = true,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA
};

struct no_os_irq_init_param irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = IRQ_OPS,
	.extra = 0
};

struct no_os_timer_init_param timer_ip = {
	.id = TIMER_ID,
	.freq_hz = TIMER_FREQ,
	.ticks_count = 0,
	.platform_ops = TIMER_OPS,
	.extra = TIMER_EXTRA
};

struct no_os_gpio_init_param gpio_wifi_rst_ip = {
	.port = GPIO_PORT,
	.number = GPIO_NR,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};





int32_t init_and_connect_wifi(struct wifi_desc **wifi)
{
	static struct no_os_irq_ctrl_desc	*irq_ctrl;
	static struct no_os_uart_desc		*udesc;
	struct no_os_gpio_desc			*wifi_rst_gpio;
	struct wifi_init_param			wifi_param;
	int32_t				ret;

	if (!WIFI_SW_RESET) {
		ret = no_os_gpio_get(&wifi_rst_gpio, &gpio_wifi_rst_ip);
		if (ret) {
			pr_err("Error getting wifi reset gpio!\n");
			return ret;
		}

		ret = no_os_gpio_direction_output(wifi_rst_gpio, NO_OS_GPIO_LOW);
		if (ret) {
			pr_err("Error setting wifi reset gpio low!\n");
			goto error_gpio;
		}

		no_os_mdelay(1000);

		ret = no_os_gpio_set_value(wifi_rst_gpio, NO_OS_GPIO_HIGH);
		if (ret) {
			pr_err("Error setting wifi reset gpio high!\n");
			goto error_gpio;
		}

		/* Allow the wifi module to bring up after reset */
		no_os_mdelay(5000);
	}

	ret = no_os_irq_ctrl_init(&irq_ctrl, &irq_ip);
	if (ret) {
		pr_err("Error irq_ctrl_init\n");
		goto error_gpio;
	}

	ret = no_os_uart_init(&udesc, &uart_ip);
	if (ret) {
		pr_err("Error uart_init\n");
		goto error_irq;
	}

	/* Initialize wifi descriptor */
	wifi_param = (struct wifi_init_param) {
		.irq_desc = irq_ctrl,
		.uart_desc = udesc,
		.uart_irq_conf = udesc,
		.uart_irq_id = UART_CONFIG_IRQ_ID,
		.sw_reset_en = WIFI_SW_RESET
	};

	ret = wifi_init(wifi, &wifi_param);
	if (ret) {
		pr_err("Error wifi_init\n");
		goto error_uart;
	}

	/* Connect to wifi network */
	ret = wifi_connect(*wifi, WIFI_SSID, WIFI_PASS);
	if (ret) {
		pr_err("Error wifi_connect\n");
		goto error_wifi;
	}

	printf("Connected to: %s\n", WIFI_SSID);

	return 0;

error_wifi:
	wifi_remove(*wifi);
error_uart:
	no_os_uart_remove(udesc);
error_irq:
	no_os_irq_ctrl_remove(irq_ctrl);
error_gpio:
	if (!WIFI_SW_RESET)
		no_os_gpio_remove(wifi_rst_gpio);

	return ret;
}

void mqtt_message_handler(struct mqtt_message_data *msg)
{
	char	buff[MQTT_PAYLOAD_BUFF_LEN];
	int32_t	len;

	/* Message.payload don't have at the end '\0' so we have to add it. */
	len = msg->message.len > MQTT_PAYLOAD_BUFF_LEN ?
	      MQTT_PAYLOAD_BUFF_LEN : msg->message.len;
	memcpy(buff, msg->message.payload, len);
	buff[len] = 0;

	printf("Topic:%s -- Payload: %s\n", msg->topic, buff);
}

int init_and_connect_to_mqtt_broker(struct mqtt_desc **mqtt,
				    struct wifi_desc *wifi)
{
	static uint8_t			send_buff[BUFF_LEN];
	static uint8_t			read_buff[BUFF_LEN];
	static struct tcp_socket_desc	*sock;

	struct mqtt_init_param		mqtt_init_param;
	struct tcp_socket_init_param	socket_init_param;
	struct socket_address		mqtt_broker_addr;
	struct mqtt_connect_config	conn_config;
	int32_t				ret;

	/* Initialize socket structure */
	socket_init_param.max_buff_size = 0; //Default buffer size
	ret = wifi_get_network_interface(wifi, &socket_init_param.net);
	if (ret) {
		pr_err("Error wifi_get_network_interface\n");
		goto error_wifi;
	}

	ret = socket_init(&sock, &socket_init_param);
	if (ret) {
		pr_err("Error socket_init", ret);
		goto error_wifi;
	}

	/* Connect socket to mqtt borker server */
	mqtt_broker_addr = (struct socket_address) {
		.addr = SERVER_ADDR,
		.port = SERVER_PORT
	};
	ret = socket_connect(sock, &mqtt_broker_addr);
	if (ret) {
		pr_err("Error socket_connect\n");
		goto error_sock;
	}

	printf("Connection with \"%s\" established\n", SERVER_ADDR);

	/* Initialize mqtt descriptor */
	mqtt_init_param = (struct mqtt_init_param) {
		.timer_init_param = &timer_ip,
		.sock = sock,
		.command_timeout_ms = MQTT_CONFIG_CMD_TIMEOUT,
		.send_buff = send_buff,
		.read_buff = read_buff,
		.send_buff_size = BUFF_LEN,
		.read_buff_size = BUFF_LEN,
		.message_handler = mqtt_message_handler
	};
	ret = mqtt_init(mqtt, &mqtt_init_param);
	if (ret) {
		pr_err("Error mqtt_init\n");
		goto error_sock;
	}

	/* Connect to mqtt broker */
	conn_config = (struct mqtt_connect_config) {
		.version = MQTT_CONFIG_VERSION,
		.keep_alive_ms = MQTT_CONFIG_KEEP_ALIVE,
		.client_name = (int8_t *)MQTT_CONFIG_CLIENT_NAME,
		.username = MQTT_CONFIG_CLI_USER,
		.password = MQTT_CONFIG_CLI_PASS
	};

	ret = mqtt_connect(*mqtt, &conn_config, NULL);
	if (ret) {
		pr_err("Error mqtt_connect\n");
		goto error_mqtt;
	}

	printf("Connected to mqtt broker\n");

	/* Subscribe for a topic */
	ret = mqtt_subscribe(*mqtt, (const int8_t *)MQTT_SUBSCRIBE_TOPIC, MQTT_QOS0, NULL);
	if (ret) {
		pr_err("Error mqtt_subscribe\n");
		goto error_mqtt;
	}

	printf("Subscribed to topic: %s\n", MQTT_SUBSCRIBE_TOPIC);

	return 0;

error_wifi:
	wifi_remove(wifi);
error_mqtt:
	mqtt_remove(*mqtt);
error_sock:
	socket_remove(sock);

	return ret;
}

int32_t mqtt_send(struct mqtt_desc *mqtt)
{
	struct mqtt_message	msg;
	char			buff[100];
	uint32_t		len;

	/* Serialize data */
	len = sprintf(buff, "Data sent to broker.");

	/* Send data to mqtt broker */
	msg = (struct mqtt_message) {
		.qos = MQTT_QOS0,
		.payload = (uint8_t *)buff,
		.len = len,
		.retained = false
	};
	return mqtt_publish(mqtt, (const int8_t *)MQTT_PUBLISH_TOPIC, &msg);
}



int main(void)
{
	struct max_uart_init_param uart_init_max = {
		.flow = UART_FLOW_DIS
	};
	struct no_os_uart_init_param uart_init = {
		.device_id = 0,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &uart_init_max,
	};
	struct no_os_uart_desc *uart;

	struct max_spi_init_param cn0414_spi_extra  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	struct no_os_spi_init_param cn0414_spi_init = {
		.device_id = 1,
		.max_speed_hz = 4000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &cn0414_spi_extra,
	};
	ad717x_init_param ad4111_init = {
		.spi_init = cn0414_spi_init,
		.regs = ad4111_regs,
		.num_regs = NO_OS_ARRAY_SIZE(ad4111_regs),
		.active_device = ID_AD4111,
		.ref_en = true,
		.num_channels = 1,
		.num_setups = 1,
		.chan_map = {
			{
				.channel_enable = true,
				.setup_sel = 0,
				.analog_inputs.analog_input_pairs = VIN6_VIN7
			},
		},
		.setups = {
			{
				.bi_unipolar = false,
				.ref_buff = false,
				.input_buff = true,
				.ref_source = INTERNAL_REF
			},
		},
		.filter_configuration = {
			{
				.odr = 0
			},
		},
		.mode =  CONTINUOUS,
	};
	ad717x_dev *ad4111;
	int ret;

	ret = no_os_init();
	if (ret < 0)
		return ret;

	ret = no_os_uart_init(&uart, &uart_init);
	if (ret < 0)
		return ret;
	
	no_os_uart_stdio(uart);

	printf("Hello\n\r");

	ret = ad717x_init(&ad4111, ad4111_init);
	if (ret < 0)
		return ret;



	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("ad4111", ad4111,
			       &ad717x_iio_descriptor, NULL, NULL, NULL),
	};



	uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};

	struct max_gpio_init_param adin1110_reset_gpio_extra_ip = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct max_spi_init_param adin1110_spi_extra_ip  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	const struct no_os_gpio_init_param adin1110_reset_ip = {
		.port = 0,
		.number = 15,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &adin1110_reset_gpio_extra_ip,
	};

	const struct no_os_spi_init_param adin1110_spi_ip = {
		.device_id = 3,
		.max_speed_hz = 25000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &adin1110_spi_extra_ip,
	};

	struct adin1110_init_param adin1110_init = {
		.chip_type = ADIN1110,
		.comm_param = adin1110_spi_ip,
		.reset_param = adin1110_reset_ip,
		.append_crc = false,
	};

	memcpy(adin1110_init.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
	app_init_param.lwip_param.mac_param = &adin1110_init;
	app_init_param.lwip_param.extra = NULL;
	memcpy(app_init_param.lwip_param.hwaddr, adin1110_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}

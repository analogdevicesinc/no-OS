#include "network.h"
#include "error.h"
#include "string.h"
#include "uart_extra.h"
#include "irq_extra.h"
#include "delay.h"
#include <stdio.h>

#define RESPONSE_BUFF_SIZE 1000

extern char wifi_ssid[];
extern char wifi_pass[];

void example_main()
{
	int32_t	ret;
	uint8_t	*msg;
	uint8_t	response[RESPONSE_BUFF_SIZE + 1];
	int32_t	len;
	struct aducm_uart_init_param aducm_param = {
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS
	};
	struct uart_init_param		uart_param = {0,
			       BD_115200,
			       &aducm_param
	};
	struct irq_init_param		irq_par = {0, 0};
	struct wifi_init_param		wifi_param;
	struct tcp_socket_init_param	sock_param;

	struct uart_desc		*udesc;
	struct irq_ctrl_desc		*idesc;
	struct wifi_desc		*wifi;
	struct network_interface	*net;
	struct tcp_socket_desc		*sock;
	struct socket_address		server;

	printf("Example started\n");
	ret = irq_ctrl_init(&idesc, &irq_par);
	if (IS_ERR_VALUE(ret))
		printf("Irq init failed\n");
	ret = uart_init(&udesc, &uart_param);
	if (IS_ERR_VALUE(ret))
		printf("Uart init failed\n");

	wifi_param.irq_desc = idesc;
	wifi_param.uart_desc = udesc;
	wifi_param.uart_irq_conf = udesc;
	wifi_param.uart_irq_id = ADUCM_UART_INT_ID;

	ret = wifi_init(&wifi, &wifi_param);
	if (IS_ERR_VALUE(ret))
		printf("Wifi init failed\n");

	ret = wifi_connect(wifi, wifi_ssid, wifi_pass);
	if (IS_ERR_VALUE(ret))
		printf("Connect failed\n");

	ret = wifi_get_network_interface(wifi, &net);
	if (IS_ERR_VALUE(ret))
		printf("Get network interface failed\n");

	sock_param.net = net;
	ret = socket_init(&sock, &sock_param);
	if (IS_ERR_VALUE(ret))
		printf("Socket init failed\n");

	server.addr = "192.168.1.5";
	server.port = 1883;
	ret = socket_connect(sock, &server);
	if (IS_ERR_VALUE(ret))
		printf("Socket connect failed\n");

	msg = "Hello server\n";
	ret = socket_send(sock, msg, strlen(msg));
	if (IS_ERR_VALUE(ret))
		printf("Socket send failed\n");

	printf("Waiting for buffer\n");
	mdelay(5000);
	len = socket_recv(sock, response, RESPONSE_BUFF_SIZE);
	if (IS_ERR_VALUE(len))
		printf("Socket recv failed\n");
	response[len] = 0;
	printf("Response len: %d -- Response: %s\n",len, response);

	printf("Cleaning up\n");
	ret = socket_remove(sock);
	if (IS_ERR_VALUE(ret))
		printf("Socket remove failed\n");
	ret = wifi_remove(wifi);
	if (IS_ERR_VALUE(ret))
		printf("Wifi remove failed\n");
	ret = uart_remove(udesc);
	if (IS_ERR_VALUE(ret))
		printf("Uart remove failed\n");
	ret = irq_ctrl_remove(idesc);
	if (IS_ERR_VALUE(ret))
		printf("Irq remove failed\n");
	printf("Example ended\n");
}

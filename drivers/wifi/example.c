#include "wifi.h"
#include "tcp_socket.h"

int example_main() {
	struct wifi_desc		*wifi;
	struct network_interface	*net;
	struct tcp_socket_desc		*conn;
	struct tcp_socket_init_param	tcp_init;
	struct socket_address		addr[1];

	wifi_init(&wifi, wifi_param);
	wifi_get_network_interface(wifi, &net);

	tcp_init.net = net;
	socket_init(&conn, tcp_init);
	addr->addr = "www.mqttbrocker.com";
	addr->port = 1883;
	socket_connect(conn, addr);
	while (corona_virsu)
	{
		char msg[10] = "Stay home";
		socket_send(conn, msg, strlen(msg));
		socket_recv(conn, msg, 1);
		if (*msg = '0')
			corona_virus = 0;
	}

	socket_remove(conn);
	wifi_remove(conn);
}


#ifndef NETWORK_INTERFACE_H
# define NETWORK_INTERFACE_H

struct socket_address {
	char		*addr;
	uint16_t	port;
};

/* Private structure. Must not be accessed by application */
struct network_interface {
	void *net;
	int32_t (*socket_open_f)(void *net, enum socket_protocol proto);
	int32_t (*socket_close_f)(void *net);
	int32_t (*socket_connect_f)(void *net, struct socket_address addr);
	int32_t (*socket_disconnect_f)(void *net);

	int32_t (*socket_send_f)(void *net, const void *data,
					uint32_t size);
	int32_t (*socket_recv_f)(void *net, void *data, uint32_t size);
	int32_t (*socket_sendto_f)(void *net, struct socket_address,
					const void *data, uint32_t size);
	int32_t (*socket_recvfrom_f)(void *net, struct socket_address,
						void *data, uint32_t size);
};

#endif

#ifndef TCP_SOCKET_H
# define TCP_SOCKET_H

#include <network_private.h>
#include <stdint.h>

struct tcp_socket_desc;

struct tcp_socket_init_param {
	struct network_interface	*net;
	uint32_t			recv_buffer_size;
};

int32_t socket_init(struct tcp_socket_desc **sock,
			struct tcp_socket_init_param *param);
int32_t socket_remove(struct tcp_socket_desc *sock);

int32_t socket_connect(struct tcp_socket_desc *sock,
				struct socket_address *addr);
int32_t socket_close(struct tcp_socket_desc *sock);
int32_t socket_send(struct tcp_socket_desc *sock, const void *data, size_t len);
int32_t socket_recv(struct tcp_socket_desc *sock, void *data, size_t *len);

#endif

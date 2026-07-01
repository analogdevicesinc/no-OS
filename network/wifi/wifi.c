/***************************************************************************//**
 *   @file   wifi.c
 *   @brief  Wifi implementation for ESP8266
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <string.h>
#include "wifi.h"
#include "at_parser.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

#define INVALID_ID	0xffffffff
#define NB_SOCKETS	(MAX_CONNECTIONS + 1)
#define NB_CLI_SOCKETS	MAX_CONNECTIONS

/* Structure storing data used by a socket */
struct socket_desc {
	/* Buffer given to at_parser */
	struct no_os_circular_buffer	*cb;
	/* Circular buffer size */
	uint32_t		cb_size;
	/* Socket type */
	enum no_os_socket_protocol	type;
	/* Connection id */
	uint32_t		conn_id;
	/* States of a socket structure */
	enum {
		/* The socket structure is unused */
		SOCKET_UNUSED,
		/* Socket is waiting for accept */
		SOCKET_WAITING_ACCEPT,
		/* Socket structure have been initialized */
		SOCKET_DISCONNECTED,
		/* Socket connected to a server or to a client */
		SOCKET_CONNECTED,
		/* State of server socket when it listen to new connections */
		SOCKET_LISTENING
	}			state;
};

struct server_desc {
	/* Server id */
	uint32_t	id;
	/* Server port */
	uint16_t	port;
	/* Ids of clientes socket that are available*/
	uint32_t	client_ids[NB_CLI_SOCKETS];
	/* Number of cliente socket available*/
	uint32_t	back_log_clients;
};

/* Wifi descriptor */
struct wifi_desc {
	/* Sockets. Last socket is server */
	struct socket_desc		sockets[NB_SOCKETS];
	/* Server descriptor */
	struct server_desc		server;
	/* Reference to the AT parser */
	struct at_desc			*at;
	/* Will be used in callback */
	int32_t				conn_id_to_sock_id[MAX_CONNECTIONS];
};

static int32_t wifi_socket_open(struct wifi_desc *desc, uint32_t *sock_id,
				enum no_os_socket_protocol proto, uint32_t buff_size);
static int32_t wifi_socket_close(struct wifi_desc *desc, uint32_t sock_id);
static int32_t wifi_socket_connect(struct wifi_desc *desc, uint32_t sock_id,
				   struct no_os_sockaddr *addr);
static int32_t wifi_socket_disconnect(struct wifi_desc *desc, uint32_t sock_id);
static int32_t wifi_socket_send(struct wifi_desc *desc, uint32_t sock_id,
				const void *data, uint32_t size);
static int32_t wifi_socket_recv(struct wifi_desc *desc, uint32_t sock_id,
				void *data, uint32_t size);
static int32_t wifi_socket_bind(struct wifi_desc *desc, uint32_t sock_id,
				uint16_t port);
static int32_t wifi_socket_listen(struct wifi_desc *desc, uint32_t sock_id,
				  uint32_t back_log);
static int32_t wifi_socket_accept(struct wifi_desc *desc, uint32_t sock_id,
				  uint32_t *client_socket_id);

/* Returns the index of a socket in SOCKET_UNUSED state */
static inline int32_t _wifi_get_unused_socket(struct wifi_desc *desc,
		uint32_t *idx)
{
	uint32_t i;

	for (i = 0; i < NB_SOCKETS; i++)
		if (desc->sockets[i].state == SOCKET_UNUSED) {
			desc->sockets[i].state = SOCKET_DISCONNECTED;
			*idx = i;

			return 0;
		}

	/* All the available connections are used */
	return -EMLINK;
}

/* Marks the socket at the index id as SOCKET_UNUSED */
static inline void _wifi_release_socket(struct wifi_desc *desc, uint32_t id)
{
	desc->sockets[id].state = SOCKET_UNUSED;
}

/* Set the socket connection ID with an ID that is not used yet */
static inline uint32_t _wifi_get_unused_conn(struct wifi_desc *desc,
		uint32_t sock_id)
{
	uint32_t i;

	for (i = 0; i < NB_CLI_SOCKETS; i++)
		if (desc->conn_id_to_sock_id[i] == INVALID_ID) {
			desc->conn_id_to_sock_id[i] = sock_id;
			desc->sockets[sock_id].conn_id = i;

			return 0;
		}

	return -EMLINK;
}

/* Unset the socket connection ID and mark it as available */
static inline void _wifi_release_conn(struct wifi_desc *desc,
				      uint32_t sock_id)
{
	uint32_t conn_id;

	conn_id = desc->sockets[sock_id].conn_id;
	if (conn_id != INVALID_ID) {
		/*
		 * Connection can be released both from callback or
		 * stop_connection command, and it shouldn't be released twice
		 */
		desc->sockets[sock_id].conn_id = INVALID_ID;
		desc->conn_id_to_sock_id[conn_id] = INVALID_ID;
	}
}

static inline int32_t _get_initialized_client_id(struct wifi_desc *desc)
{
	uint32_t i;
	uint32_t id;

	for (i = 0; i < desc->server.back_log_clients; i++) {
		id = desc->server.client_ids[i];
		if (desc->sockets[id].state == SOCKET_DISCONNECTED) {
			desc->sockets[id].state = SOCKET_WAITING_ACCEPT;

			return id;
		}
	}

	return INVALID_ID;
}

/* Callback to be submmited to the at_parser to get notification when a
connection is created or closed */
static void _wifi_connection_callback(void *ctx, enum at_event event,
				      uint32_t conn_id,
				      struct no_os_circular_buffer **cb)
{
	struct wifi_desc	*desc = ctx;
	struct socket_desc	*sock;
	int32_t			sock_id;

	sock_id = desc->conn_id_to_sock_id[conn_id];
	if (event == AT_NEW_CONNECTION) {
		if (sock_id != INVALID_ID) {
			*cb = desc->sockets[sock_id].cb;
		} else {
			sock_id = _get_initialized_client_id(desc);
			if (sock_id == INVALID_ID) {
				*cb = NULL;
			} else {
				sock = &desc->sockets[sock_id];
				/* Link new client conection to id */
				desc->conn_id_to_sock_id[conn_id] = sock_id;
				sock->conn_id = conn_id;
				sock->state = SOCKET_WAITING_ACCEPT;
				*cb = sock->cb;
			}

		}
	} else if (event == AT_CLOSED_CONNECTION) {
		if (sock_id != INVALID_ID) {
			desc->sockets[sock_id].state = SOCKET_DISCONNECTED;
			_wifi_release_conn(desc, sock_id);
		}
	}
}

/**
 * @brief Allocate resources and initializes a wifi descriptor
 * @param desc - Address where to store the wifi descriptor
 * @param param - Initializing data
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t wifi_init(struct wifi_desc **desc, struct wifi_init_param *param)
{
	struct wifi_desc	*ldesc;
	struct at_init_param	at_param;
	int32_t			result;
	union in_out_param	par;

	if (!desc || !param)
		return -1;

	ldesc = (struct wifi_desc *)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -1;

	memset(ldesc->conn_id_to_sock_id, (int8_t)INVALID_ID,
	       sizeof(ldesc->conn_id_to_sock_id));
	ldesc->server.id = INVALID_ID;

	at_param.irq_desc = param->irq_desc;
	at_param.uart_desc = param->uart_desc;
	at_param.uart_irq_conf = param->uart_irq_conf;
	at_param.uart_irq_id = param->uart_irq_id;
	at_param.connection_callback = _wifi_connection_callback;
	at_param.callback_ctx = ldesc;
	at_param.sw_reset_en = param->sw_reset_en;

	result = at_init(&ldesc->at, &at_param);
	if (NO_OS_IS_ERR_VALUE(result))
		goto ldesc_err;

	par.in.wifi_mode = CLIENT;
	result = at_run_cmd(ldesc->at, AT_SET_OPERATION_MODE, AT_SET_OP, &par);
	if (NO_OS_IS_ERR_VALUE(result))
		goto at_err;

	par.in.conn_type = MULTIPLE_CONNECTION;
	result = at_run_cmd(ldesc->at, AT_SET_CONNECTION_TYPE, AT_SET_OP, &par);
	if (NO_OS_IS_ERR_VALUE(result))
		goto at_err;
	*desc = ldesc;

	return 0;
at_err:
	at_remove(ldesc->at);
ldesc_err:
	no_os_free(ldesc);

	return -1;
}

/**
 * @brief Deallocate resources from the wifi descriptor
 * @param desc - Wifi descriptor
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t wifi_remove(struct wifi_desc *desc)
{
	uint32_t i;

	if (!desc)
		return -1;

	for (i = 0; i < NB_SOCKETS; i++)
		wifi_socket_close(desc, i);

	wifi_disconnect(desc);
	at_remove(desc->at);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Connect to a wifi network
 * @param desc - Socket descriptor
 * @param ssid - Wifi SSID
 * @param pass - Wifi password
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t wifi_connect(struct wifi_desc *desc, const char *ssid,
		     const char *pass)
{
	union in_out_param	param;

	if (!desc)
		return -1;

	str_to_at(&param.in.network.ssid, (uint8_t *)ssid);
	str_to_at(&param.in.network.pwd, (uint8_t *)pass);
	return at_run_cmd(desc->at, AT_CONNECT_NETWORK, AT_SET_OP, &param);
}

/**
 * @brief Disconnect from the wifi network
 * @param desc - Socket descriptor
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t wifi_disconnect(struct wifi_desc *desc)
{
	if (!desc)
		return -1;

	return at_run_cmd(desc->at, AT_DISCONNECT_NETWORK, AT_EXECUTE_OP, NULL);
}

/**
 * @brief Get ip
 * @param desc - Wifi descriptor
 * @param ip_buff - Buffer where to copy the null terminated ip string
 * @param buff_size - Size of the buffer
 * @return
 *  - 0 : On success
 *  - \ref -EINVAL : For invalid parameters
 *  - \ref -ENOMEM : If buffer is too small
 */
int32_t wifi_get_ip(struct wifi_desc *desc, char *ip_buff, uint32_t buff_size)
{
	int32_t			ret;
	union in_out_param	result;
	uint8_t			*buff;

	if (!desc || !ip_buff)
		return -EINVAL;

	ret = at_run_cmd(desc->at, AT_GET_IP, AT_EXECUTE_OP, &result);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	at_to_str(&buff, &result.out.result);
	if (strlen((char *)buff) + 1 > buff_size)
		return (-ENOMEM);

	strcpy(ip_buff, (char *)buff);

	return 0;
}

/** @brief See \ref no_os_socket_ops.socket_open */
static int32_t wifi_socket_open(struct wifi_desc *desc, uint32_t *sock_id,
				enum no_os_socket_protocol proto, uint32_t buff_size)
{
	uint32_t	id;
	int32_t		ret;

	if (!desc || !sock_id)
		return -1;

	ret = _wifi_get_unused_socket(desc, &id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = no_os_cb_init(&desc->sockets[id].cb, buff_size);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		_wifi_release_socket(desc, id);
		return ret;
	}

	desc->sockets[id].type = proto;
	desc->sockets[id].cb_size = buff_size;

	*sock_id = id;

	return 0;
}

static inline bool _is_server_socket(struct wifi_desc *desc, uint32_t sock_id)
{
	int32_t i;

	for (i = 0; i < desc->server.back_log_clients; i++)
		if (desc->server.client_ids[i] == sock_id)
			return true;

	return false;
}

/** @brief See \ref no_os_socket_ops.socket_close */
static int32_t wifi_socket_close(struct wifi_desc *desc, uint32_t sock_id)
{
	struct socket_desc	*sock;
	int32_t			ret;
	uint32_t		i;

	if (!desc || sock_id >= NB_SOCKETS)
		return -1;

	sock = &desc->sockets[sock_id];
	if (sock->state == SOCKET_UNUSED)
		return 0;

	ret = wifi_socket_disconnect(desc, sock_id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	/* Server socket circular buffer will be released only when server
	 * is removed */
	if (!_is_server_socket(desc, sock_id)) {
		no_os_cb_remove(sock->cb);
		sock->cb = NULL;
	} else if (sock_id == desc->server.id) {
		for (i = 0; i < desc->server.back_log_clients; i++) {
			sock = &desc->sockets[desc->server.client_ids[i]];
			no_os_cb_remove(sock->cb);
			_wifi_release_socket(desc, desc->server.client_ids[i]);
		}
	}
	_wifi_release_socket(desc, sock_id);

	return 0;
}

/** @brief See \ref no_os_socket_ops.socket_connect */
static int32_t wifi_socket_connect(struct wifi_desc *desc, uint32_t sock_id,
				   struct no_os_sockaddr *addr)
{
	union in_out_param	param;
	int32_t			ret;
	struct socket_desc	*sock;

	if (!desc || !addr || sock_id >= NB_SOCKETS ||
	    sock_id == desc->server.id)
		return -1;

	sock = &desc->sockets[sock_id];
	if (sock->state == SOCKET_UNUSED)
		return -ENOENT;

	if (sock->state == SOCKET_CONNECTED)
		return -EISCONN;

	ret = _wifi_get_unused_conn(desc, sock_id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	str_to_at(&param.in.connection.addr, (uint8_t *)addr->addr);
	param.in.connection.port = addr->port;
	param.in.connection.id = sock->conn_id;
	param.in.connection.soket_type = sock->type;

	ret = at_run_cmd(desc->at, AT_START_CONNECTION, AT_SET_OP, &param);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		_wifi_release_conn(desc, sock_id);
		return ret;
	}

	sock->state = SOCKET_CONNECTED;

	return 0;
}

static void _remove_server_back_log(struct wifi_desc *desc)
{
	uint32_t i;
	uint32_t id;

	for (i = 0; i < desc->server.back_log_clients; i++) {
		id = desc->server.client_ids[i];
		no_os_cb_remove(desc->sockets[id].cb);
		wifi_socket_close(desc, id);
	}
}

/** @brief See \ref no_os_socket_ops.socket_disconnect */
static int32_t wifi_socket_disconnect(struct wifi_desc *desc, uint32_t sock_id)
{
	union in_out_param	param;
	int32_t			ret;
	struct socket_desc	*sock;

	if (!desc || sock_id >= NB_SOCKETS)
		return -1;

	sock = &desc->sockets[sock_id];
	if (sock->state == SOCKET_UNUSED)
		return -ENOENT;

	if (sock->state == SOCKET_DISCONNECTED)
		/* A socket can be disconnected by the peer */
		return 0;

	if (sock_id == desc->server.id) {
		/* Disable esp8266 server mode */
		param.in.server.action = DELETE_SERVER;
		ret = at_run_cmd(desc->at, AT_SET_SERVER, AT_SET_OP, &param);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
		_remove_server_back_log(desc);

		/* Remove server reference */
		desc->server.id = INVALID_ID;
	} else {
		param.in.conn_id = sock->conn_id;
		ret = at_run_cmd(desc->at, AT_STOP_CONNECTION, AT_SET_OP,
				 &param);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
		_wifi_release_conn(desc, sock_id);
	}

	sock->state = SOCKET_DISCONNECTED;

	return 0;
}

/** @brief See \ref no_os_socket_ops.socket_send */
static int32_t wifi_socket_send(struct wifi_desc *desc, uint32_t sock_id,
				const void *data, uint32_t size)
{
	union in_out_param	param;
	int32_t			ret;
	struct socket_desc	*sock;
	uint32_t		to_send;
	uint32_t		i;

	if (!desc || sock_id >= NB_SOCKETS || desc->server.id == sock_id)
		return -EINVAL;

	sock = &desc->sockets[sock_id];
	if (sock->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	i = 0;
	do {
		to_send = no_os_min(size - i, MAX_CIPSEND_DATA);
		param.in.send_data.id = sock->conn_id;
		param.in.send_data.data.buff = ((uint8_t *)data) + i;
		param.in.send_data.data.len = to_send;
		ret = at_run_cmd(desc->at, AT_SEND, AT_SET_OP, &param);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		i += to_send;
	} while (i < size);

	return (int32_t)size;
}

/** @brief See \ref no_os_socket_ops.socket_recv */
static int32_t wifi_socket_recv(struct wifi_desc *desc, uint32_t sock_id,
				void *data, uint32_t size)
{
	struct socket_desc	*sock;
	uint32_t		available_size;
	int32_t			ret;

	if (!desc || sock_id >= NB_SOCKETS || !size ||
	    desc->server.id == sock_id)
		return -EINVAL;

	/* TODO read data even if disconnected ? */
	sock = &desc->sockets[sock_id];
	if (sock->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	no_os_cb_size(sock->cb, &available_size);
	if (available_size == 0)
		return -EAGAIN;

	size = no_os_min(available_size, size);
	ret = no_os_cb_read(sock->cb, data, size);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return size;
}

/** @brief See \ref no_os_socket_ops.socket_bind */
static int32_t wifi_socket_bind(struct wifi_desc *desc, uint32_t sock_id,
				uint16_t port)
{
	if (!desc || sock_id >= NB_SOCKETS)
		return -EINVAL;

	if (desc->server.id != INVALID_ID)
		return -EMLINK;

	if (desc->sockets[sock_id].state == SOCKET_UNUSED)
		return -ENODEV;

	/* Configure current socket as server */
	desc->server.id = sock_id;
	desc->server.port = port;

	return 0;
}

/** @brief See \ref no_os_socket_ops.socket_listen */
static int32_t wifi_socket_listen(struct wifi_desc *desc, uint32_t sock_id,
				  uint32_t back_log)
{
	struct socket_desc	*server_sock;
	struct socket_desc	*cli_sock;
	union in_out_param	param;
	int32_t			ret;
	uint32_t		*i;
	uint32_t		id;

	if (!desc)
		return -EINVAL;

	/* Socket bind must be done befor listen */
	if (desc->server.id == INVALID_ID || sock_id != desc->server.id)
		return -ENOTCONN;

	server_sock = &desc->sockets[desc->server.id];
	/* Initialize sockets in order to don't allocate memory in interrupts */
	i = &desc->server.back_log_clients;
	if (back_log == 0)
		back_log = MAX_CONNECTIONS;
	else
		back_log = no_os_min(back_log, MAX_CONNECTIONS);

	for (*i = 0; *i < back_log; (*i)++) {
		ret = wifi_socket_open(desc, &id, server_sock->type,
				       server_sock->cb_size);
		if (NO_OS_IS_ERR_VALUE(ret))
			break;

		cli_sock = &desc->sockets[id];
		if (*i == 0 && server_sock->cb) {
			cli_sock->cb = server_sock->cb;
			server_sock->cb = NULL;
		} else {
			ret = no_os_cb_init(&cli_sock->cb, server_sock->cb_size);
			if (NO_OS_IS_ERR_VALUE(ret)) {
				wifi_socket_close(desc, id);
				break;
			}
		}
		cli_sock->state = SOCKET_DISCONNECTED;
		desc->server.client_ids[*i] = id;
	}
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_resources;

	param.in.server.action = CREATE_SERVER;
	param.in.server.port = desc->server.port;

	ret = at_run_cmd(desc->at, AT_SET_SERVER, AT_SET_OP, &param);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_resources;

	desc->sockets[sock_id].state = SOCKET_LISTENING;

	return 0;

free_resources:
	_remove_server_back_log(desc);

	return ret;
}

/** @brief See \ref no_os_socket_ops.socket_accept */
static int32_t wifi_socket_accept(struct wifi_desc *desc, uint32_t sock_id,
				  uint32_t *client_socket_id)
{
	uint32_t		i;

	if (!desc || !client_socket_id || desc->server.id != sock_id)
		return -EINVAL;

	/* Bind should be called first */
	if (desc->sockets[desc->server.id].state != SOCKET_LISTENING)
		return -ENOTCONN;

	for (i = 0; i < NB_SOCKETS; i++)
		if (desc->sockets[i].state == SOCKET_WAITING_ACCEPT) {
			desc->sockets[i].state = SOCKET_CONNECTED;
			*client_socket_id = i;

			return 0;
		}

	return -EAGAIN;
}

/* ---- no_os_net / no_os_socket backend --------------------------------- */
/*
 * The wifi (ESP8266) backend keeps its socket pool inside wifi_desc, indexed by
 * a small integer id. In the new API, no_os_net_desc.extra holds the wifi_desc,
 * and each no_os_socket_desc wraps a pool id in its "id" field. RX is driven by
 * the UART IRQ into per-socket circular buffers, so step() is a no-op.
 */

#include "no_os_socket.h"

static const struct no_os_socket_ops wifi_socket_ops;

static int32_t wifi_net_init(struct no_os_net_desc **desc,
			     const struct no_os_net_init_param *param)
{
	struct wifi_net_param *wp;
	struct no_os_net_desc *nd;
	struct wifi_desc *wifi;
	int32_t ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	wp = param->extra;

	nd = no_os_calloc(1, sizeof(*nd));
	if (!nd)
		return -ENOMEM;

	ret = wifi_init(&wifi, &wp->wifi_ip);
	if (ret) {
		no_os_free(nd);
		return ret;
	}

	if (wp->ssid) {
		ret = wifi_connect(wifi, wp->ssid, wp->pwd);
		if (ret) {
			wifi_remove(wifi);
			no_os_free(nd);
			return ret;
		}
	}

	nd->extra = wifi;
	*desc = nd;

	return 0;
}

static int32_t wifi_net_remove(struct no_os_net_desc *desc)
{
	struct wifi_desc *wifi = desc->extra;
	int32_t ret;

	ret = wifi_remove(wifi);
	no_os_free(desc);

	return ret;
}

static int32_t wifi_net_step(struct no_os_net_desc *desc)
{
	/* ESP8266 RX is serviced by the UART interrupt; nothing to pump here. */
	return 0;
}

static int32_t wifi_net_get_ip(struct no_os_net_desc *desc, char *buf,
			       uint32_t size)
{
	return wifi_get_ip(desc->extra, buf, size);
}

static int32_t wifi_net_socket_open(struct no_os_net_desc *net,
				    struct no_os_socket_desc **sock,
				    enum no_os_socket_protocol proto,
				    uint32_t buff_size)
{
	struct wifi_desc *wifi = net->extra;
	struct no_os_socket_desc *s;
	uint32_t id;
	int32_t ret;

	ret = wifi_socket_open(wifi, &id,
			       (proto == NO_OS_SOCKET_UDP) ? NO_OS_SOCKET_UDP :
			       NO_OS_SOCKET_TCP, buff_size);
	if (ret)
		return ret;

	s = no_os_socket_alloc(net, &wifi_socket_ops, id);
	if (!s) {
		wifi_socket_close(wifi, id);
		return -ENOMEM;
	}

	*sock = s;

	return 0;
}

static int32_t wifi_sock_close(struct no_os_socket_desc *sock)
{
	int32_t ret;

	ret = wifi_socket_close(sock->net->extra, sock->id);
	no_os_free(sock);

	return ret;
}

static int32_t wifi_sock_connect(struct no_os_socket_desc *sock,
				 struct no_os_sockaddr *addr)
{
	return wifi_socket_connect(sock->net->extra, sock->id, addr);
}

static int32_t wifi_sock_disconnect(struct no_os_socket_desc *sock)
{
	return wifi_socket_disconnect(sock->net->extra, sock->id);
}

static int32_t wifi_sock_send(struct no_os_socket_desc *sock, const void *data,
			      uint32_t size)
{
	return wifi_socket_send(sock->net->extra, sock->id, data, size);
}

static int32_t wifi_sock_recv(struct no_os_socket_desc *sock, void *data,
			      uint32_t size)
{
	return wifi_socket_recv(sock->net->extra, sock->id, data, size);
}

static int32_t wifi_sock_sendto(struct no_os_socket_desc *sock,
				const void *data, uint32_t size,
				const struct no_os_sockaddr *to)
{
	return -ENOSYS;
}

static int32_t wifi_sock_recvfrom(struct no_os_socket_desc *sock, void *data,
				  uint32_t size, struct no_os_sockaddr *from)
{
	return -ENOSYS;
}

static int32_t wifi_sock_bind(struct no_os_socket_desc *sock, uint16_t port)
{
	return wifi_socket_bind(sock->net->extra, sock->id, port);
}

static int32_t wifi_sock_listen(struct no_os_socket_desc *sock,
				uint32_t backlog)
{
	return wifi_socket_listen(sock->net->extra, sock->id, backlog);
}

static int32_t wifi_sock_accept(struct no_os_socket_desc *sock,
				struct no_os_socket_desc **client)
{
	struct no_os_socket_desc *c;
	uint32_t id;
	int32_t ret;

	ret = wifi_socket_accept(sock->net->extra, sock->id, &id);
	if (ret)
		return ret;

	c = no_os_socket_alloc(sock->net, &wifi_socket_ops, id);
	if (!c) {
		wifi_socket_close(sock->net->extra, id);
		return -ENOMEM;
	}

	*client = c;

	return 0;
}

const struct no_os_net_platform_ops wifi_net_platform_ops = {
	.init = wifi_net_init,
	.remove = wifi_net_remove,
	.step = wifi_net_step,
	.get_ip = wifi_net_get_ip,
	.socket_open = wifi_net_socket_open,
};

static const struct no_os_socket_ops wifi_socket_ops = {
	.close = wifi_sock_close,
	.connect = wifi_sock_connect,
	.disconnect = wifi_sock_disconnect,
	.send = wifi_sock_send,
	.recv = wifi_sock_recv,
	.sendto = wifi_sock_sendto,
	.recvfrom = wifi_sock_recvfrom,
	.bind = wifi_sock_bind,
	.listen = wifi_sock_listen,
	.accept = wifi_sock_accept,
};

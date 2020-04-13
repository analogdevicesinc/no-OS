/***************************************************************************//**
 *   @file   wifi.c
 *   @brief  Wifi implementation for ESP8266
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include "wifi.h"
#include "at_parser.h"
#include "error.h"
#include "util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define CONNECTION_BUFFER_SIZE 1500

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Structure storing data used by a socket */
struct socket_desc {
	/* Buffer submitted to the at parser */
	uint8_t			*buff;
	/* Socket type */
	enum socket_protocol	type;
	/* True if a socket connect was called on this id */
	bool			connected;
};

/* Wifi descriptor */
struct wifi_desc {
	/* Sockets */
	struct socket_desc		sockets[MAX_CONNECTIONS];
	/* Reference to the AT parser */
	struct at_desc			*at;
	/* Network interface */
	struct network_interface	interface;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/


static int32_t wifi_socket_open(struct wifi_desc *desc, uint32_t *sock_id,
				enum socket_protocol proto);
static int32_t wifi_socket_close(struct wifi_desc *desc, uint32_t sock_id);
static int32_t wifi_socket_connect(struct wifi_desc *desc, uint32_t sock_id,
				   struct socket_address *addr);
static int32_t wifi_socket_disconnect(struct wifi_desc *desc, uint32_t sock_id);
static int32_t wifi_socket_send(struct wifi_desc *desc, uint32_t sock_id,
				const void *data, uint32_t size);
static int32_t wifi_socket_recv(struct wifi_desc *desc, uint32_t sock_id,
				void *data, uint32_t size);
static int32_t wifi_socket_sendto(struct wifi_desc *desc, uint32_t sock_id,
				  const void *data, uint32_t size,
				  struct socket_address to);
static int32_t wifi_socket_recvfrom(struct wifi_desc *desc, uint32_t sock_id,
				    void *data, uint32_t size,
				    struct socket_address *from);

/* Connect internal functions to network interface */
static void wifi_init_intarface(struct wifi_desc *desc)
{

	desc->interface.net = desc;
	desc->interface.socket_open =
		(int32_t (*)(void *, uint32_t *, enum socket_protocol))
		wifi_socket_open;
	desc->interface.socket_close =
		(int32_t (*)(void *, uint32_t))
		wifi_socket_close;
	desc->interface.socket_connect =
		(int32_t (*)(void *, uint32_t, struct socket_address *))
		wifi_socket_connect;
	desc->interface.socket_disconnect =
		(int32_t (*)(void *, uint32_t))
		wifi_socket_disconnect;
	desc->interface.socket_send =
		(int32_t (*)(void *, uint32_t, const void *, uint32_t))
		wifi_socket_send;
	desc->interface.socket_recv =
		(int32_t (*)(void *, uint32_t, void *, uint32_t))
		wifi_socket_recv;
	desc->interface.socket_sendto =
		(int32_t (*)(void *, uint32_t, const void *, uint32_t,
			     const struct socket_address *))
		wifi_socket_sendto;
	desc->interface.socket_recvfrom =
		(int32_t (*)(void *, uint32_t, void *, uint32_t,
			     struct socket_address *))
		wifi_socket_recvfrom;
}

/**
 * @brief Allocate resources and initializes a wifi descriptor
 * @param desc - Address where to store the wifi descriptor
 * @param param - Initializing data
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t wifi_init(struct wifi_desc **desc, struct wifi_init_param *param)
{
	struct wifi_desc	*ldesc;
	struct at_init_param	at_param;
	int32_t			result;
	union in_out_param	par;

	if (!desc || !param)
		return FAILURE;

	ldesc = (struct wifi_desc *)calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	at_param.irq_desc = param->irq_desc;
	at_param.uart_desc = param->uart_desc;
	at_param.uart_irq_conf = param->uart_irq_conf;
	at_param.uart_irq_id = param->uart_irq_id;

	result = at_init(&ldesc->at, &at_param);
	if (IS_ERR_VALUE(result))
		goto ldesc_err;

	wifi_init_intarface(ldesc);

	result = at_run_cmd(ldesc->at, AT_RESET, AT_EXECUTE_OP, NULL);
	if (IS_ERR_VALUE(result))
		goto ldesc_err;

	par.in.conn_type = MULTIPLE_CONNECTION;
	result = at_run_cmd(ldesc->at, AT_SET_CONNECTION_TYPE, AT_SET_OP, &par);
	if (IS_ERR_VALUE(result))
		goto ldesc_err;
	*desc = ldesc;

	return SUCCESS;
ldesc_err:
	free(ldesc);

	return FAILURE;
}

/**
 * @brief Deallocate resources from the wifi descriptor
 * @param desc - Wifi descriptor
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t wifi_remove(struct wifi_desc *desc)
{
	uint32_t i;

	if (!desc)
		return FAILURE;

	for (i = 0; i < MAX_CONNECTIONS; i++)
		wifi_socket_close(desc, i);

	wifi_disconnect(desc);

	at_remove(desc->at);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Connect to a wifi network
 * @param desc - Socket descriptor
 * @param ssid - Wifi SSID
 * @param pass - Wifi password
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t wifi_connect(struct wifi_desc *desc, const char *ssid,
		     const char *pass)
{
	int32_t			result;
	union in_out_param	param;

	if (!desc)
		return FAILURE;

	result = wifi_disconnect(desc);
	if (IS_ERR_VALUE(result))
		return FAILURE;

	str_to_at(&param.in.network.ssid, (uint8_t *)ssid);
	str_to_at(&param.in.network.pwd, (uint8_t *)pass);
	return at_run_cmd(desc->at, AT_CONNECT_NETWORK, AT_SET_OP, &param);
}

/**
 * @brief Disconnect from the wifi network
 * @param desc - Socket descriptor
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t wifi_disconnect(struct wifi_desc *desc)
{
	if (!desc)
		return FAILURE;

	return at_run_cmd(desc->at, AT_DISCONNECT_NETWORK, AT_EXECUTE_OP, NULL);
}

/**
 * @brief Get network interface reference
 * @param desc - Socket descriptor
 * @param net - Address where to store the reference to the network interface
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t wifi_get_network_interface(struct wifi_desc *desc,
				   struct network_interface **net)
{
	if (!desc || !net)
		return FAILURE;

	*net = &desc->interface;

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_open */
static int32_t wifi_socket_open(struct wifi_desc *desc, uint32_t *sock_id,
				enum socket_protocol proto)
{
	uint32_t		i;
	struct socket_desc	*sock;

	if (!desc || !sock_id)
		return FAILURE;

	sock = NULL;
	for (i = 0; i < MAX_CONNECTIONS; i++)
		if (!desc->sockets[i].buff) {
			sock = &desc->sockets[i];
			break;
		}
	if (!sock)
		return FAILURE; //All the available connections are used

	sock->type = proto;
	sock->buff = (uint8_t *)malloc(CONNECTION_BUFFER_SIZE);
	if (!sock->buff)
		return FAILURE;

	*sock_id = i;
	at_submit_buffer(desc->at, i, sock->buff, CONNECTION_BUFFER_SIZE);

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_close */
static int32_t wifi_socket_close(struct wifi_desc *desc, uint32_t sock_id)
{
	int32_t ret;

	if (!desc || sock_id >= MAX_CONNECTIONS)
		return FAILURE;

	if (!desc->sockets[sock_id].buff)
		return SUCCESS;

	ret = wifi_socket_disconnect(desc, sock_id);
	if (IS_ERR_VALUE(ret))
		return ret;

	free(desc->sockets[sock_id].buff);
	desc->sockets[sock_id].buff = NULL;

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_connect */
static int32_t wifi_socket_connect(struct wifi_desc *desc, uint32_t sock_id,
				   struct socket_address *addr)
{
	union in_out_param	param;
	uint32_t		ret;

	if (!desc || !addr || sock_id >= MAX_CONNECTIONS)
		return FAILURE;

	if (!desc->sockets[sock_id].buff)
		return FAILURE;

	str_to_at(&param.in.connection.addr, (uint8_t *)addr->addr);
	param.in.connection.port = addr->port;
	param.in.connection.id = sock_id;
	param.in.connection.soket_type = desc->sockets[sock_id].type;

	ret = at_run_cmd(desc->at, AT_START_CONNECTION, AT_SET_OP, &param);
	if (IS_ERR_VALUE(ret))
		return ret;

	desc->sockets[sock_id].connected = true;

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_disconnect */
static int32_t wifi_socket_disconnect(struct wifi_desc *desc, uint32_t sock_id)
{
	union in_out_param	param;
	uint32_t		ret;

	if (!desc || sock_id >= MAX_CONNECTIONS)
		return FAILURE;

	if (!desc->sockets[sock_id].buff)
		return FAILURE;

	param.in.conn_id = sock_id;
	ret = at_run_cmd(desc->at, AT_STOP_CONNECTION, AT_SET_OP, &param);
	if (IS_ERR_VALUE(ret))
		return ret;

	desc->sockets[sock_id].connected = false;

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_send */
static int32_t wifi_socket_send(struct wifi_desc *desc, uint32_t sock_id,
				const void *data, uint32_t size)
{
	union in_out_param	param;
	uint32_t		ret;

	if (!desc || sock_id >= MAX_CONNECTIONS)
		return FAILURE;

	if (!desc->sockets[sock_id].buff)
		return FAILURE;

	param.in.send_data.id = sock_id;
	param.in.send_data.data.buff = (uint8_t *)data;
	param.in.send_data.data.len = size;
	ret = at_run_cmd(desc->at, AT_SEND, AT_SET_OP, &param);
	if (IS_ERR_VALUE(ret))
		return ret;

	desc->sockets[sock_id].connected = true;

	return (int32_t)size;
}

/** @brief See \ref network_interface.socket_recv */
static int32_t wifi_socket_recv(struct wifi_desc *desc, uint32_t sock_id,
				void *data, uint32_t size)
{
	uint32_t	read;
	bool		is_overflow;

	if (!desc || sock_id >= MAX_CONNECTIONS || !size)
		return FAILURE;

	if (!desc->sockets[sock_id].buff)
		return FAILURE;

	at_read_buffer(desc->at, sock_id, data, size, &read, &is_overflow);
	if (is_overflow)
		return FAILURE;

	return (int32_t)read;
}

/** @brief See \ref network_interface.socket_sendto */
static int32_t wifi_socket_sendto(struct wifi_desc *desc, uint32_t sock_id,
				  const void *data, uint32_t size,
				  const struct socket_address to)
{
	UNUSED_PARAM(desc);
	UNUSED_PARAM(sock_id);
	UNUSED_PARAM(data);
	UNUSED_PARAM(size);
	UNUSED_PARAM(to);

	/* TODO: Implement for UDP */
	return FAILURE;
}

/** @brief See \ref network_interface.socket_recvfrom */
static int32_t wifi_socket_recvfrom(struct wifi_desc *desc, uint32_t sock_id,
				    void *data, uint32_t size,
				    struct socket_address *from)
{
	UNUSED_PARAM(desc);
	UNUSED_PARAM(sock_id);
	UNUSED_PARAM(data);
	UNUSED_PARAM(size);
	UNUSED_PARAM(from);

	/* TODO: Implement for UDP */
	return FAILURE;
}

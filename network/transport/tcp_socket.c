/***************************************************************************//**
 *   @file   tcp_socket.c
 *   @brief  TCP Socket functions
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
#include "error.h"
#include "util.h"
#include "socket.h"
#include "socket_interface.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct tcp_socket_desc
 * @brief TCP Socket descriptor
 */
struct tcp_socket_desc {
	/** Id of the opened socket to be passed to the network interface */
	uint32_t			id;
	/** Reference to the network interface */
	struct network_interface	*net;
};

/******************************************************************************/
/**************************** Function prototypes *****************************/
/******************************************************************************/

static int32_t tcp_socket_init(struct tcp_socket_desc **desc,
			       struct network_interface *net,
			       struct tcp_socket_init_param *param);
static int32_t tcp_socket_remove(struct tcp_socket_desc *desc);
static int32_t tcp_socket_connect(struct tcp_socket_desc *desc,
				  struct socket_address *addr);
static int32_t tcp_socket_disconnect(struct tcp_socket_desc *desc);
static int32_t tcp_socket_send(struct tcp_socket_desc *desc, const void *data,
			       uint32_t len);
static int32_t tcp_socket_recv(struct tcp_socket_desc *desc, void *data,
			       uint32_t len);

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Interface to used by generic socket.c */
const struct socket_interface tcp_socket_interface = {
	.protocol = PROTOCOL_TCP,
	.init = (int32_t (*)())tcp_socket_init,
	.remove = (int32_t (*)())tcp_socket_remove,
	.connect = (int32_t (*)())tcp_socket_connect,
	.disconnect = (int32_t (*)())tcp_socket_disconnect,
	.send = (int32_t (*)())tcp_socket_send,
	.recv = (int32_t (*)())tcp_socket_recv,
	.sendto = NULL,
	.recvfrom = NULL
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Allocate resources and initializes the socket descriptor
 * @param desc - Address where to store the socket descriptor
 * @param param - Initializing data
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
static int32_t tcp_socket_init(struct tcp_socket_desc **desc,
			       struct network_interface *net,
			       struct tcp_socket_init_param *param)
{
	struct tcp_socket_desc	*ldesc;
	int32_t			ret;

	UNUSED_PARAM(param);

	if (!desc)
		return FAILURE;

	ldesc = (typeof(ldesc))calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	ldesc->net = net;

	ret = ldesc->net->socket_open(ldesc->net->net, &ldesc->id,
				      PROTOCOL_TCP);
	if (IS_ERR_VALUE(ret)) {
		free(ldesc);
		return ret;
	}
	*desc = ldesc;

	return SUCCESS;
}

/**
 * @brief Deallocate resources from the socket descriptor
 * @param desc - Socket descriptor
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
static int32_t tcp_socket_remove(struct tcp_socket_desc *desc)
{
	int32_t ret;

	if (!desc)
		return FAILURE;

	ret = desc->net->socket_close(desc->net->net, desc->id);
	if (IS_ERR_VALUE(ret))
		return ret;
	free(desc);

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_connect */
static int32_t tcp_socket_connect(struct tcp_socket_desc *desc,
				  struct socket_address *addr)
{
	if (!desc || !addr)
		return FAILURE;

	return desc->net->socket_connect(desc->net->net, desc->id, addr);
}

/** @brief See \ref network_interface.socket_disconnect */
static int32_t tcp_socket_disconnect(struct tcp_socket_desc *desc)
{
	if (!desc)
		return FAILURE;

	return desc->net->socket_disconnect(desc->net->net, desc->id);
}

/** @brief See \ref network_interface.socket_send */
static int32_t tcp_socket_send(struct tcp_socket_desc *desc, const void *data,
			       uint32_t len)
{

	if (!desc)
		return FAILURE;

	return desc->net->socket_send(desc->net->net, desc->id, data, len);
}

/** @brief See \ref network_interface.socket_recv */
static int32_t tcp_socket_recv(struct tcp_socket_desc *desc, void *data,
			       uint32_t len)
{
	if (!desc)
		return FAILURE;

	return desc->net->socket_recv(desc->net->net, desc->id, data, len);
}

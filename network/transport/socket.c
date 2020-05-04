/***************************************************************************//**
 *   @file   socket.c
 *   @brief  Generic socket interface implementation
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
#include "socket.h"
#include "socket_interface.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

extern const struct socket_interface tcp_socket_interface;
extern const struct socket_interface stcp_socket_interface;

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Socket descriptor */
struct socket_desc {
	/* Interface reference */
	struct socket_interface	*interface;
	/* Reference to specific socket */
	void			*sock;
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
int32_t socket_init(struct socket_desc **desc, struct socket_init_param *param)
{
	struct socket_desc	*ldesc;
	void			*init_param;
	int32_t			ret;

	if (!desc || !param)
		return FAILURE;

	ldesc = (struct socket_desc *)calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	switch (param->type) {
	case TCP_SOCKET:
		ldesc->interface = &tcp_socket_interface;
		init_param = (void *)param->init_param.tcp;
		break;
	case SECURE_TCP_SOCKET:
		ldesc->interface = &stcp_socket_interface;
		init_param = (void *)param->init_param.secure_tcp;
		break;
	default:
		ret = FAILURE;
		goto exit;
	}

	ret = ldesc->interface->init(&ldesc->sock, param->net, init_param);
	if (IS_ERR_VALUE(ret))
		goto exit;

	*desc = ldesc;

	return SUCCESS;
exit:
	free(ldesc);
	return ret;
}

/**
 * @brief Deallocate resources from the socket descriptor
 * @param desc - Socket descriptor
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t socket_remove(struct socket_desc *desc)
{
	if (!desc || !desc->interface->remove)
		return FAILURE;

	return desc->interface->remove(desc->sock);
}

/** @brief See \ref network_interface.socket_connect */
int32_t socket_connect(struct socket_desc *desc, struct socket_address *addr)
{
	if (!desc || !desc->interface->connect)
		return FAILURE;

	return desc->interface->connect(desc->sock, addr);

}

/** @brief See \ref network_interface.socket_disconnect */
int32_t socket_disconnect(struct socket_desc *desc)
{
	if (!desc || !desc->interface->disconnect)
		return FAILURE;

	return desc->interface->disconnect(desc->sock);
}

/** @brief See \ref network_interface.socket_send */
int32_t socket_send(struct socket_desc *desc, const void *data, uint32_t len)
{
	if (!desc || !desc->interface->send)
		return FAILURE;

	return desc->interface->send(desc->sock, data, len);
}

/** @brief See \ref network_interface.socket_recv */
int32_t socket_recv(struct socket_desc *desc, void *data, uint32_t len)
{
	if (!desc || !desc->interface->recv)
		return FAILURE;

	return desc->interface->recv(desc->sock, data, len);
}

/* UDP Socket sendto */
int32_t socket_sendto(struct socket_desc *desc, const void *data, uint32_t size,
		      const struct socket_address *to)
{
	if (!desc || !desc->interface->sendto)
		return FAILURE;

	return desc->interface->sendto(desc->sock, data, size, to);
}
/* UDP Socket recvfrom */
int32_t socket_recvfrom(struct socket_desc *desc, void *data, uint32_t size,
			struct socket_address *from)
{
	if (!desc || !desc->interface->recvfrom)
		return FAILURE;

	return desc->interface->recvfrom(desc->sock, data, size, from);
}

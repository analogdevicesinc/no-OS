/***************************************************************************//**
 *   @file   socket.h
 *   @brief  Socket interface
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

#ifndef SOCKET_H
# define SOCKET_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "network_interface.h"

/** Returned by a secure tcp socket when a transaction is close */
#define TLS_RET_CLOSE_NOTIFY	-0x7880

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct socket_desc
 * @brief Socket descriptor
 */
struct socket_desc;

/**
 * @struct tcp_socket_init_param
 * @brief Parameter to initialize a TCP Socket
 */
struct tcp_socket_init_param {
	//No parameters
};

/**
 * @struct stcp_socket_init_param
 * @brief Parameter to initialize a TCP Socket
 */
struct stcp_socket_init_param {
	char				*ca_cert;
	char				*hostname;
	char				*client_cert;
	char				*client_key;
};

enum socket_type {
	TCP_SOCKET,
	SECURE_TCP_SOCKET
};

/**
 * @struct socket_init_param
 * @brief Parameter to initialize a Socket
 */
struct socket_init_param {
	/** Network interface. Ex: Wifi, Ethernet */
	struct network_interface	*net;
	/** Reference to one of the values */
	enum socket_type		type;
	/** Socket init param */
	union {
		struct tcp_socket_init_param	*tcp;
		struct stcp_socket_init_param	*secure_tcp;
	}				init_param;

};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init socket */
int32_t socket_init(struct socket_desc **desc, struct socket_init_param *param);

/* Socket remove */
int32_t socket_remove(struct socket_desc *desc);

/* Socket connect */
int32_t socket_connect(struct socket_desc *desc, struct socket_address *addr);

/* Socket disconnect */
int32_t socket_disconnect(struct socket_desc *desc);

/* TCP Socket send */
int32_t socket_send(struct socket_desc *desc, const void *data, uint32_t len);

/* TCP Socket recv */
int32_t socket_recv(struct socket_desc *desc, void *data, uint32_t len);

/* UDP Socket sendto */
int32_t socket_sendto(struct socket_desc *desc, const void *data, uint32_t size,
		      const struct socket_address *to);
/* UDP Socket recvfrom */
int32_t socket_recvfrom(struct socket_desc *desc, void *data, uint32_t size,
			struct socket_address *from);

#endif

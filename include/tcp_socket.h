/***************************************************************************//**
 *   @file   tcp_socket.h
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

#ifndef TCP_SOCKET_H
# define TCP_SOCKET_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "network_interface.h"
#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct tcp_socket_desc
 * @brief TCP Socket descriptor
 */
struct tcp_socket_desc;

/**
 * @struct stcp_socket_init_param
 * @brief Parameter to initialize a TCP Socket
 */
struct secure_init_param {
	/** Certificate authority certificate */
	char			*ca_cert;
	/** Server name */
	char			*hostname;
	/** Client certificate */
	char			*client_cert;
	/** Client key */
	char			*client_key;
	/** Init param for true random number generator */
	struct trng_init_param	*trng_init_param;
};

/**
 * @struct tcp_socket_init_param
 * @brief Parameter to initialize a TCP Socket
 */
struct tcp_socket_init_param {
	/** Reference to the network interface */
	struct network_interface	*net;
	/**
	 * Reference to \ref secure_init_param if a TCP socket over TLS should
	 * be used. NULL if just raw connection is needed.
	 */
	struct secure_init_param	*secure_init_param;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init socket */
int32_t socket_init(struct tcp_socket_desc **desc,
		    struct tcp_socket_init_param *param);

/* Socket remove */
int32_t socket_remove(struct tcp_socket_desc *desc);

/* Socket connect */
int32_t socket_connect(struct tcp_socket_desc *desc,
		       struct socket_address *addr);

/* Socket disconnect */
int32_t socket_disconnect(struct tcp_socket_desc *desc);

/* Socket send */
int32_t socket_send(struct tcp_socket_desc *desc, const void *data,
		    uint32_t len);

/* Socket recv */
int32_t socket_recv(struct tcp_socket_desc *desc, void *data, uint32_t len);

#endif

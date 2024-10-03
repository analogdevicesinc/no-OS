/***************************************************************************//**
 *   @file   tcp_socket.h
 *   @brief  TCP Socket functions
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

#ifndef TCP_SOCKET_H
# define TCP_SOCKET_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "network_interface.h"
#ifndef DISABLE_SECURE_SOCKET
#include "mbedtls/ssl.h"
#endif
#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
#define MAX_BACKLOG 0xFFFFFFFF

/* Socket descriptor */
struct tcp_socket_desc {
	/* Id of the opened socket */
	uint32_t			id;
	/* Reference to the network interface */
	struct network_interface	*net;
#ifndef DISABLE_SECURE_SOCKET
	/* Reference to secure descriptor */
	struct secure_socket_desc	*secure;
#endif /* DISABLE_SECURE_SOCKET */
};

#ifndef DISABLE_SECURE_SOCKET
/**
 * @struct stcp_socket_init_param
 * @brief Parameter to initialize a TCP Socket
 */
struct secure_init_param {
	/** Init param for true random number generator */
	struct no_os_trng_init_param	*trng_init_param;
	/** Server Hostname */
	uint8_t			*hostname;
	/** Certificate Verification Mode:
	 * MBEDTLS_SSL_VERIFY_NONE: peer certificate is not checked
	 * MBEDTLS_SSL_VERIFY_OPTIONAL: peer certificate is checked, however the
	 * handshake continues even if verification failed;
	 * MBEDTLS_SSL_VERIFY_REQUIRED: peer *must* present a valid certificate,
	 * handshake is aborted if verification failed.
	 */
	int			cert_verify_mode;
	/**
	  * Certificate authority certificate.
	  * If set, server identity will be verified, otherwise not.
	  * ca_cert should be given in binary(der) format.
	  * If ENABLE_PEM_CERT is not defined, the ca_cert memory should not be
	  * modified.
	  */
	uint8_t			*ca_cert;
	/** Ca_cert length */
	uint32_t		ca_cert_len;
	/**
	 * Client certificate.
	 * Must be set if server requires it, otherwise it can be NULL
	 * If ENABLE_PEM_CERT is not defined, the cli_cert memory should not be
	 * modified.
	 */
	uint8_t			*cli_cert;
	/** cli_cert length */
	uint32_t		cli_cert_len;
	/** Client certificate private key. Can't be NULL if cli_cert is set */
	uint8_t			*cli_pk;
	/** cli_pk length */
	uint32_t		cli_pk_len;
};

#endif /* DISABLE_SECURE_SOCKET */

/**
 * @struct tcp_socket_init_param
 * @brief Parameter to initialize a TCP Socket
 */
struct tcp_socket_init_param {
	/** Reference to the network interface */
	struct network_interface	*net;
	/**
	 *  Max buffer size for incoming data.
	 *  If set to 0, default value will be used:
	 *  DEFAULT_CONNECTION_BUFFER_SIZE from tcp_socket.c
	 */
	uint32_t			max_buff_size;
#ifndef DISABLE_SECURE_SOCKET
	/**
	 * Reference to \ref secure_init_param if a TCP socket over TLS should
	 * be used. NULL if just raw connection is needed.
	 */
	struct secure_init_param	*secure_init_param;
#endif /* DISABLE_SECURE_SOCKET */
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

/* Socket bind */
int32_t socket_bind(struct tcp_socket_desc *desc, uint16_t port);

/* Socket listen */
int32_t socket_listen(struct tcp_socket_desc *desc, uint32_t back_log);

/* Socket accept */
int32_t socket_accept(struct tcp_socket_desc *desc,
		      struct tcp_socket_desc **new_client);

#endif

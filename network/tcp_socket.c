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
#include "no_os_error.h"
#include "tcp_socket.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

#ifndef DISABLE_SECURE_SOCKET
#include "noos_mbedtls_config.h"
#include "no_os_trng.h"
#endif /* DISABLE_SECURE_SOCKET */

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef DISABLE_SECURE_SOCKET

#warning "TCP socket communication is not secured"

#define DEFAULT_CONNECTION_BUFFER_SIZE 256

#else

#ifdef MAX_CONTENT_LEN
#define DEFAULT_CONNECTION_BUFFER_SIZE MAX_CONTENT_LEN
#else
#define DEFAULT_CONNECTION_BUFFER_SIZE 16384
#endif /* MAX_CONTENT_LEN */

#endif /* DISABLE_SECURE_SOCKET */

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

#ifndef DISABLE_SECURE_SOCKET
/**
 * @struct secure_socket_desc
 * @brief Fields used by secure socket
 */
struct secure_socket_desc {
	/** True random number generator reference */
	struct no_os_trng_desc	*trng;
	/* Mbed structures */
	/** CA certificate */
	mbedtls_x509_crt	cacert;
	/** Client certificate */
	mbedtls_x509_crt	clicert;
	/** Client private key */
	mbedtls_pk_context	pkey;
	/** SSL configuration structure */
	mbedtls_ssl_config	conf;
	/** Mbedtls tls context */
	mbedtls_ssl_context	ssl;
};
#endif /* DISABLE_SECURE_SOCKET */

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#ifndef DISABLE_SECURE_SOCKET
/* Wrapper over socket_recv */
static int tls_net_recv(struct tcp_socket_desc *sock, unsigned char *buff,
			size_t len)
{
	int32_t ret;

	ret = sock->net->socket_recv(sock->net->net, sock->id, buff, len);
	if (ret == -EAGAIN)
		return MBEDTLS_ERR_SSL_WANT_READ;

	return ret;
}

/* Wrapper over socket_recv */
static int tls_net_send(struct tcp_socket_desc *sock, unsigned char *buff,
			size_t len)
{
	return sock->net->socket_send(sock->net->net, sock->id, buff, len);
}

/* Remove secure descriptor*/
static void stcp_socket_remove(struct secure_socket_desc *desc)
{
	mbedtls_pk_free(&desc->pkey);
	mbedtls_x509_crt_free(&desc->clicert);
	mbedtls_x509_crt_free(&desc->cacert);
	mbedtls_ssl_config_free(&desc->conf);
	if (desc->trng)
		no_os_trng_remove(desc->trng);

	no_os_free(desc);
}

/* Init secure descriptor */
static int32_t stcp_socket_init(struct secure_socket_desc **desc,
				struct tcp_socket_desc *sock,
				struct secure_init_param *param)
{
	struct secure_socket_desc	*ldesc;
	int32_t				ret;

	if (!desc || !param)
		return -1;

	ldesc = (typeof(ldesc))no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -1;

	/* Initialize structures */
	mbedtls_ssl_config_init(&ldesc->conf);
	mbedtls_x509_crt_init(&ldesc->cacert);
	mbedtls_x509_crt_init(&ldesc->clicert);
	mbedtls_pk_init(&ldesc->pkey);
	mbedtls_ssl_init(&ldesc->ssl);

	ret = no_os_trng_init(&ldesc->trng, param->trng_init_param);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		ldesc->trng = NULL;
		goto exit;
	}

	/* Set default configuration: TLS client socket */
	ret = mbedtls_ssl_config_defaults(&ldesc->conf,
					  MBEDTLS_SSL_IS_CLIENT,
					  MBEDTLS_SSL_TRANSPORT_STREAM,
					  MBEDTLS_SSL_PRESET_DEFAULT);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto exit;

	if (param->ca_cert) {
#ifdef ENABLE_PEM_CERT
		ret = mbedtls_x509_crt_parse( &ldesc->cacert,
#else
		ret = mbedtls_x509_crt_parse_der_nocopy(&ldesc->cacert,
#endif /* ENABLE_PEM_CERT */
					      (const unsigned char *)param->ca_cert,
					      (size_t)param->ca_cert_len);
		if (ret < 0)
			goto exit;

		mbedtls_ssl_conf_ca_chain(&ldesc->conf, &ldesc->cacert, NULL );
		/* Verify server identity */
		mbedtls_ssl_conf_authmode(&ldesc->conf,
					  param->cert_verify_mode);
	} else {
		/* Do not verify server identity */
		mbedtls_ssl_conf_authmode(&ldesc->conf,
					  MBEDTLS_SSL_VERIFY_NONE);
	}

	if (param->cli_cert) {
		if (!param->cli_pk) {
			ret = -EINVAL;
			goto exit;
		}
#ifdef ENABLE_PEM_CERT
		ret = mbedtls_x509_crt_parse( &ldesc->clicert,
#else
		ret = mbedtls_x509_crt_parse_der_nocopy(&ldesc->clicert,
#endif /* ENABLE_PEM_CERT */
					      (const unsigned char *)param->cli_cert,
					      (size_t)param->cli_cert_len);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto exit;
		ret = mbedtls_pk_parse_key(&ldesc->pkey,
					   (const unsigned char *)param->cli_pk,
					   param->cli_pk_len, NULL, 0 );
		if (NO_OS_IS_ERR_VALUE(ret))
			goto exit;

		ret = mbedtls_ssl_conf_own_cert(&ldesc->conf, &ldesc->clicert,
						&ldesc->pkey);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto exit;
	}

	/* Config Random number generator */
	mbedtls_ssl_conf_rng(&ldesc->conf,
			     (int (*)(void *, unsigned char *, size_t))
			     no_os_trng_fill_buffer,
			     (void *)ldesc->trng);

	/* Set the resulting protocol configuration */
	ret = mbedtls_ssl_setup(&ldesc->ssl, &ldesc->conf);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto exit;

	ret = mbedtls_ssl_set_hostname(&ldesc->ssl, param->hostname);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto exit;

	/* Set socket callbacks */
	mbedtls_ssl_set_bio(&ldesc->ssl, sock,
			    (mbedtls_ssl_send_t *)tls_net_send,
			    (mbedtls_ssl_recv_t *)tls_net_recv, NULL);

	*desc = ldesc;

	return 0;

exit:
	stcp_socket_remove(ldesc);

	return ret;
}
#endif /* DISABLE_SECURE_SOCKET */

/**
 * @brief Allocate resources and initializes the socket descriptor
 * @param desc - Address where to store the socket descriptor
 * @param param - Initializing data
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t socket_init(struct tcp_socket_desc **desc,
		    struct tcp_socket_init_param *param)
{
	struct tcp_socket_desc	*ldesc;
	int32_t			ret;
	uint32_t		buff_size;

	if (!desc || !param)
		return -1;

	ldesc = (typeof(ldesc))no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -1;

	ldesc->net = param->net;

	if (param->max_buff_size != 0)
		buff_size = param->max_buff_size;
	else
		buff_size = DEFAULT_CONNECTION_BUFFER_SIZE;

	ret = ldesc->net->socket_open(ldesc->net->net, &ldesc->id, PROTOCOL_TCP,
				      buff_size);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		no_os_free(ldesc);
		return ret;
	}

#ifndef DISABLE_SECURE_SOCKET
	if (!param->secure_init_param)
		ldesc->secure = NULL;
	else
		ret = stcp_socket_init(&ldesc->secure, ldesc,
				       param->secure_init_param);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		ldesc->net->socket_close(ldesc->net->net, ldesc->id);
		no_os_free(ldesc);
		return ret;
	}
#endif /* DISABLE_SECURE_SOCKET */

	*desc = ldesc;

	return 0;
}

/**
 * @brief Deallocate resources from the socket descriptor
 * @param desc - Socket descriptor
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t socket_remove(struct tcp_socket_desc *desc)
{
	int32_t ret;

	if (!desc)
		return -1;

#ifndef DISABLE_SECURE_SOCKET
	if (desc->secure)
		stcp_socket_remove(desc->secure);
#endif /* DISABLE_SECURE_SOCKET */

	ret = desc->net->socket_close(desc->net->net, desc->id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;
	no_os_free(desc);

	return 0;
}

/** @brief See \ref network_interface.socket_connect */
int32_t socket_connect(struct tcp_socket_desc *desc,
		       struct socket_address *addr)
{
	int32_t ret;

	if (!desc || !addr)
		return -1;

	ret = desc->net->socket_connect(desc->net->net,
					desc->id, addr);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

#ifndef DISABLE_SECURE_SOCKET
	if (desc->secure) {
		do {
			ret = mbedtls_ssl_handshake(&desc->secure->ssl);
		} while (ret == MBEDTLS_ERR_SSL_WANT_READ);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
	}
#endif /* DISABLE_SECURE_SOCKET */

	return 0;
}

/** @brief See \ref network_interface.socket_disconnect */
int32_t socket_disconnect(struct tcp_socket_desc *desc)
{
	if (!desc)
		return -1;

#ifndef DISABLE_SECURE_SOCKET
	if (desc->secure)
		mbedtls_ssl_close_notify(&desc->secure->ssl);
#endif /* DISABLE_SECURE_SOCKET */

	return desc->net->socket_disconnect(desc->net->net, desc->id);
}

/** @brief See \ref network_interface.socket_send */
int32_t socket_send(struct tcp_socket_desc *desc, const void *data,
		    uint32_t len)
{

	if (!desc)
		return -1;

#ifndef DISABLE_SECURE_SOCKET
	if (desc->secure)
		return mbedtls_ssl_write(&desc->secure->ssl, data, len);
#endif /* DISABLE_SECURE_SOCKET */

	return desc->net->socket_send(desc->net->net, desc->id,
				      data, len);
}

/** @brief See \ref network_interface.socket_recv */
int32_t socket_recv(struct tcp_socket_desc *desc, void *data, uint32_t len)
{
	if (!desc)
		return -1;

#ifndef DISABLE_SECURE_SOCKET
	int32_t ret;

	if (desc->secure) {
		ret = mbedtls_ssl_read(&desc->secure->ssl, data, len);
		if (ret == MBEDTLS_ERR_SSL_WANT_READ)
			return -EAGAIN;

		return ret;
	}
#endif /* DISABLE_SECURE_SOCKET */

	return desc->net->socket_recv(desc->net->net, desc->id, data,
				      len);
}

/** @brief See \ref network_interface.socket_bind */
int32_t socket_bind(struct tcp_socket_desc *desc, uint16_t port)
{
#ifndef DISABLE_SECURE_SOCKET
	if (desc->secure)
		return -ENOSYS;
#endif

	return desc->net->socket_bind(desc->net->net, desc->id, port);
}

/** @brief See \ref network_interface.socket_listen */
int32_t socket_listen(struct tcp_socket_desc *desc, uint32_t back_log)
{
#ifndef DISABLE_SECURE_SOCKET
	if (desc->secure)
		return -ENOSYS;
#endif

	return desc->net->socket_listen(desc->net->net, desc->id, back_log);
}

/** @brief See \ref network_interface.socket_accept */
int32_t socket_accept(struct tcp_socket_desc *desc,
		      struct tcp_socket_desc **new_client)
{
	uint32_t		new_cli_id;
	int32_t			ret;

#ifndef DISABLE_SECURE_SOCKET
	if (desc->secure)
		return -ENOSYS;
#endif

	ret = desc->net->socket_accept(desc->net->net, desc->id, &new_cli_id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	*new_client = (typeof(*new_client))no_os_calloc(1, sizeof(**new_client));
	if (!*new_client) {
		desc->net->socket_close(desc->net->net, desc->id);
		return -ENOMEM;
	}
	(*new_client)->net = desc->net;
	(*new_client)->id = new_cli_id;

	return 0;
}


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
#include "tcp_socket.h"
#include "util.h"
#include "trng.h"
#include "mbedtls/ssl.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct secure_socket_desc
 * @brief Fields used by secure socket
 */
struct secure_socket_desc {
	/** True random number generator reference */
	struct trng_desc		*trng;
	/* Mbed structures */
	/** Certification authority certificate */
	mbedtls_x509_crt		ca_cert;
	/** Client certificate */
	mbedtls_x509_crt		client_cert;
	/** SSL configuration structure */
	mbedtls_ssl_config		conf;
	/** Mbedtls tls context */
	mbedtls_ssl_context		ssl;
};

/* Socket descriptor */
struct tcp_socket_desc {
	/* Id of the opened socket */
	uint32_t			id;
	/* Reference to the network interface */
	struct network_interface	*net;
	/* Reference to secure descriptor */
	struct secure_socket_desc	*secure;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
	mbedtls_x509_crt_free(&desc->ca_cert);
	mbedtls_x509_crt_free(&desc->client_cert);
	mbedtls_ssl_config_free(&desc->conf);
	if (desc->trng)
		trng_remove(desc->trng);

	free(desc);
}

/* Init secure descriptor */
static int32_t stcp_socket_init(struct secure_socket_desc **desc,
				struct tcp_socket_desc *sock,
				struct secure_init_param *param)
{
	struct secure_socket_desc	*ldesc;
	int32_t				ret;

	if (!desc || !param || !param->ca_cert)
		return FAILURE;

	ldesc = (typeof(ldesc))calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	/* Initialize structures */
	mbedtls_x509_crt_init(&ldesc->ca_cert);
	mbedtls_x509_crt_init(&ldesc->client_cert);
	mbedtls_ssl_config_init(&ldesc->conf);

	ret = trng_init(&ldesc->trng, param->trng_init_param);
	if (IS_ERR_VALUE(ret)) {
		ldesc->trng = NULL;
		goto exit;
	}

	/* Set default configuration: TLS client socket */
	ret = mbedtls_ssl_config_defaults(&ldesc->conf,
					  MBEDTLS_SSL_IS_CLIENT,
					  MBEDTLS_SSL_TRANSPORT_STREAM,
					  MBEDTLS_SSL_PRESET_DEFAULT);
	if (IS_ERR_VALUE(ret))
		goto exit;

	/* Certificate validation is set as mandatory */
	mbedtls_ssl_conf_authmode(&ldesc->conf, MBEDTLS_SSL_VERIFY_NONE);

	/* Decode the root certificate */
	ret = mbedtls_x509_crt_parse(&ldesc->ca_cert,
				     (const unsigned char *)param->ca_cert,
				     strlen(param->ca_cert) + 1);
	if (IS_ERR_VALUE(ret))
		goto exit;
	/* Set issuer certificate authority. NULL for revoked certificates */
	mbedtls_ssl_conf_ca_chain(&ldesc->conf, &ldesc->ca_cert, NULL);

	/* Decode client certificate */
	if (param->client_cert) {
		ret = mbedtls_x509_crt_parse(&ldesc->client_cert,
					     (const unsigned char *)
					     param->client_cert,
					     strlen(param->client_cert) + 1);
		if (IS_ERR_VALUE(ret))
			goto exit;
		/* Set own certificate */
		mbedtls_ssl_conf_own_cert(&ldesc->conf, &ldesc->client_cert,
					  NULL);
	}

	/* Config Random number generator */
	mbedtls_ssl_conf_rng(&ldesc->conf,
			     (int (*)(void *, unsigned char *, size_t))
			     trng_fill_buffer,
			     (void *)ldesc->trng);

	/* Set the resulting protocol configuration */
	ret = mbedtls_ssl_setup(&ldesc->ssl, &ldesc->conf);
	if (IS_ERR_VALUE(ret))
		goto exit;

	if (param->hostname) {
		ret = mbedtls_ssl_set_hostname(&ldesc->ssl, param->hostname);
		if (IS_ERR_VALUE(ret))
			goto exit;
	}

	/* Set socket callbacks */
	mbedtls_ssl_set_bio(&ldesc->ssl, sock,
			    (mbedtls_ssl_send_t *)tls_net_send,
			    (mbedtls_ssl_recv_t *)tls_net_recv, NULL);

	*desc = ldesc;

	return SUCCESS;

exit:
	stcp_socket_remove(ldesc);

	return ret;
}

/**
 * @brief Allocate resources and initializes the socket descriptor
 * @param desc - Address where to store the socket descriptor
 * @param param - Initializing data
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t socket_init(struct tcp_socket_desc **desc,
		    struct tcp_socket_init_param *param)
{
	struct tcp_socket_desc	*ldesc;
	int32_t			ret;

	if (!desc || !param)
		return FAILURE;

	ldesc = (typeof(ldesc))calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	ldesc->net = param->net;

	ret = ldesc->net->socket_open(ldesc->net->net, &ldesc->id,
				      PROTOCOL_TCP);
	if (IS_ERR_VALUE(ret)) {
		free(ldesc);
		return ret;
	}

	if (!param->secure_init_param)
		ldesc->secure = NULL;
	else
		ret = stcp_socket_init(&ldesc->secure, ldesc,
				       param->secure_init_param);
	if (IS_ERR_VALUE(ret)) {
		ldesc->net->socket_close(ldesc->net->net, ldesc->id);
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
int32_t socket_remove(struct tcp_socket_desc *desc)
{
	int32_t ret;

	if (!desc)
		return FAILURE;
	if (desc->secure)
		stcp_socket_remove(desc->secure);
	ret = desc->net->socket_close(desc->net->net, desc->id);
	if (IS_ERR_VALUE(ret))
		return ret;
	free(desc);

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_connect */
int32_t socket_connect(struct tcp_socket_desc *desc,
		       struct socket_address *addr)
{
	int32_t ret;

	if (!desc || !addr)
		return FAILURE;

	ret = desc->net->socket_connect(desc->net->net,
					desc->id, addr);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (desc->secure) {
		do {
			ret = mbedtls_ssl_handshake(&desc->secure->ssl);
		} while (ret == MBEDTLS_ERR_SSL_WANT_READ);
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_disconnect */
int32_t socket_disconnect(struct tcp_socket_desc *desc)
{
	if (!desc)
		return FAILURE;

	if (desc->secure)
		mbedtls_ssl_close_notify(&desc->secure->ssl);

	return desc->net->socket_disconnect(desc->net->net, desc->id);
}

/** @brief See \ref network_interface.socket_send */
int32_t socket_send(struct tcp_socket_desc *desc, const void *data,
		    uint32_t len)
{

	if (!desc)
		return FAILURE;

	if (desc->secure)
		return mbedtls_ssl_write(&desc->secure->ssl, data, len);
	else
		return desc->net->socket_send(desc->net->net, desc->id,
					      data, len);
}

/** @brief See \ref network_interface.socket_recv */
int32_t socket_recv(struct tcp_socket_desc *desc, void *data, uint32_t len)
{
	int32_t ret;

	if (!desc)
		return FAILURE;

	if (!desc->secure)
		return desc->net->socket_recv(desc->net->net, desc->id, data,
					      len);

	ret = mbedtls_ssl_read(&desc->secure->ssl, data, len);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ)
		return -EAGAIN;

	return ret;
}

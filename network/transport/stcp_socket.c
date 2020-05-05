/***************************************************************************//**
 *   @file   stcp_socket.c
 *   @brief  Secure TCP Socket functions
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


#include <stdlib.h>
#include <string.h>
#include "socket.h"
#include "socket_interface.h"
#include "error.h"
#include "util.h"
#include "trng.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"

/**
 * @struct stcp_socket_desc
 * @brief Fields used by socket
 */
struct stcp_socket_desc {
	/** TCP Socket reference */
	struct socket_desc		*sock;
	/** True random number generator reference */
	struct trng_desc		*trng;
	/* Mbed structures */
	/** Certification authority certificate */
	mbedtls_x509_crt		ca_cert[1];
	/** Client certificate */
	mbedtls_x509_crt		client_cert[1];
	/** Client private key */
	mbedtls_pk_context		client_pkey[1];
	/** SSL configuration strucure */
	mbedtls_ssl_config		conf[1];
	/** Mbedtls tls context */
	mbedtls_ssl_context		ssl[1];
};

static int32_t stcp_socket_init(struct stcp_socket_desc **desc,
				struct network_interface *net,
				struct stcp_socket_init_param *param);
static int32_t stcp_socket_remove(struct stcp_socket_desc *desc);
static int32_t stcp_socket_connect(struct stcp_socket_desc *desc,
				   struct socket_address *addr);
static int32_t stcp_socket_disconnect(struct stcp_socket_desc *desc);
static int32_t stcp_socket_send(struct stcp_socket_desc *desc, const void *data,
				uint32_t len);
static int32_t stcp_socket_recv(struct stcp_socket_desc *desc, void *data,
				uint32_t len);

/* Secure socket interface */
const struct socket_interface stcp_socket_interface = {
	.protocol = PROTOCOL_TCP,
	.init = (int32_t (*)())stcp_socket_init,
	.remove = (int32_t (*)())stcp_socket_remove,
	.connect = (int32_t (*)())stcp_socket_connect,
	.disconnect = (int32_t (*)())stcp_socket_disconnect,
	.send = (int32_t (*)())stcp_socket_send,
	.recv = (int32_t (*)())stcp_socket_recv,
	/* Only for PROTOCOL UDP */
	.sendto = NULL,
	.recvfrom = NULL
};

/* Wrapper over socket_recv */
static int tls_net_recv(struct socket_desc *sock, unsigned char *buf,
			size_t len)
{
	int32_t ret;

	ret = socket_recv(sock, buf, len);
	if (ret == -EAGAIN)
		return MBEDTLS_ERR_SSL_WANT_READ;

	return ret;
}

/** @brief See \ref socket_init */
static int32_t stcp_socket_init(struct stcp_socket_desc **desc,
				struct network_interface *net,
				struct stcp_socket_init_param *param)
{
	struct socket_init_param	sock_param;
	struct stcp_socket_desc		*ldesc;
	int32_t				ret;

	if (!desc || !param || !param->ca_cert)
		return FAILURE;

	ldesc = (typeof(ldesc))calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	/* Initialize structures */
	mbedtls_x509_crt_init(ldesc->ca_cert);
	mbedtls_x509_crt_init(ldesc->client_cert);
	mbedtls_pk_init(ldesc->client_pkey);
	mbedtls_ssl_config_init(ldesc->conf);

	sock_param.type = TCP_SOCKET;
	sock_param.net = net;
	sock_param.init_param.tcp = NULL;
	ret = socket_init(&ldesc->sock, &sock_param);
	if (IS_ERR_VALUE(ret)) {
		ldesc->sock = NULL;
		goto exit;
	}

	ret = trng_init(&ldesc->trng, param->trng_init_param);
	if (IS_ERR_VALUE(ret)) {
		ldesc->trng = NULL;
		goto exit;
	}

	/* Set default configuration: TLS client socket */
	ret = mbedtls_ssl_config_defaults(ldesc->conf,
					  MBEDTLS_SSL_IS_CLIENT,
					  MBEDTLS_SSL_TRANSPORT_STREAM,
					  MBEDTLS_SSL_PRESET_DEFAULT);
	if (IS_ERR_VALUE(ret))
		goto exit;

	/* Certificate validation is set as mandatory */
	mbedtls_ssl_conf_authmode(ldesc->conf, MBEDTLS_SSL_VERIFY_NONE);

	/* Decode the root certificate */
	ret = mbedtls_x509_crt_parse(ldesc->ca_cert,
				     (const unsigned char *)param->ca_cert,
				     strlen(param->ca_cert) + 1);
	if (IS_ERR_VALUE(ret))
		goto exit;
	/* Set issuer certificate authority. NULL for revoked certificates */
	mbedtls_ssl_conf_ca_chain(ldesc->conf, ldesc->ca_cert, NULL);

	/* Decode client certificate */
	if (param->client_cert) {
		ret = mbedtls_x509_crt_parse(ldesc->client_cert,
					     (const unsigned char *)param->client_cert,
					     strlen(param->client_cert) + 1);
		if (IS_ERR_VALUE(ret))
			goto exit;

		if (param->client_key) {
			ret = mbedtls_pk_parse_key(ldesc->client_pkey,
						   (const unsigned char *)param->client_key,
						   strlen(param->client_key) + 1,
						   NULL, 0);
			if (IS_ERR_VALUE(ret))
				goto exit;
		}
		/* Set own certificate */
		mbedtls_ssl_conf_own_cert(ldesc->conf, ldesc->client_cert,
					  ldesc->client_pkey);
	}


	/* Config Random number generator */
	mbedtls_ssl_conf_rng(ldesc->conf,
			     (int (*)(void *, unsigned char *, size_t))
			     trng_fill_buffer,
			     (void *)ldesc->trng);

	/* Set the resulting protocol configuration */
	ret = mbedtls_ssl_setup(ldesc->ssl, ldesc->conf);
	if (IS_ERR_VALUE(ret))
		goto exit;

	if (param->hostname) {
		ret = mbedtls_ssl_set_hostname(ldesc->ssl, param->hostname);
		if (IS_ERR_VALUE(ret))
			goto exit;
	}

	/* Set socket callbacks */
	mbedtls_ssl_set_bio(ldesc->ssl, ldesc->sock,
			    (mbedtls_ssl_send_t *)socket_send,
			    (mbedtls_ssl_recv_t *)tls_net_recv, NULL);

	*desc = ldesc;

	return SUCCESS;

exit:
	stcp_socket_remove(ldesc);

	return ret;
}

/** @brief See \ref socket_remove */
static int32_t stcp_socket_remove(struct stcp_socket_desc *desc)
{
	if (!desc)
		return FAILURE;

	stcp_socket_disconnect(desc);

	mbedtls_x509_crt_free(desc->ca_cert);
	mbedtls_x509_crt_free(desc->client_cert);
	mbedtls_pk_free(desc->client_pkey);
	mbedtls_ssl_config_free(desc->conf);
	if (desc->trng)
		trng_remove(desc->trng);
	if (desc->sock)
		socket_remove(desc->sock);
	free(desc);

	return SUCCESS;
}

/** @brief See \ref socket_connect */
static int32_t stcp_socket_connect(struct stcp_socket_desc *desc,
				   struct socket_address *addr)
{
	int32_t ret;

	if (!desc)
		return FAILURE;

	ret = socket_connect(desc->sock, addr);
	if (IS_ERR_VALUE(ret))
		return ret;
	do {
		ret = mbedtls_ssl_handshake(desc->ssl);
	} while (ret == MBEDTLS_ERR_SSL_WANT_READ);

	if (IS_ERR_VALUE(ret))
		return ret;

	return SUCCESS;
}

/** @brief See \ref socket_disconnect */
static int32_t stcp_socket_disconnect(struct stcp_socket_desc *desc)
{
	if (!desc)
		return FAILURE;

	return socket_disconnect(desc->sock);
}

/** @brief See \ref socket_send */
static int32_t stcp_socket_send(struct stcp_socket_desc *desc, const void *data,
				uint32_t len)
{
	if (!desc)
		return FAILURE;

	return mbedtls_ssl_write(desc->ssl, data, len);
}

/** @brief See \ref socket_recv */
static int32_t stcp_socket_recv(struct stcp_socket_desc *desc, void *data,
				uint32_t len)
{
	int32_t ret;

	if (!desc)
		return FAILURE;

	ret = mbedtls_ssl_read(desc->ssl, data, len);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ)
		return -EAGAIN;

	return ret;
}

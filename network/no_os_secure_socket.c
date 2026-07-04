/***************************************************************************//**
 *   @file   no_os_secure_socket.c
 *   @brief  TLS socket that composes over a no_os_socket transport.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#ifndef DISABLE_SECURE_SOCKET

#include <stdlib.h>
#include "no_os_secure_socket.h"
#include "no_os_net.h"
#include "no_os_socket.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_trng.h"
#include "noos_mbedtls_config.h"
#include "mbedtls/ssl.h"

/**
 * @struct secure_socket_extra
 * @brief Backend state stored in a secure no_os_socket_desc's "extra".
 */
struct secure_socket_extra {
	/* Underlying (plaintext) transport socket */
	struct no_os_socket_desc	*inner;
	/* True random number generator reference */
	struct no_os_trng_desc		*trng;
	/* CA certificate */
	mbedtls_x509_crt		cacert;
	/* Client certificate */
	mbedtls_x509_crt		clicert;
	/* Client private key */
	mbedtls_pk_context		pkey;
	/* SSL configuration structure */
	mbedtls_ssl_config		conf;
	/* mbedTLS SSL context */
	mbedtls_ssl_context		ssl;
};

static const struct no_os_socket_ops secure_socket_ops;

/* Map the backend-neutral verify policy to an mbedTLS authmode value. */
static int secure_authmode(enum no_os_secure_verify_mode mode)
{
	switch (mode) {
	case NO_OS_SECURE_VERIFY_OPTIONAL:
		return MBEDTLS_SSL_VERIFY_OPTIONAL;
	case NO_OS_SECURE_VERIFY_REQUIRED:
		return MBEDTLS_SSL_VERIFY_REQUIRED;
	default:
		return MBEDTLS_SSL_VERIFY_NONE;
	}
}

/* mbedTLS BIO recv: pull ciphertext from the inner transport socket. */
static int tls_net_recv(void *ctx, unsigned char *buff, size_t len)
{
	struct secure_socket_extra *s = ctx;
	int32_t ret;

	ret = no_os_socket_recv(s->inner, buff, len);
	if (ret == -EAGAIN)
		return MBEDTLS_ERR_SSL_WANT_READ;

	return ret;
}

/* mbedTLS BIO send: push ciphertext to the inner transport socket. */
static int tls_net_send(void *ctx, const unsigned char *buff, size_t len)
{
	struct secure_socket_extra *s = ctx;

	return no_os_socket_send(s->inner, buff, len);
}

static void secure_free(struct secure_socket_extra *s)
{
	mbedtls_ssl_free(&s->ssl);
	mbedtls_pk_free(&s->pkey);
	mbedtls_x509_crt_free(&s->clicert);
	mbedtls_x509_crt_free(&s->cacert);
	mbedtls_ssl_config_free(&s->conf);
	if (s->trng)
		no_os_trng_remove(s->trng);

	no_os_free(s);
}

int32_t no_os_secure_socket_init(struct no_os_socket_desc **desc,
				 struct no_os_socket_desc *inner,
				 const struct no_os_secure_init_param *param)
{
	struct no_os_socket_desc *sock;
	struct secure_socket_extra *s;
	int32_t ret;

	if (!desc || !inner || !param)
		return -EINVAL;

	sock = no_os_calloc(1, sizeof(*sock));
	if (!sock)
		return -ENOMEM;

	s = no_os_calloc(1, sizeof(*s));
	if (!s) {
		no_os_free(sock);
		return -ENOMEM;
	}

	s->inner = inner;

	mbedtls_ssl_config_init(&s->conf);
	mbedtls_x509_crt_init(&s->cacert);
	mbedtls_x509_crt_init(&s->clicert);
	mbedtls_pk_init(&s->pkey);
	mbedtls_ssl_init(&s->ssl);

	ret = no_os_trng_init(&s->trng, param->trng_init_param);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		s->trng = NULL;
		goto error;
	}

	ret = mbedtls_ssl_config_defaults(&s->conf, MBEDTLS_SSL_IS_CLIENT,
					  MBEDTLS_SSL_TRANSPORT_STREAM,
					  MBEDTLS_SSL_PRESET_DEFAULT);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error;

	if (param->ca_cert) {
#ifdef ENABLE_PEM_CERT
		ret = mbedtls_x509_crt_parse(&s->cacert,
#else
		ret = mbedtls_x509_crt_parse_der_nocopy(&s->cacert,
#endif /* ENABLE_PEM_CERT */
					     (const unsigned char *)param->ca_cert,
					     (size_t)param->ca_cert_len);
		if (ret < 0)
			goto error;

		mbedtls_ssl_conf_ca_chain(&s->conf, &s->cacert, NULL);
		mbedtls_ssl_conf_authmode(&s->conf,
					  secure_authmode(param->cert_verify_mode));
	} else {
		mbedtls_ssl_conf_authmode(&s->conf, MBEDTLS_SSL_VERIFY_NONE);
	}

	if (param->cli_cert) {
		if (!param->cli_pk) {
			ret = -EINVAL;
			goto error;
		}
#ifdef ENABLE_PEM_CERT
		ret = mbedtls_x509_crt_parse(&s->clicert,
#else
		ret = mbedtls_x509_crt_parse_der_nocopy(&s->clicert,
#endif /* ENABLE_PEM_CERT */
					     (const unsigned char *)param->cli_cert,
					     (size_t)param->cli_cert_len);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error;

		ret = mbedtls_pk_parse_key(&s->pkey,
					   (const unsigned char *)param->cli_pk,
					   param->cli_pk_len, NULL, 0);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error;

		ret = mbedtls_ssl_conf_own_cert(&s->conf, &s->clicert, &s->pkey);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error;
	}

	mbedtls_ssl_conf_rng(&s->conf,
			     (int (*)(void *, unsigned char *, size_t))
			     no_os_trng_fill_buffer, (void *)s->trng);

	ret = mbedtls_ssl_setup(&s->ssl, &s->conf);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error;

	ret = mbedtls_ssl_set_hostname(&s->ssl, (const char *)param->hostname);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error;

	mbedtls_ssl_set_bio(&s->ssl, s, tls_net_send, tls_net_recv, NULL);

	sock->net = inner->net;
	sock->ops = &secure_socket_ops;
	sock->id = inner->id;
	sock->extra = s;
	*desc = sock;

	return 0;

error:
	secure_free(s);
	no_os_free(sock);

	return ret;
}

static int32_t secure_close(struct no_os_socket_desc *sock)
{
	struct secure_socket_extra *s = sock->extra;
	int32_t ret;

	ret = no_os_socket_remove(s->inner);
	secure_free(s);
	no_os_free(sock);

	return ret;
}

static int32_t secure_connect(struct no_os_socket_desc *sock,
			      struct no_os_sockaddr *addr)
{
	struct secure_socket_extra *s = sock->extra;
	int32_t ret;

	ret = no_os_socket_connect(s->inner, addr);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	do {
		ret = mbedtls_ssl_handshake(&s->ssl);
	} while (ret == MBEDTLS_ERR_SSL_WANT_READ ||
		 ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	return NO_OS_IS_ERR_VALUE(ret) ? ret : 0;
}

static int32_t secure_disconnect(struct no_os_socket_desc *sock)
{
	struct secure_socket_extra *s = sock->extra;

	mbedtls_ssl_close_notify(&s->ssl);

	return no_os_socket_disconnect(s->inner);
}

static int32_t secure_send(struct no_os_socket_desc *sock, const void *data,
			   uint32_t size)
{
	struct secure_socket_extra *s = sock->extra;

	return mbedtls_ssl_write(&s->ssl, data, size);
}

static int32_t secure_recv(struct no_os_socket_desc *sock, void *data,
			   uint32_t size)
{
	struct secure_socket_extra *s = sock->extra;
	int32_t ret;

	ret = mbedtls_ssl_read(&s->ssl, data, size);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ)
		return -EAGAIN;

	return ret;
}

static const struct no_os_socket_ops secure_socket_ops = {
	.close = secure_close,
	.connect = secure_connect,
	.disconnect = secure_disconnect,
	.send = secure_send,
	.recv = secure_recv,
};

#endif /* DISABLE_SECURE_SOCKET */

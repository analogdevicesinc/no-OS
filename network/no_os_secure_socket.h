/***************************************************************************//**
 *   @file   no_os_secure_socket.h
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
#ifndef _NO_OS_SECURE_SOCKET_H_
#define _NO_OS_SECURE_SOCKET_H_

#ifndef DISABLE_SECURE_SOCKET

#include <stdint.h>
#include "no_os_socket.h"

struct no_os_trng_init_param;

/**
 * @enum no_os_secure_verify_mode
 * @brief Peer certificate verification policy for the TLS handshake.
 *
 * Backend-neutral: the secure socket implementation maps these to the
 * equivalent mbedTLS MBEDTLS_SSL_VERIFY_* values, so applications never need to
 * include any mbedTLS header.
 */
enum no_os_secure_verify_mode {
	/** Peer certificate is not checked. */
	NO_OS_SECURE_VERIFY_NONE,
	/** Peer certificate is checked, but the handshake continues on failure. */
	NO_OS_SECURE_VERIFY_OPTIONAL,
	/** Peer must present a valid certificate or the handshake aborts. */
	NO_OS_SECURE_VERIFY_REQUIRED,
};

/**
 * @struct no_os_secure_init_param
 * @brief Parameters used to configure the TLS layer.
 */
struct no_os_secure_init_param {
	/** Init param for the true random number generator */
	struct no_os_trng_init_param	*trng_init_param;
	/** Server hostname (used for SNI / certificate CN check) */
	uint8_t			*hostname;
	/** Peer certificate verification policy. */
	enum no_os_secure_verify_mode	cert_verify_mode;
	/** CA certificate (DER, unless ENABLE_PEM_CERT). NULL to skip verify. */
	uint8_t			*ca_cert;
	/** CA certificate length */
	uint32_t		ca_cert_len;
	/** Client certificate (optional, if the server requires one) */
	uint8_t			*cli_cert;
	/** Client certificate length */
	uint32_t		cli_cert_len;
	/** Client certificate private key (required if cli_cert is set) */
	uint8_t			*cli_pk;
	/** Client private key length */
	uint32_t		cli_pk_len;
};

/*
 * Wrap an already-open (but not yet connected) inner socket in a TLS layer.
 * On success, *desc is the secure socket; the caller must NOT use `inner`
 * directly afterwards (it is owned by the secure socket and closed with it).
 */
int32_t no_os_secure_socket_init(struct no_os_socket_desc **desc,
				 struct no_os_socket_desc *inner,
				 const struct no_os_secure_init_param *param);

#endif /* DISABLE_SECURE_SOCKET */
#endif /* _NO_OS_SECURE_SOCKET_H_ */

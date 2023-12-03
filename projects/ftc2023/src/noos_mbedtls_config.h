/***************************************************************************//**
 *   @file   noos_mbedtls_config.h
 *   @brief  Config to build mbedtls library
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2023(c) Analog Devices, Inc.
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

#ifndef NOOS_MBEDTLS_CONFIG_H
#define NOOS_MBEDTLS_CONFIG_H

/******************************************************************************/
/***************************** User configuration *****************************/
/******************************************************************************/

/*
 * For an easy configuration of mbedtls library only this macros from user
 * configuration should be modified. For more in deepth configuration see
 * https://tls.mbed.org/ and noos/libraries/mbedtls/includes/config.h
 */

#define MBEDTLS_DEBUG_C
#define MBEDTLS_ERROR_C

/* Latest supported version by mbedtls */
#define ENABLE_TLS1_2

/*
 * Enabled chipersuites.
 * Ordered from most preferred to least preferred in terms of security.
 *
 * These are the chipersuites supported by http://test.mosquitto.org
 * or the local mqtt server: https://mosquitto.org/download
 * For the moment there will be examples only for this servers but these
 * chipersuites should work for other servers too.
 */

//#define ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_GCM_SHA384
//#define ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_CBC_SHA
#define ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_GCM_SHA256
//#define ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA256
//#define ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA

/* Eliptic curves to be used by the chiper */
#define ENABLE_ECP_DP_SECP256R1_ENABLED
//#define ENABLE_ECP_DP_SECP384R1_ENABLED

/*
 * Parse certificates in PEM format.
 * If not defined, only DER format can be used.
 * On ADuCM3029 DER is recommended due to memmory limitations
 */
#define ENABLE_PEM_CERT

/*
 * Maximum length (in bytes) of incoming and outgoing plaintext fragments.
 * If not defined, default of 16kb will be used.
 * 2000 is a eoungh to do the tls handshake and is no to much
 * platforms with memory constrains like ADuCM3029
 */
//#define MAX_CONTENT_LEN 2500

/*
 * ENABLE_MEMORY_OPTIMIZATIONS should be defined in the case memory
 * is not enough. This could happen is using both a secure connection with
 * server an client verification.
 */
#define ENABLE_MEMORY_OPTIMIZATIONS

/******************************************************************************/
/********************* Minimal tls client requirements ************************/
/******************************************************************************/

/* Minimal requirements */
/* Hardware entropy is used (trng.h) */
#define MBEDTLS_NO_PLATFORM_ENTROPY
/* Needed in order to use TLS features */
#define MBEDTLS_SSL_TLS_C
/* TLS Client features */
#define MBEDTLS_SSL_CLI_C

/*
 * Define available chippersuites. Available only if the requierements are meet.
 * The requierements are generated depending on user configuration
 */
#define MBEDTLS_SSL_CIPHERSUITES \
	MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,\
	MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,\
	MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,\
	MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,\
	MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA

/******************************************************************************/
/*************** Configuration depending on user input ************************/
/******************************************************************************/

#ifdef MAX_CONTENT_LEN
#define MBEDTLS_SSL_MAX_CONTENT_LEN	MAX_CONTENT_LEN
#endif

#ifdef ENABLE_TLS1_2

#define MBEDTLS_SSL_PROTO_TLS1_2

/* Key exchange enabled types */
#if (defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_GCM_SHA384) || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_CBC_SHA)    || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_GCM_SHA256) || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA256) || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA) )

/* Enable KEY_EXCHANGE_ECDHE_RSA_ENABLED if used one of these chipersuites is defined */
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED

#endif /* Chipers that use ENABLE_KEY_EXCHANGE_ECDHE_RSA_ENABLED */
#endif /* ENABLE_TLS1_2 */

#ifdef ENABLE_MEMORY_OPTIMIZATIONS

#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_ECP_WINDOW_SIZE 2

#endif /* ENABLE_MEMORY_OPTIMIZATIONS */

#ifdef ENABLE_PEM_CERT

#define MBEDTLS_BASE64_C
#define MBEDTLS_PEM_PARSE_C

#endif /* ENABLE_PEM_CERT */

/******************************************************************************/
/**************** Solve dependencies needed by modules ************************/
/******************************************************************************/

/* Dependencies for MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED */
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED

#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECP_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C

#ifdef ENABLE_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#endif
#ifdef ENABLE_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#endif

#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED */

#ifdef MBEDTLS_SSL_TLS_C
#define MBEDTLS_MD_C
#define MBEDTLS_CIPHER_C
#endif /* MBEDTLS_SSL_TLS_C */

#if (defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_GCM_SHA384) ||\
		defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_CBC_SHA) ||\
		defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA) ||\
		defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_GCM_SHA256) ||\
		defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA256) )

# define MBEDTLS_AES_C

# if (defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_CBC_SHA) || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA) || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA256) )
#  define MBEDTLS_CIPHER_MODE_CBC
#  if defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA256)
#   define MBEDTLS_SHA256_C
#  endif
#  if (defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_CBC_SHA) || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_CBC_SHA))
#   define MBEDTLS_SHA1_C
#  endif
# endif

# if (defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_GCM_SHA256) || \
	defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_GCM_SHA384))
#  define MBEDTLS_GCM_C
#  if (defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_128_GCM_SHA256))
#   define MBEDTLS_SHA256_C
#  endif
#  if (defined(ENABLE_CHIPERSUITE_ECDHE_RSA_WITH_AES_256_GCM_SHA384))
#   define MBEDTLS_SHA512_C
#  endif
# endif

#endif

#ifdef MBEDTLS_SSL_PROTO_TLS1_2
#if (!defined(MBEDTLS_SHA512_C) && !defined(MBEDTLS_SHA256_C) &&\
		!defined(MBEDTLS_SHA1_C))
#define MBEDTLS_SHA256_C
#endif
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

/* Check if the configuration is ok */
#include "mbedtls/check_config.h"

#endif /* NOOS_MBEDTLS_CONFIG_H */

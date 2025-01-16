/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME

/* MAXQ Cryptographic Capabilities */
#define MBEDTLS_CIPHER_MAXQ
#define MBEDTLS_AES_MAXQ
#define MBEDTLS_CCM_MAXQ
#define MBEDTLS_GCM_MAXQ
#define MBEDTLS_ECDSA_MAXQ

#ifndef __MAXQ106x_C2__
#define MBEDTLS_CIPHER_MODE_CBC
#endif /* __MAXQ106x_C2__ */

/* MAXQ Personalization */
#ifdef __MAXQ10XX_PERSO__
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CSR_WRITE_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PEM_WRITE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_BASE64_C
#endif /* __MAXQ10XX_PERSO__ */

/* GCM */
#ifdef MBEDTLS_GCM_MAXQ
#define MBEDTLS_GCM_C
#define MBEDTLS_CCM_C
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_C
#endif /* MBEDTLS_GCM_MAXQ */

/* EC Functionalities */
#define MBEDTLS_ECP_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_ECP_NIST_OPTIM

/* NIST EC Curves */
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP521R1_ENABLED

#ifndef __MAXQ106x_C2__
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#endif /* __MAXQ106x_C2__ */

/* Key Exchange */
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

/* Hash */
#ifndef __MAXQ106x_C2__
#define MBEDTLS_SHA1_C
#endif /* __MAXQ106x_C2__ */
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C

/* Debug */
#define MBEDTLS_SSL_DEBUG_ALL
#define MBEDTLS_DEBUG_C
#define MBEDTLS_ERROR_C

/* TLS/DTLS */
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_COOKIE_C

#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C

#define MBEDTLS_MD_C
#define MBEDTLS_OID_C

#ifdef __MAXQ10XX__
#define MBEDTLS_X509_ALLOW_EXTENSIONS_NON_V3
#endif /* __MAXQ10XX__ */

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */

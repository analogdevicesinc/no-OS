/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef _CRYPTO_CIPHER_H_
#define _CRYPTO_CIPHER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include "crypto_common.h"
#include "crypto_config_def.h"
#include "MXQ_API.h"

#define PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(key_type, alg, input_length)     ((key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_CTR)             ? (input_length + 12) : \
                                                                        (key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_CBC_NO_PADDING)   ? (input_length + 16) : \
                                                                        (key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_ECB_NO_PADDING)   ? input_length : \
                                                                        0)
#define PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE(input_length)                (input_length + 16)
#define PSA_CIPHER_DECRYPT_OUTPUT_SIZE(key_type, alg, input_length)     ((key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_CTR)             ? (input_length - 12) : \
                                                                        (key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_CBC_NO_PADDING)   ? (input_length - 16) : \
                                                                        (key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_ECB_NO_PADDING)   ? input_length : \
                                                                        0)
#define PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(input_length)                (input_length)
#define PSA_CIPHER_IV_MAX_SIZE                                          (16)
#define PSA_CIPHER_IV_LENGTH(key_type, alg)                             ((key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_CTR) ? 12 : \
                                                                        (key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_CBC_NO_PADDING) ? 16 : \
                                                                        (key_type == PSA_KEY_TYPE_AES) && (alg == PSA_ALG_ECB_NO_PADDING) ? 0 : \
                                                                        0)

#define PSA_CIPHER_UPDATE_OUTPUT_SIZE(key_type, alg, input_length)      (input_length)
#define PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE(input_length)                 (input_length)
#define PSA_BLOCK_CIPHER_BLOCK_LENGTH(type)                                     \
                                                                        ((type & USS_PSA_KEY_TYPE_MASK) == USS_PSA_KEY_TYPE_SYMMETRIC_MASK ? \
                                                                        (1u << (((type) >> 8) & 7)) : 0u)
#define PSA_CIPHER_FINISH_OUTPUT_SIZE(key_type, alg)                    (PSA_ALG_IS_CIPHER(alg) ? PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) : 0)
#define PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE                               (0)                                                                        

#define PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE                                 USS_PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE

#define PSA_CIPHER_OPERATION_INIT (psa_cipher_operation_t)              {0}

#define PSA_ALG_IS_STREAM_CIPHER(alg) (((alg) & 0x7f800000) == 0x04800000)


#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //_CRYPTO_CIPHER_H_
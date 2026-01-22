/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef _CRYPTO_HASH_H_
#define _CRYPTO_HASH_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include "crypto_typedefs.h"
#include "crypto_common.h"
#include "crypto_config_def.h"

/** MD5 */
#define PSA_ALG_MD5                             ((psa_algorithm_t) 0x02000003)
/** PSA_ALG_RIPEMD160 */
#define PSA_ALG_RIPEMD160                       ((psa_algorithm_t) 0x02000004)
/** SHA1 */
#define PSA_ALG_SHA_1                           ((psa_algorithm_t) 0x02000005)
/** SHA2-224 */
#define PSA_ALG_SHA_224                         ((psa_algorithm_t) 0x02000008)
/** SHA2-256 */
#define PSA_ALG_SHA_256                         ((psa_algorithm_t) 0x02000009)
/** SHA2-384 */
#define PSA_ALG_SHA_384                         ((psa_algorithm_t) 0x0200000a)
/** SHA2-512 */
#define PSA_ALG_SHA_512                         ((psa_algorithm_t) 0x0200000b)
/** SHA2-512/224 */
#define PSA_ALG_SHA_512_224                     ((psa_algorithm_t) 0x0200000c)
/** SHA2-512/256 */
#define PSA_ALG_SHA_512_256                     ((psa_algorithm_t) 0x0200000d)
/** SHA3-224 */
#define PSA_ALG_SHA3_224                        ((psa_algorithm_t) 0x02000010)
/** SHA3-256 */
#define PSA_ALG_SHA3_256                        ((psa_algorithm_t) 0x02000011)
/** SHA3-384 */
#define PSA_ALG_SHA3_384                        ((psa_algorithm_t) 0x02000012)
/** SHA3-512 */
#define PSA_ALG_SHA3_512                        ((psa_algorithm_t) 0x02000013)
/** SM3 */
#define PSA_ALG_SM3                             ((psa_algorithm_t) 0x02000014)
/** First 512 bits (64 bytes) of the SHAKE256 output */
#define PSA_ALG_SHAKE256_512                    ((psa_algorithm_t) 0x02000015)
/** Wildcard hash*/
#define PSA_ALG_ANY_HASH                        ((psa_algorithm_t) 0x020000FF)

/**
 * @brief           Provides the output size of hash algorithm alg, in bytes
 * 
 * @param[in] alg   A hash algorithm or an HMAC algorithm: a value of type
 *                  psa_algorithm_t such that (PSA_ALG_IS_HASH(alg) ||
 *                  PSA_ALG_IS_HMAC(alg)) is true.
 *
 * @return          The hash length for the specified hash algorithm. 
 *                  If the hash algorithm is not recognized, return 0.
 */

#define PSA_HASH_LENGTH(alg) (                 \
        (alg == PSA_ALG_MD5) ? 16u :           \
        (alg == PSA_ALG_RIPEMD160) ? 20u :     \
        (alg == PSA_ALG_SHA_1) ? 20u :         \
        (alg == PSA_ALG_SHA_224) ? 28u :       \
        (alg == PSA_ALG_SHA_256) ? 32u :       \
        (alg == PSA_ALG_SHA_384) ? 48u :       \
        (alg == PSA_ALG_SHA_512) ? 64u :       \
        (alg == PSA_ALG_SHA_512_224) ? 28u :   \
        (alg == PSA_ALG_SHA_512_256) ? 32u :   \
        (alg == PSA_ALG_SHA3_224) ? 28u :      \
        (alg == PSA_ALG_SHA3_256) ? 32u :      \
        (alg == PSA_ALG_SHA3_384) ? 48u :      \
        (alg == PSA_ALG_SHA3_512) ? 64u :      \
        0u)

#ifdef USS_PSA_AVAILABLE_ALG_SHA512
        #define PSA_HASH_MAX_SIZE           64u
#elif defined(USS_PSA_AVAILABLE_ALG_SHA384)
        #define PSA_HASH_MAX_SIZE           48u
#elif defined(USS_PSA_AVAILABLE_ALG_SHA256)
        #define PSA_HASH_MAX_SIZE           32u
#endif

/**
 * @brief           Provides the block size of hash algorithm alg, in bytes
 * 
 * @param[in] alg   A hash algorithm such that PSA_ALG_IS_HASH(alg) is true.
 *
 * @return          The block length for the specified hash algorithm. 
 *                  If the hash algorithm is not recognized, return 0.
 */
#define PSA_HASH_BLOCK_LENGTH(alg) (           \
        (alg == PSA_ALG_MD5) ? 64u :           \
        (alg == PSA_ALG_RIPEMD160) ? 64u :     \
        (alg == PSA_ALG_SHA_1) ? 64u :         \
        (alg == PSA_ALG_SHA_224) ? 64u :       \
        (alg == PSA_ALG_SHA_256) ? 64u :       \
        (alg == PSA_ALG_SHA_384) ? 128u :      \
        (alg == PSA_ALG_SHA_512) ? 128u :      \
        (alg == PSA_ALG_SHA_512_224) ? 128u :  \
        (alg == PSA_ALG_SHA_512_256) ? 128u :  \
        (alg == PSA_ALG_SHA3_224) ? 144u :     \
        (alg == PSA_ALG_SHA3_256) ? 136u :     \
        (alg == PSA_ALG_SHA3_384) ? 104u :     \
        (alg == PSA_ALG_SHA3_512) ? 72u :      \
        0u)

/**
 * @brief          Check if the specified algorithm is a hash algorithm
 * 
 * @param[in] alg  An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return         1 if alg is a hash algorithm and is supported, 0 otherwise
 */
#define PSA_ALG_IS_HASH(alg)            ((alg & USS_PSA_ALG_MASK) == USS_PSA_ALG_TYPE_HASH)

/**
 * @brief         Get the hash used by a composite algorithm
 * 
 * @param[in] alg An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return        The underlying hash algorithm if alg is a composite algorithm that uses a hash algorithm.
 *                PSA_ALG_NONE if alg is not a composite algorithm that uses a hash.
 */
#define PSA_ALG_GET_HASH(alg) \
( ((alg & USS_PSA_ALG_HASH_MASK) == 0) ? PSA_ALG_NONE : (USS_PSA_ALG_TYPE_HASH | (alg & USS_PSA_ALG_HASH_MASK)) )

/* Initializer for hashing operation */
#define PSA_HASH_OPERATION_INIT      (psa_hash_operation_t){.alg = PSA_ALG_NONE, .isInitialized = false }

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //_CRYPTO_HASH_H_

/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef _CRYPTO_SIGN_H_
#define _CRYPTO_SIGN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include "crypto_typedefs.h"
#include "crypto_common.h"
#include "crypto_config_def.h"
#include "key_management.h"

#define PSA_ALG_SIGN_MASK                       ((psa_algorithm_t) 0x0000ff00)

/**
 * @brief   This macro serves as base for deriving other macros for
 *          RSA PKCS#1 v1.5 message signature scheme, with hashing
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN_BASE          ((psa_algorithm_t) 0x06000200)

/**
 * @brief   The RSA PKCS#1 v1.5 message signature scheme, with hashing
 *          This algorithm can be used with both the message and hash signature functions
 * 
 * @param[in] hash_alg  A hash algorithm: a value of type psa_algorithm_t such that
 *                      PSA_ALG_IS_HASH(hash_alg) is true. This includes PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a key policy.
 * 
 * @return  The corresponding RSA PKCS#1 v1.5 signature algorithm
 * 
 * @note    It is recommended to check the hash_alg for valid/supported algorithm before using 
 *          this macro
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN(hash_alg)     (PSA_ALG_RSA_PKCS1V15_SIGN_BASE | (USS_PSA_ALG_HASH_MASK & hash_alg))

/**
 * @brief   The raw RSA PKCS#1 v1.5 signature algorithm, without hashing
 *          This algorithm can be only used with the psa_sign_hash() and psa_verify_hash() functions
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN_RAW           PSA_ALG_RSA_PKCS1V15_SIGN_BASE 

/**
 * @brief   This macro serves as base for deriving other macros for 
 *          RSA PSS message signature scheme, with hashing
 */
#define PSA_ALG_RSA_PSS_BASE          ((psa_algorithm_t) 0x06000300)

/**
 * @brief   The RSA PSS message signature scheme, with hashing
 *          This algorithm can be used with both the message and hash signature functions
 * 
 * @param[in] hash_alg  A hash algorithm: a value of type psa_algorithm_t such that
 *                      PSA_ALG_IS_HASH(hash_alg) is true. This includes PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a key policy.
 * 
 * @return  The corresponding RSA PSS signature algorithm
 * 
 * @note    It is recommended to check the hash_alg for valid/supported algorithm before using 
 *          this macro
 */
#define PSA_ALG_RSA_PSS(hash_alg)     (PSA_ALG_RSA_PSS_BASE | (USS_PSA_ALG_HASH_MASK & hash_alg)) 

/**
 * @brief   This macro serves as base for deriving other macros for 
 *          RSA PSS message signature scheme, with hashing
 */
#define PSA_ALG_RSA_PSS_ANY_SALT_BASE          ((psa_algorithm_t) 0x06001300)

/**
 * @brief   The RSA PSS message signature scheme, with hashing
 *          This variant permits any salt length for signature verification
 *          This algorithm can be used with both the message and hash signature functions
 * 
 * @param[in] hash_alg  A hash algorithm: a value of type psa_algorithm_t such that
 *                      PSA_ALG_IS_HASH(hash_alg) is true. This includes PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a key policy.
 * 
 * @return  The corresponding RSA PSS signature algorithm
 * 
 * @note    It is recommended to check the hash_alg for valid/supported algorithm before using 
 *          this macro
 */
#define PSA_ALG_RSA_PSS_ANY_SALT(hash_alg)     (PSA_ALG_RSA_PSS_ANY_SALT_BASE | (USS_PSA_ALG_HASH_MASK & hash_alg)) 

/**
 * @brief   This macro serves as base for deriving other macros for 
 *          ECDSA signature scheme, with hashing
 */
#define PSA_ALG_ECDSA_BASE          ((psa_algorithm_t) 0x06000600)

/**
 * @brief   The randomized ECDSA signature scheme, with hashing
 *          This algorithm can be used with both the message and hash signature functions
 * 
 * @param[in] hash_alg  A hash algorithm: a value of type psa_algorithm_t such that
 *                      PSA_ALG_IS_HASH(hash_alg) is true. This includes PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a key policy.
 * 
 * @return  The corresponding randomized ECDSA signature algorithm
 * 
 * @note    It is recommended to check the hash_alg for valid/supported algorithm before using 
 *          this macro
 */
#define PSA_ALG_ECDSA(hash_alg)     (PSA_ALG_ECDSA_BASE | (USS_PSA_ALG_HASH_MASK & hash_alg)) 

/**
 * @brief   The randomized ECDSA signature scheme, without hashing
 *          This algorithm can be only used with the psa_sign_hash() and psa_verify_hash() functions
 *          This is the same signature scheme as PSA_ALG_ECDSA(), but without specifying a hash algorithm, and
 *          skipping the message hashing operation
 */
#define PSA_ALG_ECDSA_ANY           PSA_ALG_ECDSA_BASE 

/**
 * @brief   This macro serves as base for deriving other macros for 
 *          Deterministic ECDSA signature scheme, with hashing
 */
#define PSA_ALG_DETERMINISTIC_ECDSA_BASE          ((psa_algorithm_t) 0x06000700)

/**
 * @brief   Deterministic ECDSA signature scheme, with hashing
 *          This algorithm can be used with both the message and hash signature functions
 *          When based on the same hash algorithm, the verification operations for PSA_ALG_ECDSA and
 *          PSA_ALG_DETERMINISTIC_ECDSA are identical. A signature created using PSA_ALG_ECDSA can be verified
 *          with the same key using either PSA_ALG_ECDSA or PSA_ALG_DETERMINISTIC_ECDSA. Similarly, a signature
 *          created using PSA_ALG_DETERMINISTIC_ECDSA can be verified with the same key using either
 *          PSA_ALG_ECDSA or PSA_ALG_DETERMINISTIC_ECDSA
 * 
 * @param[in] hash_alg  A hash algorithm: a value of type psa_algorithm_t such that
 *                      PSA_ALG_IS_HASH(hash_alg) is true. This includes PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a key policy.
 * 
 * @return  The corresponding deterministic ECDSA signature algorithm
 * 
 * @note    It is recommended to check the hash_alg for valid/supported algorithm before using 
 *          this macro
 */
#define PSA_ALG_DETERMINISTIC_ECDSA(hash_alg)     (PSA_ALG_DETERMINISTIC_ECDSA_BASE | (USS_PSA_ALG_HASH_MASK & hash_alg)) 

/**
 * @brief   The Edwards-curve digital signature algorithm without prehashing (PureEdDSA), using standard parameters
 *          This algorithm can be only used with the psa_sign_message() and psa_verify_message() functions
 */
#define PSA_ALG_PURE_EDDSA                      ((psa_algorithm_t) 0x06000800) 


#define PSA_ALG_HASH_EDDSA_BASE                 ((psa_algorithm_t) 0x06000900)

/**
 * @brief   The Edwards-curve digital signature algorithm with prehashing (HashEdDSA), using the Edwards25519 curve
 *          This algorithm can be used with both the message and hash signature functions
 *          When used with psa_sign_hash() or psa_verify_hash(), the provided hash parameter is the SHA-512
 *          message digest
 */
#define PSA_ALG_ED25519PH                       ((psa_algorithm_t) 0x0600090B)

/**
 * @brief   The Edwards-curve digital signature algorithm with prehashing (HashEdDSA), using the Edwards448 curve
 *          This algorithm can be used with both the message and hash signature functions
 *          When used with psa_sign_hash() or psa_verify_hash(), the provided hash parameter is the truncated
 *          SHAKE256 message digest
 */
#define PSA_ALG_ED448PH                         ((psa_algorithm_t) 0x06000915)

/**
 * @brief           To check whether the specified algorithm is an asymmetric signature algorithm, also known as public-key signature
 *                  algorithm
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is an asymmetric signature algorithm, 0 otherwise. This macro can return either 0 or 1 if alg is not a
 *                  supported algorithm identifier
 */
#define PSA_ALG_IS_SIGN(alg)                    (((alg) & USS_PSA_ALG_MASK) == USS_PSA_ALG_TYPE_SIGN)

/**
 * @brief           To check whether the specified algorithm is a signature algorithm that can be used with psa_sign_message() and
 *                  psa_verify_message()
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is a signature algorithm that can be used to sign a message. 0 if alg is a signature algorithm that can
 *                  only be used to sign an already-calculated hash. 0 if alg is not a signature algorithm
 */
#define PSA_ALG_IS_SIGN_MESSAGE(alg)            (PSA_ALG_IS_SIGN(alg) && \
                                                (alg) != PSA_ALG_ECDSA_ANY && (alg) != PSA_ALG_RSA_PKCS1V15_SIGN_RAW)

/**
 * @brief           To check whether the specified algorithm is an RSA PKCS#1 v1.5 signature algorithm
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is an RSA PKCS#1 v1.5 signature algorithm, 0 otherwise. This macro can return either 0 or 1 if alg is not a
 *                  supported algorithm identifier
 */
#define PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)       (((alg) & ~USS_PSA_ALG_HASH_MASK) == PSA_ALG_RSA_PKCS1V15_SIGN_BASE)

/**
 * @brief           To check whether the specified algorithm is an RSA PSS signature algorithm that permits any salt length
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is an RSA PSS signature algorithm that permits any salt length, 0 otherwise. This macro can return either 0 
 *                  or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_RSA_PSS_ANY_SALT(alg)        (((alg) & ~USS_PSA_ALG_HASH_MASK) == PSA_ALG_RSA_PSS_ANY_SALT_BASE)

/**
 * @brief           To check whether the specified algorithm is an RSA PSS signature algorithm that requires the standard salt length
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is an RSA PSS signature algorithm that requires the standard salt length, 0 otherwise. This macro can return 
 *                  either 0 or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg)   (((alg) & ~USS_PSA_ALG_HASH_MASK) == PSA_ALG_RSA_PSS_BASE)

/**
 * @brief           To check whether the specified algorithm is an RSA PSS signature algorithm
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is an RSA PSS signature algorithm, 0 otherwise. This macro can return 
 *                  either 0 or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_RSA_PSS(alg)                 (PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg) || PSA_ALG_IS_RSA_PSS_ANY_SALT(alg))

/**
 * @brief           To check whether the specified algorithm is ECDSA
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is an ECDSA algorithm, 0 otherwise. This macro can return 
 *                  either 0 or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_ECDSA_DETERMINISTIC_FLAG        ((psa_algorithm_t) 0x00000100)
#define PSA_ALG_IS_ECDSA(alg)                   ((alg & ~(USS_PSA_ALG_HASH_MASK | PSA_ALG_ECDSA_DETERMINISTIC_FLAG)) == PSA_ALG_ECDSA_BASE)

/**
 * @brief           To check whether the specified algorithm is a deterministic ECDSA
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is a deterministic ECDSA algorithm, 0 otherwise. This macro can return 
 *                  either 0 or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_DETERMINISTIC_ECDSA(alg)     (((alg) & ~USS_PSA_ALG_HASH_MASK) == PSA_ALG_DETERMINISTIC_ECDSA_BASE)

/**
 * @brief           To check whether the specified algorithm is randomized ECDSA
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is a randomized ECDSA algorithm, 0 otherwise. This macro can return 
 *                  either 0 or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_RANDOMIZED_ECDSA(alg)        (((alg) & ~USS_PSA_ALG_HASH_MASK) == PSA_ALG_ECDSA_BASE)

/**
 * @brief           To check whether the specified algorithm is HashEdDSA
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is a HashEdDSA algorithm, 0 otherwise. This macro can return 
 *                  either 0 or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_HASH_EDDSA(alg)              (((alg) & ~USS_PSA_ALG_HASH_MASK) == PSA_ALG_HASH_EDDSA_BASE)

/**
 * @brief           To check whether the specified algorithm is a signature algorithm that can be used with psa_sign_hash() and
 *                  psa_verify_hash()
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is a signature algorithm that can be used to sign a hash. 0 if alg is a signature algorithm that can
 *                  only be used to sign a message. 0 if alg is not a signature algorithm. This macro can return either 0 or 1 if
 *                  alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_SIGN_HASH(alg)               (PSA_ALG_IS_RSA_PSS(alg) || PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||    \
                                                PSA_ALG_IS_ECDSA(alg) || PSA_ALG_IS_HASH_EDDSA(alg))

/**
 * @brief           To check whether the specified algorithm is a a hash-and-sign algorithm that signs exactly the hash value
 * 
 * @param[in] alg   An algorithm identifier: a value of type psa_algorithm_t
 * 
 * @return          1 if alg is a signature algorithm that signs exactly the hash value, 0 otherwise. This macro can return either 
 *                  0 or 1 if alg is not a supported algorithm identifier
 */
#define PSA_ALG_IS_HASH_AND_SIGN(alg)           (PSA_ALG_IS_SIGN_HASH(alg) && ((alg) & USS_PSA_ALG_HASH_MASK != 0))

/**
 * @brief       Helper macro to return minimum signature buffer size for 
 *              ECDSA algo
 * @param[in] curve_bits    Curve size in bits
 * 
 * @return      Size of the buffer
 */
#define USS_PSA_ECDSA_SIGNATURE_SIZE(curve_bits)    \
    (USS_PSA_BITS_TO_BYTES(curve_bits) * 2u)

/**
 * @brief Sufficient signature buffer size for psa_sign_message() and psa_sign_hash()
 * 
 * @param[in] key_type  An asymmetric key type. This can be a key pair type or a public key
 *                      type  
 * 
 * @param[in] key_bits  The size of the key in bits
 * 
 * @param[in] alg       The signature algorithm
 */
#define PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg) \
    (PSA_KEY_TYPE_IS_RSA(key_type) ? ((void) alg, USS_PSA_BITS_TO_BYTES(key_bits)) : \
    PSA_KEY_TYPE_IS_ECC(key_type) ? USS_PSA_ECDSA_SIGNATURE_SIZE(key_bits) : ((void) alg, 0u))

#ifdef USS_PSA_AVAILABLE_ALG_RSA
    #define PSA_SIGNATURE_MAX_SIZE      USS_PSA_BITS_TO_BYTES(USS_PSA_RSA_MAX_KEY_BITS)
#elif defined(USS_PSA_AVAILABLE_ALG_ECDSA)
    #define PSA_SIGNATURE_MAX_SIZE      USS_PSA_BITS_TO_BYTES(USS_PSA_ECC_MAX_CURVE_BITS)
#else   
    #define PSA_SIGNATURE_MAX_SIZE      0
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //_CRYPTO_SIGN_H_
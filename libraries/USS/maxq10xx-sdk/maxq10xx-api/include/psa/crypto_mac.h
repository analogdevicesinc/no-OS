/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/
#ifndef _CRYPTO_MAC_H_
#define _CRYPTO_MAC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include "crypto_config_def.h"
#include "crypto_common.h"
#include "crypto_typedefs.h"
#include "crypto_hash.h"
#include "crypto_cipher.h"

#define USS_PSA_ALG_HMAC_BASE                                   ((psa_algorithm_t) (USS_PSA_ALG_TYPE_MAC | 0x00800000))
#define USS_PSA_ALG_CIPHER_MAC_BASE                             ((psa_algorithm_t) (USS_PSA_ALG_TYPE_MAC | 0x00c00000))
#define USS_PSA_ALG_MAC_SUBCATEGORY_MASK                        ((psa_algorithm_t) 0x00c00000)

/**
 * @brief Macro to define an HMAC algorithm based on a hash algorithm
 *
 * @param hash_alg The hash algorithm to be used in the HMAC
 * @return The HMAC algorithm
 */
#define PSA_ALG_HMAC(hash_alg)                                  ((psa_algorithm_t)(USS_PSA_ALG_HMAC_BASE | (hash_alg & USS_PSA_ALG_HASH_MASK)))

/**
 * @brief Macro to define the CBC-MAC algorithm
 */
#define PSA_ALG_CBC_MAC                                         ((psa_algorithm_t)0x03c00100)

/**
 * @brief Macro to define the CMAC algorithm
 */
#define PSA_ALG_CMAC                                            ((psa_algorithm_t)0x03c00200)

/**
 * @brief Macro to define the truncated MAC algorithm
 */
#define USS_PSA_ALG_MAC_TRUNCATION_MASK                         ((psa_algorithm_t) 0x003f0000)
#define USS_PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG               ((psa_algorithm_t) 0x00008000)
#define USS_PSA_MAC_TRUNCATION_OFFSET                           16u

/**
 * @brief Macro to define a truncated MAC algorithm with a specific length
 *
 * @param mac_alg The MAC algorithm to be truncated
 * @param mac_length The desired length of the truncated MAC
 * @return The truncated MAC algorithm
 */
#define PSA_ALG_TRUNCATED_MAC(mac_alg, mac_length)              \
    (psa_algorithm_t)((mac_alg & ~(USS_PSA_ALG_MAC_TRUNCATION_MASK | USS_PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG)) |   \
     ((mac_length << USS_PSA_MAC_TRUNCATION_OFFSET) & USS_PSA_ALG_MAC_TRUNCATION_MASK))

/**
 * @brief Macro to define a full-length MAC algorithm
 *
 * This macro returns a MAC algorithm without any truncation or minimum length requirements
 *
 * @param mac_alg The MAC algorithm to be used
 * @return The full-length MAC algorithm
 */
#define PSA_ALG_FULL_LENGTH_MAC(mac_alg)                        \
    (psa_algorithm_t)((mac_alg) & ~(USS_PSA_ALG_MAC_TRUNCATION_MASK | USS_PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG))


/**
 * @brief Macro to define a MAC algorithm with at least a minimum length
 *
 * This macro returns a MAC algorithm that is truncated to a minimum length
 *
 * @param mac_alg The MAC algorithm to be truncated
 * @param min_mac_length The minimum length of the truncated MAC
 * @return The MAC algorithm with at least the minimum length
 */
#define PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(mac_alg, min_mac_length)   \
    (psa_algorithm_t)(PSA_ALG_TRUNCATED_MAC(mac_alg, min_mac_length)| USS_PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG)


/**
 * @brief Check if the given algorithm is a MAC algorithm
 *
 * This macro checks if the given algorithm belongs to the MAC category.
 *
 * @param alg The algorithm to check
 * @return True if the algorithm is a MAC algorithm, false otherwise
 */
#define PSA_ALG_IS_MAC(alg)                                     (((alg) & USS_PSA_ALG_MASK) == USS_PSA_ALG_TYPE_MAC)

/**
 * @brief Check if the given algorithm is an HMAC algorithm
 *
 * This macro checks if the given algorithm is based on the HMAC subcategory.
 *
 * @param alg The algorithm to check
 * @return True if the algorithm is an HMAC algorithm, false otherwise
 */
#define PSA_ALG_IS_HMAC(alg)                                    (((alg) & (USS_PSA_ALG_MASK | USS_PSA_ALG_MAC_SUBCATEGORY_MASK)) == \
                                                                USS_PSA_ALG_HMAC_BASE)

/**
 * @brief Check if the given algorithm is a block cipher MAC algorithm
 *
 * This macro checks if the given algorithm is based on the block cipher MAC subcategory.
 *
 * @param alg The algorithm to check
 * @return True if the algorithm is a block cipher MAC algorithm, false otherwise
 */
#define PSA_ALG_IS_BLOCK_CIPHER_MAC(alg)                        (((alg) & (USS_PSA_ALG_MASK | USS_PSA_ALG_MAC_SUBCATEGORY_MASK)) == \
                                                                USS_PSA_ALG_CIPHER_MAC_BASE)

#define USS_PSA_MAC_TRUNCATED_LENGTH(mac_alg)                   (((mac_alg) & USS_PSA_ALG_MAC_TRUNCATION_MASK) >> USS_PSA_MAC_TRUNCATION_OFFSET)
#define USS_PSA_ALG_HMAC_GET_HASH(hmac_alg)                     (USS_PSA_ALG_TYPE_HASH | ((hmac_alg) & USS_PSA_ALG_HASH_MASK))
/**
 * @brief Macro to calculate the length of the MAC based on the key type, key bits, and algorithm.
 *
 * This macro calculates the length of the MAC based on the provided key type, key bits, and algorithm.
 * If the algorithm specifies a truncated MAC, the length of the truncated MAC is returned.
 * If the algorithm is an HMAC algorithm, the length of the corresponding hash algorithm is returned.
 * If the algorithm is a block cipher MAC algorithm, the block length of the key type is returned.
 * If none of the above conditions are met, 0 is returned.
 *
 * @param key_type The type of the key
 * @param key_bits The number of bits in the key
 * @param alg The algorithm to calculate the MAC length for
 * @return The length of the MAC
 */
#define PSA_MAC_LENGTH(key_type, key_bits, alg)                 ((alg) & USS_PSA_ALG_MAC_TRUNCATION_MASK ? USS_PSA_MAC_TRUNCATED_LENGTH(alg) :  \
                                                                PSA_ALG_IS_HMAC(alg) ? PSA_HASH_LENGTH(USS_PSA_ALG_HMAC_GET_HASH(alg)) :        \
                                                                PSA_ALG_IS_BLOCK_CIPHER_MAC(alg) ? PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) :    \
                                                                ((void) (key_type), (void) (key_bits), 0u))

/**
 * @brief This macro returns a suitable initializer for a MAC operation object of type psa_mac_operation_t
 * 
 */
#define PSA_MAC_OPERATION_INIT                                  {0}

#define PSA_MAC_MAX_SIZE                                        PSA_HASH_MAX_SIZE

#define USS_MAC_VERIFY_FLAG                                     0x01u
#define USS_MAC_COMPUTE_FLAG                                    0x02u

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //_CRYPTO_MAC_H_

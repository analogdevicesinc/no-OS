/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef _CRYPTO_AEAD_H_
#define _CRYPTO_AEAD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include "crypto_common.h"
#include "crypto_cipher.h"
#include "MXQ_API.h"

#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
#define PSA_ALG_CCM                                     ((psa_algorithm_t)0x05500100)
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM

#ifdef USS_PSA_AVAILABLE_ALG_AES_GCM
#define PSA_ALG_GCM                                     ((psa_algorithm_t)0x05500200)                
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM                  

#ifdef USS_PSA_AVAILABLE_ALG_CHACHA20_POLY1305
#define PSA_ALG_CHACHA20_POLY1305                       ((psa_algorithm_t)0x05100500)
#define PSA_ALG_XCHACHA20_POLY1305                      ((psa_algorithm_t)0x05100600)
#endif //USS_PSA_AVAILABLE_ALG_CHACHA20_POLY1305

#define USS_PSA_ALG_AEAD_TAG_LENGTH_MASK                ((psa_algorithm_t)0x003f0000)
#define USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG         ((psa_algorithm_t)0x00008000)
#define USS_PSA_AEAD_TAG_LENGTH_OFFSET                  16u

/**
 * \brief Checks if the given algorithm is an AEAD algorithm.
 *
 * \param alg The algorithm to check.
 *
 * \return 1 if the algorithm is an AEAD algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_AEAD(alg)    ((alg & USS_PSA_ALG_MASK) == USS_PSA_ALG_TYPE_AEAD)

/**
 * \brief Creates an AEAD algorithm with a shortened tag length.
 *
 * \param aead_alg The base AEAD algorithm.
 * \param tag_length The desired tag length.
 *
 * \return The AEAD algorithm with the specified tag length.
 */
#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length)           \
    (((aead_alg) & ~(USS_PSA_ALG_AEAD_TAG_LENGTH_MASK |                 \
                     USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG)) |        \
     ((tag_length << USS_PSA_AEAD_TAG_LENGTH_OFFSET) &                  \
        USS_PSA_ALG_AEAD_TAG_LENGTH_MASK))

/**
 * \brief Creates an AEAD algorithm with the default tag length.
 *
 * \param aead_alg The AEAD algorithm for which needs to be transformed.
 *
 * \return The AEAD algorithm with the default tag length.
 */
#define PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(aead_alg) \
    (((aead_alg & ~(USS_PSA_ALG_AEAD_TAG_LENGTH_MASK | USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG)) == 0x05400100) ? PSA_ALG_CCM : \
    ((aead_alg & ~(USS_PSA_ALG_AEAD_TAG_LENGTH_MASK | USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG)) == 0x05400200) ? PSA_ALG_GCM : \
    PSA_ALG_NONE)

/**
 * \brief Creates an AEAD algorithm with at least the specified tag length.
 *
 * \param aead_alg The base AEAD algorithm.
 * \param min_tag_length The minimum desired tag length.
 *
 * \return The AEAD algorithm with at least the specified tag length.
 */
#define PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(aead_alg, min_tag_length) \
    (PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, min_tag_length) |            \
     USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG)

#define USS_PSA_ALG_AEAD_FROM_BLOCK_FLAG                    ((psa_algorithm_t) 0x00400000)

/**
 * \brief Checks if two AEAD algorithms are equal.
 *
 * \param aead_alg_1 The first AEAD algorithm.
 * \param aead_alg_2 The second AEAD algorithm.
 *
 * \return 1 if the AEAD algorithms are equal, 0 otherwise.
 */
#define USS_PSA_ALG_AEAD_EQUAL(aead_alg_1, aead_alg_2) \
    (!(((aead_alg_1) ^ (aead_alg_2)) & \
       ~(USS_PSA_ALG_AEAD_TAG_LENGTH_MASK | USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG)))

/**
 * @brief Checks if the specified algorithm is an AEAD on a block cipher.
 *
 * This macro checks if the given algorithm is an AEAD on a block cipher by performing a bitwise comparison.
 * It checks if the algorithm is of type AEAD and has the AEAD_FROM_BLOCK_FLAG set.
 *
 * @param alg The algorithm to check.
 * @return 1 if the algorithm is an AEAD on a block cipher, 0 otherwise.
 */
#define PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg)    \
    (((alg) & (USS_PSA_ALG_MASK | USS_PSA_ALG_AEAD_FROM_BLOCK_FLAG)) == \
     (USS_PSA_ALG_TYPE_AEAD | USS_PSA_ALG_AEAD_FROM_BLOCK_FLAG))

/**
 * \brief Gets the tag length of an AEAD algorithm.
 *
 * \param aead_alg The AEAD algorithm.
 *
 * \return The tag length of the AEAD algorithm.
 */
#define USS_PSA_ALG_AEAD_GET_TAG_LENGTH(aead_alg)                           \
    (((aead_alg) & USS_PSA_ALG_AEAD_TAG_LENGTH_MASK) >>                     \
     USS_PSA_AEAD_TAG_LENGTH_OFFSET)

/**
 * \brief Gets the nonce length for a given key type and AEAD algorithm.
 *
 * \param key_type The key type.
 * \param alg The AEAD algorithm.
 *
 * \return The nonce length.
 */
#define PSA_AEAD_NONCE_LENGTH(key_type, alg) \
    (PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) == 16u ? \
     USS_PSA_ALG_AEAD_EQUAL(alg, PSA_ALG_CCM) ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE : \
     USS_PSA_ALG_AEAD_EQUAL(alg, PSA_ALG_GCM) ? USS_PSA_AEAD_GCM_NONCE_SIZE : \
     0u \
     : 0u)

#define PSA_AEAD_NONCE_MAX_SIZE     USS_PSA_AEAD_NONCE_MAX_SIZE

/**
 * \brief Gets the output size of an AEAD encryption operation.
 *
 * \param key_type The key type.
 * \param alg The AEAD algorithm.
 * \param plaintext_length The length of the plaintext.
 *
 * \return The output size of the AEAD encryption operation.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, plaintext_length) \
    (PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 ?                      \
     (plaintext_length) + USS_PSA_ALG_AEAD_GET_TAG_LENGTH(alg) :          \
     0u)

/**
 * \brief Gets the maximum output size of an AEAD encryption operation.
 *
 * \param plaintext_length The length of the plaintext.
 *
 * \return The maximum output size of the AEAD encryption operation.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(plaintext_length)          \
    ((plaintext_length) + USS_PSA_AEAD_TAG_MAX_SIZE)

/**
 * \brief Gets the output size of an AEAD decryption operation.
 *
 * \param key_type The key type.
 * \param alg The AEAD algorithm.
 * \param ciphertext_length The length of the ciphertext.
 *
 * \return The output size of the AEAD decryption operation.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg, ciphertext_length) \
    (PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 &&                      \
     (ciphertext_length) > USS_PSA_ALG_AEAD_GET_TAG_LENGTH(alg) ?      \
     (ciphertext_length) - USS_PSA_ALG_AEAD_GET_TAG_LENGTH(alg) :      \
     0u)

/**
 * \brief Gets the maximum output size of an AEAD decryption operation.
 *
 * \param ciphertext_length The length of the ciphertext.
 *
 * \return The maximum output size of the AEAD decryption operation.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(ciphertext_length)     (ciphertext_length)

/**
 * \brief Gets the output size of an AEAD update operation.
 *
 * \param key_type The key type.
 * \param alg The AEAD algorithm.
 * \param input_length The length of the input.
 *
 * \return The output size of the AEAD update operation.
 */
#define PSA_AEAD_UPDATE_OUTPUT_SIZE(key_type, alg, input_length)            \
    (PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 ?                                             \
     PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) ?                                              \
     USS_PSA_ROUND_UP_TO_MULTIPLE(PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type), input_length) : \
     (input_length) : \
     0u)

/**
 * \brief Gets the maximum output size of an AEAD update operation.
 *
 * \param input_length The length of the input.
 *
 * \return The maximum output size of the AEAD update operation.
 */
#define PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(input_length) \
    (USS_PSA_ROUND_UP_TO_MULTIPLE(PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE, input_length))

/**
 * \brief Gets the output size of an AEAD finish operation.
 *
 * \param key_type The key type.
 * \param alg The AEAD algorithm.
 *
 * \return The output size of the AEAD finish operation.
 */
#define PSA_AEAD_FINISH_OUTPUT_SIZE(key_type, alg) \
    (PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 &&  \
     PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) ?    \
     PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) : \
     0u)

#define PSA_AEAD_FINISH_OUTPUT_MAX_SIZE     PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE

/**
 * \brief Gets the tag length of an AEAD algorithm.
 *
 * \param key_type The key type.
 * \param key_bits The key size in bits.
 * \param alg The AEAD algorithm.
 *
 * \return The tag length of the AEAD algorithm.
 */
#define PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg) \
     PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 ?     \
     USS_PSA_ALG_AEAD_GET_TAG_LENGTH(alg) :          \
     0u  

#define PSA_AEAD_TAG_MAX_SIZE               USS_PSA_AEAD_TAG_MAX_SIZE

/**
 * \brief Gets the output size of an AEAD verify operation.
 *
 * \param key_type The key type.
 * \param alg The AEAD algorithm.
 *
 * \return The output size of the AEAD verify operation.
 */
#define PSA_AEAD_VERIFY_OUTPUT_SIZE(key_type, alg)  \
    (PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 &&  \
     PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) ?    \
     PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) : \
     0u)

#define USS_PSA_ALG_AEAD_IS_WILDCARD(alg)   \
     ((alg & USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG) != 0)

#define PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE     PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE

/**
 * \brief Initializes an AEAD operation.
 *
 * \return The initialized AEAD operation.
 */
#define PSA_AEAD_OPERATION_INIT   {0}

#if defined(USS_PSA_AVAILABLE_ALG_AES_CCM) || defined(USS_PSA_AVAILABLE_ALG_AES_GCM)
    #define USS_PSA_AEAD_AAD_MAX_SIZE           2048u
    #define USS_PSA_AEAD_MSG_MAX_SIZE           UINT16_MAX
    #define USS_PSA_AEAD_CCM_MIN_NONCE_SIZE     7u
    #define USS_PSA_AEAD_CCM_MAX_NONCE_SIZE     13u
    #define USS_PSA_AEAD_GCM_NONCE_SIZE         12u                    
    #define USS_PSA_AEAD_NONCE_MAX_SIZE         USS_PSA_AEAD_CCM_MAX_NONCE_SIZE > USS_PSA_AEAD_GCM_NONCE_SIZE \
                                                ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE : USS_PSA_AEAD_GCM_NONCE_SIZE
    #if defined(USS_PSA_EMULATION)
        #define USS_PSA_AEAD_TAG_OVERHEAD           0xDEu
        #define USS_PSA_AEAD_TAG_OVERHEAD_SZ        0x1u
    #else
        #define USS_PSA_AEAD_TAG_OVERHEAD           0x0u
        #define USS_PSA_AEAD_TAG_OVERHEAD_SZ        0x0u
    #endif //USS_PSA_EMULATION
    #define USS_PSA_AEAD_TAG_MAX_SIZE           16u 
    #define USS_PSA_AEAD_TAG_LEN_4              4u
    #define USS_PSA_AEAD_TAG_LEN_6              6u
    #define USS_PSA_AEAD_TAG_LEN_8              8u
    #define USS_PSA_AEAD_TAG_LEN_10             10u
    #define USS_PSA_AEAD_TAG_LEN_12             12u
    #define USS_PSA_AEAD_TAG_LEN_13             13u
    #define USS_PSA_AEAD_TAG_LEN_14             14u
    #define USS_PSA_AEAD_TAG_LEN_15             15u
    #define USS_PSA_AEAD_TAG_LEN_16             16u
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //_CRYPTO_AEAD_H_

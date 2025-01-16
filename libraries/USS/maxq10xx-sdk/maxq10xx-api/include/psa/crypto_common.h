/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef _CRYPTO_COMMON_H_
#define _CRYPTO_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include "crypto_typedefs.h"
/**
 * @brief An invalid algorithm ID
 * 
 */
#define PSA_ALG_NONE        ((psa_algorithm_t)0)
#define PSA_ALG_FFDH ((psa_algorithm_t)0x09010000)
#define PSA_ALG_ECDH ((psa_algorithm_t)0x09020000)
#define PSA_ALG_PBKDF2_AES_CMAC_PRF_128 ((psa_algorithm_t)0x08800200)
#define PSA_ALG_STREAM_CIPHER ((psa_algorithm_t)0x04800100)
#define PSA_ALG_CTR ((psa_algorithm_t)0x04c01000)
#define PSA_ALG_CFB ((psa_algorithm_t)0x04c01100)
#define PSA_ALG_OFB ((psa_algorithm_t)0x04c01200)
#define PSA_ALG_XTS ((psa_algorithm_t)0x0440ff00)
#define PSA_ALG_ECB_NO_PADDING ((psa_algorithm_t)0x04404400)
#define PSA_ALG_CBC_NO_PADDING ((psa_algorithm_t)0x04404000)
#define PSA_ALG_CBC_PKCS7 ((psa_algorithm_t)0x04404100)
#define PSA_ALG_PBKDF2_AES_CMAC_PRF_128 ((psa_algorithm_t)0x08800200)
#define PSA_ALG_RSA_PKCS1V15_CRYPT ((psa_algorithm_t)0x07000200)


#define PSA_ALG_HKDF(hash_alg) \
    ((psa_algorithm_t) (0x08000100 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_HKDF_EXPAND(hash_alg) \
    ((psa_algorithm_t) (0x08000500 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_HKDF_EXTRACT(hash_alg) \
    ((psa_algorithm_t) (0x08000400 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) \
    (((alg) & 0x7f000000) == 0x07000000)

#define PSA_ALG_IS_CIPHER(alg) \
    (((alg) & 0x7f000000) == 0x04000000)

#define PSA_ALG_IS_ECDH(alg) \
    (((alg) & 0x7fff0000) == 0x09020000)

#define PSA_ALG_IS_FFDH(alg) \
    (((alg) & 0x7fff0000) == 0x09010000)

#define PSA_ALG_IS_HKDF(alg) \
    (((alg) & ~0x000000ff) == 0x08000100)

#define PSA_ALG_IS_HKDF_EXPAND(alg) \
    (((alg) & ~0x000000ff) == 0x08000500)

#define PSA_ALG_IS_HKDF_EXTRACT(alg) \
    (((alg) & ~0x000000ff) == 0x08000400)

#define PSA_ALG_IS_KEY_AGREEMENT(alg) \
    (((alg) & 0x7f000000) == 0x09000000)

#define PSA_ALG_IS_KEY_DERIVATION(alg) \
    (((alg) & 0x7f000000) == 0x08000000)

#define PSA_ALG_IS_KEY_DERIVATION_STRETCHING(alg) \
    (((alg) & 0x7f800000) == 0x08800000)

#define PSA_ALG_IS_PBKDF2_HMAC(alg) \
    (((alg) & ~0x000000ff) == 0x08800100)

#define PSA_ALG_IS_RAW_KEY_AGREEMENT(alg) \
    (((alg) & 0x7f00ffff) == 0x09000000)

#define PSA_ALG_IS_RSA_OAEP(alg) \
    (((alg) & ~0x000000ff) == 0x07000300)

#define PSA_ALG_IS_STREAM_CIPHER(alg) \
    (((alg) & 0x7f800000) == 0x04800000)

#define PSA_ALG_IS_TLS12_PRF(alg) \
    (((alg) & ~0x000000ff) == 0x08000200)

#define PSA_ALG_IS_TLS12_PSK_TO_MS(alg) \
    (((alg) & ~0x000000ff) == 0x08000300)

#define PSA_ALG_IS_WILDCARD(alg) \
    ((PSA_ALG_GET_HASH(alg) == PSA_ALG_ANY_HASH) || \
     (((alg) & 0x7f008000) == 0x03008000) || \
     (((alg) & 0x7f008000) == 0x05008000))

#define PSA_ALG_KEY_AGREEMENT(ka_alg, kdf_alg) \
    ((ka_alg) | (kdf_alg))

#define PSA_ALG_KEY_AGREEMENT_GET_BASE(alg) \
    ((psa_algorithm_t)((alg) & 0xffff0000))

#define PSA_ALG_KEY_AGREEMENT_GET_KDF(alg) \
    ((psa_algorithm_t)((alg) & 0xfe00ffff))

#define PSA_ALG_PBKDF2_HMAC(hash_alg) \
    ((psa_algorithm_t)(0x08800100 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_RSA_OAEP(hash_alg) \
    ((psa_algorithm_t)(0x07000300 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_TLS12_PRF(hash_alg) \
    ((psa_algorithm_t) (0x08000200 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_TLS12_PSK_TO_MS(hash_alg) \
    ((psa_algorithm_t) (0x08000300 | ((hash_alg) & 0x000000ff)))


/*** Implementation specific macros ***/

#define USS_PSA_ALG_MASK                        ((psa_algorithm_t) 0x7f000000)
#define USS_PSA_ALG_TYPE_HASH                   ((psa_algorithm_t) 0x02000000)
#define USS_PSA_ALG_TYPE_MAC                    ((psa_algorithm_t) 0x03000000)
#define USS_PSA_ALG_TYPE_CIPHER                 ((psa_algorithm_t) 0x04000000)
#define USS_PSA_ALG_TYPE_AEAD                   ((psa_algorithm_t) 0x05000000)
#define USS_PSA_ALG_TYPE_SIGN                   ((psa_algorithm_t) 0x06000000)
#define USS_PSA_ALG_TYPE_ASYMMETRIC_ENCRYPTION  ((psa_algorithm_t) 0x07000000)
#define USS_PSA_ALG_TYPE_KEY_DERIVATION         ((psa_algorithm_t) 0x08000000)
#define USS_PSA_ALG_TYPE_KEY_AGREEMENT          ((psa_algorithm_t) 0x09000000)

#define USS_PSA_ALG_HASH_MASK                   ((psa_algorithm_t) 0x000000ff)

#define USS_PSA_KEY_TYPE_MASK                   ((psa_key_type_t) 0x7000)
#define USS_PSA_KEY_TYPE_RAW_MASK               ((psa_key_type_t) 0x1000)
#define USS_PSA_KEY_TYPE_SYMMETRIC_MASK         ((psa_key_type_t) 0x2000)
#define USS_PSA_KEY_TYPE_PUBLIC_KEY_MASK        ((psa_key_type_t) 0x4000)
#define USS_PSA_KEY_TYPE_KEY_PAIR_MASK          ((psa_key_type_t) 0x7000)

#define USS_PSA_BITS_TO_BYTES(bits)             ((bits + 7u) / 8u)
#define USS_PSA_BYTES_TO_BITS(bytes)            (bytes * 8u)
#define USS_PSA_ROUND_UP_TO_MULTIPLE(block_size, length) \
    (((length + (block_size - 1)) / block_size) * block_size)

#define USS_ENCRYPT     1u
#define USS_DECRYPT     0u

#define USS_TRUE        1u
#define USS_FALSE       0u


/**
 * @brief Converts a PSA key type to a MAXQ key type.
 *
 * This function takes a PSA key type as input and converts it to the corresponding
 * MAXQ key type.
 *
 * @param[in] psa_key_type The PSA key type to be converted.
 *
 * @return The corresponding MAXQ key type.
 */
mxq_keytype_id_t uss_psa_to_maxq_key_type(psa_key_type_t psa_key_type);

/**
 * @brief Converts a PSA key lifetime to a MAXQ key lifetime.
 *
 * This function takes a PSA key lifetime as input and converts it to the corresponding
 * MAXQ key lifetime.
 *
 * @param[in] psa_key_lifetime The PSA key lifetime to be converted.
 *
 * @return The corresponding MAXQ key lifetime.
 */
mxq_object_prop_t uss_psa_to_maxq_key_lifetime(psa_key_lifetime_t psa_key_lifetime);

/**
 * @brief Converts a PSA key usage to a MAXQ key usage.
 *
 * This function takes a PSA key usage as input and converts it to the corresponding
 * MAXQ key usage.
 *
 * @param[in] psa_key_use The PSA key usage to be converted.
 *
 * @return The corresponding MAXQ key usage.
 */
mxq_keyuse_t uss_psa_to_maxq_keyuse(psa_key_usage_t psa_key_use);

/**
 * @brief Converts a PSA key type to a MAXQ object type.
 *
 * This function takes a PSA key type as input and converts it to the corresponding
 * MAXQ object type.
 *
 * @param[in] psa_key_type The PSA key type to be converted.
 *
 * @return The corresponding MAXQ object type.
 */
mxq_objecttype_t uss_psa_to_maxq_object_type(psa_key_type_t psa_key_type);

/**
 * @brief Converts a MAXQ algorithm ID to a PSA algorithm ID.
 *
 * This function takes a MAXQ algorithm ID as input and converts it to the corresponding
 * PSA algorithm ID.
 *
 * @param[in] mxq_algo_type The MAXQ algorithm ID to be converted.
 *
 * @return The corresponding PSA algorithm ID.
 */
psa_algorithm_t uss_maxq_to_psa_algo_id(mxq_algo_id_t mxq_algo_type);

/**
 * @brief Converts a MAXQ key usage to a PSA key usage.
 *
 * This function takes a MAXQ key usage as input and converts it to the corresponding
 * PSA key usage.
 *
 * @param[in] mxq_key_use The MAXQ key usage to be converted.
 *
 * @return The corresponding PSA key usage.
 */
psa_key_usage_t uss_maxq_to_psa_keyuse(mxq_keyuse_t mxq_key_use);

/**
 * @brief Converts a MAXQ key type, along with additional parameters and object type, to a PSA key type.
 *
 * This function takes a MAXQ key type, additional parameters, and the MAXQ object type as input,
 * and converts them to the corresponding PSA key type.
 *
 * @param[in] mxq_key_type The MAXQ key type to be converted.
 * @param[in] params Additional parameters for the conversion.
 * @param[in] mxq_object_type The MAXQ object type to be considered in the conversion.
 *
 * @return The corresponding PSA key type.
 */
psa_key_type_t uss_maxq_to_psa_key_type(mxq_keytype_id_t mxq_key_type, mxq_keyparam_id_t params, mxq_objecttype_t mxq_object_type);

/**
 * @brief       Helper function that translates the MAXQ status codes to PSA 
 *              status codes
 * 
 * @param[in]   maxqCode Status code from underlying MAXQ driver code
 * 
 * @return psa_status_t Corresponding PSA status code
 */
psa_status_t uss_to_psa_error_code(mxq_err_t maxqCode);

 /**
 * @brief       Helper function that translates the PSA algo IDs to MAXQ 
 *              algo IDs
 * 
 * @param[in] psa_algo_type PSA algo ID 
 * @return mxq_algo_id_t MAXQ algo ID
 */
mxq_algo_id_t uss_psa_to_maxq_algo_id(psa_algorithm_t psa_algo_type);

/**
 * @brief       Helper function that translates the PSA MD algos to MAXQ algos
 *              during signing (verifying) operation
 * 
 * @param[in] psa_algo PSA MD algo ID 
 * 
 * @param[in] isMsg If this function is called from message signing/verifying 
 *                  or hash signing/verifying
 *                  True: For message
 *                  False: For hash
 * 
 * @return mxq_algo_id_t MAXQ algo ID 
 * 
 * @note    Only invoked from psa_sign_* or psa_verify_*
 */
mxq_algo_id_t uss_sign_algo_from_psa(psa_algorithm_t psa_algo, bool isMsg);

/**
 * @brief           Helper function to check if the key has required algorithm
 *                  set for performing the requested operation 
 * @param psa_algo  PSA algo
 * @param attr      Attribute of key, which will be used to perform the operation
 * @return true     If key can be used for the operation
 * @return false    If not
 */
bool check_key_policy(psa_algorithm_t psa_algo, psa_key_attributes_t *attr);

/**
 * @brief         Helper function to retrieve data from a pointer to memory
 *
 * @param[in]     psrc   Pointer to input buffer from which data will be retrieved
 * @param[in,out] offset Offset from begining of psrc where data is located
 *
 * @return Two bytes of data located at specified offset within psrc
 */
mxq_u2 get_u2(mxq_u1* psrc, mxq_length* offset);

/**
 * @brief         Helper function to retrieve data from a pointer to memory
 *
 * @param[in]     psrc   Pointer to input buffer from which data will be retrieved
 * @param[in,out] offset Offset from begining of psrc where data is located
 *
 * @return One byte of data located at specified offset within psrc
 */
mxq_u1 get_u1(mxq_u1* psrc, mxq_length* offset);

/**
 * @brief           Helper function to check if psa_crypto_init() was invoked
 *                  and if it successfully returned
 * @return true 
 * @return false 
 */

bool ifCryptoInitDone(void);

/**
 * @brief Constant time memcmp
 * 
 * @param[in] s1        First buffer
 * @param[in] s2        Second buffer
 * @param[in] n         Size of the buffer
 *
 * @return 0        If the buffers are equal 
 */
uint8_t ct_memcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief           Helper function to check if the tag length is valid for the 
 *                  given algorithm
 * @param[in] psa_algo  PSA AEAD algo
 *
 * @return true     If the tag length is valid
 * @return false    If not
 */
bool check_aead_tag_len(psa_algorithm_t psa_algo);

/**
 * @brief Cleans up a buffer by setting its contents to zero.
 * 
 * @param[in,out] buffer The buffer to be cleaned up.
 * @param[in]     buffer_size The size of the buffer.
 */
static inline void buffer_cleanup(uint8_t * buffer, size_t buffer_size){
    if(buffer != NULL){
        memset(buffer, 0x0, buffer_size);
    }
}


#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //_CRYPTO_COMMON_H_

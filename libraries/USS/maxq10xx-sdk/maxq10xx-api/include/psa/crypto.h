/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

/**
 * This file is a reference template for implementation of the
 * PSA Certified Crypto API v1.1
 */

#ifndef PSA_CRYPTO_H
#define PSA_CRYPTO_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "crypto_config_def.h"  // Include the configuration file as the fist include to handle function-mangling and other configurations
#include "crypto_errors.h"
#include "crypto_typedefs.h"
#include "crypto_hash.h"
#include "crypto_sign.h"
#include "key_management.h"
#include "crypto_key_derivation.h"
#include "crypto_cipher.h"
#include "crypto_aead.h"
#include "crypto_mac.h"

#define PSA_CRYPTO_API_VERSION_MAJOR 1
#define PSA_CRYPTO_API_VERSION_MINOR 1

/**
 * @brief   Crypto Library initialization. It is recommended that applications call 
 *          this function before calling any other function in this module
 * 
 * @return psa_status_t 
 */
psa_status_t psa_crypto_init(void);

/**
 * @brief Return an initial value for a key attribute object.
 * 
 * @return psa_key_attributes_t 
 */
psa_key_attributes_t psa_key_attributes_init(void);

/**
 * @brief Retrieve the attributes of a key.
 * 
 * @param key           Identifier of the key to query.
 * @param attributes    On entry, *attributes must be in a valid state. On successful return, it contains the attributes of the key. 
 *                      On failure, it is equivalent to a freshly-initialized attribute object.
 * 
 * @return psa_status_t PSA_SUCCESS                 Success. attributes contains the attributes of the key.
 *                      PSA_ERROR_BAD_STATE         The library requires initializing by a call to psa_crypto_init().
 *                      PSA_ERROR_INVALID_HANDLE    key is not a valid key identifier.
 */
psa_status_t psa_get_key_attributes(psa_key_id_t key,
                                    psa_key_attributes_t * attributes);

/**
 * @brief Reset a key attribute object to a freshly initialized state.
 * 
 * @param attributes The attribute object to reset.
 */
void psa_reset_key_attributes(psa_key_attributes_t * attributes);

/**
 * @brief Declare the type of a key.
 * 
 * @param attributes The attribute object to write to.
 * @param type The key type to write. If this is PSA_KEY_TYPE_NONE, the key type in attributes becomes unspecified.
 */
void psa_set_key_type(psa_key_attributes_t * attributes,
                      psa_key_type_t type);

/**
 * @brief Retrieve the key type from key attributes.
 * 
 * @param attributes The key attribute object to query.
 * @return psa_key_type_t The key type stored in the attribute object.
 */
psa_key_type_t psa_get_key_type(const psa_key_attributes_t * attributes);

/**
 * @brief Retrieve the key size from key attributes.
 * 
 * @param attributes The key attribute object to query.
 * @return size_t The key size stored in the attribute object, in bits.
 */
size_t psa_get_key_bits(const psa_key_attributes_t * attributes);

/**
 * @brief Declare the size of a key.
 * 
 * @param attributes The attribute object to write to.
 * @param bits The key size in bits. If this is 0, the key size in attributes becomes unspecified. Keys of size 0 are not supported.
 */
void psa_set_key_bits(psa_key_attributes_t * attributes,
                      size_t bits);

/**
 * @brief Set the location of a persistent key.
 * @note We provide a vendor specific lifetime USS_PSA_KEY_LIFETIME_WIPE_ON_TAMPER 
 *      which creates objects that are erased on tamper detection.
 * @param attributes The attribute object to write to.
 * @param lifetime The lifetime for the key. If this is PSA_KEY_LIFETIME_VOLATILE, the key will be volatile, 
 *                  and the key identifier attribute is reset to PSA_KEY_ID_NULL.
 */
void psa_set_key_lifetime(psa_key_attributes_t * attributes,
                          psa_key_lifetime_t lifetime);

/**
 * @brief Retrieve the lifetime from key attributes.
 * 
 * @param attributes The key attribute object to query.
 * @return psa_key_lifetime_t The lifetime value stored in the attribute object.
 */
psa_key_lifetime_t psa_get_key_lifetime(const psa_key_attributes_t * attributes);

/**
 * @brief Declare a key as persistent and set its key identifier.
 * @note PSA specification defines the  PSA_KEY_ID_USER_MIN as 0x00000001 and 
 *   PSA_KEY_ID_USER_MAX as 0x3fffffff but we support only two byte key_id,
 *   limiting the value of the key ID that can be created.
 * 
 * @param attributes The attribute object to write to.
 * @param id The persistent identifier for the key.
 */
void psa_set_key_id(psa_key_attributes_t * attributes,
                    psa_key_id_t id);

/**
 * @brief Retrieve the key identifier from key attributes.
 * 
 * @param attributes The key attribute object to query.
 * @return psa_key_id_t The persistent identifier stored in the attribute object. 
 *                      This value is unspecified if the attribute object declares the key as volatile.
 */
psa_key_id_t psa_get_key_id(const psa_key_attributes_t * attributes);

/**
 * @brief Declare the permitted-algorithm policy for a key.
 * 
 * @param attributes The attribute object to write to.
 * @param alg The permitted algorithm to write.
 */
void psa_set_key_algorithm(psa_key_attributes_t * attributes,
                           psa_algorithm_t alg);

/**
 * @brief Retrieve the permitted-algorithm policy from key attributes.
 * 
 * @param attributes The key attribute object to query.
 * @return psa_algorithm_t The algorithm stored in the attribute object.
 */
psa_algorithm_t psa_get_key_algorithm(const psa_key_attributes_t * attributes);

/**
 * @brief Declare usage flags for a key.
 * 
 * @param attributes The attribute object to write to.
 * @param usage_flags The usage flags to write.
 */
void psa_set_key_usage_flags(psa_key_attributes_t * attributes,
                             psa_key_usage_t usage_flags);

/**
 * @brief Retrieve the usage flags from key attributes.
 * 
 * @param attributes The key attribute object to query.
 * @return psa_key_usage_t The usage flags stored in the attribute object.
 */
psa_key_usage_t psa_get_key_usage_flags(const psa_key_attributes_t * attributes);

/**
 * @brief Import a key in binary format.
 * @note PSA specification defines the  PSA_KEY_ID_USER_MIN as 0x00000001 and 
 *   PSA_KEY_ID_USER_MAX as 0x3fffffff but we support only two byte key_id,
 *   limiting the value of the key ID that can be created.
 * 
 * @param attributes The attributes for the new key. This function uses the attributes as follows:
 *                     1. The key type is required, and determines how the data buffer is interpreted.
 *                     2. The key size is always determined from the data buffer. 
 *                          If the key size in attributes is nonzero, it must be equal to the size determined from data.
 *                     3. The key permitted-algorithm policy is required for keys that will be used for a cryptographic operation, see Permitted algorithms.
 *                     4. The key usage flags define what operations are permitted with the key, see Key usage flags.
 *                     5. The key lifetime and identifier are required for a persistent key.
 * 
 * @param data Buffer containing the key data. The content of this buffer is interpreted according to the type declared in attributes. 
 *             All implementations must support at least the format described in Key formats for the chosen type. 
 *             Implementations can support other formats, but be conservative in interpreting the key data: 
 *             it is recommended that implementations reject content if it might be erroneous, for example, if it is the wrong type or is truncated.
 * 
 * @param data_length Size of the data buffer in bytes.
 * @param key On success, an identifier for the newly created key. PSA_KEY_ID_NULL on failure.
 * @return psa_status_t 
 */
psa_status_t psa_import_key(const psa_key_attributes_t * attributes,
                            const uint8_t * data,
                            size_t data_length,
                            psa_key_id_t * key);

/**
 * @brief Generate a key or key pair.
 * @note PSA specification defines the  PSA_KEY_ID_USER_MIN as 0x00000001 and 
 *   PSA_KEY_ID_USER_MAX as 0x3fffffff but we support only two byte key_id,
 *   limiting the value of the key ID that can be created.
 * 
 * @param attributes The attributes for the new key. This function uses the attributes as follows:
 *                   1. The key type is required. It cannot be an asymmetric public key.
 *                   2. The key size is required. It must be a valid size for the key type.
 *                   3. The key permitted-algorithm policy is required for keys that will be used for a cryptographic operation, see Permitted algorithms.
 *                   4. The key usage flags define what operations are permitted with the key, see Key usage flags.
 *                   5. The key lifetime and identifier are required for a persistent key.
 * @param key On success, an identifier for the newly created key. PSA_KEY_ID_NULL on failure.
 * @return psa_status_t 
 */
psa_status_t psa_generate_key(const psa_key_attributes_t * attributes,
                              psa_key_id_t * key);

/**
 * @brief Make a copy of a key. This function is currently not supported. 
 * 
 * @param source_key The key to copy. It must permit the usage PSA_KEY_USAGE_COPY. 
 *                      If a private or secret key is being copied outside of a secure element it must also permit PSA_KEY_USAGE_EXPORT.
 * @param attributes The attributes for the new key.
 * @param target_key On success, an identifier for the newly created key. PSA_KEY_ID_NULL on failure.
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED 
 */
psa_status_t psa_copy_key(psa_key_id_t source_key,
                          const psa_key_attributes_t * attributes,
                          psa_key_id_t * target_key);

/**
 * @brief Destroy a key.
 * 
 * @param key Identifier of the key to erase. If this is PSA_KEY_ID_NULL, do nothing and return PSA_SUCCESS.
 * @return psa_status_t 
 */
psa_status_t psa_destroy_key(psa_key_id_t key);

/**
 * @brief Remove non-essential copies of key material from memory. This function is currently not supported.
 * 
 * @param key Identifier of the key to purge.
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED 
 */
psa_status_t psa_purge_key(psa_key_id_t key);

/**
 * @brief This function is currently not supported.
 * 
 * @param key Identifier of the key to export. It must permit the usage PSA_KEY_USAGE_EXPORT, unless it is a public key.
 * @param data Buffer where the key data is to be written.
 * @param data_size Size of the data buffer in bytes.
 * @param data_length On success, the number of bytes that make up the key data.
 * @return psa_status_t 
 */
psa_status_t psa_export_key(psa_key_id_t key,
                            uint8_t * data,
                            size_t data_size,
                            size_t * data_length);

/**
 * @brief 
 * 
 * @param key Identifier of the key to export.
 * @param data Buffer where the key data is to be written.
 * @param data_size Size of the data buffer in bytes. 
 * @param data_length On success, the number of bytes that make up the key data.
 * @return psa_status_t 
 */
psa_status_t psa_export_public_key(psa_key_id_t key,
                                   uint8_t * data,
                                   size_t data_size,
                                   size_t * data_length);

/**
 * @brief                   Single-part hashing functions that calculate the 
 *                          hash (digest) of a message
 * 
 * 
 * @param[in] alg           The hash algorithm to compute: a value of type psa_algorithm_t such
 *                          that PSA_ALG_IS_HASH(alg) is true
 * @param[in] input         Buffer containing the message to hash
 * @param[in] input_length  Size of the input buffer in bytes
 * @param[out] hash         Buffer where the hash is to be written
 * @param[in] hash_size     Size of the hash buffer in bytes. This must be at least
 *                          PSA_HASH_LENGTH(alg).
 * @param[out] hash_length  On success, the number of bytes that make up the hash value. This is
 *                          always PSA_HASH_LENGTH(alg)
 * 
 * @return PSA_SUCCESS              Success. The first (*hash_length) bytes of hash contain the hash value
 * @return PSA_ERROR_BAD_STATE      The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT under following condition
 *                                  alg is not a hash algorithm
 *                                  input_length is too large for alg
 * @return PSA_ERROR_NOT_SUPPORTED  under following condition
 *                                  alg is not supported or is not a hash algorithm
 *                                  input_length is too large for the implementation.
 */
psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              uint8_t * hash,
                              size_t hash_size,
                              size_t * hash_length);

/**
 * @brief                   Calculate the hash (digest) of a message and compare it with a reference value
 * 
 * 
 * @param[in] alg           The hash algorithm to compute: a value of type psa_algorithm_t such
 *                          that PSA_ALG_IS_HASH(alg) is true
 * @param[in] input         Buffer containing the message to hash
 * @param[in] input_length  Size of the input buffer in bytes
 * @param[in] hash          Buffer containing the expected hash value
 * @param[in] hash_length   Size of the hash buffer in bytes
 * 
 * @return PSA_SUCCESS              Success. The expected hash is identical to the actual hash of the input
 * @return PSA_ERROR_BAD_STATE      The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT under following condition
 *                                  alg is not a hash algorithm
 *                                  input_length is too large for alg
 * @return PSA_ERROR_NOT_SUPPORTED  under following condition
 *                                  alg is not supported or is not a hash algorithm
 *                                  input_length is too large for the implementation.
 */
psa_status_t psa_hash_compare(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              const uint8_t * hash,
                              size_t hash_length);
/**
 * @brief   Function that returns initial value for a hash operation object
 * 
 * @return psa_hash_operation_t     Hash operation object
 */
psa_hash_operation_t psa_hash_operation_init(void);

/**
 * @brief                   Set up a multi-part hash operation
 * 
 * @param[in] operation     The operation object to set up. It must have been initialized as per
 *                          the documentation for psa_hash_operation_t and not yet in use
 * @param[in] alg           The hash algorithm to compute: a value of type psa_algorithm_t such
 *                          that PSA_ALG_IS_HASH(alg) is true.
 * 
 * @return PSA_SUCCESS              Success. The operation is now active
 * @return PSA_ERROR_BAD_STATE      The following conditions can result in this error:
 *                                  ∙ The operation state is not valid: it must be inactive.
 *                                  ∙ The library requires initializing by a call to psa_crypto_init().
 * @return PSA_ERROR_INVALID_ARGUMENT   alg is not a hash algorithm
 * @return PSA_ERROR_NOT_SUPPORTED  alg is not supported or is not a hash algorithm
 */
psa_status_t psa_hash_setup(psa_hash_operation_t * operation,
                            psa_algorithm_t alg);

/**
 * @brief                   Add a message fragment to a multi-part hash operation.
 *                          The application must call psa_hash_setup() or psa_hash_resume() 
 *                          before calling this function
 * 
 * @param[in] operation     Active hash operation
 * @param[in] input         Buffer containing the message fragment to hash
 * @param[in] input_length  Size of the input buffer in bytes
 * @return PSA_SUCCESS              Success
 * @return PSA_ERROR_BAD_STATE      The following conditions can result in this error:
 *                                  ∙ The operation state is not valid: it must be active.
 *                                  ∙ The library requires initializing by a call to psa_crypto_init().
 * @return PSA_ERROR_INVALID_ARGUMENT alg is not a hash algorithm or the total input for the operation is 
 *                                    too large for the hash algorithm
 * @return PSA_ERROR_NOT_SUPPORTED  The total input for the operation is too large for the implementation
 */
psa_status_t psa_hash_update(psa_hash_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length);

/**
 * @brief                   Finish the calculation of the hash of a message
 * 
 * @param[in] operation     Active hash operation
 * @param[out] hash         Buffer where the hash is to be written     
 * @param[in] hash_size     Size of the hash buffer in bytes. This must be at least
 *                          PSA_HASH_LENGTH(alg) where alg is the algorithm that the operation performs
 * @param[out] hash_length  On success, the number of bytes that make up the hash value. This is
 *                          always PSA_HASH_LENGTH(alg) where alg is the hash algorithm that the
 *                          operation performs
 * 
 * @return PSA_SUCCESS              Success. The first (*hash_length) bytes of hash contain the hash value
 * @return PSA_ERROR_BAD_STATE      The following conditions can result in this error:
 *                                  ∙ The operation state is not valid: it must be active.
 *                                  ∙ The library requires initializing by a call to psa_crypto_init().
 * @return PSA_ERROR_INVALID_ARGUMENT alg is not a hash algorithm 
 * @return PSA_ERROR_BUFFER_TOO_SMALL   The size of the hash buffer is too small. PSA_HASH_LENGTH() can be used
 *                                      to determine a sufficient buffer size
 */
psa_status_t psa_hash_finish(psa_hash_operation_t * operation,
                             uint8_t * hash,
                             size_t hash_size,
                             size_t * hash_length);

/**
 * @brief                   Finish the calculation of the hash of a message and compare it 
 *                          with an expected value
 * 
 * @param[in] operation     Active hash operation
 * 
 * @param[in] hash          Buffer containing the expected hash value
 * 
 * @param[in] hash_length   Size of the hash buffer in bytes
 * 
 * @return PSA_SUCCESS              Success. The expected hash is identical to the actual hash of the message
 * @return PSA_ERROR_BAD_STATE      The following conditions can result in this error:
 *                                  ∙ The operation state is not valid: it must be active.
 *                                  ∙ The library requires initializing by a call to psa_crypto_init().
 * @return PSA_ERROR_INVALID_SIGNATURE The calculated hash of the message does not match the value in hash
 */

psa_status_t psa_hash_verify(psa_hash_operation_t * operation,
                             const uint8_t * hash,
                             size_t hash_length);
/**
 * @brief               This function is used to abort a hash operation.
 * 
 * @param operation 
 * @return psa_status_t PSA_SUCCESS
 */
psa_status_t psa_hash_abort(psa_hash_operation_t * operation);

/**
 * @brief               This function is not supported.
 * 
 * @param operation 
 * @param hash_state 
 * @param hash_state_size 
 * @param hash_state_length 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_hash_suspend(psa_hash_operation_t * operation,
                              uint8_t * hash_state,
                              size_t hash_state_size,
                              size_t * hash_state_length);

/**
 * @brief               This function is not supported.
 * 
 * @param operation 
 * @param hash_state 
 * @param hash_state_length 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_hash_resume(psa_hash_operation_t * operation,
                             const uint8_t * hash_state,
                             size_t hash_state_length);
/**
 * @brief               This function is not supported.
 * 
 * @param source_operation 
 * @param target_operation 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_hash_clone(const psa_hash_operation_t * source_operation,
                            psa_hash_operation_t * target_operation);

/**
 * @brief Compute the Message Authentication Code (MAC) for the given input using the specified key and algorithm
 * 
 * @param[in] key               Identifier of the key to be used for MAC computation
 * @param[in] alg               Algorithm to be used for MAC computation
 * @param[in] input             Pointer to the input data
 * @param[in] input_length      Length of the input data in bytes
 * @param[out] mac              Pointer to the buffer to store the computed MAC
 * @param[in] mac_size          Size of the MAC buffer in bytes
 * @param[out] mac_length       Pointer to the variable to store the length of the computed MAC
 * 
 * @return PSA_SUCCESS                  Success. The MAC computation was performed successfully
 * @return PSA_ERROR_INVALID_ARGUMENT   Invalid argument provided, such as an invalid key or algorithm
 * @return PSA_ERROR_BUFFER_TOO_SMALL   The size of the MAC buffer is too small to store the computed MAC
 * @return PSA_ERROR_BAD_STATE          The operation state is not valid, or the library requires initialization
 * @return PSA_ERROR_NOT_SUPPORTED      The specified algorithm, key, or input length is not supported
 * @return PSA_ERROR_NOT_PERMITTED      The key does not have the required usage flags or does not permit the requested algorithm
 * @return PSA_ERROR_INVALID_HANDLE     key is not a valid key identifier
 */
psa_status_t psa_mac_compute(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * input,
                             size_t input_length,
                             uint8_t * mac,
                             size_t mac_size,
                             size_t * mac_length);

/**
 * @brief Verify the Message Authentication Code (MAC) for the given input using the specified key and algorithm
 * 
 * @param[in] key               Identifier of the key used for MAC verification
 * @param[in] alg               Algorithm used for MAC verification
 * @param[in] input             Pointer to the input data
 * @param[in] input_length      Length of the input data in bytes
 * @param[in] mac               Pointer to the MAC to be verified
 * @param[in] mac_length        Length of the MAC in bytes
 * 
 * @return PSA_SUCCESS                  Success. The MAC verification was performed successfully
 * @return PSA_ERROR_INVALID_ARGUMENT   Invalid argument provided, such as an invalid key or algorithm
 * @return PSA_ERROR_BAD_STATE          The operation state is not valid, or the library requires initialization
 * @return PSA_ERROR_NOT_SUPPORTED      The specified algorithm, key, or input length is not supported
 * @return PSA_ERROR_NOT_PERMITTED      The key does not have the required usage flags or does not permit the requested algorithm
 * @return PSA_ERROR_INVALID_HANDLE     key is not a valid key identifier
 * @return PSA_ERROR_INVALID_SIGNATURE  The calculated MAC of the message does not match the value in mac
 */
psa_status_t psa_mac_verify(psa_key_id_t key,
                            psa_algorithm_t alg,
                            const uint8_t * input,
                            size_t input_length,
                            const uint8_t * mac,
                            size_t mac_length);

/**
 * @brief Initialize a MAC operation object
 *
 * This function initializes the specified MAC operation object
 *
 * @return psa_mac_operation_t The initialized MAC operation object
 */
psa_mac_operation_t psa_mac_operation_init(void);

/**
 * @brief Set up a MAC signing operation with the specified key and algorithm
 *
 * This function initializes the specified MAC signing operation object with the provided key and algorithm
 *
 * @param[in,out] operation   Pointer to the MAC signing operation object
 * @param[in] key             Identifier of the key to be used for MAC signing
 * @param[in] alg             Algorithm to be used for MAC signing
 *
 * @return PSA_SUCCESS                   Success. The MAC signing operation was set up successfully
 * @return PSA_ERROR_INVALID_ARGUMENT    Invalid argument provided, such as an invalid key or algorithm
 * @return PSA_ERROR_BAD_STATE           The operation state is not valid, or the library requires initialization
 * @return PSA_ERROR_NOT_SUPPORTED       The specified algorithm, key, or input length is not supported
 * @return PSA_ERROR_NOT_PERMITTED       The key does not have the required usage flags or does not permit the requested algorithm
 * @return PSA_ERROR_INVALID_HANDLE      The key is not a valid key identifier
 */
psa_status_t psa_mac_sign_setup(psa_mac_operation_t * operation,
                                psa_key_id_t key,
                                psa_algorithm_t alg);
/**
 * @brief Set up a MAC verification operation with the specified key and algorithm
 *
 * This function initializes the specified MAC verification operation object with the provided key and algorithm
 *
 * @param[in,out] operation   Pointer to the MAC verification operation object
 * @param[in] key             Identifier of the key to be used for MAC verification
 * @param[in] alg             Algorithm to be used for MAC verification
 *
 * @return PSA_SUCCESS                   Success. The MAC verification operation was set up successfully
 * @return PSA_ERROR_INVALID_ARGUMENT    Invalid argument provided, such as an invalid key or algorithm
 * @return PSA_ERROR_BAD_STATE           The operation state is not valid, or the library requires initialization
 * @return PSA_ERROR_NOT_SUPPORTED       The specified algorithm, key, or input length is not supported
 * @return PSA_ERROR_NOT_PERMITTED       The key does not have the required usage flags or does not permit the requested algorithm
 * @return PSA_ERROR_INVALID_HANDLE      The key is not a valid key identifier
 */
psa_status_t psa_mac_verify_setup(psa_mac_operation_t * operation,
                                  psa_key_id_t key,
                                  psa_algorithm_t alg);

/**
 * @brief Update the MAC operation with input data
 *
 * This function updates the specified MAC operation with input data
 *
 * @param[in,out] operation   Pointer to the MAC operation object
 * @param[in] input           Pointer to the input data
 * @param[in] input_length    Length of the input data in bytes
 *
 * @return PSA_SUCCESS                   Success. The MAC operation was updated successfully
 * @return PSA_ERROR_INVALID_ARGUMENT    Invalid argument provided, such as an invalid operation or input data
 * @return PSA_ERROR_BAD_STATE           The operation state is not valid, or the library requires initialization
 * @return PSA_ERROR_NOT_SUPPORTED       The specified algorithm, key, or input length is not supported
 */
psa_status_t psa_mac_update(psa_mac_operation_t * operation,
                            const uint8_t * input,
                            size_t input_length);

/**
 * @brief Finalize a MAC signing operation and retrieve the calculated MAC
 *
 * This function finalizes the specified MAC signing operation, retrieves the calculated MAC,
 * and stores it in the provided buffer.
 *
 * @param[in,out] operation   Pointer to the MAC signing operation object
 * @param[out] mac            Pointer to the buffer to store the calculated MAC
 * @param[in] mac_size        Size of the buffer to store the calculated MAC
 * @param[out] mac_length     Pointer to the variable to store the length of the calculated MAC
 *
 * @return PSA_SUCCESS                   Success. The MAC signing operation was finalized and the calculated MAC was retrieved successfully
 * @return PSA_ERROR_INVALID_ARGUMENT    Invalid argument provided, such as an invalid operation or buffer
 * @return PSA_ERROR_BAD_STATE           The operation state is not valid, or the library requires initialization
 * @return PSA_ERROR_BUFFER_TOO_SMALL    The size of the mac buffer is too small
 */
psa_status_t psa_mac_sign_finish(psa_mac_operation_t * operation,
                                 uint8_t * mac,
                                 size_t mac_size,
                                 size_t * mac_length);
                                 
/**
 * @brief Finalize a MAC verification operation and compare the calculated MAC with the provided MAC
 *
 * This function finalizes the specified MAC verification operation, compares the calculated MAC with the provided MAC,
 * and returns the result of the comparison.
 *
 * @param[in,out] operation   Pointer to the MAC verification operation object
 * @param[in] mac             Pointer to the calculated MAC
 * @param[in] mac_length      Length of the calculated MAC in bytes
 *
 * @return PSA_SUCCESS                   Success. The MAC verification operation was finalized and the MAC comparison passed
 * @return PSA_ERROR_INVALID_ARGUMENT    Invalid argument provided, such as an invalid operation or MAC
 * @return PSA_ERROR_BAD_STATE           The operation state is not valid, or the library requires initialization
 * @return PSA_ERROR_INVALID_SIGNATURE   The MAC comparison failed
 */
psa_status_t psa_mac_verify_finish(psa_mac_operation_t * operation,
                                   const uint8_t * mac,
                                   size_t mac_length);
/**
 * @brief Abort a MAC operation
 *
 * This function aborts the specified MAC operation, releasing any resources associated with it.
 *
 * @param[in,out] operation   Pointer to the MAC operation object
 *
 * @return PSA_SUCCESS                   Success. The MAC operation was aborted successfully
 */
psa_status_t psa_mac_abort(psa_mac_operation_t * operation);

/**
 * @brief Encrypt a message using a symmetric cipher.
 *
 * @param[in]  key           The identifier of the key to use for the operation.
 * @param[in]  alg           The algorithm to use (must be a symmetric cipher algorithm).
 * @param[in]  input         The buffer containing the plaintext message to encrypt.
 * @param[in]  input_length  The size of the plaintext message in bytes.
 * @param[out] output        The buffer where the ciphertext will be written.
 * @param[in]  output_size   The size of the output buffer in bytes.
 * @param[out] output_length The size of the output written in bytes.
 *
 * @return PSA_SUCCESS                The operation completed successfully
 * @return PSA_ERROR_BUFFER_TOO_SMALL Size of provided buffer is less than required
 * @return PSA_ERROR_GENERIC_ERROR    Default IV length is larger than allowed
 * @return PSA_ERROR_INVALID_ARGUMENT The specified algorithm is not a Cipher algorithm
 */
psa_status_t psa_cipher_encrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                uint8_t * output,
                                size_t output_size,
                                size_t * output_length);

/**
 * @brief Decrypt a message using a symmetric cipher.
 *
 * This function decrypts the input data using the specified key and algorithm,
 * and writes the decrypted data to the output buffer.
 *
 * @param[in]  key           The identifier of the key to use for decryption.
 * @param[in]  alg           The algorithm to use for decryption.
 * @param[in]  input         Pointer to the buffer containing the data to decrypt.
 * @param[in]  input_length  Size of the input data in bytes.
 * @param[out] output        Pointer to the buffer where the decrypted data will be written.
 * @param[in]  output_size   Size of the output buffer in bytes.
 * @param[out] output_length On success, the number of bytes written to the output buffer.
 *
 * @return PSA_SUCCESS                   The operation completed successfully
 * @return PSA_ERROR_CORRUPTION_DETECTED Unlock of key slot failed
 * @return PSA_ERROR_INVALID_ARGUMENT    The specified algorithm is not a Cipher algorithm
 */
psa_status_t psa_cipher_decrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                uint8_t * output,
                                size_t output_size,
                                size_t * output_length);

/**
 * @brief Initialize a cipher operation context.
 *
 * This function initializes a cipher operation context to a default state.
 * It must be called before any other cipher operation functions.
 *
 * @return psa_cipher_operation_t structure initialized to 0.
  */
psa_cipher_operation_t psa_cipher_operation_init(void);

/**
 * @brief Set up a cipher operation for encryption.
 *
 * This function sets up a cipher operation context for encryption using the specified key and algorithm.
 * It must be called before any encryption operations are performed.
 *
 * @param[in,out] operation Pointer to the cipher operation context to set up.
 * @param[in]     key       The identifier of the key to use for encryption.
 * @param[in]     alg       The algorithm to use for encryption.
 *
 * @return PSA_SUCCESS                   Operation completed successfully
 * @return PSA_ERROR_BAD_STATE           Provided operation variable is not initialized
 * @return PSA_ERROR_CORRUPTION_DETECTED Unlock of key slot failed
 * @return PSA_ERROR_INVALID_ARGUMENT    Specified algorithm is not a Cipher algorithm
 */
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t * operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);

/**
 * @brief Set up a cipher operation for decryption.
 *
 * This function sets up a cipher operation context for decryption using the specified key and algorithm.
 * It must be called before any decryption operations are performed.
 *
 * @param[in,out] operation Pointer to the cipher operation context to set up.
 * @param[in]     key       The identifier of the key to use for decryption.
 * @param[in]     alg       The algorithm to use for decryption.
 *
 * @return PSA_SUCCESS                   Operation completed successfully
 * @return PSA_ERROR_BAD_STATE           Provided operation variable is not initialized
 * @return PSA_ERROR_CORRUPTION_DETECTED Unlock of key slot failed
 * @return PSA_ERROR_INVALID_ARGUMENT    Specified algorithm is not a Cipher algorithm
 */
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t * operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg);

/**
 * @brief Generate an initialization vector (IV) for a cipher operation.
 *
 * This function generates an IV for the specified cipher operation context.
 * The generated IV is written to the provided buffer.
 *
 * @param[in,out] operation Pointer to the cipher operation context.
 * @param[out]    iv        Pointer to the buffer where the generated IV will be written.
 * @param[in]     iv_size   Size of the IV buffer in bytes.
 * @param[in]     iv_length On success, the number of bytes written to the IV buffer.
 *
 * @return PSA_SUCCESS                Operation completed successfully
 * @return PSA_ERROR_BAD_STATE        Provided operation variable is not initialized
 * @return PSA_ERROR_BUFFER_TOO_SMALL Provided IV buffer is smaller than required
 * @return PSA_ERROR_GENERIC_ERROR    Default IV length specified by operation variable is larger than allowed
 * @return PSA_ERROR_INVALID_ARGUMENT Provided operation variable contains an invalid id member
 */
psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t * operation,
                                    uint8_t * iv,
                                    size_t iv_size,
                                    size_t * iv_length);

/**
 * @brief Set the initialization vector (IV) for a cipher operation.
 *
 * This function sets the IV for the specified cipher operation context.
 * It must be called before any encryption or decryption operations are performed.
 *
 * @param[in,out] operation Pointer to the cipher operation context.
 * @param[out]    iv        Pointer to the buffer containing the IV.
 * @param[in]     iv_length Size of the IV in bytes.
 *
 * @return PSA_SUCCESS                Operation completed successfully
 * @return PSA_ERROR_BAD_STATE        Provided operation variable is not initialized
 * @return PSA_ERROR_INVALID_ARGUMENT Requested IV length is larger than maximum supported size
 */
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t * operation,
                               const uint8_t * iv,
                               size_t iv_length);

/**
 * @brief Update a cipher operation with input data.
 *
 * This function processes a chunk of input data for the specified cipher operation context.
 * It can be called multiple times to process successive chunks of data.
 *
 * @param[in,out] operation     Pointer to the cipher operation context.
 * @param[in]     input         Pointer to the buffer containing the input data.
 * @param[in]     input_length  Size of the input data in bytes.
 * @param[out]    output        Pointer to the buffer where the output data will be written.
 * @param[in]     output_size   Size of the output buffer in bytes.
 * @param[out]    output_length On success, the number of bytes written to the output buffer.
 *
 * @return PSA_SUCCESS                Operation completed successfully
 * @return PSA_ERROR_BAD_STATE        Provided operation variable is not initialized
 * @return PSA_ERROR_INVALID_ARGUMENT Unsupported Operation ID provided
 */
psa_status_t psa_cipher_update(psa_cipher_operation_t * operation,
                               const uint8_t * input,
                               size_t input_length,
                               uint8_t * output,
                               size_t output_size,
                               size_t * output_length);

/**
 * @brief Finish a cipher operation.
 *
 * This function finishes the specified cipher operation, processing any remaining data and writing the final output.
 * It must be called after all input data has been processed with psa_cipher_update().
 *
 * @param[in,out] operation     Pointer to the cipher operation context.
 * @param[out]    output        Pointer to the buffer where the final output data will be written.
 * @param[in]     output_size   Size of the output buffer in bytes.
 * @param[out]    output_length On success, the number of bytes written to the output buffer.
 *
 * @return PSA_SUCCESS                Operation completed successfully
 * @return PSA_ERROR_BAD_STATE        Provided operation variable is not initialized
 * @return PSA_ERROR_INVALID_ARGUMENT Unsupported Operation ID provided
 */
psa_status_t psa_cipher_finish(psa_cipher_operation_t * operation,
                               uint8_t * output,
                               size_t output_size,
                               size_t * output_length);
/**
 * @brief Abort a cipher operation.
 *
 * This function aborts the specified cipher operation, releasing any resources that were allocated.
 * It can be called at any time after the operation has been initialized.
 *
 * @param[in,out] operation Pointer to the cipher operation context.
 *
 * @return PSA_SUCCESS Operation completed successfully
 */
psa_status_t psa_cipher_abort(psa_cipher_operation_t * operation);


/**
 * @brief Encrypts the plaintext using the specified key and algorithm in an Authenticated Encryption 
 *        with Associated Data (AEAD) mode
 *
 * This function encrypts the plaintext using the specified key and algorithm in an AEAD mode. 
 * It takes in the nonce, additional data, and plaintext as input, and outputs the encrypted ciphertext
 *
 * @param[in]  key The identifier of the key to be used for encryption
 * @param[in]  alg The algorithm to be used for encryption
 * @param[in]  nonce A pointer to the nonce value used for encryption
 * @param[in]  nonce_length The length of the nonce value in bytes
 * @param[in]  additional_data A pointer to the additional data associated with the plaintext
 * @param[in]  additional_data_length The length of the additional data in bytes
 * @param[in]  plaintext A pointer to the plaintext to be encrypted
 * @param[in]  plaintext_length The length of the plaintext in bytes
 * @param[out] ciphertext A pointer to the buffer where the encrypted ciphertext will be stored
 * @param[in]  ciphertext_size The size of the buffer allocated for the ciphertext
 * @param[out] ciphertext_length A pointer to the variable where the length of the encrypted ciphertext will be stored
 *
 * @return PSA_SUCCESS if the encryption was successful
 * @return PSA_ERROR_BAD_STATE The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED The key does not have the PSA_KEY_USAGE_ENCRYPT flag, or it does not
           permit the requested algorithm
 * @return PSA_ERROR_BUFFER_TOO_SMALL The size of the ciphertext buffer is too small      
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED if the specified algorithm, key, nonce_length are not supported or 
 *         additional_data_length or plaintext_length are too large for the
 *         implementation
 */
psa_status_t psa_aead_encrypt(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * nonce,
                              size_t nonce_length,
                              const uint8_t * additional_data,
                              size_t additional_data_length,
                              const uint8_t * plaintext,
                              size_t plaintext_length,
                              uint8_t * ciphertext,
                              size_t ciphertext_size,
                              size_t * ciphertext_length);

/**
 * @brief Decrypts the ciphertext using the specified key and algorithm in an Authenticated Encryption 
 *        with Associated Data (AEAD) mode
 *
 * This function decrypts the ciphertext using the specified key and algorithm in an AEAD mode. 
 * It takes in the nonce, additional data, and ciphertext as input, and outputs the decrypted plaintext
 *
 * @param[in]  key The identifier of the key to be used for decryption
 * @param[in]  alg The algorithm to be used for decryption
 * @param[in]  nonce A pointer to the nonce value used for encryption
 * @param[in]  nonce_length The length of the nonce value in bytes
 * @param[in]  additional_data A pointer to the additional data associated with the ciphertext
 * @param[in]  additional_data_length The length of the additional data in bytes
 * @param[in]  ciphertext A pointer to the ciphertext to be decrypted
 * @param[in]  ciphertext_length The length of the ciphertext in bytes
 * @param[out] plaintext A pointer to the buffer where the decrypted plaintext will be stored
 * @param[in]  plaintext_size The size of the buffer allocated for the plaintext
 * @param[out] plaintext_length A pointer to the variable where the length of the decrypted plaintext will be stored
 *
 * @return PSA_SUCCESS if the decryption was successful
 * @return PSA_ERROR_BAD_STATE The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED The key does not have the PSA_KEY_USAGE_DECRYPT flag, or it does not
           permit the requested algorithm
 * @return PSA_ERROR_INVALID_SIGNATURE The ciphertext is not authentic
 * @return PSA_ERROR_BUFFER_TOO_SMALL The size of the plaintext buffer is too small      
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED if the specified algorithm, key, nonce_length are not supported or 
 *         additional_data_length or plaintext_length are too large for the
 *         implementation
 */
psa_status_t psa_aead_decrypt(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * nonce,
                              size_t nonce_length,
                              const uint8_t * additional_data,
                              size_t additional_data_length,
                              const uint8_t * ciphertext,
                              size_t ciphertext_length,
                              uint8_t * plaintext,
                              size_t plaintext_size,
                              size_t * plaintext_length);


/**
 * @brief Initializes an AEAD operation
 *
 * This function initializes an AEAD operation and returns an opaque structure that represents the operation
 *
 * @return The initialized AEAD operation structure
 */
psa_aead_operation_t psa_aead_operation_init(void);

/**
 * @brief Sets up an AEAD operation for encryption
 *
 * This function sets up an AEAD operation for encryption using the specified key and algorithm
 *
 * @param[in,out] operation A pointer to the AEAD operation structure to set up
 * @param[in] key The identifier of the key to be used for encryption
 * @param[in] alg The algorithm to be used for encryption
 *
 * @return PSA_SUCCESS if the operation was set up successfully
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED The key does not have the PSA_KEY_USAGE_ENCRYPT flag, or it does not
           permit the requested algorithm
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED if the specified algorithm or key is not supported
 */
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);


/**
 * @brief Sets up an AEAD operation for decryption.
 *
 * This function sets up an AEAD operation for decryption using the specified key and algorithm.
 *
 * @param[in,out] operation A pointer to the AEAD operation structure to set up.
 * @param[in] key The identifier of the key to be used for decryption.
 * @param[in] alg The algorithm to be used for decryption.
 *
 * @return PSA_SUCCESS if the operation was set up successfully
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED The key does not have the PSA_KEY_USAGE_DECRYPT flag, or it does not
           permit the requested algorithm
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED if the specified algorithm or key is not supported
 */
psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg);


/**
 * @brief Sets the lengths of the additional data and plaintext for an AEAD operation
 *
 * This function sets the lengths of the additional data and plaintext for an AEAD operation.
 * It is used to provide the lengths of the additional data and plaintext before calling the psa_aead_update function
 *
 * @param[in,out] operation A pointer to the AEAD operation structure
 * @param[in] ad_length The length of the additional data in bytes
 * @param[in] plaintext_length The length of the plaintext in bytes
 *
 * @return PSA_SUCCESS if the lengths were set successfully
 * @return PSA_ERROR_BAD_STATE The following conditions can result in this error:
 *                           ∙ The operation state is not valid: it must be active, and
 *                           psa_aead_set_nonce() and psa_aead_generate_nonce() must not
 *                           have been called yet
 *                           ∙ The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * 
 * @note This function only supports plaintext length <= UINT16_MAX for AES CCM
 */
psa_status_t psa_aead_set_lengths(psa_aead_operation_t * operation,
                                  size_t ad_length,
                                  size_t plaintext_length);


/**
 * @brief Generates a nonce for an AEAD operation
 *
 * This function generates a nonce for an AEAD operation and stores it in the provided buffer
 *
 * @param[in,out] operation A pointer to the AEAD operation structure
 * @param[out] nonce A pointer to the buffer where the generated nonce will be stored
 * @param[in] nonce_size The size of the buffer in bytes
 * @param[out] nonce_length A pointer to the variable where the length of the generated nonce will be stored
 *
 * @return PSA_SUCCESS if the nonce was generated successfully
 * @return PSA_ERROR_BAD_STATE The following conditions can result in this error:
 *                           ∙ The operation state is not valid: it must be active, with no nonce set
 *                           ∙ The operation state is not valid: this is an algorithm which
 *                             requires psa_aead_set_lengths() to be called before setting the nonce
 *                           ∙ The library requires initializing by a call to psa_crypto_init()
 *                           ∙ The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_BUFFER_TOO_SMALL if the buffer size is not sufficient to store the generated nonce
 */
psa_status_t psa_aead_generate_nonce(psa_aead_operation_t * operation,
                                     uint8_t * nonce,
                                     size_t nonce_size,
                                     size_t * nonce_length);


/**
 * @brief Sets the nonce for an AEAD operation.
 *
 * This function sets the nonce for an AEAD operation.
 *
 * @param[in,out] operation A pointer to the AEAD operation structure.
 * @param[in] nonce A pointer to the buffer containing the nonce.
 * @param[in] nonce_length The length of the nonce in bytes.
 *
 * @return PSA_SUCCESS if the nonce was set successfully.
 * @return PSA_ERROR_BAD_STATE The following conditions can result in this error:
 *                           ∙ The operation state is not valid: it must be active, with no nonce set
 *                           ∙ The operation state is not valid: this is an algorithm which
 *                             requires psa_aead_set_lengths() to be called before setting the nonce
 *                           ∙ The library requires initializing by a call to psa_crypto_init()
 *                           ∙ The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED if the specified nonce length is not supported
 * 
 * @note This function only supports nonce length of:
 *       nonce_length = 12 bytes for AES GCM 
 *       7 <= nonce_length <= 13 for AES CCM provided that payload len is less than < UINT16_MAX
 */
psa_status_t psa_aead_set_nonce(psa_aead_operation_t * operation,
                                const uint8_t * nonce,
                                size_t nonce_length);


/**
 * @brief Updates the additional data for an AEAD operation
 *
 * This function updates the additional data for an AEAD operation.
 * It is used to provide additional data that is authenticated but not encrypted
 *
 * @param[in,out] operation A pointer to the AEAD operation structure
 * @param[in] input A pointer to the buffer containing the additional data
 * @param[in] input_length The length of the additional data in bytes
 *
 * @return PSA_SUCCESS if the additional data was updated successfully
 * @return PSA_ERROR_BAD_STATE The following conditions can result in this error:
 *                             ∙ The operation state is not valid: it must be active, have a nonce
 *                               set, have lengths set if required by the algorithm, and
 *                               psa_aead_update() must not have been called yet
 *                             ∙ The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED The total additional data length is too large for the implementation
 * 
 * @note This function can only be called once for adding additional data
 */
psa_status_t psa_aead_update_ad(psa_aead_operation_t * operation,
                                const uint8_t * input,
                                size_t input_length);


/**
 * @brief Updates the data for an AEAD operation.
 *
 * This function updates the data for an AEAD operation.
 * It can be used for both encryption and decryption
 *
 * @param[in,out] operation A pointer to the AEAD operation structure
 * @param[in] input A pointer to the buffer containing the input data
 * @param[in] input_length The length of the input data in bytes
 * @param[out] output A pointer to the buffer where the output data will be stored
 * @param[in] output_size The size of the output buffer in bytes
 * @param[out] output_length A pointer to the variable where the length of the output data will be stored
 *
 * @return PSA_SUCCESS if the data was updated successfully
 * @return PSA_ERROR_BAD_STATE The following conditions can result in this error:
 *                             ∙ The operation state is not valid: it must be active, have a nonce
 *                               set, have lengths set if required by the algorithm
 *                             ∙ The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_BUFFER_TOO_SMALL if the output buffer is too small to store the output data
 * 
 * @note This function needs to be called atleast once for the operation to be successful
 */
psa_status_t psa_aead_update(psa_aead_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length,
                             uint8_t * output,
                             size_t output_size,
                             size_t * output_length);


/**
 * @brief Finishes an AEAD operation and produces the ciphertext and tag
 *
 * This function finishes an AEAD operation and produces the ciphertext and tag.
 * It should be called after all the input data has been processed using the psa_aead_update function
 *
 * @param[in,out] operation A pointer to the AEAD operation structure
 * @param[out] ciphertext A pointer to the buffer where the ciphertext will be stored
 * @param[in] ciphertext_size The size of the ciphertext buffer in bytes
 * @param[out] ciphertext_length A pointer to the variable where the length of the ciphertext will be stored
 * @param[out] tag A pointer to the buffer where the tag will be stored
 * @param[in] tag_size The size of the tag buffer in bytes
 * @param[out] tag_length A pointer to the variable where the length of the tag will be stored
 *
 * @return PSA_SUCCESS if the operation was finished successfully and the ciphertext and tag were produced
 * @return PSA_ERROR_BAD_STATE The following conditions can result in this error:
 *                             ∙ The operation state is not valid: it must be active, have a nonce
 *                               set
 *                             ∙ The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_BUFFER_TOO_SMALL if the ciphertext or tag buffer is too small to store the output data
 */
psa_status_t psa_aead_finish(psa_aead_operation_t * operation,
                             uint8_t * ciphertext,
                             size_t ciphertext_size,
                             size_t * ciphertext_length,
                             uint8_t * tag,
                             size_t tag_size,
                             size_t * tag_length);


/**
 * @brief Verifies the authenticity of the ciphertext and tag in an AEAD operation
 *
 * This function verifies the authenticity of the ciphertext and tag in an AEAD operation.
 * It compares the computed tag with the provided tag and returns the result
 *
 * @param[in] operation A pointer to the AEAD operation structure
 * @param[in,out] plaintext A pointer to the buffer where the decrypted plaintext will be stored
 * @param[in] plaintext_size The size of the plaintext buffer in bytes
 * @param[out] plaintext_length A pointer to the variable where the length of the decrypted plaintext will be stored
 * @param[in] tag A pointer to the buffer containing the tag to be verified
 * @param[in] tag_length The length of the tag in bytes
 *
 * @return PSA_SUCCESS if the ciphertext and tag are authentic and the plaintext was successfully decrypted
 * @return PSA_ERROR_BAD_STATE The following conditions can result in this error:
 *                             ∙ The operation state is not valid: it must be active, have a nonce
 *                               set
 *                             ∙ The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_SIGNATURE if the ciphertext and tag are not authentic
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_BUFFER_TOO_SMALL if the buffer is too small to store the plaintext
 */
psa_status_t psa_aead_verify(psa_aead_operation_t * operation,
                             uint8_t * plaintext,
                             size_t plaintext_size,
                             size_t * plaintext_length,
                             const uint8_t * tag,
                             size_t tag_length);
                            

/**
 * @brief Aborts an AEAD operation.
 *
 * This function aborts an AEAD operation and frees any resources associated with it.
 * It should be called to clean up an operation that is no longer needed.
 *
 * @param[in,out] operation A pointer to the AEAD operation structure.
 *
 * @return PSA_SUCCESS if the operation was successfully aborted.
 */
psa_status_t psa_aead_abort(psa_aead_operation_t * operation);

/* Implement key derivation and uncomment */

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED 
*/
psa_key_derivation_operation_t psa_key_derivation_operation_init(void);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t * operation,
                                      psa_algorithm_t alg);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t * operation,
                                             size_t * capacity);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t * operation,
                                             size_t capacity);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t * operation,
                                            psa_key_derivation_step_t step,
                                            const uint8_t * data,
                                            size_t data_length);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_input_integer(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              uint64_t value);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t * operation,
                                          psa_key_derivation_step_t step,
                                          psa_key_id_t key);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t * operation,
                                             uint8_t * output,
                                             size_t output_length);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_output_key(const psa_key_attributes_t * attributes,
                                           psa_key_derivation_operation_t * operation,
                                           psa_key_id_t * key);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_verify_bytes(psa_key_derivation_operation_t * operation,
                                             const uint8_t *expected_output,
                                             size_t output_length);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_verify_key(psa_key_derivation_operation_t * operation,
                                           psa_key_id_t expected);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t * operation);

/**
 * @brief                       Sign a message with a private key. For hash-and-sign algorithms, 
 *                              this includes the hashing step
 * 
 * @param[in] key               Identifier of the key to use for the operation. It must be an
 *                              asymmetric key pair. The key must permit the usage
 *                              PSA_KEY_USAGE_SIGN_MESSAGE
 * @param[in] alg               An asymmetric signature algorithm: a value of type psa_algorithm_t
 *                              such that PSA_ALG_IS_SIGN_MESSAGE(alg) is true
 * @param[in] input             The input message to sign
 * @param[in] input_length      Size of the input buffer in bytes
 * @param[out] signature        Buffer where the signature is to be written
 * @param[in] signature_size    Size of the signature buffer in bytes. This must be appropriate for the
 *                              selected algorithm and key
 * @param[out] signature_length On success, the number of bytes that make up the returned signature
 *                              value
 * @return PSA_SUCCESS              Success. The first (*signature_length) bytes of signature contain the
 *                                  signature value
 * @return PSA_ERROR_BAD_STATE      The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED  The key does not have the PSA_KEY_USAGE_SIGN_MESSAGE flag, or it does
 *                                  not permit the requested algorithm
 * @return PSA_ERROR_BUFFER_TOO_SMALL The size of the signature buffer is too small
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED  The following conditions can result in this error:
 *                                  ∙ alg is not supported or is not an asymmetric signature algorithm
 *                                  ∙ key is not supported for use with alg
 *                                  ∙ input_length is too large for the implementation
 */
psa_status_t psa_sign_message(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              uint8_t * signature,
                              size_t signature_size,
                              size_t * signature_length);

/**
 * @brief                       Verify the signature of a message with a public key. For hash-and-sign 
 *                              algorithms, this includes the hashing step
 * 
 * @param[in] key               Identifier of the key to use for the operation. It must be a public key or an
 *                              asymmetric key pair. The key must permit the usage PSA_KEY_USAGE_SIGN_MESSAGE
 * @param[in] alg               An asymmetric signature algorithm: a value of type psa_algorithm_t
 *                              such that PSA_ALG_IS_SIGN_MESSAGE(alg) is true
 * @param[in] input             The message whose signature is to be verified
 * @param[in] input_length      Size of the input buffer in bytes
 * @param[in] signature         Buffer where the signature is to verify
 * @param[in] signature_length  Size of the signature buffer in bytes
 * @return PSA_SUCCESS              Success. The signature is valid
 * @return PSA_ERROR_BAD_STATE      The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED  The key does not have the PSA_KEY_USAGE_VERIFY_MESSAGE flag, or it does
 *                                  not permit the requested algorithm
 * @return PSA_ERROR_INVALID_SIGNATURE  signature is not the result of signing the input message with
 *                                      algorithm alg using the private key corresponding to key
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED  The following conditions can result in this error:
 *                                  ∙ alg is not supported or is not an asymmetric signature algorithm
 *                                  ∙ key is not supported for use with alg
 *                                  ∙ input_length is too large for the implementation
 */
psa_status_t psa_verify_message(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t * input,
                                size_t input_length,
                                const uint8_t * signature,
                                size_t signature_length);

/**
 * @brief                       Sign an already-calculated hash with a private key
 * 
 * @param[in] key               Identifier of the key to use for the operation. It must be an
 *                              asymmetric key pair. The key must permit the usage
 *                              PSA_KEY_USAGE_SIGN_HASH
 * @param[in] alg               An asymmetric signature algorithm that separates the hash and sign
 *                              operations: a value of type psa_algorithm_t such that
 *                              PSA_ALG_IS_SIGN_HASH(alg) is true
 * @param[in] hash              The input to sign. This is usually the hash of a message
 * @param[in] hash_length       Size of the hash buffer in bytes
 * @param[out] signature        Buffer where the signature is to be written
 * @param[in] signature_size    Size of the signature buffer in bytes. This must be appropriate for the
 *                              selected algorithm and key
 * @param[out] signature_length On success, the number of bytes that make up the returned signature
 *                              value
 * 
 * @return PSA_SUCCESS          Success. The first (*signature_length) bytes of signature contain the
 *                              signature value
 * @return PSA_ERROR_BAD_STATE      The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED  The key does not have the PSA_KEY_USAGE_SIGN_HASH flag, or it does
 *                                  not permit the requested algorithm
 * @return PSA_ERROR_BUFFER_TOO_SMALL The size of the signature buffer is too small
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED  The following conditions can result in this error:
 *                                  ∙ alg is not supported or is not an asymmetric signature algorithm
 *                                  ∙ key is not supported for use with alg
 *                                  ∙ input_length is too large for the implementation
 */
psa_status_t psa_sign_hash(psa_key_id_t key,
                           psa_algorithm_t alg,
                           const uint8_t * hash,
                           size_t hash_length,
                           uint8_t * signature,
                           size_t signature_size,
                           size_t * signature_length);

/**
 * @brief                       Verify the signature of a hash
 * 
 * @param[in] key               Identifier of the key to use for the operation. It must be a public key or an
 *                              asymmetric key pair. The key must permit the usage PSA_KEY_USAGE_SIGN_HASH
 * @param[in] alg               An asymmetric signature algorithm: a value of type psa_algorithm_t
 *                              such that PSA_ALG_IS_SIGN_HASH(alg) is true
 * @param[in] hash              The input whose signature is to be verified. This is usually the hash of
 *                              a message
 * @param[in] hash_length       Size of the hash buffer in bytes
 * @param[in] signature         Buffer where the signature is to verify
 * @param[in] signature_length  Size of the signature buffer in bytes
 * @return PSA_SUCCESS          Success. The signature is valid
 * @return PSA_ERROR_BAD_STATE      The library requires initializing by a call to psa_crypto_init()
 * @return PSA_ERROR_INVALID_HANDLE key is not a valid key identifier
 * @return PSA_ERROR_NOT_PERMITTED  The key does not have the PSA_KEY_USAGE_VERIFY_HASH flag, or it does
 *                                  not permit the requested algorithm
 * @return PSA_ERROR_INVALID_SIGNATURE  signature is not the result of signing hash with
 *                                      algorithm alg using the private key corresponding to key
 * @return PSA_ERROR_INVALID_ARGUMENT if any of the input parameters are invalid
 * @return PSA_ERROR_NOT_SUPPORTED  The following conditions can result in this error:
 *                                  ∙ alg is not supported or is not an asymmetric signature algorithm
 *                                  ∙ key is not supported for use with alg
 *                                  ∙ input_length is too large for the implementation
 */
psa_status_t psa_verify_hash(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t * hash,
                             size_t hash_length,
                             const uint8_t * signature,
                             size_t signature_length);

/**
 * @brief                       Encrypt data using an asymmetric key
 * 
 * @param[in] key               Identifier of the key to use for the operation. It must be an
 *                              asymmetric key pair. The key must permit the usage
 *                              PSA_KEY_USAGE_ENCRYPT
 * @param[in] alg               An asymmetric encryption algorithm: a value of type psa_algorithm_t
 *                              such that PSA_ALG_IS_ENCRYPT(alg) is true
 * @param[in] input             The input data to encrypt
 * @param[in] input_length      Size of the input buffer in bytes
 * @param[in] salt              A salt to use for key derivation. The salt is optional and its
 *                              length is specified by salt_length
 * @param[in] salt_length       Size of the salt buffer in bytes
 * @param[out] output           Buffer where the encrypted data is to be written
 * @param[in] output_size       Size of the output buffer in bytes. This must be appropriate for the
 *                              selected algorithm and key
 * @param[out] output_length    On success, the number of bytes that make up the encrypted data
 * 
 * @return PSA_ERROR_NOT_SUPPORTED  This function is not supported
 */
psa_status_t psa_asymmetric_encrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length);


/**
 * @brief                       Decrypt data using an asymmetric key
 * 
 * @param[in] key               Identifier of the key to use for the operation. It must be an
 *                              asymmetric key pair. The key must permit the usage
 *                              PSA_KEY_USAGE_DECRYPT
 * @param[in] alg               An asymmetric encryption algorithm: a value of type psa_algorithm_t
 *                              such that PSA_ALG_IS_DECRYPT(alg) is true
 * @param[in] input             The input data to decrypt
 * @param[in] input_length      Size of the input buffer in bytes
 * @param[in] salt              A salt to use for key derivation. The salt is optional and its
 *                              length is specified by salt_length
 * @param[in] salt_length       Size of the salt buffer in bytes
 * @param[out] output           Buffer where the decrypted data is to be written
 * @param[in] output_size       Size of the output buffer in bytes. This must be appropriate for the
 *                              selected algorithm and key
 * @param[out] output_length    On success, the number of bytes that make up the decrypted data
 * 
 * @return PSA_ERROR_NOT_SUPPORTED  This function is not supported
 */
psa_status_t psa_asymmetric_decrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length);

/**
 * Perform a raw key agreement.
 *
 * This function performs a raw key agreement using the specified algorithm and private key,
 * and writes the shared secret to the output buffer.
 *
 * @param[in]  alg              An algorithm identifier: a value of type psa_algorithm_t
 * @param[in]  private_key      The identifier of the private key to use.
 * @param[in]  peer_key         Pointer to the buffer containing the peer's public key.
 * @param[in]  peer_key_length  Size of the peer's public key in bytes.
 * @param[out] output           Pointer to the buffer where key data will be written.
 * @param[in]  output_size      Size of the output buffer in bytes.
 * @param[out] output_length    Number of bytes of key data written to output buffer.
 *
 * @return PSA_SUCCESS                   Operation was successful
 * @return PSA_ERROR_BUFFER_TOO_SMALL    Provided output buffer size is smaller than required size
 * @return PSA_ERROR_CORRUPTION_DETECTED Unlock of key slot failed
 * @return PSA_ERROR_INVALID_ARGUMENT    Specified algorithm is not a key agreement algorithm
 */
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   psa_key_id_t private_key,
                                   const uint8_t * peer_key,
                                   size_t peer_key_length,
                                   uint8_t * output,
                                   size_t output_size,
                                   size_t * output_length);

/**
 * @brief This function is currently not supported.
 * 
 * @return psa_status_t PSA_ERROR_NOT_SUPPORTED
*/
psa_status_t psa_key_derivation_key_agreement(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              psa_key_id_t private_key,
                                              const uint8_t * peer_key,
                                              size_t peer_key_length);

/**
 * @brief Generate random data.
 *
 * This function generates random data and writes it to the provided buffer.
 *
 * @param[out] output        Pointer to the buffer where the random data will be written.
 * @param[in]  output_size   Size of the output buffer in bytes.
 *
 * @return PSA_SUCCESS                       Operation completed successfully
 * @return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED Unable to generate random data
 * @return PSA_ERROR_HARDWARE_FAILURE        A failure of the random generator hardware
 * @return PSA_ERROR_INSUFFICIENT_ENTROPY    Unable to generate sufficient random data
 */
psa_status_t psa_generate_random(uint8_t * output,
                                 size_t output_size);

#ifdef __cplusplus
}
#endif

#endif // PSA_CRYPTO_H

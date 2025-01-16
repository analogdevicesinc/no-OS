/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef _CRYPTO_TYPEDEFS_H_
#define _CRYPTO_TYPEDEFS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include <stdint.h>
#include <stdbool.h>
#include "MXQ_API.h"

/** typedef for Key identifier */
typedef uint32_t psa_key_id_t;
/** typedef for PSA status code */
typedef int32_t psa_status_t;
/** typedef for Encoding of key lifetimes */
typedef uint32_t psa_key_lifetime_t;
/** typedef for Encoding of key persistence levels */
typedef uint8_t psa_key_persistence_t;
/** typedef for Encoding of key location indicators */
typedef uint32_t psa_key_location_t;
/** typedef for Encoding of a key type */
typedef uint16_t psa_key_type_t;
/** typedef for identifiers of an elliptic curve family */
typedef uint8_t psa_ecc_family_t;
/** typedef for identifiers of a finite-field Diffie-Hellman group family */
typedef uint8_t psa_dh_family_t;
/** typedef for Encoding of permitted usage on a key */
typedef uint32_t psa_key_usage_t;
/** typedef for Encoding of a cryptographic algorithm */
typedef uint32_t psa_algorithm_t;
/** typedef for Encoding of the step of a key derivation */
typedef uint16_t psa_key_derivation_step_t;

/* TODO: Implement below tyepdefs*/
typedef struct
{
    uint32_t handle;
}psa_key_derivation_operation_s;

//typedef of the state object for key derivation operations
typedef psa_key_derivation_operation_s psa_key_derivation_operation_t;

typedef struct
{
    uint8_t perform;
    uint32_t key_id;
    psa_algorithm_t alg;
    mxq_algo_id_t mxq_algo;
    size_t mac_length;
    bool isActive;
} psa_mac_operation_s;

/* Implementation defined struct
   Type of the state object for multi-part MAC operations */
typedef psa_mac_operation_s psa_mac_operation_t;
 
typedef struct {
    psa_algorithm_t alg;	/**< Algorithm identifier */
    bool isInitialized;		/**< Initialization flag */
    // Additional attributes can be added if necessary
} psa_uss_hash_t;

/** Implementation defined struct.
    Application should not make any assumption about the content of the structure */
typedef psa_uss_hash_t psa_hash_operation_t;


/** Implementation defined struct.
    Attributes associated with key like policy, usage etc. */
typedef struct
{
    psa_key_type_t key_type;			/**< Encoding of key type */
    psa_key_lifetime_t key_lifetime;	/**< Lifetime of key */
    psa_key_usage_t key_use;			/**< Usage of key */
    psa_algorithm_t key_algo;			/**< Key algorithm type */
    size_t key_bits;					/**< Key size in bits */
    psa_key_id_t key_id;				/**< Key identifier */
} psa_key_attributes_t;

typedef struct{
    uint8_t nonce[16];
    uint32_t body_available;
    uint32_t aad_available;
    uint32_t isActive : 1;
    uint32_t encrypt_decrypt : 1;        //Encrypt: 1 Decrypt: 0
    uint32_t isNonceSet : 1;
    uint32_t isLenSet : 1;
    uint32_t isAADBegin : 1;
    uint32_t isBodyBegin : 1;
    uint32_t isCipherInitDone: 1;
}uss_aead_ctx_t;

typedef struct{
    uint8_t nonce[16];
    uint32_t isActive : 1;
    uint32_t encrypt_decrypt : 1;        //Encrypt: 1 Decrypt: 0
    uint32_t isIVSet : 1;
    uint32_t isBodyBegin : 1;
    uint32_t isCipherInitDone: 1;
    uint32_t processed_input_len;
}uss_cipher_ctx_t;

typedef struct
{
    psa_algorithm_t alg;
    mxq_algo_id_t mxq_algo;
    psa_key_attributes_t key_attr;
    ciph_params_t ciph_params;
    uss_aead_ctx_t ctx;
} psa_aead_operation_s;

/** Implementation defined struct.
    Type of the state object for multi-part AEAD operations. */
typedef psa_aead_operation_s psa_aead_operation_t;


typedef struct
{
    uint32_t                handle;
    uint32_t                num_blocks_processed;
    psa_key_attributes_t    key_attr;
    mxq_algo_id_t           cipher_algo;
    ciph_params_t           ciph_params;
    uss_cipher_ctx_t        ctx;
} psa_cipher_operation_s;
/* Implementation defined struct
   Type of the state object for multi-part cipher operations */

typedef psa_cipher_operation_s psa_cipher_operation_t;

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //_CRYPTO_TYPEDEFS_H_

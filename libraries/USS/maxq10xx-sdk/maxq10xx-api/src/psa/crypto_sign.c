/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include "psa/crypto.h"
#include "psa/crypto_common.h"
#include "psa/key_management.h"


psa_status_t psa_sign_message(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * input,
                              size_t input_length, uint8_t * signature, size_t signature_size,
                              size_t * signature_length){

    mxq_algo_id_t maxq_algo;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    psa_key_attributes_t attributes = {0};

    if(!ifCryptoInitDone()){
        return PSA_ERROR_BAD_STATE;
    }
    psa_status = psa_get_key_attributes(key, &attributes);

    if(psa_status != PSA_SUCCESS){
        return PSA_ERROR_INVALID_HANDLE;
    }else if(!PSA_ALG_IS_SIGN_MESSAGE(alg) || input == NULL || signature == NULL 
        || signature_length == NULL || PSA_ALG_GET_HASH(alg) == PSA_ALG_ANY_HASH){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(signature_size < PSA_SIGN_OUTPUT_SIZE(attributes.key_type, attributes.key_bits, alg)){
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }else if( ((attributes.key_use & PSA_KEY_USAGE_SIGN_MESSAGE) != PSA_KEY_USAGE_SIGN_MESSAGE) || !check_key_policy(alg, &attributes)){
        return PSA_ERROR_NOT_PERMITTED;
    }else if(PSA_KEY_TYPE_IS_DH(attributes.key_type) || !PSA_KEY_TYPE_IS_KEY_PAIR(attributes.key_type)){
        return PSA_ERROR_INVALID_ARGUMENT;
    }
 
    maxq_algo = uss_sign_algo_from_psa(alg, true);

    if(maxq_algo == ALGO_INVALID){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    mxq_err_t maxq_status = MXQ_Sign(maxq_algo, key, input, input_length, signature, &signature_size);    

    psa_status = uss_to_psa_error_code(maxq_status);

    if(psa_status == PSA_SUCCESS){
        *signature_length = signature_size;
    }else{
        *signature_length = 0;
    }
    return psa_status;

}

psa_status_t psa_verify_message(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * input,
                                size_t input_length, const uint8_t * signature,
                                size_t signature_length){
    
    mxq_algo_id_t maxq_algo;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    psa_key_attributes_t attributes = {0};

    if(!ifCryptoInitDone()){
        return PSA_ERROR_BAD_STATE;
    }

    psa_status = psa_get_key_attributes(key, &attributes);
    
    if(psa_status != PSA_SUCCESS){
        return PSA_ERROR_INVALID_HANDLE;
    }else if(!PSA_ALG_IS_SIGN_MESSAGE(alg) || input == NULL || signature == NULL ||
        PSA_ALG_GET_HASH(alg) == PSA_ALG_ANY_HASH){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if( ((attributes.key_use & PSA_KEY_USAGE_VERIFY_MESSAGE) != PSA_KEY_USAGE_VERIFY_MESSAGE) || !check_key_policy(alg, &attributes)){
        return PSA_ERROR_NOT_PERMITTED;
    }else if(PSA_KEY_TYPE_IS_DH(attributes.key_type) || !(PSA_KEY_TYPE_IS_KEY_PAIR(attributes.key_type) || PSA_KEY_TYPE_IS_PUBLIC_KEY(attributes.key_type))){
        return PSA_ERROR_INVALID_ARGUMENT;
    }
 
    maxq_algo = uss_sign_algo_from_psa(alg, true);

    if(maxq_algo == ALGO_INVALID){
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    
    mxq_err_t maxq_status = MXQ_Verify(maxq_algo, key, input, input_length, signature, signature_length);

    psa_status = uss_to_psa_error_code(maxq_status);

    return psa_status;

}

psa_status_t psa_sign_hash(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * hash,
                           size_t hash_length, uint8_t * signature, size_t signature_size,
                           size_t * signature_length){

    mxq_algo_id_t maxq_algo;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    psa_key_attributes_t attributes = {0};

    if(!ifCryptoInitDone()){
        return PSA_ERROR_BAD_STATE;
    }

    psa_status = psa_get_key_attributes(key, &attributes);
    
    if(psa_status != PSA_SUCCESS){
        return PSA_ERROR_INVALID_HANDLE;
    }else if(!PSA_ALG_IS_SIGN_HASH(alg) || hash == NULL || signature == NULL || signature_length == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(signature_size < PSA_SIGN_OUTPUT_SIZE(attributes.key_type, attributes.key_bits, alg)){
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }else if( ((attributes.key_use & PSA_KEY_USAGE_SIGN_HASH) != PSA_KEY_USAGE_SIGN_HASH) || !check_key_policy(alg, &attributes)){
        return PSA_ERROR_NOT_PERMITTED;
    }else if(PSA_KEY_TYPE_IS_DH(attributes.key_type) || !PSA_KEY_TYPE_IS_KEY_PAIR(attributes.key_type)
             || PSA_ALG_GET_HASH(alg) == PSA_ALG_ANY_HASH){
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    //The PSA spec v1.2.1 requires all the signature algorithms with this property except these:
    if( (hash_length != PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))) && (alg != PSA_ALG_RSA_PKCS1V15_SIGN_RAW) && (alg != PSA_ALG_ECDSA_ANY) ){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    maxq_algo = uss_sign_algo_from_psa(alg, false);

    if(maxq_algo == ALGO_INVALID){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    mxq_err_t maxq_status = MXQ_Sign(maxq_algo, key, hash, hash_length, signature, &signature_size);

    psa_status = uss_to_psa_error_code(maxq_status);

    if(psa_status == PSA_SUCCESS){
        *signature_length = signature_size;
    }else{
        *signature_length = 0;
    }
    return psa_status;

}

psa_status_t psa_verify_hash(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * hash,
                             size_t hash_length, const uint8_t * signature, size_t signature_length){
    
    mxq_algo_id_t maxq_algo;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    psa_key_attributes_t attributes = {0};

    if(!ifCryptoInitDone()){
        return PSA_ERROR_BAD_STATE;
    }

    psa_status = psa_get_key_attributes(key, &attributes);
    
    if(psa_status != PSA_SUCCESS){
        return PSA_ERROR_INVALID_HANDLE;
    }else if(!PSA_ALG_IS_SIGN_HASH(alg) || hash == NULL || signature == NULL || PSA_ALG_GET_HASH(alg) == PSA_ALG_ANY_HASH){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if( ((attributes.key_use & PSA_KEY_USAGE_VERIFY_HASH) != PSA_KEY_USAGE_VERIFY_HASH) || !check_key_policy(alg, &attributes)){
        return PSA_ERROR_NOT_PERMITTED;
    }else if(PSA_KEY_TYPE_IS_DH(attributes.key_type) || !(PSA_KEY_TYPE_IS_KEY_PAIR(attributes.key_type) || PSA_KEY_TYPE_IS_PUBLIC_KEY(attributes.key_type))){
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    //The PSA spec v1.2.1 requires all the signature algorithms with this property except these:
    if( (hash_length != PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))) && (alg != PSA_ALG_RSA_PKCS1V15_SIGN_RAW) && (alg != PSA_ALG_ECDSA_ANY) ){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    maxq_algo = uss_sign_algo_from_psa(alg, false);

    if(maxq_algo == ALGO_INVALID){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    mxq_err_t maxq_status = MXQ_Verify(maxq_algo, key, hash, hash_length, signature, signature_length);

    psa_status = uss_to_psa_error_code(maxq_status);

    return psa_status;
    
}


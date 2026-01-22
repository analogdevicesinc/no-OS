/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include "psa/crypto.h"

psa_mac_operation_t psa_mac_operation_init(void) {
    psa_mac_operation_t v = PSA_MAC_OPERATION_INIT;
    return v;
}

psa_status_t psa_mac_abort(psa_mac_operation_t *operation) {
    if (operation == NULL || operation->alg == PSA_ALG_NONE) {
        return PSA_SUCCESS;
    }
    MXQ_MAC_Finish(NULL, NULL);
    buffer_cleanup((uint8_t *)operation, sizeof(psa_mac_operation_t));
    return PSA_SUCCESS;
}

static bool check_mac_length(psa_algorithm_t alg, psa_key_attributes_t attributes) {
    size_t mac_length = PSA_MAC_LENGTH(attributes.key_type, attributes.key_bits, alg);
    if (mac_length == 0) {
        return false;
    } else if (mac_length > PSA_MAC_LENGTH(attributes.key_type, attributes.key_bits, PSA_ALG_FULL_LENGTH_MAC(alg))) {
        return false;
    } else if (mac_length > PSA_MAC_MAX_SIZE) {
        return false;
    }
    return true;
}

static bool check_key_for_mac(psa_algorithm_t alg, psa_key_type_t key_type) {
    if (PSA_ALG_IS_HMAC(alg)) {
        if (key_type == PSA_KEY_TYPE_HMAC) {
            return true;
        }
    }

    if (PSA_ALG_IS_BLOCK_CIPHER_MAC(alg)) {
        if ((key_type & USS_PSA_KEY_TYPE_MASK) == USS_PSA_KEY_TYPE_SYMMETRIC_MASK) {
            /* Currently, we only support AES keys for message-authentication-code algorithm, 
               constructed over a block cipher*/
            if (key_type == PSA_KEY_TYPE_AES) {
                return true;
            }
        }
    }

    return false;
}

psa_status_t psa_mac_setup(psa_mac_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg, uint8_t perform) {
    mxq_algo_id_t maxq_algo;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    psa_key_attributes_t attributes = {0};
    psa_key_usage_t key_usage = 0;

    if(perform == USS_MAC_COMPUTE_FLAG){
        key_usage = PSA_KEY_USAGE_SIGN_MESSAGE;
    } else {
        key_usage = PSA_KEY_USAGE_VERIFY_MESSAGE;
    }

    //Parameter sanitization
    if (operation == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if (PSA_ALG_IS_MAC(alg) == 0) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }else if (operation->alg != PSA_ALG_NONE || !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    } else if(psa_get_key_attributes(key, &attributes) != PSA_SUCCESS) {
        psa_status = PSA_ERROR_INVALID_HANDLE;
        goto exit;
    } else if(!check_key_for_mac(alg, attributes.key_type) || !check_mac_length(alg, attributes)) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    } else if( ((attributes.key_use & key_usage) != key_usage) || !check_key_policy(alg, &attributes)) {
        psa_status = PSA_ERROR_NOT_PERMITTED;
        goto exit;
    }
    
    buffer_cleanup((uint8_t *)operation, sizeof(psa_mac_operation_t));

    maxq_algo = uss_psa_to_maxq_algo_id(alg);

    if (maxq_algo == ALGO_INVALID) {
        psa_status = PSA_ERROR_NOT_SUPPORTED;
    } else {
        operation->alg = PSA_ALG_FULL_LENGTH_MAC(alg);
        operation->mac_length = PSA_MAC_LENGTH(attributes.key_type, attributes.key_bits, alg);
        operation->key_id = key;
        operation->perform = perform;
        operation->mxq_algo = maxq_algo;
        operation->isActive = true;
        psa_status = uss_to_psa_error_code(MXQ_MAC_Init(perform, maxq_algo, key, NULL,0));
    }

exit:
    if(psa_status != PSA_SUCCESS){
        psa_mac_abort(operation);
    }
    return psa_status;
}

psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg){
    return psa_mac_setup(operation, key, alg, USS_MAC_COMPUTE_FLAG);
}

psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg){
    return psa_mac_setup(operation, key, alg, USS_MAC_VERIFY_FLAG);
}

psa_status_t psa_mac_update(psa_mac_operation_t * operation, const uint8_t * input,
                            size_t input_length){

    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    uint8_t *pInput = (uint8_t *)input;
    //Parameter sanitization
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if(input_length != 0 && input == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    } else if(operation->alg == PSA_ALG_NONE || !ifCryptoInitDone() || !operation->isActive){
        psa_status = PSA_ERROR_BAD_STATE;
    } else{
        psa_status = uss_to_psa_error_code(MXQ_MAC_Update(pInput, input_length));
    }

    if(PSA_SUCCESS != psa_status){
        psa_mac_abort(operation);
    }
    return psa_status;
}

psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation, uint8_t * mac, size_t mac_size, size_t * mac_length){
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;

    //Parameter sanitization
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if(operation->alg == PSA_ALG_NONE || !ifCryptoInitDone() || !operation->isActive){
        psa_status = PSA_ERROR_BAD_STATE;
    } else if(mac_size < operation->mac_length){
        psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
    } else if(mac_length == NULL || mac == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    } else{
        *mac_length = operation->mac_length;
        psa_status = uss_to_psa_error_code(MXQ_MAC_Finish(mac, mac_length));
    }
    if(PSA_SUCCESS == psa_status){
        buffer_cleanup(&mac[*mac_length], mac_size - *mac_length);
    } else{
        buffer_cleanup(mac, mac_size);
    }
    psa_mac_abort(operation);
    return psa_status;
}

psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation, const uint8_t * mac, size_t mac_length){
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    uint8_t *pMac = (uint8_t *)mac;
    //Parameter sanitization
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if(operation->alg == PSA_ALG_NONE || !ifCryptoInitDone() || !operation->isActive){
        psa_status = PSA_ERROR_BAD_STATE;
    } else if(mac == NULL && mac_length != 0){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    } else{
        psa_status = uss_to_psa_error_code(MXQ_MAC_Verify(pMac, mac_length));
        if(PSA_SUCCESS != psa_status){
            psa_status = PSA_ERROR_INVALID_SIGNATURE;
        }
    }
    psa_mac_abort(operation);
    return psa_status;
}

psa_status_t psa_mac_compute_verify(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * input, 
                                    size_t input_length, const uint8_t * mac, size_t mac_size, 
                                    size_t * mac_length, uint8_t perform){
    
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    
    if(perform == USS_MAC_COMPUTE_FLAG){
        psa_status = psa_mac_sign_setup(&operation, key, alg);
    } else{
        psa_status = psa_mac_verify_setup(&operation, key, alg);
    }
    
    if(psa_status != PSA_SUCCESS){
        goto exit;
    }

    psa_status = psa_mac_update(&operation, (uint8_t *) input, input_length);

    if(psa_status != PSA_SUCCESS){
        goto exit;
    }

    if(perform == USS_MAC_COMPUTE_FLAG){
        psa_status = psa_mac_sign_finish(&operation, (uint8_t *) mac, mac_size, mac_length);
    } else{
        psa_status = psa_mac_verify_finish(&operation, mac, mac_size);
    }

exit:
    psa_mac_abort(&operation);
    return psa_status;
}

psa_status_t psa_mac_compute(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * input, size_t input_length,
                             uint8_t * mac, size_t mac_size, size_t * mac_length){

    return psa_mac_compute_verify(key, alg, input, input_length, mac, mac_size, mac_length, USS_MAC_COMPUTE_FLAG);

}

psa_status_t psa_mac_verify(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * input, size_t input_length,
                            const uint8_t * mac, size_t mac_length){
    
    return psa_mac_compute_verify(key, alg, input, input_length, mac, mac_length, NULL, USS_MAC_VERIFY_FLAG);
}
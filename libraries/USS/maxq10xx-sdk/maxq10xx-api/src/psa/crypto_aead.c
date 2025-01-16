/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include "psa/crypto_config_def.h"
#include "psa/crypto_typedefs.h"
#include "psa/crypto_errors.h"
#include "psa/crypto_aead.h"
#include "psa/key_management.h"
#include "psa/crypto_common.h"

static psa_status_t psa_aead_setup(uint8_t perform, psa_aead_operation_t * operation,
                                    psa_key_id_t key, psa_algorithm_t alg);
static size_t psa_aead_get_min_nonce_length(psa_aead_operation_t * operation);
static psa_status_t uss_check_aead_nonce_length(psa_algorithm_t alg, size_t nonce_length);
static psa_status_t psa_aead_encrypt_decrypt(uint8_t perform, psa_key_id_t key, psa_algorithm_t alg, const uint8_t * nonce,
                                             size_t nonce_length, const uint8_t * additional_data, size_t additional_data_length,
                                             const uint8_t * input, size_t input_length, uint8_t * output,
                                             size_t output_size, size_t * output_length);

extern psa_status_t psa_get_key_attributes(psa_key_id_t key, psa_key_attributes_t * attributes);

psa_status_t psa_aead_abort(psa_aead_operation_t * operation){
    uint8_t lastData[PSA_AEAD_FINISH_OUTPUT_MAX_SIZE + PSA_AEAD_TAG_MAX_SIZE];
    size_t lastDataSz = PSA_AEAD_FINISH_OUTPUT_MAX_SIZE + PSA_AEAD_TAG_MAX_SIZE;
    uint8_t *lastDataPtr = lastData;

    if(operation->ctx.isActive == USS_FALSE){
        return PSA_SUCCESS;
    }
    MXQ_Cipher_Finish(&lastDataPtr, &lastDataSz);
    buffer_cleanup((uint8_t *)operation, sizeof(psa_aead_operation_t)); //Resetting the state
    return PSA_SUCCESS;
}

psa_aead_operation_t psa_aead_operation_init(void)
{
    psa_aead_operation_t val = PSA_AEAD_OPERATION_INIT;
    return val;
}

static psa_status_t psa_aead_setup(uint8_t perform, psa_aead_operation_t * operation,
                                    psa_key_id_t key, psa_algorithm_t alg){

    //Parameter sanitization
    psa_key_attributes_t attributes = {0};
    psa_status_t psa_status = PSA_SUCCESS;
    mxq_algo_id_t mxq_algo = uss_psa_to_maxq_algo_id(alg);
    psa_key_usage_t key_usage = 0;
    
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if(perform == USS_ENCRYPT){
        key_usage = PSA_KEY_USAGE_ENCRYPT;
    }else{
        key_usage = PSA_KEY_USAGE_DECRYPT;
    }

    if(!ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }

    psa_status = psa_get_key_attributes(key, &attributes);
    if(psa_status != PSA_SUCCESS){
        psa_status = PSA_ERROR_INVALID_HANDLE;
    }else if(PSA_ALG_IS_AEAD(alg) == 0 || 
             USS_PSA_ALG_AEAD_IS_WILDCARD(alg) || !check_aead_tag_len(alg)){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    }else if (operation->alg != PSA_ALG_NONE || operation->ctx.isActive == USS_TRUE 
                || operation->ctx.isNonceSet == USS_TRUE || operation->ctx.isLenSet == USS_TRUE){
        psa_status = PSA_ERROR_BAD_STATE;
    }else if( ((attributes.key_use & key_usage) != key_usage) || !check_key_policy(alg, &attributes)){
        psa_status = PSA_ERROR_NOT_PERMITTED;
    }else if(mxq_algo == ALGO_INVALID){
        psa_status = PSA_ERROR_NOT_SUPPORTED;
    }else{
        operation->alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);
        operation->mxq_algo = mxq_algo;
        operation->key_attr.key_id = key;
        operation->key_attr.key_type = attributes.key_type;
        operation->ciph_params.aead_tag_len = PSA_AEAD_TAG_LENGTH(attributes.key_type, attributes.key_bits, alg); 
        operation->ctx.isActive = USS_TRUE;
        operation->ctx.encrypt_decrypt = perform;
        buffer_cleanup(operation->ciph_params.aead_tag, sizeof(operation->ciph_params.aead_tag));  //To pass zero buffer at Cipher_Init
    }

exit:
    if(psa_status != PSA_SUCCESS){
        psa_aead_abort(operation);
    }
    return psa_status;
}

psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key, psa_algorithm_t alg){
    
    return psa_aead_setup(USS_ENCRYPT, operation, key, alg);
    
}

psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t * operation,
                                    psa_key_id_t key, psa_algorithm_t alg){
    
    return psa_aead_setup(USS_DECRYPT, operation, key, alg);
    
}

psa_status_t psa_aead_set_lengths(psa_aead_operation_t * operation, size_t ad_length,
                                  size_t plaintext_length){
    
    psa_status_t psa_status = PSA_SUCCESS;

    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    
    if(ad_length > USS_PSA_AEAD_AAD_MAX_SIZE){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    }
#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    else if(operation->alg == PSA_ALG_CCM && plaintext_length > USS_PSA_AEAD_MSG_MAX_SIZE){
        psa_status = PSA_ERROR_NOT_SUPPORTED;
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM
    else if(operation->ctx.isActive == USS_FALSE || operation->ctx.isNonceSet == USS_TRUE
             || !ifCryptoInitDone() || operation->ctx.isLenSet == USS_TRUE){
        psa_status = PSA_ERROR_BAD_STATE;
    }else{
        operation->ciph_params.data_length = plaintext_length;
        operation->ciph_params.aad_length = ad_length;
        operation->ctx.aad_available = ad_length;   
        operation->ctx.body_available = plaintext_length;
        operation->ctx.isLenSet = USS_TRUE;
    }
    if(psa_status != PSA_SUCCESS){
        psa_aead_abort(operation);
    }
    return psa_status;
}

psa_status_t psa_aead_set_nonce(psa_aead_operation_t * operation, const uint8_t * nonce,
                                size_t nonce_length){

    psa_status_t psa_status = PSA_SUCCESS;
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(operation->ctx.isActive == USS_FALSE || operation->ctx.isNonceSet == USS_TRUE
             || !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
    }else if(uss_check_aead_nonce_length(operation->alg, nonce_length) != PSA_SUCCESS){
        psa_status = PSA_ERROR_NOT_SUPPORTED;
    }
#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    else if(operation->ctx.isLenSet == USS_FALSE && operation->alg == PSA_ALG_CCM){
        psa_status = PSA_ERROR_BAD_STATE;
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM
    else{
        memcpy(operation->ctx.nonce, nonce, nonce_length);
        operation->ciph_params.iv_length = nonce_length;
        operation->ciph_params.p_iv = (mxq_u1*) operation->ctx.nonce;
        operation->ctx.isNonceSet = USS_TRUE;
    }
    if(psa_status != PSA_SUCCESS){
        psa_aead_abort(operation);
    }
    return psa_status;
}

psa_status_t psa_aead_generate_nonce(psa_aead_operation_t * operation, uint8_t * nonce,
                                     size_t nonce_size, size_t * nonce_length){
    
    size_t req_nonce_len = 0;
    mxq_err_t maxq_status;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    uint8_t local_nonce[PSA_AEAD_NONCE_MAX_SIZE];

    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(!ifCryptoInitDone() || operation->ctx.isActive == USS_FALSE 
             || operation->ctx.isNonceSet == USS_TRUE || operation->ctx.encrypt_decrypt != USS_ENCRYPT){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }else if(nonce == NULL || nonce_length == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }
#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    if(operation->ctx.isLenSet == USS_FALSE && operation->alg == PSA_ALG_CCM){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM

    req_nonce_len = psa_aead_get_min_nonce_length(operation);

    if(nonce_size < req_nonce_len){
        psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }

    maxq_status = MXQ_Get_Random(local_nonce, req_nonce_len);

    psa_status = uss_to_psa_error_code(maxq_status);

    if(psa_status == PSA_SUCCESS){
        psa_status = psa_aead_set_nonce(operation, local_nonce, req_nonce_len);
    }

exit:
    if(psa_status != PSA_SUCCESS){
        psa_aead_abort(operation);
    }else{
        memcpy(nonce, local_nonce, req_nonce_len);
        *nonce_length = req_nonce_len;
    }
    return psa_status;
}

static size_t psa_aead_get_min_nonce_length(psa_aead_operation_t * operation){
    size_t nLen = 0;
#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    if(operation->alg == PSA_ALG_CCM){
        nLen = operation->ciph_params.data_length >> 13u == 0u ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE :          //L = 2 Nonce_len = 15 - L = 13   //2^16 bits = 2^13 bytes
               operation->ciph_params.data_length >> 21u == 0u ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE - 1u :     //L = 3 Nonce_len = 15 - L = 12
               operation->ciph_params.data_length >> 29u == 0u ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE - 2u :     //L = 4 Nonce_len = 15 - L = 11
#if SIZE_MAX > UINT32_MAX
     (uint64_t)operation->ciph_params.data_length >> 37u == 0u ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE - 3u :     //L = 5 Nonce_len = 15 - L = 10
     (uint64_t)operation->ciph_params.data_length >> 45u == 0u ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE - 4u :     //L = 6 Nonce_len = 15 - L = 9
     (uint64_t)operation->ciph_params.data_length >> 53u == 0u ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE - 5u :     //L = 7 Nonce_len = 15 - L = 8
     (uint64_t)operation->ciph_params.data_length >> 61u == 0u ? USS_PSA_AEAD_CCM_MAX_NONCE_SIZE - 6u :     //L = 8 Nonce_len = 15 - L = 7
#endif //SIZE_MAX
               0;
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM
#if defined(USS_PSA_AVAILABLE_ALG_AES_GCM)    
    if(operation->alg == PSA_ALG_GCM){
        nLen = PSA_AEAD_NONCE_LENGTH(operation->key_attr.key_type, operation->alg);
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM
    return nLen;
}

static psa_status_t uss_check_aead_nonce_length(psa_algorithm_t alg, size_t nonce_length){
    psa_status_t psa_status = PSA_ERROR_NOT_SUPPORTED;
    
    switch (alg)
    {
#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    case PSA_ALG_CCM:
        psa_status = (nonce_length >= USS_PSA_AEAD_CCM_MIN_NONCE_SIZE && 
                      nonce_length <= USS_PSA_AEAD_CCM_MAX_NONCE_SIZE) ? PSA_SUCCESS : psa_status;
        break;
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM

#ifdef USS_PSA_AVAILABLE_ALG_AES_GCM
    case PSA_ALG_GCM:
        psa_status = (nonce_length == USS_PSA_AEAD_GCM_NONCE_SIZE) ? PSA_SUCCESS : psa_status;
        break;
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM
    default:
        break;
    }
    return psa_status;
}

psa_status_t psa_aead_update_ad(psa_aead_operation_t * operation, const uint8_t * input,
                                size_t input_length){

    psa_status_t psa_status = PSA_SUCCESS;                              
    mxq_err_t mxq_status;
    if(operation == NULL ){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(operation->ctx.isActive == USS_FALSE || operation->ctx.isNonceSet == USS_FALSE
             || operation->ctx.isBodyBegin == USS_TRUE || !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
    }else if(input == NULL && input_length != 0){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    }
#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    else if(operation->ctx.isLenSet == USS_FALSE && operation->alg == PSA_ALG_CCM){
        psa_status = PSA_ERROR_BAD_STATE;
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM
    /**
     * Underlying MAXQ implementation doesn't support 
     * adding additional data for AEAD in multiple-parts. 
     * Therefore, if this is called more than once, we'll
     * error out.
     */
    else if(operation->ctx.isAADBegin == USS_TRUE){
        psa_status = PSA_ERROR_BAD_STATE;
    }
    else if(input_length > USS_PSA_AEAD_AAD_MAX_SIZE){
        psa_status = PSA_ERROR_NOT_SUPPORTED;
    }

    if(psa_status != PSA_SUCCESS){
        goto exit;
    }

    if(operation->ctx.isLenSet == USS_TRUE){
        //Since we do not support multi-part addition of AAD
        //we're enforcing that the input_length should be 
        //equal to length of AAD set in psa_aead_set_lengths()
        if(operation->ciph_params.aad_length != input_length){    
            psa_status = PSA_ERROR_INVALID_ARGUMENT;
            goto exit;
        }
    }else{
        operation->ciph_params.aad_length = input_length;
        operation->ctx.aad_available = input_length;
    }
    
    operation->ciph_params.p_aad = (uint8_t *)input;
    operation->ctx.isAADBegin = USS_TRUE;

    // Doing Cipher_Init here since it is possible that app 
    // choose not to encrypt any plaintext data. In that case, 
    // psa_aead_update() will not be called and we will just authenticate data

    mxq_status = MXQ_Cipher_Init(operation->ctx.encrypt_decrypt, operation->mxq_algo, operation->key_attr.key_id,
                              &operation->ciph_params, USS_FALSE);

    if (mxq_status){
        psa_status = uss_to_psa_error_code(mxq_status);
        goto exit;
    }
    operation->ctx.aad_available -= input_length; //Consumed all AAD
    operation->ctx.isCipherInitDone = USS_TRUE;

exit:
    if(psa_status != PSA_SUCCESS){
        psa_aead_abort(operation);
    }
    return psa_status;

}

psa_status_t psa_aead_update(psa_aead_operation_t * operation, const uint8_t * input,
                             size_t input_length, uint8_t * output, size_t output_size,
                             size_t * output_length){
    
    size_t in_out_len = input_length;
    mxq_err_t mxq_status;
    psa_status_t psa_status = PSA_SUCCESS;

    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(operation->ctx.isActive == USS_FALSE || operation->ctx.isNonceSet == USS_FALSE
             || !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;        
    }else if(input == NULL || output == NULL || output_length == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    }else if(output_size < input_length){
        psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
    }

#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    else if(operation->ctx.isLenSet == USS_FALSE && operation->alg == PSA_ALG_CCM){
        psa_status = PSA_ERROR_BAD_STATE;
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM

    if(psa_status != PSA_SUCCESS){
        goto exit;
    }

    // Doing Cipher_Init only if app doesn't call psa_aead_update_ad()
    if(operation->ctx.isCipherInitDone == USS_FALSE){
        //Resetting the aad_length as app didn't pass any data to authenticate
        operation->ctx.aad_available = operation->ciph_params.aad_length = 0; 

        mxq_status = MXQ_Cipher_Init(operation->ctx.encrypt_decrypt, operation->mxq_algo, operation->key_attr.key_id,
								&operation->ciph_params, USS_FALSE);
        if (mxq_status){
            psa_status = uss_to_psa_error_code(mxq_status);
            goto exit;
        }
        
        operation->ctx.isCipherInitDone = USS_TRUE;
    }
    if(operation->ctx.isLenSet == USS_TRUE){
        if(operation->ctx.aad_available != 0 || //There should not be any AAD pending at this stage
           operation->ctx.body_available < input_length){
            psa_status = PSA_ERROR_INVALID_ARGUMENT;
            goto exit;
        }
        operation->ctx.body_available -= input_length; //Only if length was set earlier
    }
    //TODO: Confirm the behavior of MAXQ APIs if the input is not 
    //multiple of the cipher block size

    mxq_status = MXQ_Cipher_Update(&output, input, &in_out_len);

    psa_status = uss_to_psa_error_code(mxq_status);

exit:
    if(psa_status != PSA_SUCCESS){
        buffer_cleanup(output, output_size);
        psa_aead_abort(operation);
    }else{
        *output_length = in_out_len;
        operation->ctx.isBodyBegin = USS_TRUE;
    }
    return psa_status;
    
}

psa_status_t psa_aead_done(psa_aead_operation_t * operation, uint8_t * output,
                             size_t output_size, size_t * output_length, uint8_t * tag,
                             size_t tag_size, size_t * tag_length){
    mxq_err_t mxq_status;
    psa_status_t psa_status = PSA_SUCCESS;
    uint8_t lastData[PSA_AEAD_FINISH_OUTPUT_MAX_SIZE + PSA_AEAD_TAG_MAX_SIZE + USS_PSA_AEAD_TAG_OVERHEAD_SZ];
    size_t lastDataSz = PSA_AEAD_FINISH_OUTPUT_MAX_SIZE + PSA_AEAD_TAG_MAX_SIZE;
    size_t assignedTagSz = operation->ciph_params.aead_tag_len;
    uint8_t *lastDataPtr = lastData;


    #if defined(USS_PSA_EMULATION)
    if(operation->ctx.encrypt_decrypt == USS_DECRYPT){
        lastData[0] = USS_PSA_AEAD_TAG_OVERHEAD;
        lastDataSz = assignedTagSz;
        memcpy(&lastData[USS_PSA_AEAD_TAG_OVERHEAD_SZ], tag, lastDataSz);
    }
    #endif //USS_PSA_EMULATION

    if(operation->ctx.isActive == USS_FALSE || operation->ctx.isNonceSet == USS_FALSE
             || !ifCryptoInitDone()){
        return PSA_ERROR_BAD_STATE;        
    }else if(operation->ctx.isLenSet == USS_TRUE && 
            (operation->ctx.body_available != 0 || operation->ctx.aad_available != 0)){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(tag_size < assignedTagSz || 
             output_size < PSA_AEAD_FINISH_OUTPUT_SIZE(operation->key_attr.key_type, operation->alg)){
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }else if(output == NULL || output_length == NULL 
        || tag == NULL || tag_length == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else{
        mxq_status = MXQ_Cipher_Finish(&lastDataPtr, &lastDataSz);
        psa_status = uss_to_psa_error_code(mxq_status);
    }
    
    if(psa_status == PSA_SUCCESS){    
        if(operation->ctx.encrypt_decrypt == USS_ENCRYPT){
            //First lastDataSz - assignedTagSz holds the residual cipher-text
            memcpy(output, lastData, lastDataSz - assignedTagSz);    
            *output_length = lastDataSz - assignedTagSz;
            *tag_length = lastDataSz - *output_length;
            memcpy(tag, &lastData[*output_length], *tag_length);
        }else{
            memcpy(output, lastData, lastDataSz);    
            *output_length = lastDataSz ;  
        }
    } else{
        buffer_cleanup(output, output_size);
    }
    return psa_status;
}

psa_status_t psa_aead_finish(psa_aead_operation_t * operation, uint8_t * ciphertext,
                             size_t ciphertext_size, size_t * ciphertext_length, uint8_t * tag,
                             size_t tag_size, size_t * tag_length){

    psa_status_t psa_status = PSA_ERROR_CORRUPTION_DETECTED;

    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(operation->ctx.encrypt_decrypt != USS_ENCRYPT){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }

    psa_status = psa_aead_done(operation, ciphertext, ciphertext_size, ciphertext_length, tag,
                               tag_size, tag_length); 

exit:

    if(psa_status != PSA_SUCCESS){  
        buffer_cleanup(tag, tag_size);
    }
    psa_aead_abort(operation);
    return psa_status;
}

psa_status_t psa_aead_verify(psa_aead_operation_t * operation, uint8_t * plaintext,
                             size_t plaintext_size, size_t * plaintext_length,
                             const uint8_t * tag, size_t tag_length){

    psa_status_t psa_status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t calcTagLen = 0;  

    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if(operation->ctx.isActive == USS_FALSE || operation->ctx.isNonceSet == USS_FALSE
             || !ifCryptoInitDone() || operation->ctx.encrypt_decrypt != USS_DECRYPT){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }

    if(tag_length != operation->ciph_params.aead_tag_len){
        psa_status = PSA_ERROR_INVALID_SIGNATURE;
        goto exit;
    }

    psa_status = psa_aead_done(operation, plaintext, plaintext_size, plaintext_length,
                                (uint8_t *)tag, tag_length, &calcTagLen);

exit:
    if(psa_status != PSA_SUCCESS){
        buffer_cleanup(plaintext, plaintext_size);
    }
    psa_aead_abort(operation);
    return psa_status;
}

static psa_status_t psa_aead_encrypt_decrypt(uint8_t perform, psa_key_id_t key, psa_algorithm_t alg, const uint8_t * nonce,
                                             size_t nonce_length, const uint8_t * additional_data, size_t additional_data_length,
                                             const uint8_t * input, size_t input_length, uint8_t * output,
                                             size_t output_size, size_t * output_length){
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    size_t tag_size = 0, tag_length = 0, remainingDataSz = 0, min_output_len = 0;
    psa_aead_operation_t operation = psa_aead_operation_init();
    uint8_t tag[PSA_AEAD_TAG_MAX_SIZE];

    if(!PSA_ALG_IS_AEAD(alg) || nonce == NULL || input == NULL || output == NULL 
       || output_length == NULL || (additional_data_length > 0 && additional_data == NULL)
       || !check_aead_tag_len(alg)){
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    //Failing early
    if(additional_data_length > USS_PSA_AEAD_AAD_MAX_SIZE){
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if(perform == USS_ENCRYPT){
        psa_status = psa_aead_encrypt_setup(&operation, key, alg);
        min_output_len = PSA_AEAD_ENCRYPT_OUTPUT_SIZE(operation.key_attr.key_type, alg, input_length);

    }else{
        psa_status = psa_aead_decrypt_setup(&operation, key, alg);
        min_output_len = PSA_AEAD_DECRYPT_OUTPUT_SIZE(operation.key_attr.key_type, alg, input_length);
        input_length -= operation.ciph_params.aead_tag_len;
#ifndef USS_PSA_EMULATION
        memcpy(operation.ciph_params.aead_tag, (input + input_length), operation.ciph_params.aead_tag_len);
#endif
        memcpy(tag, (input + input_length), operation.ciph_params.aead_tag_len);
    }
    
    if(psa_status != PSA_SUCCESS){
        goto exit;
    }

    if(output_size < min_output_len){
        psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }

    psa_status = psa_aead_set_lengths(&operation, additional_data_length, input_length);

    if(psa_status != PSA_SUCCESS){
        goto exit;
    }
    
    psa_status = psa_aead_set_nonce(&operation, nonce, nonce_length);
    
    if(psa_status != PSA_SUCCESS){
        goto exit;
    }

    if(additional_data_length > 0){
        psa_status = psa_aead_update_ad(&operation, additional_data, additional_data_length);
        if(psa_status != PSA_SUCCESS){
            goto exit;
        }
    }

    psa_status = psa_aead_update(&operation, input, input_length, output, output_size, output_length);

    if(psa_status != PSA_SUCCESS){
        goto exit;
    }
    tag_size = operation.ciph_params.aead_tag_len;
    if(perform == USS_ENCRYPT){
        psa_status = psa_aead_finish(&operation, &output[*output_length], (output_size - *output_length), 
                                 &remainingDataSz, tag, tag_size, &tag_length);
    }else{
        psa_status = psa_aead_verify(&operation, &output[*output_length], (output_size - *output_length), 
                                 &remainingDataSz, tag, tag_size);
    }

exit:
    if(psa_status == PSA_SUCCESS){
        if(perform == USS_DECRYPT){
            *output_length += remainingDataSz;
        }else{
            memcpy(&output[*output_length + remainingDataSz], tag, tag_length);
            *output_length += remainingDataSz + tag_length;
        }
    }
    else{
        buffer_cleanup(output, output_size);
    }
    psa_aead_abort(&operation);
    return psa_status;
}

psa_status_t psa_aead_encrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * nonce,
                              size_t nonce_length, const uint8_t * additional_data,
                              size_t additional_data_length, const uint8_t * plaintext,
                              size_t plaintext_length, uint8_t * ciphertext,
                              size_t ciphertext_size, size_t * ciphertext_length){

    return (psa_aead_encrypt_decrypt(USS_ENCRYPT, key,  alg, nonce, nonce_length, additional_data,
                              additional_data_length, plaintext, plaintext_length, ciphertext, ciphertext_size,
                              ciphertext_length));
}

psa_status_t psa_aead_decrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * nonce,
                              size_t nonce_length, const uint8_t * additional_data, size_t additional_data_length,
                              const uint8_t * ciphertext, size_t ciphertext_length, uint8_t * plaintext,
                              size_t plaintext_size, size_t * plaintext_length){
    
    return (psa_aead_encrypt_decrypt(USS_DECRYPT, key,  alg, nonce, nonce_length, additional_data,
                              additional_data_length, ciphertext, ciphertext_length, plaintext,
                              plaintext_size, plaintext_length));
}


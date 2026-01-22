/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include "psa/crypto.h"
#include "MXQ_API.h"
#include "psa/crypto_cipher.h"
#include "psa/key_management.h"
#include "stdlib.h"


/**
 * \brief Initializes a cipher operation.
 *
 * This function initializes a `psa_cipher_operation_t` structure with default values.
 *
 * \return The initialized `psa_cipher_operation_t` structure.
 */
psa_cipher_operation_t psa_cipher_operation_init(void)
{
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    return operation;
}

static psa_status_t psa_cipher_encrypt_decrypt(psa_cipher_operation_t* operation, const uint8_t * input, size_t input_length, uint8_t * output, size_t output_size, size_t * output_length){
    mxq_err_t err;
    size_t local_len = 0;

    err = psa_cipher_update(operation, input, input_length, output, output_size, &local_len);
    if(err != PSA_SUCCESS)
    {
        return err;
    }
    
    err = psa_cipher_finish(operation, &output[local_len], output_size, output_length);
    if(err != PSA_SUCCESS)
    {
        return err;
    }
    *output_length += local_len;
    return PSA_SUCCESS;
}

/**
 * \brief Encrypts the input data using the specified key and algorithm.
 *
 * \param key The identifier of the key to be used for encryption.
 * \param alg The algorithm to be used for encryption.
 * \param input Pointer to the input data buffer.
 * \param input_length The length of the input data in bytes.
 * \param output Pointer to the output data buffer.
 * \param output_size The size of the output data buffer in bytes.
 * \param output_length Pointer to a variable to store the length of the output data.
 *
 * \return PSA_SUCCESS if the encryption operation was successful, or an error code otherwise.
 */
psa_status_t psa_cipher_encrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * input, size_t input_length, uint8_t * output, size_t output_size, size_t * output_length){
    
    psa_cipher_operation_t operation;
    uint8_t iv[PSA_CIPHER_IV_MAX_SIZE] = {0};
    psa_status_t psa_status;
    size_t iv_len = 0;

    operation = PSA_CIPHER_OPERATION_INIT;

    psa_status = psa_cipher_encrypt_setup(&operation, key, alg);
    if(psa_status != PSA_SUCCESS){
        return psa_status;
    }
    operation.ciph_params.iv_length = PSA_CIPHER_IV_LENGTH(operation.key_attr.key_type, alg);
    if(operation.ciph_params.iv_length > 0){
        psa_status = psa_cipher_generate_iv(&operation, iv, operation.ciph_params.iv_length, &operation.ciph_params.iv_length);
        if(psa_status != PSA_SUCCESS){
            return psa_status;
        }
        psa_status = psa_cipher_set_iv(&operation, iv, operation.ciph_params.iv_length);
        if(psa_status != PSA_SUCCESS){
            return psa_status;
        }
        // doing this because iv_length seems to be written to 0 by MAXQ. 
        iv_len = operation.ciph_params.iv_length;
        memcpy(output, iv, operation.ciph_params.iv_length);
    }
    psa_status = psa_cipher_encrypt_decrypt(&operation, input, input_length, output + operation.ciph_params.iv_length, output_size - operation.ciph_params.iv_length, output_length);
    *output_length += iv_len;
    return psa_status;

}

/**
 * \brief Decrypts the input data using the specified key and algorithm.
 *
 * \param key The identifier of the key to be used for decryption.
 * \param alg The algorithm to be used for decryption.
 * \param input Pointer to the input data to be decrypted.
 * \param input_length The length of the input data.
 * \param output Pointer to the output buffer to store the decrypted data.
 * \param output_size The size of the output buffer.
 * \param output_length Pointer to store the actual length of the decrypted data.
 *
 * \return PSA_SUCCESS if the decryption operation is successful, or an error code otherwise.
 */
psa_status_t psa_cipher_decrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t * input, size_t input_length, uint8_t * output, size_t output_size, size_t * output_length){
    psa_cipher_operation_t  operation;
    psa_status_t            psa_status;
    uint8_t iv[PSA_CIPHER_IV_MAX_SIZE] = {0};

    operation = PSA_CIPHER_OPERATION_INIT;
    psa_status = psa_cipher_decrypt_setup(&operation, key, alg);
    if(psa_status != PSA_SUCCESS){
        return psa_status;
    }
    operation.ciph_params.iv_length = PSA_CIPHER_IV_LENGTH(operation.key_attr.key_type, alg);
    if(operation.ciph_params.iv_length > 0){
        memcpy(iv, input, operation.ciph_params.iv_length);
        psa_status = psa_cipher_set_iv(&operation, iv, operation.ciph_params.iv_length);
        if(psa_status != PSA_SUCCESS){
            return psa_status;
        }
    }
    psa_status = psa_cipher_encrypt_decrypt(&operation, input + operation.ciph_params.iv_length, input_length - operation.ciph_params.iv_length, output, output_size, output_length);
    return psa_status;
}



static psa_status_t psa_cipher_setup(uint8_t type, psa_cipher_operation_t * operation, psa_key_id_t key, psa_algorithm_t alg)
{
    psa_status_t            psa_status;
    psa_key_attributes_t    attributes = {0};
    mxq_algo_id_t           mxq_algo = uss_psa_to_maxq_algo_id(alg);
    psa_key_usage_t         key_usage = 0;
    
    // Check if the crypto module is initialized
    if(!ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }
    
    // Determine the key usage based on the cipher operation type
    if(type == USS_ENCRYPT)
        key_usage = PSA_KEY_USAGE_ENCRYPT;
    else
        key_usage = PSA_KEY_USAGE_DECRYPT;

    // Get the key attributes
    psa_status = psa_get_key_attributes(key, &attributes);
    if(psa_status != PSA_SUCCESS){
        psa_status = PSA_ERROR_INVALID_HANDLE;
    } else if(!PSA_ALG_IS_CIPHER(alg)) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    }
    else if(operation->ctx.isActive == USS_TRUE){
        psa_status = PSA_ERROR_BAD_STATE;
    }else if( (attributes.key_use & key_usage) != key_usage ){
        psa_status = PSA_ERROR_NOT_PERMITTED;
    } else if (attributes.key_type != PSA_KEY_TYPE_AES){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    }else if(mxq_algo == ALGO_INVALID ){
        psa_status = PSA_ERROR_NOT_SUPPORTED;
    }else{
        // Set up the cipher operation
        buffer_cleanup((uint8_t *)operation, sizeof(psa_cipher_operation_t)); // Resetting the state
        operation->cipher_algo = mxq_algo;
        operation->key_attr.key_id = key;
        operation->key_attr.key_type = attributes.key_type;
        if(type == USS_DECRYPT)
            operation->ctx.encrypt_decrypt = USS_DECRYPT;
        else{
            operation->ctx.encrypt_decrypt = USS_ENCRYPT;
        }
    }
    
    // Activate the cipher operation if setup is successful
    if(psa_status == MXQ_OK){
        operation->ctx.isActive = USS_TRUE;
    }
    
exit:
    // Abort the cipher operation if setup fails
    if(psa_status != PSA_SUCCESS)
        psa_cipher_abort(operation);

    return psa_status;
}

/**
 * \brief Sets up a cipher operation with the specified parameters.
 *
 * This function initializes a cipher operation for encryption with the given type, 
 * key, and algorithm. It checks the validity of the parameters and sets up the 
 * operation accordingly.
 *
 * \param operation Pointer to the cipher operation structure to be set up.
 * \param key The key identifier for the cipher operation.
 * \param alg The algorithm to be used for the cipher operation.
 *
 * \return PSA_SUCCESS if the cipher operation is set up successfully.
 *         PSA_ERROR_INVALID_HANDLE if the key handle is invalid.
 *         PSA_ERROR_BAD_STATE if the cipher operation is already active.
 *         PSA_ERROR_NOT_PERMITTED if the key does not have the required usage permissions.
 *         PSA_ERROR_INVALID_ARGUMENT if the key type is not supported.
 *         PSA_ERROR_NOT_SUPPORTED if the algorithm is not supported.
 */
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t* operation, psa_key_id_t key, psa_algorithm_t alg)
{
    return psa_cipher_setup(USS_ENCRYPT, operation, key, alg);
}

/**
 * \brief Sets up a cipher operation with the specified parameters.
 *
 * This function initializes a cipher operation for encryption with the given type, 
 * key, and algorithm. It checks the validity of the parameters and sets up the 
 * operation accordingly.
 *
 * \param operation Pointer to the cipher operation structure to be set up.
 * \param key The key identifier for the cipher operation.
 * \param alg The algorithm to be used for the cipher operation.
 *
 * \return PSA_SUCCESS if the cipher operation is set up successfully.
 *         PSA_ERROR_INVALID_HANDLE if the key handle is invalid.
 *         PSA_ERROR_BAD_STATE if the cipher operation is already active.
 *         PSA_ERROR_NOT_PERMITTED if the key does not have the required usage permissions.
 *         PSA_ERROR_INVALID_ARGUMENT if the key type is not supported.
 *         PSA_ERROR_NOT_SUPPORTED if the algorithm is not supported.
 */
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t* operation, psa_key_id_t key, psa_algorithm_t alg)
{
    return psa_cipher_setup(USS_DECRYPT, operation, key, alg);

}

/**
 * \brief Sets the initialization vector (IV) for a cipher operation.
 *
 * \param operation     The cipher operation object.
 * \param iv            Pointer to the IV data.
 * \param iv_length     Length of the IV data in bytes.
 *
 * \return              PSA_SUCCESS if the IV is set successfully.
 *                      PSA_ERROR_BAD_STATE if the cipher operation is in an invalid state.
 *                      PSA_ERROR_INVALID_ARGUMENT if the IV length is invalid for the cipher algorithm.
 */
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t * operation, const uint8_t * iv, size_t iv_length)
{
    psa_status_t psa_status = PSA_SUCCESS;
    if(operation->cipher_algo == ALGO_CIPHER_AES_ECB || (!operation->ctx.isActive) || !ifCryptoInitDone() || operation->ctx.isIVSet == USS_TRUE){
        psa_status = PSA_ERROR_BAD_STATE;
    } else if (iv_length != PSA_CIPHER_IV_LENGTH(operation->key_attr.key_type, uss_maxq_to_psa_algo_id(operation->cipher_algo))){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    } 
    else
    {
        memcpy(operation->ctx.nonce, iv, iv_length);
        operation->ciph_params.p_iv = operation->ctx.nonce;
        operation->ciph_params.iv_length = iv_length;
        operation->ctx.isIVSet = USS_TRUE;
    }
    if(PSA_SUCCESS != psa_status)
    {
        psa_cipher_abort(operation);
    }

    return psa_status;
}

/**
 * \brief Generates an initialization vector (IV) for a cipher operation.
 *
 * \param operation The cipher operation context.
 * \param iv The buffer to store the generated IV.
 * \param iv_size The size of the IV buffer.
 * \param iv_length Pointer to a variable to store the number of output bytes written to the IV buffer.
 *
 * \return PSA_SUCCESS if the IV generation is successful.
 *         PSA_ERROR_BAD_STATE if the cipher algorithm is AES ECB, the operation context is not active,
 *                              the cryptographic module is not initialized, or the IV is already set.
 *         PSA_ERROR_BUFFER_TOO_SMALL if the IV buffer size is smaller than required.
 *         Other error codes indicating the cause of failure.
 */
psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t * operation, uint8_t * iv, size_t iv_size, size_t * iv_length) 
{
    mxq_err_t err;
    psa_status_t psa_status = PSA_SUCCESS;
    size_t iv_len_local;
	/* generate random iv here */
 	/* a change needs to be made to this function to write the number of output bytes written to the variable iv_length */
    if(operation->cipher_algo == ALGO_CIPHER_AES_ECB || (!operation->ctx.isActive) || !ifCryptoInitDone() || operation->ctx.isIVSet == USS_TRUE){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }
    if(iv_size < PSA_CIPHER_IV_LENGTH(operation->key_attr.key_type, uss_maxq_to_psa_algo_id(operation->cipher_algo))){
        psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }
    iv_len_local = PSA_CIPHER_IV_LENGTH(operation->key_attr.key_type, uss_maxq_to_psa_algo_id(operation->cipher_algo));
	err = MXQ_Get_Random(iv, iv_len_local);
    if(err != MXQ_OK){
        psa_status = uss_to_psa_error_code(err);
    }
    *iv_length = iv_len_local;

    exit:
    if (psa_status != PSA_SUCCESS) {
        psa_cipher_abort(operation);
    }
    return psa_status;
}

/**
 * \brief Updates a cipher operation with input data.
 *
 * This function updates a cipher operation with the provided input data. It performs the cipher operation on the input data and produces the output data.
 *
 * \param operation         The cipher operation context.
 * \param input             Pointer to the input data buffer.
 * \param input_length      Length of the input data buffer.
 * \param output            Pointer to the output data buffer.
 * \param output_size       Size of the output data buffer.
 * \param output_length     Pointer to the variable to store the length of the output data.
 *
 * \return                  PSA_SUCCESS if the operation was successful.
 *                          PSA_ERROR_INVALID_ARGUMENT if any of the input arguments are invalid.
 *                          PSA_ERROR_BAD_STATE if the cipher operation context is in an invalid state.
 *                          PSA_ERROR_BUFFER_TOO_SMALL if the output buffer is too small to hold the output data.
 *                          Other error codes indicating the specific error encountered during the operation.
 */
psa_status_t psa_cipher_update(psa_cipher_operation_t * operation, const uint8_t * input, size_t input_length, uint8_t * output, size_t output_size, size_t * output_length)
{
    mxq_err_t mxq_status;
    mxq_length mxq_input_length;
    psa_status_t psa_status = PSA_SUCCESS;
    mxq_input_length = input_length;

    // Add check for IV setting
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    }else if(operation->ctx.isActive == USS_FALSE || !ifCryptoInitDone() || 
            (!operation->ctx.isIVSet && (operation->cipher_algo != ALGO_CIPHER_AES_ECB))){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;        
    }else if(input == NULL || (output == NULL && output_size != 0) || output_length == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }else if(output_size < input_length){
        psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }else if(operation->ctx.isCipherInitDone == USS_FALSE){
        mxq_status = MXQ_Cipher_Init(operation->ctx.encrypt_decrypt, operation->cipher_algo, operation->key_attr.key_id,
								&operation->ciph_params, USS_FALSE);
        if (mxq_status){
            psa_status = uss_to_psa_error_code(mxq_status);
            goto exit;
        }
        
        operation->ctx.isCipherInitDone = USS_TRUE;
    }
    mxq_status = MXQ_Cipher_Update(&output, input, &mxq_input_length); 
    if (mxq_status){
        psa_status = uss_to_psa_error_code(mxq_status);
    }
    else
    {
        operation->ctx.processed_input_len += input_length;
        *output_length = mxq_input_length;
    }
    exit:
    if (psa_status != PSA_SUCCESS) {
        psa_cipher_abort(operation);
    }
    return psa_status;
}

/**
 * \brief Finish a cipher operation and retrieve the final output.
 *
 * This function is used to finish a cipher operation and retrieve the final output.
 *
 * \param operation     The cipher operation object.
 * \param output        Pointer to the buffer where the final output will be stored.
 * \param output_size   The size of the output buffer.
 * \param output_length Pointer to a variable that will hold the length of the final output.
 *
 * \return PSA_SUCCESS if the operation was successful.
 * \return PSA_ERROR_BAD_STATE if the cipher operation is not active or if the cryptographic module is not initialized.
 * \return Other error codes indicating the specific error encountered during the operation.
 */
psa_status_t psa_cipher_finish(psa_cipher_operation_t * operation, uint8_t * output, size_t output_size, size_t * output_length)
{
    psa_status_t psa_status;

    size_t length = 0;
    
    if(NULL == operation)
    {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if(output == NULL || output_length == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    } 
    else if(operation->ctx.isActive == USS_FALSE || !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;        
    } else if((operation->ctx.processed_input_len % PSA_BLOCK_CIPHER_BLOCK_LENGTH(operation->key_attr.key_type)) && (!PSA_ALG_IS_STREAM_CIPHER(uss_maxq_to_psa_algo_id(operation->cipher_algo))))
    {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if (output_size < PSA_CIPHER_FINISH_OUTPUT_SIZE(operation->key_attr.key_type, uss_maxq_to_psa_algo_id(operation->cipher_algo)))
    {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    
    else
    {
        psa_status = uss_to_psa_error_code(MXQ_Cipher_Finish(&output, &length));
        if (PSA_SUCCESS == psa_status)
        {
            *output_length = length;
        }
    }

    psa_cipher_abort(operation);
    return psa_status;
}

/**
 * \brief Aborts a cipher operation.
 *
 * This function aborts a cipher operation and resets the state of the operation.
 *
 * \param operation     Pointer to the cipher operation structure.
 *
 * \return              PSA_SUCCESS if the operation was successfully aborted.
 *                      PSA_ERROR_BAD_STATE if the operation is not active or if the cryptographic module is not initialized.
 *                      Other appropriate error codes indicating the reason for failure.
 */
psa_status_t psa_cipher_abort(psa_cipher_operation_t * operation)
{
    uint8_t          dummy_data[PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE];
    size_t      dummy_data_length = PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE;

    uint8_t * ptr_dummy_data = dummy_data; 

    if (operation->ctx.isActive == USS_FALSE) {
        return PSA_SUCCESS;
    }

    MXQ_Cipher_Finish(&ptr_dummy_data, &dummy_data_length);

    buffer_cleanup((uint8_t *)operation, sizeof(psa_cipher_operation_t)); // Resetting the state

    return PSA_SUCCESS;
}
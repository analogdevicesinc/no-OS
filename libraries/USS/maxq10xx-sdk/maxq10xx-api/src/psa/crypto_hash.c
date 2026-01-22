/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include <stdlib.h>
#include "psa/crypto_config_def.h"
#include "psa/crypto_errors.h"
#include "psa/crypto_hash.h"
#include "psa/crypto_common.h"
#include "MXQ_API.h"
#include "MXQ_Error.h"

psa_hash_operation_t psa_hash_operation_init(void){

    psa_hash_operation_t v = PSA_HASH_OPERATION_INIT;
    return v;
}

psa_status_t psa_hash_abort(psa_hash_operation_t * operation){
    if(operation == NULL || operation->isInitialized == false || operation->alg == PSA_ALG_NONE){
        return PSA_SUCCESS;
    }
    MXQ_MD_Finish(NULL, NULL);
    memset(operation, 0, sizeof(psa_hash_operation_t));
    return PSA_SUCCESS;
}

psa_status_t psa_hash_setup(psa_hash_operation_t * operation, psa_algorithm_t alg){

    mxq_algo_id_t maxq_algo;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    
    //Parameter sanitization
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if(PSA_ALG_IS_HASH(alg) == 0){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;       
        goto exit;
    } else if (operation->alg != PSA_ALG_NONE || operation->isInitialized ||
                !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
        goto exit;
    }

    memset(operation, 0, sizeof(psa_hash_operation_t));

    maxq_algo = uss_psa_to_maxq_algo_id(alg);

    if(maxq_algo == ALGO_INVALID){
        psa_status = PSA_ERROR_NOT_SUPPORTED;
    }else{
        psa_status = uss_to_psa_error_code(MXQ_MD_Init(maxq_algo));
    }

exit:
    if(PSA_SUCCESS == psa_status){
        operation->isInitialized = true;
        operation->alg = alg;
    }else{
       psa_hash_abort(operation);
    }
    return psa_status; 
}


psa_status_t psa_hash_update(psa_hash_operation_t * operation,
                             const uint8_t * input,
                             size_t input_length){
    
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
    //Parameter sanitization
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if(input_length != 0 && input == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    } else if (operation->alg == PSA_ALG_NONE || !operation->isInitialized || !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
    } else{
    //TODO: Add check for restriction on maximum input length, if any
        psa_status =  uss_to_psa_error_code(MXQ_MD_Update(input, input_length));
    }

    if(PSA_SUCCESS != psa_status){
        psa_hash_abort(operation);
    }
    
    return psa_status; 

}

psa_status_t psa_hash_finish(psa_hash_operation_t * operation,
                             uint8_t * hash,
                             size_t hash_size,
                             size_t * hash_length){
    size_t min_hash_sz = 0;
    psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;

    //Parameter sanitization
    if(operation == NULL){
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if (hash == NULL || hash_length == NULL){
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
    } else if (operation->alg == PSA_ALG_NONE || !operation->isInitialized || !ifCryptoInitDone()){
        psa_status = PSA_ERROR_BAD_STATE;
    } else{
        *hash_length = 0;
        min_hash_sz = PSA_HASH_LENGTH(operation->alg);
        if(hash_size < min_hash_sz){
            psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
            goto exit;
        }
    psa_status = uss_to_psa_error_code(MXQ_MD_Finish(hash, &hash_size));        
    }


exit:
    if(PSA_SUCCESS == psa_status){
        *hash_length = hash_size;
    }
    psa_hash_abort(operation);
    return psa_status;
}

psa_status_t psa_hash_verify(psa_hash_operation_t * operation,
                             const uint8_t * hash,
                             size_t hash_length){
    
    uint8_t calcHash[PSA_HASH_MAX_SIZE];
    psa_status_t ret_status = PSA_ERROR_GENERIC_ERROR;
    size_t cal_hashLen = 0;
    ret_status =  psa_hash_finish(operation, calcHash, sizeof(calcHash), &cal_hashLen);

    if(ret_status != PSA_SUCCESS){
        ret_status = (ret_status != PSA_ERROR_BUFFER_TOO_SMALL) ? ret_status : PSA_ERROR_INVALID_SIGNATURE;
    } else if (cal_hashLen != hash_length || ct_memcmp(hash, calcHash, hash_length) != 0){
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return ret_status;
}

psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              uint8_t * hash,
                              size_t hash_size,
                              size_t * hash_length){
    
    psa_status_t ret_status = PSA_ERROR_GENERIC_ERROR;        
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;

    if ((input == NULL && input_length != 0) || hash == NULL || hash_length == NULL){
        //Operation is not active and therefore it is safe to return without calling psa_hash_abort
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    *hash_length = 0;
    ret_status  = psa_hash_setup(&operation, alg);
    if (ret_status != PSA_SUCCESS){
        return ret_status;
    }
    ret_status  = psa_hash_update(&operation, input, input_length);
    if (ret_status != PSA_SUCCESS){
        return ret_status;
    }
    ret_status  = psa_hash_finish(&operation, hash, hash_size, hash_length);
    return ret_status;
}

psa_status_t psa_hash_compare(psa_algorithm_t alg,
                              const uint8_t * input,
                              size_t input_length,
                              const uint8_t * hash,
                              size_t hash_length){
    
    uint8_t calcHash[PSA_HASH_MAX_SIZE];
    psa_status_t ret_status = PSA_ERROR_GENERIC_ERROR;
    size_t calcHashLen = 0;
    ret_status = psa_hash_compute(alg, input, input_length, calcHash, hash_length, &calcHashLen);

    if(ret_status != PSA_SUCCESS){
        ret_status = (ret_status != PSA_ERROR_BUFFER_TOO_SMALL) ? ret_status : PSA_ERROR_INVALID_SIGNATURE;
        return ret_status;
    } else if (calcHashLen != hash_length || ct_memcmp(hash, calcHash, hash_length) != 0){
        return PSA_ERROR_INVALID_SIGNATURE;
    }  
    return ret_status;    
}

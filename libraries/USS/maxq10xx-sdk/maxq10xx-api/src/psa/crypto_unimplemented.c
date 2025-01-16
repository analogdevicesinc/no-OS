#include "psa/crypto.h"

/*
 * This file contains all the definitions of functions that are not supported by the current PSA implementation.  
 */

psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t * operation, psa_algorithm_t alg)                             
                                      
{
   (void)operation;
   (void)alg;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t * operation,
                                             size_t * capacity)                             
                                             
{
   (void)operation;
   (void)capacity;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t * operation, size_t capacity)                             
{
   (void) operation;
   (void) capacity;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t * operation,
                                            psa_key_derivation_step_t step,
                                            const uint8_t * data,
                                            size_t data_length)                             
{
   (void) operation;
   (void) step;
   (void) data;
   (void) data_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_input_integer(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              uint64_t value)                             
{
   (void) operation;
   (void) step;
   (void) value;
   return PSA_ERROR_NOT_SUPPORTED;
}
psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t * operation,
                                          psa_key_derivation_step_t step,
                                          psa_key_id_t key)                             
{
   (void) operation;
   (void) step;
   (void) key;
   return PSA_ERROR_NOT_SUPPORTED;
}
psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t * operation,
                                             uint8_t * output,
                                             size_t output_length)                             
{
   (void) operation;
   (void) output;
   (void) output_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_output_key(const psa_key_attributes_t * attributes,
                                           psa_key_derivation_operation_t * operation,
                                           psa_key_id_t * key)                             
{
   (void) attributes;
   (void) operation;
   (void) key;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_verify_bytes(psa_key_derivation_operation_t * operation,
                                             const uint8_t *expected_output,
                                             size_t output_length)                             
{
   (void) operation;
   (void) expected_output;
   (void) output_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_verify_key(psa_key_derivation_operation_t * operation,
                                           psa_key_id_t expected)                             
{
   (void) operation;
   (void) expected;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t * operation)                             
{
   (void) operation;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_asymmetric_encrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length)
{
   (void) key;
   (void) alg;
   (void) input;
   (void) input_length;
   (void) salt;
   (void) salt_length;
   (void) output;
   (void) output_size;
   (void) output_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_asymmetric_decrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t * input,
                                    size_t input_length,
                                    const uint8_t * salt,
                                    size_t salt_length,
                                    uint8_t * output,
                                    size_t output_size,
                                    size_t * output_length)
{
   (void) key;
   (void) alg;
   (void) input;
   (void) input_length;
   (void) salt;
   (void) salt_length;
   (void) output;
   (void) output_size;
   (void) output_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   psa_key_id_t private_key,
                                   const uint8_t * peer_key,
                                   size_t peer_key_length,
                                   uint8_t * output,
                                   size_t output_size,
                                   size_t * output_length)
{
   (void) alg;
   (void) private_key;
   (void) peer_key;
   (void) peer_key_length;
   (void) output;
   (void) output_size;
   (void) output;
   (void) output_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_copy_key(psa_key_id_t source_key,
                          const psa_key_attributes_t * attributes,
                          psa_key_id_t * target_key)
                          
{
   (void) source_key;
   (void) attributes;
   (void) target_key;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_purge_key(psa_key_id_t key)                          
{
   (void) key;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_export_key(psa_key_id_t key,
                            uint8_t * data,
                            size_t data_size,
                            size_t * data_length)
{
   (void) key;
   (void) data;
   (void) data_size;
   (void) data_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_key_agreement(psa_key_derivation_operation_t * operation,
                                              psa_key_derivation_step_t step,
                                              psa_key_id_t private_key,
                                              const uint8_t * peer_key,
                                              size_t peer_key_length)
{
   (void) operation;
   (void) step;
   (void) private_key;
   (void) peer_key;
   (void) peer_key_length;
   return PSA_ERROR_NOT_SUPPORTED;
}

psa_key_derivation_operation_t psa_key_derivation_operation_init(void)
{
    psa_key_derivation_operation_t v = PSA_KEY_DERIVATION_OPERATION_INIT;
    return v;
}

psa_status_t psa_hash_suspend(psa_hash_operation_t *   
                              operation,
                              uint8_t * hash_state,
                              size_t hash_state_size,
                              size_t * hash_state_length){
    (void) operation;
    (void) hash_state;
    (void) hash_state_size;
    (void) hash_state_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_hash_resume(psa_hash_operation_t *
                             operation,
                             const uint8_t * hash_state,
                             size_t hash_state_length)
{
    (void) operation;
    (void) hash_state;
    (void) hash_state_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_hash_clone(const psa_hash_operation_t *   
                            source_operation,
                            psa_hash_operation_t *  
                            target_operation)
{
    (void) source_operation;
    (void) target_operation;
    return PSA_ERROR_NOT_SUPPORTED;
}
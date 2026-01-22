/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include "psa/crypto_config_def.h"
#include "psa/crypto_errors.h"
#include "psa/crypto_common.h"
#include "psa/crypto_hash.h"
#include "psa/crypto_sign.h"
#include "psa/key_management.h"
#include "psa/crypto_aead.h"
#include "psa/crypto_mac.h"
#include "MXQ_API.h"

extern int fw_init(void);
#if defined(USS_PSA_AVAILABLE_ALG_AES_CCM) || defined(USS_PSA_AVAILABLE_ALG_AES_GCM)
/**
 * @brief Checks if the key policy for an AEAD algorithm is valid.
 *
 * This function checks if the key policy for a given AEAD algorithm is valid.
 *
 * @param key_algo The MXQ algorithm ID of the key.
 * @param psa_algo The PSA algorithm ID of the AEAD algorithm.
 * @return true if the key policy is valid, false otherwise.
 */
static bool check_aead_key_policy(mxq_algo_id_t key_algo, psa_algorithm_t psa_algo);
#endif
bool isCryptoInitDone = false;

psa_status_t psa_crypto_init(void){
    mxq_err_t status = MXQ_ERR_TRANS_TIMEOUT;
#ifdef USS_PSA_EMULATION
    static bool init_done = false;
    if(!init_done){

        status = fw_init();
        if(status == MXQ_OK){
            init_done = true;
        }else{
            return PSA_ERROR_COMMUNICATION_FAILURE;
        }
    }
#endif //USS_PSA_EMULATION
    if((status = MXQ_Module_Init()) == MXQ_OK){
        isCryptoInitDone = true;
        return PSA_SUCCESS;
    }
    
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

bool ifCryptoInitDone(void){
    return isCryptoInitDone;
}

uint8_t ct_memcmp(const void *s1, const void *s2, size_t n){
    const unsigned char *p1 =(const unsigned char *) s1;
    const unsigned char *p2 =(const unsigned char *) s2;
    unsigned char c1, c2;
    uint8_t diff = 0;
    while (n-- > 0) {
        c1 = *p1++;
        c2 = *p2++;
        diff |= c1 ^ c2;
    }
    return diff;
}

psa_status_t uss_to_psa_error_code(mxq_err_t retValue){
    switch (retValue)
    {
    case MXQ_OK:
        return PSA_SUCCESS;

    case MXQ_RSP_ERROR_UNKNOWN:
    case MXQ_RSP_OTHER_UNKNOWN_ERROR:
    case MXQ_ERR_INTERNAL_ERROR:
    case MXQ_ERR_GCU_UNKNOWN_STATUS:
        return PSA_ERROR_GENERIC_ERROR;

    case MXQ_RSP_FS_INVALID_OBJECT:
    case MXQ_RSP_OBJECT_EMPTY:
    case MXQ_RSP_BAD_KEY_TYPE:
        return PSA_ERROR_INVALID_HANDLE;

    //case MXQ_RSP_BAD_PARAM: Same as value MXQ_RSP_ID_ALREADY_EXISTS
    case MXQ_RSP_BAD_OBJ_ACCESS:
    case MXQ_RSP_FS_WRONG_OBJECT_TYPE:
    //case MXQ_RSP_WRONG_LENGTH: Same value as MXQ_RSP_OTHER_UNKNOWN_ERROR
    case MXQ_RSP_CIPHER_BAD_INPUT_DATA:
    case MXQ_RSP_ERR_WRONG_MODE:
    case MXQ_RSP_WRONG_PARAMS:
    case MXQ_RSP_WRONG_SIZE:
    case MXQ_RSP_CONSISTENCY_ERROR:
    case MXQ_ERR_WRONG_PARAMETER:
        return PSA_ERROR_INVALID_ARGUMENT;
    
    case MXQ_RSP_NOT_FOUND:
    //case MXQ_RSP_NO_HASH_AVAIL: Same value as MXQ_RSP_ACCESS_DENIED
        return PSA_ERROR_DOES_NOT_EXIST;
    
    case MXQ_RSP_ID_ALREADY_EXISTS:
    case MXQ_RSP_FS_NAME_ALREADY_EXISTS:
        return PSA_ERROR_ALREADY_EXISTS;
    
    case MXQ_RSP_FS_WRONG_PARAM_ACCESS_COND:
    case MXQ_RSP_ACCESS_DENIED:
    case MXQ_RSP_NOT_ALLOWED:
    case MXQ_RSP_ACC_DENIED:
        return PSA_ERROR_NOT_PERMITTED;
    
    case MXQ_RSP_FS_OUT_OF_BOUNDS:
    case MXQ_RSP_MEMORY_FULL:
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    
    case MXQ_RSP_SIGNATURE_IS_WRONG:
    case MXQ_RSP_WRONG_MAC:
        return PSA_ERROR_INVALID_SIGNATURE;

    case MXQ_RSP_SC_ERROR:
        return PSA_ERROR_CORRUPTION_DETECTED;

    case MXQ_RSP_ERR_NOT_IMPLEMENTED:
    case MXQ_RSP_ALGO_NOT_SUPPORTED:
    case MXQ_RSP_CURVE_NOT_SUPPORTED:
    case MXQ_RSP_INVALID_SIZE:
    case MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE:
    case MXQ_ERR_MODE:
        return PSA_ERROR_NOT_SUPPORTED;

    case MXQ_RSP_CR_NOK:
    case MXQ_ERR_GCU_ERROR_STATUS:
        return PSA_ERROR_HARDWARE_FAILURE;
    
    case MXQ_RSP_CRYPTO_FAILURE:
    case MXQ_RSP_COUNTER_OVERFLOW:
        return PSA_ERROR_SERVICE_FAILURE;

    case MXQ_RSP_BUFFER_TOO_SMALL:
    case MXQ_RSP_HASH_OUTPUT_LENGTH_TOO_SHORT:
        return PSA_ERROR_BUFFER_TOO_SMALL;

    case MXQ_RSP_INTERNAL_BUFFER_OVERFLOW:
    case MXQ_RSP_BUFFER_OVERFLOW:
    case MXQ_ERR_TRANS_BUFFER_OVERFLOW:
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    
    case MXQ_RSP_CR_PADDING_ERROR:
        return PSA_ERROR_INVALID_PADDING;

    //case MXQ_ERR_TRANS_ERROR: Same value as MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE
    case MXQ_ERR_TRANS_TIMEOUT:
    case MXQ_ERR_GCU_TIMEOUT:
        return PSA_ERROR_COMMUNICATION_FAILURE;
    
    case MXQ_ERR_TRANS_NOT_ENOUGH_DATA:
        return PSA_ERROR_INSUFFICIENT_DATA;

    case MXQ_ERR_TRANS_BAD_CRC:
    case MXQ_ERR_TRANS_RX_BAD_FORMAT:
    case MXQ_ERR_SECURE_CHANNEL_INTEGRITY:
        return PSA_ERROR_DATA_INVALID;

    //case MXQ_RSP_NO_RANDOM_AVAIL: Same value as MXQ_RSP_WRONG_MAC
    //    return PSA_ERROR_INSUFFICIENT_ENTROPY;

    default:
        return PSA_ERROR_GENERIC_ERROR;
    }

}

mxq_keytype_id_t uss_psa_to_maxq_key_type(psa_key_type_t psa_key_type)
{
    switch(psa_key_type)
    {
        case PSA_KEY_TYPE_RAW_DATA:
            return MXQ_KEYTYPE_IKM;
        case PSA_KEY_TYPE_AES:
            return MXQ_KEYTYPE_AES;
        case PSA_KEY_TYPE_HMAC:
            return MXQ_KEYTYPE_HMAC;
        case PSA_KEY_TYPE_RSA_KEY_PAIR:
            return MXQ_KEYTYPE_RSA;
        case PSA_KEY_TYPE_RSA_PUBLIC_KEY:
            return MXQ_KEYTYPE_RSA;
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1): /* PSA APIs will handle the key size */
            return MXQ_KEYTYPE_ECC;
        case PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1):
            return MXQ_KEYTYPE_ECC;
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1): /* PSA APIs will handle the key size */
            return MXQ_KEYTYPE_ECC;
        case PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1):
            return MXQ_KEYTYPE_ECC;
        case PSA_KEY_TYPE_DERIVE:
        case PSA_KEY_TYPE_PASSWORD:
        case PSA_KEY_TYPE_PASSWORD_HASH:
        case PSA_KEY_TYPE_PEPPER:
        default: 
            return MXQ_KEYTYPE_UNKNOWN;
    }
}

mxq_object_prop_t uss_psa_to_maxq_key_lifetime(psa_key_lifetime_t psa_key_lifetime)
{
    switch(psa_key_lifetime)
    {
        case PSA_KEY_LIFETIME_VOLATILE:
            return OBJPROP_TEMPORARY;
        case PSA_KEY_LIFETIME_PERSISTENT:
            return OBJPROP_PERSISTENT;
        /* 
         * Persistent key with a vendor-specified lifetime.
         * No PSA Certified API will define the meaning of these values, 
         * so implementations may choose the meaning freely. 
         * As a guideline, higher persistence levels should cause a 
         * key to survive more management events than lower levels. 
         */
        case USS_PSA_KEY_LIFETIME_WIPE_ON_TAMPER:
            return OBJPROP_WIPE_ON_TAMPER;
        case USS_PSA_KEY_LIFETIME_WIPE_ON_TAMPER | PSA_KEY_LIFETIME_PERSISTENT:
                    return OBJPROP_WIPE_ON_TAMPER | OBJPROP_PERSISTENT;
        default: 
            return PSA_KEY_PERSISTENCE_DEFAULT;
    }
}

mxq_algo_id_t uss_psa_to_maxq_algo_id(psa_algorithm_t psa_algo_type)
{

#if defined(USS_PSA_AVAILABLE_ALG_AES_CCM) || defined(USS_PSA_AVAILABLE_ALG_AES_GCM)
    //MAXQ implementation have single algo for all different algos which are derived
    //for different tag size from a given AEAD algo. Therefore, need to extract the 
    //parent/default algo
    if(PSA_ALG_IS_AEAD(psa_algo_type)){
        psa_algo_type = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(psa_algo_type);
    }
#endif

    if(PSA_ALG_IS_MAC(psa_algo_type)){
        psa_algo_type = PSA_ALG_FULL_LENGTH_MAC(psa_algo_type);
    }

    switch(psa_algo_type)
    {
#ifdef USS_PSA_AVAILABLE_ALG_SHA256
        case PSA_ALG_SHA_256:
            return ALGO_MD_SHA256;
#endif //USS_PSA_AVAILABLE_ALG_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_SHA384
        case PSA_ALG_SHA_384:
            return ALGO_MD_SHA384;
#endif //USS_PSA_AVAILABLE_ALG_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_SHA512
        case PSA_ALG_SHA_512:
            return ALGO_MD_SHA512;    
#endif //USS_PSA_AVAILABLE_ALG_SHA512

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA256
        case PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256):
            return ALGO_RSASSAPSSPKCS1_V1_5_SHA256;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA384
        case PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_384):
            return ALGO_RSASSAPSSPKCS1_V1_5_SHA384;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA512
        case PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_512):
            return ALGO_RSASSAPSSPKCS1_V1_5_SHA512;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA512

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15
        case PSA_ALG_RSA_PKCS1V15_SIGN_RAW:
            return ALGO_RSASSAPSSPKCS1_V1_5_PLAIN;

        case PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH):   //Only used to set key-algo policy
            return ALGO_RSASIGN_ANY;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS
        case PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH):             //Only used to set key-algo policy
        case PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_ANY_HASH):
            return ALGO_RSASIGN_ANY;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA256
        case PSA_ALG_RSA_PSS(PSA_ALG_SHA_256):
        case PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256):
            return ALGO_RSASSAPSSPKCS1_V2_1_SHA256;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA384
        case PSA_ALG_RSA_PSS(PSA_ALG_SHA_384):
        case PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_384):
            return ALGO_RSASSAPSSPKCS1_V2_1_SHA384;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA512
        case PSA_ALG_RSA_PSS(PSA_ALG_SHA_512):
        case PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_512):
            return ALGO_RSASSAPSSPKCS1_V2_1_SHA512;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA512

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA_SHA256
        case PSA_ALG_ECDSA(PSA_ALG_SHA_256):
            return ALGO_ECDSA_SHA_256;
#endif //USS_PSA_AVAILABLE_ALG_ECDSA_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA_SHA384
        case PSA_ALG_ECDSA(PSA_ALG_SHA_384):
            return ALGO_ECDSA_SHA_384;
#endif //USS_PSA_AVAILABLE_ALG_ECDSA_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA_SHA512
        case PSA_ALG_ECDSA(PSA_ALG_SHA_512):
            return ALGO_ECDSA_SHA_512;
#endif //USS_PSA_AVAILABLE_ALG_ECDSA_SHA512

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA
        case PSA_ALG_ECDSA(PSA_ALG_ANY_HASH):               //Only used to set key-algo policy
            return ALGO_ECDSA_SHA_any;

        case PSA_ALG_ECDSA_ANY:
            return ALGO_ECDSA_PLAIN;
#endif //USS_PSA_AVAILABLE_ALG_ECDSA

        case PSA_ALG_PURE_EDDSA:        //Not supported yet
        case PSA_ALG_ED25519PH:
        case PSA_ALG_ED448PH:
            return ALGO_INVALID;
#if defined(USS_PSA_AVAILABLE_ALG_SHA256)
        case PSA_ALG_HMAC(PSA_ALG_SHA_256):
            return ALGO_HMAC_SHA256;
#endif //USS_PSA_AVAILABLE_ALG_SHA256

#if defined(USS_PSA_AVAILABLE_ALG_SHA384)
        case PSA_ALG_HMAC(PSA_ALG_SHA_384):
            return ALGO_HMAC_SHA384;
#endif //USS_PSA_AVAILABLE_ALG_SHA384

#if defined(USS_PSA_AVAILABLE_ALG_SHA512)
        case PSA_ALG_HMAC(PSA_ALG_SHA_512):
            return ALGO_HMAC_SHA512;
#endif //USS_PSA_AVAILABLE_ALG_SHA512

        case PSA_ALG_CBC_MAC:
            return ALGO_MAC_AES_CBC_MAC;

        case PSA_ALG_CMAC:
            return ALGO_MAC_AES_CMAC;

#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
        case PSA_ALG_CCM:
            return ALGO_CIPHER_AES_CCM;
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM

#ifdef USS_PSA_AVAILABLE_ALG_AES_GCM
        case PSA_ALG_GCM:
            return ALGO_CIPHER_AES_GCM;
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM

#ifdef USS_PSA_AVAILABLE_ALG_CHACHA20_POLY1305
        case PSA_ALG_CHACHA20_POLY1305:
        case PSA_ALG_XCHACHA20_POLY1305:
            return ALGO_INVALID;
#endif //USS_PSA_AVAILABLE_ALG_CHACHA20_POLY1305
            
        case PSA_ALG_ECB_NO_PADDING:
            return ALGO_CIPHER_AES_ECB;
        case PSA_ALG_CBC_NO_PADDING:
            return ALGO_CIPHER_AES_CBC;
#ifdef USS_PSA_AVAILABLE_ALG_AES_CTR
        case PSA_ALG_CTR:
            return ALGO_CIPHER_AES_CTR;
#endif
        default: 
            return ALGO_INVALID;
    }
}

mxq_keyuse_t uss_psa_to_maxq_keyuse(psa_key_usage_t psa_key_use)
{
    switch(psa_key_use)
    {
        case PSA_KEY_USAGE_ENCRYPT:
            return MXQ_KEYUSE_ENCRYPTION;
        case PSA_KEY_USAGE_DECRYPT:
            return MXQ_KEYUSE_ENCRYPTION;
        case (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT):
            return MXQ_KEYUSE_ENCRYPTION;
        case PSA_KEY_USAGE_SIGN_MESSAGE:
            return MXQ_KEYUSE_DATASIGNATURE;
        case PSA_KEY_USAGE_VERIFY_MESSAGE: 
            return MXQ_KEYUSE_DATASIGNATURE;
        case (PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE):
            return MXQ_KEYUSE_DATASIGNATURE;
        case PSA_KEY_USAGE_SIGN_HASH: 
            return MXQ_KEYUSE_DATASIGNATURE;
        case PSA_KEY_USAGE_VERIFY_HASH: 
            return MXQ_KEYUSE_DATASIGNATURE;
        case (PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH):
            return MXQ_KEYUSE_DATASIGNATURE;
        case PSA_KEY_USAGE_DERIVE: 
            return MXQ_KEYUSE_TLSAUTH;
        case PSA_KEY_USAGE_VERIFY_DERIVATION: 
            return MXQ_KEYUSE_TLSAUTH;
        default: 
            return MXQ_KEYUSE_NONE;
    }
}

mxq_objecttype_t uss_psa_to_maxq_object_type(psa_key_type_t psa_key_type)
{
    switch(psa_key_type)
    {
        case PSA_KEY_TYPE_RAW_DATA:
            return MXQ_OBJTYPE_TRANSPARENT;
        case PSA_KEY_TYPE_AES:
            return MXQ_OBJTYPE_SECRETKEY;
        case PSA_KEY_TYPE_HMAC:
            return MXQ_OBJTYPE_SECRETKEY;
        case PSA_KEY_TYPE_RSA_KEY_PAIR:
            return MXQ_OBJTYPE_KEYPAIR;
        case PSA_KEY_TYPE_RSA_PUBLIC_KEY:
            return MXQ_OBJTYPE_PUBKEY;
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1): /* PSA APIs will handle the key size */
            return MXQ_OBJTYPE_KEYPAIR;
        case PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1):
            return MXQ_OBJTYPE_PUBKEY;
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1): /* PSA APIs will handle the key size */
            return MXQ_OBJTYPE_KEYPAIR;
        case PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1):
            return MXQ_OBJTYPE_PUBKEY;
        default: 
            return MXQ_OBJTYPE_UNKNOWN;
    }
}

psa_algorithm_t uss_maxq_to_psa_algo_id(mxq_algo_id_t mxq_algo_type)
{
    switch(mxq_algo_type)
    {
#ifdef USS_PSA_AVAILABLE_ALG_SHA256
        case ALGO_MD_SHA256:
            return PSA_ALG_SHA_256;
#endif //USS_PSA_AVAILABLE_ALG_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_SHA384
        case ALGO_MD_SHA384:
            return PSA_ALG_SHA_384;
#endif //USS_PSA_AVAILABLE_ALG_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_SHA512
        case ALGO_MD_SHA512:
            return PSA_ALG_SHA_512;    
#endif //USS_PSA_AVAILABLE_ALG_SHA512

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA256
        case ALGO_RSASSAPSSPKCS1_V1_5_SHA256:
            return PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA384
        case ALGO_RSASSAPSSPKCS1_V1_5_SHA384:
            return PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_384);
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA512
        case ALGO_RSASSAPSSPKCS1_V1_5_SHA512:
            return PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_512);
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA512

#if defined(USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15) || defined(USS_PSA_AVAILABLE_ALG_RSA_PSS)
        case ALGO_RSASSAPSSPKCS1_V1_5_PLAIN:
            return PSA_ALG_RSA_PKCS1V15_SIGN_RAW;

        case ALGO_RSASIGN_ANY:   //Only used to set key-algo policy
            return PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_ANY_HASH);
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA256
        case ALGO_RSASSAPSSPKCS1_V2_1_SHA256:
            return PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256);
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA384
        case ALGO_RSASSAPSSPKCS1_V2_1_SHA384:
            return PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_384);
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA512
        case ALGO_RSASSAPSSPKCS1_V2_1_SHA512:
            return PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_512);
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA512

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA_SHA256
        case ALGO_ECDSA_SHA_256:
            return PSA_ALG_ECDSA(PSA_ALG_SHA_256);
#endif //USS_PSA_AVAILABLE_ALG_ECDSA_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA_SHA384
        case ALGO_ECDSA_SHA_384:
            return PSA_ALG_ECDSA(PSA_ALG_SHA_384);
#endif //USS_PSA_AVAILABLE_ALG_ECDSA_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA_SHA512
        case ALGO_ECDSA_SHA_512:
            return PSA_ALG_ECDSA(PSA_ALG_SHA_512);
#endif //USS_PSA_AVAILABLE_ALG_ECDSA_SHA512

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA
        case ALGO_ECDSA_SHA_any:               //Only used to set key-algo policy
            return PSA_ALG_ECDSA(PSA_ALG_ANY_HASH);

        case ALGO_ECDSA_PLAIN:
            return PSA_ALG_ECDSA_ANY;
#endif //USS_PSA_AVAILABLE_ALG_ECDSA

#ifdef USS_PSA_AVAILABLE_ALG_SHA256
        case ALGO_HMAC_SHA256:
            return PSA_ALG_HMAC(PSA_ALG_SHA_256);
#endif //USS_PSA_AVAILABLE_ALG_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_SHA384
        case ALGO_HMAC_SHA384:
            return PSA_ALG_HMAC(PSA_ALG_SHA_384);
#endif //USS_PSA_AVAILABLE_ALG_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_SHA512
        case ALGO_HMAC_SHA512:
            return PSA_ALG_HMAC(PSA_ALG_SHA_512);
#endif //USS_PSA_AVAILABLE_ALG_SHA512

        case ALGO_MAC_AES_CBC_MAC:
            return PSA_ALG_CBC_MAC;

        case ALGO_MAC_AES_CMAC:
            return PSA_ALG_CMAC;

#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
        case ALGO_CIPHER_AES_CCM:
            return PSA_ALG_CCM;
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM

#ifdef USS_PSA_AVAILABLE_ALG_AES_GCM
        case ALGO_CIPHER_AES_GCM:
            return PSA_ALG_GCM;
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM

        case ALGO_CIPHER_AES_ECB:
            return PSA_ALG_ECB_NO_PADDING;
        case ALGO_CIPHER_AES_CBC:
            return PSA_ALG_CBC_NO_PADDING;
#ifdef USS_PSA_AVAILABLE_ALG_AES_CTR
        case ALGO_CIPHER_AES_CTR:
            return PSA_ALG_CTR;
#endif
        default: 
            return PSA_ALG_NONE;
    }
}

psa_key_usage_t uss_maxq_to_psa_keyuse(mxq_keyuse_t mxq_key_use)
{
    switch(mxq_key_use)
    {
        case MXQ_KEYUSE_ENCRYPTION:
            return (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
        case MXQ_KEYUSE_DATASIGNATURE:
            return (PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE | 
                PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);
        default: 
            return 0; //no specified key usage exists
    }
}

psa_key_type_t uss_maxq_to_psa_key_type(mxq_keytype_id_t mxq_key_type, mxq_keyparam_id_t params, mxq_objecttype_t mxq_object_type)
{
    switch(mxq_key_type)
    {
        case MXQ_KEYTYPE_IKM :
            return PSA_KEY_TYPE_RAW_DATA;
        case MXQ_KEYTYPE_AES:
            return PSA_KEY_TYPE_AES;
        case MXQ_KEYTYPE_HMAC:
            return PSA_KEY_TYPE_HMAC;
        case MXQ_KEYTYPE_RSA:
            if(MXQ_OBJTYPE_KEYPAIR == mxq_object_type)
            {
                return PSA_KEY_TYPE_RSA_KEY_PAIR;
            }
            else if(MXQ_OBJTYPE_PUBKEY == mxq_object_type)
            {
                return PSA_KEY_TYPE_RSA_PUBLIC_KEY; 
            }
            else 
            {
                return PSA_KEY_TYPE_NONE;
            }
        case MXQ_KEYTYPE_ECC:
            if(MXQ_OBJTYPE_KEYPAIR == mxq_object_type)
            {
                if(MXQ_KEYPARAM_EC_BP256R1 == params || MXQ_KEYPARAM_EC_BP384R1 == params || MXQ_KEYPARAM_EC_BP512R1 == params)
                {
                    return PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1);
                }
                else if(MXQ_KEYPARAM_EC_P256R1 == params || MXQ_KEYPARAM_EC_P384R1 == params || MXQ_KEYPARAM_EC_P521R1 == params)
                {
                    return PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
                }
                else
                {
                    return PSA_KEY_TYPE_NONE;
                }
            }
            else if(MXQ_OBJTYPE_PUBKEY == mxq_object_type)
            {
                if(MXQ_KEYPARAM_EC_BP256R1 == params || MXQ_KEYPARAM_EC_BP384R1 == params || MXQ_KEYPARAM_EC_BP512R1 == params)
                {
                    return PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1);
                }
                else if(MXQ_KEYPARAM_EC_P256R1 == params || MXQ_KEYPARAM_EC_P384R1 == params || MXQ_KEYPARAM_EC_P521R1 == params)
                {
                    return PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);
                }
                else
                {
                    return PSA_KEY_TYPE_NONE;
                }
            }
            else 
            {
                return PSA_KEY_TYPE_NONE;
            }

        default: 
            return PSA_KEY_TYPE_NONE;
    }
}

psa_status_t psa_generate_random(uint8_t * output, size_t output_size)
{
    mxq_err_t err = MXQ_OK;

    // check if psa crypto init is done
    if(!ifCryptoInitDone())
    {
        return PSA_ERROR_BAD_STATE;
    }

    //TODO: Verify output and output buffer size
    err = MXQ_Get_Random(output, output_size);
    if(MXQ_OK == err)
    {
        return PSA_SUCCESS;
    }
    return PSA_ERROR_GENERIC_ERROR;
}

mxq_algo_id_t uss_sign_algo_from_psa(psa_algorithm_t psa_algo, bool isMsg){
    psa_algorithm_t hash_algo =  PSA_ALG_GET_HASH(psa_algo); 

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15
    if(PSA_ALG_IS_RSA_PKCS1V15_SIGN(psa_algo) && ((psa_algo & USS_PSA_ALG_HASH_MASK) != 0)){ //RSA_PKCS1V15_SIGN
    
        return ((isMsg && hash_algo == PSA_ALG_SHA_256) ? ALGO_RSASSAPSSPKCS1_V1_5_SHA256  :\
                (isMsg && hash_algo == PSA_ALG_SHA_384) ? ALGO_RSASSAPSSPKCS1_V1_5_SHA384  :\
                (isMsg && hash_algo == PSA_ALG_SHA_512) ? ALGO_RSASSAPSSPKCS1_V1_5_SHA512  :\
                (!isMsg && hash_algo == PSA_ALG_SHA_256) ? ALGO_RSASSAPSSPKCS1_V1_5_PLAIN  :\
                (!isMsg && hash_algo == PSA_ALG_SHA_384) ? ALGO_RSASSAPSSPKCS1_V1_5_PLAIN  :\
                (!isMsg && hash_algo == PSA_ALG_SHA_512) ? ALGO_RSASSAPSSPKCS1_V1_5_PLAIN  :\
                ALGO_INVALID);
    }
    if(PSA_ALG_RSA_PKCS1V15_SIGN_RAW == psa_algo && !isMsg){  //RSA_PKCS1V15_SIGN_RAW
        return ALGO_RSASSAPSSPKCS1_V1_5_PLAIN;
    }
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS
    if(PSA_ALG_IS_RSA_PSS(psa_algo)){                         //RSA_PSS, PSA_ALG_RSA_PSS_ANY_SALT

        return ((isMsg && hash_algo == PSA_ALG_SHA_256) ? ALGO_RSASSAPSSPKCS1_V2_1_SHA256  :\
                (isMsg && hash_algo == PSA_ALG_SHA_384) ? ALGO_RSASSAPSSPKCS1_V2_1_SHA384  :\
                (isMsg && hash_algo == PSA_ALG_SHA_512) ? ALGO_RSASSAPSSPKCS1_V2_1_SHA512  :\
                (!isMsg && hash_algo == PSA_ALG_SHA_256) ? ALGO_RSASSAPSSPKCS1_V2_1_PLAIN  :\
                (!isMsg && hash_algo == PSA_ALG_SHA_384) ? ALGO_RSASSAPSSPKCS1_V2_1_PLAIN  :\
                (!isMsg && hash_algo == PSA_ALG_SHA_512) ? ALGO_RSASSAPSSPKCS1_V2_1_PLAIN  :\
                ALGO_INVALID);
    }
#endif //USS_PSA_AVAILABLE_ALG_RSA

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA
    if(PSA_ALG_IS_RANDOMIZED_ECDSA(psa_algo) && ((psa_algo & USS_PSA_ALG_HASH_MASK) != 0)){     //ECDSA

        return ((isMsg && hash_algo == PSA_ALG_SHA_256) ? ALGO_ECDSA_SHA_256              :\
                (isMsg && hash_algo == PSA_ALG_SHA_384) ? ALGO_ECDSA_SHA_384              :\
                (isMsg && hash_algo == PSA_ALG_SHA_512) ? ALGO_ECDSA_SHA_512              :\
                (!isMsg && hash_algo == PSA_ALG_SHA_256) ? ALGO_ECDSA_PLAIN               :\
                (!isMsg && hash_algo == PSA_ALG_SHA_384) ? ALGO_ECDSA_PLAIN               :\
                (!isMsg && hash_algo == PSA_ALG_SHA_512) ? ALGO_ECDSA_PLAIN               :\
                ALGO_INVALID);
    }
    if(PSA_ALG_ECDSA_ANY == psa_algo && !isMsg){
        return ALGO_ECDSA_PLAIN;
    }
    if(PSA_ALG_IS_DETERMINISTIC_ECDSA(psa_algo)){ //Not supported
        return ALGO_INVALID;
    }
#endif //USS_PSA_AVAILABLE_ALG_ECDSA
    if(PSA_ALG_PURE_EDDSA == psa_algo || PSA_ALG_IS_HASH_EDDSA(psa_algo)){    //Not supported
        return ALGO_INVALID;
    }else{
        return ALGO_INVALID; 
    }

}

bool check_key_policy(psa_algorithm_t psa_algo, psa_key_attributes_t *attr){
    bool allowed = false;
    
    //psa_algo should not have Wildcard value as this is only allowed while 
    //setting the key algo policy
    switch(uss_psa_to_maxq_algo_id(attr->key_algo)){
#ifdef USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15
        case ALGO_RSASSAPSSPKCS1_V1_5_SHA256:
            allowed = (psa_algo == PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256));
            break;

        case ALGO_RSASSAPSSPKCS1_V1_5_SHA384:
            allowed = (psa_algo == PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_384));
            break;

        case ALGO_RSASSAPSSPKCS1_V1_5_SHA512:
            allowed = (psa_algo == PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_512));
            break;

        case ALGO_RSASSAPSSPKCS1_V1_5_PLAIN:
            allowed = (psa_algo == PSA_ALG_RSA_PKCS1V15_SIGN_RAW);
            break;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15

#if defined (USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15) || defined(USS_PSA_AVAILABLE_ALG_RSA_PSS)
        case ALGO_RSASIGN_ANY:      //Mapping both RSA_PKCS1V15 and RSA_PSS to ALGO_RSASIGN_ANY cuz
                                    //underlying implementation has same key policy for both the implementation
            allowed = (PSA_ALG_IS_RSA_PKCS1V15_SIGN(psa_algo) ||    //The wildcard key policy PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH) also permits a key to be used with    
                       PSA_ALG_IS_RSA_PSS(psa_algo));               //the PSA_ALG_RSA_PKCS1V15_SIGN_RAW signature algorithm.                
            break;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15 USS_PSA_AVAILABLE_ALG_RSA_PSS

#ifdef USS_PSA_AVAILABLE_ALG_RSA_PSS
        case ALGO_RSASSAPSSPKCS1_V2_1_SHA256:
            allowed = (psa_algo == PSA_ALG_RSA_PSS(PSA_ALG_SHA_256) || 
                       psa_algo == PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256));
            break;

        case ALGO_RSASSAPSSPKCS1_V2_1_SHA384:
            allowed = (psa_algo == PSA_ALG_RSA_PSS(PSA_ALG_SHA_384) ||
                      psa_algo == PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_384));
            break;

        case ALGO_RSASSAPSSPKCS1_V2_1_SHA512:
            allowed = (psa_algo == PSA_ALG_RSA_PSS(PSA_ALG_SHA_512) ||
                      psa_algo == PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_512));
            break;
#endif //USS_PSA_AVAILABLE_ALG_RSA_PSS

#ifdef USS_PSA_AVAILABLE_ALG_ECDSA
        case ALGO_ECDSA_SHA_256:
            allowed = (psa_algo == PSA_ALG_ECDSA(PSA_ALG_SHA_256));
            break;
        
        case ALGO_ECDSA_SHA_384:
            allowed = (psa_algo == PSA_ALG_ECDSA(PSA_ALG_SHA_384));
            break;
        
        case ALGO_ECDSA_SHA_512:
            allowed = (psa_algo == PSA_ALG_ECDSA(PSA_ALG_SHA_512));
            break;
        
        case ALGO_ECDSA_SHA_any:
            allowed = PSA_ALG_IS_RANDOMIZED_ECDSA(psa_algo) && (psa_algo != PSA_ALG_ECDSA_ANY);    //Deterministic ECDSA not supported
            break;

        case ALGO_ECDSA_PLAIN:
            allowed = (psa_algo == PSA_ALG_ECDSA_ANY);  //This algorithm cannot be used with the wildcard key policy PSA_ALG_ECDSA(PSA_ALG_ANY_HASH).
            break;                                      //It is only permitted when PSA_ALG_ECDSA_ANY is the key’s permitted-algorithm policy.
#endif //USS_PSA_AVAILABLE_ALG_ECDSA

#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
        /** We're not able to put any restrictions on the psa_algo based on key_policy 
         * since there are no unique algo ID corresponding to different tag length in the
         * underlying implemenation. As a result, currently, if a key is generated with 
         * an AEAD algo for specific shortened_tag_length_algo or at_least_this_length_algo, 
         * we would still allow the operation to go through even though tag length(and hence the psa_algo) 
         * does'nt satisfy the key_policy, provided the base algorithm is same. 
         */
        case ALGO_CIPHER_AES_CCM:
            allowed = check_aead_key_policy(ALGO_CIPHER_AES_CCM, psa_algo);
            break;
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM

#ifdef USS_PSA_AVAILABLE_ALG_AES_GCM
        case ALGO_CIPHER_AES_GCM:
            allowed = check_aead_key_policy(ALGO_CIPHER_AES_GCM, psa_algo);
            break;
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM

/**
 * For MAC, currently, if user tries to set key policy for specific mac length or at_least_this_length_algo, 
 * we're defaulting it to full length mac (due to limitations of unability of storing length as part of key meta-data). 
 * As a result, we would still allow the operation to go through even though length(and hence the psa_algo) does'nt 
 * satisfy the key_policy, provided the base algorithm is same. 
 */
        case ALGO_MAC_AES_CBC_MAC:
            allowed = (PSA_ALG_FULL_LENGTH_MAC(psa_algo) == PSA_ALG_CBC_MAC);
            break;

        case ALGO_MAC_AES_CMAC:
            allowed = (PSA_ALG_FULL_LENGTH_MAC(psa_algo) == PSA_ALG_CMAC);
            break;

#ifdef USS_PSA_AVAILABLE_ALG_SHA256
        case ALGO_HMAC_SHA256:
            allowed = (PSA_ALG_FULL_LENGTH_MAC(psa_algo) == PSA_ALG_HMAC(PSA_ALG_SHA_256));
            break;
#endif //USS_PSA_AVAILABLE_ALG_SHA256

#ifdef USS_PSA_AVAILABLE_ALG_SHA384
        case ALGO_HMAC_SHA384:
            allowed = (PSA_ALG_FULL_LENGTH_MAC(psa_algo) == PSA_ALG_HMAC(PSA_ALG_SHA_384));
            break;
#endif //USS_PSA_AVAILABLE_ALG_SHA384

#ifdef USS_PSA_AVAILABLE_ALG_SHA512
        case ALGO_HMAC_SHA512:
            allowed = (PSA_ALG_FULL_LENGTH_MAC(psa_algo) == PSA_ALG_HMAC(PSA_ALG_SHA_512));
            break;
#endif //USS_PSA_AVAILABLE_ALG_SHA512

        default:
            break;
    }

    return allowed;
}

bool check_aead_tag_len(psa_algorithm_t psa_algo){
    uint8_t taglen = USS_PSA_ALG_AEAD_GET_TAG_LENGTH(psa_algo);
    psa_algorithm_t base_algo = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(psa_algo);
    switch(base_algo){
#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
        case PSA_ALG_CCM:
            return((taglen >= 4) && (taglen <= 16) && (taglen % 2 == 0));
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM
#ifdef USS_PSA_AVAILABLE_ALG_AES_GCM
        case PSA_ALG_GCM:
            return((taglen == 4) || (taglen == 8) || (taglen == 12) || (taglen == 13) || (taglen == 14) || (taglen == 15) || (taglen == 16));
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM
        default:
            break;
    }
    return false;
}

#if defined(USS_PSA_AVAILABLE_ALG_AES_CCM) || defined(USS_PSA_AVAILABLE_ALG_AES_GCM)
static bool check_aead_key_policy(mxq_algo_id_t key_algo, psa_algorithm_t psa_algo){
    //psa_algo should not have Wildcard value as this is only allowed while 
    //setting the key algo policy

#ifdef USS_PSA_AVAILABLE_ALG_AES_CCM
    if (key_algo == ALGO_CIPHER_AES_CCM){
        switch (psa_algo)
        {
        case PSA_ALG_CCM:
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, USS_PSA_AEAD_TAG_LEN_4):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, USS_PSA_AEAD_TAG_LEN_6):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, USS_PSA_AEAD_TAG_LEN_8):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, USS_PSA_AEAD_TAG_LEN_10):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, USS_PSA_AEAD_TAG_LEN_12):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, USS_PSA_AEAD_TAG_LEN_14):
       // case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, USS_PSA_AEAD_TAG_LEN_16): Same as PSA_ALG_CCM
            return true;
        default:
            break;
        }
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_CCM
#ifdef USS_PSA_AVAILABLE_ALG_AES_GCM
    if(key_algo == ALGO_CIPHER_AES_GCM){
        switch (psa_algo)
        {
        case PSA_ALG_GCM:
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, USS_PSA_AEAD_TAG_LEN_4):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, USS_PSA_AEAD_TAG_LEN_8):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, USS_PSA_AEAD_TAG_LEN_12):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, USS_PSA_AEAD_TAG_LEN_13):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, USS_PSA_AEAD_TAG_LEN_14):
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, USS_PSA_AEAD_TAG_LEN_15):
      //  case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, USS_PSA_AEAD_TAG_LEN_16): Same as PSA_ALG_GCM
            return true;
        
        default:
            break;
        }
    }
#endif //USS_PSA_AVAILABLE_ALG_AES_GCM
    return false;
}
#endif

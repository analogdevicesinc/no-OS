/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include "psa/crypto.h"
#include "MXQ_API.h"
#include "psa/examples.h"
#include <stdlib.h>

#define MAXQ1065_OLD_VERS_LISTOBJ_RESP_SIZE (21)
#define OBJ_HEADER_KEY_OBJ_SIZE             (10)

psa_key_attributes_t psa_key_attributes_init(void)
{   
    psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;
    return key_attr;
}

psa_status_t psa_get_key_attributes(psa_key_id_t key, psa_key_attributes_t * attributes)
{
    // check if psa crypto init is done
    if(!ifCryptoInitDone())
    {
        return PSA_ERROR_BAD_STATE;
    }

    mxq_err_t mxq_rc;
    mxq_objecttype_t mxq_object_type;
    mxq_keytype_id_t mxq_key_type;
    mxq_algo_id_t mxq_algo_id;
    mxq_keyuse_t mxq_key_use;
    mxq_keyparam_id_t mxq_key_params;
    mxq_u2 key_size;
    /* using uint8_t because we don't want lifetime enum value*/
    uint8_t mxq_key_lifetime; 
    /* setting the offset to 2 because we already have the key id which is stored in the first two bytes */
    mxq_length offset = 2;
    mxq_u1 mxq_object[290] = {0};
    mxq_length mxq_object_length = sizeof(mxq_object);

    mxq_rc = MXQ_ListObject(0, key, mxq_object, &mxq_object_length);

    if(MXQ_OK == mxq_rc)
    {
        mxq_object_type =  get_u1(mxq_object, &offset);
        //ignoring the initialization state of the object
        offset+=1;
        // get lifetime of object
        mxq_key_lifetime = get_u1(mxq_object, &offset);

        if(mxq_key_lifetime & OBJPROP_TEMPORARY)
        {
            attributes->key_lifetime = PSA_KEY_LIFETIME_VOLATILE;
        }
        else if(mxq_key_lifetime & OBJPROP_PERSISTENT)
        {
            attributes->key_lifetime = PSA_KEY_LIFETIME_PERSISTENT;
        }
        else if(mxq_key_lifetime & OBJPROP_WIPE_ON_TAMPER)
        {
            attributes->key_lifetime = USS_PSA_KEY_LIFETIME_WIPE_ON_TAMPER;
        }
        
        /* setting the offset to 15 because atleast as of now we don't need to check access permissions for a MXQ_OBJECT*/
        offset = 15;
        mxq_u1 mxq_object_size = get_u1(mxq_object, &offset);
        mxq_key_type = get_u1(mxq_object, &offset);

        // older fw versions do not return key_params and key_len parameters
        if(mxq_object_length == MAXQ1065_OLD_VERS_LISTOBJ_RESP_SIZE)
        {
            if(MXQ_KEYTYPE_ECC == mxq_key_type)
            {
                mxq_key_params = MXQ_KEYPARAM_EC_P256R1;
                key_size = KEYCOMPLEN256;
            }
            else
            {
                mxq_key_params = MXQ_UNKNOWN_CURVE;
                key_size = mxq_object_size - OBJ_HEADER_KEY_OBJ_SIZE;
            }
        }
        else
        {
            mxq_key_params = get_u1(mxq_object, &offset);
            key_size = get_u2(mxq_object, &offset);
        }

        mxq_key_use = get_u1(mxq_object, &offset);
        mxq_algo_id = get_u1(mxq_object, &offset);

        /* fill up attribute structure */
        attributes->key_type = uss_maxq_to_psa_key_type(mxq_key_type, mxq_key_params, mxq_object_type);
        attributes->key_use = uss_maxq_to_psa_keyuse(mxq_key_use);
        attributes->key_algo = uss_maxq_to_psa_algo_id(mxq_algo_id);
        attributes->key_bits = key_size * 8;
        attributes->key_id = key;

        return PSA_SUCCESS;
    }
    else
    {
        return PSA_ERROR_GENERIC_ERROR;
    }
}


void psa_reset_key_attributes(psa_key_attributes_t * attributes)
{
    *attributes = PSA_KEY_ATTRIBUTES_INIT;
}

/* PSA specification defines the  PSA_KEY_ID_USER_MIN as 0x00000001 and 
    PSA_KEY_ID_USER_MAX as 0x3fffffff but, the MAXQ object id type is unsigned short, 
    limiting the value of the key ID that can be created. */

psa_key_id_t generate_key_id()
{
    psa_key_id_t key_id = 0;
    mxq_u1 key_id_str[sizeof(psa_key_id_t)] = {0};
    do
    {
        MXQ_Get_Random(key_id_str, sizeof(psa_key_id_t));
        key_id = ((psa_key_id_t) key_id_str[0] << 8) + (psa_key_id_t) key_id_str[1];
    } while((!(key_id > 0x000f)) || ((0x1000 < key_id) && (key_id < 0x1008)) || (!(key_id < 0xffff)));

    return key_id;
}

size_t calculate_key_bits(const psa_key_attributes_t * attributes, size_t data_size)
{
    size_t key_bits = 0;
    if(PSA_KEY_TYPE_IS_UNSTRUCTURED(attributes->key_type))
    {
        switch(attributes->key_type)
        {
            case PSA_KEY_TYPE_RAW_DATA:
            case PSA_KEY_TYPE_DERIVE:
            case PSA_KEY_TYPE_PASSWORD:
            case PSA_KEY_TYPE_PASSWORD_HASH:
            case PSA_KEY_TYPE_ARIA:
            case PSA_KEY_TYPE_CAMELLIA:
            case PSA_KEY_TYPE_DES:
            case PSA_KEY_TYPE_CHACHA20:
                break;
            case PSA_KEY_TYPE_HMAC:
                key_bits = data_size * 8;
                break;
            case PSA_KEY_TYPE_AES:
            // This if loop returns error if AES algo is unsupported. 
            // Hence, instead of multiplying by 8, we assign bit values individually
                if(16 == data_size)
                {
                    key_bits = 128;
                }
                else if (24 == data_size)
                {
                    key_bits = 192;
                }
                else if(32 == data_size)
                {
                    key_bits = 256;
                } 
                break;
            default:
                return PSA_ERROR_NOT_SUPPORTED;
        }
    }
    else if (PSA_KEY_TYPE_IS_ASYMMETRIC(attributes->key_type))
    {
        if (PSA_KEY_TYPE_IS_ECC(attributes->key_type))
        {
            if(PSA_KEY_TYPE_IS_PUBLIC_KEY(attributes->key_type))
            {
                if((data_size & 1) == 0)
                {
                    return PSA_ERROR_INVALID_ARGUMENT;
                }
                key_bits = (data_size/2) * 8;
            }
            else
            {
                key_bits = data_size * 8;
            }
        }
        if (PSA_KEY_TYPE_IS_RSA(attributes->key_type))
        {
            if(PSA_KEY_TYPE_IS_KEY_PAIR(attributes->key_type))
            {
                key_bits = (data_size/2) * 8;
            }
            else
            {
                key_bits = data_size * 8;
            }
        }   
    }
    return key_bits;
}

mxq_keyparam_id_t get_ecc_curve_bits(const psa_key_attributes_t *attributes, size_t key_bits)
{
    mxq_keyparam_id_t ecc_curve_bits = MXQ_UNKNOWN_CURVE; 
    if(PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->key_type) == PSA_ECC_FAMILY_BRAINPOOL_P_R1)
    {
        switch(key_bits)
        {
            case 256:
                ecc_curve_bits = MXQ_KEYPARAM_EC_BP256R1;
                break;
            case 384: 
                ecc_curve_bits = MXQ_KEYPARAM_EC_BP384R1;
                break;
            case 512: 
                ecc_curve_bits = MXQ_KEYPARAM_EC_BP512R1;
                break;
            default: 
                ecc_curve_bits = MXQ_UNKNOWN_CURVE;
                break;
        }
    }
    else if (PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->key_type) == PSA_ECC_FAMILY_SECP_R1)
    {
        switch(key_bits)
        {
            case 256:
                ecc_curve_bits = MXQ_KEYPARAM_EC_P256R1;
                break;
            case 384: 
                ecc_curve_bits = MXQ_KEYPARAM_EC_P384R1;
                break;
            case 521: 
                ecc_curve_bits = MXQ_KEYPARAM_EC_P521R1;
                break;
            case 528: 
                ecc_curve_bits = MXQ_KEYPARAM_EC_P521R1;
                break;
            default: 
                ecc_curve_bits = MXQ_UNKNOWN_CURVE;
                break;
        }            
    }
    return ecc_curve_bits;
}

mxq_err_t key_mgmt_setup(const psa_key_attributes_t * attributes, psa_key_id_t *key, mxq_keyparam_id_t* params, size_t data_length, size_t * key_bits)
{
    mxq_err_t err = MXQ_OK;

    // check if psa crypto init is done
    if(!ifCryptoInitDone())
    {
        return PSA_ERROR_BAD_STATE;
    }

    err = Administrator_Authentication();

    /* PSA_KEY_ID_VENDOR_MIN – PSA_KEY_ID_VENDOR_MAX
        Implementations can define additional persistent key identifiers in this range, 
        and must allocate any volatile key identifiers from this range.
    */
    if(PSA_KEY_LIFETIME_VOLATILE == attributes->key_lifetime)
    {
        *key = generate_key_id();
        *key |= PSA_KEY_ID_VENDOR_MIN;
    }
    else
    {
        *key = attributes->key_id;
    }

    // Calculate and set key bits if not explicitly set
    if(!(attributes->key_bits))
    {
        *key_bits = calculate_key_bits(attributes, data_length);
    }
    else
    {
        *key_bits = attributes->key_bits;
    }

    size_t object_len = *key_bits;

    *params = MXQ_UNKNOWN_CURVE;

    if (PSA_KEY_TYPE_IS_ECC(attributes->key_type))
    {
        *params = get_ecc_curve_bits(attributes, *key_bits);
    }

#ifdef ENABLE_RSA
    if (PSA_KEY_TYPE_IS_RSA(attributes->key_type))
    {
        /* For RSA keys (PSA_KEY_TYPE_RSA_KEY_PAIR), the public exponent is 65537.
         * The modulus is a product of two probabilistic primes between 2^{n-1} and 2^n where n is the bit size specified in the attributes.
         */
        *params = MXQ_KEYPARAM_RSA_PUB65537;

        if (PSA_KEY_TYPE_IS_KEY_PAIR(attributes->key_type))
        {
            object_len = ((*key_bits) * 7 ) >> 1; // make space for private and public key, this is recommended length
        }
        else if (PSA_KEY_TYPE_IS_PUBLIC_KEY(attributes->key_type))
        {
            object_len = (*key_bits); // this is recommended length
        }

    }
    else
#endif
    {
        if (PSA_KEY_TYPE_IS_KEY_PAIR(attributes->key_type))
        {
            object_len = (*key_bits) * 3; // make space for private and public key, this is recommended length
        }
        else if (PSA_KEY_TYPE_IS_PUBLIC_KEY(attributes->key_type))
        {
            object_len = (*key_bits) * 2; // this is recommended length
        }
    }

    if (MXQ_OK == err)
    {
        err = MXQ_CreateObject(*key, 
                                object_len >> 3, //this is object size in bytes
                                uss_psa_to_maxq_object_type(attributes->key_type), 
                                uss_psa_to_maxq_key_lifetime(attributes->key_lifetime), 
                                (char*)"x=rwdgx:x=rwdgx:x=rwdgx");
    }

    return err;
}

psa_status_t psa_generate_key(const psa_key_attributes_t * attributes, psa_key_id_t * key)
{
    mxq_err_t err;
    mxq_u1 signature[64];
    static mxq_u1 request_data[2048];
    mxq_length request_data_len = sizeof(request_data);
    *key = PSA_KEY_ID_NULL;
    mxq_keyparam_id_t params;
    size_t key_bits;

    err = key_mgmt_setup(attributes, key, &params, 0 /* data length not provided to psa_generate_key*/, &key_bits);
    
    /* we do not pass key_bits to generate function*/
    (void) key_bits;

    if (MXQ_OK == err)
    {
        err = MXQ_BuildKey(request_data, 
                           &request_data_len, 
                           uss_psa_to_maxq_key_type(attributes->key_type), 
                           params, 
                           ((attributes->key_bits) >> 3), // this is key size in bytes
                           0 /* to generate a key, the total_keylen parameter needs to be 0 */,
                           uss_psa_to_maxq_keyuse(attributes->key_use), 
                           uss_psa_to_maxq_algo_id(attributes->key_algo),
                           /* multiple key usages cannot be used according to PSA*/
                           MXQ_KEYUSE_NONE, 
                           ALGO_NONE,
                           NULL); 
    }
    if (MXQ_OK == err)
    {
        err = ECDSA_sign_secp256r1_sha256(signature, KeyPairImport, request_data, request_data_len);
    }
    if (MXQ_OK == err)
    {
        err = MXQ_ImportKey(*key, ALGO_ECDSA_SHA_256, PUBKEYIMPORTOBJID, request_data, request_data_len, signature, KEYCOMPLEN * 2);
    }

    if(MXQ_OK == err)
    {
        return PSA_SUCCESS;
    }

    *key = PSA_KEY_ID_NULL;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_import_key(const psa_key_attributes_t * attributes,
                            const uint8_t * data,
                            size_t data_length,
                            psa_key_id_t * key)
{
    mxq_err_t err;
    mxq_u1 signature[64];
    static mxq_u1 request_data[2048];
    mxq_length request_data_len = sizeof(request_data);
    *key = PSA_KEY_ID_NULL;
    size_t key_bits;

    size_t total_key_len = data_length;
    mxq_keyparam_id_t params;
    

    err = key_mgmt_setup(attributes, key, &params, data_length, &key_bits);

    // calculate total key length from key_bits
#ifdef ENABLE_RSA
    if (PSA_KEY_TYPE_IS_RSA(attributes->key_type))
    {

        if (PSA_KEY_TYPE_IS_KEY_PAIR(attributes->key_type))
        {
            total_key_len = ((key_bits) * 2 ) >> 3;
        }
        else if (PSA_KEY_TYPE_IS_PUBLIC_KEY(attributes->key_type))
        {
            total_key_len = ((key_bits)  >> 3);
        }

    }
    else
#endif
    {
		if (PSA_KEY_TYPE_IS_KEY_PAIR(attributes->key_type))
		{
			total_key_len = ((key_bits) * 3) >> 3;
		}
		else if (PSA_KEY_TYPE_IS_PUBLIC_KEY(attributes->key_type))
		{
			total_key_len = ((key_bits) * 2) >> 3;
		}
    }

    if (MXQ_OK == err)
    {
        err = MXQ_BuildKey(request_data,
                           &request_data_len,
                           uss_psa_to_maxq_key_type(attributes->key_type),
                           params,
                           ((key_bits) >> 3),
                           total_key_len,
                           uss_psa_to_maxq_keyuse(attributes->key_use), 
                           uss_psa_to_maxq_algo_id(attributes->key_algo),
                           /* multiple key usages cannot be used according to PSA*/
                           MXQ_KEYUSE_NONE, 
                           ALGO_NONE,
                           (uint8_t *)data);
    }
    if (MXQ_OK == err)
    {
        err = ECDSA_sign_secp256r1_sha256(signature, KeyPairImport, request_data, request_data_len);
    }
    if (MXQ_OK == err)
    {
        err = MXQ_ImportKey(*key, ALGO_ECDSA_SHA_256, PUBKEYIMPORTOBJID, request_data, request_data_len, signature, KEYCOMPLEN * 2);
    }

    if(MXQ_OK == err)
    {
        return PSA_SUCCESS;
    }

    *key = PSA_KEY_ID_NULL;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_destroy_key(psa_key_id_t key)
{
    // check if psa crypto init is done
    if(!ifCryptoInitDone())
    {
        return PSA_ERROR_BAD_STATE;
    }

    mxq_err_t err = Administrator_Authentication();
    if (MXQ_OK == err)
    {
        err = MXQ_DeleteObject(key);
        if(MXQ_OK == err)
        {
            return PSA_SUCCESS;
        }
    }
    else 
    {
        return PSA_ERROR_NOT_PERMITTED;
    }
    
    return PSA_ERROR_GENERIC_ERROR;
}

psa_status_t psa_export_public_key(psa_key_id_t key,
                                   uint8_t * data,
                                   size_t data_size,
                                   size_t * data_length)
{
    // check if psa crypto init is done
    if(!ifCryptoInitDone())
    {
        return PSA_ERROR_BAD_STATE;
    }

    mxq_err_t err = MXQ_OK;
    mxq_length key_len = data_size;
    err = MXQ_ReadObject(key, 0, data, &key_len);
    if(MXQ_OK == err)
    {
        *data_length = key_len;
        return PSA_SUCCESS;
    }
    else
    {
        return PSA_ERROR_INVALID_HANDLE;
    }

    return PSA_ERROR_GENERIC_ERROR;
}

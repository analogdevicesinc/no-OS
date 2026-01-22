/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef CRYPTO_CONFIG_DEF_H
#define CRYPTO_CONFIG_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#if !defined(MAXQ1065) && !defined(MAXQ1080)
    #error "Missing valid authenticator!"
#elif defined(MAXQ1065) && defined(MAXQ1080)
    #error "Multiple authenticators defined!"
#endif

#if defined(TFM_WITH_USS_DRIVER_SUPPORT)
    #include "psa_name_mangling.h"
#endif //TFM_WITH_USS_DRIVER_SUPPORT

#if defined(USE_PROTOCOL_MAILBOX)
    #define USS_PSA_EMULATION
#endif

//Algo configuration for Ciphering
#if defined(MAXQ1065) || defined(MAXQ1080)
    #define USS_PSA_AVAILABLE_ALG_AES_CCM
    #define USS_PSA_AVAILABLE_ALG_AES_GCM
    #define USS_PSA_AVAILABLE_ALG_AES_ECB
    #define USS_PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE 16u    
    #ifdef USS_PSA_AVAILABLE_ALG_CHACHA20_POLY1305
        #undef USS_PSA_AVAILABLE_ALG_CHACHA20_POLY1305
    #endif //USS_PSA_AVAILABLE_ALG_CHACHA20_POLY1305
#endif
#if defined(MAXQ1080)
    #define USS_PSA_AVAILABLE_ALG_AES_CTR
#endif

//Algo configuration for Hashing
#if defined(MAXQ1065) || defined(MAXQ1080)
    #define USS_PSA_AVAILABLE_ALG_SHA256
#endif 
#if defined(MAXQ1080)
    #define USS_PSA_AVAILABLE_ALG_SHA384
    #define USS_PSA_AVAILABLE_ALG_SHA512
#endif

//Signing Algo
#if defined(MAXQ1065) || defined(MAXQ1080)
    #define USS_PSA_AVAILABLE_ALG_ECDSA
#endif
#if defined(MAXQ1080)
    #define USS_PSA_AVAILABLE_ALG_RSA
#endif

//Elliptic curve configuration
#if defined(MAXQ1065) || defined(MAXQ1080)
    #define USS_PSA_ECC_SECP_R1_256
#endif
#if defined(MAXQ1080)
    #define USS_PSA_ECC_SECP_R1_384
    #define USS_PSA_ECC_SECP_R1_521
    #define USS_PSA_ECC_BRAINPOOL_P_R1_256
    #define USS_PSA_ECC_BRAINPOOL_P_R1_384
    #define USS_PSA_ECC_BRAINPOOL_P_R1_512
#endif

//RSA configuration
#if defined(USS_PSA_AVAILABLE_ALG_RSA)
    #define USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15
    #define USS_PSA_AVAILABLE_ALG_RSA_PSS    
    #define USS_PSA_AVAILABLE_ALG_RSA4096
    #define USS_PSA_AVAILABLE_ALG_RSA3072
    #define USS_PSA_AVAILABLE_ALG_RSA2048
#endif //MAXQ1080


//Algo configuration for ECDSA Signing
#if defined(USS_PSA_AVAILABLE_ALG_ECDSA) && defined(USS_PSA_AVAILABLE_ALG_SHA256) 
    #define USS_PSA_AVAILABLE_ALG_ECDSA_SHA256
#endif
#if defined(USS_PSA_AVAILABLE_ALG_ECDSA) && defined(USS_PSA_AVAILABLE_ALG_SHA384)
    #define USS_PSA_AVAILABLE_ALG_ECDSA_SHA384
#endif
#if defined(USS_PSA_AVAILABLE_ALG_ECDSA) && defined(USS_PSA_AVAILABLE_ALG_SHA512)
    #define USS_PSA_AVAILABLE_ALG_ECDSA_SHA512
#endif

//Algo configuration for RSA signing
#if defined(USS_PSA_AVAILABLE_ALG_RSA) && defined(USS_PSA_AVAILABLE_ALG_SHA256) 
    #define USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA256
    #define USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA256
#endif
#if defined(USS_PSA_AVAILABLE_ALG_RSA) && defined(USS_PSA_AVAILABLE_ALG_SHA384)
    #define USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA384
    #define USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA384
#endif
#if defined(USS_PSA_AVAILABLE_ALG_RSA) && defined(USS_PSA_AVAILABLE_ALG_SHA512)
    #define USS_PSA_AVAILABLE_ALG_RSA_PKCS1V15_SHA512
    #define USS_PSA_AVAILABLE_ALG_RSA_PSS_SHA512
#endif

/* The maximum size of hash, in bits*/
#ifdef USS_PSA_AVAILABLE_ALG_SHA512
    #define USS_PSA_MAX_HASH_BITS 512u
#elif defined(USS_PSA_AVAILABLE_ALG_SHA384)
    #define USS_PSA_MAX_HASH_BITS 384u
#elif defined(USS_PSA_AVAILABLE_ALG_SHA256)
    #define USS_PSA_MAX_HASH_BITS 256u
#endif

/* The maximum size of an ECC key on this implementation, in bits.*/
#if defined(USS_PSA_ECC_SECP_R1_521)
    #define USS_PSA_ECC_MAX_CURVE_BITS 521u
#elif defined(USS_PSA_ECC_BRAINPOOL_P_R1_512)
    #define USS_PSA_ECC_MAX_CURVE_BITS 512u
#elif defined(USS_PSA_ECC_SECP_R1_384) || defined(USS_PSA_ECC_BRAINPOOL_P_R1_384)
    #define USS_PSA_ECC_MAX_CURVE_BITS 384u
#elif defined(USS_PSA_ECC_SECP_R1_256) || defined(USS_PSA_ECC_BRAINPOOL_P_R1_256)
    #define USS_PSA_ECC_MAX_CURVE_BITS 256u
#else
    #define USS_PSA_ECC_MAX_CURVE_BITS 0u
#endif

//RSA configuration
#if defined(USS_PSA_AVAILABLE_ALG_RSA4096)
    #define USS_PSA_RSA_MAX_KEY_BITS 4096u
#elif defined(USS_PSA_AVAILABLE_ALG_RSA3072)
    #define USS_PSA_RSA_MAX_KEY_BITS 3072u
#elif defined(USS_PSA_AVAILABLE_ALG_RSA2048)
    #define USS_PSA_RSA_MAX_KEY_BITS 2048u
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif //CRYPTO_CONFIG_DEF_H



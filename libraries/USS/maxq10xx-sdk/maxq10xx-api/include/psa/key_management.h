/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#ifndef _KEY_MANANGEMENT_H_
#define _KEY_MANANGEMENT_H_

#include "psa/crypto_config_def.h"
#include "psa/crypto_typedefs.h"

/* Initialize key attributes object according to specification */
#define PSA_KEY_ATTRIBUTES_INIT (psa_key_attributes_t) {PSA_KEY_TYPE_NONE, PSA_KEY_LIFETIME_VOLATILE, 0, PSA_ALG_NONE, 0, 0}

#define PSA_KEY_TYPE_NONE ((psa_key_type_t)0x0000)
#define PSA_KEY_TYPE_RAW_DATA ((psa_key_type_t)0x1001)
#define PSA_KEY_TYPE_HMAC ((psa_key_type_t)0x1100)
#define PSA_KEY_TYPE_DERIVE ((psa_key_type_t)0x1200)
#define PSA_KEY_TYPE_PASSWORD ((psa_key_type_t)0x1203)
#define PSA_KEY_TYPE_PASSWORD_HASH ((psa_key_type_t)0x1205)
#define PSA_KEY_TYPE_PEPPER ((psa_key_type_t)0x1206)

#define PSA_KEY_TYPE_AES ((psa_key_type_t)0x2400)
#define PSA_KEY_TYPE_ARIA ((psa_key_type_t)0x2406)
#define PSA_KEY_TYPE_DES ((psa_key_type_t)0x2301)
#define PSA_KEY_TYPE_CAMELLIA ((psa_key_type_t)0x2403)
#define PSA_KEY_TYPE_SM4 ((psa_key_type_t)0x2405)
#define PSA_KEY_TYPE_ARC4 ((psa_key_type_t)0x2002)
#define PSA_KEY_TYPE_CHACHA20 ((psa_key_type_t)0x2004)
#define PSA_KEY_TYPE_RSA_KEY_PAIR ((psa_key_type_t)0x7001)
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY ((psa_key_type_t)0x4001)
#define PSA_ECC_FAMILY_SECP_K1 ((psa_ecc_family_t) 0x17)
#define PSA_ECC_FAMILY_SECP_R1 ((psa_ecc_family_t) 0x12)
#define PSA_ECC_FAMILY_SECP_R2 ((psa_ecc_family_t) 0x1b)
#define PSA_ECC_FAMILY_SECT_K1 ((psa_ecc_family_t) 0x27)
#define PSA_ECC_FAMILY_SECT_R1 ((psa_ecc_family_t) 0x22)
#define PSA_ECC_FAMILY_SECT_R2 ((psa_ecc_family_t) 0x2b)
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1 ((psa_ecc_family_t) 0x30)
#define PSA_ECC_FAMILY_FRP ((psa_ecc_family_t) 0x33)
#define PSA_ECC_FAMILY_MONTGOMERY ((psa_ecc_family_t) 0x41)
#define PSA_ECC_FAMILY_TWISTED_EDWARDS ((psa_ecc_family_t) 0x42)


#define PSA_KEY_TYPE_DH_GET_FAMILY(type) \
    ((psa_dh_family_t) ((type) & 0x00ff))

#define PSA_KEY_TYPE_DH_KEY_PAIR(group) \
    ((psa_key_type_t) (0x7200 | (group)))

#define PSA_KEY_TYPE_DH_PUBLIC_KEY(group) \
    ((psa_key_type_t) (0x4200 | (group)))

#define PSA_KEY_TYPE_ECC_GET_FAMILY(type) \
    ((psa_ecc_family_t) ((type) & 0x00ff))

#define PSA_KEY_TYPE_ECC_KEY_PAIR(curve) \
    ((psa_key_type_t) (0x7100 | (curve)))

#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve) \
    ((psa_key_type_t) (0x4100 | (curve)))

#define PSA_KEY_TYPE_IS_ASYMMETRIC(type) \
    (((type) & 0x4000) == 0x4000)

#define PSA_KEY_TYPE_IS_DH(type) \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) & 0xff00) == 0x4200)

#define PSA_KEY_TYPE_IS_DH_KEY_PAIR(type) \
    (((type) & 0xff00) == 0x7200)

#define PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type) \
    (((type) & 0xff00) == 0x4200)

#define PSA_KEY_TYPE_IS_ECC(type) \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) & 0xff00) == 0x4100)

#define PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type) \
    (((type) & 0xff00) == 0x7100)

#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type) \
    (((type) & 0xff00) == 0x4100)

#define PSA_KEY_TYPE_IS_KEY_PAIR(type) \
    (((type) & 0x7000) == 0x7000)

#define PSA_KEY_TYPE_IS_PUBLIC_KEY(type) \
    (((type) & 0x7000) == 0x4000)

#define PSA_KEY_TYPE_IS_RSA(type) \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == 0x4001)

#define PSA_KEY_TYPE_IS_UNSTRUCTURED(type) \
    (((type) & 0x7000) == 0x1000 || ((type) & 0x7000) == 0x2000)

#define PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY(type) \
    ((psa_key_type_t) ((type) | 0x3000))

#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) \
    ((psa_key_type_t) ((type) & ~0x3000))

#define PSA_KEY_LIFETIME_VOLATILE ((psa_key_lifetime_t) 0x00000000)

#define PSA_KEY_LIFETIME_PERSISTENT ((psa_key_lifetime_t) 0x00000001)

#define PSA_KEY_PERSISTENCE_VOLATILE ((psa_key_persistence_t) 0x00)

#define PSA_KEY_PERSISTENCE_DEFAULT ((psa_key_persistence_t) 0x01)
#define PSA_KEY_PERSISTENCE_READ_ONLY ((psa_key_persistence_t) 0xff)
#define PSA_KEY_LOCATION_LOCAL_STORAGE ((psa_key_location_t) 0x000000)
#define PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT ((psa_key_location_t) 0x000001)
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) \
    ((psa_key_persistence_t) ((lifetime) & 0x000000ff))
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime) \
    ((psa_key_location_t) ((lifetime) >> 8))
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime) \
    (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location) \
    ((location) << 8 | (persistence))

/* vendor specified lifetime */
#define USS_PSA_KEY_LIFETIME_WIPE_ON_TAMPER ((psa_key_persistence_t) 0x80)

#define PSA_KEY_ID_NULL ((psa_key_id_t)0)
#define PSA_KEY_ID_USER_MIN ((psa_key_id_t)0x00000001)
#define PSA_KEY_ID_USER_MAX ((psa_key_id_t)0x3fffffff)
#define PSA_KEY_ID_VENDOR_MIN ((psa_key_id_t)0x40000000)
#define PSA_KEY_ID_VENDOR_MAX ((psa_key_id_t)0x7fffffff)
#define PSA_KEY_USAGE_EXPORT ((psa_key_usage_t)0x00000001)
#define PSA_KEY_USAGE_COPY ((psa_key_usage_t)0x00000002)
#define PSA_KEY_USAGE_CACHE ((psa_key_usage_t)0x00000004)
#define PSA_KEY_USAGE_ENCRYPT ((psa_key_usage_t)0x00000100)
#define PSA_KEY_USAGE_DECRYPT ((psa_key_usage_t)0x00000200)
#define PSA_KEY_USAGE_SIGN_MESSAGE ((psa_key_usage_t)0x00000400)
#define PSA_KEY_USAGE_VERIFY_MESSAGE ((psa_key_usage_t)0x00000800)
#define PSA_KEY_USAGE_SIGN_HASH ((psa_key_usage_t)0x00001000)
#define PSA_KEY_USAGE_VERIFY_HASH ((psa_key_usage_t)0x00002000)
#define PSA_KEY_USAGE_DERIVE ((psa_key_usage_t)0x00004000)
#define PSA_KEY_USAGE_VERIFY_DERIVATION ((psa_key_usage_t)0x00008000)

#define PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */
#define PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits) \
    /* implementation-defined value */
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE /* implementation-defined value */
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE /* implementation-defined value */

/* 
 * The following simple accessor functions can be static inlined and need not 
 * validate inputs 
 */

/**
 * @brief Sets the key ID in the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and a key ID, and sets the key ID
 * in the given attributes.
 *
 * @param[in,out] attributes A pointer to the PSA key attributes.
 * @param[in]     id         The key ID to be set in the attributes.
 */
static inline void psa_set_key_id(psa_key_attributes_t * attributes,
                    psa_key_id_t id)
{
	attributes->key_id = id;
}

/**
 * @brief Retrieves the key ID from the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and returns the key ID
 * associated with those attributes.
 *
 * @param[in] attributes A pointer to the PSA key attributes.
 *
 * @return The key ID associated with the given attributes.
 */
static inline psa_key_id_t psa_get_key_id(const psa_key_attributes_t * attributes)
{
	return attributes->key_id;
}

/**
 * @brief Sets the key lifetime in the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and a key lifetime, and sets the key lifetime
 * in the given attributes.
 *
 * @param[in,out] attributes A pointer to the PSA key attributes.
 * @param[in]     lifetime   The key lifetime to be set in the attributes.
 */
static inline void psa_set_key_lifetime(psa_key_attributes_t * attributes,
                          psa_key_lifetime_t lifetime)
{
	attributes->key_lifetime = lifetime;
}

/**
 * @brief Retrieves the key lifetime from the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and returns the key lifetime
 * associated with those attributes.
 *
 * @param[in] attributes A pointer to the PSA key attributes.
 *
 * @return The key lifetime associated with the given attributes.
 */
static inline psa_key_lifetime_t psa_get_key_lifetime(const psa_key_attributes_t * attributes)
{
	return attributes->key_lifetime;
}

/**
 * @brief Sets the key type in the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and a key type, and sets the key type
 * in the given attributes.
 *
 * @param[in,out] attributes A pointer to the PSA key attributes.
 * @param[in]     type       The key type to be set in the attributes.
 */
static inline void psa_set_key_type(psa_key_attributes_t * attributes,
                      psa_key_type_t type)
{
	attributes->key_type = type;
}

/**
 * @brief Retrieves the key type from the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and returns the key type
 * associated with those attributes.
 *
 * @param[in] attributes A pointer to the PSA key attributes.
 *
 * @return The key type associated with the given attributes.
 */
static inline psa_key_type_t psa_get_key_type(const psa_key_attributes_t * attributes)
{
	return attributes->key_type; 
}

/**
 * @brief Sets the key bits in the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and a size value representing the key bits,
 * and sets the key bits in the given attributes.
 *
 * @param[in,out] attributes A pointer to the PSA key attributes.
 * @param[in]     bits       The size value representing the key bits to be set in the attributes.
 */
static inline void psa_set_key_bits(psa_key_attributes_t * attributes,
                      size_t bits)
{
	attributes->key_bits = bits;
}

/**
 * @brief Retrieves the key bits from the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and returns the size value
 * representing the key bits associated with those attributes.
 *
 * @param[in] attributes A pointer to the PSA key attributes.
 *
 * @return The size value representing the key bits associated with the given attributes.
 */
static inline size_t psa_get_key_bits(const psa_key_attributes_t * attributes)
{
	return attributes -> key_bits;
}

/**
 * @brief Sets the key usage flags in the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and key usage flags, and sets the key usage flags
 * in the given attributes.
 *
 * @param[in,out] attributes  A pointer to the PSA key attributes.
 * @param[in]     usage_flags The key usage flags to be set in the attributes.
 */
static inline void psa_set_key_usage_flags(psa_key_attributes_t * attributes,
                             psa_key_usage_t usage_flags)
{
	attributes->key_use = usage_flags;
}

/**
 * @brief Retrieves the key usage flags from the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and returns the key usage flags
 * associated with those attributes.
 *
 * @param[in] attributes A pointer to the PSA key attributes.
 *
 * @return The key usage flags associated with the given attributes.
 */
static inline psa_key_usage_t psa_get_key_usage_flags(const psa_key_attributes_t * attributes)
{
	return attributes -> key_use;
}

/**
 * @brief Sets the key algorithm in the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and a key algorithm, and sets the key algorithm
 * in the given attributes.
 *
 * @param[in,out] attributes A pointer to the PSA key attributes.
 * @param[in]     alg        The key algorithm to be set in the attributes.
 */
static inline void psa_set_key_algorithm(psa_key_attributes_t * attributes,
                           psa_algorithm_t alg)
{
	attributes->key_algo = alg;
}

/**
 * @brief Retrieves the key algorithm from the given PSA key attributes.
 *
 * This function takes a pointer to PSA key attributes and returns the key algorithm
 * associated with those attributes.
 *
 * @param[in] attributes A pointer to the PSA key attributes.
 *
 * @return The key algorithm associated with the given attributes.
 */
static inline psa_algorithm_t psa_get_key_algorithm(const psa_key_attributes_t * attributes)
{
	return attributes->key_algo;
}

/**
 * @brief Generate a random key ID.
 *
 * This function generates a random key ID.
 *
 * @return A new key ID.
 */
psa_key_id_t generate_key_id(void);

#endif //_KEY_MANANGEMENT_H_

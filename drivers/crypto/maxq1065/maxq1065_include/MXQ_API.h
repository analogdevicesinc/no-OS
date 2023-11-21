/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc., All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
******************************************************************************/

/**
 * @internal
 * @file
 * @date Dec 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Julien Vercauteren, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @ingroup API
 */


#ifndef __MXQ_API_H_INCLUDED__
#define __MXQ_API_H_INCLUDED__

#include "MXQ_Types.h"
#include "MXQ_Error.h"
#include "MXQ_Config.h"

#define __API__

#define __API_VERSION__ 	5_4_0

/* Sanity checks */

/* The below test is GCC specific. I fnot using GCC, make sure your compiler is little endian */
#if (!defined(__BYTE_ORDER__))||((__BYTE_ORDER__!=__ORDER_LITTLE_ENDIAN__))
#error Your target must be little endian.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Active communication protocol.
 * \internal
 */
typedef enum {
    MXQ_MODE_TLS,  /**< TLS */
    MXQ_MODE_GCU   /**< GCU ( AES SPI ) */
} mxq_mode_t;

/**
 * \internal
 */
typedef enum
{
	IN_ONE_SEND = 0,
	IS_FIRST_SEND,
	IS_NOT_FIRST_SEND
}send_flag_t;

/**
 * \internal
 */
typedef enum
{
	PSK_FIRST =  0,/**< PSK_FIRST */
	PSK_LAST,      /**< PSK_LAST */
	ECDHE_SECRET,  /**< ECDHE_SECRET */
	ECDHE_IMPORT,   /**< ECDHE_IMPORT */
    RSA_PMS         /**< RSA_PMS */
}key_exchange_case_t;

/**
 * \internal
 */
typedef enum
{
    GEN_KEY =  0,
    CREATE_SECRET,   //1
    EXTRACT_SECRET,  //2
    EXPAND_SECRET,   //3
    UPDATE_IV        //4

}tls13_key_exchange_case_t;

/**
 * \internal
 */
typedef enum
{
	PASS_NB_1					= 1,
	PASS_ENABLE_SECUREBOOT		= 1,
	PASS_KEY_IMPORT_GEN			= 1,
	PASS_KEY_IMPORT_ENC			= 4,
	PASS_OPEN_CHANNEL			= 1,
	PASS_OPEN_ADMIN				= 1,
	PASS_VERIFY_SIGN			= 1,
	PASS_NB_2					= 2,
	PASS_COMPUTE_SIGN			= 2,
	PASS_NB_3					= 3,
	PASS_CANCEL_SECUREBOOT		= 3,
	PASS_CLOSE_CHANNEL			= 3,
	PASS_CLOSE_ADMIN			= 3,
	PASS_CLOSE_TLS				= 4,
	PASS_SAVE_TLS				= 5,
	PASS_RESTORE_TLS			= 6
}pass_t;

/**
 * \internal
 */
typedef enum {
    MXQ_OP_NONE    = -1,  //!< MXQ_OPERATION_NONE
    MXQ_OP_ENCRYPT =  0,  //!< MXQ_ENCRYPT
    MXQ_OP_DECRYPT        //!< MXQ_DECRYPT
} mxq_operation_t;
/******************************************************/

/**
 * Input enumeration for the MXQ_SetState API
 * @ingroup Administration
 */
typedef enum _PACKED
{
  LC_DELIVERY     = 3,       	/**< Delivery state, with default key */
  LC_INITIALIZED  = 4,       	/**< Customer intermediate mode */
  LC_OPERATIONAL  = 5,       	/**< In the field */
  LC_TERMINATED   = 6         	/**< Terminated */
} mxq_state_t;


/*  Definitions for the MXQ_MAXQ106x_Info_t */
/**
* \internal
*/
#define MXQ_MAXQ106x_CHIP_NAME_LENGTH    (16)
/**
* \internal
*/
#define MXQ_MAXQ106x_FW_VERSION_LENGTH   (3)
/**
* \internal
*/
#define MXQ_MAXQ106x_USN_LENGTH          (24)

#define MAXQ1080_BOOTLOADER_PRODUCT_NAME    "MAXQ1080Btldr"
#define MAXQ1065_BOOTLOADER_PRODUCT_NAME    "MAXQ1065Btldr"
#define KEYCOMPLEN256 32
#define KEYCOMPLEN384 48
#define KEYCOMPLEN521 66

/**
 * MAXQ10xx configuration information.
 *
 * Configuration can be modified using MXQ_SetConfig() (administrator authentication needed),
 * and read using MXQ_GetStatus().
 * 
 * @ingroup Administration
 */
typedef struct _PACKED
{
	mxq_u2 i2cslaveaddr;       /**< I2C Slave address (default is 0x60) */
	mxq_u2 i2cchartimeout;     /**< Max. allowed time (in ms) between incoming I2C chars before resetting comm */
	mxq_u2 spichartimeout;     /**< Max. allowed time (in ms) between incoming SPI chars before resetting comm */
	mxq_u2 blocktimeout;       /**< Max. allowed time (in ms) before next incoming block from host before resetting comm (I2C/SPI) */
#if defined(MAXQ1065)
    mxq_u2 rfu1;		       /**< Not used */
    mxq_u2 en_factory_reset;   /**< If 0x55AA, Factory reset command enabled. Default=0xAA55 (disabled).(See User's guide). */
#else
    mxq_u2 ext_sensors;        /**< External Tamper Sensors for MAXQ1080*/
	mxq_u2 int_sensors;        /**< Internal Tamper Sensors for MAXQ1080*/
#endif

	mxq_u2 secpol_needhost;    /**< If 0x55AA, HOST security condition is required for all commands (See User's guide). */
                               /**< Default=0xAA55 (disabled). */
	mxq_u2 tamper_action;      /**< Tamper Action*/
	mxq_u2 sleep_restore_enable;/**< If 0x55AA, Sleep Restore Command is allowed. Default=0xAA55 (disabled). */
	mxq_u2 terminated_enable;  /**< If 0x55AA, MXQ_SetState(LC_TERMINATED) is allowed. Default=0xAA55 (disabled). */
	mxq_u2 wd_period_initial;  /**< WD initial period (first period aplicable after MAXQ10xx resets) (in ms), 0 to deactivate. */
	mxq_u2 wd_period_normal;   /**< WD period (in ms) after the first period has been validated by the WDI pin, 0 to deactivate. */
	mxq_u2 reset_pulse_width;  /**< RESET_OUT pulse duration (in ms) */
	mxq_u2 reset_pulse_pol;    /**< RESET_OUT pulse polarity, 1: positive, 0: negative */

	mxq_u2 reset_events;       /**< Defined events that trigger RESET_OUT PIN and the MAXQ10xx internal reset (See RST_EV_XXX constants). */
                                   /**< None by default */
	mxq_u2 tlsmastersecretallowoutput; /**< Allows outputing TLS master secret if 0x55AA, 0xAA55 prevents this. Default is 0x55AA (enabled). */
	mxq_u2 rfu4;		   /**< Not used */
	mxq_u2 rfu5;		   /**< Not used */

} mxq_config_t;

/** @addtogroup Administration
 *  @{
*/
#if defined(MAXQ1065)
#define RST_EV_HOST_SECURE_BOOT_FAILURE      0x0001 /**< RESET OUT pin pulse when the secure boot fails (signature error)*/
#endif
#define RST_EV_HOST_SECURE_CHANNEL_ERROR     0x0002 /**< RESET OUT pin pulse when the current secure channel fails (signature or decryption error)*/
#define RST_EV_HOST_EXTERNAL_TAMPER          0x0004 /**< RESET OUT pin pulse when the external TAMPER pin is triggered */
                                                    /**< (MAXQ10xx must not be in sleep mode or powered off, and must be in crypto toolbox mode).*/
#define RST_EV_HOST_POR                      0x0008 /**< RESET OUT pin pulse when the MAXQ10xx reboots */
#define RST_EV_HOST_WD_TIMEOUT               0x0010 /**< RESET OUT pin pulse when the MAXQ10xx watchdog counter expires */

#if defined(MAXQ1065)
#define RST_EV_MAXQ106x_SECURE_BOOT_FAILURE      0x0001 /**< Performs an internal reset when the secure boot fails (signature error) */
#endif
#define RST_EV_MAXQ106x_SECURE_CHANNEL_ERROR 0x0200 /**< Performs an internal reset when the current secure channel fails (signature or decryption error) */
#define RST_EV_MAXQ106x_EXTERNAL_TAMPER      0x0400 /**< Performs an internal reset when the external TAMPER pin is triggered */
#define RST_EV_MAXQ106x_WD_TIMEOUT           0x1000 /**< Performs an internal reset when the MAXQ10xx watchdog counter expires */


typedef enum {
    eSCInit             = 1,
    eSCAesCbcInit       = 2,
    eSCFinish           = 3,
    eSCAesGcmInit       = 4,
    eSCPreSharedKeyMode = 5,
    eSCEcdhe            = 0x10,
    eSCAuthEcdhe        = 0x13,
    eSCEcdheMode1065    = 0x15
} teSCProcessState;

/*! @} */

 /**
  * MAXQ10xx configuration information.
  *
  * @ingroup Administration
  */
 typedef struct _PACKED
 {
	 mxq_u2 rfu1; /**<  not used */
	 mxq_u2 rfu2; /**<  not used */
	 mxq_u2 rfu3; /**<  not used */

	 mxq_u2 allowfactoryreset; /**< Value is 0x55AA if factory reset is enabled (special part# only) */
  } mxq_sysconfig_t;


typedef struct _PACKED
{
	mxq_u1 product_name[MXQ_MAXQ106x_CHIP_NAME_LENGTH];    /**< MAXQ106x chip name */

	mxq_u1 v_fw_maj;			/**< Firmware version major part */
	mxq_u1 v_fw_min;			/**< Firmware version minor part */
	mxq_u1 v_fw_rev;			/**< Firmware version revision part */

	mxq_u1 v_hw_cod;			/**< Hardware version, e.g. 0xC3 for rev C3 chip */

	mxq_u1 serial_num[MXQ_MAXQ106x_USN_LENGTH]; /**< MAXQ106x unique serial number */

	mxq_u1 lifecycle_state;		  	/**< MAXQ106x lifecycle state (see ::mxq_state_t) */
    #if defined(MAXQ106x)
    struct {
        mxq_u1 H;
        mxq_u1 L;
    } freemem;                      /**< MAXQ106x file system free size in bytes */
    struct {
        mxq_u1 H;
        mxq_u1 L;
    } totalmem;                     /**< MAXQ106x file system total size in bytes */
    #else
    struct {
        mxq_u1 mem_byte3;
        mxq_u1 mem_byte2;
        mxq_u1 mem_byte1;
        mxq_u1 mem_byte0;
    } freemem;                      /**< MAXQ106x file system free size in bytes */
    struct {
        mxq_u1 mem_byte3;
        mxq_u1 mem_byte2;
        mxq_u1 mem_byte1;
        mxq_u1 mem_byte0;
    } totalmem;                     /**< MAXQ106x file system total size in bytes */
    #endif
	mxq_config_t    config;                 /**< MAXQ106x configuration see MXQ_SetConfig() */
	mxq_sysconfig_t sysconfig;              /**< MAXQ106x system configuration (read-only), (see ::mxq_sysconfig_t) */

} mxq_status_raw_t;

 /**
  * Output structure for the MXQ_GetStatus function.
  * @ingroup Administration
  */
typedef struct _PACKED
{
	mxq_u1 product_name[MXQ_MAXQ106x_CHIP_NAME_LENGTH];    /**< MAXQ10xx chip name */

	mxq_u1 v_fw_maj;			/**< Firmware version major part */
	mxq_u1 v_fw_min;			/**< Firmware version minor part */
	mxq_u1 v_fw_rev;			/**< Firmware version revision part */

	mxq_u1 v_hw_cod;			/**< Hardware version, e.g. 0xC3 for rev C3 chip */

	mxq_u1 serial_num[MXQ_MAXQ106x_USN_LENGTH]; /**< MAXQ10xx unique serial number */

	mxq_u1 lifecycle_state;		  	/**< MAXQ10xx lifecycle state (see ::mxq_state_t) */
    #if defined(MAXQ106x)
    mxq_u2 freemem;				/**< MAXQ10xx file system free size in bytes */
    mxq_u2 totalmem;			/**< MAXQ10xx file system total size in bytes */
    #else
    mxq_u4 freemem;				/**< MAXQ10xx file system free size in bytes */
    mxq_u4 totalmem;			/**< MAXQ10xx file system total size in bytes */
    #endif
 
	mxq_config_t    config;                 /**< MAXQ10xx configuration see MXQ_SetConfig() */
	mxq_sysconfig_t sysconfig;              /**< MAXQ10xx system configuration (read-only), (see ::mxq_sysconfig_t) */

} mxq_status_t;

/**
* \internal
*/
typedef struct
{
    mxq_length aead_tag_len; 
    mxq_u1 aead_tag[16];			/**< Tag (16 max)*/
    mxq_length aad_length; 	/**< Additional Data Length */
    mxq_u1* p_aad;			/**< Additional Data Buffer */
    mxq_length key_length;  /**< Key Length */
    mxq_u1* p_key;			/**< Key Buffer */
    mxq_length iv_length;	/**< Initialization Vector Length */
    mxq_u1* p_iv;			/**< Initialization Vector Buffer */
    mxq_length data_length;	/**< Data Length required for CCM and GCM*/
	mxq_u1* R;
	mxq_length Rlen;
    mxq_u1 ecies_cipher_algo;
    mxq_u1 ecies_keylen;
    mxq_u1 ecies_hmac_algo;

} ciph_params_t;

/**
 * Definition of cryptographic algorithms.
 * @ingroup Cryptotools
 */
typedef enum _PACKED
{
	ALGO_NONE,           /* \internal ALGO_NONE */

	/*  HASH */
	ALGO_MD_FIRST,      
	ALGO_MD_SHA256,      /**< SHA-256 (FIPS 180-4: http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf) */
	ALGO_MD_SHA384,      /**< SHA-384 (FIPS 180-4: http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf) */
	ALGO_MD_SHA512,      /**< SHA-512 (FIPS 180-4: http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf) */
	ALGO_MD_LAST,        

	/*  SIGNATURE */
	ALGO_SIGN_FIRST,     
	ALGO_ECDSA_SHA_any,  /**< For key algo field only: ECDSA with any SHA. Cannot be used in MXQ_Sign, MXQ_Adminauthenticate, MXQ_Verifyboot, ...)*/
	ALGO_ECDSA_PLAIN,    /**< ECDSA with already hashed value as input (FIPS 186-4 https://csrc.nist.gov/publications/detail/fips/186/4/final) */
	ALGO_ECDSA_INNER_MD, /**< ECDSA with already hashed value using COMPUTE MD command */
	ALGO_ECDSA_SHA_256,  /**< ECDSA with SHA-256 (FIPS 186-4 https://csrc.nist.gov/publications/detail/fips/186/4/final)*/
	ALGO_ECDSA_SHA_384,  /**< ECDSA with SHA-384 (FIPS 186-4 https://csrc.nist.gov/publications/detail/fips/186/4/final)*/
	ALGO_ECDSA_SHA_512,  /**< ECDSA with SHA-512 (FIPS 186-4 https://csrc.nist.gov/publications/detail/fips/186/4/final)*/
	ALGO_SIGN_LAST,


	ALGO_MAC_FIRST,      
	ALGO_MAC_NULL,       
	ALGO_HMAC_SHA_any,   /**< For key algo field only: HMAC with any SHA (cannot be used in MXQ_MAC_Init)*/
	ALGO_HMAC_SHA256,    /**< HMAC-SHA-256 (FIPS 198-1 https://csrc.nist.gov/publications/detail/fips/198/1/final)*/
	ALGO_HMAC_SHA384,    /**< HMAC-SHA-384 (FIPS 198-1 https://csrc.nist.gov/publications/detail/fips/198/1/final)*/
	ALGO_HMAC_SHA512,    /**< HMAC-SHA-512 (FIPS 198-1 https://csrc.nist.gov/publications/detail/fips/198/1/final)*/

	ALGO_MAC_AES_CMAC,   /**< AES-CMAC (NIST SP800-38B https://csrc.nist.gov/publications/detail/sp/800-38b/final)*/
	ALGO_MAC_AES_GMAC,   /**< */
	ALGO_MAC_AES_CBC_MAC,/**< AES-CBC-MAC (ISO/IEC 9797-1 MAC Algorithm 1)*/
	ALGO_MAC_LAST,       

	/* Ciphers */
	ALGO_CIPHER_FIRST,   
	ALGO_CIPHER_NULL,    
	ALGO_CIPHER_AES_any, /**< AES with any mode */
	ALGO_CIPHER_AES_ECB, /**< AES-ECB (NIST SP800-38A https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-38a.pdf)*/
	ALGO_CIPHER_AES_CBC, /**< AES-CBC (NIST SP800-38A https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-38a.pdf)*/
	ALGO_CIPHER_AES_CCM, /**< AES-CCM (NIST SP800-38C https://csrc.nist.gov/publications/detail/sp/800-38c/final) */
	ALGO_CIPHER_AES_GCM, /*  */

	ALGO_CIPHER_AES_GCU, /**< A key with this algorithm value is transferred from the secure storage to the AES-SPI key register when used */
	ALGO_CIPHER_ECIES_AES_HMAC, /**< ECIES using AES-CBC with HMAC */

	ALGO_CIPHER_LAST,    /* ALGO_CIPHER_LAST */

	ALGO_MD_SHA1=0x81,
	
    ALGO_RSASIGN_FIRST = 0XA0,
    ALGO_RSASSAPSSPKCS1_V2_1_PLAIN = 0xA1,  // PKCS#1 V2.1 RSASSA-PSS with already hashed value as input
    ALGO_RSASSAPSSPKCS1_V2_1_SHA256 = 0xA2,  // PKCS#1 V2.1 RSASSA-PSS with SHA-256
    ALGO_RSASSAPSSPKCS1_V2_1_SHA384 = 0xA3,  // PKCS#1 V2.1 RSASSA-PSS with SHA-384
    ALGO_RSASSAPSSPKCS1_V2_1_SHA512 = 0xA4,  // PKCS#1 V2.1 RSASSA-PSS with SHA-512
    ALGO_RSASSAPSSPKCS1_V2_1_INNERMD = 0xA5,  // PKCS#1 V2.1 RSASSA-PSS with already hashed value using COMPUTE MD command
    ALGO_RSASSAPSSPKCS1_V1_5_PLAIN = 0xA6,  // PKCS#1 V2.1 RSASSA-PKCS1V15 with already hashed value as input
    ALGO_RSASSAPSSPKCS1_V1_5_SHA256 = 0xA7,  // PKCS#1 V2.1 RSASSA-PKCS1V15 with SHA-256
    ALGO_RSASSAPSSPKCS1_V1_5_SHA384 = 0xA8,  // PKCS#1 V2.1 RSASSA-PKCS1V15 with SHA-384
    ALGO_RSASSAPSSPKCS1_V1_5_SHA512 = 0xA9,  // PKCS#1 V2.1 RSASSA-PKCS1V15 with SHA-512
    ALGO_RSASSAPSSPKCS1_V1_5_INNERMD = 0xAA,  // PKCS#1 V2.1 RSASSA-PKCS1V15 with already hashed value using COMPUTE MD command
    ALGO_RSASIGN_ANY = 0XAB,
    ALGO_RSASIGN_LAST = 0XAC,

	ALGO_INVALID=0xFF
	
} mxq_algo_id_t;


/**
 * Definition of cryptographic key usage.
 * @ingroup KeyManagement
 */

typedef enum _PACKED
{
	MXQ_KEYUSE_SECURECHANNEL    =0x00U, /**< Key can be used for secure channel */
	MXQ_KEYUSE_DATASIGNATURE	=0x01U, /**< Key/Cert can be used for generic data signature/verification */
	MXQ_KEYUSE_ATTESTATION		=0x11U, /**< Key/Cert can be used for generic data signature/verification */
	MXQ_KEYUSE_INTERNALENCRYPTION =0xA2U, /**< Key can be used for object or key data encryption/decryption */
	MXQ_KEYUSE_INTERNALMAC		=0xA1U, /**< Key can be used for MAC key derivation only*/
	MXQ_KEYUSE_ENCRYPTION		=0x02U, /**< Key can be used for generic data encryption/decryption */
	MXQ_KEYUSE_TLSAUTH			=0x04U, /**< Key can be used in TLS authentication */
	MXQ_KEYUSE_ADMINAUTH		=0x08U, /**< Key can be used to authenticate the manufacturer */
	MXQ_KEYUSE_ENDCUSTOMERAUTH	=0x18U, /**< Key can be used to authenticate the end customer */
	MXQ_KEYUSE_VERIFY_KEY_CERT	=0x10U, /**< Key/Cert can be used to verify certificates */
	MXQ_KEYUSE_VERIFY_FW		=0x20U, /**< Key can be used to verify firmware */
	MXQ_KEYUSE_RFU   			=0x40U, /**< RFU */
	MXQ_KEYUSE_KEY_IMPORT		=0x80U, /**< Key can be used to verify import/generate key requests */
	MXQ_KEYUSE_GPIO_CONTROL 	=0x8AU, /**< Key can be used to Secure GPIO Control */
	MXQ_KEYUSE_KEY_EXCHAGE		=0xD1U, /**< Key can be used for key exchange requests */
#ifdef MXQ_FW_UPDATE_CAP
	MXQ_KEYUSE_FIRMWARE_UPDATE  =0xA0U, /**< Key can be used to authenticate firmware updates */
#endif
	MXQ_KEYUSE_NONE         	=0xFFU  /**< Key can't be used */

} mxq_keyuse_t;


/**
 * Definition of Key Curves.
 * @ingroup KeyManagement
 */
typedef enum _PACKED
{
    /*  For MXQ_KEYTYPE_ECC: */
    MXQ_KEYPARAM_EC_P256R1, /**< NIST secP256r1 */
    MXQ_KEYPARAM_EC_P384R1, /**< NIST secP384r1  */
    MXQ_KEYPARAM_EC_P521R1, /**< NIST secP521r1 */
    MXQ_KEYPARAM_EC_BP256R1, /**< Brainpool P256r1 */
    MXQ_KEYPARAM_EC_BP384R1, /**< Brainpool P384r1  */
    MXQ_KEYPARAM_EC_BP512R1, /**< Brainpool P521r1 */
    MXQ_KEYPARAM_EDWARD_TBD,
#ifdef ENABLE_RSA
    MXQ_KEYPARAM_RSA_PUB3,
    MXQ_KEYPARAM_RSA_CRT_PUB3,
    MXQ_KEYPARAM_RSA_PUB65537,
    MXQ_KEYPARAM_RSA_CRT_PUB65537,
    MXQ_KEYPARAM_DHE = 0xAA,
#endif
    MXQ_UNKNOWN_CURVE
} mxq_keyparam_id_t;

/**
 * Definition of key types.
 * @ingroup KeyManagement
 */

typedef enum _PACKED
{
    MXQ_KEYTYPE_AES,  /**< AES Key. Supports AES 128 and 256, not 192 (less secure) */
    MXQ_KEYTYPE_PSK,  /**< TLS Pre Shared Key */
    MXQ_KEYTYPE_HMAC, /**< HMAC key. Key length must be suitable, adjusted to the used SHA algo */
    MXQ_KEYTYPE_ECC,  /**< ECC key. Key length must fit the curve! */
    MXQ_KEYTYPE_IKM,  /**< Input Key Material for HKDF! */
    MXQ_KEYTYPE_RSA,  /**< RSA key */

} mxq_keytype_id_t;

/**
 * Definition of export key command option flags.
 * @ingroup KeyManagement
 */
typedef enum _PACKED
{
    SHARE_SECRET= 1,
    GENERATE_M = 2,
    IS_AUTH_ENABLED = 8
} mxq_export_key_flag;

/**
 * MAXQ106x COMMAND SET (informative)
 * @ingroup Communication
 */
typedef enum  _PACKED{

    CMDNUM_CREATE_OBJECT					= 0x10U, /**< CREATE OBJECT */
    CMDNUM_DELETE_OBJECT					= 0x11U, /**< DELETE OBJECT */
    CMDNUM_READ_OBJECT					    = 0x12U, /**< READ OBJECT */
    CMDNUM_WRITE_OBJECT					    = 0x13U, /**< WRITE OBJECT */
    CMDNUM_UPDATE_COUNTER					= 0x14U, /**< UPDATE COUNTER*/
    CMDNUM_LIST_OBJECTS						= 0x15U, /**< LIST OBJECT */

    CMDNUM_TLS13_KEY_EXCHANGE				= 0xB0U, /**< TLS1.3 Key Exchange*/

    CMDNUM_ENCRYPT_DECRYPT					= 0xC0U,/**< ENCRYPT/DECRYPT DATA*/
    CMDNUM_MANAGE_SECURE_CHANNEL			= 0xC1U, /**< MANAGE SECURE CHANNEL*/
    CMDNUM_ADMIN_AUTHENTICATE				= 0xC2U,/**< ADMIN AUTHENTICATE*/
    CMDNUM_KEY_EXCHANGE						= 0xC3U, /**< */
    CMDNUM_TLS_RECORD						= 0xC4U, /**< */
    CMDNUM_IMPORT_KEY_ENC					= 0xC6U, /**< IMPORT KEY encrypted with a KEK. Not a command per se, uses command 0xCA*/
    CMDNUM_GET_CHALLENGE					= 0xC7U, /**< GET CHALLENGE*/
    CMDNUM_COMPUTE_VERIFY_DIGITAL_SIGNATURE = 0xC8U,/**< COMPUTE/VERIFY DIGITAL SIGNATURE*/
    CMDNUM_GET_RANDOM						= 0xC9U, /**< GET RANDOM*/
    CMDNUM_IMPORT_KEY						= 0xCAU, /**< IMPORT KEY*/
    CMDNUM_IMPORT_ROOTCERT					= 0xCBU, /**< IMPORT ROOT CERTIFICATE*/
    CMDNUM_VERIFY_BOOT						= 0xCCU,/**< VERIFY BOOT*/
    CMDNUM_ATTESTATION						= 0xCDU, /**< ATTESTATION*/
    CMDNUM_ESTABLISH_KEY					= 0xCEU, /**< ESTABLISH KEY*/
    CMDNUM_IMPORT_CHILDCERT					= 0xCFU, /**< IMPORT CHILD CERTIFICATE*/

    CMDNUM_COMPUTE_MESSAGE_DIGEST			= 0xD0U,/**< COMPUTE MESSAGE DIGEST*/
    CMDNUM_COMPUTE_MAC						= 0xD1U,/**< COMPUTE/VERIFY MAC*/

    CMDNUM_SLEEP							= 0xF1U, /**< SLEEP */
    CMDNUM_GETSTATUS						= 0xF2U, /**< GET STATUS */
    CMDNUM_SET_LIFE_CYCLE_STATE				= 0xF3U,/**< SET LIFE CYCLE STATE*/
    CMDNUM_SETCONFIG						= 0xF4U,/**< SET CONFIG*/
    CMDNUM_RESET_FACTORY					= 0xF5U,/**< FACTORY RESET*/
    CMDNUM_RESTORE							= 0xF6U,/**< RESTORE MAXQ1065*/
    CMDNUM_DUMP								= 0xF6U,/**< */
    CMDNUM_PING								= 0xF7U,/**< PING*/
    CMDNUM_GPIO_CONTROL 					= 0xFBU,/**< GPIO CONTROL*/
    CMDNUM_GPIO_READ 						= 0xFCU,/**< GPIO READ*/

#ifdef MXQ_FW_UPDATE_CAP
    CMDNUM_START_UPDATE					= 0xE0U, /**< START FW UPDATE*/
    CMDNUM_PROCESS_UPDATE				= 0xE1U, /**< PROCESS FW UPDATE*/
    CMDNUM_FINISH_UPDATE				= 0xE2U, /**< FINISH FW UPDATE*/
    CMDNUM_ENTER_BOOTLOADER				= 0xE3U, /**< ENTER BOOTLOADER*/
#endif
} cmd_num_t;


/** \addtogroup Ciphersuites
 *  @{
 */

#define TLS_DHE_PSK_WITH_AES_128_CBC_SHA         0x0090 /**< 0x00,0x90	TLS_DHE_PSK_WITH_AES_128_CBC_SHA [RFC4279]*/
#define TLS_DHE_PSK_WITH_AES_256_CBC_SHA         0x0091 /**< 0x00,0x91	TLS_DHE_PSK_WITH_AES_256_CBC_SHA [RFC4279]*/

#define TLS_PSK_WITH_AES_128_GCM_SHA256          0x00A8 /**< 0x00,0xA8	TLS_PSK_WITH_AES_128_GCM_SHA256  [RFC5487] - Only handshake*/
#define TLS_PSK_WITH_AES_256_GCM_SHA384          0x00A9 /**< 0x00,0xA9	TLS_PSK_WITH_AES_256_GCM_SHA384	 [RFC5487] - Only handshake*/  
#define TLS_DHE_PSK_WITH_AES_128_GCM_SHA256      0x00AA /**< 0x00,0xAA	TLS_DHE_PSK_WITH_AES_128_GCM_SHA256 [RFC5487] - Only handshake*/  
#define TLS_DHE_PSK_WITH_AES_256_GCM_SHA384      0x00AB /**< 0x00,0xAB	TLS_DHE_PSK_WITH_AES_256_GCM_SHA384 [RFC5487] - Only handshake*/  

#define TLS_PSK_WITH_AES_128_CBC_SHA256          0x00AE /**< 0x00,0xAE	TLS_PSK_WITH_AES_128_CBC_SHA256	[RFC5487]*/
#define TLS_PSK_WITH_AES_256_CBC_SHA384          0x00AF /**< 0x00,0xAF	TLS_PSK_WITH_AES_256_CBC_SHA384	[RFC5487]*/

#define TLS_DHE_PSK_WITH_AES_128_CBC_SHA256      0x00B2 /**< 0x00,0xB2	TLS_DHE_PSK_WITH_AES_128_CBC_SHA256 [RFC5487]*/
#define TLS_DHE_PSK_WITH_AES_256_CBC_SHA384      0x00B3 /**< 0x00,0xB3	TLS_DHE_PSK_WITH_AES_256_CBC_SHA384 [RFC5487]*/

#define TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA      0xC004 /**< 0xC0,0x04	TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA [RFC-ietf-tls-rfc4492bis-17]*/
#define TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA      0xC005 /**< 0xC0,0x05	TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA [RFC-ietf-tls-rfc4492bis-17]*/

#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA     0xC009 /**< 0xC0,0x09	TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA [RFC-ietf-tls-rfc4492bis-17]*/
#define TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA     0xC00A /**< 0xC0,0x0A	TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA [RFC-ietf-tls-rfc4492bis-17]*/

#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256  0xC023 /**< 0xC0,0x23	TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256	[RFC5289]*/
#define TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384  0xC024 /**< 0xC0,0x24	TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384	[RFC5289]*/
#define TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256   0xC025 /**< 0xC0,0x25	TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256	[RFC5289]*/
#define TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384   0xC026 /**< 0xC0,0x26	TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384	[RFC5289]*/

#define TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256  0xC02B /**< 0xC0,0x2B	TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256	[RFC5289] - Only handshake*/
#define TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384  0xC02C /**< 0xC0,0x2C	TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 [RFC5289] - Only handshake*/
#define TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256   0xC02D /**< 0xC0,0x2D	TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256	[RFC5289] - Only handshake*/
#define TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384   0xC02E /**< 0xC0,0x2E	TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384	[RFC5289] - Only handshake*/

#define TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA       0xC035 /**< 0xC0,0x35	TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA [RFC5489]*/
#define TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA       0xC036 /**< 0xC0,0x36	TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA [RFC5489]*/
#define TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256    0xC037 /**< 0xC0,0x37	TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 [RFC5489]*/
#define TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384    0xC038 /**< 0xC0,0x38	TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384 [RFC5489]*/

#define TLS_PSK_WITH_AES_128_CCM                 0xC0A4  /**< 0xC0,0xA4	TLS_PSK_WITH_AES_128_CCM [RFC6655]*/
#define TLS_PSK_WITH_AES_256_CCM                 0xC0A5  /**< 0xC0,0xA5	TLS_PSK_WITH_AES_256_CCM [RFC6655]*/
#define TLS_DHE_PSK_WITH_AES_128_CCM             0xC0A6  /**< 0xC0,0xA6	TLS_DHE_PSK_WITH_AES_128_CCM [RFC6655]*/
#define TLS_DHE_PSK_WITH_AES_256_CCM             0xC0A7  /**< 0xC0,0xA7	TLS_DHE_PSK_WITH_AES_256_CCM [RFC6655]*/
#define TLS_PSK_WITH_AES_128_CCM_8               0xC0A8  /**< 0xC0,0xA8	TLS_PSK_WITH_AES_128_CCM_8 [RFC6655]*/
#define TLS_PSK_WITH_AES_256_CCM_8               0xC0A9  /**< 0xC0,0xA9	TLS_PSK_WITH_AES_256_CCM_8 [RFC6655]*/
#define TLS_DHE_PSK_WITH_AES_128_CCM_8           0xC0AA  /**< 0xC0,0xAA	TLS_PSK_DHE_WITH_AES_128_CCM_8 [RFC6655]*/
#define TLS_DHE_PSK_WITH_AES_256_CCM_8           0xC0AB  /**< 0xC0,0xAB	TLS_PSK_DHE_WITH_AES_256_CCM_8 [RFC6655]*/

#define TLS_ECDHE_ECDSA_WITH_AES_128_CCM         0xC0AC  /**< 0xC0,0xAC	TLS_ECDHE_ECDSA_WITH_AES_128_CCM [RFC7251]*/
#define TLS_ECDHE_ECDSA_WITH_AES_256_CCM         0xC0AD  /**< 0xC0,0xAD	TLS_ECDHE_ECDSA_WITH_AES_256_CCM [RFC7251]*/
#define TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8       0xC0AE  /**< 0xC0,0xAE	TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 [RFC7251]*/
#define TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8       0xC0AF  /**< 0xC0,0xAF	TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 [RFC7251]*/

/** @}*/


/*
 *Defines. Do not modify.
 */

#define MAX_RANDOM_NUMBER_SIZE 		1024
#define LEN_KEY_BLOCK			256
#define KEY_EXCHANGE_BUFF_SIZE 		10
#define SERVER_RANDOM_SIZE 		256
#define BID_SIZE			16
#define MAC_LEN_SHA256			32
#define MAX_PAD_LEN			17
#define MAX_ECDH_KEY_SIZE		256	/* 2 * Max key size (521 bits) + 0x04 + margin */
#define MAX_EC_KEY_SIZE			(66 * 2)
#define MAX_DH_KEY_SIZE			(512*3) // RSA4096 Key Size * 3
#define MAX_CCM_PACKET_LENGTH		1920
#define MAX_TLS_PACKET_SIZE		16384
#define MAX_AAD_CCM_LENGTH		64

#define MAX_COMM_BUFFER			2048  	/* if change, update MXQ_API.h */
#define MAX_COMM_DATA_LEN		(MAX_COMM_BUFFER-128)

#ifdef __MAXQ106x_C2__
#define CHALLENGE_SIZE				80
#else
#define CHALLENGE_SIZE				96
#endif


/**
 * Access conditions for objects.
 * @internal
 */
typedef enum _PACKED
{
	COND_NA			=0x00U, /**< Never */
#if !defined(MAXQ106x)
	COND_ENDUSER	=0x01U, /**< Access if secure boot (independent of others) */
#endif
	COND_ADMIN		=0x02U, /**< Access if authenticate as administrator  (independent of others) */
	COND_SECUREBOOT	=0x04U, /**< Access if secure boot (independent of others) */
	COND_HOST		=0x08U, /**< Access if host  (independent of others) */
#if !defined(MAXQ106x)
	COND_NONE		=COND_ENDUSER+COND_HOST+COND_ADMIN+COND_SECUREBOOT /**< Access always */
#else
	COND_NONE		=COND_HOST+COND_ADMIN+COND_SECUREBOOT /**< Access always */
#endif
} access_cond_t;

#define NUM_ACC_COND_BITS 4

/**
 * Definition of key object types.
 * @ingroup ObjectManagement
 */
typedef enum _PACKED
{
	MXQ_OBJTYPE_SECRETKEY     = 0x01U, /**< Secret Key */
	MXQ_OBJTYPE_PUBKEY        = 0x02U, /**< Public key */
	MXQ_OBJTYPE_KEYPAIR       = 0x03U, /**< Key Pair */
	MXQ_OBJTYPE_TRANSPARENT   = 0x04U, /**< Transparent object */
	MXQ_OBJTYPE_COUNTER_UP    = 0x05U, /**< Monotonic counter incrementing */
	MXQ_OBJTYPE_X509          = 0x06U, /**< X.509 certificate */
	MXQ_OBJTYPE_COUNTER_DOWN  = 0x07U /**< Monotonic counter decrementing */
} mxq_objecttype_t;

/**
 * Object properties. Wipe on tamper can be combined using bitwise OR.
 * @ingroup ObjectManagement
 */
typedef enum _PACKED
{
	OBJPROP_TEMPORARY	    = 0x01,   /**< Data is volatile, but object remains. Use as little as possible (uses RAM) */
	OBJPROP_PERSISTENT	    = 0x02,  /**< Fully persistent object. */
	OBJPROP_WIPE_ON_TAMPER	    = 0x04  /**< Object data destroyed on GPIO tamper event (applies to both persistent and temporary) */

} mxq_object_prop_t;


/**
 * Definition TLS PSK context.
 * @internal
 */
typedef struct
{
	mxq_u2 psk_id;
	mxq_u1 psk_key_bloc[LEN_KEY_BLOCK];

} psk_id_t;


/**
 * TLS context structure. Do not modify the content.
 * @internal
 */
typedef struct
{
	key_exchange_case_t 	pass;
	mxq_u2 		CSID;
	mxq_u1* 	Random;
	mxq_u2  	Random_size;
	psk_id_t 	PSK_info;
	mxq_u2 	    is_session_key_secret;
	mxq_u1*     master_key; // unused placeholder
}mxq_secret_context_api_t;



/**
 * Initializes the host to enable communication with the security module.
 *
 * The underlying platform dependent functions will enable the
 * actual communication interface and required GPIOs.
 *
 * @return Execution status.
 * @retval MXQ_OK                         Success.
 *
 * @pre None
 * @post MAXQ10xx is ready to receive commands.
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_Module_Init(void);

/**
 * Deinitializes the host to close communication with the security module.
 *
 *
 *
 * @return Execution status.
 * @retval MXQ_OK                         Success.
 *
 * @pre None
 * @post MAXQ10xx communication interface is closed.
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_Module_Deinit(void);

void* __API__ MXQ_Module_Interface(void);
mxq_err_t __API__ MXQ_Module_SetInterface(void* i) ;

/**
 * Performs a Factory Reset 
 * @note Only available on debug parts
 *
 * @return Execution status.
 * @retval MXQ_OK                         Success.
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post All security conditions are disabled.
 * @post All pending operations (message digest, cipher, mac) are aborted
 * @post All internal context is reset (TLS session keys, Secure channel session keys, Internally stored randoms or digests)
 * @post All objects are deleted. Object 0x1000 (Key importation key) is reset to its default value. Object 0x1001 (Adminsitrator key
 *       is reset to its defautl value.
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_ResetFactory(void);


/**
 * Resets the MAXQ10xx through the RESET pin
 *
 * @return Execution status.
 * @retval MXQ_OK                         Success.
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post All security conditions are disabled.
 * @post All pending operations (message digest, cipher, mac) are aborted
 * @post All internal context is reset (TLS session keys, Secure channel session keys, Internally stored randoms or digests)
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_Reset(void);

/**
 * Displays the results in detail for the list object commands. It lists the existing object ids for option 1 and object
 * details for option 0 for given object id
 *
 * @param pOption           Option to use for displaying result of list object command. 0 for specific object details 1 for listing object ids found
 * @param pObjDetails       Result of the list object command that will be processed by display object details
 * @param pDetailsLen       Length of the input to be processed by the display function
 *
 * @return Execution status.
 * @retval MXQ_OK       Success.
 * @retval MXQ_RSP_ERROR_UNKNOWN    Unspecified error
 *
 * @pre MXQ_ListObject() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_DisplayObjectDetails(mxq_u1 pOption, mxq_u1* pObjDetails, mxq_length pDetailsLen);

/**
 * Lists existing object ids or object details in the MAXQ10xx non-volatile memory depending on the selected option.
 *
 * @param option            Option to use for list object. 0 for specific object details 1 for getting all object ids
 * @param objectid          Identifier of the object of interest for which details are queried
 * @param pdest             Destination buffer to send back the results
 * @param pmaxlen_readlen   Maximum read length for response
 *
 * @return Execution status.
 * @retval MXQ_OK       Success.
 * @retval 0x0002   Wrong access conditions
 * @retval MXQ_RSP_ERROR_UNKNOWN    Unspecified error
 * @retval MXQ_RSP_OTHER_UNKNOWN_ERROR  Unspecified error
 * @retval MXQ_RSP_ACCESS_DENIED    Security condition not satisfied
 * @retval MXQ_RSP_NOT_FOUND        Object not found
 * @retval MXQ_RSP_WRONG_PARAMS     Wrong parameter value (Invalid object ID, ...)
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_ListObject( mxq_u1 option, mxq_u2 objectid, mxq_u1* pdest, mxq_length* pmaxlen_readlen );

/**
 * Creates a new object in the MAXQ10xx non-volatile memory.
 *
 * @param objectid    Identifier of the new object
 * @param objectlen   Number of bytes allocated for the object
 * @param objtype     Object type (See mxq_objecttype_t)
 * @param objprops    Object type (See mxq_object_prop_t)
 * @param accessrules Object access rules
[ahs]=[rwdgx](,[ahs]=[rwdgx])*:[ahs]=[rwdgx](,[ahs]=[rwdgx])*:[ahs]=[rwdgx](,[ahs]=[rwdgx])* 
Delivery                      :Initialized                   :Operational 
- role:   a: admin, h:host, s:secureboot 
- access: r:read, w:write, d: delete, g: generate, x:execute

e.g. ah=r,s=w:a=d means 
Admin and Host can read and SecureBoot can write in Delivery state,
and admin can delete in Initialized state.

 *
 * @return Execution status.
 * @retval MXQ_OK       Success.
 * @retval ERR_FS_ID_ALREADY_EXISTS	Duplicated Object ID
 * @retval 0x0002 	Wrong access conditions
 * @retval MXQ_RSP_FS_WRONG_OBJECT_TYPE 	Wrong object type
 * @retval MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval MXQ_RSP_MEMORY_FULL 	Out of memory
 * @retval MXQ_RSP_ACCESS_DENIED 	Security condition not satisfied
 * @retval MXQ_RSP_INVALID_SIZE 	Allocated length too long
 * @retval MXQ_RSP_WRONG_PARAMS 	Wrong parameter value (Invalid object ID, ...)
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement	
 */

mxq_err_t __API__ MXQ_CreateObject(mxq_u2 objectid, mxq_length objectlen,
		mxq_objecttype_t objtype, mxq_object_prop_t objprops, char* accessrules);

/**
 * Creates a new counter object in the MAXQ10xx non-volatile memory.
 *
 * @param objectid     Identifier of the new object
 * @param objtype      Object type (See mxq_objecttype_t)
 * @param objprops     Object type (See mxq_object_prop_t)
 * @param accessrules  Object access rules (see MXQ_CreateObject())
 * @param init_max_val Start or Maximum Value
 *
 * @return Status of execution
 * @retval MXQ_OK       Success.
 * @retval ERR_FS_ID_ALREADY_EXISTS	Duplicated Object ID
 * @retval 0x0002 	Wrong access conditions
 * @retval MXQ_RSP_FS_WRONG_OBJECT_TYPE 	Wrong object type
 * @retval MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval MXQ_RSP_MEMORY_FULL 	Out of memory
 * @retval MXQ_RSP_ACCESS_DENIED 	Security condition not satisfied
 * @retval MXQ_RSP_INVALID_SIZE 	Allocated length too long
 * @retval MXQ_RSP_WRONG_PARAMS 	Wrong parameter value (Invalid object ID, ...)
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_CreateCounter(mxq_u2 objectid, mxq_objecttype_t objtype,
		mxq_object_prop_t objprops, char* accessrules, mxq_u4 init_max_val);

/**
 * Deletes an object from the MAXQ10xx non-volatile memory.
 *
 * @param objectid  Identifier of the  object
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_DeleteObject(mxq_u2 objectid);

/**
 * Writes data into an object located in the MAXQ10xx non-volatile memory.
 * The object must be of type "Transparent".
 *
 * @param objectid    Identifier of the object
 * @param dest_offset Start offset in the object
 * @param psrc        Pointer to the data to copy into the object
 * @param len         Length of the data to write
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_WriteObject(mxq_u2 objectid, mxq_u2 dest_offset, mxq_u1* psrc, mxq_u2 len);

/**
 * Writes data into an object located in the MAXQ10xx non-volatile memory.
 * The object must be of type "Transparent".
 *
 * @param objectid    Identifier of the object
 * @param dest_offset Start offset in the object
 * @param psrc        Pointer to the data to copy into the object
 * @param len         Length of the data to write
 * @param keyid       Identifier of the key object used for encryption purposes
 * @param algoid      Identifier of the desired encryption algorithm
 * @param pIv         Pointer to the IV buffer if encryption is enabled and selected algorithm supports IV
 * @param pTag        Pointer to the Tag buffer if encryption is enabled and selected algorithm supports Tag
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval  MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_WriteObjectV2(
                                    mxq_u2 objectid,
                                    mxq_u2 dest_offset,
                                    mxq_u1* psrc,
                                    mxq_u2 len,
                                    mxq_u2 keyid,
                                    mxq_algo_id_t algoid,
                                    mxq_u1* pIv,
                                    mxq_u1* pTag);

/**
 * Increments or decrements the specified counter object.
 *
 * @param objectid    Identifier of the object
 * @param countval    Return Counter value
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_UpdateCounter(mxq_u2 objectid, mxq_u4* countval);


/**
 * Reads data from an object located in the MAXQ10xx non-volatile memory.
 * The object must be of type "Transparent", "Public key" or "Counter".
 *
 * @param objectid        Identifier of the object
 * @param src_offset      Start offset in the object
 * @param pdest           Pointer to the data to copy into the object
 * @param pmaxlen_readlen In input: length of the data to read.
 *                        In output, length actually read
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_ReadObject(mxq_u2 objectid , mxq_u2 src_offset, mxq_u1* pdest, mxq_length* pmaxlen_readlen);

/**
 * Reads data from a counter object located in the MAXQ10xx non-volatile memory.
 *
 * @param objectid        Identifier of the object
 * @param countval        Pointer to a variable to receive counter value
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup ObjectManagement
 */
mxq_err_t __API__ MXQ_ReadCounter(mxq_u2 objectid, mxq_u4* countval);


/**
 * Tests the communication link with the MAXQ10xx by sending data to the MAXQ10xx.
 *
 * The MAXQ10xx echoes back the transmitted data. An error is returned if the received bytes do not match.
 *
 * @param l length of data sent back and forth.
 *
 * @return Execution status.
 * @retval MXQ_OK       Success.
 * @retval MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Communication
 */
mxq_err_t __API__ MXQ_Ping(mxq_length l);

/**
 * Generates random challenge.
 *
 * The generated random challenge  is preserved in memory for a potential further usage (Admin authenticate, Open Secure Channel).
 * The random preserved in memory is discarded once another command has been issued (whether the command used the random or not).
 *
 * Designed for NIST SP_800-90A/B https://csrc.nist.gov/CSRC/media/Publications/sp/800-90a/rev-1/final/documents/sp800-90a_r1_draft_november2014_ver.pdf
 *
 * @param p_buff_out	output buffer for the generated random bytes
 * @param len			number of random bytes to generate
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	     Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR  Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	     Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Random
 */
mxq_err_t __API__ MXQ_Get_Challenge(mxq_u1* p_buff_out, mxq_length len);


/**
 * Generates random string of bytes.
 *
 * Options allow to bypass the DRBG post processing, or run a on-demand health test
 *
 * Designed for NIST SP_800-90A/B https://csrc.nist.gov/CSRC/media/Publications/sp/800-90a/rev-1/final/documents/sp800-90a_r1_draft_november2014_ver.pdf
 *
 * @param p_buff_out	output buffer for the generated random bytes
 * @param len			number of random bytes to generate
 * @param option		0: get post processed random with HMAC-DRBG, 1: Run ODHT, 2: Get Entropy bytes
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	     Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR  Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	     Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Random
 */
mxq_err_t __API__ MXQ_Get_Random_Ext(mxq_u1* p_buff_out, mxq_length len, mxq_u1 option);

/**
 * Generates random string of bytes compliant with SP800-90B/A
 *
 * @param p	    output buffer for the generated random bytes
 * @param l		number of random bytes to generate
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	     Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR  Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	     Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Random
 */
#ifdef MAXQ106x
#define MXQ_Get_Random(p,l) MXQ_Get_Challenge(p,l)
#else
#define MXQ_Get_Random(p,l) MXQ_Get_Random_Ext(p,l,0)
#endif

/**
*
*/
mxq_err_t __API__ MXQ_Attestation(mxq_u1* p_buff_out, mxq_length len, mxq_u1 option, mxq_u2 key_id, mxq_algo_id_t alg_id, mxq_u2 objid);

#ifdef MAXQ108x
/**
 * Secure gpio control command
 *
 * @param pSecGpioId		gpio id
 * @param pSecGpioMode		gpio mode
 * @param pPulseDuration	pulse duration
 * @param pOutputCond		output condition
 * @param pSignAlgo			signature algorithm
 * @param pKeyId			key id for verification
 * @param pSignature		singature data
 * @param pSignLen			length of signature
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	     Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR  Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	     Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_GPIO_Ctrl(mxq_u1 pSecGpioId, mxq_u1 pSecGpioMode,
								mxq_u2 pPulseDuration, mxq_u2 pOutputCond,
								mxq_algo_id_t pSignAlgo, mxq_u2 pKeyId,
								mxq_u1* pSignature, mxq_u1 pSignLen);

/**
 * Secure gpio read command
 *
 * @param pSecGpioId	gpio id
 * @param pSignAlgo		signature algorithm
 * @param pKeyId		key id for verification
 * @param output		output buffer
 * @param olen			output data length
 * @param pChallenge	16-byte random number
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	     Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR  Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	     Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_GPIO_Read(mxq_u1 pSecGpioId, mxq_algo_id_t pSignAlgo, 
								mxq_u2 pKeyId, mxq_u1* output, mxq_length* olen,
								mxq_u1* pChallenge);
#endif

/**
 * Performs ECDH-based key exchange with MAXQ106x..
 * Internally computes the TLS session keys based on:
 *
 * @param option_flag		    Option Flag
 * @param ec_type		        Eliptic Curve ID
 * @param key_pair_id		    ECDH Local Key pair id
 * @param pub_key_id		    Server ECDH Pubkey ID
 * @param pub_key		        Explicit pub key buffer (if Server PubKey ID is 0xffff)
 * @param server_rand_len	    Server Random length
 * @param server_rand_buf	    Server Random Buffer
 * @param dst_Aes1_kid		    Derived AES Key 1 Dest ID
 * @param dst1_key_len		    Derived AES Key 1 Length
 * @param dst_Aes2_kid		    Derived AES Key 2 Dest ID
 * @param dst2_key_len		    Derived AES Key 2 Length
 * @param fixed_info_len	    Fixed info len
 * @param fixed_info		    Fixed info buffer
 * @param sign_verif_key_id		Signature Verification key pair ID
 * @param signing_key_pair_id	Signing key pair ID
 * @param signature		        Server’s Signature
 * @param out_buf		        Output buffer
 * @param out_buf_len           Output Data Length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup KeyManagement
 */
mxq_err_t __API__ MXQ_EstablishKey(
    mxq_u1 option_flag, mxq_keyparam_id_t ec_type, mxq_u2 key_pair_id, mxq_u2 pub_key_id, mxq_u1* pub_key,
    mxq_u1 server_rand_len, mxq_pu1 server_rand_buf,
    mxq_u2 dst_Aes1_kid, mxq_u1 dst1_key_len,
    mxq_u2 dst_Aes2_kid, mxq_u1 dst2_key_len,
    mxq_u1 fixed_info_len, mxq_pu1 fixed_info,
    mxq_u2 sign_verif_key_id, mxq_u2 signing_key_pair_id, mxq_u1* signature,
    mxq_pu1 out_buf, mxq_length* out_buf_len);

/**
 * Performs the TLS key exchange.
 * Internally computes the TLS session keys based on:
 *  - the PSK ID
 *  - the previous ECDH operation
 *  
 * Session keys can be kept internal to the MAXQ10xx or exported as output of this command.
 * 
 * @param key_Exchange_Session		Context
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_Perform_Key_Exchange( mxq_secret_context_api_t *key_Exchange_Session );

/**
 * Generates MAXQ1080 side TLS Key.
 *
 * @param key                   Public Key read back
 * @param key_id                Identifier of the Key object
 * @param key_type              Type of Key
 * @param key_param_id          Key Param Type
 * @param key_size              Size of Key
 * @param p                     DH p param (Only used at Diffie-Hellman)
 * @param g_size                DH g param size (Only used at Diffie-Hellman)
 * @param g                     DH g param (Only used at Diffie-Hellman)
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_TLS13_Generate_Key(mxq_u1* key, mxq_u2 key_id, mxq_keytype_id_t key_type, mxq_keyparam_id_t key_param_id, mxq_u2 key_size, mxq_u1* p, mxq_u2 g_size, mxq_u1* g);
/**
 * Creates premaster secret on MAXQ1080.
 *
 * @param peer_key              Peer Public Key
 * @param peer_key_size         Size of Peer Key
 * @param key_id                Previously generated MAXQ ECC/DH key ID
 * @param key_type              Type of Key
 * @param key_param_id          Key Param Type
 * @param CSID                  Cipher Suite ID
 * @param shared_secret_id   	Shared Secret Object ID
 * @param secret                Premaster secret
 * @param secret_size           Size of Premaster Secret
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_TLS13_Create_Secret(mxq_u1* peer_key, mxq_u2 peer_key_size, mxq_u2 key_id, mxq_keytype_id_t key_type, mxq_keyparam_id_t key_param_id, mxq_u2 CSID, mxq_u2 shared_secret_id, mxq_u1* secret, mxq_length *secret_size);

/**
 * Updates  the previously set IV value
 *
 * @param key_id                Key ID of the IV to updated
 * @param iv                    IV value
 * @param iv_len                Length of the IV
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_TLS13_Update_IV(mxq_u2 key_id, const mxq_u1* iv, mxq_u2 iv_len);
/**
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_TLS13_Extract_Secret(mxq_algo_id_t digest_id, mxq_u2 ret_kid, mxq_u1* prk, mxq_length* prk_len, mxq_u2 salt_kid, const mxq_u1* salt, mxq_u2 salt_len, mxq_u2 ikm_kid, const mxq_u1* ikm, mxq_u2 ikm_len);

/**
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_TLS13_Expand_Secret(mxq_algo_id_t digest_id, mxq_u2 ret_kid, mxq_keytype_id_t ret_keytype, mxq_u1 ret_isiv, mxq_u1* okm, mxq_length* okm_len, mxq_u2 prk_kid, const mxq_u1* prk, mxq_u2 prk_len, const mxq_u1* info, mxq_u2 info_len);

/**
 * Creates premaster sercret on MAXQ1080 and encrypts it by using RSA.
 *
 * @param data		            TLS Version Data
 * @param encrypted_pms		    Encrypted premaster secretvalue
 * @param data_len				Input: version data length, output: encrypted data length
 * @param cert_id				Certificate ID used for encryption
 * @param CSID					Cipher Suite ID
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_Encrypt_PMS_with_RSA(mxq_u1* data, mxq_u1* encrypted_pms, mxq_length* data_len, mxq_u2 cert_id, mxq_u2 CSID);

/**
 * Performs an ECDH operation to compute a shared secret Zab and keep it stored into the MAXQ10xx
 * for further use as a premaster secret for TLS.
 * 
 * The client public key is output as a result of this operation (ephemeral (ECDHE) or static (ECDH))
 * 
 * @param curve_id				ECC Curve ID
 * @param server_public_key		Server Public Key
 * @param client_public_key		Client Oublic Key
 * @param key_len				Key Length
 * @param CSID					Cipher Suite ID
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @retval  other   See mxq_err_t
 * @ingroup TLS

 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 */
mxq_err_t __API__ MXQ_Ecdh_Compute_Shared(
		mxq_keyparam_id_t 	curve_id,
		mxq_u1* server_public_key,
		mxq_u1* client_public_key,
		mxq_length key_len, mxq_u2 CSID);

/**
 * Invokes the command PERFORM_KEY_EXCHANGE within the MAXQ10xx with options
 * to set the server ECDHE key to be used within the scope of an ECDHE cipher suite.
 *
 * This function is designed to handle the ServerKeyExchange message where the server
 * sends its ephemeral ECDHE public key into a signed structure.
 * 
 * This function verifies the signature of the structure with the server's certificate.
 * 
 * The server certificate must have been provided through the command MXQ_ImportChildCert beforehand.
 *
 * @param algoid         Algorithm that was used to sign the ECDHE parameters (one of ALGO_ECDSA_SHA_xxx)
 * @param curveID		 ECC Curve ID
 * @param na 			 Unused
 * @param random		 Random Buffer
 * @param randomlen		 Random Length
 * @param ecdheparams	 ECDHE Parameters from server
 * @param ecdheparamslen ECDHE Parameters length
 * @param signature		 Signature of ECDHE parameters
 * @param signaturelen	 Signature Length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 * @ingroup TLS
 *
 */
mxq_err_t __API__ MXQ_SetECDHEKey(
			mxq_algo_id_t algoid,
			mxq_keyparam_id_t curveID,
			mxq_u2 na,
			mxq_u1* random,      mxq_length randomlen,
			mxq_u1*ecdheparams,  mxq_length ecdheparamslen,
			mxq_u1*signature,    mxq_length signaturelen);

/**
 * Invokes the command PERFORM_KEY_EXCHANGE within the MAXQ10xx with options
 * to perform a PRF SHA256 operation according to RFC5246 Chap. 5.
 *
 * @param key_ID	Identifier of the Key object
 * @param label		Label
 * @param llen		Label Length	
 * @param rand		Random 
 * @param rlen		Random Length
 * @param destbuf	Destination Buffer
 * @param dlen		Destination buffer length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_tls_prf_sha_256(mxq_u2 key_ID,
								const mxq_u1* label,mxq_length llen,
								const mxq_u1* rand, mxq_length rlen,
								mxq_u1* destbuf, mxq_length dlen);

/**
 * Performs AES CBC processing of a TLS packet.
 * It uses the MAXQ10xx by using the command PROCESS_TLS_RECORD.
 *
 * @param direction	Direction (Server => Client / Client => Server )
 * @param key_ID	Identifier of the Key object
 * @param iv		Initialisation Vector
 * @param iv_len	Initialisation Vector Length
 * @param input		Input Buffer
 * @param ilen		Input Buffer Length
 * @param output	Output Buffer
 * @param olen		Output Buffer Length
 * @param hdr_msg	Header Message
 * @param hdr_msg_len 	Header message Length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__ MXQ_tls_records(mxq_operation_t direction,
								mxq_u2 key_ID,
								const unsigned char *iv, size_t iv_len,
								const mxq_u1* input, mxq_length ilen,
								mxq_u1* output, mxq_length *olen,
								mxq_u1* hdr_msg, mxq_length hdr_msg_len);

mxq_err_t __API__  MXQ_cipher_auth_ccm(mxq_u2 key_ID,
		mxq_u1 direction,
		mxq_u1 *iv, mxq_length iv_len,
		mxq_u1 *ad, mxq_length ad_len,
		mxq_u1 *input, mxq_length ilen,
		mxq_u1 *output, mxq_length *olen,
		mxq_u1 *tag, mxq_length tag_len);

mxq_err_t __API__ MXQ_tls_cipher_cbc(mxq_operation_t direction,
		mxq_u2 key_ID,
		const unsigned char *iv, size_t iv_len,
		const mxq_u1* input, mxq_length ilen,
		mxq_u1* output, mxq_length *olen,
		mxq_u1* hdr_msg, mxq_length hdr_msg_len,
		mxq_length maclen);

/**
 * Performs AES CCM or GCM processing of a TLS packet.
 * It uses the MAXQ10xx by using the command PROCESS_TLS_RECORD.
 *
 * @param key_ID	Identifier of the Key object
 * @param direction	Direction (Server => Client / Client => Server )
 * @param iv		Initialisation Vector
 * @param iv_len	Initialisation Vector Length
 * @param ad		Aditional Data Buffer
 * @param ad_len	Aditional Data Length
 * @param input		Input Buffer
 * @param ilen		Input Buffer Length
 * @param output	Output Buffer
 * @param olen		Output Buffer Length
 * @param tag		Tag
 * @param tag_len	Tag Length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__  MXQ_tls_cipher_aead(
		mxq_u2 key_ID,
		mxq_operation_t direction, // 1: encrypt (TX packets, outbound), 0: decrypt (RX packets, inbound)
		mxq_u2 algo,
		mxq_u1 *iv, mxq_length iv_len,    // IV/nonce
		mxq_u1 *aad, mxq_length aad_len,  // AAD
		mxq_u1 *input, mxq_length ilen,   // Payload
		mxq_u1 *output, mxq_length *olen, // output
		mxq_u1 *tag, mxq_length tag_len);  // input or output tag (depends on direction)



#define SIZE_RND_BYTES 32

/**
 * server certificate structure
 */
typedef struct {
        mxq_u1* der; /*< Array of bytes containing the ASN.1 DER encoding of the certificate */
        mxq_length len; /* Lengh of DER encoding */
        mxq_keyparam_id_t curve; /* EC Curve of the public key of this certificate */
        mxq_length pk_offset; /* Offset of the SubjectPublicKey field in this certificate */
        mxq_length tbslen; /* Length of the signed section of this certificate*/
        mxq_u2 parent_id; /* Object ID of the root certificate in the MAXQ */
        mxq_keyparam_id_t parent_curve; /* Curve of the root certificate pub key in the MAXQ */
}    mxq_cert;


/**
 * @brief Performs an ECDH key exchange
 * 
 * Generates a fresh ECDH key pair in the MAXQ1061 and computes ECDH with previously imported certificate using MXQ_ImportChildCert()
 *
 * Optionnally authenticates the MAXQ1061 by signing client_random|server_random|client_ecdhe_key
 *
 * Generated keys can be used with function MXQ_client_encrypt() and MXQ_client_decrypt(). They can be
 * kept internal to the MAXQ10xx in that case (Modify MXQ_API.c  DEBUG_KEY define to '1' if you need to output the keys).
 *
 * @param[in]  remote_random      The server random
 * @param      srvcert            The server certificate structure. The ec-curve used for the ECDH is defined by this certificate.
 * @param      client_ecdhe_key   The generated client ecdhe key encoded as 0x04 | X | Y
 * @param      client_random      The generated client random
 * @param[in]  client_signkey     The client signature key to sign message client_random|remote_random|client_ecdhe_key
 * @param      client_ecdsa_sign  The client ecdsa signature (not generated if client_signkey=0)
 * @param      aeskey             The 2 generated aeskeys: first is for client->server, 2nd is for server->client. 
 *                                Output only if built with option DEBUG_KEY 1
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval  Other   Error code
 * 
 * @pre MXQ_Module_Init() and MXQ_ImportChildCert() must have been called before. The MAXQ1061 must contain a Root public key
 * for server certificate verification, and optionally and key pair for the client authentication
 * 
 * @ingroup KeyManagement
 */
mxq_err_t MXQ_client_key_exchange(
        const mxq_u1* remote_random, //in
        mxq_cert* srvcert,
        mxq_u1* client_ecdhe_key, // out. format 0x04 X Y
        mxq_u1* client_random, // out
        mxq_u2 client_signkey,
        mxq_u1* client_ecdsa_sign,
        mxq_u1* aeskey[] //out
        );

/**
 * Performs an AES-CBC decryption with the keys exchanged using MXQ_client_key_exchange().
 *
 * @param cleartext The cleartext data (up to 2048 bytes)
 * @param ciphertext The ciphertext data (up to 2048 bytes)
 * @param ciphertext_len The length of the ciphertext, must be multiple of 16 bytes
 * @param IV Externally supplied IV
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval  Other   Error code
 *
 * @pre MXQ_client_key_exchange() must have been called before.
 * @post None
 *
 * @ingroup Cipher
 */
mxq_err_t MXQ_client_decrypt(mxq_u1* cleartext, const mxq_u1* ciphertext, mxq_length ciphertext_len, const mxq_u1 IV[16]);

/**
 * Performs an AES-CBC encryption with the keys exchanged using MXQ_client_key_exchange().
 *
 * @param ciphertext The ciphertext data (up to 2048 bytes)
 * @param cleartext The cleartext data (up to 2048 bytes)
 * @param cleartext_len The length of the cleartext, must be multiple of 16 bytes
 * @param IV Externally supplied IV (should be impredictable)
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval  Other   Error code
 *
 * @pre MXQ_client_key_exchange() must have been called before.
 * @post None
 *
 * @ingroup Cipher
 */
mxq_err_t MXQ_client_encrypt(mxq_u1* ciphertext, const mxq_u1* cleartext, mxq_length cleartext_len, const mxq_u1 IV[16]);


/**
 * @brief Performs an ECDHE key exchange with pre-shared keys
 * 
 * Generates a fresh ECDH key pair in the MAXQ1061 and computes ECDH based on a pre-shared key
 *
 * The generated shared AES key is output for further use by the host processor, including using the MAXQ1061 AES engine.
 *
 * Generated keys can be used with function MXQ_client_encrypt() and MXQ_client_decrypt()
 * 
 * @param      remote_ecdhe_pubkey    The ECDHE Public Key
 * @param[in]  remote_random          The server random
 * @param      client_ecdhe_key       The generated client ecdhe key encoded as 0x04 | X | Y
 * @param      client_random          The generated client random
 * @param[in]  curve_id               The EC-Curve to use for the ECDHE protocol
 * @param[in]  PSK_ID                 The Pre-shared Key ID
 * @param      aeskey                 The generated shared AES key
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval  Other   Error code
 * 
 * @pre MXQ_Module_Init() and MXQ_ImportChildCert() must have been called before. The MAXQ1061 must contain a Root public key
 * for server certificate verification, and optionally and key pair for the client authentication
 * 
 * @ingroup KeyManagement
 */

mxq_err_t MXQ_client_key_exchange_psk(
        mxq_u1* remote_ecdhe_pubkey,
        const mxq_u1* remote_random, //in
         // out. format 0x04 X Y
        mxq_u1* client_ecdhe_key, // out. format 0x04 X Y
        mxq_u1 client_random[SIZE_RND_BYTES], // out
        mxq_keyparam_id_t curve_id,de .
	
        int PSK_ID, 
        mxq_u1 aeskey[32] //out
        );

/**
 * Starts a new cipher operation.
 *
 * Initializes the Cipher engine with the specified algorithm for encryption/decryption.
 *
 * @param _1encrypt_0decrypt Operation: Encrypt(1) or Decrypt(0)
 * @param cipher_algo_id	Identification of Cipher Algorithm
 * @param key_object_id		Key object Identifier
 * @param param				Algorithm dependent parameters
 * @param doSwitchAesSpi	(on MAXQ1061/62 only) Fetch key object from internal file system, loads it into the AES-SPI engine, and switches to AES-SPI mode.
 *
 * Depending on the current mode, this function will either use the Crypto toolbox mode or the AES-SPI mode.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval      MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @retval      MXQ_RSP_WRONG_PARAMS    Erroneous parameter values
 * @retval      MXQ_RSP_WRONG_PARAM     Erroneous parameter values
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Cipher
 */
mxq_err_t __API__ MXQ_Cipher_Init(
		mxq_u1 _1encrypt_0decrypt,
		mxq_algo_id_t cipher_algo_id,
		mxq_u2 	key_object_id,
		ciph_params_t * param,
		int doSwitchAesSpi);

/**
 * Continues the on-going cipher operation.
 *
 * This function encrypts or decrypts the provided message part. The message
 * to process can be split arbitrarily in variable size chunks.
 *
 * The input buffer size can be any. The output buffer size will be
 * rounded to the cipher block size.
 *
 * When the last chunk is provided, the overall message length must be a
 * multiple of the cipher block size, meaning that padding must be applied by the caller.
 * 
 * Depending on the current mode, this function will either use the Crypto toolbox mode or the AES-SPI mode (on MAXQ1061/62 only).
 *
 * @param pdest Destination buffer for the result
 * @param psrc  Source buffer
 * @param len   Message chunk length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre MXQ_Cipher_Init() must have been caled before.
 * @post None
 *
 * @ingroup Cipher
 *
 */
mxq_err_t __API__ MXQ_Cipher_Update(mxq_u1** pdest, const mxq_u1* psrc, mxq_length* len);

/**
 * Finishes the current cipher operation.
 *
 * Depending on the current mode, this function will either use the Crypto toolbox mode or the AES-SPI mode (on MAXQ1061/62 only)
 *
 * @param additional_data additional data returned by the algorithm (AES-CCM encryption: U)
 * @param additional_data_len
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval 	MXQ_RSP_OTHER_UNKNOWN_ERROR 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @retval      MXQ_RSP_WRONG_PARAMS    Erroneous parameter values
 * @retval      MXQ_RSP_WRONG_PARAM     Erroneous parameter values
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre MXQ_Cipher_Update() must have been caled before.
 * @post None
 *
 * @ingroup Cipher
 */
mxq_err_t __API__ MXQ_Cipher_Finish(mxq_u1** additional_data, mxq_length* additional_data_len);

/**
 * Starts a new MAC operation. 
 *
 * Initializes the MAC engine with the specified algorithm.
 *
 * @param _1compute_0verify Computation mode: 1 compute MAC, 0: verify MAC
 * @param mac_algo_id     Algorithm
 * @param key_object_id   MAC key to use
 * @param IV              Initialization Vector (IV)
 * @param ivlen           IV length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval      MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @retval      MXQ_RSP_WRONG_PARAMS    Erroneous parameter values
 * @retval      MXQ_RSP_WRONG_PARAM     Erroneous parameter values
 * @ingroup MAC
 */
mxq_err_t __API__ MXQ_MAC_Init(
		mxq_u1 _1compute_0verify,
		mxq_algo_id_t mac_algo_id,
		mxq_u2 	key_object_id,
		mxq_u1* IV,
		mxq_length ivlen);

/**
 * Continues the on-going MAC operation.
 * 
 * Accumulates the provided message part into the current MAC value. The message
 * to process can be split arbitrarily in variable size chunks.
 *
 * @param psrc   Message chunk source
 * @param inlen  Message chunk length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre MXQ_MAC_Init() must have been called before.
 * @post None
 *
 * @ingroup MAC
 */
mxq_err_t __API__ MXQ_MAC_Update(mxq_u1* psrc, mxq_length inlen);

/**
 * Finishes the on-going MAC operation and returns the result.
 *
 * @param pdest    Destination buffer for the new MAC value
 * @param destlen  Input: maximum MAC length, output: actual MAC length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre MXQ_MAC_Update() must have been called before.
 * @post None
 *
 * @ingroup MAC
 */
mxq_err_t __API__ MXQ_MAC_Finish(mxq_u1* pdest, mxq_length* destlen);

/**
 * Finishes the on-going MAC operation and compares internal result with provided MAC value.
 *
 * @param pmac     MAC value to be compared against the fresh value just computed
 * @param maclen   Length of the MAC value
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre MXQ_MAC_Update() must have been called before.
 * @post None
 *
 * @ingroup MAC
 */
mxq_err_t __API__ MXQ_MAC_Verify(mxq_u1 *pmac, mxq_length maclen);


/**
 * Starts a new message digest operation.
 *
 * Initializes the Message Digest engine with the specified algorithm.
 *
 * @param md_algo_id Message digest algorithm
 *
 * @return Status of execution
 * @retval	MXQ_OK			OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup MessageDigest
 */
mxq_err_t __API__ MXQ_MD_Init(mxq_algo_id_t md_algo_id);

/**
 * Continues the on-going message digest operation. 
 * 
 * Accumulates the provided message fragment to the current message digest value. The message
 * to process can be split arbitrarily in variable size fragments.
 *
 *
 * @param psrc   Message fragments source
 * @param inlen  Message fragments length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre The message digest engine must be initialized with MXQ_MD_Init() before sending the first fragment.
 * @post None
 *
 * @ingroup MessageDigest
 */
mxq_err_t __API__ MXQ_MD_Update(const mxq_u1* psrc, mxq_length inlen);


/**
 * Finishes the on-going message digest operation and returns the result.
 *
 * @param pdest   Destination buffer for the resulting message digest value
 * @param destlen Maximum length of the destination buffer. On output it contains
 *                the actual message digest length.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre At least one message fragment must have been processed using MXQ_MD_Update().
 * @post The result is held in an internal register for further use via MXQ_Sign(), MXQ_Verify(), MXQ_VerifyBoot()
 *
 * @ingroup MessageDigest
 */
mxq_err_t __API__ MXQ_MD_Finish(mxq_u1* pdest, mxq_length* destlen);

/**
 * Returns the output size of the provided Message Digest algorithm.
 *
 * @param md_algo_id Message digest algorithm
 *
 * @return Length or -1 if wrong algorithm
 *
 * @pre  None
 * @post None
 *
 * @ingroup MessageDigest
 */
int __API__ MXQ_Get_MD_OutputSize(mxq_algo_id_t md_algo_id);


int __API__ MXQ_get_curve_bytelen(mxq_keyparam_id_t curve_id);


mxq_algo_id_t __API__ MXQ_get_sign_alg_from_curve(mxq_keyparam_id_t maxq_id);


/**
 * Computes a digital signature using the specified key and algorithm.
 * Data to sign can be input directly or previously hashed using MXQ_MD_XXX functions and algorithm ALGO_ECDSA_INNER_MD.
 *
 * @param signalgo          Signature algorithm
 * @param privkey_objectid  Object ID of private key to use for signing
 * @param pdata             Data to sign (NULL if reusing result of data previously hashed using MXQ_MD_XXX functions)
 * @param datalen           Length of data to sign (or 0 if pdata==NULL)
 * @param psignout          Destination buffer for the computed signature
 * @param signlen           Maximum length of the destination buffer. On output, actual lenght of the signature.
 * @note When using ECDSA_PLAIN algorithm, the "data" must actually be the hash of the message to sign. Otherwise, the "data"
 * is the message to sign (it will be hashed by the MAXQ1061). For longer messages, the message can be hashed with MXQ_MD_XXX functions
 * and, as the last hash is stored internally, use the algorithm ECDSA_INNER_MD to sign this internal hash value.
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval  MXQ_RSP_NOT_FOUND 	Object not found
 * @retval  MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Data length error / Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @retval  MXQ_RSP_WRONG_PARAMS    Erroneous parameter values
 * @retval  MXQ_RSP_WRONG_PARAM     Erroneous parameter values
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre When using the algorithm ALGO_ECDSA_INNER_MD, a full message digest operation must
        have been performed using MXQ_MD_Init(), MXQ_MD_Update() and MXQ_MD_Finish()
 * @post None
 *
 * @ingroup Signature
 */
mxq_err_t __API__ MXQ_Sign(mxq_algo_id_t signalgo, mxq_u2 privkey_objectid, const mxq_u1* pdata, mxq_u2 datalen, mxq_u1* psignout, mxq_length* signlen);

/**
 * Verifies a Digital Signature using the specified key and algorithm.
 * Data to sign can be input directly or previously hashed using MXQ_MD_XXX functions and algorithm ALGO_ECDSA_INNER_MD.
 *
 * @param signalgo          Signature algorithm
 * @param pubkey_objectid   Object ID of public key to use for verifying the signature
 * @param pdata             Data to sign (NULL if reusing result of data previously hashed using MXQ_MD_XXX functions)
 * @param datalen           Length of data to sign (or 0 if pdata==NULL)
 * @param signature         Signature to verify
 * @param signlen           Length of the signature
 * @note When using ECDSA_PLAIN algorithm, the "data" must actually be the hash of the message to verify. Otherwise, the "data"
 * is the message to verify (it will be hashed by the MAXQ1061). For longer messages, the message can be hashed with MXQ_MD_XXX functions
 * and, as the last hash is stored internally, use the algorithm ECDSA_INNER_MD to verify this internal hash value.
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval  MXQ_RSP_NOT_FOUND 	Object not found
 * @retval  MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Data length error / Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @retval  MXQ_RSP_WRONG_PARAMS    Erroneous parameter values
 * @retval  MXQ_RSP_WRONG_PARAM     Erroneous parameter values
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre When using the algorithm ALGO_ECDSA_INNER_MD, a full message digest operation must
        have been performed using MXQ_MD_Init(), MXQ_MD_Update() and MXQ_MD_Finish()
 * @post None
 *
 * @ingroup Signature
 */
mxq_err_t __API__ MXQ_Verify(mxq_algo_id_t signalgo, mxq_u2 pubkey_objectid, const mxq_u1* pdata, mxq_u2 datalen, const mxq_u1* signature, mxq_u2 signlen);


/**
 * Builds a key importation request, to be further transmitted using MXQ_ImportKey().
 *
 * This function must be used to prepare the importation of any kind of key (secret, key pair, public).
 *
 * This function mangles the parameters into the structure "Certificate/Key data for importation" described in the MAXQ10xx User guide.
 *
 * @param pdest        Destination buffer for the key importation data.
 * @param destlen      Maximum length of the destination buffer. On output it contains
 *                     the actual certificate length.
 * @param keytype      Type of the key
 * @param keyparam     Key parameters (key curve ID)
 * @param keycomplen   Length of a key component 
 *                        (e.g same as total key length for AES, but length of EC curve coordinates for EC keys)
 * @param total_keylen Total length of the key
 * @param uu1          Usage#1 for the key
 * @param a1           Algorithm for usage#1
 * @param uu2          Usage#2 for the key
 * @param a2           Algorithm for usage#2
 * @param psrc         Plain value of the key (concatenation of x and y for ECC keys). Length is assumed 'total_keylen'.
 *
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval 	MXQ_ERR_BAD_LENGTH 	total_keylen must not be zero
 *
 * @ingroup KeyManagement
 */
mxq_err_t __API__ MXQ_BuildKey(
		mxq_u1* pdest, mxq_length* destlen,
		mxq_keytype_id_t keytype,
		mxq_keyparam_id_t keyparam,
		mxq_length keycomplen, mxq_length total_keylen,
		mxq_keyuse_t uu1, mxq_algo_id_t a1,
		mxq_keyuse_t uu2, mxq_algo_id_t a2,
		mxq_u1* psrc );

/**
 * Builds a key importation request, to be further transmitted using MXQ_ImportKey().
 *
 * This function must be used to prepare the importation of any kind of key (secret, key pair, public).
 *
 * This function mangles the parameters into the structure "Certificate/Key data for importation" described in the MAXQ10xx User guide.
 *
 * @param pdest        Destination buffer for the key importation data.
 * @param destlen      Maximum length of the destination buffer. On output it contains
 *                     the actual certificate length.
 * @param keytype      Type of the key
 * @param keyparam     Key parameters (key curve ID)
 * @param keycomplen   Length of a key component 
 *                        (e.g same as total key length for AES, but length of EC curve coordinates for EC keys)
 * @param total_keylen Total length of the key
 * @param uu1          Usage#1 for the key
 * @param a1           Algorithm for usage#1
 * @param uu2          Usage#2 for the key
 * @param a2           Algorithm for usage#2
 * @param psrc         Plain value of the key (concatenation of x and y for ECC keys)
 * @param len          Length of data in psrc
 * 
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval 	MXQ_ERR_BAD_LENGTH 	total_keylen must not be zero
 *
 * @ingroup KeyManagement
 */
mxq_err_t __API__ MXQ_BuildKey_Ex(
		mxq_u1* pdest, mxq_length* destlen,
		mxq_keytype_id_t keytype,
		mxq_keyparam_id_t keyparam,
		mxq_length keycomplen, mxq_length total_keylen,
		mxq_keyuse_t uu1, mxq_algo_id_t a1,
		mxq_keyuse_t uu2, mxq_algo_id_t a2,
		mxq_u1* psrc, mxq_length len );


mxq_err_t __API__ MXQ_BuildEK_Material(mxq_u1 *pdest, mxq_length *destlen, mxq_u1 *psrc, mxq_length srclen, 
 									   mxq_u2 kek_id, mxq_u1 *pIV, mxq_u1 *pTag);
/**
 * Builds a root certificate importation request, to be further transmitted using
 * MXQ_ImportRootCert() or builds a child certificate importation request, to be further transmitted using
 * MXQ_ImportChildCert().
 *
 * This function mangles the parameters into the structure "Certificate/Key data for importation" described in the MAXQ10xx User guide.
 *
 * @param pdest          Destination buffer for the generated importation request.
 * @param destlen        Maximum length of the destination buffer. On output it contains the actual importation request length.
 * @param keytype        Type of the certificate
 * @param keyparam       Key parameters (public key curve ID mxq_keyparam_id_t of the public key in the certificate)
 * @param keycomplen     Length of public 
 *                          (e.g same as total key length for AES, but length of EC curve coordinates for EC keys)
 * @param total_keylen   Total length of the key.
 * @param pubkeyoffset   Offset of the public key in the certificate
 * @param tbs_start      Start of the "to be signed" data
 * @param tbs_len        Length of the "to be signed" data
 * @param total_certlen  Total length of the certificate
 * @param verifalgo      Algorithm for the signature verification
 * @param verifkeyobjid  Key object ID for verification of this importation request (ID of the public Key importation key in the MAXQ10xx)
 * @param uu1            Usage#1 for the certificate
 * @param a1             Algorithm for usage#1
 * @param uu2            Usage#2 for the certificate
 * @param a2             Algorithm for usage#2
 * @param pcertbody      Buffer containing the X.509 DER encoded certificate body
 *
 * @return Status of execution
 * @retval	0x0000	OK
 *
 * @ingroup KeyManagement
 */
mxq_err_t __API__ MXQ_Build_EC_Cert(
		mxq_u1* pdest, mxq_length* destlen,
		mxq_keytype_id_t keytype,
		mxq_keyparam_id_t keyparam,
		mxq_length keycomplen, mxq_length total_keylen,
		mxq_u2  pubkeyoffset,
		mxq_length tbs_start,
		mxq_length tbs_len,
		mxq_length total_certlen,
		mxq_algo_id_t verifalgo,
		mxq_u2  verifkeyobjid,
		mxq_keyuse_t uu1, mxq_algo_id_t a1,
		mxq_keyuse_t uu2, mxq_algo_id_t a2,
		mxq_u1* pcertbody );


/**
 * Imports a key or a root certificate into the MAXQ10xx.

 * Convenience macro functions MXQ_ImportRootCert() and MXQ_ImportKeyCert() are a better choice
 * (those macro functions actually call MXQ_ImportKeyCert()).

 * @param destobj_id     Destination object ID for storing the imported key or certificate
 * @param verifalgo      Algorithm for the signature verification of the import request 
 * @param verifkey_id    Public key object ID for verification of the import request (Key Importation Key object is in the MAXQ10xx)
 * @param pdata          Key/certificate import request (generated using MXQ_BuildKey() or MXQ_Build_EC_Cert())
 * @param datalen        Length of key/certificate import request
 * @param sign           Signature of the key/certificate import request (signed with the Key importation key)
 * @param signlen        Length of the signature of the key/certificate import request
 * @param cmd            Type of request (certificate: 0xCB or key: 0xCA)
 *
 * @note The input pdata must be signed with the Key importation key.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval      MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval      MXQ_RSP_FS_OUT_OF_BOUNDS Destination object is too small to contain the key or certificate
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 * @retval      MXQ_RSP_WRONG_PARAMS    Erroneous parameter values
 * @retval      MXQ_RSP_WRONG_PARAM     Erroneous parameter values
 * @retval 	MXQ_RSP_CONSISTENCY_ERROR Bad certificate format
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup KeyManagement
 */
mxq_err_t __API__ MXQ_ImportKeyCert(
		mxq_u2 destobj_id,
		mxq_algo_id_t verifalgo,
		mxq_u2 verifkey_id,
		mxq_u1* pdata, mxq_length datalen,
		mxq_u1* sign, mxq_length signlen,
		cmd_num_t cmd
		);

/**
 * Imports a key into the MAXQ10xx.
 *
 * @param destid         Destination object ID for storing the imported key 
 * @param valg           Algorithm for the signature verification of the import request 
 * @param vkid           Public key object ID for verification of the import request (Key Importation Key object is in the MAXQ10xx)
 * @param data           Key import request (to be generated using MXQ_BuildKey())
 * @param datalen        Length of key import request
 * @param sign           Signature of the key import request (signed with the Key importation key)
 * @param signlen        Length of the signature of the key import request
 *
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @ingroup KeyManagement
 */
#define MXQ_ImportKey(destid,valg,vkid,data,datalen,sign,signlen) \
	MXQ_ImportKeyCert(destid,valg,vkid,data,datalen,sign,signlen,\
			CMDNUM_IMPORT_KEY)

/**
 * Imports an encrypted key into the MAXQ10xx.
 *
 * @param destid         Destination object ID for storing the imported key 
 * @param valg           Algorithm for the signature verification of the import request 
 * @param vkid           Public key object ID for verification of the import request (Key Importation Key object is in the MAXQ10xx)
 * @param data           Key import request (to be generated using MXQ_BuildKey())
 * @param datalen        Length of key import request
 * @param sign           Signature of the key import request (signed with the Key importation key)
 * @param signlen        Length of the signature of the key import request
 *
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval  MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @ingroup KeyManagement
 */
#define MXQ_ImportKeyEnc(destid,valg,vkid,data,datalen,sign,signlen) \
	MXQ_ImportKeyCert(destid,valg,vkid,data,datalen,sign,signlen,\
			CMDNUM_IMPORT_KEY_ENC)

/**
 * Imports a root certificate into the MAXQ10xx.
 *
 * @param destid         Destination object ID for storing the imported certificate
 * @param valg           Algorithm for the signature verification of the import request 
 * @param vkid           Public key object ID for verification of the import request (Key Importation Key object is in the MAXQ10xx)
 * @param data           Root certificate import request (to be generated using MXQ_Build_EC_Cert())
 * @param datalen        Length of the root certificate import request
 * @param sign           Signature of the root certificate import request (signed with the Key importation key)
 * @param signlen        Length of the signature of the root certificate import request
 *
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @ingroup KeyManagement
 */
#define MXQ_ImportRootCert(destid,valg,vkid,data,datalen,sign,signlen) \
	MXQ_ImportKeyCert(destid,valg,vkid,data,datalen,sign,signlen,\
			CMDNUM_IMPORT_ROOTCERT)

/**
 * Imports a child certificate into the MAXQ10xx.
 *
 * @param destobj_id     Object ID that will store the provided certificate
 * @param pdata          Child certificate import request (generated using MXQ_Build_EC_Cert())
 * @param datalen        Length of the child certificate import request
 *
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup KeyManagement
 */
mxq_err_t __API__ MXQ_ImportChildCert(
		mxq_u2  destobj_id,
		mxq_u1* pdata, mxq_length datalen
		);


/**
 * Authenticates the administrator of the MAXQ10xx. The administrator
 * must previously request a random using MXQ_GetRandom(), then sign the
 * random number using the private key matching the public key used for verification.
 *
 * Note: the Administrator private key is needed in order to compute the signature.
 *
 * In case of success, the ADMIN security condition is enabled.
 *
 * @param verifkey_id Public key used for verification of the signed random
 * @param algo        Algorithm used for the verification
 * @param sign        Signature of the random usign the Administrator private key
 * @param signlen     Length of the signature
 *
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre MXQ_GetRandom must have been called before (length 16 bytes).
 * @post If success, the ADMIN security condition is enabled.
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_AdminAuth(
		mxq_u2 verifkey_id,
		mxq_algo_id_t algo,
		mxq_u1* sign,
		mxq_length signlen);

/**
 * Resets the ADMIN security condition (cancels administrator authentication).
 *
 * @return Status of execution
 * @retval	MXQ_OK	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post The ADMIN security condition is disabled.
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_AdminAuthReset(void);


/**
 * Verifies the signature of the firmware previously hashed using MXQ_MD_XXX
 * functions.
 *
 * @note If the command fails, the MAXQ10xx may perform an internal reset and/or a host processor reset
 * (see MXQ_SetConfig()).
 *
 * @param verifkey_id Public key used for the verification of the firmware
 * @param algo        Algorithm used for the verification
 * @param sign        Signature of the firmware
 * @param signlen     Lenght of the signature
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval      MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ALGO_NOT_SUPPORTED  Invalid algorithm selection
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre A full message digest operation must
        have been performed using MXQ_MD_Init(), MXQ_MD_Update() and MXQ_MD_Finish()
 * @post In case of success, the SECURE_BOOT security condition is enabled.
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_VerifyBoot(
		mxq_u2 verifkey_id,
		mxq_algo_id_t algo,
		mxq_u1* sign, mxq_length signlen);

/**
 * Resets the SECURE_BOOT security condition (cancels firmware authentication).
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post The SECURE_BOOT security condition is disabled.
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_VerifyBootReset(void);

#if defined(MAXQ106x)
/**
 * Opens a secure channel with the host processor based on a shared secret key.
 * The host processor must previously request a random using MXQ_GetRandom().
 * On success, further communication with the MAXQ106x is encrypted and signed
 * using session keys. The HOST condition becomes active in the MAXQ106x.
 *
 * @note Once the secure channel is successfully initiated, all commands 
 * and responses are cryptographically protected. If any of the host or MAXQ106x
 * fails to verify the cryptographic protection of the received command or response,
 * the secure channel is immediately closed ans the HOST condition is reset.
 * In addition, the MAXQ106x may perform an internal reset and/or a host processor reset
 * (see MXQ_SetConfig()).
 *
 * @param channel_key_id Key used for the secure channel.
 * @param local_key Value of the secret key (owned by the host)
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval      MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post In case of success, the HOST security condition is enabled.
 * @post None
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_OpenSecureChannel(mxq_u2 channel_key_id, mxq_u1* local_key);
mxq_err_t __API__ MXQ_OpenSecureChannel2(mxq_u2 channel_key_id, mxq_u1* local_key);
#elif defined(MAXQ1065)
/**
 * Opens a secure channel with the host processor based on a shared secret key.
 * The host processor must previously request a random using MXQ_GetRandom().
 * On success, further communication with the MAXQ10xx is encrypted and signed
 * using session keys. The HOST condition becomes active in the MAXQ10xx.
 *
 * @note Once the secure channel is successfully initiated, all commands
 * and responses are cryptographically protected. If any of the host or MAXQ10xx
 * fails to verify the cryptographic protection of the received command or response,
 * the secure channel is immediately closed ans the HOST condition is reset.
 * In addition, the MAXQ10xx may perform an internal reset and/or a host processor reset
 * (see MXQ_SetConfig()).
 *
 * @param pMode Secure Channel Mode
 * @param pSCKeyId Secure Channel Key ID
 * @param pPrivateKey The secure channel Host private Key
 * @param pHostPubId Host Public Key or Certificate ID
 * @param pDevicePubKey The secure channel MAXQ public Key
 * @param local_key Value of the secret key (owned by the host)
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval      MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post In case of success, the HOST security condition is enabled.
 * @post None
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_OpenSecureChannel(mxq_u1 pMode, mxq_u2 pSCKeyId, mxq_pu1 pPrivateKey, mxq_u2 pHostPubId, mxq_pu1 pDevicePubKey, mxq_u1* local_key);
#else
/**
 * Opens a secure channel with the host processor based on a shared secret key.
 * The host processor must previously request a random using MXQ_GetRandom().
 * On success, further communication with the MAXQ10xx is encrypted and signed
 * using session keys. The HOST condition becomes active in the MAXQ10xx.
 *
 * @note Once the secure channel is successfully initiated, all commands
 * and responses are cryptographically protected. If any of the host or MAXQ10xx
 * fails to verify the cryptographic protection of the received command or response,
 * the secure channel is immediately closed ans the HOST condition is reset.
 * In addition, the MAXQ10xx may perform an internal reset and/or a host processor reset
 * (see MXQ_SetConfig()).
 *
 * @param pMode Secure Channel Mode
 * @param pSCKeyId Secure Channel Key ID
 * @param pPrivateKey The secure channel Host private Key
 * @param local_key Value of the secret key (owned by the host)
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval      MXQ_RSP_NOT_FOUND 	Object not found
 * @retval      MXQ_RSP_OBJECT_EMPTY    Specified key object is empty
 * @retval 	MXQ_RSP_CRYPTO_FAILURE  Error during operation (bad sequence of commands, wrong key length, or other internal error)
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post In case of success, the HOST security condition is enabled.
 * @post None
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_OpenSecureChannel(mxq_u1 pMode, mxq_u2 pSCKeyId, mxq_pu1 pPrivateKey, mxq_u1* local_key);
#endif
/**
 * Closes the active secure channel. Disables the HOST security condition.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post The HOST security condition is disabled.
 *
 * @ingroup Security
 */
mxq_err_t __API__ MXQ_CloseSecureChannel(void);


/**
 * Changes configuration parameters of the MAXQ10xx. Current configuration
 * can be read out using MXQ_GetStatus().
 *
 * @param s Pointer to the structure holding the new configuration.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre Administrator authentication is required
 * @post None
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_SetConfig(mxq_config_t* s);

/**
 * Changes the life cycle state value of the MAXQ10xx.
 * 
 * @warning This is a one way process. It is not possible to revert to a previous state.
 * 
 * @param newstate New life cycle state.
 * 
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @pre Administrator authentication is required
 * @post None
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_SetState(mxq_state_t newstate);


/**
 * Gets information about the MAXQ10xx.
 *
 * @param status Pointer to the structure that receives the status information.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_GetStatus(mxq_status_t* status);

 #if defined(MAXQ106x) || defined(MAXQ108x)
/**
 * Puts the MAXQ106x into sleep mode.
 *
 * @warning
 * The function MXQ_Wakeup must be supported by the underlying platform when using this function.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Administration
 */

mxq_err_t __API__ MXQ_Sleep(void);
 
/**
 * Triggers the WAKEUP pin of the MAXQ106x to resume from sleep mode.
 *
 * This function may be implemented by the underlying platform.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 *
 * @ingroup Administration
 */

mxq_err_t __API__ MXQ_Wakeup(void);
#endif

#if defined(MAXQ106x)
/**
 * Activates the AES-SPI mode (on MAXQ1061/62 only)
 *
 * This function may be implemented by the underlying platform.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 *
 * @warning This performs a reset of the MAXQ106x.
 *
 * @post All security conditions are disabled.
 * @post All pending operations (message digest, cipher, mac) are aborted
 * @post All internal context is reset (TLS session keys, Secure channel session keys, Internally stored randoms or digests)
 *
 * @ingroup Administration
 */
mxq_err_t __API__  MXQ_GCU_Enable(void);
mxq_err_t __API__ MXQ_GCU_Direct(void);
#endif
#if defined(MAXQ1065)
/**
 * Puts the MAXQ1065 into sleep mode.
 *
 * @warning
 * The function MXQ_1065_Sleep must be supported by the underlying platform when using this function.
 *
 * @param p_buff_out MAXQ1065 Sleep context
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Administration
 */

mxq_err_t __API__ MXQ_1065_Sleep(mxq_pu1 p_buff_out);

/**
 * Restores the MAXQ1065 from sleep mode.
 *
 * @warning
 * The function MXQ_Restore must be supported by the underlying platform when using this function.
 *
 * @param p_restore_data MAXQ1065 Restore context
 * @param data_len MAXQ1065 Restore context data length
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup Administration
 */
mxq_err_t __API__ MXQ_1065_Restore(mxq_pu1 p_restore_data, mxq_length data_len);
#endif


/**
 * Activates the Crypto toolbox mode.
 *
 * This function may be implemented by the underlying platform.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 *
 * @warning This performs a reset of the MAXQ10xx.
 *
 * @post All security conditions are disabled.
 * @post All pending operations (message digest, cipher, mac) are aborted
 * @post All internal context is reset (TLS session keys, Secure channel session keys, Internally stored randoms or digests)
 *
 * @ingroup Administration
 */
mxq_err_t __API__  MXQ_TLS_Enable(void);
mxq_err_t __API__ MXQ_TLS_Direct(void);

/**
 * Restores the saved TLS session status within the MAXQ106x.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 *
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__  MXQ_TLS_Restore(void);

/**
 * Saves the current TLS session status within the MAXQ106x. TLS session is saved
 * into persistent memory.
 *
 * @return Status of execution
 * @retval	0x0000	OK
 * @retval 	MXQ_RSP_ERROR_UNKNOWN 	Unspecified error
 * @retval	MXQ_RSP_ACCESS_DENIED	Security Condition not Satisfied
 *
 * @pre MXQ_Module_Init() must have been called before.
 * @post None
 *
 * @ingroup TLS
 */
mxq_err_t __API__  MXQ_TLS_Save(void);


/**
 * Displays the mxq_status_t structure content in a human friendly way.
 * @param s Status structure to be displayed
 * @ingroup Utility
 */
void __API__ MXQ_DisplayStatus(mxq_status_t* s);

/**
 * Displays the mxq_config_t structure content in a human friendly way.
 * @param c Config structure to be displayed
 * @ingroup Utility
 *
 */
void __API__ MXQ_DisplayConfig(mxq_config_t* c);

/**
 * Displays the mxq_sysconfig_t structure content in a human friendly way.
 * @param c Config structure to be displayed
 * @ingroup Utility
 */
void __API__ MXQ_DisplaySysConfig(mxq_sysconfig_t* c);

#ifdef MXQ_FW_UPDATE_CAP
/**
 * Start and update MXQ Firmware Update.
 *
 * @param fw_img Firmware image data
 * @param fw_len Firmware image data length
 * @param keyID Update capable public key ID, valid for MAXQ18x
 * @param signature Update header signature, signed by customer, valid for MAXQ18x
 * @param algo Signature algorithm, valid for MAXQ18x
 * @param keyparam Curve parameters, valid for MAXQ18x
 *
 * @return Status of execution
 * @retval  0x0000	OK
 * @retval  MXQ_RSP_WRONG_PARAM     Null firmware or signature
 * @retval  MXQ_ERR_BAD_LENGTH      Wrong or mismatching firmware length
 * @retval  MXQ_RSP_ERROR_UNKNOWN   Unspecified error
 *
 * @warning This performs a reset of the MAXQ10xx.
 *
 * @post All security conditions are disabled.
 * @post All pending operations (message digest, cipher, mac) are aborted
 * @post All internal context is reset (TLS session keys, Secure channel session keys, Internally stored randoms or digests)
 *
 */
#if defined(MAXQ108x)
mxq_err_t __API__ MAXQ_FW_Update(mxq_pu1 fw_img, mxq_length fw_len, mxq_u2 key_ID, const mxq_pu1 signature, mxq_algo_id_t algo, mxq_keyparam_id_t keyparam);
#elif defined(MAXQ1065)
mxq_err_t __API__ MAXQ_FW_Update(mxq_pu1 fw_img, mxq_length fw_len);
#endif
#endif

#if defined(MAXQ106x)
/**
 * Delays usec
 * @param duration_usec Duration to wait in terms of usec
 * @ingroup Utility
 *
 */
void HOST_wait_usec(mxq_u4 duration_usec);
#endif

char* __API__ MXQ_get_str_code(int error_code);

void start_time(void);
void stop_time(void);

#ifdef __cplusplus
}
#endif

#endif /*  __MXQ_API_H_INCLUDED__ */

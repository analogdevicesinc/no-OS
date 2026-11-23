/*! ****************************************************************************
 *
 * @file:    device_profile_pfir_common_t.h
 *
 * @brief:   PFIR driver - common exported types and definitions
 *
 * @details: Types and defines used by the Rx, Tx PFIR functions
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_PFIR_COMMON_T_H__
#define __DEVICE_PROFILE_PFIR_COMMON_T_H__

#include <stdint.h>
#include "adi_device_profile_pack.h"

/**
 *  @addtogroup pfir
 *  @ingroup drivers
 *  @{
 */


/*! PFIR coefficients symmetric settings */
typedef enum {
    PFIR_COEF_NON_SYMMETRIC = 0u,           /*!< Coefficients are expected to be non-symmetric */
    PFIR_COEF_SYMMETRIC  = 1u,              /*!< Coefficients are expected to be symmetric */
} ADI_NEVIS_PACK_ENUM pfirSymmetric_e;

/*! PFIR tap number select */
typedef enum {
    PFIR_32_TAPS     = 0u,                  /*!<  32 taps PFIR */
    PFIR_64_TAPS     = 1u,                  /*!<  64 taps PFIR */
    PFIR_96_TAPS     = 2u,                  /*!<  96 taps PFIR */
    PFIR_128_TAPS    = 3u,                  /*!< 128 taps PFIR */
    PFIR_32_NB_TAPS  = 4u,                  /*!<  32 NB taps PFIR */
    PFIR_64_NB_TAPS  = 5u,                  /*!<  64 NB taps PFIR */
    PFIR_96_NB_TAPS  = 6u,                  /*!<  96 NB taps PFIR */
    PFIR_128_NB_TAPS = 7u,                  /*!< 128 NB taps PFIR */
    PFIR_TAPS_MAX_ID = 7u,    /*!< PFIR taps max ID */
    PFIR_TAPS_SELECT_MAX = 8u
} ADI_NEVIS_PACK_ENUM pfirNumTaps_e;

/*! PFIR gain settings */
typedef enum {
    PFIR_GAIN_MINUS_12DB  = 0u,             /*!< -12dB */
    PFIR_GAIN_MINUS_6DB   = 1u,             /*!< - 6dB */
    PFIR_GAIN_0DB         = 2u,             /*!<   0dB */
    PFIR_GAIN_PLUS_6DB    = 3u,             /*!< + 6dB */
    PFIR_GAIN_PLUS_9_54DB = 4u,             /*!< + 9.54dB */
    PFIR_GAIN_PLUS_12DB   = 5u,             /*!< +12dB */
    PFIR_GAIN_PLUS_14DB   = 6u,             /*!< +14dB */
    PFIR_GAIN_PLUS_20DB   = 7u,             /*!< +20dB */
    PFIR_GAIN_RX_MAX      = 7u,
    PFIR_GAIN_PLUS_24DB   = 8u,             /*!< +24dB */
    PFIR_GAIN_PLUS_26DB   = 9u,             /*!< +26dB */
    PFIR_GAIN_MAX         = 9u,
} ADI_NEVIS_PACK_ENUM pfirGain_e;

/*! PFIR bank settings */
typedef enum {
    PFIR_BANK_A = 0u,                       /*!< bank A */
    PFIR_BANK_B = 1u,                       /*!< bank B */
    PFIR_BANK_C = 2u,                       /*!< bank C */
    PFIR_BANK_D = 3u,                       /*!< bank D */
    PFIR_BANK_E = 4u,                       /*!< bank E */
    PFIR_BANK_F = 5u,                       /*!< bank F */    
} ADI_NEVIS_PACK_ENUM pfirBank_e;

/**@}*/
#endif /* __DEVICE_PROFILE_PFIR_COMMON_T_H__ */

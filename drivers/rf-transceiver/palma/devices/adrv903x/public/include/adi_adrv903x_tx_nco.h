/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**
 * \file adi_adrv903x_tx_nco.h
 * 
 * \brief   Contains ADRV903X Tx NCO data structures.
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADI_ADRV903X_TX_NCO_H__
#define __ADI_ADRV903X_TX_NCO_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"

#define ADI_ADRV903X_TX_ATTEN_PHASE_SIZE        (65u)

/**
 * \brief NCO set Tx MIX command structure
 */
ADI_ADRV903X_PACK_START
typedef struct adi_adrv903x_TxNcoMixConfig
{
    uint8_t         chanSelect;          /*!< Select the Tx channels (bit mapped) to enable/disable */
    uint8_t         enable;              /*!< 0: Disable, 1: Enable  */
    uint32_t        phase;               /*!< Phase in degrees (0 - 359) */
    int32_t         frequencyKhz;        /*!< Desired frequency in KHz */
} adi_adrv903x_TxNcoMixConfig_t;
ADI_ADRV903X_PACK_FINISH

/**
 * \brief Enumeration for DDC channel number
 */
typedef enum adi_adrv903x_DucNumber
{
    ADI_ADRV903X_DUC_BAND_0 = 0u,              /*!< DUC channel 0 */
    ADI_ADRV903X_DUC_NUM_BAND 
} adi_adrv903x_DucNumber_e;

/**
 * \brief NCO TX MIX set command response structure
 */
ADI_ADRV903X_PACK_START
typedef struct adi_adrv903x_TxNcoMixConfigReadbackResp
{
    adi_adrv903x_CpuErrorCode_t status;      /*!< CPU error status code */
    uint8_t                 chanSelect;      /*!< Tx channels (bit mapped) */
    uint8_t                 enable;          /*!< 0: Disabled, 1: Enabled */
    uint32_t                phase;           /*!< Phase in degrees (0 - 359) */
    int32_t                 frequencyKhz;    /*!< frequency in KHz */
} adi_adrv903x_TxNcoMixConfigReadbackResp_t;
ADI_ADRV903X_PACK_FINISH

typedef uint8_t adi_adrv903x_TxTestNcoSelect_t;
typedef uint8_t adi_adrv903x_TxTestNcoAtten_t;

/**
 * \brief NCO TX Test Tone NCO Select
 */
typedef enum adi_adrv903x_TxTestNcoSelect
{
    ADI_ADRV903X_TX_TEST_NCO_0  = 0x0u,  /*!<  First  NCO */
    ADI_ADRV903X_TX_TEST_NCO_1  = 0x1u,  /*!<  Second NCO */
    ADI_ADRV903X_TX_TEST_NCO_MAX_NUM = 0x2u
} adi_adrv903x_TxTestNcoSelect_e;

/**
 * \brief NCO TX Test Tone NCO Attenuation
 */
typedef enum adi_adrv903x_TxTestNcoAtten
{
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_0DB  = 0x0u,  /*!<  0 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_6DB  = 0x1u,  /*!<  6 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_12DB = 0x2u,  /*!< 12 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_18DB = 0x3u,  /*!< 18 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_24DB = 0x4u,  /*!< 24 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_30DB = 0x5u,  /*!< 30 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_36DB = 0x6u,  /*!< 36 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_42DB = 0x7u,  /*!< 42 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_48DB = 0x8u,  /*!< 48 dB */
    ADI_ADRV903X_TX_TEST_NCO_ATTEN_MAX_NUM = 0x9
} adi_adrv903x_TxTestNcoAtten_e;

/**
 * \brief NCO Tx Test set command structure
 */
ADI_ADRV903X_PACK_START
typedef struct adi_adrv903x_TxTestNcoConfig
{
    uint8_t                         chanSelect;      /*!< Select Tx channel (bit mapped) */
    uint8_t                         enable;          /*!< 0: Disable, 1: Enable */
    adi_adrv903x_TxTestNcoSelect_t  ncoSelect;       /*!< 0 or 1 */
    uint32_t                        phase;           /*!< Phase in degrees (0 - 359) */
    int32_t                         frequencyKhz;    /*!< Desired frequency in KHz */
    adi_adrv903x_TxTestNcoAtten_t   attenCtrl;       /*!< Attenuation control */
    /* TODO: Add summing control? */
} adi_adrv903x_TxTestNcoConfig_t;
ADI_ADRV903X_PACK_FINISH

/**
 * \brief NCO Tx Test set command response structure
 */
ADI_ADRV903X_PACK_START
typedef struct adi_adrv903x_TxTestNcoConfigReadbackResp
{
    adi_adrv903x_CpuErrorCode_t     status;          /*!< CPU error status code */
    uint8_t                         chanSelect;      /*!< Select Tx channel (bit mapped) */
    adi_adrv903x_TxTestNcoSelect_t  ncoSelect;       /*!< 0 or 1 */
    uint8_t                         enabled;         /*!< 0: Disabled, 1: Enabled */
    uint32_t                        phase;           /*!< Phase in degrees (0 - 359) */
    int32_t                         frequencyKhz;    /*!< Desired frequency in KHz */
    adi_adrv903x_TxTestNcoAtten_t   attenCtrl;       /*!< Attenuation control */
} adi_adrv903x_TxTestNcoConfigReadbackResp_t;
ADI_ADRV903X_PACK_FINISH


/**
*  \brief Data structure for Tx Atten Phase configuration
*/
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_TxAttenPhaseCfg
{
    uint8_t                   chanSelect;                                 /*!< Tx channel (bit mapped) */
    int16_t                   txAttenPhase[ADI_ADRV903X_TX_ATTEN_PHASE_SIZE]; /*!< Tx atten phase values */
} adi_adrv903x_TxAttenPhaseCfg_t;)

#endif /* __ADI_ADRV903X_TX_NCO_H__ */



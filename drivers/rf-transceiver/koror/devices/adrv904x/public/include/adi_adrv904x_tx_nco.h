/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adi_adrv904x_tx_nco.h
 * 
 * \brief   Contains ADRV904X Tx NCO data structures.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADI_ADRV904X_TX_NCO_H__
#define __ADI_ADRV904X_TX_NCO_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

#define ADI_ADRV904X_TX_ATTEN_PHASE_SIZE        (65u)

/**
 * \brief NCO set Tx MIX command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_TxNcoMixConfig
{
    uint8_t         chanSelect;          /*!< Select the Tx channels (bit mapped) to enable/disable */

    uint8_t         bandSelect;          /*!< Select the TX DUC band */
    uint8_t         enable;              /*!< 0: Disable, 1: Enable  */
    uint32_t        phase;               /*!< Phase in degrees (0 - 359) */
    int32_t         frequencyKhz;        /*!< Desired frequency in KHz */
} adi_adrv904x_TxNcoMixConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Enumeration for DDC channel number
 */
typedef enum adi_adrv904x_DucNumber
{
    ADI_ADRV904X_DUC_BAND_0 = 0u,              /*!< DUC channel 0 */
    ADI_ADRV904X_DUC_BAND_1,                   /*!< DUC channel 1 */
    ADI_ADRV904X_DUC_NUM_BAND 
} adi_adrv904x_DucNumber_e;

/**
 * \brief NCO TX MIX set command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_TxNcoMixConfigReadbackResp
{
    adi_adrv904x_CpuErrorCode_t status;      /*!< CPU error status code */
    uint8_t                 chanSelect;      /*!< Tx channels (bit mapped) */

    uint8_t         bandSelect;          /*!< Select the TX DUC band */
    uint8_t                 enable;          /*!< 0: Disabled, 1: Enabled */
    uint32_t                phase;           /*!< Phase in degrees (0 - 359) */
    int32_t                 frequencyKhz;    /*!< frequency in KHz */
} adi_adrv904x_TxNcoMixConfigReadbackResp_t;
ADI_ADRV904X_PACK_FINISH

typedef uint8_t adi_adrv904x_TxTestNcoSelect_t;
typedef uint8_t adi_adrv904x_TxTestNcoAtten_t;

/**
 * \brief NCO TX Test Tone NCO Select
 */
typedef enum adi_adrv904x_TxTestNcoSelect
{
    ADI_ADRV904X_TX_TEST_NCO_0  = 0x0u,  /*!<  First  NCO */
    ADI_ADRV904X_TX_TEST_NCO_1  = 0x1u,  /*!<  Second NCO */
    ADI_ADRV904X_TX_TEST_NCO_MAX_NUM = 0x2u
} adi_adrv904x_TxTestNcoSelect_e;

/**
 * \brief NCO TX Test Tone NCO Attenuation
 */
typedef enum adi_adrv904x_TxTestNcoAtten
{
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_0DB  = 0x0u,  /*!<  0 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_6DB  = 0x1u,  /*!<  6 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_12DB = 0x2u,  /*!< 12 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_18DB = 0x3u,  /*!< 18 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_24DB = 0x4u,  /*!< 24 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_30DB = 0x5u,  /*!< 30 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_36DB = 0x6u,  /*!< 36 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_42DB = 0x7u,  /*!< 42 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_48DB = 0x8u,  /*!< 48 dB */
    ADI_ADRV904X_TX_TEST_NCO_ATTEN_MAX_NUM = 0x9
} adi_adrv904x_TxTestNcoAtten_e;

/**
 * \brief NCO Tx Test set command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_TxTestNcoConfig
{
    uint8_t                         chanSelect;      /*!< Select Tx channel (bit mapped) */

    uint8_t         bandSelect;          /*!< Select the TX DUC band */
    uint8_t                         enable;          /*!< 0: Disable, 1: Enable */
    adi_adrv904x_TxTestNcoSelect_t  ncoSelect;       /*!< 0 or 1 */
    uint32_t                        phase;           /*!< Phase in degrees (0 - 359) */
    int32_t                         frequencyKhz;    /*!< Desired frequency in KHz */
    adi_adrv904x_TxTestNcoAtten_t   attenCtrl;       /*!< Attenuation control */
    /* TODO: Add summing control? */
} adi_adrv904x_TxTestNcoConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief NCO Tx Test set command response structure
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_TxTestNcoConfigReadbackResp
{
    adi_adrv904x_CpuErrorCode_t     status;          /*!< CPU error status code */

    uint8_t         bandSelect;          /*!< Select the TX DUC band */
    uint8_t                         chanSelect;      /*!< Select Tx channel (bit mapped) */
    adi_adrv904x_TxTestNcoSelect_t  ncoSelect;       /*!< 0 or 1 */
    uint8_t                         enabled;         /*!< 0: Disabled, 1: Enabled */
    uint32_t                        phase;           /*!< Phase in degrees (0 - 359) */
    int32_t                         frequencyKhz;    /*!< Desired frequency in KHz */
    adi_adrv904x_TxTestNcoAtten_t   attenCtrl;       /*!< Attenuation control */
} adi_adrv904x_TxTestNcoConfigReadbackResp_t;
ADI_ADRV904X_PACK_FINISH


/**
*  \brief Data structure for Tx Atten Phase configuration
*/
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_TxAttenPhaseCfg
{
    uint8_t                   chanSelect;                                 /*!< Tx channel (bit mapped) */
    int16_t                   txAttenPhase[ADI_ADRV904X_TX_ATTEN_PHASE_SIZE]; /*!< Tx atten phase values */
} adi_adrv904x_TxAttenPhaseCfg_t;)

/**
 * \brief NCO set Tx CDUC command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_TxCducNcoConfig
{
    uint8_t         chanSelect;          /*!< Select the Tx channels (bit mapped) to enable/disable */
    uint8_t         carrierSelect;       /*!< Select the carriers (bit mapped) */
    uint8_t         enable;              /*!< 0: Disable, 1: Enable  */
    uint32_t        phase;               /*!< Phase in degrees (0 - 359) */
    int32_t         frequencyKhz;        /*!< Desired frequency in KHz */
} adi_adrv904x_TxCducNcoConfig_t;)

/**
 * \brief NCO TX MIX set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_TxCducNcoConfigReadbackResp
{
    adi_adrv904x_CpuErrorCode_t status;          /*!< CPU error status code */
    uint8_t                     chanSelect;      /*!< Tx channels (bit mapped) */
    uint8_t                     carrierSelect;   /*!< carriers (bit mapped) */
    uint8_t                     enable;          /*!< 0: Disabled, 1: Enabled */
    uint32_t                    phase;           /*!< Phase in degrees (0 - 359) */
    int32_t                     frequencyKhz;    /*!< frequency in KHz */
} adi_adrv904x_TxCducNcoConfigReadbackResp_t;)

#endif /* __ADI_ADRV904X_TX_NCO_H__ */



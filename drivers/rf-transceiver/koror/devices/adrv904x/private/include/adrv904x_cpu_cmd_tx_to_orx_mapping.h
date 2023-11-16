/**
 * \file adrv904x_cpu_cmd_tx_to_orx_mapping.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_ATTEN,
 *                                 ADRV904X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_NCO
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_ATTEN,
 *                                 ADRV904X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_NCO
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_TX_TO_ORX_MAPPING_H__
#define __ADRV904X_CPU_CMD_TX_TO_ORX_MAPPING_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/* Flag to indicate the extended mapping is used */
#define TX_TO_ORX_EXTENDED_MAPPING_FLAG (0x0000100UL)

/**
 * \brief SET_TX_TO_ORX_PRESET_ATTEN command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetTxToOrxPresetAtten
{
    uint8_t         chanSelect;             /*!< Select the Tx channels (bit mapped) */
    uint8_t         extendedMappingFlag;    /*!< Extended Mapping Flag */
    uint8_t         presetAtten_dB;         /*!< Orx Atten Preset value (0-16) */
    uint8_t         immediateUpdate;        /*!< Enable/disable flag to update Orx Atten ctrl regs for Orx channels that can observe selected Tx channels */
} adrv904x_CpuCmd_SetTxToOrxPresetAtten_t;)

/**
 * \brief SET_TX_TO_ORX_PRESET_ATTEN command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetTxToOrxPresetAttenResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_SetTxToOrxPresetAttenResp_t;)



/**
 * \brief SET_TX_TO_ORX_PRESET_NCO command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetTxToOrxPresetNco
{
    uint8_t     chanSelect;             /*!< Select the Tx channels (bit mapped) */
    uint8_t     extendedMappingFlag;    /*!< Extended Mapping Flag */
    uint32_t    ncoFreqAdc_Khz;         /*!< Orx ADC NCO Frequency (kHz) Preset value */
    uint32_t    ncoFreqDatapath_Khz;    /*!< Orx Datapath NCO Frequency (kHz) Preset value */
    uint8_t     immediateUpdate;        /*!< Enable/disable flag to update Orx NCO ctrl regs for Orx channels that can observe selected Tx channels */
} adrv904x_CpuCmd_SetTxToOrxPresetNco_t;)

/**
 * \brief SET_TX_TO_ORX_PRESET_NCO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetTxToOrxPresetNcoResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_SetTxToOrxPresetNcoResp_t;)


#endif /* __ADRV904X_CPU_CMD_TX_TO_ORX_MAPPING_H__ */


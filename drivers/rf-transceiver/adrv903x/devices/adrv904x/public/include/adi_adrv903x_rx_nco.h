/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**
 * \file adi_adrv903x_rx_nco.h
 * 
 * \brief   Contains ADRV903X Rx NCO data structures.
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADI_ADRV903X_RX_NCO_H__
#define __ADI_ADRV903X_RX_NCO_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"

typedef uint8_t adi_adrv903x_DdcNumber_t;
typedef uint8_t adi_adrv903x_OrxNcoSelect_t;

/**
 * \brief Enumeration for DDC channel number
 */
typedef enum adi_adrv903x_DdcNumber
{
    ADI_ADRV903X_DDC_BAND_0       = 0u,        /*!< DDC channel 0 */
    ADI_ADRV903X_DDC_BAND_1,                   /*!< DDC channel 1 */  
    ADI_ADRV903X_DDC_NUM_BAND     
} adi_adrv903x_DdcNumber_e;

/**
 * \brief Enumeration for ORx NCO selections
 */
typedef enum adi_adrv903x_ORxNcoSelect
{
    ADI_ADRV903X_ORX_NCO_DDC       = 0u,        /*!< DDC NCO selected */
    ADI_ADRV903X_ORX_DATAPATH,                  /*!< DP NCO selected  */  
    ADI_ADRV903X_ORX_NUM_NCO,                   /*!< Number of ORX NCOs */
} adi_adrv903x_OrxNcoSelect_e;

/**
 * \brief NCO Rx set command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_RxNcoConfig
{
    uint8_t                   chanSelect;             /*!< Rx channel (bit mapped) */
    adi_adrv903x_DdcNumber_t  bandSelect;             /*!< which band */
    uint8_t                   enable;                 /*!< 0: Disable, 1: Enable */
    uint32_t                  phase;                  /*!< Phase in degrees (0 - 359) */
    int32_t                   frequencyKhz;           /*!< Frequency in KHz */
} adi_adrv903x_RxNcoConfig_t;)

/**
 * \brief NCO Rx set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_RxNcoConfigReadbackResp
{
    adi_adrv903x_CpuErrorCode_t status;              /*!< CPU error status code */
    uint8_t                     chanSelect;          /*!< Rx channel (bit mapped) */
    adi_adrv903x_DdcNumber_t    bandSelect;          /*!< which band */
    uint8_t                     enabled;             /*!< 0: Disabled, 1: Enabled */
    uint32_t                    phase;               /*!< Phase in degrees (0 - 359) */
    int32_t                     frequencyKhz;        /*!< Frequency in KHz */
} adi_adrv903x_RxNcoConfigReadbackResp_t;)

/**
 * \brief NCO ORx set command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_ORxNcoConfig
{
    uint8_t                     chanSelect;          /*!< Select ORx channel (bit mapped) */
    uint8_t                     ncoSelect;           /*!< NCO select; 0 ADC, 1 Datapath */
    uint8_t                     enable;              /*!< 0: Disable, 1: Enable */
    uint32_t                    phase;               /*!< Phase in degrees (0 - 359) */
    int32_t                     frequencyKhz;        /*!< Desired frequency in KHz */
} adi_adrv903x_ORxNcoConfig_t;)

/**
 * \brief NCO Orx set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_ORxNcoConfigReadbackResp
{
    adi_adrv903x_CpuErrorCode_t status;              /*!< CPU error status code */
    uint8_t                     chanSelect;          /*!< Select ORx channel (bit mapped) */
    uint8_t                     ncoSelect;           /*!< NCO select; 0 ADC, 1 Datapath */
    uint8_t                     enabled;             /*!< 0: Disabled, 1: Enabled */
    uint32_t                    phase;               /*!< Phase in degrees (0 - 359) */
    int32_t                     frequencyKhz;        /*!< Desired frequency in KHz */
} adi_adrv903x_ORxNcoConfigReadbackResp_t;)

/**
 * \brief Rx Spur Configuration Set structure 
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_RxSpurFreqSet
{
    uint32_t                  rxChannelMask; /*!< Rx Channels for which gain index needs to be updated. Bits 0-7 correspond to Rx0-Rx7 respectively */
    int32_t                   baseBandFreqKHz;      /*!< Baseband freq at which Rx SPUR freq needs to be cancelled */
    uint8_t                   enable;               /*!< 0: Disable, 1: Enable */
} adi_adrv903x_RxSpurFreqSet_t;)

/**
 * \brief Rx Spur Configuration Get Structure 
 */
typedef struct adi_adrv903x_RxSpurFreqGet
{
    uint32_t                 rxChannelMask; /*!< Rx Channels for which gain index needs to be updated. Bits 0-7 correspond to Rx0-Rx7 respectively */
} adi_adrv903x_RxSpurFreqGet_t;

/**
 * \brief Rx Spur Configuration response structure 
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_RxSpurFreqConfigResp
{
    adi_adrv903x_CpuErrorCode_t status;                         /*!< Rx Spur Frequency Config response status */
    uint32_t                    rxChannelMask;                  /*!< Rx Channel for which result is returned */
    int32_t                     baseBandFreqKHz;                /*!< Baseband freq at which Rx SPUR freq needs to be cancelled */
    uint8_t                     enabled;                        /*!< 0: Disabled, 1: Enabled */
} adi_adrv903x_RxSpurFreqConfigResp_t;)

#endif /* __ADI_ADRV903X_RX_NCO_H__ */


/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adi_adrv904x_rx_nco.h
 * 
 * \brief   Contains ADRV904X Rx NCO data structures.
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_RX_NCO_H__
#define __ADI_ADRV904X_RX_NCO_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

typedef uint8_t adi_adrv904x_DdcNumber_t;
typedef uint8_t adi_adrv904x_OrxNcoSelect_t;

/**
 * \brief Enumeration for DDC channel number
 */
typedef enum adi_adrv904x_DdcNumber
{
    ADI_ADRV904X_DDC_BAND_0       = 0u,        /*!< DDC channel 0 */
    ADI_ADRV904X_DDC_BAND_1,                   /*!< DDC channel 1 */  
    ADI_ADRV904X_DDC_NUM_BAND     
} adi_adrv904x_DdcNumber_e;

/**
 * \brief Enumeration for ORx NCO selections
 */
typedef enum adi_adrv904x_ORxNcoSelect
{
    ADI_ADRV904X_ORX_NCO_DDC       = 0u,        /*!< DDC NCO selected */
    ADI_ADRV904X_ORX_DATAPATH,                  /*!< DP NCO selected  */  
    ADI_ADRV904X_ORX_NUM_NCO,                   /*!< Number of ORX NCOs */
} adi_adrv904x_OrxNcoSelect_e;

/**
 * \brief NCO Rx set command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_RxNcoConfig
{
    uint8_t                   chanSelect;             /*!< Rx channel (bit mapped) */
    adi_adrv904x_DdcNumber_t  bandSelect;             /*!< which band */
    uint8_t                   enable;                 /*!< 0: Disable, 1: Enable */
    uint32_t                  phase;                  /*!< Phase in degrees (0 - 359) */
    int32_t                   frequencyKhz;           /*!< Frequency in KHz */
} adi_adrv904x_RxNcoConfig_t;)

/**
 * \brief NCO Rx set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_RxNcoConfigReadbackResp
{
    adi_adrv904x_CpuErrorCode_t status;              /*!< CPU error status code */
    uint8_t                     chanSelect;          /*!< Rx channel (bit mapped) */
    adi_adrv904x_DdcNumber_t    bandSelect;          /*!< which band */
    uint8_t                     enabled;             /*!< 0: Disabled, 1: Enabled */
    uint32_t                    phase;               /*!< Phase in degrees (0 - 359) */
    int32_t                     frequencyKhz;        /*!< Frequency in KHz */
} adi_adrv904x_RxNcoConfigReadbackResp_t;)

/**
 * \brief NCO ORx set command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_ORxNcoConfig
{
    uint8_t                     chanSelect;          /*!< Select ORx channel (bit mapped) */
    uint8_t                     ncoSelect;           /*!< NCO select; 0 ADC, 1 Datapath */
    uint8_t                     enable;              /*!< 0: Disable, 1: Enable */
    uint32_t                    phase;               /*!< Phase in degrees (0 - 359) */
    int32_t                     frequencyKhz;        /*!< Desired frequency in KHz */
} adi_adrv904x_ORxNcoConfig_t;)

/**
 * \brief NCO Orx set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_ORxNcoConfigReadbackResp
{
    adi_adrv904x_CpuErrorCode_t status;              /*!< CPU error status code */
    uint8_t                     chanSelect;          /*!< Select ORx channel (bit mapped) */
    uint8_t                     ncoSelect;           /*!< NCO select; 0 ADC, 1 Datapath */
    uint8_t                     enabled;             /*!< 0: Disabled, 1: Enabled */
    uint32_t                    phase;               /*!< Phase in degrees (0 - 359) */
    int32_t                     frequencyKhz;        /*!< Desired frequency in KHz */
} adi_adrv904x_ORxNcoConfigReadbackResp_t;)

/**
 * \brief NCO CDDC Rx set command structure (Koror only)
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_RxCddcNcoConfig
{
    uint8_t                   chanSelect;             /*!< Rx channels (bit mapped) */
    uint8_t                   carrierSelect;          /*!< carriers    (bit mapped) */
    uint8_t                   enable;                 /*!< 0: Disable, 1: Enable */
    uint32_t                  phase;                  /*!< Phase in degrees (0 - 359) */
    int32_t                   frequencyKhz;           /*!< Frequency in KHz */
} adi_adrv904x_RxCddcNcoConfig_t;)

/**
 * \brief NCO CDDC Rx set command response structure (Koror only)
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_RxCddcNcoConfigReadbackResp
{
    adi_adrv904x_CpuErrorCode_t   status;             /*!< CPU error status code */
    uint8_t                       chanSelect;         /*!< Rx channel (bit mapped) */
    uint8_t                       carrierSelect;      /*!< carrier    (bit mapped) */
    uint8_t                       enabled;            /*!< 0: Disabled, 1: Enabled */
    uint32_t                      phase;              /*!< Phase in degrees (0 - 359) */
    int32_t                       frequencyKhz;       /*!< Frequency in KHz */
} adi_adrv904x_RxCddcNcoConfigReadbackResp_t;)

#endif /* __ADI_ADRV904X_RX_NCO_H__ */


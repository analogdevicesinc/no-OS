/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adi_adrv904x_cpu_cmd_dc_offset.h
 * 
 * \brief   Contains ADRV904X Rx DC Offset data structures.
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_CPU_CMD_DC_OFFSET_H__
#define __ADI_ADRV904X_CPU_CMD_DC_OFFSET_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

/**
 * \brief DC Offset get command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuCmd_GetDcOffset
{
    uint8_t                   chanSelect;             /*!< Rx channel number */
} adi_adrv904x_CpuCmd_GetDcOffset_t;)


/**
 * \brief lofreq set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuCmd_GetDcOffsetResp
{
    adi_adrv904x_CpuErrorCode_t status;       /*!< CPU error status code */
    uint8_t                     chanSelect;   /*!< Rx channel number */    
    uint32_t                    mShift;       /*!< mShift setting */
    uint8_t                     multEnb;      /*!< Multiplier Enabled */	
    uint32_t                    mult;         /*!< Multiplier value */    
} adi_adrv904x_CpuCmd_GetDcOffsetResp_t;)


/**
 * \brief DC Offset Rx set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuCmd_SetDcOffset
{
    uint8_t                   chanSelect;             /*!< Rx channel number */
    uint32_t                  dcFilterBw_kHz;         /*!< DC offset 1db filter corner */
    uint8_t                   multEnb;                /*!< Multiplier Enabled */	
} adi_adrv904x_CpuCmd_SetDcOffset_t;)


/**
 * \brief lofreq set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuCmd_SetDcOffsetResp
{
    adi_adrv904x_CpuErrorCode_t status;  /*!< CPU error status code */
} adi_adrv904x_CpuCmd_SetDcOffsetResp_t;)



#endif /* __ADI_ADRV904X_CPU_CMD_DC_OFFSET_H__ */



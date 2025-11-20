/**
 * \file adrv904x_cpu_cmd_ram.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_RAM_ACCESS_START,
 *                                 ADRV904X_CPU_CMD_ID_RAM_ACCESS_STOP
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_RAM_ACCESS_START,
 *                                 ADRV904X_CPU_CMD_ID_RAM_ACCESS_STOP
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_RAM_H__
#define __ADRV904X_CPU_CMD_RAM_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 * \brief Capture RAM type enumeration
 */
typedef enum adrv904x_CpuCmd_CaptureRamType
{
	ADRV904X_CPU_CMD_CAP_RAM_TYPE_DPD,  /*!< DPD capture RAM */
	ADRV904X_CPU_CMD_CAP_RAM_TYPE_ORX   /*!< ORx capture RAM */
} adrv904x_CpuCmd_CaptureRamType_e;
typedef uint8_t adrv904x_CpuCmd_CaptureRamType_t;

/**
 * \brief RAM_ACCESS_START command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RamAccessStart
{
    adrv904x_CpuCmd_CaptureRamType_t captureRamType;  /*!< Type of the capture RAM to lock */
    uint32_t channelNumber;                           /*!< Channel number of the capture RAM to lock */
} adrv904x_CpuCmd_RamAccessStart_t;)

/**
 * \brief RAM_ACCESS_START command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RamAccessStartResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_RamAccessStartResp_t;)

/**
 * \brief RAM_ACCESS_STOP command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RamAccessStop
{
    adrv904x_CpuCmd_CaptureRamType_t captureRamType;  /*!< Type of the capture RAM to unlock */
    uint32_t channelNumber;                           /*!< Channel number of the capture RAM to unlock */
} adrv904x_CpuCmd_RamAccessStop_t;)

/**
 * \brief RAM_ACCESS_STOP command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RamAccessStopResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_RamAccessStopResp_t;)

#endif /* __ADRV904X_CPU_CMD_RAM_H__ */


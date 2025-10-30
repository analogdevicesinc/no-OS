/**
 * \file adrv903x_cpu_cmd_ram.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_RAM_ACCESS_START,
 *                                 ADRV903X_CPU_CMD_ID_RAM_ACCESS_STOP
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_RAM_ACCESS_START,
 *                                 ADRV903X_CPU_CMD_ID_RAM_ACCESS_STOP
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_RAM_H__
#define __ADRV903X_CPU_CMD_RAM_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"

/**
 * \brief Capture RAM type enumeration
 */
typedef enum adrv903x_CpuCmd_CaptureRamType
{
	ADRV903X_CPU_CMD_CAP_RAM_TYPE_DPD,  /*!< DPD capture RAM */
	ADRV903X_CPU_CMD_CAP_RAM_TYPE_ORX   /*!< ORx capture RAM */
} adrv903x_CpuCmd_CaptureRamType_e;
typedef uint8_t adrv903x_CpuCmd_CaptureRamType_t;

/**
 * \brief RAM_ACCESS_START command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RamAccessStart
{
    adrv903x_CpuCmd_CaptureRamType_t captureRamType;  /*!< Type of the capture RAM to lock */
    uint32_t channelNumber;                           /*!< Channel number of the capture RAM to lock */
} adrv903x_CpuCmd_RamAccessStart_t;)

/**
 * \brief RAM_ACCESS_START command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RamAccessStartResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv903x_CpuCmd_RamAccessStartResp_t;)

/**
 * \brief RAM_ACCESS_STOP command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RamAccessStop
{
    adrv903x_CpuCmd_CaptureRamType_t captureRamType;  /*!< Type of the capture RAM to unlock */
    uint32_t channelNumber;                           /*!< Channel number of the capture RAM to unlock */
} adrv903x_CpuCmd_RamAccessStop_t;)

/**
 * \brief RAM_ACCESS_STOP command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RamAccessStopResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv903x_CpuCmd_RamAccessStopResp_t;)

#endif /* __ADRV903X_CPU_CMD_RAM_H__ */


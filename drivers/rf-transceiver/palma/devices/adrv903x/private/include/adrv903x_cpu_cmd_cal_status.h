/**
 * \file adrv903x_cpu_cmd_cal_status.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_GET_CAL_STATUS
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_CAL_STATUS_H__
#define __ADRV903X_CPU_CMD_CAL_STATUS_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_object_ids_types.h"
#include "adi_adrv903x_cals_types.h"

/**
 * \brief Calibration status type enumeration
 */
typedef enum adrv903x_CpuCmd_CalStatusType
{
	ADRV903X_CPU_CMD_CAL_STATUS_COMMON,    /*!< Common calibration status */
	ADRV903X_CPU_CMD_CAL_STATUS_SPECIFIC,  /*!< Calibration-specific status */
	ADRV903X_CPU_CMD_CAL_STATUS_PRIVATE    /*!< Private calibration-specific status */
} adrv903x_CpuCmd_CalStatusType_e;
typedef uint8_t adrv903x_CpuCmd_CalStatusType_t;


/**
 * \brief GET_CAL_STATUS command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetCalStatus
{
	adrv903x_CpuCmd_CalStatusType_t type;  /*!< Calibration status type to be retrieved */
	adrv903x_CpuObjectId_t calObjId;       /*!< Object ID of calibration */
	uint32_t channelNum;                   /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
} adrv903x_CpuCmd_GetCalStatus_t;)


/**
 * \brief GET_CAL_STATUS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetCalStatusResp
{
	adrv903x_CpuErrorCode_e cmdStatus;  /*!< Command status */

    /* Cal status payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* calStatus[];
     */
} adrv903x_CpuCmd_GetCalStatusResp_t;)


/**
 * \brief Calibration status payload union.
 *        Used to determine the maximum cal status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef union adrgven6_CalStatusMaxSize
{
    adi_adrv903x_CalStatus_t calStatus;
} adrgven6_CalStatusMaxSize_t;


/**
 * \brief Status payload size struct.
 *        Used to determine the maximum cal status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_GetCalStatusMaxSize
{
    adrv903x_CpuCmd_GetCalStatusResp_t getCalStatusCmdResp;
    adrgven6_CalStatusMaxSize_t        calStatusMaxSize;
} adrv903x_CpuCmd_GetCalStatusMaxSize_t;

#endif /* __ADRV903X_CPU_CMD_CAL_STATUS_H__ */


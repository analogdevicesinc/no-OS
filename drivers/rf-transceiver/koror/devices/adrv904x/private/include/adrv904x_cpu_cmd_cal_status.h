/**
 * \file adrv904x_cpu_cmd_cal_status.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_GET_CAL_STATUS
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_CAL_STATUS_H__
#define __ADRV904X_CPU_CMD_CAL_STATUS_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"
#include "adrv904x_cpu_object_ids_types.h"
#include "adi_adrv904x_cals_types.h"

/**
 * \brief Calibration status type enumeration
 */
typedef enum adrv904x_CpuCmd_CalStatusType
{
	ADRV904X_CPU_CMD_CAL_STATUS_COMMON,    /*!< Common calibration status */
	ADRV904X_CPU_CMD_CAL_STATUS_SPECIFIC,  /*!< Calibration-specific status */
	ADRV904X_CPU_CMD_CAL_STATUS_PRIVATE    /*!< Private calibration-specific status */
} adrv904x_CpuCmd_CalStatusType_e;
typedef uint8_t adrv904x_CpuCmd_CalStatusType_t;


/**
 * \brief GET_CAL_STATUS command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetCalStatus
{
	adrv904x_CpuCmd_CalStatusType_t type;  /*!< Calibration status type to be retrieved */
	adrv904x_CpuObjectId_t calObjId;       /*!< Object ID of calibration */
	uint32_t channelNum;                   /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
} adrv904x_CpuCmd_GetCalStatus_t;)


/**
 * \brief GET_CAL_STATUS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetCalStatusResp
{
	adrv904x_CpuErrorCode_e cmdStatus;  /*!< Command status */

    /* Cal status payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* calStatus[];
     */
} adrv904x_CpuCmd_GetCalStatusResp_t;)


/**
 * \brief Calibration status payload union.
 *        Used to determine the maximum cal status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef union adrgven6_CalStatusMaxSize
{
    adi_adrv904x_CalStatus_t calStatus;
} adrgven6_CalStatusMaxSize_t;


/**
 * \brief Status payload size struct.
 *        Used to determine the maximum cal status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv904x_CpuCmd_GetCalStatusMaxSize
{
    adrv904x_CpuCmd_GetCalStatusResp_t getCalStatusCmdResp;
    adrgven6_CalStatusMaxSize_t        calStatusMaxSize;
} adrv904x_CpuCmd_GetCalStatusMaxSize_t;

#endif /* __ADRV904X_CPU_CMD_CAL_STATUS_H__ */


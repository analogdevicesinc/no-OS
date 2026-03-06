/**
 * \file adrv903x_cpu_cmd_sys_status.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_GET_SYS_STATUS
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_SYS_STATUS_H__
#define __ADRV903X_CPU_CMD_SYS_STATUS_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_object_ids_types.h"

/**
 * \brief System status type enumeration
 */
typedef enum adrv903x_CpuCmd_SysStatusType
{
	ADRV903X_CPU_CMD_SYS_STATUS_PUBLIC,    /*!< Public System status */
	ADRV903X_CPU_CMD_SYS_STATUS_PRIVATE    /*!< Private System status */
} adrv903x_CpuCmd_SysStatusType_e;
typedef uint8_t adrv903x_CpuCmd_SysStatusType_t;


/**
 * \brief GET_SYS_STATUS command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetSysStatus
{
	adrv903x_CpuCmd_SysStatusType_t type;  /*!< System status type to be retrieved */
	adrv903x_CpuObjectId_t sysObjId;       /*!< Object ID of system component */
	uint32_t channelNum;                   /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
} adrv903x_CpuCmd_GetSysStatus_t;)


/**
 * \brief GET_SYS_STATUS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetSysStatusResp
{
	adi_adrv903x_CpuErrorCode_t cmdStatus;  /*!< Command status */
    /* System status payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* sysStatus[];
     */
} adrv903x_CpuCmd_GetSysStatusResp_t;)

/**
 * \brief System status payload union.
 *        Used to determine the maximum system status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef union adrgven6_SysStatusMaxSize
{
    uint8_t sysStatusMax[256];  /*!< Sys response */ 
} adrgven6_SysStatusMaxSize_t;

/**
 * \brief Status payload size struct.
 *        Used to determine the maximum cal status payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_GetSysStatusMaxSize
{
    adrv903x_CpuCmd_GetSysStatusResp_t getSysStatusCmdResp;
    adrgven6_SysStatusMaxSize_t        sysStatusMaxSize;
} adrv903x_CpuCmd_GetSysStatusMaxSize_t;

#endif /* __ADRV903X_CPU_CMD_SYS_STATUS_H__ */


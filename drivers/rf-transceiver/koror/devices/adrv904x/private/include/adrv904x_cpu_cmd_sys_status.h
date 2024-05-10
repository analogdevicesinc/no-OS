/**
 * \file adrv904x_cpu_cmd_sys_status.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_GET_SYS_STATUS
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_SYS_STATUS_H__
#define __ADRV904X_CPU_CMD_SYS_STATUS_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"
#include "adrv904x_cpu_object_ids_types.h"

/**
 * \brief System status type enumeration
 */
typedef enum adrv904x_CpuCmd_SysStatusType
{
	ADRV904X_CPU_CMD_SYS_STATUS_PUBLIC,    /*!< Public System status */
	ADRV904X_CPU_CMD_SYS_STATUS_PRIVATE    /*!< Private System status */
} adrv904x_CpuCmd_SysStatusType_e;
typedef uint8_t adrv904x_CpuCmd_SysStatusType_t;


/**
 * \brief GET_SYS_STATUS command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetSysStatus
{
	adrv904x_CpuCmd_SysStatusType_t type;  /*!< System status type to be retrieved */
	adrv904x_CpuObjectId_t sysObjId;       /*!< Object ID of system component */
	uint32_t channelNum;                   /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
} adrv904x_CpuCmd_GetSysStatus_t;)


/**
 * \brief GET_SYS_STATUS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetSysStatusResp
{
	adi_adrv904x_CpuErrorCode_t cmdStatus;  /*!< Command status */
    /* System status payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* sysStatus[];
     */
} adrv904x_CpuCmd_GetSysStatusResp_t;)

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
typedef struct adrv904x_CpuCmd_GetSysStatusMaxSize
{
    adrv904x_CpuCmd_GetSysStatusResp_t getSysStatusCmdResp;
    adrgven6_SysStatusMaxSize_t        sysStatusMaxSize;
} adrv904x_CpuCmd_GetSysStatusMaxSize_t;

#endif /* __ADRV904X_CPU_CMD_SYS_STATUS_H__ */


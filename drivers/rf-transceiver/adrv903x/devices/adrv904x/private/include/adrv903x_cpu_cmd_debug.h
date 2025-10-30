/**
 * \file adrv903x_cpu_cmd_debug.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_DEBUG
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_DEBUG_H__
#define __ADRV903X_CPU_CMD_DEBUG_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_object_ids_types.h"
#include "adrv903x_cpu_debug_types.h"

/**
 * \brief Maximum debug payload size
 */
#define MAX_DEBUG_DATA_SIZE    (256u)

/**
 * \brief Maximum debug response size
 */
#define MAX_DEBUG_RESP_SIZE    (256u)

/**
 * \brief DEBUG command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_Debug
{
	adrv903x_CpuObjectId_t objId;   /*!< Object ID of the cal or system component */
	uint16_t debugCmd;              /*!< Command to be executed */
	uint32_t channelNum;            /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
	uint16_t length;                /*!< Length of the debug data payload in bytes */

	/* Debug data payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t dbgData[];
     */
} adrv903x_CpuCmd_Debug_t;)


/**
 * \brief DEBUG command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_DebugResp
{
	adrv903x_CpuErrorCode_e cmdStatus;  /*!< Command status */
	uint16_t length;					/*!< Length of the response data in bytes */

	/* Debug data response payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t respData[];
     */
} adrv903x_CpuCmd_DebugResp_t;)

/**
 * \brief Debug payload size struct.
 *        Used to determine the maximum debug payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_DebugMaxSize
{
    adrv903x_CpuCmd_Debug_t  setDebugCmd;
    uint8_t                  debugData[MAX_DEBUG_DATA_SIZE];   /*!< Debug data */ 
} adrv903x_CpuCmd_DebugMaxSize_t;

/**
 * \brief Debug payload size struct.
 *        Used to determine the maximum debug payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_DebugRespMaxSize
{
    adrv903x_CpuCmd_DebugResp_t  setDebugCmdResp;
    uint8_t                      debugResp[MAX_DEBUG_RESP_SIZE];   /*!< Debug response */ 
} adrv903x_CpuCmd_DebugRespMaxSize_t;


#endif /* __ADRV903X_CPU_CMD_DEBUG_H__ */


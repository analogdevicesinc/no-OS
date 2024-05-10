/**
 * \file adrv904x_cpu_cmd_debug.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_DEBUG
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_DEBUG_H__
#define __ADRV904X_CPU_CMD_DEBUG_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"
#include "adrv904x_cpu_object_ids_types.h"
#include "adrv904x_cpu_debug_types.h"

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
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_Debug
{
	adrv904x_CpuObjectId_t objId;   /*!< Object ID of the cal or system component */
	uint16_t debugCmd;              /*!< Command to be executed */
	uint32_t channelNum;            /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
	uint16_t length;                /*!< Length of the debug data payload in bytes */

	/* Debug data payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t dbgData[];
     */
} adrv904x_CpuCmd_Debug_t;)


/**
 * \brief DEBUG command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_DebugResp
{
	adrv904x_CpuErrorCode_e cmdStatus;  /*!< Command status */
	uint16_t length;					/*!< Length of the response data in bytes */

	/* Debug data response payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t respData[];
     */
} adrv904x_CpuCmd_DebugResp_t;)

/**
 * \brief Debug payload size struct.
 *        Used to determine the maximum debug payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv904x_CpuCmd_DebugMaxSize
{
    adrv904x_CpuCmd_Debug_t  setDebugCmd;
    uint8_t                  debugData[MAX_DEBUG_DATA_SIZE];   /*!< Debug data */ 
} adrv904x_CpuCmd_DebugMaxSize_t;

/**
 * \brief Debug payload size struct.
 *        Used to determine the maximum debug payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv904x_CpuCmd_DebugRespMaxSize
{
    adrv904x_CpuCmd_DebugResp_t  setDebugCmdResp;
    uint8_t                      debugResp[MAX_DEBUG_RESP_SIZE];   /*!< Debug response */ 
} adrv904x_CpuCmd_DebugRespMaxSize_t;


#endif /* __ADRV904X_CPU_CMD_DEBUG_H__ */


/**
 * \file adrv903x_cpu_cmd_config.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_SET_CONFIG 
 *          and ADRV903X_CPU_CMD_ID_GET_CONFIG
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_CONFIG_H__
#define __ADRV903X_CPU_CMD_CONFIG_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_object_ids_types.h"

#ifndef ADI_ADRV903X_FW
#include "adi_adrv903x_user.h"
#endif

/**
 * \brief Maximum configuration payload size
 */
#define MAX_CONFIG_DATA_SIZE    (256u)

   
/**
 * \brief UNLOCK_CONFIG command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_UnlockConfig
{
    uint32_t configKey;
} adrv903x_CpuCmd_UnlockConfig_t;)


/**
 * \brief UNLOCK_CONFIG command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_UnlockConfigResp
{
	adrv903x_CpuErrorCode_e cmdStatus;      /*!< Command status */
} adrv903x_CpuCmd_UnlockConfigResp_t;) 

   
/**
 * \brief SET_CONFIG command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetConfig
{
	adrv903x_CpuObjectId_t objId;          /*!< Object ID of calibration or system component */
	uint16_t               offset;         /*!< Offset into the configuration structure */
    uint16_t               length;         /*!< Length of the configuration in bytes */

    /* Config data payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t configData[];
     */
} adrv903x_CpuCmd_SetConfig_t;)


/**
 * \brief SET_CONFIG command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetConfigResp
{
	adrv903x_CpuErrorCode_e cmdStatus;  /*!< Command status */
} adrv903x_CpuCmd_SetConfigResp_t;)


/**
 * \brief GET_CONFIG command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetConfig
{
	adrv903x_CpuObjectId_t objId;          /*!< Object ID of calibration or system component */
	uint16_t               offset;         /*!< Offset into the configuration structure */
    uint16_t               length;         /*!< Length of the configuration in bytes */
} adrv903x_CpuCmd_GetConfig_t;)


/**
 * \brief GET_CONFIG command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetConfigResp
{
	adrv903x_CpuErrorCode_e cmdStatus;      /*!< Command status */

    /* Config data payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t configData[];
     */   
} adrv903x_CpuCmd_GetConfigResp_t;)


/**
 * \brief Configuration payload size struct.
 *        Used to determine the maximum configuration payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_SetConfigMaxSize
{
    adrv903x_CpuCmd_SetConfig_t  setConfigCmd;
    uint8_t                      configData[MAX_CONFIG_DATA_SIZE];   /*!< Configuration data */ 
} adrv903x_CpuCmd_SetConfigMaxSize_t;

#ifndef ADI_ADRV903X_FW
#endif

/**
 * \brief Configuration payload size struct.
 *        Used to determine the maximum configuration payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_GetConfigMaxSize
{
    adrv903x_CpuCmd_GetConfigResp_t  getConfigCmdResp;
    uint8_t                          configData[MAX_CONFIG_DATA_SIZE];   /*!< Configuration data */ 
} adrv903x_CpuCmd_GetConfigMaxSize_t;

#endif /* __ADRV903X_CPU_CMD_CONFIG_H__ */


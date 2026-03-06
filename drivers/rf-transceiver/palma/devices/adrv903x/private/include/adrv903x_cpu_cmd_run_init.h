/**
 * \file adrv903x_cpu_cmd_run_init.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_RUN_INIT,
 *          ADRV903X_CPU_CMD_ID_RUN_INIT_GET_COMPLETION_STATUS,
 *          ADRV903X_CPU_CMD_ID_RUN_INIT_GET_DETAILED_STATUS,
 *          ADRV903X_CPU_CMD_ID_RUN_INIT_ABORT
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_RUN_INIT_H__
#define __ADRV903X_CPU_CMD_RUN_INIT_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cals_types.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_object_ids_types.h"

/**
 * \brief RUN_INIT command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RunInit
{
    adi_adrv903x_InitCals_t config; /*!< Initial calibrations configuration structure */
} adrv903x_CpuCmd_RunInit_t;)

/**
 * \brief RUN_INIT command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RunInitResp
{
    adrv903x_CpuErrorCode_e status;  /*!< Error status code. CPU_NO_ERROR if initial calibration was successfully started. */
} adrv903x_CpuCmd_RunInitResp_t;)

/**
 * \brief RUN_INIT_GET_COMPLETION_STATUS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RunInitGetCompletionStatusResp
{
    uint8_t inProgress;      /*!< Init cal progress flag. Will be set to 1 if an initial calibration run is currently in progress. Set to 0 otherwise. */
    uint8_t success;         /*!< Init cal success/failure status. Set to 1 if all cals completed successfully. Set to 0 otherwise. Only valid when
                                  inProgress equals 0. Issue ADRV903X_CPU_CMD_ID_RUN_INIT_GET_DETAILED_STATUS for detailed error info. */
} adrv903x_CpuCmd_RunInitGetCompletionStatusResp_t;)

/**
 * \brief RUN_INIT_GET_DETAILED_STATUS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_RunInitGetDetailedStatusResp
{
    adi_adrv903x_InitCalStatus_t status; /*!< Structure containing init cal detailed status */
} adrv903x_CpuCmd_RunInitGetDetailedStatusResp_t;)

#endif /* __ADRV903X_CPU_CMD_RUN_INIT_H__ */


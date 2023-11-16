/**
 * \file adrv904x_cpu_cmd_run_init.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_RUN_INIT,
 *          ADRV904X_CPU_CMD_ID_RUN_INIT_GET_COMPLETION_STATUS,
 *          ADRV904X_CPU_CMD_ID_RUN_INIT_GET_DETAILED_STATUS,
 *          ADRV904X_CPU_CMD_ID_RUN_INIT_ABORT
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_RUN_INIT_H__
#define __ADRV904X_CPU_CMD_RUN_INIT_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cals_types.h"
#include "adrv904x_cpu_error_codes_types.h"
#include "adrv904x_cpu_object_ids_types.h"

/**
 * \brief RUN_INIT command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunInit
{
    adi_adrv904x_InitCals_t config; /*!< Initial calibrations configuration structure */
} adrv904x_CpuCmd_RunInit_t;)

/**
 * \brief RUN_INIT command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunInitResp
{
    adrv904x_CpuErrorCode_e status;  /*!< Error status code. CPU_NO_ERROR if initial calibration was successfully started. */
} adrv904x_CpuCmd_RunInitResp_t;)

/**
 * \brief RUN_INIT_GET_COMPLETION_STATUS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunInitGetCompletionStatusResp
{
    uint8_t inProgress;      /*!< Init cal progress flag. Will be set to 1 if an initial calibration run is currently in progress. Set to 0 otherwise. */
    uint8_t success;         /*!< Init cal success/failure status. Set to 1 if all cals completed successfully. Set to 0 otherwise. Only valid when
                                  inProgress equals 0. Issue ADRV904X_CPU_CMD_ID_RUN_INIT_GET_DETAILED_STATUS for detailed error info. */
} adrv904x_CpuCmd_RunInitGetCompletionStatusResp_t;)

/**
 * \brief RUN_INIT_GET_DETAILED_STATUS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunInitGetDetailedStatusResp
{
    adi_adrv904x_InitCalStatus_t status; /*!< Structure containing init cal detailed status */
} adrv904x_CpuCmd_RunInitGetDetailedStatusResp_t;)

#endif /* __ADRV904X_CPU_CMD_RUN_INIT_H__ */


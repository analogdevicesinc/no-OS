/**
 * \file adrv904x_cpu_cmd_mcs.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_START_MCS,
 *          ADRV904X_CPU_CMD_ID_MCS_COMPLETE
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_MCS_H__
#define __ADRV904X_CPU_CMD_MCS_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/* START_MCS command has no payload */
/**
 * \brief START_MCS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_StartMcsResp
{
    adrv904x_CpuErrorCode_e status;  /*!< Error status code. CPU_NO_ERROR if MCS successfully started. */
} adrv904x_CpuCmd_StartMcsResp_t;)

/* MCS_COMPLETE command has no payload */
/* MCS_COMPLETE command response structure */
/**
 * \brief MCS_COMPLETEcommand response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_McsCompleteResp
{
    adrv904x_CpuErrorCode_e status;  /*!< Error status code. CPU_NO_ERROR if MCS successfully started. */
} adrv904x_CpuCmd_McsCompleteResp_t;)

#endif /* __ADRV904X_CPU_CMD_MCS_H__ */


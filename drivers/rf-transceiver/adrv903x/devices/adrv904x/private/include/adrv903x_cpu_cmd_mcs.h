/**
 * \file adrv903x_cpu_cmd_mcs.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_START_MCS,
 *          ADRV903X_CPU_CMD_ID_MCS_COMPLETE
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_MCS_H__
#define __ADRV903X_CPU_CMD_MCS_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"

/* START_MCS command has no payload */
/**
 * \brief START_MCS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_StartMcsResp
{
    adrv903x_CpuErrorCode_e status;  /*!< Error status code. CPU_NO_ERROR if MCS successfully started. */
} adrv903x_CpuCmd_StartMcsResp_t;)

/* MCS_COMPLETE command has no payload */
/* MCS_COMPLETE command response structure */
/**
 * \brief MCS_COMPLETEcommand response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_McsCompleteResp
{
    adrv903x_CpuErrorCode_e status;  /*!< Error status code. CPU_NO_ERROR if MCS successfully started. */
} adrv903x_CpuCmd_McsCompleteResp_t;)

#endif /* __ADRV903X_CPU_CMD_MCS_H__ */


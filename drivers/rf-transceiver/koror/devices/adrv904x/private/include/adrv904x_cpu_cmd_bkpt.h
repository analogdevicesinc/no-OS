/**
 * \file adrv904x_cpu_cmd_bkpt.h
 * 
 * \brief   Command definitions for SW breakpoints.
 *
 * ADRV904X API Version: 2.15.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV904X_CPU_CMD_BKPT_H__
#define __ADRV904X_CPU_CMD_BKPT_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"


/**
 * \brief RESUME_BKTPT command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_ResumeBkpt
{
    uint32_t objectID;
    uint8_t chanMask;
    uint8_t bResumeAll;
} adrv904x_CpuCmd_ResumeBkpt_t;)


/**
 * \brief RESUME_BKTPT command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_ResumeBkptResp
{
	adrv904x_CpuErrorCode_e cmdStatus;      /*!< Command status */
} adrv904x_CpuCmd_ResumeBkptResp_t;)

#endif /* __ADRV904X_CPU_CMD_RESUME_BKPT_H__ */


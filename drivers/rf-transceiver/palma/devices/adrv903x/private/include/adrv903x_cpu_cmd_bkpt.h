/**
 * \file adrv903x_cpu_cmd_bkpt.h
 * 
 * \brief   Command definitions for SW breakpoints.
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_BKPT_H__
#define __ADRV903X_CPU_CMD_BKPT_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"


/**
 * \brief RESUME_BKTPT command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_ResumeBkpt
{
    uint32_t objectID;
    uint8_t chanMask;
    uint8_t bResumeAll;
} adrv903x_CpuCmd_ResumeBkpt_t;)


/**
 * \brief RESUME_BKTPT command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_ResumeBkptResp
{
	adrv903x_CpuErrorCode_e cmdStatus;      /*!< Command status */
} adrv903x_CpuCmd_ResumeBkptResp_t;)

#endif /* __ADRV903X_CPU_CMD_RESUME_BKPT_H__ */


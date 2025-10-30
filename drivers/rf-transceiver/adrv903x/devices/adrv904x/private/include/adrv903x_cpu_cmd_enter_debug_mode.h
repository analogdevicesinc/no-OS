/**
 * \file adrv903x_cpu_cmd_enter_debug_mode.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_ENTER_DEBUG_MODE
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_ENTER_DEBUG_MODE_H__
#define __ADRV903X_CPU_CMD_ENTER_DEBUG_MODE_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"


/**
 * \brief ENTER_DEBUG_MODE command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_EnterDebugMode
{
    uint32_t debugModeKey;
} adrv903x_CpuCmd_EnterDebugMode_t;)


/**
 * \brief ENTER_DEBUG_MODE command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_EnterDebugModeResp
{
	adrv903x_CpuErrorCode_e cmdStatus;      /*!< Command status */
} adrv903x_CpuCmd_EnterDebugModeResp_t;)


#endif /* __ADRV903X_CPU_CMD_ENTER_DEBUG_MODE_H__ */


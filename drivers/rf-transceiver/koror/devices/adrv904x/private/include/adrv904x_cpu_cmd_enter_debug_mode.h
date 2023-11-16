/**
 * \file adrv904x_cpu_cmd_enter_debug_mode.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_ENTER_DEBUG_MODE
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_ENTER_DEBUG_MODE_H__
#define __ADRV904X_CPU_CMD_ENTER_DEBUG_MODE_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"


/**
 * \brief ENTER_DEBUG_MODE command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_EnterDebugMode
{
    uint32_t debugModeKey;
} adrv904x_CpuCmd_EnterDebugMode_t;)


/**
 * \brief ENTER_DEBUG_MODE command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_EnterDebugModeResp
{
	adrv904x_CpuErrorCode_e cmdStatus;      /*!< Command status */
} adrv904x_CpuCmd_EnterDebugModeResp_t;)


#endif /* __ADRV904X_CPU_CMD_ENTER_DEBUG_MODE_H__ */


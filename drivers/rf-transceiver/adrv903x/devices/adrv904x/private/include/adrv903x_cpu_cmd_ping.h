/**
 * \file adrv903x_cpu_cmd_ping.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_PING
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_PING
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_PING_H__
#define __ADRV903X_CPU_CMD_PING_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"

/**
 * \brief PING command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_Ping
{
    uint32_t echoData;      /*!< Data to be echoed back by CPU */
} adrv903x_CpuCmd_Ping_t;)

/**
 * \brief PING command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_PingResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint32_t echoData;               /*!< Echoed data from CPU */
} adrv903x_CpuCmd_PingResp_t;)

#endif /* __ADRV903X_CPU_CMD_PING_H__ */


/**
 * \file adrv904x_cpu_cmd_ping.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_PING
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_PING
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_PING_H__
#define __ADRV904X_CPU_CMD_PING_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 * \brief PING command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_Ping
{
    uint32_t echoData;      /*!< Data to be echoed back by CPU */
} adrv904x_CpuCmd_Ping_t;)

/**
 * \brief PING command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_PingResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint32_t echoData;               /*!< Echoed data from CPU */
} adrv904x_CpuCmd_PingResp_t;)

#endif /* __ADRV904X_CPU_CMD_PING_H__ */


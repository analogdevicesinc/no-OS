/**
 * \file adi_adrv903x_cpu_cmd_ser_reset.h
 * 
 * \brief   Command definition for ADI_ADRV903X_CPU_CMD_ID_SER_RESET
 *
 * \details Command definition for ADI_ADRV903X_CPU_CMD_ID_SER_RESET
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2021 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADI_ADRV903X_CPU_CMD_SER_RESET_H__
#define __ADI_ADRV903X_CPU_CMD_SER_RESET_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"

/**
 * \brief SER_RESET command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SerReset
{
    uint32_t serResetParm;      /*!< Serializer reset parameter */
} adrv903x_CpuCmd_SerReset_t;)

/**
 * \brief SER_RESET command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SerResetResp
{
    adrv903x_CpuErrorCode_e status;     /*!< CPU error status code */
    uint32_t serResetResults;           /*!< Ser Reset results CPU */
} adrv903x_CpuCmd_SerResetResp_t;)

#endif /* __ADI_ADRV903X_CPU_CMD_SER_RESET_H__ */



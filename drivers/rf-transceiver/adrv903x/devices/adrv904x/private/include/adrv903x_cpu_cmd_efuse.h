/**
 * \file adrv903x_cpu_cmd_efuse.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_EFUSE_GET
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_EFUSE_GET
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_EFUSE_H__
#define __ADRV903X_CPU_CMD_EFUSE_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"

/**
 * \brief EFUSE get command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_EfuseGet
{
    uint32_t addr;      /*!< Address to read from */
} adrv903x_CpuCmd_EfuseGet_t;)

/**
 * \brief EFUSE get command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_EfuseGetResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint32_t value;                      /*!< data read from EFUSE address */
} adrv903x_CpuCmd_EfuseGetResp_t;)

#endif /* __ADRV903X_CPU_CMD_EFUSE_H__ */


/**
 * \file adrv904x_cpu_cmd_efuse.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_EFUSE_GET
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_EFUSE_GET
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_EFUSE_H__
#define __ADRV904X_CPU_CMD_EFUSE_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 * \brief EFUSE get command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_EfuseGet
{
    uint32_t addr;      /*!< Address to read from */
} adrv904x_CpuCmd_EfuseGet_t;)

/**
 * \brief EFUSE get command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_EfuseGetResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
    uint32_t value;                      /*!< data read from EFUSE address */
} adrv904x_CpuCmd_EfuseGetResp_t;)

#endif /* __ADRV904X_CPU_CMD_EFUSE_H__ */


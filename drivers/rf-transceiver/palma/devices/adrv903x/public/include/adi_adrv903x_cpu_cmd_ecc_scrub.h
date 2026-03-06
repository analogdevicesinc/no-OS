/**
 * Disclaimer Legal Disclaimer
 * Copyright 2022 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**
 * \file adi_adrv903x_cpu_cmd_ecc_scrub.h
 * 
 * \brief   Contains ADRV903X CPU ECC Scrub enable command data structures.
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADI_ADRV903X_CPU_ECC_SCRUB_CMD_H__
#define __ADI_ADRV903X_CPU_ECC_SCRUB_CMD_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"

/**
 * \brief CPU ECC Set command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuSetEccScrubEnableCmd
{
    uint8_t    eccScrubEnable;      /*!< ECC scrubbing enable setting */
} adi_adrv903x_CpuSetEccScrubEnableCmd_t;)

/**
 * \brief CPU ECC Set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuSetEccScrubEnableCmdResp
{
    adrv903x_CpuErrorCode_e status;      /*!< CPU error status code */
} adi_adrv903x_CpuSetEccScrubEnableCmdResp_t;)

/**
 * \brief CPU ECC Get command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuGetEccScrubEnableCmdResp
{
    adrv903x_CpuErrorCode_e status;      /*!< CPU error status code */
    uint8_t                     eccScrubEnable;      /*!< ECC scrubbing enable state */
} adi_adrv903x_CpuGetEccScrubEnableCmdResp_t;)

#endif /* __ADI_ADRV903X_CPU_ECC_SCRUB_CMD_H__ */


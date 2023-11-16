/**
 * Disclaimer Legal Disclaimer
 * Copyright 2022 - 2022 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adi_adrv904x_cpu_cmd_ecc_scrub.h
 * 
 * \brief   Contains ADRV904X CPU ECC Scrub enable command data structures.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADI_ADRV904X_CPU_ECC_SCRUB_CMD_H__
#define __ADI_ADRV904X_CPU_ECC_SCRUB_CMD_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

/**
 * \brief CPU ECC Set command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuSetEccScrubEnableCmd
{
    uint8_t    eccScrubEnable;      /*!< ECC scrubbing enable setting */
} adi_adrv904x_CpuSetEccScrubEnableCmd_t;)

/**
 * \brief CPU ECC Set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuSetEccScrubEnableCmdResp
{
    adrv904x_CpuErrorCode_e status;      /*!< CPU error status code */
} adi_adrv904x_CpuSetEccScrubEnableCmdResp_t;)

/**
 * \brief CPU ECC Get command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuGetEccScrubEnableCmdResp
{
    adrv904x_CpuErrorCode_e status;      /*!< CPU error status code */
    uint8_t                     eccScrubEnable;      /*!< ECC scrubbing enable state */
} adi_adrv904x_CpuGetEccScrubEnableCmdResp_t;)

#endif /* __ADI_ADRV904X_CPU_ECC_SCRUB_CMD_H__ */


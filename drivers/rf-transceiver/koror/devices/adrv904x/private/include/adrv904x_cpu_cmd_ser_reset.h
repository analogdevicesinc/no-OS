/**
 * \file adi_adrv904x_cpu_cmd_ser_reset.h
 * 
 * \brief   Command definition for ADI_ADRV904X_CPU_CMD_ID_SER_RESET
 *
 * \details Command definition for ADI_ADRV904X_CPU_CMD_ID_SER_RESET
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2021 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADI_ADRV904X_CPU_CMD_SER_RESET_H__
#define __ADI_ADRV904X_CPU_CMD_SER_RESET_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 * \brief SER_RESET command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SerReset
{
    uint32_t serResetParm;      /*!< Serializer reset parameter */
} adrv904x_CpuCmd_SerReset_t;)

/**
 * \brief SER_RESET command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SerResetResp
{
    adrv904x_CpuErrorCode_e status;     /*!< CPU error status code */
    uint32_t serResetResults;           /*!< Ser Reset results CPU */
} adrv904x_CpuCmd_SerResetResp_t;)

#endif /* __ADI_ADRV904X_CPU_CMD_SER_RESET_H__ */



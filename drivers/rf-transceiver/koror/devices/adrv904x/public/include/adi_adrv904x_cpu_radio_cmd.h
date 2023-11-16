/**
 * Disclaimer Legal Disclaimer
 * Copyright 2022 - 2022 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adi_adrv904x_cpu_radio_cmd.h
 * 
 * \brief   Contains ADRV904X CPU Radio command data structures.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADI_ADRV904X_CPU_RADIO_CMD_H__
#define __ADI_ADRV904X_CPU_RADIO_CMD_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

#define CPU_RADIO_CMD_BUFFER_SIZE      (16u)
#define CPU_RADIO_CMD_RESP_BUFFER_SIZE (22u)

/**
 * \brief CPU Radio command response structure (Koror only)
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuRadioCmd
{
    uint8_t    opcode;                                  /*!< opcode for the radio msg: 0, 1, .. */
    uint8_t    buffer[CPU_RADIO_CMD_BUFFER_SIZE];       /*!< container for radio command message */
} adi_adrv904x_CpuRadioCmd_t;)

/**
 * \brief CPU Radio command response structure (Koror only)
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuRadioCmdResp
{
    adi_adrv904x_CpuErrorCode_t status;      /*!< CPU error status code */
    uint8_t                     buffer[CPU_RADIO_CMD_RESP_BUFFER_SIZE];  /*!< container for radio command response message */
} adi_adrv904x_CpuRadioCmdResp_t;)

#endif /* __ADI_ADRV904X_CPU_RADIO_CMD_H__ */


/**
 * \file adrv904x_cpu_cmd_radio.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_RADIO
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_RADIO
 *
 * ADRV904X API Version: 2.15.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2022 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV904X_CPU_CMD_RADIO_H__
#define __ADRV904X_CPU_CMD_RADIO_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 *  \brief Enum of radio subcommands
 */
typedef enum adrv904x_CpuCmd_RadioSubcommand
{
    ADI_ADRV904X_RADIO_PING = 0,                  /*!< Radio ping command */
    ADI_ADRV904X_RADIO_TXLOL_GET_PARAMS = 1,      /*!< Radio TxLOL get correction parameters */
} adrv904x_CpuCmd_RadioSubcommand_e;

/**
 * \brief Radio TxLOL parameters get command structure
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_CpuCmd_RadioTxLolParamsGet
{
    uint8_t txChan;      /*!< Tx channel (0 - 7) */
    uint8_t hpAtten;     /*!< Tx attenuation (1 - 64) */
} adrv904x_CpuCmd_RadioTxLolParamsGet_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_CPU_CMD_RADIO_H__ */


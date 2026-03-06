/**
 * \file adrv903x_cpu_cmd_radio.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_RADIO
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_RADIO
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2022 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_RADIO_H__
#define __ADRV903X_CPU_CMD_RADIO_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"

/**
 *  \brief Enum of radio subcommands
 */
typedef enum adrv903x_CpuCmd_RadioSubcommand
{
    ADI_ADRV903X_RADIO_PING = 0,                  /*!< Radio ping command */
    ADI_ADRV903X_RADIO_TXLOL_GET_PARAMS = 1,      /*!< Radio TxLOL get correction parameters */
} adrv903x_CpuCmd_RadioSubcommand_e;

/**
 * \brief Radio TxLOL parameters get command structure
 */
ADI_ADRV903X_PACK_START
typedef struct adrv903x_CpuCmd_RadioTxLolParamsGet
{
    uint8_t txChan;      /*!< Tx channel (0 - 7) */
    uint8_t hpAtten;     /*!< Tx attenuation (1 - 64) */
} adrv903x_CpuCmd_RadioTxLolParamsGet_t;
ADI_ADRV903X_PACK_FINISH

#endif /* __ADRV903X_CPU_CMD_RADIO_H__ */


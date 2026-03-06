/**
 * \file adrv903x_cpu_cmd_fast_attack.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_FAST_ATTACK
 *
 * \details Command definition for ADRV903X_CPU_CMD_FAST_ATTACK
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_FAST_ATTACK_H__
#define __ADRV903X_CPU_CMD_FAST_ATTACK_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"

#define ADRV903X_RX_ADC_FAST_ATTACK_STATUS_IDX      (11u)
#define ADRV903X_ORX_ADC_FAST_ATTACK_STATUS_IDX     (6u)
#define ADRV903X_TXLB_ADC_FAST_ATTACK_STATUS_IDX    (6u)

/**
 * \brief Fast Attack command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_EnFastAttack
{
    uint32_t calMask;     /*!< Cals to be set in Fast Attack mode */
} adrv903x_CpuCmd_EnFastAttack_t;)

/**
 * \brief ast Attack command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_EnFastAttackResp
{
    adi_adrv903x_CpuErrorCode_t status;         /*!< CPU error status code */
    uint32_t                    failedCalMask;  /*!< Cals failed to be set in Fast Attack mode */
} adrv903x_CpuCmd_EnFastAttackResp_t;)

#endif /* __ADRV903X_CPU_CMD_FAST_ATTACK__ */



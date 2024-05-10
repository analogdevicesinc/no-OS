/**
 * \file adrv904x_cpu_cmd_fast_attack.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_FAST_ATTACK
 *
 * \details Command definition for ADRV904X_CPU_CMD_FAST_ATTACK
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_FAST_ATTACK_H__
#define __ADRV904X_CPU_CMD_FAST_ATTACK_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

#define ADRV904X_RX_ADC_FAST_ATTACK_STATUS_IDX      (11u)
#define ADRV904X_ORX_ADC_FAST_ATTACK_STATUS_IDX     (6u)
#define ADRV904X_TXLB_ADC_FAST_ATTACK_STATUS_IDX    (6u)

/**
 * \brief Fast Attack command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_EnFastAttack
{
    uint32_t calMask;     /*!< Cals to be set in Fast Attack mode */
} adrv904x_CpuCmd_EnFastAttack_t;)

/**
 * \brief ast Attack command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_EnFastAttackResp
{
    adi_adrv904x_CpuErrorCode_t status;         /*!< CPU error status code */
    uint32_t                    failedCalMask;  /*!< Cals failed to be set in Fast Attack mode */
} adrv904x_CpuCmd_EnFastAttackResp_t;)

#endif /* __ADRV904X_CPU_CMD_FAST_ATTACK__ */



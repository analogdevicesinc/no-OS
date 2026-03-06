/**
 * \file adrv903x_cpu_cmd_getset_txatten_phase.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_GET_TXATTEN_PHASE,
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_SET_TXATTEN_PHASE,
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_GETSET_TXATTEN_PHASE_H__
#define __ADRV903X_CPU_CMD_GETSET_TXATTEN_PHASE_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_device_profile_types.h"
#include "adi_adrv903x_tx_nco.h"

/**
 * \brief tx atten phase set command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetTxAttenPhase
{
    uint8_t                   chanSelect;                         /*!< Tx channel (bit mapped) */
    int16_t                   txAttenPhase[ADI_ADRV903X_TX_ATTEN_PHASE_SIZE];  /*!< Tx atten phase values */
} adrv903x_CpuCmd_SetTxAttenPhase_t;)

/**
 * \brief tx atten phase set command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetTxAttenPhaseResp
{
    adrv903x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv903x_CpuCmd_SetTxAttenPhaseResp_t;)

/**
 * \brief tx atten phase get command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetTxAttenPhase
{
    uint8_t                   chanSelect;             /*!< Tx channel (bit mapped) */
} adrv903x_CpuCmd_GetTxAttenPhase_t;)

/**
 * \brief tx atten phase get command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetTxAttenPhaseResp
{
    adrv903x_CpuErrorCode_e    status;                                              /*!< CPU error status code */
    int16_t                    txAttenPhase[ADI_ADRV903X_TX_ATTEN_PHASE_SIZE];      /*!< Tx atten phase values */  
} adrv903x_CpuCmd_GetTxAttenPhaseResp_t;)



#endif /* __ADRV903X_CPU_CMD_GETSET_TXATTEN_PHASE_H__ */


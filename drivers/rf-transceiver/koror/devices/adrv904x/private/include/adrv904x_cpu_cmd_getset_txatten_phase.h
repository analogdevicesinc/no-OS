/**
 * \file adrv904x_cpu_cmd_getset_txatten_phase.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_GET_TXATTEN_PHASE,
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_SET_TXATTEN_PHASE,
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_GETSET_TXATTEN_PHASE_H__
#define __ADRV904X_CPU_CMD_GETSET_TXATTEN_PHASE_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"
#include "adrv904x_cpu_device_profile_types.h"
#include "adi_adrv904x_tx_nco.h"

/**
 * \brief tx atten phase set command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetTxAttenPhase
{
    uint8_t                   chanSelect;                         /*!< Tx channel (bit mapped) */
    int16_t                   txAttenPhase[ADI_ADRV904X_TX_ATTEN_PHASE_SIZE];  /*!< Tx atten phase values */
} adrv904x_CpuCmd_SetTxAttenPhase_t;)

/**
 * \brief tx atten phase set command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetTxAttenPhaseResp
{
    adrv904x_CpuErrorCode_e status;  /*!< CPU error status code */
} adrv904x_CpuCmd_SetTxAttenPhaseResp_t;)

/**
 * \brief tx atten phase get command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetTxAttenPhase
{
    uint8_t                   chanSelect;             /*!< Tx channel (bit mapped) */
} adrv904x_CpuCmd_GetTxAttenPhase_t;)

/**
 * \brief tx atten phase get command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetTxAttenPhaseResp
{
    adrv904x_CpuErrorCode_e    status;                                              /*!< CPU error status code */
    int16_t                    txAttenPhase[ADI_ADRV904X_TX_ATTEN_PHASE_SIZE];      /*!< Tx atten phase values */  
} adrv904x_CpuCmd_GetTxAttenPhaseResp_t;)



#endif /* __ADRV904X_CPU_CMD_GETSET_TXATTEN_PHASE_H__ */


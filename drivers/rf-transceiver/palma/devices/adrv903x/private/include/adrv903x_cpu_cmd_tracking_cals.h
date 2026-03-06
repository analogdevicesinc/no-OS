/**
 * \file adrv903x_cpu_cmd_tracking_cals.h
 * 
 * \brief   Command definition for: ADRV903X_CPU_CMD_ID_SET_ENABLED_TRACKING_CALS,
 *          ADRV903X_CPU_CMD_ID_GET_ENABLED_TRACKING_CALS,
 *          ADRV903X_CPU_CMD_ID_GET_TRACKING_CAL_STATE
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_TRACKING_CALS_H__
#define __ADRV903X_CPU_CMD_TRACKING_CALS_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cals_types.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_object_ids_types.h"

typedef uint8_t  adrv903x_TrackingCalEnableDisable_t;

/**
 * \brief SET_ENABLED_TRACKING_CALS command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetEnabledTrackingCals
{
    adi_adrv903x_TrackingCalibrationMask_t calMask;
    uint32_t                               channelMask;
    adrv903x_TrackingCalEnableDisable_t    enableDisable;
} adrv903x_CpuCmd_SetEnabledTrackingCals_t;)

/**
 * \brief SET_ENABLED_TRACKING_CALS_V2 command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetEnabledTrackingCals_v2
{
    adi_adrv903x_TrackingCalibrationMask_t calMask;
    uint32_t rxChannel; 
    uint32_t orxChannel; 
    uint32_t txChannel; 
    uint32_t laneSerdes; 
    adrv903x_TrackingCalEnableDisable_t    enableDisable;
} adrv903x_CpuCmd_SetEnabledTrackingCals_v2_t;)
    
/**
 * \brief SET_ENABLED_TRACKING_CALS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetEnabledTrackingCalsResp
{
    adrv903x_CpuErrorCode_e status;
} adrv903x_CpuCmd_SetEnabledTrackingCalsResp_t;)

/**
 * \brief GET_ENABLED_TRACKING_CALS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetEnabledTrackingCalsResp
{
    adrv903x_CpuErrorCode_e               status;
    adi_adrv903x_TrackingCalEnableMasks_t enableMasks;
} adrv903x_CpuCmd_GetEnabledTrackingCalsResp_t;)

/**
 * \brief GET_TRACKING_CAL_STATE command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetTrackingCalStateResp
{
	adrv903x_CpuErrorCode_e         status;
    adi_adrv903x_TrackingCalState_t calState;
} adrv903x_CpuCmd_GetTrackingCalStateResp_t;)

#ifdef __ICCARM__
#pragma diag_default=Pm009
#endif /* __ICCARM__ */

#endif /* __ADRV903X_CPU_CMD_TRACKING_CALS_H__ */


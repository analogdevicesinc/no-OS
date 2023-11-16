/**
 * \file adrv904x_cpu_cmd_tracking_cals.h
 * 
 * \brief   Command definition for: ADRV904X_CPU_CMD_ID_SET_ENABLED_TRACKING_CALS,
 *          ADRV904X_CPU_CMD_ID_GET_ENABLED_TRACKING_CALS,
 *          ADRV904X_CPU_CMD_ID_GET_TRACKING_CAL_STATE
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_TRACKING_CALS_H__
#define __ADRV904X_CPU_CMD_TRACKING_CALS_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cals_types.h"
#include "adrv904x_cpu_error_codes_types.h"
#include "adrv904x_cpu_object_ids_types.h"

typedef uint8_t  adrv904x_TrackingCalEnableDisable_t;

/**
 * \brief SET_ENABLED_TRACKING_CALS command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetEnabledTrackingCals
{
    adi_adrv904x_TrackingCalibrationMask_t calMask;
    uint32_t                               channelMask;
    adrv904x_TrackingCalEnableDisable_t    enableDisable;
} adrv904x_CpuCmd_SetEnabledTrackingCals_t;)

/**
 * \brief SET_ENABLED_TRACKING_CALS_V2 command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetEnabledTrackingCals_v2
{
    adi_adrv904x_TrackingCalibrationMask_t calMask;
    uint32_t rxChannel; 
    uint32_t orxChannel; 
    uint32_t txChannel; 
    uint32_t laneSerdes; 
    adrv904x_TrackingCalEnableDisable_t    enableDisable;
} adrv904x_CpuCmd_SetEnabledTrackingCals_v2_t;)
    
/**
 * \brief SET_ENABLED_TRACKING_CALS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetEnabledTrackingCalsResp
{
    adrv904x_CpuErrorCode_e status;
} adrv904x_CpuCmd_SetEnabledTrackingCalsResp_t;)

/**
 * \brief GET_ENABLED_TRACKING_CALS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetEnabledTrackingCalsResp
{
    adrv904x_CpuErrorCode_e               status;
    adi_adrv904x_TrackingCalEnableMasks_t enableMasks;
} adrv904x_CpuCmd_GetEnabledTrackingCalsResp_t;)

/**
 * \brief GET_TRACKING_CAL_STATE command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetTrackingCalStateResp
{
	adrv904x_CpuErrorCode_e         status;
    adi_adrv904x_TrackingCalState_t calState;
} adrv904x_CpuCmd_GetTrackingCalStateResp_t;)

#ifdef __ICCARM__
#pragma diag_default=Pm009
#endif /* __ICCARM__ */

#endif /* __ADRV904X_CPU_CMD_TRACKING_CALS_H__ */


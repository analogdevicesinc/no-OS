/*! **************************************************************************** 
 *
 * @file:    device_profile_phase_sync_nco_t.h
 *
 * @brief:   Phase Sync NCO driver exported NCO typedefs and #defines
 *
 * @details: 
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved. This software is 
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using 
  this software you agree to the terms of the associated Analog Devices License 
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_PHASE_SYNC_NCO_T_H__
#define __DEVICE_PROFILE_PHASE_SYNC_NCO_T_H__

#include "adi_device_profile_pack.h"

typedef enum
{
    PHASE_SYNC_NCO_MODE_ORX_MOD_OFF  = 0u,
    PHASE_SYNC_NCO_MODE_ORX_MOD_ON = 1u,
} ADI_NEVIS_PACK_ENUM phaseSyncNcoMode_e;
  
typedef enum
{
    NCO_START_SYNC_SW  = 0u,  /* select nco_start_sync_a  from NCO control  register*/
    HW_NCO_ENA = 1u,    /* Select the rising edge of  hw_nco_enb provided by HW */
} ADI_NEVIS_PACK_ENUM phaseSyncNcoEnable_e;

ADI_NEVIS_PACK_START
typedef struct
{
    int32_t        freq;        /* NCO output frequency */
    uint32_t       sampleFreq;  /* Sample rate frequency NCO connects to */
    uint32_t       ftwRemainder;
    uint16_t       phaseAvThreshold;
    phaseSyncNcoMode_e phaseSyncNcoMode;
    phaseSyncNcoEnable_e enableSource;
    uint8_t        freeRunClkEnable;
    uint32_t       FTW;   
    
    /* Following two parameters are used for phase sync NCO only */
    double         ncoOutputDouble;   // NCO output freq (not nominal freq)
    float          mismatchMicroHz;   // abs(deltaRxLoTxLo - ncoOutputDouble)
    int32_t        targetedNcoFreq;
} phaseSyncNcoConfig_t;
ADI_NEVIS_PACK_FINISH


#endif /* __DEVICE_PROFILE_PHASE_SYNC_NCO_T_H__ */

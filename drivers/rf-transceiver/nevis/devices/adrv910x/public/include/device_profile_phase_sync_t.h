/*! **************************************************************************** 
 *
 * @file:    device_profile_phase_sync_t.h
 *
 * @brief:   Phase Sync type definitions 
 *
 * @details: This file contains type definition for Phase Sync driver 
 *
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
#ifndef __DEVICE_PROFILE_PHASE_SYNC_T_HEADER__
#define __DEVICE_PROFILE_PHASE_SYNC_T_HEADER__

#include "adi_device_profile_pack.h"

typedef enum
{
    PHSYNC_OFF               = 0x00,          /*!< Phase tracking disabled */
    PHSYNC_INIT_ONLY         = 0x01,          /*!< Do only 1 phase init */
    PHSYNC_CONTINUOUS_TRACK  = 0x02           /*!< Do only 1 phase init and continuous track update */
} ADI_NEVIS_PACK_ENUM PhaseTrackMode_e;

#endif /* __DEVICE_PROFILE_PHASE_SYNC_T_HEADER__ */
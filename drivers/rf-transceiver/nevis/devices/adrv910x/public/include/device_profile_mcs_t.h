/*!
 * @brief   Multipole Chip Synchronization
 *
 * @details Contains functions for system clock alignment
 */

/*******************************************************************************
  Copyright(c) 2010 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
#ifndef __DEVICE_PROFILE_MCS_T_H__
#define __DEVICE_PROFILE_MCS_T_H__

#include "adi_device_profile_pack.h"

/*!
 *  @addtogroup mcs
 *  @ingroup system
 *  @{
 */

/* Enumeration for MCS inferface type */
typedef enum
{
    MCS_INTF_DISABLE = 0u,        /*!< MCS disalbed*/
    MCS_CMOS,
    MCS_LVDS,
} ADI_NEVIS_PACK_ENUM mcsInf_e;
    
/* Enumeration for MCS mode */
typedef enum
{
    MCS_DISABLE = 0u,        /*!< MCS disalbed*/
    MCS_ONLY,
    MCS_PHASE_SYNC,
} ADI_NEVIS_PACK_ENUM mcsMode_e;    


/* Enumeration for MCS substates */
typedef enum
{
    MCS_READY = 0u,        /*!< MCS is ready to receive MCS pulse */
    MCS_TRANSITION = 1u,   /*!< MCS is in MCS pulse transition */
    MCS_DONE = 2u,         
    NUM_MCS_STATE = 3u,
} ADI_NEVIS_PACK_ENUM mcsSubstate_e;

/* Enumeration for MCS status */
typedef enum
{
    MCS_PULSE2 = 0u, 
    MCS_PULSE3,
    MCS_PULSE4, 
    MCS_PULSE5,
    MCS_HS_CLK,  
    NUM_MCS_STATUS,  
} ADI_NEVIS_PACK_ENUM mcsStatus_e;

/*! @} */
#endif /* __DEVICE_PROFILE_MCS_T_H__ */

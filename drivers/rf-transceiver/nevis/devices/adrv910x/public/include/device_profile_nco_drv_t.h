/*! **************************************************************************** 
 *
 * @file:    device_profile_nco_drv_t.h
 *
 * @brief:   NCO driver exported types and definitions
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
#ifndef __DEVICE_PROFILE_NCO_DRV_T_H__
#define __DEVICE_PROFILE_NCO_DRV_T_H__

#include <stdint.h>
#include <stdbool.h>
#include "adi_device_profile_pack.h"

/**
 *  @addtogroup nco
 *  @ingroup drivers
 *  @{
 */

#define RX_NCO1_FTW_UPDATE_TABLE_SIZE 6u /*! number of instructions for DMA NCO FTW update */

ADI_NEVIS_PACK_START
typedef struct
{
    int32_t        freq;        /*! NCO output frequency */
    uint32_t       sampleFreq;  /*! Sample rate frequency NCO connects to */    
    uint16_t       phase;       /*! phase offset */
    uint8_t        realOut;     /*! real out enable  */
    uint8_t        freeRunClkEnable;
} ncoDpConfig_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    uint16_t    txNco1PhaseOffset;  /*! txNCO1 phase offset */
    uint16_t    txNco2PhaseOffset; /*! txNCO1_2 phase offset */
    uint32_t    txNco1Dc_I; /*! txNCO1 DC offset I */
    uint32_t    txNco1Dc_Q;
    uint32_t    txNco2Dc_I;
    uint32_t    txNco2Dc_Q;    
}txSrcNcoPhaseOffsetDc_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    uint8_t     bbfFreqCal_F_Fc;  /*! NCO freq for the high freq tone in FC step F=N/512*184.32*/
    uint16_t    bbfFreqCal_F_LOW; /*! NCO low freq F=N/4096*184.32*/
    uint16_t    bbfFreqCal_F_Q;   /*! NCO freq for the high freq tone in Q step F=N/4096*184*/
    uint8_t     bbfFreqCal_Ai_Fc; /*! Attenuation for the I channel during FC */
    uint8_t     bbfFreqCal_Ai_Q; 
    uint8_t     bbfFreqCal_Aq;    /*! Attenuation for Q channel for Fc and Q step */  
} bbfNcoConfig_t;
ADI_NEVIS_PACK_FINISH

/**@}*/
#endif /* __DEVICE_PROFILE_NCO_DRV_T_H__ */

/*! **************************************************************************** 
 *
 * @file:    device_profile_loop_filter_t.h
 *
 * @brief:   Loopfilter exported type definitions 
 *
 * @details: This file contains type definition for Loopfilter driver 
 *
 * @version: $Revision:
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2014 Analog Devices, Inc. All Rights Reserved. This software is 
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using 
  this software you agree to the terms of the associated Analog Devices License 
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_LOOP_FILTER_T_HEADER__
#define __DEVICE_PROFILE_LOOP_FILTER_T_HEADER__

#include "adi_device_profile_pack.h"

#define MIN_CLKGEN_PHASE_MARGIN       (40U)
#define MAX_CLKGEN_PHASE_MARGIN       (85U)
#define MIN_CLKGEN_LOOP_BW_KHZ        (50U)
#define MAX_CLKGEN_LOOP_BW_KHZ       (400U)
#define MAX_CLKGEN_POWER_SCALE        (10U)

#define LF_MAX_PHASE                  (85u)
#define LF_MIN_PHASE                  (40u)
#define LF_MAX_LOOPFREQ             (1500u)
#define LF_MIN_LOOPFREQ               (50u)
#define LF_MAX_POWER                  (10u)
#define LF_MIN_POWER                   (0u) 

typedef enum {
    PLL_NORM_MODE                   = 0x00,     /*!< Loopfilter PLL setting In slow mode   */
    PLL_FAST_MODE                   = 0x01,     /*!< Loopfilter PLL setting In Fast mode   */
    PLL_NO_CAL_MODE                 = 0x02,     /*!< Loopfilter PLL setting in no cal mode */
} ADI_NEVIS_PACK_ENUM LfPllMode_e;


ADI_NEVIS_PACK_START
typedef struct {
    uint8_t      C1;                            /*!< Loopfilter C1 value */
    uint8_t      C2;                            /*!< Loopfilter C2 value */
    uint8_t      C3;                            /*!< Loopfilter C3 value */

    uint8_t      R1;                            /*!< Loopfilter R1 value */
    uint8_t      R3;                            /*!< Loopfilter R3 value */
    uint8_t      ICP;                           /*!< Loopfilter ICP value */
    uint32_t     effectiveloopBW;               /*!< Loopfilter Effective bw from calculated R/Cs */
} LoopFilterResult_t;
ADI_NEVIS_PACK_FINISH

#endif /* __DEVICE_PROFILE_LOOP_FILTER_T_HEADER__ */

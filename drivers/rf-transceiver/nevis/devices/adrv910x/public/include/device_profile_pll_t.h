/*! ****************************************************************************
 *
 * @file:    device_profile_pll_t.h
 *
 * @brief:   PLL type definitions
 *
 * @details: This file contains type definition for PLL driver
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
#ifndef __DEVICE_PROFILE_PLL_T_HEADER__
#define __DEVICE_PROFILE_PLL_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_pll_common_t.h"

#define PLL_NUM_BANDS                                            (53u)

#define LDO_TEST_RFPLL_FREQ                             (3200000000ul)
#define MIN_RFPLL_FREQ                                    (25000000ul)
#define MAX_RFPLL_FREQ                                  (6000000000ul)
#define MIN_AUXPLL_FREQ                                   (25000000ul)
#define MAX_AUXPLL_FREQ                                 (6000000000ul)

#define MIN_RFPLL_TONE_FREQ                               (20000000ul)
#define MAX_RFPLL_TONE_FREQ                             (6100000000ul)
#define MIN_AUXPLL_TONE_FREQ                              (20000000ul)
#define MAX_AUXPLL_TONE_FREQ                            (6100000000ul)

#define MAX_RFPLL_FREQ_LOGEN_DIV_CAL_A0                 (4250000000ul)
#define MAX_RFPLL_FREQ_LOGEN_DIV_CAL                    (6000000000ul)
#define MIN_RFPLL_FREQ_LOGEN_DIV_CAL                    (3000000000ul)   
#define MAX_AUXPLL_FREQ_LOGEN_DIV_CAL                   (6000000000ul)
#define MIN_AUXPLL_FREQ_LOGEN_DIV_CAL                   (3000000000ul)
#define BEST_PHASE_NOISE_LO                             (4250000000ul) 
   
#define MIN_RFPLL_FREQ_KHZ                              (MIN_RFPLL_FREQ / 1000u)
#define MAX_RFPLL_FREQ_KHZ                              (MAX_RFPLL_FREQ / 1000u)
#define MAX_AUXPLL_FREQ_KHZ                             (MAX_AUXPLL_FREQ /1000u) 

#define PLL_OFFSETLO_DELTA_FREQ_HZ                        (5000000UL)

#define PLL_SHIFT_RF_TO_AUX                                      (-1)
#define PLL_SHIFT_AUX_TO_RF                                      ( 1)
#define SYNC_CYCLE_COUNT                                          (8u)

#endif /* __DEVICE_PROFILE_PLL_T_HEADER__ */

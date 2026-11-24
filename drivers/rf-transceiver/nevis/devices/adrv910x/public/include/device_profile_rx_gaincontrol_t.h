/*! ****************************************************************************
 *
 * @file:    device_profile_rx_gaincontrol_t.h
 *
 * @brief:   Gain Control type definitions
 *
 * @details: This file contains type definition for gain control driver
 *
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_RX_GAINCONTROL_T_HEADER__
#define __DEVICE_PROFILE_RX_GAINCONTROL_T_HEADER__

#include "adi_device_profile_pack.h"
#include <stdint.h>

/*!
 *  @addtogroup rx_gaincontrol
 *  @ingroup drivers
 *  @{
 */

/*! rxNbAgc settings*/
ADI_NEVIS_PACK_START
typedef struct
{
    uint16_t ApdPeakOverloadThresh; /*!< APD Peak overload threshold*/
    uint16_t ApdPeakUnderloadThresh; /*!< APD Peak underload threshold, Optional*/
    uint16_t Hb2PeakOverloadThresh; /*!< HB2 Peak overload threshold*/
    uint16_t Hb2PeakUnderloadThresh; /*!< HB2 Peak underload threshold, Optional*/
    uint16_t Hb2SecondaryUpperThresh; /*!< HB2 Secondary upper threshold*/
    uint8_t padding[2];
} rxNbAgcHighThresholdSettings_t; 
ADI_NEVIS_PACK_FINISH

/*! Gain Control settings*/
ADI_NEVIS_PACK_START
typedef struct
{
    bool rxNbAgcHighThresholdModeEnable; /*!< Enable PFIR on AGC Path*/
    bool rxNbApdLegacyMode;              /*!< Use new highpass prefilter when set to 'false' and use legacy mode (old LUT) if 'true'. */
    uint8_t padding[2];
    rxNbAgcHighThresholdSettings_t agcHighThresholdSettings; /*!< rxNbAgc settings for the profile */
} rx_gainControl_t; 
ADI_NEVIS_PACK_FINISH

#endif /*! __DEVICE_PROFILE_RX_GAINCONTROL_T_HEADER__ */

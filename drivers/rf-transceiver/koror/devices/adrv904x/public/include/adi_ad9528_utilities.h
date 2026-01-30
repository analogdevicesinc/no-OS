/**
* Copyright 2020 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_ad9528_utilities.h
* \brief Contains prototypes and macro definitions for utility helper
*        functions implemented in adi_ad9528_utilities.c
*
*/

#ifndef _ADI_AD9528_UTILITIES_H_
#define _ADI_AD9528_UTILITIES_H_

#include "adi_ad9528_core.h"

/**
* \brief Service to Create AD9528 Init Structure based on Clock Settings
*
* NOTE: This function will modify the contents of the AD9528 device data 
* structure.
*
* <B>Dependencies</B>
* - device->common.devHalInfo
*
* \param[in,out] device Context variable - Pointer to the AD9528 device data structure to initialize
* \param[in]     clockFrequencies adi_ad9528_ClockFrequencySettings_t data structure.
* \param[out]    init Pointer to the adi_ad9528_Init_t data structure.
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_InitStructConfig( adi_ad9528_Device_t* const                          device,
                                                            const adi_ad9528_ClockFrequencySettings_t* const    clockFrequencies,
                                                            adi_ad9528_Init_t* const                            init);

#endif /* ADI_AD9528_UTILITIES_H_ */

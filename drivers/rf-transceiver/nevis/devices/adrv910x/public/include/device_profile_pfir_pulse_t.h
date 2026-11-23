/*!
 * @file    device_profile_pfir_pulse_t.h
 *
 * @brief   PFIR (pulse shaping) data types.
 *
 * @details This file provides data type definitions for PFIR (pulse shaping).
 *
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
#ifndef __DEVICE_PROFILE_PFIR_PULSE_T_HEADER__
#define __DEVICE_PROFILE_PFIR_PULSE_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_pfir_common_t.h"

/**
 *  @addtogroup pfir
 *  @ingroup drivers
 *  @{
 */

#define PFIR_PULSE_SIZE     (128u)

/*! RX NB pulse shaping RFIR Buffer structure */
typedef struct {
	uint8_t  		numCoeff;			/*!< number of coefficients */
    pfirSymmetric_e symmetricSel;   	/*!< symmetric selection */
    uint8_t         taps;           	/*!< taps in number */
    pfirGain_e      gainSel;        	/*!< gain selection */
    int32_t 		coefficients[PFIR_PULSE_SIZE];	/*!< coefficients   */
} pfirPulseBuffer_t;

/**@}*/
#endif  /* __DEVICE_PROFILE_PFIR_PULSE_T_HEADER__ */

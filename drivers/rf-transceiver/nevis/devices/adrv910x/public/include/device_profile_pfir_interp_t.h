/*!
 * @file    t_pfir_interp.h
 *
 * @brief   PFIR (interpolator) data types.
 *
 * @details This file provides data type definitions for PFIR (interpolator).
 *
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
#ifndef __DEVICE_PROFILE_PFIR_INTERP_T_HEADER__
#define __DEVICE_PROFILE_PFIR_INTERP_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_pfir_common_t.h"

/**
 *  @addtogroup pfir
 *  @ingroup drivers
 *  @{
 */

#define PFIR_INTERP_24TAPS      (24u)


/*! Interpolator PFIR bank settings */
typedef enum {
    PFIR_BANK_PING      = 0u,                   /*!< current bank */
    PFIR_BANK_PONG      = 1u,                   /*!< next bank */
} ADI_NEVIS_PACK_ENUM pfirInterpBank_e;

/**@}*/
#endif  /* __DEVICE_PROFILE_PFIR_INTERP_T_HEADER__ */

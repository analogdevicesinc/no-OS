/*!
 * @file    device_profile_pfir_wbnb_t.h
 *
 * @brief   PFIR (WB/NB compensation) data types.
 *
 * @details This file provides data type definitions for PFIR (WB/NB compensation).
 *
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
#ifndef __DEVICE_PROFILE_PFIR_WBNB__T_HEADER__
#define __DEVICE_PROFILE_PFIR_WBNB__T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_pfir_common_t.h"

/**
 *  @addtogroup pfir
 *  @ingroup drivers
 *  @{
 */

#define PFIR_WBNB_SIZE    (128u)

/*! WB/NB compensation PFIR structure */
ADI_NEVIS_PACK_START
typedef struct {
    uint8_t  		numCoeff;               /*!< number of coefficients */
    pfirSymmetric_e symmetricSel;           /*!< symmetric selection */
    pfirNumTaps_e   tapsSel;                /*!< taps selection */
    pfirGain_e      gainSel;                /*!< gain selection */
    int32_t 		coefficients[PFIR_WBNB_SIZE];		/*!< coefficients */
} pfirWbNbBuffer_t;
ADI_NEVIS_PACK_FINISH

/**@}*/
#endif  /* __DEVICE_PROFILE_PFIR_WBNB__T_HEADER__ */

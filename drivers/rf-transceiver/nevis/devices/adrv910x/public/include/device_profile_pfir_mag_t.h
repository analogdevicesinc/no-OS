/*!
 * @file    t_pfir_mag.h
 *
 * @brief   PFIR (magnitude compensation) data types.
 *
 * @details This file provides data type definitions for PFIR (magnitude compensation).
 *
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
#ifndef __DEVICE_PROFILE_PFIR_MAG_T_HEADER__
#define __DEVICE_PROFILE_PFIR_MAG_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_pfir_common_t.h"

/**
 *  @addtogroup pfir
 *  @ingroup drivers
 *  @{
 */

#define PFIR_MAG_COMP_21TAPS    (21u)
#define PFIR_MAG_COMP_13TAPS    (13u)

/*! PFIR magnitude compensation IDs */
typedef enum {
    PFIR_RX_MAG_COMP_21TAPS     = 0u,           /*!< RX magnitude compensation PFIR (21 taps) */
    PFIR_TX_MAG_COMP_21TAPS     = 1u,           /*!< TX magnitude compensation PFIR (21 taps) */
	PFIR_RX_MITIGATION_21TAPS	= 2u,			/*!< RX mitigation PFIR (21 taps) */
	PFIR_MAG_21TAP_MAX_ID		= 2u,			/*!< maximum ID of the 21 tap filters */
    PFIR_TX_MAG_COMP_13TAPS_G10 = 3u,           /*!< TX magnitude compensation PFIR (13 taps) in group 10 */
//  PFIR_TX_MAG_COMP_13TAPS_G3  = 3u,           /*!< TX magnitude compensation PFIR (13 taps) in group 3 in A0 */
    PFIR_RX_MAG_COMP_13TAPS     = 4u,           /*!< RX magnitude compensation PFIR (13 taps) in B0 */
    PFIR_MAG_COMP_MAX_ID        = 4u,           /*!<    magnitude compensation PFIR max ID */
} ADI_NEVIS_PACK_ENUM pfirMagCompId_e;

ADI_NEVIS_PACK_START
typedef struct {
	uint8_t	numCoeff;
	uint8_t padding[3u];
	int32_t	coefficients[PFIR_MAG_COMP_21TAPS];
} pfirMag21Buffer_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct {
	uint8_t	numCoeff;
	uint8_t padding[3u];
	int32_t coefficients[PFIR_MAG_COMP_13TAPS];
} pfirMag13Buffer_t;
ADI_NEVIS_PACK_FINISH

/**@}*/
#endif  /* __DEVICE_PROFILE_PFIR_MAG_T_HEADER__ */

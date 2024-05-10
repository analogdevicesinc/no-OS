/**
* Copyright 2023 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_sbet_types.h
*
* \brief Contains ADRV904X data types for on board sbet feature
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_DFE_SBET_TYPES_H_
#define _ADI_ADRV904X_DFE_SBET_TYPES_H_

#include "adi_adrv904x_user.h"
#include "adi_library_types.h"
#include "adi_adrv904x_platform_pack.h"

/**
* \brief Data structure to hold Tx attenuation to be used when high power DPD Model Index is selected while using SBET feature
*/
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_SbetTxAtten
{
    uint32_t txChannelMask;     /*!< Tx Channel mask selection */
    uint16_t txAttenuation_mdB; /*!< Tx Attenuation value to use during Sbet in millidB */
} adi_adrv904x_SbetTxAtten_t;
ADI_ADRV904X_PACK_FINISH

#endif /* _ADI_ADRV904X_DFE_SBET_TYPES_H_ */
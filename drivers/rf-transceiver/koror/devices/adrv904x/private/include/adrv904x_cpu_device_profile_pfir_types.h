/**
 * \file adrv904x_cpu_device_profile_pfir_types.h
 *
 * \brief   Contains ADRV904X Device Profile PFIR type definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_DEVICE_PROFILE_PFIR_TYPES_H__
#define __ADRV904X_CPU_DEVICE_PROFILE_PFIR_TYPES_H__

#include "adi_adrv904x_platform_pack.h"

#define ADRV904X_NUM_PFIR_COEF    (24u)

typedef enum adrv904x_PfirGain
{
    ADRV904X_PFIR_GAIN_MINUS_12_DB = 0u,
    ADRV904X_PFIR_GAIN_MINUS_6_DB  = 1u,
    ADRV904X_PFIR_GAIN_0_DB        = 2u,
    ADRV904X_PFIR_GAIN_PLUS_6_DB   = 3u
} adrv904x_PfirGain_e;

ADI_ADRV904X_PACK_START
typedef struct adrv904x_RxPfirData
{
    uint8_t enable;
    uint8_t gain;
    uint8_t sym;
    int16_t pfirCoef[ADRV904X_NUM_PFIR_COEF];
} adrv904x_RxPfirData_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_TxPfirData
{
    uint8_t enable;
    uint8_t gain;
    uint8_t sym;
    uint8_t reserved;
    int16_t pfirCoefI[ADRV904X_NUM_PFIR_COEF];
    int16_t pfirCoefQ[ADRV904X_NUM_PFIR_COEF];
} adrv904x_TxPfirData_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_CPU_DEVICE_PROFILE_PFIR_TYPES_H__*/


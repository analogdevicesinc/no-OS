/**
 * \file adrv903x_cpu_device_profile_pfir_types.h
 *
 * \brief   Contains ADRV903X Device Profile PFIR type definitions
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_DEVICE_PROFILE_PFIR_TYPES_H__
#define __ADRV903X_CPU_DEVICE_PROFILE_PFIR_TYPES_H__

#include "adi_adrv903x_platform_pack.h"

#define ADRV903X_NUM_PFIR_COEF    (24u)

typedef enum adrv903x_PfirGain
{
    ADRV903X_PFIR_GAIN_MINUS_12_DB = 0u,
    ADRV903X_PFIR_GAIN_MINUS_6_DB  = 1u,
    ADRV903X_PFIR_GAIN_0_DB        = 2u,
    ADRV903X_PFIR_GAIN_PLUS_6_DB   = 3u
} adrv903x_PfirGain_e;

ADI_ADRV903X_PACK_START
typedef struct adrv903x_RxPfirData
{
    uint8_t enable;
    uint8_t gain;
    uint8_t sym;
    int16_t pfirCoef[ADRV903X_NUM_PFIR_COEF];
} adrv903x_RxPfirData_t;
ADI_ADRV903X_PACK_FINISH

    ADI_ADRV903X_PACK_START
typedef struct adrv903x_TxPfirData
{
    uint8_t enable;
    uint8_t gain;
    uint8_t sym;
    uint8_t reserved;
    int16_t pfirCoefI[ADRV903X_NUM_PFIR_COEF];
    int16_t pfirCoefQ[ADRV903X_NUM_PFIR_COEF];
} adrv903x_TxPfirData_t;
ADI_ADRV903X_PACK_FINISH

#endif /* __ADRV903X_CPU_DEVICE_PROFILE_PFIR_TYPES_H__*/


/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv904x_dfe_osal_t.h
 *
 * \brief Contains shared types used for OSAL CPU load logging.
 * \details Contains typedef which are shared between the BBIC and DFE and used for logging OSAL CPU load information.
 *
 * ADRV904X API Version: 2.15.0.4
 */

#ifndef __ADI_ADRV904X_DFE_OSAL_T_H__
#define __ADI_ADRV904X_DFE_OSAL_T_H__

#include "adi_adrv904x_platform_pack.h"


/** \brief The number of processor cores on the DFE CPU */
#define ADI_ADRV904X_DFE_CPU_MAX_CORES    (4u)


ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeCpuLoad
{
    uint8_t core[ADI_ADRV904X_DFE_CPU_MAX_CORES];    /**< The load of each DFE CPU core, in units of percent */
    uint8_t total;                           /**< The load of the entire DFE CPU, in units of percent */
} adi_adrv904x_DfeCpuLoad_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeCpuAndMemLoad
{
    adi_adrv904x_DfeCpuLoad_t cpuLoad;                  /**< The load of the entire and each DFE CPU, in units of percent */
    uint32_t           memAvailable;             /**< The RAM available for the entire DFE CPU, in units of byte */
    uint32_t           memAllocated;             /**< The RAM allocated for the entire DFE CPU, in units of byte */
} adi_adrv904x_DfeCpuAndMemLoad_t;
ADI_ADRV904X_PACK_FINISH

#endif




/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv903x_cals_structs.h
 *
 * \brief   Contains ADRV903X Calibration data types
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_CAL_STRUCTS_H__
#define __ADRV903X_CAL_STRUCTS_H__

#include "adi_adrv903x_platform_pack.h"

/**
 * \brief Data structure to hold common init and tracking calibration status information
 */
ADI_ADRV903X_PACK_START
typedef struct adi_adrv903x_CalStatus
{
    uint32_t errorCode;                     /*!< Current error condition reported by the calibration */
    uint32_t percentComplete;               /*!< Percentage of completion of the last iteration
                                             *   %20 = Data captures have been started in HW
                                             *   %30 = Data captures are complete, DPD is calculating signal metrics and stability related data
                                             *   %40 = DPD is performing path delay calculation if needed
                                             *   %50 = DPD is performing sample and gain alignment
                                             *   %60 = DPD is running feature filter and Xcorr stage
                                             *   %80 = DPD is running Cholesky decomposition
                                             *   %90 = DPD is calculating robustness metrics
                                             *   %100 = DPD has finished iteration without any error
                                             */
    uint32_t performanceMetric;             /*!< Calibration-specific metric for how well the calibration is performing:
                                             *   ADI_ADRV903X_IC_TXBBF: N/A
                                             *   ADI_ADRV903X_IC_RXTIA: N/A
                                             */
    uint32_t iterCount;                     /*!< Running counter that increments each time the calibration runs to completion */
    uint32_t updateCount;                   /*!< Running counter that increments each time the calibration updates the correction/actuator hardware */
} adi_adrv903x_CalStatus_t;
ADI_ADRV903X_PACK_FINISH

#endif /* __ADRV903X_CAL_STRUCTS_H__ */



/**
 * \file
 * \brief Contains functions to configure AUX ADC channels on the ADRV910X device
 *
 * Copyright (c) 2025 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_AUXADC_TYPES_H_
#define _ADI_ADRV910X_AUXADC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Enum to select AuxADC
 */
typedef enum adi_adrv910x_AuxAdc
{
    ADI_ADRV910X_AUXADC0,               /*!< AuxADC0 */
    ADI_ADRV910X_AUXADC1                /*!< AuxADC1 */
} adi_adrv910x_AuxAdc_e;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_AUXADC_TYPES_H_ */

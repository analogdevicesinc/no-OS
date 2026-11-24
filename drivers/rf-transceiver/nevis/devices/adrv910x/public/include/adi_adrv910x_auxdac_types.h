/**
 * \file
 * \brief Contains functions to configure AUX DAC channels on the ADRV910X device
 *
 * Copyright (c) 2025 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_AUXDAC_TYPES_H_
#define _ADI_ADRV910X_AUXDAC_TYPES_H_

/**
 * \brief Enum to select AuxDAC
 */
typedef enum adi_adrv910x_AuxDac
{
    ADI_ADRV910X_AUXDAC0,               /*!< AuxDAC0 */
    ADI_ADRV910X_AUXDAC1,               /*!< AuxDAC1 */
    ADI_ADRV910X_AUXDAC2,               /*!< AuxDAC2 */
    ADI_ADRV910X_AUXDAC3                /*!< AuxDAC3 */
} adi_adrv910x_AuxDac_e;

#endif /* _ADI_ADRV910X_AUXDAC_TYPES_H_ */

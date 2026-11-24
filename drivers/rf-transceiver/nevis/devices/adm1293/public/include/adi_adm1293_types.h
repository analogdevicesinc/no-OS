/**
 * \file
 * \brief Types related to the power monitoring chips on the CE board
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADM1293_TYPES_H_
#define _ADI_ADM1293_TYPES_H_

#include "adi_common_types.h"

/**
 * \brief Available power monitoring chips by associated I2C slave address
 */
typedef enum adi_adrv910x_PowerMonitor
{
    ADI_ADRV910X_POWERMONITOR_VDD_1P0 = 0x30,  /*!< VDD 1.0V domain */
    ADI_ADRV910X_POWERMONITOR_VDDA_1P0 = 0x31, /*!< VDDA 1.0V domain */
    ADI_ADRV910X_POWERMONITOR_VDD_1P8 = 0x32,  /*!< VDD 1.8V domain */
    ADI_ADRV910X_POWERMONITOR_VDDA_1P8 = 0x33  /*!< VDDA 1.8V domain */
} adi_adrv910x_PowerMonitor_e;

#ifndef CLIENT_IGNORE
/**
 * \brief Data structure to hold ADM1293 device instance settings
 */
typedef struct adi_adm1293_Device
{
    adi_common_Device_t common; /*!< Common layer structure */
} adi_adm1293_Device_t;
#endif

#endif /* _ADI_ADM1293_TYPES_H_ */

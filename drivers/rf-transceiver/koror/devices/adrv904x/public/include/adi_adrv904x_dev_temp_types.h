/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adi_adrv904x_dev_temp_types.h
 * 
 * \brief   Contains ADRV904X device temperature data structures.
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_DEV_TEMP_TYPES_H__
#define __ADRV904X_DEV_TEMP_TYPES_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

/**
 *  \brief Enum of device temperature sensor IDs
 */
typedef enum adi_adrv904x_DevTempSensor
{
    ADI_ADRV904X_DEVTEMP_TX0 = 0u,     /*!< Tx channel 0 temperature sensor */
    ADI_ADRV904X_DEVTEMP_TX1,          /*!< Tx channel 1 temperature sensor */
    ADI_ADRV904X_DEVTEMP_TX2,          /*!< Tx channel 2 temperature sensor */
    ADI_ADRV904X_DEVTEMP_TX3,          /*!< Tx channel 3 temperature sensor */
    ADI_ADRV904X_DEVTEMP_TX4,          /*!< Tx channel 4 temperature sensor */
    ADI_ADRV904X_DEVTEMP_TX5,          /*!< Tx channel 5 temperature sensor */
    ADI_ADRV904X_DEVTEMP_TX6,          /*!< Tx channel 6 temperature sensor */
    ADI_ADRV904X_DEVTEMP_TX7,          /*!< Tx channel 7 temperature sensor */
    ADI_ADRV904X_DEVTEMP_CLKPLL,       /*!< Clk PLL temperature sensor      */
    ADI_ADRV904X_DEVTEMP_RF0PLL,       /*!< RF0 PLL temperature sensor      */
    ADI_ADRV904X_DEVTEMP_RF1PLL,       /*!< RF1 PLL temperature sensor      */
    ADI_ADRV904X_DEVTEMP_SERDESPLL,    /*!< SERDES PLL temperature sensor   */
    ADI_ADRV904X_DEVTEMP_MAX_SENSORS   /*!< Max number of temperature sensors */
} adi_adrv904x_DevTempSensor_e;

/**
 *  \brief Enum of device temperature sensor ID masks
 */
typedef enum adi_adrv904x_DevTempSensorMask
{
    ADI_ADRV904X_DEVTEMP_MASK_TX0        = (1u << ADI_ADRV904X_DEVTEMP_TX0),
    ADI_ADRV904X_DEVTEMP_MASK_TX1        = (1u << ADI_ADRV904X_DEVTEMP_TX1),
    ADI_ADRV904X_DEVTEMP_MASK_TX2        = (1u << ADI_ADRV904X_DEVTEMP_TX2),
    ADI_ADRV904X_DEVTEMP_MASK_TX3        = (1u << ADI_ADRV904X_DEVTEMP_TX3),
    ADI_ADRV904X_DEVTEMP_MASK_TX4        = (1u << ADI_ADRV904X_DEVTEMP_TX4),
    ADI_ADRV904X_DEVTEMP_MASK_TX5        = (1u << ADI_ADRV904X_DEVTEMP_TX5),
    ADI_ADRV904X_DEVTEMP_MASK_TX6        = (1u << ADI_ADRV904X_DEVTEMP_TX6),
    ADI_ADRV904X_DEVTEMP_MASK_TX7        = (1u << ADI_ADRV904X_DEVTEMP_TX7),
    ADI_ADRV904X_DEVTEMP_MASK_CLKPLL     = (1u << ADI_ADRV904X_DEVTEMP_CLKPLL), 
    ADI_ADRV904X_DEVTEMP_MASK_RF0PLL     = (1u << ADI_ADRV904X_DEVTEMP_RF0PLL),
    ADI_ADRV904X_DEVTEMP_MASK_RF1PLL     = (1u << ADI_ADRV904X_DEVTEMP_RF1PLL), 
    ADI_ADRV904X_DEVTEMP_MASK_SERDESPLL  = (1u << ADI_ADRV904X_DEVTEMP_SERDESPLL),
} adi_adrv904x_DevTempSensorMask_e;

#define ADI_ADRV904X_DEVTEMP_INVALID   ~(ADI_ADRV904X_DEVTEMP_MASK_TX0 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_TX1 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_TX2 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_TX3 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_TX4 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_TX5 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_TX6 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_TX7 | \
                                         ADI_ADRV904X_DEVTEMP_MASK_CLKPLL | \
                                         ADI_ADRV904X_DEVTEMP_MASK_RF0PLL | \
                                         ADI_ADRV904X_DEVTEMP_MASK_RF1PLL | \
                                         ADI_ADRV904X_DEVTEMP_MASK_SERDESPLL)

/**
* \brief Data structure to hold device temperature data
*/
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_DevTempData
{
    int16_t tempDegreesCelsius[ADI_ADRV904X_DEVTEMP_MAX_SENSORS];  /*!< Temperature readings from all temperature sensors */
    int16_t tempDegreesCelsiusAvg;                                 /*!< Average temperature reading of temperature sensors specified in avgMask */
    uint16_t avgMask;                                              /*!< Bitmask of adi_adrv904x_DevTempSensorMask_e values indicating which temperature sensors are averaged in tempDegreesCelciusAvg */
} adi_adrv904x_DevTempData_t;)

#endif /* __ADRV904X_DEV_TEMP_TYPES_H__ */



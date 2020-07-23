/**
 * \file
 * \brief Type definitions for ADRV9001 device system configuration
 * \copyright Analog Devices Inc. 2019. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_DEVICESYSCONFIG_TYPES_H_
#define _ADI_ADRV9001_DEVICESYSCONFIG_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "adi_adrv9001_rxSettings_types.h"

#define ADI_ADRV9001_MAX_NUM_PLL 5
#define ADI_ADRV9001_NUM_RF_PLL  2

/**
* \brief Data structure to hold PLL modulus settings
*/
typedef struct adi_adrv9001_pllModulus
{
    uint32_t modulus[ADI_ADRV9001_MAX_NUM_PLL];  /*!< PLL modulus */
    uint32_t dmModulus[ADI_ADRV9001_NUM_RF_PLL]; /*!< RF PLL modulus in DM mode */
} adi_adrv9001_pllModulus_t;

typedef enum adi_adrv9001_DuplexMode
{
    ADI_ADRV9001_TDD_MODE = 0,
    ADI_ADRV9001_FDD_MODE = 1
} adi_adrv9001_DuplexMode_e;

/**
* \brief Data structure to hold Device system configuration
*/
typedef struct adi_adrv9001_DeviceSysConfig
{
    adi_adrv9001_DuplexMode_e duplexMode;
    uint8_t fhModeOn;
    uint8_t numDynamicProfile;             /*!< Number of dynamic Profile */
    uint8_t extMcsOn;                      /*!< External MCS On flag. 0 means off */
    adi_adrv9001_AdcType_e adcTypeMonitor; /*!< ADC type used in Monitor Mode */
    uint16_t pllLockTime_us;               /*!< Required lock time in microseconds for PLLs, based on ref_clk and loop bandwidth */
    adi_adrv9001_pllModulus_t pllModulus;  /*!< PLL modulus */
} adi_adrv9001_DeviceSysConfig_t;

#endif /* _ADI_ADRV9001_DEVICESYSCONFIG_TYPES_H_ */

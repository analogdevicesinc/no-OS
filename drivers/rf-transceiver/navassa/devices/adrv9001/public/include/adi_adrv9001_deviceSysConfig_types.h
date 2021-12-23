/**
 * \file
 * \brief Type definitions for ADRV9001 device system configuration
 * \copyright Analog Devices Inc. 2021. All rights reserved.
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
 *  \brief Enum of MCS Modes
 */
typedef enum adi_adrv9001_McsMode
{
    ADI_ADRV9001_MCSMODE_DISABLED = 0,              /*!< Multi Chip Synchronization disabled */
    ADI_ADRV9001_MCSMODE_ENABLED,                   /*!< Multi Chip Synchronization enabled */
    ADI_ADRV9001_MCSMODE_ENABLED_WITH_RFPLL_PHASE   /*!< Multi Chip Synchronization enabled with RFPLL phase */
} adi_adrv9001_McsMode_e;

/**
 *  \brief Enum of Dynamic Profile configuration values
 */
typedef enum adi_adrv9001_NumDynamicProfiles
{
    ADI_ADRV9001_NUM_DYNAMIC_PROFILES_DISABLED = 1, /*!< Dynamic profile switching disabled */
    ADI_ADRV9001_NUM_DYNAMIC_PROFILES_2,            /*!< Number of dynamic profiles = 2 */
    ADI_ADRV9001_NUM_DYNAMIC_PROFILES_3,            /*!< Number of dynamic profiles = 3 */
    ADI_ADRV9001_NUM_DYNAMIC_PROFILES_4,            /*!< Number of dynamic profiles = 4 */
    ADI_ADRV9001_NUM_DYNAMIC_PROFILES_5,            /*!< Number of dynamic profiles = 5 */
    ADI_ADRV9001_NUM_DYNAMIC_PROFILES_6             /*!< Number of dynamic profiles = 6 */
} adi_adrv9001_NumDynamicProfiles_e;

/**
* \brief Data structure to hold Device system configuration
*/
typedef struct adi_adrv9001_DeviceSysConfig
{
    adi_adrv9001_DuplexMode_e duplexMode;
    uint8_t fhModeOn;
    adi_adrv9001_NumDynamicProfiles_e numDynamicProfiles; /*!< Number of dynamic Profiles */
    adi_adrv9001_McsMode_e mcsMode;             /*!< Multi Chip Synchronization mode */
    adi_adrv9001_SsiType_e mcsInterfaceType;    /*!< Multi Chip Synchronization interface type */
    adi_adrv9001_AdcType_e adcTypeMonitor;      /*!< ADC type used in Monitor Mode */
    uint16_t pllLockTime_us;                    /*!< Required lock time in microseconds for PLLs, based on ref_clk and loop bandwidth */
    uint16_t pllPhaseSyncWait_us;               /*!< Worst case phase sync wait time in FH */
    adi_adrv9001_pllModulus_t pllModulus;       /*!< PLL modulus */
    bool warmBootEnable;                        /*!< Enable WarmBoot - Load initCal cefficients instead of running initCals */
} adi_adrv9001_DeviceSysConfig_t;

#endif /* _ADI_ADRV9001_DEVICESYSCONFIG_TYPES_H_ */

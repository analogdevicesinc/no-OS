/**
 * \file
 * \brief Contains ADRV9001 ARM data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_ARM_TYPES_H_
#define _ADI_ADRV9001_ARM_TYPES_H_

#include "adi_adrv9001_stream_types.h"
#include "adi_adrv9001_gpio_types.h"

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/**
* \brief Enumerated list of ARM System States.
*/
typedef enum adi_adrv9001_ArmSystemStates
{
    ADI_ADRV9001_ARM_SYSTEM_POWERUP,         /*!< Powerup State */
    ADI_ADRV9001_ARM_SYSTEM_NORMALMODE,      /*!< Normal TDD/FDD State */
    ADI_ADRV9001_ARM_SYSTEM_POWERSAVINGMODE, /*!< System Power Saving Mode */
    ADI_ADRV9001_ARM_SYSTEM_MCS              /*!< MCS State */
} adi_adrv9001_ArmSystemStates_e;

/**
 * \brief The MCS substates
 */
typedef enum adi_adrv9001_ArmMcsStates
{
    ADI_ADRV9001_ARMMCSSTATES_READY,      /*!< MCS Ready state */
    ADI_ADRV9001_ARMMCSSTATES_TRANSITION, /*!< MCS Transition state */
    ADI_ADRV9001_ARMMCSSTATES_DONE,       /*!< MCS Done state */
    ADI_ADRV9001_ARMMCSSTATES_RESERVED
} adi_adrv9001_ArmMcsStates_e;

/**
* \brief Enumerated list of ARM bootup states.
*/
typedef enum adi_adrv9001_ArmBootStates
{
    ADI_ADRV9001_ARM_BOOT_POWERUP                           = 0,    /*!< Used to put API in wait for ARM state */
    ADI_ADRV9001_ARM_BOOT_READY                             = 1,    /*!< ARM booted with no failure */
    ADI_ADRV9001_ARM_BOOT_FW_CHECKSUM_ERR                   = 2,    /*!< ARM firmware checksum error */
    ADI_ADRV9001_ARM_BOOT_EFUSE_DATA_ERR                    = 3,    /*!< Efuse data error */
    ADI_ADRV9001_ARM_BOOT_STATE_DATAMEM_ERR                 = 4,    /*!< ARM data memory error */
    ADI_ADRV9001_ARM_BOOT_DEVICE_PROFILE_CHECKSUM_ERR       = 5,    /*!< Device profile checksum error */
    ADI_ADRV9001_ARM_BOOT_CLKGEN_ERR                        = 6,    /*!< Bootup clkgen setup error */
    ADI_ADRV9001_ARM_BOOT_CLKSSI_ERR                        = 7,    /*!< Bootup SSI setup error */
    ADI_ADRV9001_ARM_BOOT_DEVICE_PROFILE_INIT_ERR           = 8,    /*!< Device profile init setup error */
    ADI_ADRV9001_ARM_BOOT_JTAG_BUILD_STATUS_READY           = 9,    /*!< JTAG build status ready indication */
    ADI_ADRV9001_ARM_BOOT_CLKLOGEN_ERR                      = 10,   /*!< Bootup clock LOGEN error */
    ADI_ADRV9001_ARM_BOOT_RXQECHW_ERR                       = 11,   /*!< Error initializing RxQEC hardware */
    ADI_ADRV9001_ARM_BOOT_HM_TIMER_ERR                      = 12,   /*!< Failed to create health monitor timers */
    ADI_ADRV9001_ARM_BOOT_ADC_RCAL_ERR                      = 13,   /*!< ADC RCAL error */
    ADI_ADRV9001_ARM_BOOT_STATE_ADC_CCAL_ERR                = 14,   /*!< ADC CCAL error */
    ADI_ADRV9001_ARM_BOOT_STATE_STREAM_RUNTIME_ERR          = 15,   /*!< Stream Run error */
} adi_adrv9001_ArmBootStates_e;

/**
* \brief Enumerated list of ARM image build options.
*/
typedef enum adi_adrv9001_ArmBuildType
{
    ADI_ADRV9001_ARMBUILD_RELEASE = 0,
    ADI_ADRV9001_ARMBUILD_DEBUG = 1,
    ADI_ADRV9001_ARMBUILD_TESTOBJ = 2
} adi_adrv9001_ArmBuildType_e;

/**
 * \brief Enumerated list of ARM Clock Type options.
 */
typedef enum adi_adrv9001_ArmClockType
{
    ADI_ADRV9001_ARMCLOCK_DATAPATH = 0,
    ADI_ADRV9001_ARMCLOCK_MAX
} adi_adrv9001_ArmClockType_e;

/**
 * \brief ARM memory SPI write mode
 */
typedef enum adi_adrv9001_ArmSingleSpiWriteMode
{
    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4,    /*!< 4 bytes in a single SPI write in standard mode */
                                                                /*!< Only one write operation (4 bytes per write operation) is performed regardless of the state of chip select */
    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252,  /*!< 252 bytes in a single SPI write in standard mode */
                                                                /*!< Only one write operation (up to 252 bytes per write operation) is performed regardless of the state of chip select */
    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4,   /*!< 4 bytes in a single SPI write in streaming mode */
                                                                /*!< Four write operation (but only 4 bytes per write operation) is performed in one chip select */
}adi_adrv9001_ArmSingleSpiWriteMode_e;

/**
* \brief Data structure to hold ARM version information
*/
typedef struct adi_adrv9001_ArmVersion
{
    uint8_t majorVer;
    uint8_t minorVer;
    uint8_t maintVer;
    uint8_t rcVer;
    adi_adrv9001_ArmBuildType_e armBuildType;
} adi_adrv9001_ArmVersion_t;

/**
* \brief Data structure to hold ARM image build and runtime calculated checksums.
*/
typedef struct adi_adrv9001_Checksum
{
    uint32_t buildChecksum;
    uint32_t runChecksum;
} adi_adrv9001_Checksum_t;

/**
* \brief Data structure to hold a table of additional ARM checksums
*/
typedef struct adi_adrv9001_ChecksumTable
{
    adi_adrv9001_Checksum_t fwCheckSums;
    adi_adrv9001_Checksum_t streamsCheckSum[ADRV9001_MAX_NUM_STREAM];
    adi_adrv9001_Checksum_t deviceProfileCheckSum;
    adi_adrv9001_Checksum_t pfirProfileCheckSum;
    uint32_t fwError;
    uint32_t fwErrorCode;
} adi_adrv9001_ChecksumTable_t;

#endif /* _ADI_ADRV9001_ARM_TYPES_H_ */

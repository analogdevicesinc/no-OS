/**
 * \file
 * \brief Contains ADRV910X ARM data types
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910X_ARM_TYPES_H_
#define _ADI_ADRV910X_ARM_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/**
* \brief Enumerated list of ARM System States.
*/
typedef enum adi_adrv910x_ArmSystemStates
{
    ADI_ADRV910X_ARM_SYSTEM_POWERUP,         /*!< Powerup State */
    ADI_ADRV910X_ARM_SYSTEM_NORMALMODE,      /*!< Normal TDD/FDD State */
    ADI_ADRV910X_ARM_SYSTEM_POWERSAVINGMODE, /*!< System Power Saving Mode */
    ADI_ADRV910X_ARM_SYSTEM_MCS              /*!< MCS State */
} adi_adrv910x_ArmSystemStates_e;

/**
 * \brief The MCS substates
 */
typedef enum adi_adrv910x_ArmMcsStates
{
    ADI_ADRV910X_ARMMCSSTATES_READY,      /*!< MCS Ready state */
    ADI_ADRV910X_ARMMCSSTATES_TRANSITION, /*!< MCS Transition state */
    ADI_ADRV910X_ARMMCSSTATES_DONE,       /*!< MCS Done state */
    ADI_ADRV910X_ARMMCSSTATES_RESERVED
} adi_adrv910x_ArmMcsStates_e;

/**
* \brief Enumerated list of ARM bootup states.
*/
typedef enum adi_adrv910x_ArmBootStates
{
    ADI_ADRV910X_ARM_BOOT_POWERUP                           = 0,    /*!< Used to put API in wait for ARM state */
    ADI_ADRV910X_ARM_BOOT_READY                             = 1,    /*!< ARM booted with no failure */
    ADI_ADRV910X_ARM_BOOT_FW_CHECKSUM_ERR                   = 2,    /*!< ARM firmware checksum error */
    ADI_ADRV910X_ARM_BOOT_EFUSE_DATA_ERR                    = 3,    /*!< Efuse data error */
    ADI_ADRV910X_ARM_BOOT_STATE_DATAMEM_ERR                 = 4,    /*!< ARM data memory error */
    ADI_ADRV910X_ARM_BOOT_DEVICE_PROFILE_CHECKSUM_ERR       = 5,    /*!< Device profile checksum error */
    ADI_ADRV910X_ARM_BOOT_CLKGEN_ERR                        = 6,    /*!< Bootup clkgen setup error */
    ADI_ADRV910X_ARM_BOOT_CLKSSI_ERR                        = 7,    /*!< Bootup SSI setup error */
    ADI_ADRV910X_ARM_BOOT_DEVICE_PROFILE_INIT_ERR           = 8,    /*!< Device profile init setup error */
    ADI_ADRV910X_ARM_BOOT_JTAG_BUILD_STATUS_READY           = 9,    /*!< JTAG build status ready indication */
    ADI_ADRV910X_ARM_BOOT_CLKLOGEN_ERR                      = 10,   /*!< Bootup clock LOGEN error */
    ADI_ADRV910X_ARM_BOOT_RXQECHW_ERR                       = 11,   /*!< Error initializing RxQEC hardware */
    ADI_ADRV910X_ARM_BOOT_HM_TIMER_ERR                      = 12,   /*!< Failed to create health monitor timers */
    ADI_ADRV910X_ARM_BOOT_ADC_RCAL_ERR                      = 13,   /*!< ADC RCAL error */
    ADI_ADRV910X_ARM_BOOT_STATE_ADC_CCAL_ERR                = 14,   /*!< ADC CCAL error */
    ADI_ADRV910X_ARM_BOOT_STATE_STREAM_RUNTIME_ERR          = 15,   /*!< Stream Run error */
} adi_adrv910x_ArmBootStates_e;

/**
* \brief Enumerated list of ARM image build options.
*/
typedef enum adi_adrv910x_ArmBuildType
{
    ADI_ADRV910X_ARMBUILD_RELEASE = 0,
    ADI_ADRV910X_ARMBUILD_DEBUG = 1,
    ADI_ADRV910X_ARMBUILD_TESTOBJ = 2
} adi_adrv910x_ArmBuildType_e;

/**
 * \brief Enumerated list of ARM Clock Type options.
 */
typedef enum adi_adrv910x_ArmClockType
{
    ADI_ADRV910X_ARMCLOCK_DATAPATH = 0,
    ADI_ADRV910X_ARMCLOCK_MAX
} adi_adrv910x_ArmClockType_e;

/**
 * \brief ARM memory SPI write mode
 */
typedef enum adi_adrv910x_ArmSingleSpiWriteMode
{	
    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, /*!< Write 4 Bytes to ARM Memory. A SPI Operation (CSB-Low-to-High, 1R/W, 15 ADDR and 8 DATA bits) for each byte */
    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_CACHED_BYTES_N, /*!< Cache SPI bytes before sending to SPI buffer. Each SPI Operation (CSB-Low-to-High) writes up to N bytes (1R/W, 15 ADDR and 8 DATA bits). N is dictated by platform SPI Buffer size */
    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4,   /*!< Each SPI Operation (CSB-Low-to-High) writes 4 data bytes, Streaming 4 bytes at a time (1R/W, 15 ADDR and 32 DATA bits) */
	ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_1,   /*!< Each SPI Operation (CSB-Low-to-High) writes N data bytes, Streaming 1 byte at a time (1R/W, 15 ADDR and N*8 DATA bits) N is dictated by platform SPI Buffer size */ 
	
}adi_adrv910x_ArmSingleSpiWriteMode_e;

/**
* \brief Data structure to hold ARM version information
*/
typedef struct adi_adrv910x_ArmVersion
{
    uint8_t majorVer;
    uint8_t minorVer;
    uint8_t maintVer;
    uint8_t rcVer;
    adi_adrv910x_ArmBuildType_e armBuildType;
} adi_adrv910x_ArmVersion_t;

/**
* \brief Data structure to hold ARM image build and runtime calculated checksums.
*/
typedef struct adi_adrv910x_Checksum
{
    uint32_t buildChecksum;
    uint32_t runChecksum;
} adi_adrv910x_Checksum_t;

/**
* \brief Data structure to hold a table of additional ARM checksums
*/
typedef struct adi_adrv910x_ChecksumTable
{
    adi_adrv910x_Checksum_t fwCheckSums;
    adi_adrv910x_Checksum_t deviceProfileCheckSum;
    adi_adrv910x_Checksum_t pfirProfileCheckSum;
    uint32_t fwError;
    uint32_t fwErrorCode;
} adi_adrv910x_ChecksumTable_t;

/**
 * \brief Union to hold profiles for sending to PS1/PS2
 */
typedef struct adi_adrv910x_Profiles
{
	deviceProfile_t profilePS1;
	deviceProfilePS2_t profilePS2;
} adi_adrv910x_Profiles_t;

#endif /* _ADI_ADRV910X_ARM_TYPES_H_ */

/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_arm_types.h
 * \brief Contains ADRV9025 ARM data types
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_ARM_TYPES_H_
#define _ADI_ADRV9025_ARM_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif

#ifdef __cplusplus
extern
"C"
 {
#endif

/**
* \brief Enumerated list of ARM bootup states.
*/
typedef enum adi_adrv9025_ArmBootStates
{
    ADI_ADRV9025_ARMERR_BOOTUP_TIMEOUT_ERROR, /*!< Timed out waiting for ARM bootup to happen*/
    ADI_ADRV9025_ARMERR_BOOTUP_IDLE,          /*!< ARM in IDLE mode after bootup*/
    ADI_ADRV9025_ARMERR_BOOTUP_RADIO_ON,      /*!< ARM in RADIO_ON mode after bootup*/
    ADI_ADRV9025_ARMERR_BOOTUP_PROFILE_ERROR, /*!< ARM Profile error during bootup*/
    ADI_ADRV9025_ARMERR_BOOTUP_UNKNOWN_ERROR  /*!< ARM unknown error during bootup*/
} adi_adrv9025_ArmBootStates_e;

/**
* \brief Enumerated list of ARM image build options.
*/
typedef enum adi_adrv9025_ArmBuildType
{
    ADI_ADRV9025_ARMBUILD_RELEASE   = 0,
    ADI_ADRV9025_ARMBUILD_DEBUG     = 1,
    ADI_ADRV9025_ARMBUILD_TESTOBJ   = 2
} adi_adrv9025_ArmBuildType_e;

/**
* \brief Data structure to ARM version information
*/
typedef struct adi_adrv9025_ArmVersion
{
    uint8_t majorVer;
    uint8_t minorVer;
    uint8_t maintVer;
    uint16_t rcVer;
    adi_adrv9025_ArmBuildType_e armBuildType;
} adi_adrv9025_ArmVersion_t;

/**
* \brief Data structure to hold ARM image build and runtime calculated checksums.
*/
typedef struct adi_adrv9025_CpuChecksum adi_adrv9025_Checksum_t;

/**
* \brief Data structure to hold a table of additional ARM checksums
*/
typedef struct adi_adrv9025_CpuChecksumTable adi_adrv9025_ChecksumTable_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_ARM_TYPES_H_ */

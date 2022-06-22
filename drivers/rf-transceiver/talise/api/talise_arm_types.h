// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_arm_types.h
 * \brief Contains Talise ARM data types
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_ARM_TYPES_H_
#define TALISE_ARM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	TALAPI_ARMERR_BOOTUP_TIMEOUT_ERROR, /*!< Timed out waiting for ARM bootup to happen*/
	TALAPI_ARMERR_BOOTUP_IDLE,          /*!< ARM in IDLE mode after bootup*/
	TALAPI_ARMERR_BOOTUP_RADIO_ON,      /*!< ARM in RADIO_ON mode after bootup*/
	TALAPI_ARMERR_BOOTUP_PROFILE_ERROR, /*!< ARM Profile error during bootup*/
	TALAPI_ARMERR_BOOTUP_UNKNOWN_ERROR  /*!< ARM unknown error during bootup*/
} talApiArmErr_t;

typedef enum {
	TAL_ARM_BUILD_DEBUG,                /*!< ARM binary is Debug Object*/
	TAL_ARM_BUILD_TEST_OBJECT,          /*!< ARM binary is Test Object*/
	TAL_ARM_BUILD_RELEASE               /*!< ARM binary is Release*/
} taliseArmBuildType_t;

typedef struct {
	uint8_t majorVer;                   /*!< The ARM Major revision*/
	uint8_t minorVer;                   /*!< The ARM Minor revision*/
	uint8_t rcVer;                      /*!< The release candidate version (build number)*/
	taliseArmBuildType_t buildType;        /*!< What type of ARM binary build*/
} taliseArmVersionInfo_t;

#ifdef __cplusplus
}
#endif

#endif /* TALISE_ARM_TYPES_H_ */

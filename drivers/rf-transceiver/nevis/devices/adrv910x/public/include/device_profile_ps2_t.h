/*!
 * @file
 * @brief Macros for bit-field reference access functions.
 *
 * This file was auto-generated. Do not make local changes to this file.
 */

/*
 * Copyright (c) 2023 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the terms of the "LICENSE.md" file in the root directory.
 */

#ifndef __DEVICE_PROFILE_PS2_T_HEADER__
#define __DEVICE_PROFILE_PS2_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_version_t.h"

/*!< PS1 DMR Sync Detection Data location in PS1 Memory */
#define ADI_DMR_SYNC_DETECT_DATA_ADDR	(0x01047FE8)

/*!
* Profile information provided by BBIC at init via CONFIG command
*
* Profile struct holds PS2 arm clock in Hz
*
*/

ADI_NEVIS_PACK_START
typedef struct
{
    profileVersion_t ver;                  /*!< Version information */
    uint32_t         ps2ArmClk;            /*!< PS2 clock proveded by PS1 in Hz */

    uint32_t         checksum;             /*!< Device profile checksum */
} deviceProfilePS2_t; 
ADI_NEVIS_PACK_FINISH

#endif /*! __DEVICE_PROFILE_PS2_T_HEADER__ */

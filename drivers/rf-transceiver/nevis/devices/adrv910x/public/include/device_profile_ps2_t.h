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

#include <stdint.h>
#include "adi_device_profile_pack.h"
#include "device_profile_version_t.h"


/*!< PS1 DMR Sync Detection Data location in PS1 Memory */
#define ADI_DMR_SYNC_DETECT_DATA_ADDR	(0x01047FE8)

ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t duplexMode;
} sysConfigPS2_t;
ADI_NEVIS_PACK_FINISH

/*! PS2-specific subset of rxConfig_t (device_profile_top_t.h);
 *  these are the only parameters required by PS2 */
ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t outputSignalingSel;
    uint32_t gainTableType;
    uint32_t rxOutputRate;                     /*!< RX output rate in Hz */
} rxConfigPS2_t;
ADI_NEVIS_PACK_FINISH

/*! Shared config type for ORX, ILB, and ELB channels */
ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t rxOutputRate;                     /*!< Output rate in Hz */
} obsRxConfigPS2_t;
ADI_NEVIS_PACK_FINISH

/*! PS2-specific subset of rxNbAgcHighThresholdSettings_t (device_profile_rx_gaincontrol_t.h);
 *  these are the only parameters required by PS2, so fields are narrowed to 8-bit thresholds */
ADI_NEVIS_PACK_START
typedef struct
{
    uint8_t ApdPeakOverloadThresh;
    uint8_t ApdPeakUnderloadThresh;
    uint8_t Hb2PeakOverloadThresh;
    uint8_t Hb2PeakUnderloadThresh;
    uint8_t Hb2SecondaryUpperThresh;
} rxNbAgcHighThresholdSettingsPS2_t;
ADI_NEVIS_PACK_FINISH

/*! PS2-specific subset of rx_gainControl_t (device_profile_rx_gaincontrol_t.h);
 *  rxNbApdLegacyMode is intentionally not supported/needed by PS2 */
ADI_NEVIS_PACK_START
typedef struct
{
    uint8_t rxNbAgcHighThresholdModeEnable;
    rxNbAgcHighThresholdSettingsPS2_t agcHighThresholdSettings;
} rxGainControlPS2_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    rxGainControlPS2_t gainControl;
} rxNbConfigPS2_t;
ADI_NEVIS_PACK_FINISH

/*! PS2-specific subset of txConfig_t (device_profile_top_t.h);
 *  these are the only parameters required by PS2 */
ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t outputSignalingSel;
    uint32_t txInputRate;                      /*!< TX input rate in Hz */
} txConfigPS2_t;
ADI_NEVIS_PACK_FINISH

/*!
* Profile information provided by BBIC at init via CONFIG command
*
* Profile struct holds PS2 configuration data
*
*/

ADI_NEVIS_PACK_START
typedef struct
{
    profileVersion_t ver;                      /*!< Version information */
    uint32_t         ps2ArmClk;                /*!< PS2 clock provided by PS1 in Hz */
    uint32_t         devClock_Hz;              /*!< Nevis Device clock */
    uint32_t         ps1ArmClk;                /*!< Clock value used in PS1 and Monitor */
    uint32_t         ps1ArmClkDiv;             /*!< Clock divider value used in PS1 and Monitor */
    sysConfigPS2_t       sysConfig;
    rxConfigPS2_t        rxConfig[2];              /*!< [0] = RX1, [1] = RX2 */
    obsRxConfigPS2_t     orxConfig[2];             /*!< [0] = ORX1, [1] = ORX2 */
    obsRxConfigPS2_t     ilbConfig[2];             /*!< [0] = ILB1, [1] = ILB2 */
    obsRxConfigPS2_t     elbConfig[2];             /*!< [0] = ELB1, [1] = ELB2 */
    rxNbConfigPS2_t      rxNbConfig;
    txConfigPS2_t        txConfig[2];              /*!< [0] = TX1, [1] = TX2 */
    uint32_t         checksum;                 /*!< Device profile checksum */
} deviceProfilePS2_t;
ADI_NEVIS_PACK_FINISH

#endif /*! __DEVICE_PROFILE_PS2_T_HEADER__ */

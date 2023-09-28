/* SPDX-License-Identifier: GPL-2.0 */
/*!
* \file adi_adrv9025_utilities_types.h
* \brief Contains ADRV9025 API utility initialization type definitions
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_UTILITIES_TYPES_H_
#define _ADI_ADRV9025_UTILITIES_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include "adi_adrv9025_types.h"
#include "adi_adrv9025_tx_types.h"
#include "adi_adrv9025_rx_types.h"
#include "adi_adrv9025_data_interface_types.h"
#include "adi_common_error_types.h"
#include "adi_adrv9025_radioctrl_types.h"
#include "adi_adrv9025_cals_types.h"
#include "adi_adrv9025_arm_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
*  \brief Enum to select AuxDAC compensation mode
*/
typedef enum adi_adrv9025_AuxDacCompensationMode
{
    ADI_ADRV9025_NO_COMPENSATION = 0x01,            /*!< No compensation is applied, requested DAC word is written to device directly */
    ADI_ADRV9025_NO_TEMP_COMPENSATION = 0x02,       /*!< No temp compensation is applied, only innaccuracy compensation is applied */
    ADI_ADRV9025_COMPENSATE_WITH_INT_TEMP = 0x03,   /*!< Both inaccuracy and temp compensation is enabled. On-board temperature sensor measurement is used for temp compensation*/
    ADI_ADRV9025_COMPENSATE_WITH_EXT_TEMP = 0x04    /*!< Both inaccuracy and temp compensation is enabled. User supplied temperature data is used for temp compensation*/
} adi_adrv9025_AuxDacCompensationMode_e;

/**
* \brief Data structure to hold Radio Ctrl Utility Init structures
*/
typedef struct adi_adrv9025_RadioctrlInit_s
{
    adi_adrv9025_RadioCtrlModeCfg_t     radioCtrlModeCfg; /*!< Configuration to hold Radio Ctrl Mode (Pin vs SPI) initialization */
    adi_adrv9025_TxToOrxMappingConfig_t txToOrxMapping;   /*!< Configuration to hold Tx to ORx mapping config on initialization (Reqd for Ext LOL cal)*/
    adi_adrv9025_StreamGpioPinCfg_t     streamGpioCfg;    /*!< Configuration to hold GPIO mappings for the 4 Stream Trigger pin inputs to ADRV9025 */
    uint64_t                            lo1PllFreq_Hz;    /*!< Configuration to hold LO1 PLL frequency on init */
    uint64_t                            lo2PllFreq_Hz;    /*!< Configuration to hold LO2 PLL frequency on init */
    uint64_t                            auxPllFreq_Hz;    /*!< Configuration to hold Aux PLL frequency on init */
} adi_adrv9025_RadioctrlInit_t;

/**
* \brief Data structure to hold ADRV9025 AuxDAC Value to be compensated
*/
typedef struct adi_adrv9025_AuxDacValueToCompensate
{
    adi_adrv9025_AuxDacVoltage_t          auxDacValue;                 /* User supplied AuxDac Value */
    adi_adrv9025_AuxDacCompensationMode_e auxDacValueCompensationMode; /* AuxDac Compensation Mode */
    int16_t                               extTemperatureDegC;          /* User supplied temperature [degC], only used when auxDacValueCompensationMode = ADI_ADRV9025_COMPENSATE_WITH_EXT_TEMP  */
} adi_adrv9025_AuxDacValueToCompensate_t;

/**
* \brief Data structure to hold Utility Init structures
*/
typedef struct adi_adrv9025_PostMcsInit_s
{
    adi_adrv9025_RadioctrlInit_t radioCtrlInit;              /*!< Radioctrl Initialization configuration */
    adi_adrv9025_InitCals_t      initCals;                   /*!< Cals Initialization configuration */
    char                         calRsltsFile[ADI_MAX_PATH]; /*!< File containing cal. results for warm-boot */
    uint8_t                      checkFwVer;                 /*!< Before warm-boot check f/w ver in cal. rslts matches current f/w */
} adi_adrv9025_PostMcsInit_t;

/**
* \brief Data structure to hold Rx Gain Table files to load
*/
typedef struct adi_adrv9025_RxGainTableFile
{
    char     rxGainTableCsvFileName[128]; /*!< Rx Gain Table file name */
    uint32_t rxChannelMask;               /*!< Rx Channels to be initialized with the given Rx Gain Table file */
} adi_adrv9025_RxGainTableFile_t;

/**
* \brief Data structure to hold Tx Atten Table files to load
*/
typedef struct adi_adrv9025_TxAttenTableFile
{
    char     txAttenTableCsvFileName[128]; /*!< Tx Atten Table file name */
    uint32_t txChannelMask;                /*!< Tx Channels to be initialized with the given Tx Atten Table file */
} adi_adrv9025_TxAttenTableFile_t;

/**
* \brief Data structure to hold platform file addresses on the SD card
*/
typedef struct adi_adrv9025_PlatformFiles
{
    char                            armImageFile[168];
    char                            streamImageFile[128];
    adi_adrv9025_RxGainTableFile_t  rxGainTableFileArr[ADI_ADRV9025_MAX_RX_ONLY + ADI_ADRV9025_MAX_ORX_ONLY];
    uint8_t                         rxGainTableFileArrSize;
    adi_adrv9025_TxAttenTableFile_t txAttenTableFileArr[ADI_ADRV9025_MAX_TXCHANNELS];
    uint8_t                         txAttenTableFileArrSize;
} adi_adrv9025_PlatformFiles_t;

/**
* \brief Data Structure to hold stream settings
*/
typedef struct adi_adrv9025_StreamSettings
{
    uint8_t                         rxM;  /*!< If link sharing, M value for Rx channels */
    uint8_t                         rxS;  /*!< If link sharing, S value for Rx channels */
    uint8_t                         orxM; /*!< If link sharing, M value for ORx channels */
    uint8_t                         orxS; /*!< If link sharing, S value for ORx channels */
    adi_adrv9025_AdcSampleXbarCfg_t rxSampleXBar;
    adi_adrv9025_AdcSampleXbarCfg_t orxSampleXBar;
    uint8_t                         linkSharing; /*!<1 if link sharing. 0 otherwise */
} adi_adrv9025_StreamSettings_t;

/**
* \brief Data Structure to hold stream settings
*/
typedef struct adi_adrv9025_PinModeSettings
{
    adi_adrv9025_AdcSampleXbarCfg_t sampleXBar[3];
} adi_adrv9025_PinModeSettings_t;

/**
* \brief Data Structure to API/ARM/Stream versions
*/
typedef struct adi_adrv9025_ApiArmStreamVersions
{
    adi_adrv9025_ApiVersion_t apiVersion;
    adi_adrv9025_ArmVersion_t armVersion;
    adi_adrv9025_StreamVersion_t streamVersion;
} adi_adrv9025_ApiArmStreamVersions_t;

#ifdef __cplusplus
}
#endif

#endif

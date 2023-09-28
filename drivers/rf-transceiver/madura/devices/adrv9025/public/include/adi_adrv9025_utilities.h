/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_utilities.h
* \brief Contains ADRV9025 utility functions to load ARM binaries
*        load stream binaries, load Rx Gain Table, load Tx Atten Table.
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_UTILITIES_H_
#define _ADI_ADRV9025_UTILITIES_H_

#include "adi_adrv9025_utilities_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Macro to restore wrOnly bit in case there is a failure within PreMcsInit_v2
*
* \param commonDev Device variable having wrOnly bit to be restored
* \param wrOnlyRestore wrOnly bit value to restore
* \param recoveryAction Action to be tested against ADI_COMMON_ACT_NO_ACTION
*/
#define ADI_ADRV9025_PREMCS_RESTORE_WRONLY(commonDev, wrOnlyRestore, recoveryAction) \
    if(recoveryAction != ADI_COMMON_ACT_NO_ACTION){ commonDev.cacheInfo.wrOnly = wrOnlyRestore; }

/**
* \brief This utility function loads ADRV9025 ARM binary image through ArmImageWrite() API
*
* This function reads the ARM binary file from a specified location
* (typically in an SD card) and programs the ARM with the specified binary image
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV9025_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv9025_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param armImagePath is a string containing absolute path of the ARM image to be programmed
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ArmImageLoad(adi_adrv9025_Device_t* device,
                                  const char*            armImagePath);

/**
* \brief This utility function loads ADRV9025 CPU FW image through ArmImageWrite() API
*
* This function reads the CPU FW file from a specified location
* (typically in an SD card) and programs the FW with the specified binary image
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV9025_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv9025_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fwImagePath is a string containing absolute path of the FW image to be programmed
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuImageLoad(adi_adrv9025_Device_t* device,
                                  const char*            fwImagePath);

/**
* \brief This utility function loads ADRV9025 Stream binary image through StreamImageWrite() API
*
* This function reads the ADRV9025 stream binary file from a specified location
* (typically in an SD card) and programs the stream processor with the specified binary image
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV9025_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv9025_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available.
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param streamImagePath is a string containing absolute path of the stream image to be programmed
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_StreamImageLoad(adi_adrv9025_Device_t* device,
                                     const char*            streamImagePath);

/**
* \brief    This function performs a Sequential AHB Memory Read
*
* WARNING:  Caller is responsible for allocating the correct buffer size for the read
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device         Pointer to the ADRV9025 device data structure containing settings
* \param start_address  Start Address of the Sequential Memory Read
* \param buffer         Buffer for Data Read
* \param size           Number of Memory Locations to be Read
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG         Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM        Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE    Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION              Function completed successfully, no action required
*/
int32_t adi_adrv9025_DmaMemoryRead(adi_adrv9025_Device_t* const device,
                                   const uint32_t               start_address,
                                   uint8_t                      buffer[],
                                   const uint32_t               size);

/**
* \brief This utility function loads ADRV9025 Rx Gain table file in csv format to ADRV9025 gain table SRAM
*
* This function reads the ADRV9025 Rx Gain Table file in csv format from a specified location
* (typically in an SD card) and programs the ADRV9025 SRAM with the gain table for the requested channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param rxGainTablePath is a string containing path to the Rx gain table in csv format to be programmed
* \param rxChannelMask Rx channels to be programmed. Valid masks include
*        ADI_ADRV9025_RX1 - ADI_ADRV9025_RX4, ADI_ADRV9025_ORX1 - ADI_ADRV9025_ORX4.
*
* Rx Gain Table columns should be arranged in the following order
*
*  --------------------------------------------------------------------------------------------------------
*  | Gain Index | FE Control Word | TIA Control | ADC Control | Ext Control | Phase Offset | Digital Gain |
*  --------------------------------------------------------------------------------------------------------
*
* Rx Gain Indices should be arranged in ascending order for row entries starting with the lowest gain index and
* and progressing to highest gain Index
*
* Eg: If the gain table contains entries from index 192 to index 255, the table has to be arranged such that
*     first line should contain row entries for index 192 and the last line should contain row entries to index 255
*     with a total of 64 entries.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableLoad(adi_adrv9025_Device_t* device,
                                     const char*            rxGainTablePath,
                                     uint32_t               rxChannelMask);

/**
* \brief This utility function loads ADRV9025 Tx Attenuation table file in csv format to ADRV9025 Tx Atten table SRAM
*
* This function reads the ADRV9025 Tx Atten Table file in csv format from a specified location
* (typically in an SD card) and programs the ADRV9025 SRAM with the gain table for the requested Tx channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param txAttenTablePath is a string containing path to the Tx atten table in csv format to be programmed
* \param txChannelMask Tx channels to be programmed. Valid masks include
*        ADI_ADRV9025_TX1 - ADI_ADRV9025_TX4.
*
* Tx Atten Table columns should be arranged in the following order
*
*  ------------------------------------------------
*  | Tx Atten Index | Tx Atten Hp | Tx Atten Mult |
*  ------------------------------------------------
*
* Rx Gain Indices should be arranged in ascending order for row entries starting with the lowest tx atten index and
* and progressing to highest tx atten Index (Eg: Starting from index 0 and progressing to index 1023)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenTableLoad(adi_adrv9025_Device_t* device,
                                      const char*            txAttenTablePath,
                                      uint32_t               txChannelMask);

/**
* \brief This utility function executes the Pre-MCS init sequence
*
* This function executes the following set of sub-functions required to be
* complete before sending sysref pulses for multi chip sync
* -> Reset Hardware
* -> Setup SPI controller, master bias, enable pin pads, Load PFIRs (adi_adrv9025_Initialize)
* -> Load ARM binary image from a .bin file
* -> Load Stream binary image from a .bin file
* -> Load Requested Rx gain tables from .csv files for initialized channels
* -> Load Reqeuested Tx atten tables from .csv files for initialized channels
* -> Load ARM profile
* -> Load ADC profile
* -> ARM bootup
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param init Pointer to ADRV9025 init settings structure
* \param armImagePath is a string containing absolute path of the arm image to be programmed
* \param streamImagePath is a string containing absolute path of the stream image to be programmed
* \param rxGainTableFileArr Channelized array of strings which hold the path to Rx gain table csv file
* \param rxGainTableFileArrSize Size of rxGainTableFileArr array holding list of paths to rx gain tables
* \param txAttenTableFileArr Channelized array of strings which hold the path to Tx atten table csv file
* \param txAttenTableFileArrSize Size of txAttenTableFileArr array holding list of paths to tx atten tables
*
* Rx Gain Table columns should be arranged in the following order
*
*  --------------------------------------------------------------------------------------------------------
*  | Gain Index | FE Control Word | TIA Control | ADC Control | Ext Control | Phase Offset | Digital Gain |
*  --------------------------------------------------------------------------------------------------------
*
* Rx Gain Indices should be arranged in ascending order for row entries starting with the lowest gain index and
* and progressing to highest gain Index
*
* Eg: If the gain table contains entries from index 192 to index 255, the table has to be arranged such that
*     first line should contain row entries for index 192 and the last line should contain row entries to index 255
*     with a total of 64 entries.
*
* Tx Atten Table columns should be arranged in the following order
*
*  ------------------------------------------------
*  | Tx Atten Index | Tx Atten Hp | Tx Atten Mult |
*  ------------------------------------------------
*
* Tx Atten Indices should be arranged in ascending order for row entries starting with the lowest tx atten index and
* and progressing to highest tx atten Index (Eg: Starting from index 0 and progressing to index 1023)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PreMcsInit_v2(adi_adrv9025_Device_t*          device,
                                   adi_adrv9025_Init_t*            init,
                                   const char*                     armImagePath,
                                   const char*                     streamImagePath,
                                   adi_adrv9025_RxGainTableFile_t  rxGainTableFileArr[],
                                   uint8_t                         rxGainTableFileArrSize,
                                   adi_adrv9025_TxAttenTableFile_t txAttenTableFileArr[],
                                   uint8_t                         txAttenTableFileArrSize);

/**
* \brief This utility function executes the non-broadcastable part of
*        Pre-MCS init sequence
*
* It should be called after adi_adrv9025_PreMcsInit_v2 and before
* adi_adrv9025_MultichipSyncSet. This function executes the following
* set of sub-functions required to be completed before sending sysref
* pulses for multi chip sync.
* -> Configure Rx Data Formatter
* -> Check Arm Status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param init Pointer to ADRV9025 init settings structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PreMcsInit_NonBroadCast(adi_adrv9025_Device_t* device,
                                             adi_adrv9025_Init_t*   init);

/**
* \brief This utility function mostly performs initialization at the feature level
*
* This function attempts to place individual features into a required init state
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param utilityInit Pointer to ADRV9025 utility init settings structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PostMcsInit(adi_adrv9025_Device_t*      device,
                                 adi_adrv9025_PostMcsInit_t* utilityInit);

/**
* \brief This utility function initialize the ADC Profiles
*
* The ADC profiles is initialize using fix profile with ADC bandwidth and ADC clock.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param init Pointer to ADRV9025 init settings structure
* \param adcProfile Pointer to ADRV9025 ADC profile settings structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AdcProfilesInit(adi_adrv9025_Device_t*      device,
                                     adi_adrv9025_Init_t*        init,
                                     adi_adrv9025_AdcProfiles_t* adcProfile);

/**
* \brief This utility function edits the requested DAC word to compensate inaccuracies and
*        temperature effects and sets the updated DAC word. Compensation has best accuracy
*        between 0.25V - 1.5V outputs. Function has ADI_COMMON_LOG_API level logging. Logging
*        level may be decreased prior to this function call for faster operation.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param auxDacSet Pointer to Aux Dac Value structure consisting of AuxDAC word, compensation mode and external temperature
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AuxDacCompensatedSet(adi_adrv9025_Device_t*                  device,
                                          adi_adrv9025_AuxDacValueToCompensate_t* auxDacSet);

/**
* \brief Utility function that will initialize an ADRV9025 init struct
*
* This reads the configuration file to scan for ADRV9025 init settings.
*
* \dep_begin
* \dep{NONE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param configFile The absolute filepath of the Configuration File
* \param init Pointer to the ADRV9025 init device structure to initialize
*
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no
*  action required
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter
*  check
*/
int32_t adi_adrv9025_ConfigFileLoad(adi_adrv9025_Device_t* device,
                                    const char*            configFile,
                                    adi_adrv9025_Init_t*   init);

/**
* \brief Utility function that will initialize an ADRV9025 Utility Postmcs init struct
*
*
* This reads the utility init file to scan for ADRV9025 Utility Init settings.
*
* \dep_begin
* \dep{NONE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param utilInitFile The absolute filepath of the Configuration File
* \param utilInit Pointer to the ADRV9025 utility init structure to initialize
*
* Example json file format as follows:
*   {
*     "radioCtrlInit": {
*       "radioCtrlModeCfg": {
*         "txRadioCtrlModeCfg": {
*           "txEnableMode": 0,
*           "txChannelMask": 15
*         },
*         "rxRadioCtrlModeCfg": {
*           "rxEnableMode": 0,
*           "rxChannelMask": 1023
*         },
*         "orxRadioCtrlModeCfg": {
*           "orxEnableMode": 0,
*           "orxPinSelectSettlingDelay_armClkCycles": 0,
*           "singleChannel1PinModeOrxSel": 0,
*           "singleChannel2PinModeLowOrxSel": 0,
*           "singleChannel2PinModeHighOrxSel": 0,
*           "dualChannel2PinModeOrxSel": 0
*         }
*       },
*       "txToOrxMapping": {
*         "orx1Map": 2,
*         "orx2Map": 3,
*         "orx3Map": 2,
*         "orx4Map": 3
*       },
*       "streamGpioCfg": {
*         "streamGpInput0": 19,
*         "streamGpInput1": 19,
*         "streamGpInput2": 19,
*         "streamGpInput3": 19,
*         "streamGpInput4": 19,
*         "streamGpInput5": 19,
*         "streamGpInput6": 19,
*         "streamGpInput7": 19,
*         "streamGpInput8": 19,
*         "streamGpInput9": 19,
*         "streamGpInput10": 19,
*         "streamGpInput11": 19,
*         "streamGpInput12": 19,
*         "streamGpInput13": 19,
*         "streamGpInput14": 19,
*         "streamGpInput15": 19
*       },
*       "lo1PllFreq_Hz": 3500000000,
*       "lo2PllFreq_Hz": 3600000000,
*       "auxPllFreq_Hz": 0
*     },
*     "initCals": {
*       "calMask": 866303,
*       "channelMask": 15,
*       "warmBoot": 0
*     },
*     "calRsltsFile": "",
*     "checkFwVer": 0
*   }
*
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
*/
int32_t adi_adrv9025_UtilityInitFileLoad(adi_adrv9025_Device_t*      device,
                                         const char*                 utilInitFile,
                                         adi_adrv9025_PostMcsInit_t* utilInit);

/*
* \brief This utility function dumps the ADRV9025 ARM program and data memory through ArmMemRead() API
*
* This function reads the ARM-C Memory and writes the binary byte array directly to a binary file.First 224K bytes are program memory followed
* by 160K bytes of data memory.The binaryFilename is opened before reading the ARM memory to verify that the filepath is has valid write access
* before reading ARM memory.A file IO exception will be thrown if write access is not valid for the binaryFilename path.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param binaryFilename is a string containing absolute path of the Arm memory to be dumped
*/
int32_t adi_adrv9025_ArmMemDump(adi_adrv9025_Device_t* device,
                                const char*            binaryFilename);

/*
* \brief This utility function dumps the ADRV9025 ARM program and data memory through ArmMemRead() API
*
* This function reads the selected CPU Memory and writes the binary byte array directly to a binary file.First 224K bytes are program memory followed
* by 160K bytes of data memory for ARM-C. First 96K bytes are program memory followed by 160K bytes of data memory for ARM-D(Last 48K bytes is shared
* between ARM-C and ARM-D). The binaryFilename is opened before reading the ARM memory to verify that the filepath is has valid write access
* before reading ARM memory.A file IO exception will be thrown if write access is not valid for the binaryFilename path.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param cpuType Select CPU of which memory will be dumped
* \param binaryFilename is a string containing absolute path of the Arm memory to be dumped
*/
int32_t adi_adrv9025_CpuMemDump(adi_adrv9025_Device_t* device,
                                adi_adrv9025_CpuType_e cpuType,
                                const char*            binaryFilename);

/*
* \brief This utility function dumps the ADRV9025 ARM program and data memory through ArmMemRead() API
*
* This function reads the program and data memory of selected CPU and writes the binary byte array directly to a binary file.
* The binaryFilename is opened before reading the memory to verify that the filepath is has valid write access
* before reading memory.A file IO exception will be thrown if write access is not valid for the binaryFilename path.
* Note: This function doesn't force CPU to throw an exception, so it doesn't stop CPU's operation before reading data/program memories.
*       Hence it should only be called when CPU code isn't running.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param cpuType CPU selection for which memory dump will be performed
* \param binaryFilename is a string containing absolute path of the Arm memory to be dumped
*/
int32_t adi_adrv9025_CpuMemDumpWithoutException(adi_adrv9025_Device_t* device,
                                                adi_adrv9025_CpuType_e cpuType,
                                                const char*            binaryFilename);

/*
* \brief This utility function initializes GP interrupt masks for pin0 and pin1 as provided to it.
*
* The intent of this function is that it gets called during init time to set the GP interrupt masks. By default these are setup as
* provided by the profile files, however, this function can be called anytime to set to other custom values.
*
* \dep_begin
* \dep{NONE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param gpIntInit Pointer to adi_adrv9025_GpInterruptSettings_t structure containing mask for both pin0 and pin1.
*/
int32_t adi_adrv9025_GpIntInit(adi_adrv9025_Device_t*              device,
                               adi_adrv9025_GpInterruptSettings_t* gpIntInit);

/**
* \brief This function should be called during device initialization after Jesd Dfrm links are brought up and Pll 's are locked.
         It first clears Jesd Dfrm Irq's (by latching jrx_dl_204b_irq_clr bitfields). Then it masks all the ramp down events except
         for the ones selected through 'txRampDownEvents' field in profile (It's users responsibility to mask the events for unused PLL sources,
         if auto bit is not set in 'txRampDownEvents' field). Finally it sets master bits ('tx_data_ramp_down_on_pll_unlock' and
         'tx_data_ramp_down_on_jesd_dfrm_irq') to enable Pll Unlock and Jesd Dfrm Tx ramp down functionality.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param init Pointer to the ADRV9025 init device structure
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxRampDownInit(adi_adrv9025_Device_t* device,
                                    adi_adrv9025_Init_t*   init);

/**
* \brief This utility function reads ADRV9025 Rx Gain table file checksum value
*
* This function reads the ADRV9025 Rx Gain Table file in csv format from a specified location
* (typically in an SD card) and get checksum information, 32bit, hex format, in the last line
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param rxGainTablePath is a string containing path to the Rx gain table in csv format to be programmed
* \param rxGainTableChecksum is Rx gain table checksum information, 32bit
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableChecksumRead(adi_adrv9025_Device_t* device,
                                             const char*            rxGainTablePath,
                                             uint32_t*              rxGainTableChecksum);

/**
* \brief This utility function calculates ADRV9025 Rx Gain table file checksum value
*
* This function calculates the ADRV9025 Rx Gain Table from ARM memory and calculate checksum value
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param rxChannel is Rx channel index
* \param rxGainTableChecksum is Rx gain table checksum information, 32bit
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxGainTableChecksumCalculate(adi_adrv9025_Device_t*    device,
                                                  adi_adrv9025_RxChannels_e rxChannel,
                                                  uint32_t*                 rxGainTableChecksum);

/**
* \brief This utility function reads ADRV9025 Tx Atten Table file checksum value
*
* This function reads the ADRV9025 Tx Atten Table file in csv format from a specified location
* (typically in an SD card) and get checksum information, 32bit, hex format, in the last line
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param txAttenTablePath is a string containing path to the Tx atten table in csv format to be programmed
* \param txAttenTableChecksum is Tx atten table checksum information, 32bit
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenTableChecksumRead(adi_adrv9025_Device_t* device,
                                              const char*            txAttenTablePath,
                                              uint32_t*              txAttenTableChecksum);

/**
* \brief This utility function calculates ADRV9025 Tx Atten table file checksum value
*
* This function calculates the ADRV9025 Tx Atten Table from ARM memory and calculate checksum value
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param txChannel is a Tx channel index
* \param txAttenTableChecksum is Tx atten table checksum information, 32bit
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxAttenTableChecksumCalculate(adi_adrv9025_Device_t*    device,
                                                   adi_adrv9025_TxChannels_e txChannel,
                                                   uint32_t*                 txAttenTableChecksum);

/**
* \brief This utility function initializes radioctrl parameters
*
* This function performs the following
* -> Sets up the ARM GPIO Pins
* -> Sets up the Radio Ctrl Mode (Pin or SPI mode) for signal chain enable/disable
* -> Initializes PLL frequencies
* -> Enables/Disables Rx and Tx signal chains(only applicable for SPI mode)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param radioCtrlInit Pointer to ADRV9025 Radioctrl init settings structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RadioctrlInit(adi_adrv9025_Device_t        *device,
                                   adi_adrv9025_RadioctrlInit_t *radioCtrlInit);

/**
* \brief This utility function gets ADRV9025 API/ARM/Stream version and compares with the expected version
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param srcVersion is expected version
* \param result is compared result with bitmask type, Bit0 - API, Bit1 - ARM, Bit2 - Stream, 0 is same and 1 is different
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ApiArmStreamVersionCompare(adi_adrv9025_Device_t               *device,
                                                adi_adrv9025_ApiArmStreamVersions_t *srcVersion,
                                                uint8_t                             *result);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_UTILITIES_H_ */

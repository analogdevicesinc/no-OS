/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_utilities.h
* \brief Contains Utility features related private function definitions
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_UTILITIES_H_
#define _ADRV9025_UTILITIES_H_

#include "adi_adrv9025_utilities_types.h"
#ifdef __KERNEL__
#include <linux/kernel.h>
#endif

#define ADRV9025_MAX_FILE_BUFFER 128

#ifdef __cplusplus
extern "C" {
#endif

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
int32_t adrv9025_RadioctrlInit(adi_adrv9025_Device_t*        device,
                               adi_adrv9025_RadioctrlInit_t* radioCtrlInit);

/**
* \brief This utility function commands the ARM processor to run init cals
*
* The init cals to run are communicated via a mask passed to this function
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param initCals Pointer to ADRV9025 cals init settings structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CalsInit(adi_adrv9025_Device_t*   device,
                          adi_adrv9025_InitCals_t* initCals);

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
int32_t adrv9025_AdcProfilesInit(adi_adrv9025_Device_t*      device,
                                 adi_adrv9025_Init_t*        init,
                                 adi_adrv9025_AdcProfiles_t* adcProfile);

/**
* \brief Parses SPI init settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param spiSettings Pointer to ADRV9025 init SPI settings structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseInitSpiSettings(adi_adrv9025_Device_t*      device,
                                          adi_adrv9025_SpiSettings_t* spiSettings,
                                          FILE*                       fp);

/**
* \brief Parses clock init settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param clockSettings Pointer to ADRV9025 init clock settings structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseInitClockSettings(adi_adrv9025_Device_t*        device,
                                            adi_adrv9025_ClockSettings_t* clockSettings,
                                            FILE*                         fp);

/**
* \brief Parses Rx init settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param rxSettings Pointer to ADRV9025 init Rx settings structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseInitRxSettings(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_RxSettings_t* rxSettings,
                                         FILE*                      fp);

/**
* \brief Parses Tx init settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param txSettings Pointer to ADRV9025 init Tx settings structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseInitTxSettings(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_TxSettings_t* txSettings,
                                         FILE*                      fp);

/**
* \brief Parses Rx ADC Profiles init settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param adcProfies Pointer to ADRV9025 Adc Profiles structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
    int32_t adrv9025_JsonParseInitAdcProfileSettings(adi_adrv9025_Device_t*     device,
                                                     adi_adrv9025_AdcProfiles_t* adcProfiles,
                                                     FILE*                      fp);

/**
* \brief Parses Datapath(JESD) init settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param dataPathSettings Pointer to ADRV9025 init datapath settings structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseInitDataInterfaceSettings(adi_adrv9025_Device_t*           device,
                                                    adi_adrv9025_DataInterfaceCfg_t* dataPathSettings,
                                                    FILE*                            fp);

/**
* \brief Parses GP Interrupt mask init settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param GpInterruptSettings Pointer to ADRV9025 init GP Interrupt init settings structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseInitGpInterruptSettings(adi_adrv9025_Device_t*              device,
                                                  adi_adrv9025_GpInterruptSettings_t* gpInterruptSettings,
                                                  FILE*                               fp);

/**
* \brief Parses RadioCtrl settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param radioCtrlInit Pointer to ADRV9025 radio control structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseUtilRadioctrlInit(adi_adrv9025_Device_t*        device,
                                            adi_adrv9025_RadioctrlInit_t* radioCtrlInit,
                                            FILE*                         fp);

/**
* \brief Parses InitCals settings from a JSON file
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param initCals Pointer to ADRV9025 init cals structure
* \param fp FILE pointer to the JSON file containing init settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseUtilInitCals(adi_adrv9025_Device_t*   device,
                                       adi_adrv9025_InitCals_t* initCals,
                                       FILE*                    fp);

/**
* \brief Finds the Start of JSON file to parse
*
* The start of file is usually an open curly brace "{"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindStartOfFile(adi_adrv9025_Device_t* device,
                                     FILE*                  fp,
                                     char*                  fullStr,
                                     const char*            objNameStr,
                                     int32_t*               currChar);

/**
* \brief Finds the End of JSON file currently being parsed
*
* The start of file is usually a closed curly brace "}"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindEndOfFile(adi_adrv9025_Device_t* device,
                                   FILE*                  fp,
                                   char*                  fullStr,
                                   const char*            objNameStr,
                                   int32_t*               currChar);

/**
* \brief Finds the Start of an object node in the given JSON file
*
* The token usually is of the form "<object_name>: {"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindStartOfObject(adi_adrv9025_Device_t* device,
                                       FILE*                  fp,
                                       char*                  fullStr,
                                       const char*            objNameStr,
                                       int32_t*               currChar);

/**
* \brief Finds the corresponding end of a given object node in the given JSON file
*
* The token usually is of the form "},"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindEndOfObject(adi_adrv9025_Device_t* device,
                                     FILE*                  fp,
                                     char*                  fullStr,
                                     const char*            objNameStr,
                                     int32_t*               currChar);

/**
* \brief Finds the start of an array object node in the given JSON file
*
* The token usually is of the form "<array_object_name>: ["
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindStartOfArray(adi_adrv9025_Device_t* device,
                                      FILE*                  fp,
                                      char*                  fullStr,
                                      const char*            objNameStr,
                                      int32_t*               currChar);

/**
* \brief Finds the corresponding end of an array object node in the given JSON file
*
* The token usually is of the form "],"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindEndOfArray(adi_adrv9025_Device_t* device,
                                    FILE*                  fp,
                                    char*                  fullStr,
                                    const char*            objNameStr,
                                    int32_t*               currChar);

/**
* \brief Finds the start of an object in an array node in the given JSON file
*
* The token usually is of the form "{"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindOpenCurlBrace(adi_adrv9025_Device_t* device,
                                       FILE*                  fp,
                                       char*                  fullStr,
                                       const char*            objNameStr,
                                       int32_t*               currChar);

/**
* \brief Finds the end of an object in an array node in the given JSON file
*
* The token usually is of the form "}"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonFindClosedCurlBrace(adi_adrv9025_Device_t* device,
                                         FILE*                  fp,
                                         char*                  fullStr,
                                         const char*            objNameStr,
                                         int32_t*               currChar);

/**
* \brief Parses an unsigned integer word value of a given property name
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed uint64 value is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseU64(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint64_t*              value);

/**
* \brief Parses a signed integer word value of a given property name
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed int32 value is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseI32(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              int32_t*               value);

/**
* \brief Parses an unsigned integer word value of a given property name
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed uint32 value is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseU32(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint32_t*              value);

/**
* \brief Parses an unsigned half word value of a given property name
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed uint16 value is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseU16(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint16_t*              value);

/**
* \brief Parses an unsigned byte value of a given property name
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed uint8 value is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseU8(adi_adrv9025_Device_t* device,
                             FILE*                  fp,
                             char*                  fullStr,
                             const char*            objNameStr,
                             int32_t*               currChar,
                             uint8_t*               value);

/**
* \brief Parses a signed byte value of a given property name
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed int8 value is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseI8(adi_adrv9025_Device_t* device,
                             FILE*                  fp,
                             char*                  fullStr,
                             const char*            objNameStr,
                             int32_t*               currChar,
                             int8_t*                value);

/**
* \brief Parses an unsigned byte value of a given property name
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param objNameStr Expected object name
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed uint8 value is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseUI8(adi_adrv9025_Device_t* device,
                              FILE*                  fp,
                              char*                  fullStr,
                              const char*            objNameStr,
                              int32_t*               currChar,
                              uint8_t*               value);

/**
* \brief Parses an individual array element of type signed half word
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed Int16 array element is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseArrayElementI16(adi_adrv9025_Device_t* device,
                                          FILE*                  fp,
                                          char*                  fullStr,
                                          int*                   currChar,
                                          int16_t*               value);

/**
* \brief Parses an individual array element of type unsigned half word
*
* The token usually is of the form "<object_name> : value"
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param currChar Pointer to index location of the string to parse
* \param value Pointer to memory where parsed uint16 array element is stored
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
uint32_t adrv9025_JsonParseArrayElementU16(adi_adrv9025_Device_t* device,
                                           FILE*                  fp,
                                           char*                  fullStr,
                                           int*                   currChar,
                                           uint16_t*               value);
/**
* \brief Used to skip past CalRsltsFileStr in Json file
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param fp Pointer to the JSON file to be parsed
* \param fullStr Object and value parsed from the JSON file
* \param currChar Pointer to index location of the string to parse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JsonParseCalRsltFileStr(adi_adrv9025_Device_t* device,
                                         FILE*                  fp,
                                         char*                  fullStr,
                                         int32_t*               currChar);
#ifdef __cplusplus
}
#endif

#endif

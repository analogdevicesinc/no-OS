/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_utilities.h
* \brief Contains top level ADRV904X related function prototypes for
*        adi_adrv904x_utilities.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_UTILITIES_H_
#define _ADI_ADRV904X_UTILITIES_H_

#include "adi_adrv904x_utilities_types.h"
#include "adi_adrv904x_error.h"


/**
* \brief This utility function loads ADRV904X CPU Binary Image
*
* This function reads the CPU Binary Image file from a specified location (e.g. SD card)
* and programs the CPU through CpuImageWrite() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV904X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv904x_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]        cpuBinaryInfo       adi_adrv904x_cpuBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuImageLoad(adi_adrv904x_Device_t* const              device,
                                                           const adi_adrv904x_cpuBinaryInfo_t* const cpuBinaryInfo);

/**
* \brief This utility function loads ADRV904X Stream Binary Image
*
* This function reads the ADRV904X Stream Binary file from a specified location (e.g. SD card) 
* and programs the Stream Processor using StreamImageWrite() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV904X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv90xxx_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]        streamBinaryInfoPtr Pointer to adi_adrv904x_streamBinaryInfo_t for loading Stream Binary Image
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamImageLoad(adi_adrv904x_Device_t* const                    device,
                                                              const adi_adrv904x_streamBinaryInfo_t* const    streamBinaryInfoPtr);

/**
* \brief This utility function loads ADRV904X Rx Gain Table File
*
* This function reads the ADRV904X Rx Gain Table file (i.e. CSV Format) from a specified location
* (e.g. SD card) and programs the ADRV904X CPU SRAM with the gain table for the requested channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]        rxGainTableInfo     Array of adi_adrv904x_RxGainTableInfo_t for loading Rx Gain Tables
* \param[in]        rxGainTableArrSize  Array Size, i.e. Number of Rx gain Tables being provided to load
*
* Rx Gain Table columns should be arranged in the following order
*
*  --------------------------------------------------------------------------------------------------------
*  | Gain Index | FE Control Word | Ext Control | Phase Offset | Digital Gain |
*  --------------------------------------------------------------------------------------------------------
*
* Rx Gain Indices should be arranged in ascending order for row entries starting with the lowest gain index and 
* and progressing to highest gain Index
*
* Eg: If the gain table contains entries from index 192 to index 255, the table has to be arranged such that
*     first line should contain row entries for index 192 and the last line should contain row entries to index 255
*     with a total of 64 entries.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableLoad(adi_adrv904x_Device_t* const            device,
                                                              const adi_adrv904x_RxGainTableInfo_t    rxGainTableInfo[],
                                                              const uint32_t                          rxGainTableArrSize);

/**
* \brief This utility function extract the Init info from the ADRV904X CPU Profile Binary Image
*
* This function reads the CPU Binary Image Profile file from a specified location (e.g. SD card)
* and extract the Init Info, and save in device data structure. This is an init time function and
* is being called by adi_adrv904x_PreMcsInit().
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]        cpuProfileBinaryInfoPtr Pointer to adi_adrv904x_CpuProfileBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceInfoExtract(adi_adrv904x_Device_t* const                        device,
                                                                const adi_adrv904x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr);

/**
* \brief This utility function loads ADRV904X CPU Profile Binary Image
*
* This function reads the CPU Binary Image Profile file from a specified location (e.g. SD card)
* and programs the Binary Image Profile through CpuProfileWrite() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV904X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv904x_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]        cpuProfileBinaryInfoPtr Pointer to adi_adrv904x_CpuProfileBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuProfileImageLoad(adi_adrv904x_Device_t* const                        device,
                                                                  const adi_adrv904x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr);

#ifndef CLIENT_IGNORE
/**
* \brief This utility function copies ADRV904X Device Data
*
* This function reads the Device State Init Extract Information from one to another.
* DeviceCopy should be called before making the devices available to multiple threads.
* Unlike the other adrv904x device functions DeviceCopy is not thread-safe.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] deviceSrc Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] deviceDest Pointer to the ADRV904X device data structure to get updated settings
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceCopy(   adi_adrv904x_Device_t* const    deviceSrc,
                                                            adi_adrv904x_Device_t* const    deviceDest);
#endif /* !CLIENT_IGNORE */

/**
* \brief This Utility Function executes ADRV904X Pre-MCS Initialization Sequence
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in]        init                Pointer to the ADRV904X Init data structure
* \param[in]        trxBinaryInfoPtr    Pointer to all TRX File Information for loading
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PreMcsInit(adi_adrv904x_Device_t* const            device,
                                                         const adi_adrv904x_Init_t* const        init,
                                                         const adi_adrv904x_TrxFileInfo_t* const trxBinaryInfoPtr);

/**
* \brief This utility function executes the non-broadcastable part of
*        Pre-MCS init sequence
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in]        init                Pointer to the ADRV904X Init data structure
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PreMcsInit_NonBroadcast(adi_adrv904x_Device_t* const            device,
                                                                      const adi_adrv904x_Init_t* const        init);

/**
* \brief This Utility Function executes ADRV904X Post-MCS Initialization Sequence
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in]     utilityInit               Pointer to the ADRV904X utility Init data structure
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostMcsInit(adi_adrv904x_Device_t* const            device,
                                                          const adi_adrv904x_PostMcsInit_t* const utilityInit);

/**
* \brief This function is used to query for the Deserializer Lanes SwC status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[out]    swcEnMask                 Deserializer lane mask, which SwC is enabled
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairSwCEnableGet(adi_adrv904x_Device_t* const  device,
                                                                    uint8_t* const                swcEnMask);

/**
* \brief This function is used to set enabled or disabled the Deserializer Lanes SwC status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in]     swcEnMask                 Deserializer lane mask, in which SwC is to be enabled
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairSwCEnableSet(adi_adrv904x_Device_t* const  device,
                                                                    const uint8_t                 swcEnMask);

/**
* \brief This function is used to set enabled or disabled the Deserializer Lanes SwC status 
*        and depending on the number of lanes used use SwC or simply turning VCMs off.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] laneMask Lanes that are required to go into the Vcm fix mode
* \param[in] enableFix If Vcm lanes fix should be enabled or disabled
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairVcmLanesFix(adi_adrv904x_Device_t* const device,
                                                                   uint8_t                      laneMask,
                                                                   uint8_t                      enableFix);
/**
* \brief This function is used to enable the Jrx repair at startup.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] enableRepair If Jrx repair should be enabled or disabled
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_JrxRepairInitialization(adi_adrv904x_Device_t* const device,
                                                                      uint8_t                enableRepair);

/**
* \brief This utility function dumps the ADRV904X CPU program and data memory through Registers32Read() API
*
* This function reads the selected CPU Memory and writes the binary byte array directly to a binary file.
* The binaryFilename is opened before reading the CPU memory to verify that the filepath is has valid write access
* before reading CPU memory.A file IO exception will be thrown if write access is not valid for the binaryFilename path.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] cpuMemDumpBinaryInfoPtr Pointer to adi_adrv904x_CpuMemDumpBinaryInfo_t to be loaded
* \param[in] forceException If set to 1 force all CPUs to exception state before dumping memory.
* \param[out] dumpSize Size of the CPU dump in bytes.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuMemDump(adi_adrv904x_Device_t* const                       device,
                                                         const adi_adrv904x_CpuMemDumpBinaryInfo_t* const   cpuMemDumpBinaryInfoPtr,
                                                         const uint8_t                                      forceException,
                                                         uint32_t* const                                    dumpSize);

/**
* \brief This utility function dumps the ADRV904X CPU RAM memory through Registers32Read() API and all other regions are set to zero
*
* This function reads the selected CPU Memory and writes the binary byte array directly to a binary file.
* The binaryFilename is opened before reading the CPU memory to verify that the filepath is has valid write access
* before reading CPU memory.A file IO exception will be thrown if write access is not valid for the binaryFilename path.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] cpuMemDumpBinaryInfoPtr Pointer to adi_adrv904x_CpuMemDumpBinaryInfo_t to be loaded
* \param[in] forceException If set to 1 force all CPUs to exception state before dumping memory.
* \param[out] dumpSize Size of the CPU dump in bytes.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CpuMemDump_vRamOnly(adi_adrv904x_Device_t* const                      device,
                                                                  const adi_adrv904x_CpuMemDumpBinaryInfo_t* const  cpuMemDumpBinaryInfoPtr,
                                                                  const uint8_t                                     forceException,
                                                                  uint32_t* const                                   dumpSize);

/**
* \brief This utility function reads ADRV904X Rx Gain table file checksum value
*
* This function reads the ADRV904X Rx Gain Table file in csv format from a specified location
* (typically in an SD card) and get checksum information, 32bit, decimal format, in the second line
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxGainTableInfoPtr Pointer to adi_adrv904x_RxGainTableInfo_t for loading Rx Gain Table
* \param[out] rxGainTableChecksum is Rx gain table checksum information, 32bit
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableChecksumRead(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_RxGainTableInfo_t* const  rxGainTableInfoPtr,
                                                                      uint32_t* const rxGainTableChecksum);
/**
* \brief This utility function calculates ADRV904X Rx Gain table file checksum value
*
* This function calculates the ADRV904X Rx Gain Table from CPU memory and calculate checksum value
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] rxChannel is Rx channel index
* \param[out] rxGainTableChecksum is Rx gain table checksum information, 32bit
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableChecksumCalculate(adi_adrv904x_Device_t* const device,
                                                                           const adi_adrv904x_RxChannels_e rxChannel,
                                                                           uint32_t* const rxGainTableChecksum);
/**
* \brief This utility function loads ADRV904X api version, firmware and stream versions,
* ADRV904X Init and PostMcsInit data structures from the Binary Image that also has the profile.
*
* This function reads the CPU Binary Image file from a specified location (e.g. SD card)
* and, if it contains more information than the profile, populates the structs adi_adrv904x_Version_t for 
* the api version, adi_adrv904x_CpuFwVersion_t for the firmware version, adi_adrv904x_Version_t for 
* the stream version, adi_adrv904x_Init_t init structure and adi_adrv904x_PostMcsInit_t postMcsInit structures.
* In case this function is called and the profile binary does not contain information for the mentioned structures
* it will exit informing such state in the checkOutput output parameter.
* 
* This function should be called before adi_adrv904x_PreMcsInit, adi_adrv904x_PreMcsInit_NonBroadcast and adi_adrv904x_PostMcsInit 
* as these functions require the structures to be already populated.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] cpuBinaryInfo adi_adrv904x_cpuBinaryInfo_t to be loaded
* \param[out] apiVer Pointer to the ADRV904X api version data structure to be populated
* \param[out] fwVer Pointer to the ADRV904X firmware version data structure to be populated
* \param[out] streamVer Pointer to the ADRV904X stream version data structure to be populated
* \param[out] init Pointer to the ADRV904X Init data structure to be populated
* \param[out] postMcsInit Pointer to the ADRV904X PostMcsInit data structure to be populated
* \param[out] checkOutput Pointer to ADRV904X enum that constains if the profile binary provided contains the desired structures
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitDataExtract(adi_adrv904x_Device_t* const                     device,
                                                              const adi_adrv904x_CpuProfileBinaryInfo_t* const cpuBinaryInfo,
                                                              adi_adrv904x_Version_t* const                    apiVer,
                                                              adi_adrv904x_CpuFwVersion_t* const               fwVer,
                                                              adi_adrv904x_Version_t* const                    streamVer,
                                                              adi_adrv904x_Init_t* const                       init,
                                                              adi_adrv904x_PostMcsInit_t* const                postMcsInit,
                                                              adi_adrv904x_ExtractInitDataOutput_e* const      checkOutput);

/**
* \brief This function loads Radio Sequencer Binary Image
*
* This function reads the Radio Sequencer Binary Image file from a specified location (e.g. SD card)
* and programs the CPU through RadioSequencerImageWrite() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV904X_RS_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv904x_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]     rsBinaryInfoPtr       adi_adrv904x_RadioSequencerBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerImageLoad(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_RadioSequencerBinaryInfo_t*  const rsBinaryInfoPtr);

/**
* \brief This function reloads Radio Sequencer Binary Image at runtime
*
* This function reads the Radio Sequencer Binary Image file from a specified location (e.g. SD card)
* stops the RS and programs the RS using RadioSequencerImageLoad() API. This API is intended to facilitate
* rapid pattern development and debug in the lab
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV904X_RS_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv904x_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]     rsBinaryInfoPtr       adi_adrv904x_RadioSequencerBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerImageReload(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_RadioSequencerBinaryInfo_t*  const rsBinaryInfoPtr);

/**
* \brief This utility function extracts the pulse data from profile binary and writes it to CFR Pulse RAMs
*
* It reads the CPU Binary Image Profile file from a specified location (e.g. SD card).
* This is an init time function and is being called by adi_adrv904x_PreMcsInit().
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]     cpuProfileBinaryInfoPtr Pointer to adi_adrv904x_CpuProfileBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrPulseExtractAndWrite(adi_adrv904x_Device_t* const                        device,
                                                                      const adi_adrv904x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr);

#endif /* _ADI_ADRV904X_UTILITIES_H_ */

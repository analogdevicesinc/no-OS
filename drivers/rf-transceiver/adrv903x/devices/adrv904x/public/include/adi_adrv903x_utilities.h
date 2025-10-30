/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_utilities.h
* \brief Contains top level ADRV903X related function prototypes for
*        adi_adrv903x_utilities.c
*
* ADRV903X API Version: 2.12.1.4
*/

#ifndef _ADI_ADRV903X_UTILITIES_H_
#define _ADI_ADRV903X_UTILITIES_H_

#include "adi_adrv903x_utilities_types.h"
#include "adi_adrv903x_error.h"


/**
* \brief This utility function loads ADRV903X CPU Binary Image
*
* This function reads the CPU Binary Image file from a specified location (e.g. SD card)
* and programs the CPU through CpuImageWrite() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv903x_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in]        cpuBinaryInfo       adi_adrv903x_cpuBinaryInfo_t to be loaded
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuImageLoad(adi_adrv903x_Device_t* const              device,
                                                           const adi_adrv903x_cpuBinaryInfo_t* const cpuBinaryInfo);

/**
* \brief This utility function loads ADRV903X Stream Binary Image
*
* This function reads the ADRV903X Stream Binary file from a specified location (e.g. SD card) 
* and programs the Stream Processor using StreamImageWrite() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv90xxx_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available.
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in]        streamBinaryInfoPtr Pointer to adi_adrv903x_streamBinaryInfo_t for loading Stream Binary Image
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StreamImageLoad(adi_adrv903x_Device_t* const                    device,
                                                              const adi_adrv903x_streamBinaryInfo_t* const    streamBinaryInfoPtr);

/**
* \brief This utility function loads ADRV903X Rx Gain Table File
*
* This function reads the ADRV903X Rx Gain Table file (i.e. CSV Format) from a specified location
* (e.g. SD card) and programs the ADRV903X CPU SRAM with the gain table for the requested channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in]        rxGainTableInfo     Array of adi_adrv903x_RxGainTableInfo_t for loading Rx Gain Tables
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
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableLoad(adi_adrv903x_Device_t* const            device,
                                                              const adi_adrv903x_RxGainTableInfo_t    rxGainTableInfo[],
                                                              const uint32_t                          rxGainTableArrSize);

/**
* \brief This utility function extract the Init info from the ADRV903X CPU Profile Binary Image
*
* This function reads the CPU Binary Image Profile file from a specified location (e.g. SD card)
* and extract the Init Info, and save in device data structure. This is an init time function and
* is being called by adi_adrv903x_PreMcsInit().
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in]        cpuProfileBinaryInfoPtr Pointer to adi_adrv903x_CpuProfileBinaryInfo_t to be loaded
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DeviceInfoExtract(adi_adrv903x_Device_t* const                        device,
                                                                const adi_adrv903x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr);

/**
* \brief This utility function loads ADRV903X CPU Profile Binary Image
*
* This function reads the CPU Binary Image Profile file from a specified location (e.g. SD card)
* and programs the Binary Image Profile through CpuProfileWrite() API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* Please note that a large chunk size defined by ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv903x_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in]        cpuProfileBinaryInfoPtr Pointer to adi_adrv903x_CpuProfileBinaryInfo_t to be loaded
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuProfileImageLoad(adi_adrv903x_Device_t* const                        device,
                                                                  const adi_adrv903x_CpuProfileBinaryInfo_t* const    cpuProfileBinaryInfoPtr);

#ifndef CLIENT_IGNORE
/**
* \brief This utility function copies ADRV903X Device Data
*
* This function reads the Device State Init Extract Information from one to another.
* DeviceCopy should be called before making the devices available to multiple threads.
* Unlike the other adrv903x device functions DeviceCopy is not thread-safe.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] deviceSrc Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in,out] deviceDest Pointer to the ADRV903X device data structure to get updated settings
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DeviceCopy(   adi_adrv903x_Device_t* const    deviceSrc,
                                                            adi_adrv903x_Device_t* const    deviceDest);
#endif /* !CLIENT_IGNORE */

/**
* \brief This Utility Function executes ADRV903X Pre-MCS Initialization Sequence
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in]        init                Pointer to the ADRV903X Init data structure
* \param[in]        trxBinaryInfoPtr    Pointer to all TRX File Information for loading
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_PreMcsInit(adi_adrv903x_Device_t* const            device,
                                                         const adi_adrv903x_Init_t* const        init,
                                                         const adi_adrv903x_TrxFileInfo_t* const trxBinaryInfoPtr);

/**
* \brief This utility function executes the non-broadcastable part of
*        Pre-MCS init sequence
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in]        init                Pointer to the ADRV903X Init data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_PreMcsInit_NonBroadcast(adi_adrv903x_Device_t* const            device,
                                                                      const adi_adrv903x_Init_t* const        init);

/**
* \brief This Utility Function executes ADRV903X Post-MCS Initialization Sequence
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in]     utilityInit               Pointer to the ADRV903X utility Init data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_PostMcsInit(adi_adrv903x_Device_t* const            device,
                                                          const adi_adrv903x_PostMcsInit_t* const utilityInit);

/**
* \brief This Utility function executes ADRV903X Standby Enter/Power Down Sequence
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] standbyRecover            Pointer to the output ADRV903X Standby Recover data structure.
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyEnter(adi_adrv903x_Device_t* const         device,
                                                           adi_adrv903x_StandbyRecover_t* const standbyRecover);

/**
* \brief This Utility function executes ADRV903X Power up Sequence, recovering from the power down mode.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] standbyRecover            Pointer to the output ADRV903X Standby Recover data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyRecover(adi_adrv903x_Device_t* const         device,
                                                             adi_adrv903x_StandbyRecover_t* const standbyRecover);

/**
* \brief This Utility function executes ADRV903X Standby Exit Sequence, clearing the Standby state.
*        It must be called after StandbyRecover and after MCS/JESD link bring up.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] standbyRecover            Pointer to the ADRV903X Standby Recover data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyExit(adi_adrv903x_Device_t* const            device,
                                                          adi_adrv903x_StandbyRecover_t* const    standbyRecover);

/**
* \brief This Utility function queries the device if the ADRV903X Standby Exit Sequence has finished
*        It must be called after StandbyExit using a polling operation waiting for done to be '1'
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] standbyRecover            Pointer to the ADRV903X Standby Recover data structure
* \param[out]    done                      '1', if the Standby Exit had finished
*                                          '0', otherwise
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_StandbyExitStatusGet(adi_adrv903x_Device_t* const         device,
                                                                   adi_adrv903x_StandbyRecover_t* const standbyRecover,
                                                                   uint8_t * const                      done);



/**
* \brief This Utility function is used to load the repairHistory from a file that is stored in the specified filePath given
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in]     fileInfo                  Path to the Jrx Repair History File
* \param[in,out] repairHistory             Pointer to the ADRV903X Jrx Repair History data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairFileLoad(adi_adrv903x_Device_t* const                     device,
                                                                const adi_adrv903x_JrxRepairBinaryInfo_t* const  fileInfo,
                                                                adi_adrv903x_JrxRepairHistory_t* const           repairHistory);

/**
* \brief This Utility function is used to save the repairHistory calculated from the self repair process to a file that is stored in the specified filePath given
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in]     fileInfo                  Path to the Jrx Repair History File
* \param[in,out] repairHistory             Pointer to the ADRV903X Jrx Repair History data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairFileSave(adi_adrv903x_Device_t* const                     device,
                                                                const adi_adrv903x_JrxRepairBinaryInfo_t * const fileInfo,
                                                                adi_adrv903x_JrxRepairHistory_t* const           repairHistory);

/**
* \brief This Utility is used to check if we have a history of JRx repair.
*        This function will read the screen ID of the part.
*        This function will execute a temperature readback.
*      The result of the history check will be stored in jrxRepair->historyCheck, which can take values from adi_adrv903x_JrxRepairHistoryCheck_e
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] jrxRepair                 Pointer to adi_adrv903x_JrxRepair_t data structure
* \param[out]    checkTemp                 History Check output temperature readback
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairHistoryCheck(adi_adrv903x_Device_t* const     device,
                                                                    adi_adrv903x_JrxRepair_t* const  jrxRepair,
                                                                    int16_t* const                   checkTemp);


/**
* \brief Performs a Lane Assessment and returns a lane mask identifying which lanes have errors.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[out]    badLaneMask               Deserializer lane mask, which bad lanes are selected
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairLaneAssess(adi_adrv903x_Device_t* const  device,
                                                                  uint8_t *const                badLaneMask);

/**
* \brief This utility function is used to initialize the JrxRepair facility
*        This function sets the part in the desired state in order to execute the VCM repair. 
*        If serdes tracking cals were enabled, they will be disabled.
*        If PRBS is needed then it will be activated.
* \brief This utility function is used to initialize the JrxRepair facility.
*
*        The function stores its results in the 'jrxRepair' argument.
*
*        This function changes the device state in order to execute the VCM repair. Specifically, if serdes tracking cals were enabled
*        they will be disabled and if PRBS is needed then it will be activated. In order to restore these device state changes
*        adi_adrv903x_JrxRepairExit must be called later.
*        in order to restore this state at adi_adrv903x_JrxRepairExit()
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] jrxRepair                 Pointer to adi_adrv903x_JrxRepair_t data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairEnter(adi_adrv903x_Device_t* const    device,
                                                             adi_adrv903x_JrxRepair_t* const jrxRepair);

/**
* \brief This utility function is used to survey the enabled lanes in order to check if they have a potential issue.
*        This function loops through various settings and calls adi_adrv903x_JrxRepairTest in every iteration.
*        This function builds the adi_adrv903x_JrxRepairBiasSurvey_t that indicates the errors that occurred per lane.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] repairHistory             Pointer to the ADRV903X Jrx Repair History data structure
* \param[in,out] biasSurvey                Pointer to the ADRV903X Jrx Repair Bias Survey data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairBiasSurvey(adi_adrv903x_Device_t* const              device,
                                                                  adi_adrv903x_JrxRepairHistory_t* const    repairHistory,
                                                                  adi_adrv903x_JrxRepairBiasSurvey_t* const biasSurvey);

/**
* \brief Performs Serdes InitCal in Fast Attack mode and Lookup of the Init Cal Status
*
* \pre This function may be called any time after the device has been initialized, and
*      initialization calibrations have taken place.
*      Serdes Tracking Calibration should be disabled before calling this function
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device          Context variable - Context variable - A pointer to the device settings structure
* \param[out]    initCalErrData  Pointer passed to obtain init calibrations error data.
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairFastAttackRun(adi_adrv903x_Device_t* const          device,
                                                                     adi_adrv903x_InitCalErrData_t* const  initCalErrData);

/**
* \brief This utility function is used to test the jesd lanes for 204B or 204C link setup, it can operate in PRBS mode if it is active or with live data.
*        This function will return the test result which will be the addition of all different error types per lane or the PRBS error count per lane.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] testResult                Pointer to the ADRV903X Jrx Repair Test data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairTest(adi_adrv903x_Device_t* const        device,
                                                            adi_adrv903x_JrxRepairTest_t* const testResult);

/**
* \brief This function is used to apply the repairHistory for the jesd lanes in order to solve the issue with VCM
*        The repairHistory will be checked and applied to the HW using SwC method
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] repairHistory             Pointer to the ADRV903X Jrx Repair History data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairApply(adi_adrv903x_Device_t* const                    device,
                                                             adi_adrv903x_JrxRepairHistory_t* const          repairHistory);

/**
* \brief This function is used to query for the Deserializer Lanes SwC status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[out]    swcEnMask                 Deserializer lane mask, which SwC is enabled
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairSwCEnableGet(adi_adrv903x_Device_t* const  device,
                                                                    uint8_t* const                swcEnMask);

/**
* \brief This function is used to set enabled or disabled the Deserializer Lanes SwC status
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in]     swcEnMask                 Deserializer lane mask, in which SwC is to be enabled
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairSwCEnableSet(adi_adrv903x_Device_t* const  device,
                                                                    const uint8_t                 swcEnMask);


/**
* \brief Completes the JrxRepair procedure.
*        Uses information from 'jrxRepair' to restore any device state changes made by adi_adrv903x_JrxRepairEnter().
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] jrxRepair                 Pointer to adi_adrv903x_JrxRepair_t data structure
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairExit(adi_adrv903x_Device_t* const    device,
                                                            adi_adrv903x_JrxRepair_t* const jrxRepair);


/**
* \brief Top level function for the self repair activity
*        This function will call the following:
*                    -adi_adrv903x_JrxRepairHistoryCheck
*                    -adi_adrv903x_JrxRepairEnter
*                    -adi_adrv903x_JrxRepairSwCEnableGet
*                    -adi_adrv903x_JrxRepairBiasSurvey
*                    -adi_adrv903x_JrxRepairApply
*                    -adi_adrv903x_JrxRepairFastAttackRun
*                    -adi_adrv903x_JrxRepairTest
*                    -adi_adrv903x_JrxRepairExit
*
*       We will need to call adi_adrv903x_JrxRepairHistoryCheck
*           this function will check:
*               errors
*               temperature
*               version
*               loaded history
*           In order to decide what is the next step
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X Device data structure
* \param[in,out] jrxRepair                 Pointer to adi_adrv903x_JrxRepair_t
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairExecute(adi_adrv903x_Device_t* const    device,
                                                               adi_adrv903x_JrxRepair_t* const jrxRepair);

/**
* \brief This function is used to set enabled or disabled the Deserializer Lanes SwC status 
*        and depending on the number of lanes used use SwC or simply turning VCMs off.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] laneMask Lanes that are required to go into the Vcm fix mode
* \param[in] enableFix If Vcm lanes fix should be enabled or disabled
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairVcmLanesFix(adi_adrv903x_Device_t* const device,
                                                                   uint8_t                      laneMask,
                                                                   uint8_t                      enableFix);
/**
* \brief This function is used to enable the Jrx repair at startup.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] enableRepair If Jrx repair should be enabled or disabled
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_JrxRepairInitialization(adi_adrv903x_Device_t* const device,
                                                                      uint8_t                enableRepair);

/**
* \brief This utility function dumps the ADRV903X CPU program and data memory through Registers32Read() API
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
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] cpuMemDumpBinaryInfoPtr Pointer to adi_adrv903x_CpuMemDumpBinaryInfo_t to be loaded
* \param[in] forceException If set to 1 force all CPUs to exception state before dumping memory.
* \param[out] dumpSize Size of the CPU dump in bytes.
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuMemDump(adi_adrv903x_Device_t* const                       device,
                                                         const adi_adrv903x_CpuMemDumpBinaryInfo_t* const   cpuMemDumpBinaryInfoPtr,
                                                         const uint8_t                                      forceException,
                                                         uint32_t* const                                    dumpSize);

/**
* \brief This utility function dumps the ADRV903X CPU RAM memory through Registers32Read() API and all other regions are set to zero
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
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] cpuMemDumpBinaryInfoPtr Pointer to adi_adrv903x_CpuMemDumpBinaryInfo_t to be loaded
* \param[in] forceException If set to 1 force all CPUs to exception state before dumping memory.
* \param[out] dumpSize Size of the CPU dump in bytes.
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CpuMemDump_vRamOnly(adi_adrv903x_Device_t* const                      device,
                                                                  const adi_adrv903x_CpuMemDumpBinaryInfo_t* const  cpuMemDumpBinaryInfoPtr,
                                                                  const uint8_t                                     forceException,
                                                                  uint32_t* const                                   dumpSize);

/**
* \brief This utility function reads ADRV903X Rx Gain table file checksum value
*
* This function reads the ADRV903X Rx Gain Table file in csv format from a specified location
* (typically in an SD card) and get checksum information, 32bit, decimal format, in the second line
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxGainTableInfoPtr Pointer to adi_adrv903x_RxGainTableInfo_t for loading Rx Gain Table
* \param[out] rxGainTableChecksum is Rx gain table checksum information, 32bit
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableChecksumRead(adi_adrv903x_Device_t* const device,
                                                                      const adi_adrv903x_RxGainTableInfo_t* const  rxGainTableInfoPtr,
                                                                      uint32_t* const rxGainTableChecksum);
/**
* \brief This utility function calculates ADRV903X Rx Gain table file checksum value
*
* This function calculates the ADRV903X Rx Gain Table from CPU memory and calculate checksum value
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] rxChannel is Rx channel index
* \param[out] rxGainTableChecksum is Rx gain table checksum information, 32bit
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableChecksumCalculate(adi_adrv903x_Device_t* const device,
                                                                           const adi_adrv903x_RxChannels_e rxChannel,
                                                                           uint32_t* const rxGainTableChecksum);
/**
* \brief This utility function loads ADRV903X api version, firmware and stream versions,
* ADRV903X Init and PostMcsInit data structures from the Binary Image that also has the profile.
*
* This function reads the CPU Binary Image file from a specified location (e.g. SD card)
* and, if it contains more information than the profile, populates the structs adi_adrv903x_Version_t for 
* the api version, adi_adrv903x_CpuFwVersion_t for the firmware version, adi_adrv903x_Version_t for 
* the stream version, adi_adrv903x_Init_t init structure and adi_adrv903x_PostMcsInit_t postMcsInit structures.
* In case this function is called and the profile binary does not contain information for the mentioned structures
* it will exit informing such state in the checkOutput output parameter.
* 
* This function should be called before adi_adrv903x_PreMcsInit, adi_adrv903x_PreMcsInit_NonBroadcast and adi_adrv903x_PostMcsInit 
* as these functions require the structures to be already populated.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in] cpuBinaryInfo adi_adrv903x_cpuBinaryInfo_t to be loaded
* \param[out] apiVer Pointer to the ADRV903X api version data structure to be populated
* \param[out] fwVer Pointer to the ADRV903X firmware version data structure to be populated
* \param[out] streamVer Pointer to the ADRV903X stream version data structure to be populated
* \param[out] init Pointer to the ADRV903X Init data structure to be populated
* \param[out] postMcsInit Pointer to the ADRV903X PostMcsInit data structure to be populated
* \param[out] checkOutput Pointer to ADRV903X enum that constains if the profile binary provided contains the desired structures
*
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitDataExtract(adi_adrv903x_Device_t* const                     device,
                                                              const adi_adrv903x_CpuProfileBinaryInfo_t* const cpuBinaryInfo,
                                                              adi_adrv903x_Version_t* const                    apiVer,
                                                              adi_adrv903x_CpuFwVersion_t* const               fwVer,
                                                              adi_adrv903x_Version_t* const                    streamVer,
                                                              adi_adrv903x_Init_t* const                       init,
                                                              adi_adrv903x_PostMcsInit_t* const                postMcsInit,
                                                              adi_adrv903x_ExtractInitDataOutput_e* const      checkOutput);

#endif /* _ADI_ADRV903X_UTILITIES_H_ */

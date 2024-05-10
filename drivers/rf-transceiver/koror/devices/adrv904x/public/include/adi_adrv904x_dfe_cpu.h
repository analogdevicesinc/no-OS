/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_cpu.h
* \brief Contains ADRV904X processor function prototypes for
*    adi_adrv904x_dfe_cpu.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_DFE_CPU_H_
#define _ADI_ADRV904X_DFE_CPU_H_

#include "adi_adrv904x_dfe_cpu_types.h"
#include "adi_adrv904x_dfe_cpu_pintsw_status_types.h"
#include "adi_adrv904x_error.h"
#include "adi_common_error_types.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "adi_adrv904x_cpu_sys_types.h"
#include "adi_adrv904x_dfe_svc_dfe_actuator_cmn_t.h"
#include "adi_adrv904x_dfe_svc_log_trace_events_t.h"
#include "adi_adrv904x_dfe_app_capture_sequencer_t.h"

/**
* \brief Get DFE CPU load
*
*
*
* \pre This function is called after the device has been initialized and radio multichip-sync
* (MCS) has been completed
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in,out] pDfeCpuLoad -Pointer to DFE CPU load data.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeLoadGet(adi_adrv904x_Device_t* const        device,
                                                         adi_adrv904x_DfeCpuLoad_t* const    pDfeCpuLoad);

/**
* \brief Get DFE CPU and memory load
*
*
*
* \pre This function is called after the device has been initialized and radio multichip-sync
* (MCS) has been completed
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in,out] pDfeCpuLoad -Pointer to DFE CPU and memory load data.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuAndMemLoadGet(adi_adrv904x_Device_t* const            device,
                                                                  adi_adrv904x_DfeCpuAndMemLoad_t* const  pDfeCpuLoad);

/**
* \brief Start the DFE CPU(s)
*
* Sets run bit to 1. Then wait and check for FW Status. 
*
* 
* \pre This function is called after the device has been initialized and radio multichip-sync
* (MCS) has been completed
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuStart(adi_adrv904x_Device_t* const device);

/**
* \brief A polling wait for the CPU status to be ready.
*
* Polls firmware status (fwStatus) until it becomes ADRV904X_DFE_CPU_FW_STATUS_READY or a timeout occurs.
*
*
* \pre This function is called after the device has been initialized and after multichip-sync
* (MCS) has been completed and adi_adrv904x_DfeCpuStart is called.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in] timeout_us Timeout to stop waiting for CPU to boot up.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuStartStatusCheck(adi_adrv904x_Device_t* const   device,
                                                                     const uint32_t                 timeout_us);
/**
* \brief This function loads DFE CPU Binary Image
*
* This function reads the DFE CPU Binary Image file from a specified location (e.g. SD card)
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
* \param[in]        cpuDfeBinaryInfo       adi_adrv904x_DfeCpuBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuImageLoad(adi_adrv904x_Device_t* const                  device,
                                                              const adi_adrv904x_DfeCpuBinaryInfo_t* const  cpuDfeBinaryInfo);
/**
* \brief This function sets up the Adrv904x part to be able to load a DFE image
*
* This function sets up the A55 clock, L4 memory Settings and various other device parameters in order to be able to load a DFE Cpu Image
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in] reload a flag indicating whether to reload the image (while an image is already loaded) ADI_TRUE - Reload, ADI_FALSE - Load for the first time
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePrepareLoad(adi_adrv904x_Device_t* const   device,
                                                             const uint8_t                  reload);

/**
* \brief This function reset the required clocks, loads DFE CPU Binary Image and start the DFE processor again.
*
* This function will need to be called by BBIC for reloading the D55 processor in case DFE application report an irrecoverable * error.
* TODO: This function is broken at the moment since DFE processor retrieves the profile data from temporary location at the end of 
* L4 memory during boot up. This will updated and DFE processor will read the required profile data from radio processor memory.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]        cpuDfeBinaryInfo        adi_adrv904x_DfeCpuBinaryInfo_t to be loaded
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuReload(adi_adrv904x_Device_t* const                     device,
                                                           const adi_adrv904x_DfeCpuBinaryInfo_t*  const    cpuDfeBinaryInfo);

/**
* \brief Check if the DFE CPU's exception flag is set.
*
* \pre This function may be called any time after device initialization
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] isException - Is set to non-zero if the DFE's CPU-exception flag is set. The value set is not
*     defined but can be analyzed by ADI and should be included in bug reports.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if the exception flag was read successfully. 
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuCheckException(adi_adrv904x_Device_t* const    device,
                                                                   uint32_t* const                 isException);

/**
* \brief Transmits a DFE message to the DFE application processor and returns the response.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] command the command to send to the DFE
* \param[in] payload command data to be sent to the application processor.
* \param[in] payload_length Length of the payload data in bytes
* \param[in] response data from the output of the command, sent from the application processor.
* \param[in] response_length maximum length of the response data in bytes
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppCommand(adi_adrv904x_Device_t* const    device,
                                                            const uint8_t                   command,
                                                            const uint8_t                   payload[],
                                                            const uint32_t                  payload_length,
                                                            const uint8_t                   response[],
                                                            const uint32_t                  response_length);

/**
 * \brief This function sets the capture sequencer configuration
 * 
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] capSeqCfg  Capture sequencer configuration
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCaptureSequencerConfigSet(adi_adrv904x_Device_t* const                            device,
                                                                           const adi_adrv904x_DfeAppCaptureSequencerCfg_t* const   capSeqCfg);

/**
 * \brief This function reads the capture sequencer configuration
 * 
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[out] capSeqCfg  Capture sequencer configuration
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCaptureSequencerConfigGet(adi_adrv904x_Device_t* const                       device,
                                                                           adi_adrv904x_DfeAppCaptureSequencerCfg_t* const    capSeqCfg);
/**
* \brief Enables or disables available DFE tracking cals
*
* This function is used to enabled or disable available DFE
* tracking cals.  
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] calMask bitfield of enumerated tracking cals
* \param[in] channelMask bitfield of enumerated channels
* \param[in] enableDisableFlag enable (1) flag for enumerated channels for the calMask tracking cals
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalsEnableSet(adi_adrv904x_Device_t* const                         device,
                                                                       const uint32_t                                       calMask,
                                                                       const uint32_t                                       channelMask,
                                                                       const adi_adrv904x_DfeAppTrackingCalEnableDisable_t  enableDisableFlag); 

/**
* \brief Reads the enabled state of the available DFE tracking cals
*
* This function is used to read the current enabled state of the 
* available DFE tracking cals.  
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[out] enableMasks  bitfield of enumerated tracking cals
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalsEnableGet(adi_adrv904x_Device_t* const                                 device,
                                                                       adi_adrv904x_DfeAppFrameworkTrackingCalEnableMasks_t* const  enableMasks);

/**
* \brief Sets a timer value for the specified cal timer for the specified DFE tracking cals
*
* This function is used to set a timer value for the specified cal timer.
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] calType calibration timer to adjust (see adi_app_FrameworkTrackingCalType_e for types available)
* \param[in] calTimerValue calibration timer value in milliseconds for calTimerType
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalTimerSet(adi_adrv904x_Device_t* const                                 device,
                                                                     const adi_adrv904x_DfeAppFrameworkTrackingCalType_t          calType,
                                                                     const uint32_t                                               calTimerValue);

/**
* \brief Gets a timer value for the specified cal timer for the specified DFE tracking cals
*
* This function is used to get a timer value for the specified cal timer.  
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] calType  calibration timer to adjust (see adi_app_FrameworkTrackingCalType_e for types available)
* \param[out] calTimerValue calibration timer value in milliseconds for calTimerType
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalTimerGet(adi_adrv904x_Device_t* const                                 device,
                                                                     const adi_adrv904x_DfeAppFrameworkTrackingCalType_t          calType,
                                                                     uint32_t* const                                              calTimerValue);

/**
* \brief Writes configuration parameters to the provided SDK DFE configuration structure.  
*
* This function is used to modify configuration parameters in the SDK 
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] configID   enumeration of the configuration to access
* \param[in] offset     off set into the specified configuration
* \param[in] buffer     the data to write at offset in the configuration data
* \param[in] size       the size of the data to write
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSvcConfigSet(adi_adrv904x_Device_t* const  device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size);



/**
* \brief Reads configuration parameters from the provided SDK DFE configuration structure.  
*
* This function used to read configuration parameters from the SDK 
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in]  configID  enumeration of the configuration to access
* \param[in]  offset    off set into the specified configuration
* \param[out] buffer    the data read from offset in the configuration data
* \param[in]  size      the size of the data to write
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSvcConfigGet(adi_adrv904x_Device_t* const  device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size);

/**
* \brief Reads configuration parameters from the provided DFE App configuration structure.  
*
* This function used to read configuration parameters from the App
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in]  configID  index of the configuration to access
* \param[in]  offset    off set into the specified configuration
* \param[out] buffer    the data read from offset in the configuration data
* \param[in]  size      the size of the data to write
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppConfigGet(adi_adrv904x_Device_t* const device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size);

/**
* \brief Writes configuration parameters to the provided DFE App configuration structure.  
*
* This function is used to modify configuration parameters in the App
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in] configID   enumeration of the configuration to access
* \param[in] offset     off set into the specified configuration
* \param[in] buffer     the data to write at offset in the configuration data
* \param[in] size       the size of the data to write
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppConfigSet(adi_adrv904x_Device_t* const  device,
                                                              const uint8_t                 configID,
                                                              const uint32_t                offset,
                                                              const uint8_t                 buffer[],
                                                              const uint32_t                size);

/**
* \brief Unlocks configuration structures in the SDK DFE  
*
* This function is used to unlock the ability to modify configuration parameters in the SDK 
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device  Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in]     cfgKey  the 32 bit key used to unlock configuration access
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeConfigUnlock(adi_adrv904x_Device_t* const  device,
                                                              const uint32_t                cfgKey);

/**
* \brief Sets the specified Set Ctrl in the adi_adrv904x_DfeAppControlCmdExec structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] objId  Object ID of calibration or system component
* \param[in] cpuCmd Command to be executed
* \param[in] channel channel enum
* \param[in] cpuCtrlData Pointer to data to send TO the CPU
* \param[in] lengthSet Length of cpuCtrlData, in bytes
* \param[out] lengthResp Length of data received FROM the CPU into ctrlResp buffer, in bytes
* \param[out] ctrlResp Pointer to buffer in which data FROM the CPU should be received.
* \param[in] lengthGet Size of ctrlResp buffer, in bytes. MUST be greater than or equal to lengthResp.
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppControlCmdExec(adi_adrv904x_Device_t* const     device,
                                                                   const uint32_t                   objId,
                                                                   const uint16_t                   cpuCmd,
                                                                   const adi_adrv904x_Channels_e    channel,
                                                                   const uint8_t                    cpuCtrlData[],
                                                                   const uint32_t                   lengthSet,
                                                                   uint32_t* const                  lengthResp,
                                                                   uint8_t                          ctrlResp[],
                                                                   const uint32_t                   lengthGet);

/**
* \brief Read the system status from the SDK.  
*
* This function used to read the system status from the SDK 
*
* \pre This function is called after running the firmware.
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device data structure containing settings
* \param[in]     channel   bitfield of enumerated channels
* \param[in]     objID     enumerated objID for the status request
* \param[out]    status    the status data of the specified objID channel
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSysStatusGet(adi_adrv904x_Device_t* const          device,
                                                              const adi_adrv904x_Channels_e         channel,
                                                              const uint32_t                        objID, 
                                                              adi_adrv904x_CpuSysStatus_t* const    status);

/**
* \brief Allows reading the current state of all tracking calibrations
*
* The returned state information is indexed per channel and per calibration ID (see
* adi_adrv904x_DfeAppFrameworkTrackingCalType_e).
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[out]    calState Pointer to tracking calibration state structure to store the read values
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalAllStateGet(adi_adrv904x_Device_t* const device, 
                                                                        adi_adrv904x_DfeAppFrameworkTrackingCalState_t* const calState);


/**
 * \brief Returns the status of the tracking calibration
 *
 * The function can be called to read back the status of the 
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] calId  Indicate a type of cal tracking to get as defined in adi_adrv904x_TrackingCalibrationMask_e
 * \param[in] channel Channel selection to read back cal status. Must be a single channel.
 * \param[out] calStatus Status of the calibration, as a structure of type adi_adrv904x_CalStatus_t is returned to this pointer address
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeTrackingCalStatusGet(adi_adrv904x_Device_t* const                                  device,
                                                                      const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e   calId,
                                                                      const adi_adrv904x_Channels_e                                 channel,
                                                                      adi_adrv904x_CalStatus_t* const                               calStatus);


/**
  * \brief Returns detailed status information specific to the private calibration
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] channel Channel selection to read back cal status. Must be a single channel
 * \param[in] objId Object ID of calibration or system component
 * \param[in, out] calStatusGet Status of the calibration. Size of calStatusGet must be equal or greater than length param.
 * \param[in] length Length of the configuration in bytes
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCalPvtStatusGet(adi_adrv904x_Device_t* const   device,
                                                                 const adi_adrv904x_Channels_e  channel,
                                                                 const uint32_t                 objId,
                                                                 uint8_t                        calStatusGet[],
                                                                 const uint32_t                 length);


/**
  * \brief Returns status specific information calibration
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] channel Channel selection to read back cal status. Must be a single channel
 * \param[in] objId Object ID of calibration or system component
 * \param[in, out] calStatusGet Status of the calibration. Size of calStatusGet must be equal or greater than length param.
 * \param[in] length Length of the configuration in bytes
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCalSpecificStatusGet(adi_adrv904x_Device_t* const  device,
                                                                      const adi_adrv904x_Channels_e channel,
                                                                      const uint32_t                objId,
                                                                      uint8_t                       calStatusGet[],
                                                                      const uint32_t                length);

/**
* \brief Returns status extended information calibration
*
* \pre This function may be called any time after the device has been initialized, and
* initialization calibrations have taken place
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in] channel Channel selection to read back cal status. Must be a single channel
* \param[in] objId Object ID of calibration or system component
* \param[in, out] calStatusGet Status of the calibration. Size of calStatusGet must be equal or greater than length param.
* \param[in] length Length of the status in bytes
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCalExtendedStatusGet(adi_adrv904x_Device_t* const  device,
                                                                      const adi_adrv904x_Channels_e channel,
                                                                      const uint32_t                objId,
                                                                      uint8_t                       calStatusGet[],
                                                                      const uint32_t                length);

/**
* \brief Reads back the DFE app init status loaded into the DFE processor
*
* This function reads the DFE app after DFE initial.
*
* \pre This function may be called after the CPU binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] dfeAppInitStatus uint32_t pointer to DFE app init status
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppInitStatusGet(adi_adrv904x_Device_t* const  device,
                                                                  uint32_t* const               dfeAppInitStatus);
/**
* \brief Reads back the DFE boot error code.
*
* This function reads the DFE boor error code during DFE boot.
*
* \pre This function may be called after the CPU binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] dfeBootError uint32_t pointer to DFE boot error code
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/                                                                  
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeBootErrorGet(  adi_adrv904x_Device_t* const    device,
                                                                uint32_t* const                 dfeBootError);

/**
 * \brief Returns system status information
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] channel Channel selection to read back cal status. Must be a single channel
 * \param[in] objID Object ID of calibration or system component
 * \param[in, out] cpuSysStatusGet Status of the system. Size of cpuSysStatusGet must be equal or greater than length param.
 * \param[in] length Length of the configuration in bytes
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSysPvtStatusGet(adi_adrv904x_Device_t* const   device,
                                                                 const adi_adrv904x_Channels_e  channel,
                                                                 const uint32_t                 objID,
                                                                 uint8_t                        cpuSysStatusGet[],
                                                                 const uint32_t                 length);

/**
* \brief This function set the DFE FW Debug Mode
*  There are 4 debug modes and the GPIO Pins associated with each:
*  ADI_ADRV904X_DFE_FW_DEBUG_MODE_SWD:   GPIO Pins 13, 14
*  ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG:  GPIO Pins 10, 11, 12, 13, 14
*  ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG4: GPIO Pins 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
*  ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG8: GPIO Pins 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
*  Note that when one of mode is enable, it will reconfigure any needed GPIO pins that are being used.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. Device must be in
* debug mode.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in] dfeFwDebugMode DFE FW Debug mode specified in adi_adrv904x_DfeFwDebugMode_e 

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeFwDebugModeSet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_DfeFwDebugMode_e dfeFwDebugMode);

/**
* \brief This function enables the DFE FW Debug Mode
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. 
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
* \param[in] debugModeKey DFE FW Debug mode key 

* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDebugMode(adi_adrv904x_Device_t* const device,
                                                           const uint32_t               debugModeKey);

/**
* \brief Sets the specified Cmd for the Debug feature 
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] objId  Object ID of calibration or system component
* \param[in] cpuCmd Command to be executed
* \param[in] cpuDebugData Pointer to data to send TO the CPU
* \param[in] lengthSet Length of cpuDebugData, in bytes
* \param[out] lengthResp Length of data received FROM the CPU into ctrlResp buffer, in bytes
* \param[out] debugResp Pointer to buffer in which data FROM the CPU should be received.
* \param[in] lengthGet Size of ctrlResp buffer, in bytes. MUST be greater than or equal to lengthResp.
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDebugModeCmd(adi_adrv904x_Device_t* const    device,
                                                              const uint32_t                  objId,
                                                              const uint16_t                  cpuCmd,
                                                              const uint8_t                   cpuDebugData[],
                                                              const uint32_t                  lengthSet,
                                                              uint32_t* const                 lengthResp,
                                                              uint8_t                         debugResp[],
                                                              const uint32_t                  lengthGet);

/**
* \brief Sets the Watchdog config timeout value feature 
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] wdt Watchdog instance
* \param[in] timeout_ms Watchdog timeout value. Setting timeout_ms to 0 disables the specified WDT.
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeWatchdogConfigSet(adi_adrv904x_Device_t* const                 device,
                                                                   const adi_adrv904x_DfeSvcWatchdogInstance_t  wdt,
                                                                   const uint32_t                               timeout_ms);

/**
* \brief This function forces an ECC scrubbing Update
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization. 
*
* \param[in,out] device Context variable - Pointer to the ADRV904X Device data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeUpdateEcc(adi_adrv904x_Device_t* const device);

/**
 * \brief Sets the log level mask for DFE CPU
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[in] logMaskSet - log mask to set.
 *  logMaskSet permits following bit mapped DFE cpu log levels  enabled(1)/disabled(0).
 *    ADI_SVC_LOG_TRACE_LEVEL_DEBUG    = 0x1UL,    Debug Message logging, no action required
 *    ADI_SVC_LOG_TRACE_LEVEL_INFO     = 0x2UL,    Informative, no action required. Error code parameter will be ignore.  Info messages will not be logged to the telemetry.
 *    ADI_SVC_LOG_TRACE_LEVEL_WARNING  = 0x4UL,    Some attention required. Warning messages will not be logged to the telemetry.
 *    ADI_SVC_LOG_TRACE_LEVEL_CRITICAL = 0x8UL,    Can be continued after handling.
 *    ADI_SVC_LOG_TRACE_LEVEL_FATAL    = 0x10UL    Cannot continue, needs reboot.
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */                                                                
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuLogMaskSet( adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  logMaskSet);

/**
 * \brief Get the log level mask for DFE CPU
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[in, out] logMaskGet - log mask to get.
 *  logMaskGet will have following bit mapped DFE cpu log levels enabled(1)/disabled(0).
 *    ADI_SVC_LOG_TRACE_LEVEL_DEBUG    = 0x1UL,    Debug Message logging, no action required
 *    ADI_SVC_LOG_TRACE_LEVEL_INFO     = 0x2UL,    Informative, no action required. Error code parameter will be ignore.  Info messages will not be logged to the telemetry.
 *    ADI_SVC_LOG_TRACE_LEVEL_WARNING  = 0x4UL,    Some attention required. Warning messages will not be logged to the telemetry.
 *    ADI_SVC_LOG_TRACE_LEVEL_CRITICAL = 0x8UL,    Can be continued after handling.
 *    ADI_SVC_LOG_TRACE_LEVEL_FATAL    = 0x10UL    Cannot continue, needs reboot.
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuLogMaskGet( adi_adrv904x_Device_t* const    device,
                                                                uint32_t* const                 logMaskGet);

/**
 * \brief Sets the telemetry mask for DFE CPU
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place. adi_adrv904x_DfeSvcLogTraceEvent_e enum
 * lists all the events available on both SDK and APP layer. These event ids can be used
 *  to compose the telemetry masks. Setting the bit representing a particular event will
 *  enable telemetry for that event.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[in] sdkTelemetryMask0 - Telemetry mask for first 64 SDK events
 * \param[in] sdkTelemetryMask1 - Telemetry mask for second 64 SDK events
 * \param[in] appTelemetryMask0 - Telemetry mask for first 64 APP events
 * \param[in] appTelemetryMask1 - Telemetry mask for second 64 APP events
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuTelemetryMaskSet( adi_adrv904x_Device_t* const    device,
                                                                      const uint64_t                  sdkTelemetryMask0,
                                                                      const uint64_t                  sdkTelemetryMask1,
                                                                      const uint64_t                  appTelemetryMask0,
                                                                      const uint64_t                  appTelemetryMask1);

/**
 * \brief Reads the telemetry mask for DFE CPU
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place. adi_adrv904x_DfeSvcLogTraceEvent_e enum
 * lists all the events available on both SDK and APP layer.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[out] sdkTelemetryMask0 - Telemetry mask for first 64 SDK events
 * \param[out] sdkTelemetryMask1 - Telemetry mask for second 64 SDK events
 * \param[out] appTelemetryMask0 - Telemetry mask for first 64 APP events
 * \param[out] appTelemetryMask1 - Telemetry mask for second 64 APP events
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuTelemetryMaskGet( adi_adrv904x_Device_t* const    device,
                                                                      uint64_t * const                sdkTelemetryMask0,
                                                                      uint64_t * const                sdkTelemetryMask1,
                                                                      uint64_t * const                appTelemetryMask0,
                                                                      uint64_t * const                appTelemetryMask1);

/**
 * \brief Returns current status of DFE CPU Interrupt Sources PintSw0 and PintSw1
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[out] statusPintSw0 - Bitmask of all currently PintSw0 interrupts. Interpret at bitwise OR of interrupts included in adi_adrv904x_DfeCpuPintSw0Status_e
 * \param[out] statusPintSw1 - Bitmask of all currently PintSw1 interrupts. Interpret at bitwise OR of interrupts included in adi_adrv904x_DfeCpuPintSw1Status_e
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSwStatusGet(adi_adrv904x_Device_t* const    device,
                                                                    uint8_t* const                  statusPintSw0,
                                                                    uint8_t* const                  statusPintSw1);

/**
 * \brief Clears DFE CPU Interrupt Sources of PintSw0 and PintSw1
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[in] clearPintSw0 - Bitmask of all PintSw0 interrupts to clear. Interpret at bitwise OR of interrupts included in adi_adrv904x_DfeCpuPintSw0Status_e
 * \param[in] clearPintSw1 - Bitmask of all PintSw1 interrupts to clear. Interpret at bitwise OR of interrupts included in adi_adrv904x_DfeCpuPintSw1Status_e
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSwStatusClear(adi_adrv904x_Device_t* const  device,
                                                                      const uint8_t                 clearPintSw0,
                                                                      const uint8_t                 clearPintSw1);

/**
 * \brief Returns detailed information from the last instance that a selected interrupt from DFE CPU PintSw0 was triggered.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[in] interruptSelect - Selected interrupt of type adi_adrv904x_DfeCpuPintSw0Status_e about which to get more information.
 * \param[out] pInfo - 32bit value that indicates more information about last instance that interruptSelect was triggered
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSw0DetailedInfoGet(adi_adrv904x_Device_t* const                 device,
                                                                           const adi_adrv904x_DfeCpuPintSw0Status_e     interruptSelect,
                                                                           uint32_t* const                              pInfo);

/**
 * \brief Returns detailed information from the last instance that a selected interrupt from DFE CPU PintSw1 was triggered.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[in] interruptSelect - Selected interrupt of type adi_adrv904x_DfeCpuPintSw1Status_e about which to get more information.
 * \param[out] pInfo - 32bit value that indicates more information about last instance that interruptSelect was triggered
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCpuPintSw1DetailedInfoGet(adi_adrv904x_Device_t* const                 device,
                                                                           const adi_adrv904x_DfeCpuPintSw1Status_e     interruptSelect,
                                                                           uint32_t* const                              pInfo);


/**
 * \brief Returns detailed information from the last instance of all errorCodes and their associated severity/logLevels with timestamps.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device - Device context variable. A pointer to the device settings structure.
 * \param[out] lastErrorData - structure containing the DFE errorCodes for the severity/logLevels with timestamps
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSvcLogTraceErrLogGet(adi_adrv904x_Device_t* const                device,
                                                                      adi_adrv904x_DfeSvcLogTraceErrLog_t* const  lastErrorData);

 #endif /* _ADI_ADRV904X_DFE_CPU_H_ */

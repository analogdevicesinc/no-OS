/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_cals.h
* \brief Contains ADRV904X processor function prototypes for
*    adi_adrv904x_cals.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_CALS_H_
#define _ADI_ADRV904X_CALS_H_

#include "adi_adrv904x_cals_types.h"
#include "adi_adrv904x_error.h"
#include "adi_adrv904x_cpu_cmd_dc_offset.h"


/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Runs the ADRV904X initialization calibrations
 *
 * \pre This function is called after the device has been initialized, and the RF PLL has been
 * verified to be locked
 *
 * 
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end

 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] initCals A pointer to the InitCals structure which calibrations to run
 *
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsRun(adi_adrv904x_Device_t* const         device,
                                                          const adi_adrv904x_InitCals_t* const initCals);

/**
 * \brief Gets the device initialization calibration status
 *
 * This function provides updates to represent the status of the initialization calibrations
 * calsSincePowerUp, calsLastRun are bit wise representations of calibration status.
 * Each bit represents a calibration. adi_adrv904x_InitCalibrations_e defines the unique initialization
 * calibration bit masks
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[out] initStatus  Pointer passed to obtain init calibrations values.
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsDetailedStatusGet(adi_adrv904x_Device_t* const        device,
                                                                        adi_adrv904x_InitCalStatus_t* const initStatus);

/**
* \brief Performs a Lookup of the Init Cal Status
*
* \pre This function may be called any time after the device has been initialized, and
* initialization calibrations have taken place
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Context variable - A pointer to the device settings structure
* \param[out] initCalErrData  Pointer passed to obtain init calibrations error data.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsDetailedStatusGet_v2( adi_adrv904x_Device_t* const            device,
                                                                            adi_adrv904x_InitCalErrData_t* const    initCalErrData);

/**
 * \brief Blocking waits for the ADRV904X initialization calibrations to complete
 *
 * \pre This function is called after adi_adrv904x_InitCalsRun to wait for init cals to finish
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] timeoutMs A timeout value in milliseconds to wait for the calibrations to complete
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsWait( adi_adrv904x_Device_t* const    device,
                                                            const uint32_t                  timeoutMs);

/**
 * \brief Blocking waits for the initialization calibrations to complete and provides detailed status
 *
 * \pre This function is called after adi_adrv904x_InitCalsRun to wait for init cals to finish
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] timeoutMs A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] initCalErrData  Pointer passed to obtain init calibrations error data
 *                              Note: Optional Parameter (i.e. NULL Pointer passed will still perform the InitCalsWait)
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsWait_v2(  adi_adrv904x_Device_t* const            device,
                                                                const uint32_t                          timeoutMs,
                                                                adi_adrv904x_InitCalErrData_t* const    initCalErrData);

/**
 * \brief Immediate initialization calibration completion check
 *
 * This function is similar to adi_adrv904x_InitCalsWait except it does
 * not block the thread waiting for the enabled init calibrations to complete.
 * This function returns the initialization calibration status immediately
 * allowing the application layer to do other work while the calibrations are
 * running.
 *
 * \pre This function is called after the initialization calibrations have been
 *      started
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[out]  areCalsRunning A pointer to a uint8_t return variable: Returns 1 if
 *                  init cals are running, 0 = init cals are not running (either complete
 *                  or have not started)
 * \param[in] calErrorCheck check for InitCals error if true
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsCheckCompleteGet(adi_adrv904x_Device_t* const device,
                                                                       uint8_t* const               areCalsRunning,
                                                                       const uint8_t                calErrorCheck);

/**
* \brief Immediate initialization calibration completion check
*
* This function is similar to adi_adrv904x_InitCalsWait except it does
* not block the thread waiting for the enabled init calibrations to complete.
* This function returns the initialization calibration status immediately
* allowing the application layer to do other work while the calibrations are
* running.
*
* \pre This function is called after the initialization calibrations have been
*      started
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[out]  areCalsRunning A pointer to a uint8_t return variable: Returns 1 if
*                  init cals are running, 0 = init cals are not running (either complete
*                  or have not started)
 * \param[out] initCalErrData  Pointer passed to obtain init calibrations error data when cals have completed
 *                              Note: Optional Parameter (i.e. NULL Pointer passed will perform the InitCalsCheckCompleteGet)
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsCheckCompleteGet_v2(  adi_adrv904x_Device_t* const            device,
                                                                            uint8_t* const                          areCalsRunning,
                                                                            adi_adrv904x_InitCalErrData_t* const    initCalErrData);


/**
 * \brief Aborts an on-going CPU initialization calibration sequence
 *
 * The CPU init calibrations can take several seconds. This function is called when
 * the BBIC needs to intercede and stop the current initialization calibration sequence.
 * 
 * \pre This function is called any time during the initialization calibration process
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalsAbort(adi_adrv904x_Device_t* const device);


/**
 * \brief Enables or Disables Tracking Calibrations
 *
 * If the cal mask bit corresponding to an unconfigured Tx/Rx/ORx channel is set, that bit is ignored by this
 * function (and the CPU firmware).
 *
 * \pre This command must be called after a full device initialization
 * has taken place, all PLLs are configured and locked, Multi-Chip Sync (MCS) has taken place, and the JESD204B
 * links are configured and operational.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in]     calMask Mask of tracking calibrations to enable or disable. The calibration bit positions
 *                correspond to adi_adrv904x_TrackingCalibrationMask_e.
 * \param[in]     channelMask Mask of channels on which tracking calibrations should be enabled or disabled
 * \param[in]     enableDisableFlag Indicates if calibrations in enableMask should be enabled or disabled
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TrackingCalsEnableSet(adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_TrackingCalibrationMask_t calMask,
                                                                    const uint32_t channelMask,
                                                                    const adi_adrv904x_TrackingCalEnableDisable_e enableDisableFlag);

/**
* \brief Enables or Disables Tracking Calibrations
*
* If the cal mask bit corresponding to an unconfigured Tx/Rx/ORx channel is set, that bit is ignored by this
* function (and the CPU firmware).
*
* \pre This command must be called after a full device initialization
* has taken place, all PLLs are configured and locked, Multi-Chip Sync (MCS) has taken place, and the JESD204B
* links are configured and operational.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in]     calMask Mask of tracking calibrations to enable or disable. The calibration bit positions
*                correspond to adi_adrv904x_TrackingCalibrationMask_e.
* \param[in]     channelMask Mask of channels in structure adi_adrv904x_ChannelTrackingCals_t on which tracking calibrations should be enabled or disabled
* \param[in]     enableDisableFlag Indicates if calibrations in enableMask should be enabled or disabled
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TrackingCalsEnableSet_v2(adi_adrv904x_Device_t* const                     device,
                                                                       const adi_adrv904x_TrackingCalibrationMask_e     calMask,
                                                                       const adi_adrv904x_ChannelTrackingCals_t* const  channelMask,
                                                                       const adi_adrv904x_TrackingCalEnableDisable_e    enableDisableFlag);

/**
* \brief Gets the set of tracking calibrations that are enabled
*
* In the returned set of per-channel bitmasks, each bit represents the enable/disable
* state of one of the calibrations. The calibration bit positions correspond to
* adi_adrv904x_TrackingCalibrationMask_e. The state can be interpreted as 1 = enabled
* and 0 = disabled.
* 
* System calibration (e.g. ADI_ADRV904X_TC_TX_SERDES) state will be returned on channel 0 only
* (even if this channel is not configured).
*
* \pre This command must be called after a full device initialization has taken place.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in,out] enableMasks Pointer to location in which enable/disable state should be placed
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TrackingCalsEnableGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_TrackingCalEnableMasks_t* const enableMasks);

/**
* \brief Allows reading the current state of all tracking calibrations
*
* The returned state information is indexed per channel and per calibration ID (see
* adi_adrv904x_TrackingCalibrationId_e). System calibration (e.g. ADI_ADRV904X_TC_TX_SERDES)
* state will be returned on channel 0 only, even if this channel is not configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in,out] calState Pointer to tracking calibration state structure to store the read values
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TrackingCalAllStateGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_TrackingCalState_t* const calState);


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
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TrackingCalStatusGet(adi_adrv904x_Device_t* const                 device,
                                                                   const adi_adrv904x_TrackingCalibrationMask_e calId,
                                                                   const adi_adrv904x_Channels_e                channel,
                                                                   adi_adrv904x_CalStatus_t* const             calStatus);


/**
  * \brief Returns the status of the init calibration
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
 * \param[in] calId  Indicate a type of cal tracking to get as defined in adi_adrv904x_TrackingCalibrations_e
 * \param[in] channel Channel selection to read back cal status. Must be a single channel.
 * \param[out] calStatus Status of the calibration, as a structure of type adi_adrv904x_CalStatus_t is returned to this pointer address
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_InitCalStatusGet(adi_adrv904x_Device_t* const            device,
                                                               const adi_adrv904x_InitCalibrations_e   calId,
                                                               const adi_adrv904x_Channels_e           channel,
                                                               adi_adrv904x_CalStatus_t* const         calStatus);

/**
  * \brief Gets current Hrm filter coeficients for the provided channels in channelMask.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] channelMask Channel mask for which channels HRM data are to be retrieved. At least one bit in must be set.
 * \param[out] txHrmDataArray points to an array of adi_adrv904x_TxHrmData_t with the number of elements of the array being the number of set bits 
 *                            in channelMask. The first element in the array is the first bit set, starting from LSB. The size is expected to be 
 *                            the number of bits High in channelMask, with min size of array being 1, and max being 8. For example, for channelMask
 *                            equal to 0b01101000, the first element in the array will contain HRM data for Tx3, and the third (and last) will be 
 *                            relative to Tx6.
 * \param[in] arrayLength explicit mention of the number of elements in txHrmDataArray. Must match the number of bits set in channelMask.
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxHrmDataGet(adi_adrv904x_Device_t* const device,
                                                           const uint8_t channelMask,
                                                           adi_adrv904x_TxHrmData_t txHrmDataArray[],
                                                           uint32_t arrayLength);

/**
  * \brief Gets current Hrm filter coeficients for the provided channels in channelMask.
  *        The user should call this function following an LO reprogram that demands a change in HRM coefficients.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] channelMask Channel mask for which channels HRM data are to be retrieved. At least one bit in must be set.
 * \param[in] txHrmDataArray points to an array of adi_adrv904x_TxHrmData_t with the number of elements of the array being the number of set bits 
 *                           in channelMask. The first element in the array is the first bit set, starting from LSB. The size is expected to be 
 *                           the number of bits High in channelMask, with min size of array being 1, and max being 8. For example, for channelMask
 *                           equal to 0b01101000, the first element in the array will contain HRM data for Tx3, and the third (and last) will be 
 *                           relative to Tx6.
 * \param[in] arrayLength explicit mention of the number of elements in txHrmDataArray. Must match the number of bits set in channelMask.
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxHrmDataSet(adi_adrv904x_Device_t* const device,
                                                           const uint8_t channelMask,
                                                           const adi_adrv904x_TxHrmData_t txHrmDataArray[],
                                                           uint32_t arrayLength);



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
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CalPvtStatusGet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_Channels_e channel,
                                                              const uint32_t objId,
                                                              uint8_t  calStatusGet[],
                                                              uint32_t length);

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
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CalSpecificStatusGet(adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_Channels_e channel,
                                                                    const uint32_t objId,
                                                                    uint8_t  calStatusGet[],
                                                                    uint32_t length);

/**
* \brief Enables or disables Digital DC Offset Tracking for one or more channels.
*
* For both parameters use adi_adrv904x_Channels_e to construct the mask parameters.
*
* For example: 
*
* rxChannelMask | rxChannelEnableDisable | Effect
* --------------+------------------------+-------------------------------------------------------
*       0x01    |        0x01            | Enables Rx0. No effect on other channels.
*       0x03    |        0x02            | Disables Rx0, Enables Rx1.  No effect on other channels.
*       0xF0    |        0x00            | Disables Rx4-7. No effect on other channels.
*       0x0F    |        0xFF            | Enables Rx0-3. No effect on other channels.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in] rxChannelMask Mask to select Rx channels to be affected by this function call. A channel whose bit is not
* set in this parameter will not be affected by the call.
* \param[in] rxChannelEnableDisable Each bit indicates whether to enable or disable digital DC offset tracking for the
* corresponding channel.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DigDcOffsetEnableSet(adi_adrv904x_Device_t* const device,
                                                                   const uint32_t rxChannelMask,
                                                                   const uint32_t rxChannelEnableDisable);

/**
* \brief Query if Digital DC Offset Tracking is enabled or disabled for a single Rx channel.
* 
* It is an error to call this function for an Rx channel that is not initialized or for an ORx channel.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in] rxChannel The Rx channel to query. Values indicating an ORx channel are not a valid.
* \param[out] isEnabled Zero is written here if the feature is disabled. Otherwise a non-zero value is written.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DigDcOffsetEnableGet(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_RxChannels_e rxChannel,
                                                                   uint8_t* const isEnabled);

/**
 * \brief Reset Tx LOL
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] txLolReset pointer with special Tx LOL channel mask and reset type
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxLolReset(adi_adrv904x_Device_t* const           device,
                                                         const adi_adrv904x_TxLolReset_t* const txLolReset);

/**
* \brief Reset Tx QEC
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
* \param[in] txQecReset pointer with special Tx QEC channel mask and reset type
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxQecReset(adi_adrv904x_Device_t* const           device,
                                                         const adi_adrv904x_TxQecReset_t* const txQecReset);

/**
 * \brief Set Dig Dc offset configuration for selected channel/s. User can select 1dB corner for
 * DC offset filter through dcOffsetCfg structure. FW calculates the required mshift and multiplier
 * values based on this corner frequency and configures the transceiver device.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in] dcOffSetCfg Dc offset configuration to set
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DigDcOffsetCfgSet(adi_adrv904x_Device_t* const                    device,
                                                                const adi_adrv904x_CpuCmd_SetDcOffset_t* const  dcOffSetCfg);

/**
 * \brief This function reads Dig Dc offset configuration for selected channel. adi_adrv904x_CpuCmd_GetDcOffset_t
 * structure contains the values configured in transceiver device based upon corner frequency selected by
 * adi_adrv904x_DigDcOffsetCfgSet() function call.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param [in,out] device Context variable adi_adrv904x_Device_t pointer to the device data structure
 * \param [in,out] dcOffSetCfg Pointer to Dc offset configuration for readback
 *
 * \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DigDcOffsetCfgGet(adi_adrv904x_Device_t* const                    device,
                                                                adi_adrv904x_CpuCmd_GetDcOffsetResp_t* const    dcOffSetCfg);
#endif /* _ADI_ADRV904X_CALS_H_ */

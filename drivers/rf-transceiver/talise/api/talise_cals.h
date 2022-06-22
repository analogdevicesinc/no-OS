// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_cals.h
 * \brief Contains Talise calibration related function prototypes for
 *        talise_cals.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_CALS_H_
#define TALISE_CALS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_cals_types.h"
#include "talise_tx_types.h"
#include "talise_rx_types.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Runs the Talise initialization calibrations
 *
 * \pre This function is called after the device has been initialized, and the RF PLL has been
 * verified to be locked
 *
 *  calMask Bit | Calibration
 *  ------------|----------------------
 *       0      | Tx BB Filter
 *       1      | ADC Tuner
 *       2      | TIA 3dB Corner
 *       3      | DC Offset
 *       4      | Tx Attenuation Delay
 *       5      | Rx Gain Delay
 *       6      | Flash Cal
 *       7      | Path Delay
 *       8      | Tx LO Leakage Internal
 *       9      | Tx LO Leakage External
 *       10     | Tx QEC Init
 *       11     | LoopBack Rx LO Delay
 *       12     | LoopBack Rx QEC Init
 *       13     | Rx LO Delay
 *       14     | Rx QEC Init
 *       15     | Rx Phase Correction Calibration
 *       16     | ORx LO Delay
 *       17     | ORx QEC Init
 *       18     | Tx DAC Init
 *       19     | ADC Stitching Init
 *     [20-22]  | Reserved
 *       23     | FHM Cals
 *     [24-31]  | Reserved
 *
 * NOTE : Please note that before running Fast Freq Hop Mode cals (FHM cals),
 *        the frequency scan range needs to be set via TALISE_SetFhmConfig() API
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end

 * \param device A pointer to the device settings structure
 * \param calMask A bitmask that informs the Talise ARM processor which calibrations to run
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_runInitCals(taliseDevice_t *device, uint32_t calMask);

/**
 * \brief Blocking waits for the Talise initialization calibrations to complete
 *
 * \pre This function is called after TALISE_runInitCals to wait for them to finish
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device A pointer to the device settings structure
 * \param timeoutMs A timeout value in milliseconds to wait for the calibrations to complete
 * \param errorFlag A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                  Error flag enumerated by taliseInitCalFlags_t. A value of 0 indicates that there
 *                  was no error.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RERUN_INIT_CALS Recovery action to re-run intialization calibrations
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_waitInitCals(taliseDevice_t *device, uint32_t timeoutMs,
			     uint8_t *errorFlag);

/**
 * \brief Immediate initialization calibration completion check
 *
 * This function is similar to TALISE_waitInitCals except it does
 * not block the thread waiting for the enabled init calibrations to complete.
 * This function returns the initialization calibration status immediately
 * allowing the application layer to do other work while the calibrations are
 * running.
 *
 * The *errorFlag parameter is optional. If not used, pass a NULL parameter
 *
 * \pre This function is called after the initialization calibrations have been
 *      started
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device A pointer to the device settings structure
 *
 * \param areCalsRunning A pointer to a uint8_t return variable: Returns 1 if
 *        init cals are running, 0 = init cals are not running (either complete
 *        or have not started)
 *
 * \param errorFlag (Optional, pass NULL if unused) A pointer to a uint8_t
 *        return variable: 3-bit value which is the initialization
 *        calibration error code where 0 = No Error
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RERUN_INIT_CALS Recovery action to re-run intialization calibrations
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_checkInitCalComplete(taliseDevice_t *device,
				     uint8_t *areCalsRunning, uint8_t *errorFlag);

/**
 * \brief Aborts an on-going ARM initialization calibration sequence
 *
 * The ARM init calibrations can take several seconds. This function is called when
 * the BBIC needs to intercede and stop the current initialization calibration sequence.
 * The *calsCompleted uint32_t parameter is optional and will return the completed initialization calibration
 * bit-mask before the abort command was received. If *calsCompleted is passed as a null pointer
 * no value will be returned.
 *
 * \pre This function is called any time during the initialization calibration process
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device A pointer to the device settings structure
 * \param calsCompleted A uint32_t pointer which is passed to obtain a bitmask
 * which describes the initialization calibrations completed during the previous
 * TALISE_runInitCals() function call
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_abortInitCals(taliseDevice_t *device, uint32_t *calsCompleted);

/**
 * \brief Gets the device initialization calibration status
 *
 * This function provides updates to represent the status of the initialization calibrations
 * calsSincePowerUp, calsLastRun, calsMinimum are bit wise representations of calibration status.
 * Each bit represents a calibration. taliseInitCalibrations_t defines the unique initialization
 * calibration bit masks
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device A pointer to the device settings structure
 * \param calsSincePowerUp Pointer passed to obtain value indicating calibrations run since power up
 * \param calsLastRun Pointer passed to obtain value indicating calibrations run in during the previous
 *                    TALISE_runInitCals() call.
 * \param calsMinimum Pointer passed to obtain the value with the minimum calibrations required before allowing
 *                    TALISE_Radio to be successfully called.
 * \param initErrCal Pointer passed to obtain the object ID of the calibration reporting an error
 *                   (this contain the same information as RUN_INIT_ERR_CAL)
 * \param initErrCode Error code reported by the initialization calibration
 *                    with problem (this contains the same information as RUN_INIT_ERR_CODE)
 *
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getInitCalStatus(taliseDevice_t *device,
				 uint32_t *calsSincePowerUp, uint32_t *calsLastRun, uint32_t *calsMinimum,
				 uint8_t *initErrCal, uint8_t *initErrCode);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/**
 * \brief Sets the ARM tracking calibrations which are enabled during the radio ON state
 *
 * \pre This command must be called during radio OFF state and after a full device initialization
 * has taken place, all PLLs are configured and locked, Multi-Chip Sync (MCS) has taken place, and the JESD204B
 * links are configured and operational. If called during radioOn, an error will be returned.
 *
 * enableMask[bit]  |  Bit description
 * -----------------|----------------------
 *             [0]  | TRACK_RX1_QEC
 *             [1]  | TRACK_RX2_QEC
 *             [2]  | TRACK_ORX1_QEC
 *             [3]  | TRACK_ORX2_QEC
 *             [4]  | TRACK_TX1_LOL
 *             [5]  | TRACK_TX2_LOL
 *             [6]  | TRACK_TX1_QEC
 *             [7]  | TRACK_TX2_QEC
 *             [8]  | TRACK_RX1_HD2
 *             [9]  | TRACK_RX2_HD2
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param enableMask The bit-mask which selects the tracking
 * calibration set to run during the radio ON state. taliseTrackingCalibrations_t
 * enumerated types are or'd together to form the tracking calibration
 * mask word.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableTrackingCals(taliseDevice_t *device, uint32_t enableMask);

/**
* \brief Reads back which ARM tracking cals are enabled.
*
* \pre This command is called after a full device initialization to determine
*      which tracking cals are enabled.  The ARM must be loaded and init cals
*      run before this command can be called.
*
* enableMask[bit]  |  Bit description
* -----------------|----------------------
*             [0]  | TRACK_RX1_QEC
*             [1]  | TRACK_RX2_QEC
*             [2]  | TRACK_ORX1_QEC
*             [3]  | TRACK_ORX2_QEC
*             [4]  | TRACK_TX1_LOL
*             [5]  | TRACK_TX2_LOL
*             [6]  | TRACK_TX1_QEC
*             [7]  | TRACK_TX2_QEC
*             [8]  | TRACK_RX1_HD2
*             [9]  | TRACK_RX2_HD2
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param enableMask Pointer to single uint32_t variable that returns a bitmask
*        that shows which tracking cals are enabled.
*
* \retval TALACT_WARN_RESET_LOG Recovery action for log reset
* \retval TALACT_NO_ACTION Function completed successfully, no action required
* \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
* \retval TALACT_ERR_CHECK_PARAM Check enableMask pointer for null value
*/
uint32_t TALISE_getEnabledTrackingCals(taliseDevice_t *device,
				       uint32_t *enableMask);

/**
* \brief Returns the tracking calibration pending and error status.
*
* When in radioOn state, the enabled tracking calibrations will set the pending
* flag when the particular calibration is ready to be run, but has not completed
* yet. If a tracking calibration had an error, the corresponding
* error flag will be asserted. For Tx tracking cals to complete, the Tx to ORx
* mapping must be set to inform the calibrations which transmitters route to which
* observation receivers. Use the getStatus function for the specific calibration
* that threw an error.
*
*  pendingCalMask | bit Description
* ----------------|--------------------------------------------------------
*            [0]  | Rx1 QEC tracking pending
*            [1]  | Rx1 QEC tracking error
*            [2]  | Rx2 QEC tracking pending
*            [3]  | Rx2 QEC tracking error
*            [4]  | ORx1 QEC tracking pending
*            [5]  | ORx1 QEC tracking error
*            [6]  | ORx2 QEC tracking pending
*            [7]  | ORx2 QEC tracking error
*            [8]  | Tx1 LOL tracking pending
*            [9]  | Tx1 LOL tracking error
*           [10]  | Tx2 LOL tracking pending
*           [11]  | Tx2 LOL tracking error
*           [12]  | Tx1 QEC tracking pending
*           [13]  | Tx1 QEC tracking error
*           [14]  | Tx2 QEC tracking pending
*           [15]  | Tx2 QEC tracking error
*
*
* \param device Pointer to the device settings structure
* \param pendingCalMask Pointer to a single uint32_t variable which returns
*        the bit mask that describes which tracking cals are pending or had
*        errors
*
* \retval TALACT_ERR_OK Function completed successfully
* \retval TALACT_ERR_CHECK_PARAM Function parameter pendingCalMask is a NULL pointer
*/
uint32_t TALISE_getPendingTrackingCals(taliseDevice_t *device,
				       uint32_t *pendingCalMask);

/**
* \brief Schedules a tracking calibration to run. Can be used to
* override the tracking calibration timer and force a tracking calibration to run.
* Can be used to reschedule a tracking calibration after a tracking calibration
* error has been detected.  Only one tracking calibration object can be scheduled
* per channel per function call.
*
* \pre Command can be called in either Radio On or Radio Off state.  ARM must be
* initialized.
*
* \dep_begin
* \dep{device->devHalInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param trackingCal Selects the tracking calibration to schedule.
*
* \retval TALACT_WARN_RESET_LOG Recovery action for log reset
* \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
* \retval TALACT_NO_ACTION Function completed successfully, no action required
*/
uint32_t TALISE_rescheduleTrackingCal(taliseDevice_t *device,
				      taliseTrackingCalibrations_t trackingCal);

/**
 * \brief Suspend or resume tracking calibrations in the Radio On ARM state.
 *
 * This function is used to suspend or resume active tracking calibrations
 * for the tracking cals specified in the calSubsetMask. Normally calibrations
 * can only be enabled or disabled while in the radio off state. This function
 * allows pausing tracking cals without leaving the Radio On state. Only
 * enabled tracking calibrations can be resumed or paused.
 *
 * \pre Command can be called in Radio On. The tracking cals of interest
 * must already be enabled while in Radio Off state.
 *
 * calSubsetMask[bit] |  Bit description
 * -------------------|------------
 *          [0]       | TAL_TRACK_RX1_QEC, 1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [1]       | TAL_TRACK_RX2_QEC  1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [2]       | TAL_TRACK_ORX1_QEC 1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [3]       | TAL_TRACK_ORX2_QEC 1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [4]       | TAL_TRACK_TX1_LOL  1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [5]       | TAL_TRACK_TX2_LOL  1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [6]       | TAL_TRACK_TX1_QEC  1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [7]       | TAL_TRACK_TX2_QEC  1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [8]       | TAL_TRACK_RX1_HD2  1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *          [9]       | TAL_TRACK_RX2_HD2  1 = allow pause/resume on this cal, 0 = ignore resumeCalMask setting for this cal
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 *
 * \param calSubsetMask Selects the tracking calibrations of interest to apply
 * the suspend or resume setting to during the radio ON state.
 * taliseTrackingCalibrations_t enumerated types are or'd together to form the
 * cal subset mask word. If the bit for a particular cal is set, the
 * pause/resume mask will be applied for that cal, if the bit is low the
 * pause/resume mask will be ingored for that cal.
 *
 * \param resumeCalMask Bit per tracking cal at same bit position as the cal
 * of interest. If the bit = 1 and calSubsetMask=1 for the same bit, the cal
 * will be resumed if paused. If the bit=0 and calSubsetMask=1, the cal will
 * be paused.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setAllTrackCalState(taliseDevice_t *device,
				    uint32_t calSubsetMask, uint32_t resumeCalMask);

/**
 * \brief Get the Suspended or Resumed state for tracking calibrations
 *
 * This function is used to get the suspend or resume state of all active
 * tracking calibrations and the state is returned in resumeCalMask.
 *
 * \pre Command can be called in Radio On.
 *
 * resumeCalMask[bit]  |  Bit description
 * --------------------|------------
 *           [0]       | TAL_TRACK_RX1_QEC, 1 = cal is resumed (running), 0 = cal is paused
 *           [1]       | TAL_TRACK_RX2_QEC  1 = cal is resumed (running), 0 = cal is paused
 *           [2]       | TAL_TRACK_ORX1_QEC 1 = cal is resumed (running), 0 = cal is paused
 *           [3]       | TAL_TRACK_ORX2_QEC 1 = cal is resumed (running), 0 = cal is paused
 *           [4]       | TAL_TRACK_TX1_LOL  1 = cal is resumed (running), 0 = cal is paused
 *           [5]       | TAL_TRACK_TX2_LOL  1 = cal is resumed (running), 0 = cal is paused
 *           [6]       | TAL_TRACK_TX1_QEC  1 = cal is resumed (running), 0 = cal is paused
 *           [7]       | TAL_TRACK_TX2_QEC  1 = cal is resumed (running), 0 = cal is paused
 *           [8]       | TAL_TRACK_RX1_HD2  1 = cal is resumed (running), 0 = cal is paused
 *           [9]       | TAL_TRACK_RX2_HD2  1 = cal is resumed (running), 0 = cal is paused
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param resumeCalMask Pointer to single uint32_t variable to return the
 * tracking calibration state. If the bit is set then the tracking calibration
 * is resumed and if not set then the tracking cal is suspended, see table
 * above.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getAllTrackCalState(taliseDevice_t *device,
				    uint32_t *resumeCalMask);

/**
 * \brief Returns the status of the TxLOL external tracking calibration
 *
 * The Tx LOL external tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the TxLOL external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param channelSel The channel (Tx1/Tx2) whose status is to be read back
 * \param txLolStatus Status of the TxLOL external calibration, as a structure
 *        of type taliseTxLolStatus_t is returned to this pointer address
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getTxLolStatus(taliseDevice_t *device,
			       taliseTxChannels_t channelSel, taliseTxLolStatus_t *txLolStatus);

/**
 * \brief Returns the status of the TxQEC tracking calibration
 *
 * The Tx QEC tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the TxQEC
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param channelSel The channel (Tx1/Tx2) whose status is to be read back
 * \param txQecStatus Status of the TxQEC external calibration, as a structure
 *        of type taliseTxQecStatus_t is returned to this pointer address
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getTxQecStatus(taliseDevice_t *device,
			       taliseTxChannels_t channelSel, taliseTxQecStatus_t *txQecStatus);

/**
 * \brief Returns the status of the RxQEC tracking calibration
 *
 * The Rx QEC tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the RxQEC external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param channelSel The channel (Rx1/Rx2) whose status is to be read back
 * \param rxQecStatus Status of the RxQEC calibration, as a structure
 *        of type taliseRxQecStatus_t is returned to this pointer address
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRxQecStatus(taliseDevice_t *device,
			       taliseRxChannels_t channelSel, taliseRxQecStatus_t *rxQecStatus);

/**
 * \brief Returns the status of the ORxQEC tracking calibration
 *
 * The ORx QEC tracking calibration is run during the radioOn state.
 * The function can be called to read back the status of the ORxQEC external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.  These functions can be called in radioOff or
 * radioOn state.
 *
 * \param device Pointer to the device settings structure
 * \param channelSel The channel (ORx1/ORx2) whose status is to be read back
 * \param orxQecStatus Status of the ORxQEC external calibration, as a structure
 *        of type taliseOrxQecStatus_t is returned to this pointer address
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getOrxQecStatus(taliseDevice_t *device,
				taliseObsRxChannels_t channelSel, taliseOrxQecStatus_t *orxQecStatus);

/**
* \brief Returns the status of the RxHD2 tracking calibration
*
* The Rx HD2 tracking calibration is run during the radioOn state.
* This function can be called to read back the status of the RxHD2
* calibration including metrics like error codes, percentage of data
* collected for current cal, the performance of the cal and the number of
* times the cal has run and updated the hardware.
*
* \pre Before the function is called, the device must be initialized, the ARM
*      loaded, and init cals run.  These functions can be called in radioOff
*      or radioOn state.
*
* \param device Pointer to the device settings structure
* \param channelSel The channel (Rx1/Rx2) whose status is to be read back
* \param rxHd2Status Status of the RxHD2 calibration, as a structure
*        of type taliseRxHd2Status_t is returned to this pointer address
*
* \retval TALACT_WARN_RESET_LOG Recovery action for log reset
* \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval TALACT_NO_ACTION Function completed successfully, no action required
*/
uint32_t TALISE_getRxHd2Status(taliseDevice_t *device,
			       taliseRxChannels_t channelSel, taliseRxHd2Status_t *rxHd2Status);

/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
 * \brief Performs a blocking wait for a Talise calibration or Pll Lock
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param waitEvent the enum value of the event to wait for
 * \param timeout_us If timeout_us time has passed, function will return with
 *                   an error.
 *
 * \param waitInterval_us To prevent continuous SPI reads blocking the SPI controller
 *                        from other threads, this param sets how often the SPI read
 *                        occurs to poll the event status.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_waitForEvent(taliseDevice_t *device,
			     taliseWaitEvent_t waitEvent, uint32_t timeout_us, uint32_t waitInterval_us);

/**
 * \brief Performs a non-blocking read of wait status for a Talise calibration or Pll Lock
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param waitEvent the enumerated value of the event to read
 * \param eventDone pointer for status return flag where, 0 = not done, 1 = done
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_readEventStatus(taliseDevice_t *device,
				taliseWaitEvent_t waitEvent, uint8_t *eventDone);

/**
  * \brief Performs reset to the External Tx LO Leakage tracking calibration channel estimate
  *
  * \dep_begin
  * \dep{device->devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the Talise device data structure containing settings
  * \param channelSel Enum selects the channel to reset
  *
  * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
  * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval TALACT_ERR_RERUN_INIT_CALS Recovery action for uninitialized radio
  * \retval TALACT_NO_ACTION Function completed successfully, no action required
  */
uint32_t TALISE_resetExtTxLolChannel(taliseDevice_t *device,
				     taliseTxChannels_t channelSel);

/**
  * \brief Allows setting configuration options for Talise Rx HD2 tracking calibration
  *
  * Set hd2CalConfig->posSideBandSel to the side (negative - lower side band or
  * positive - upper side band) that has the desired
  * signal.  Correction will be correcting the 2nd harmonic distortion on the
  * same side of the spectrum.
  *
  * \pre This function can only be called when the Talise ARM is in the Radio Off state (or
  * before init cals have been run).
  *
  * \dep_begin
  * \dep{device->devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the Talise device data structure containing settings
  * \param hd2CalConfig Pointer to HD2 calibration config structure with desired settings
  *
  * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
  * \retval TALACT_ERR_RESET_ARM Recovery action if ARM is detected to be in wrong state
  * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval TALACT_NO_ACTION Function completed successfully, no action required
  */
uint32_t TALISE_setRxHd2Config(taliseDevice_t *device,
			       taliseRxHd2Config_t *hd2CalConfig);

/**
  * \brief Allows reading configuration options for Talise Rx HD2 tracking calibration from
  * Talise ARM memory
  *
  * Config values are read from the Talise ARM memory and returned at the address of
  * the hd2CalConfig function parameter.
  *
  * \pre This function can only be called when the Talise ARM is in the Radio Off state (or
  * before init cals have been run).
  *
  * \dep_begin
  * \dep{device->devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the Talise device data structure containing settings
  * \param hd2CalConfig Pointer to HD2 calibration config structure to return read settings
  *
  * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
  * \retval TALACT_ERR_RESET_ARM Recovery action if ARM is detected to be in wrong state
  * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval TALACT_NO_ACTION Function completed successfully, no action required
  */
uint32_t TALISE_getRxHd2Config(taliseDevice_t *device,
			       taliseRxHd2Config_t *hd2CalConfig);

/**
  * \brief Sets the Rx/ORx channel Digital DC offset convergence time (mShift)
  *
  * This function allows the BBIC to adjust the digital DC offset convergence
  * time.  This value (mShift) affects the corner frequency of the notch filter
  * used to filter the DC offset out of the Rx/ORx receive signal. The mShift
  * value applies to all Rx channels or all ORx channels, but Rx and ORx
  * can be set individually.
  *
  * In ARM versions less than 4.0, the ARM init cal for DC offset will override
  * the mShift setting. The BBIC should call this function after running the
  * init cals to make sure the ARM does not overwrite a custom mShift setting.
  *
  * In ARM versions 4.0 or greater, the ARM boot up will set a default calculated
  * value for the mShift based on the selected profile sample rates.
  * The BBIC may set the mShift anytime after ARM bootup
  * (before or after ARM init cals).  The ARM DC offset init function will
  * save and restore the custom mShift value if it needs to change the mShift
  * value.
  *
  * The mShift value range is limited to 8 to 20 (decimal) to prevent major
  * performance issues.
  *
  * \pre ARM version less than 4.0: This function can only be called after the
  * Talise ARM init cals have been run. For 4.0 and greater, the function
  * can be called after loading the ARM.
  *
  * \dep_begin
  * \dep{device->devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the Talise device data structure containing settings
  * \param channel Digital DC offset channel to adjust mShift value for
  * \param mShift Value to set for the given channel (valid range 8 - 20)
  *
  * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
  * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval TALACT_NO_ACTION Function completed successfully, no action required
  */
uint32_t TALISE_setDigDcOffsetMShift(taliseDevice_t *device,
				     taliseDcOffsetChannels_t channel, uint8_t mShift);

/**
  * \brief Gets the Rx/ORx channel Digital DC offset convergence time (mShift)
  *
  * This function allows the BBIC to read back the digital DC offset convergence
  * time.  This value (mShift) affects the corner frequency of the notch filter
  * used to filter the DC offset out of the Rx/ORx receive signal. The mShift
  * value applies to all Rx channels or all ORx channels, but Rx and ORx
  * can be set individually.
  *
  * In ARM versions less than 4.0, the ARM init cal for DC offset will override
  * the mShift setting. The BBIC should call this function after running the
  * init cals to make sure the ARM does not overwrite a custom mShift setting.
  *
  * In ARM versions 4.0 or greater, the ARM boot up will set a default calculated
  * value for the mShift based on the selected profile sample rates.
  * The BBIC may set the mShift anytime after ARM bootup
  * (before or after ARM init cals).  The ARM DC offset init function will
  * save and restore the custom mShift value if it needs to change the mShift
  * value.
  *
  * The mShift value range is limited to 8 to 20 (decimal) to prevent major
  * performance issues.
  *
  * \pre ARM version less than 4.0: This function can only be called after the
  * Talise ARM init cals have been run. For 4.0 and greater, the function
  * can be called after loading the ARM.
  *
  * \dep_begin
  * \dep{device->devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the Talise device data structure containing settings
  * \param channel Digital DC offset channel to read mShift value for
  * \param mShift Pointer to store the current mShift value for the requested channel
  *
  * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
  * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval TALACT_NO_ACTION Function completed successfully, no action required
  */
uint32_t TALISE_getDigDcOffsetMShift(taliseDevice_t *device,
				     taliseDcOffsetChannels_t channel, uint8_t *mShift);

/**
 * \brief Enable/ Disable Digital DC Offset channels using the channel mask.
 *  The mask can be a combination of the following channel values ( ::taliseRxDcOffsettEn_t ).
 *
 *     Channel              |  Value  |  Channel description
 * -------------------------|---------|--------------------------
 *  TAL_DC_OFFSET_ALL_OFF   |   0x00  | Disable all the channels
 *  TAL_DC_OFFSET_RX1       |   0x01  | Enables Rx1
 *  TAL_DC_OFFSET_RX2       |   0x02  | Enables Rx1
 *  TAL_DC_OFFSET_ORX1      |   0x04  | Enables ORx1
 *  TAL_DC_OFFSET_ORX2      |   0x08  | Enables ORx2
 *  TAL_DC_OFFSET_ALL_ON    |   0x0F  | Enables all the channels
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param enableMask with bits of channels to be enabled.
 *
 * \retval TALISE_ERR_DIG_DC_OFFSET_INV_ENABLE_MASK enable mask passed to the function is invalid, refer taliseRxDcOffsettEn_t enum.
 * \retval TALISE_ERR_OK Function completed successfully
 */
uint32_t TALISE_setDigDcOffsetEn(taliseDevice_t *device, uint8_t enableMask);

/**
 * \brief Reads back Enable/ Disable channels Digital DC Offset and returns a mask of it.
 * The mask returned will be a combination of the following channel values ( ::taliseRxDcOffsettEn_t ).
 * By default RX BBDC tracking is ON and ORX BBDC tracking is OFF.This API function can be used to change default behavior.
 *
 *    Channel               |  Value  |  Channel description
 * -------------------------|---------|--------------------------
 *  TAL_DC_OFFSET_ALL_OFF   |   0x00  | All channels are disabled
 *  TAL_DC_OFFSET_RX1       |   0x01  | Rx1 is enabled
 *  TAL_DC_OFFSET_RX2       |   0x02  | Rx2 is enabled
 *  TAL_DC_OFFSET_ORX1      |   0x04  | ORx1 is enabled
 *  TAL_DC_OFFSET_ORX2      |   0x08  | ORx2 is enabled
 *  TAL_DC_OFFSET_ALL_ON    |   0x0F  | All channels are enabled
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param enableMask pointer to the variable to store Enable mask of channels
 *
 * \retval TALISE_ERR_DIG_DC_OFFSET_NULL_ENABLE_MASK enableMask is NULL
 * \retval TALISE_ERR_OK Function completed successfully
 */
uint32_t TALISE_getDigDcOffsetEn(taliseDevice_t *device, uint8_t *enableMask);

/**
  * \brief Allows reading Talise tracking calibration batch size configuration option
  * from Talise ARM memory
  *
  * Config value is read from the Talise ARM memory and returned at the address of
  * the batchsize_us enum function parameter.
  *
  * \pre This function can only be called when the Talise ARM is in the RadioOff/IDLE state or ready state.
  *
  * \dep_begin
  * \dep{device->devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the Talise device data structure containing settings
  * \param batchsize_us Pointer to Tracking calibration batch size enum to return read settings
  *
  * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
  * \retval TALACT_ERR_RESET_ARM Recovery action if ARM is detected to be in wrong state
  * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval TALACT_NO_ACTION Function completed successfully, no action required
  */
uint32_t TALISE_getTrackingCalsBatchSize(taliseDevice_t *device,
		taliseTrackingCalBatchSize_t* batchsize_us);

/**
  * \brief Modifies the tracking calibrations batch capture size from 500us to 200us and vice versa.
  * Defualt value for batch size(Minimum internal Calibration, Rx Caibration, Tx Calibration and
  * Orx Calibration period) is 500us.
  *
  * \pre This function can only be called when the Talise ARM is in the RadioOff/IDLE state or ready state.
  *
  * <B>Dependencies</B>
  * - device->spiSettings->chipSelectIndex
  *
  * \param device Pointer to the Talise device data structure containing settings
  * \param batchsize_us variable to store Tracking Cal batch size.
  *
  * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
  * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
  * \retval TALACT_NO_ACTION Function completed successfully, no action required
  */
uint32_t TALISE_setTrackingCalsBatchSize(taliseDevice_t *device,
		taliseTrackingCalBatchSize_t batchsize_us);
/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

/**
 * \brief Private Helper function to return ARM error strings to the
 *        TALISE_getErrorMessage function.
 *
 *  This is a private function and is automatically called by the API.  BBIC
 *  should use the TALISE_getErrorMessage function.
 *
 * \param errSrc A value that represents the error source from the Talise API
 * \param errCode Error code that along with the error source allows looking up
 *        a specific error message.
 *
 * \retval Returns a character array with the error message specified by
 *         errSrc and errCode.
 */
const char* talGetCalErrorMessage(uint32_t errSrc, uint32_t errCode);

#define ARMINITCAL_ERRCODE(armCalId, armCalErrCode) ((armCalId << 8) | armCalErrCode)

#ifdef __cplusplus
}
#endif

#endif /* TALISE_CALS_H_ */

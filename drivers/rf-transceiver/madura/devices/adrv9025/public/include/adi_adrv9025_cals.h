/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_cals.h
 * \brief Contains ADRV9025 calibration related function prototypes for
 *        adi_adrv9025_cals.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_CALS_H_
#define _ADI_ADRV9025_CALS_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_cals_types.h"
#include "adi_adrv9025_tx_types.h"
#include "adi_adrv9025_rx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Runs the ADRV9025 initialization calibrations
 *
 * \pre This function is called after the device has been initialized, and the RF PLL has been
 * verified to be locked
 *
 *  calMask Bit | Calibration
 *  ------------|----------------------
 *       0      | Tx BB Filter
 *       1      | ADC Tuner
 *       2      | Rx TIA 3dB Corner
 *       3      | Orx TIA 3dB Corner
 *       4      | Loopback Rx TIA 3dB Corner
 *       5      | Rx DC Offset
 *       6      | Orx DC Offset
 *       7      | Loopback Rx DC Offset
 *       8      | Flash converter comparator
 *       9      | Path delay equalization
 *       10     | Internal Tx LO leakage
 *       11     | External Tx LO leakage
 *       12     | Tx quadrature error correction
 *       13     | Loopback Rx LO delay path
 *       14     | Loopback Rx quadrature error correction
 *       15     | Rx LO delay path
 *       16     | Rx quadrature error correction
 *       17     | ORx LO delay path
 *       18     | ORx quadrature error correction
 *       19     | Tx DAC passband
 *       20     | Digital Pre Distortion
 *       21     | Closed Loop Gain Control
 *       22     | Closed Loop Gain Control
 *       23     | Harmonic Distortion
 *       24     | Tx attenuation delay
 *       25     | Tx Attenuation
 *       26     | Rx gain delay
 *       27     | Rx Gain and Phase
 *     [28-31]  | Reserved
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end

 * \param device A pointer to the device settings structure
 * \param initCals A pointer to the InitCals structure which calibrations to run
 *
 * \retval ADI_ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADI_COMMON_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_InitCalsRun(adi_adrv9025_Device_t*   device,
                                 adi_adrv9025_InitCals_t* initCals);

/**
 * \brief Blocking waits for the ADRV9025 initialization calibrations to complete
 *
 * \pre This function is called after adi_adrv9025_runInitCals to wait for them to finish
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device A pointer to the device settings structure
 * \param timeoutMs A timeout value in milliseconds to wait for the calibrations to complete
 * \param errorFlag A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                  A value of 0 indicates that there was no error.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset
 * \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_COMMON_ACT_ERR_RERUN_INIT_CALS Recovery action to re-run initialization calibrations
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_InitCalsWait(adi_adrv9025_Device_t* device,
                                  uint32_t               timeoutMs,
                                  uint8_t*               errorFlag);

/**
 * \brief Immediate initialization calibration completion check
 *
 * This function is similar to adi_adrv9025_waitInitCals except it does
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
 * \dep{device->common.devHalInfo}
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
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_ERR_RERUN_INIT_CALS Recovery action to re-run initialization calibrations
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_InitCalsCheckCompleteGet(adi_adrv9025_Device_t* device,
                                              uint8_t*               areCalsRunning,
                                              uint8_t*               errorFlag);

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
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device A pointer to the device settings structure
 * \param calsCompleted A uint32_t pointer which is passed to obtain a bitmask
 * which describes the initialization calibrations completed during the previous
 * adi_adrv9025_runInitCals() function call
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_InitCalsAbort(adi_adrv9025_Device_t* device,
                                   uint32_t*              calsCompleted);

/**
 * \brief Gets the device initialization calibration status
 *
 * This function provides updates to represent the status of the initialization calibrations
 * calsSincePowerUp, calsLastRun are bit wise representations of calibration status.
 * Each bit represents a calibration. adi_adrv9025_InitCalibrations_e defines the unique initialization
 * calibration bit masks
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device A pointer to the device settings structure
 * \param initStatus  Pointer passed to obtain init calibrations values.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_InitCalsDetailedStatusGet(adi_adrv9025_Device_t*        device,
                                               adi_adrv9025_InitCalStatus_t* initStatus);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/**
 * \brief Enables or Disables Calibration Tracking.
 *        This API expects a 64 bit-mask, each bit in the mask controls one of the calibrations.
 *        The corresponding calibrations for the bits that are set to 1 will be enabled or disabled.
 *        0s in the mask are ignored. enableDisableFlag parameter indicates the enable/disable operation.
 *
 *        Example : To ENABLE TRACKING_CAL_RX1_QEC and TRACKING_CAL_TX1_QEC
 *                  This API should be called with
 *			    mask = 0x0101, enableDisableFlag = ADI_ADRV9025_TRACKING_CAL_ENABLE
 *			        To DISABLE TRACKING_CAL_RX1_QEC and TRACKING_CAL_TX1_QEC
 *			        This API should be called with
 *			        mask = 0x0101, enableDisableFlag = ADI_ADRV9025_TRACKING_CAL_DISABLE
 *
 *       Note: Observe that the mask can be same to enable and disable calibrations,
 *	           the enableDisableFlag should be used to indicate whether user wants to enable or disable.
 *
 *
 *     Bits          | Corresponding Calibration
 *     [64 bits]     |
 *    ----------------------------------------
 *      0	         | TRACKING_CAL_RX1_QEC
 *      1	         | TRACKING_CAL_RX2_QEC
 *      2	         | TRACKING_CAL_RX3_QEC
 *      3	         | TRACKING_CAL_RX4_QEC
 *      4	         | TRACKING_CAL_ORX1_QEC
 *      5	         | TRACKING_CAL_ORX2_QEC
 *      6	         | TRACKING_CAL_ORX3_QEC
 *      7	         | TRACKING_CAL_ORX4_QEC
 *      8	         | TRACKING_CAL_TX1_LOL
 *      9	         | TRACKING_CAL_TX2_LOL
 *      10           | TRACKING_CAL_TX3_LOL
 *      11           | TRACKING_CAL_TX4_LOL
 *      12           | TRACKING_CAL_TX1_QEC
 *      13           | TRACKING_CAL_TX2_QEC
 *      14           | TRACKING_CAL_TX3_QEC
 *      15           | TRACKING_CAL_TX4_QEC
 *      16           | TRACKING_CAL_TX1_DPD
 *      17           | TRACKING_CAL_TX2_DPD
 *      18           | TRACKING_CAL_TX3_DPD
 *      19           | TRACKING_CAL_TX4_DPD
 *      20           | TRACKING_CAL_TX1_CLGC
 *      21           | TRACKING_CAL_TX2_CLGC
 *      22           | TRACKING_CAL_TX3_CLGC
 *      23           | TRACKING_CAL_TX4_CLGC
 *      24           | TRACKING_CAL_TX1_VSWR
 *      25           | TRACKING_CAL_TX2_VSWR
 *      26           | TRACKING_CAL_TX3_VSWR
 *      27           | TRACKING_CAL_TX4_VSWR
 *      28           | TRACKING_CAL_RX1_HD2
 *      29           | TRACKING_CAL_RX2_HD2
 *      30           | TRACKING_CAL_RX3_HD2
 *      31           | TRACKING_CAL_RX4_HD2
 *      32           | TRACKING_CAL_SERDES
 *      33-64        | Reserved
 *
 * If the cal mask corresponding to an uninitialized Tx/Rx/ORx channel is set, this function automatically
 * clears the cal mask before commanding the ARM to enable/disable the requested cals
 *
 * \pre This command must be called after a full device initialization
 * has taken place, all PLLs are configured and locked, Multi-Chip Sync (MCS) has taken place, and the JESD204B
 * links are configured and operational.
 *
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param enableMask The 64 bit-mask to enable/ disable calibration.
 * \param enableDisableFlag to indicate whether the mask is to be used to enable or disable.
 *        Possible values are ADI_ADRV9025_TRACKING_CAL_ENABLE or ADI_ADRV9025_TRACKING_CAL_DISABLE
 *        from adi_adrv9025_TrackingCalEnableDisable_e
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for user ARM reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_TrackingCalsEnableSet(adi_adrv9025_Device_t*                  device,
                                           uint64_t                                enableMask,
                                           adi_adrv9025_TrackingCalEnableDisable_e enableDisableFlag);

/**
* \brief Reads back which ARM tracking cals are enabled.
*        In the returned 64 bit-mask, each bit represents enable/disable state of one of the calibrations.
*        state can be interpreted as 1 = enabled and 0 = disabled.
*
* \pre This command is called after a full device initialization to determine
*      which tracking cals are enabled.  The ARM must be loaded and init cals
*      run before this command can be called.
*
*     Bits          | Corresponding Calibration
*     [32 bits]     |
*    ----------------------------------------
*      0	         | TRACKING_CAL_RX1_QEC
*      1	         | TRACKING_CAL_RX2_QEC
*      2	         | TRACKING_CAL_RX3_QEC
*      3	         | TRACKING_CAL_RX4_QEC
*      4	         | TRACKING_CAL_ORX1_QEC
*      5	         | TRACKING_CAL_ORX2_QEC
*      6	         | TRACKING_CAL_ORX3_QEC
*      7	         | TRACKING_CAL_ORX4_QEC
*      8	         | TRACKING_CAL_TX1_LOL
*      9	         | TRACKING_CAL_TX2_LOL
*      10           | TRACKING_CAL_TX3_LOL
*      11           | TRACKING_CAL_TX4_LOL
*      12           | TRACKING_CAL_TX1_QEC
*      13           | TRACKING_CAL_TX2_QEC
*      14           | TRACKING_CAL_TX3_QEC
*      15           | TRACKING_CAL_TX4_QEC
*      16           | TRACKING_CAL_TX1_DPD
*      17           | TRACKING_CAL_TX2_DPD
*      18           | TRACKING_CAL_TX3_DPD
*      19           | TRACKING_CAL_TX4_DPD
*      20           | TRACKING_CAL_TX1_CLGC
*      21           | TRACKING_CAL_TX2_CLGC
*      22           | TRACKING_CAL_TX3_CLGC
*      23           | TRACKING_CAL_TX4_CLGC
*      24           | TRACKING_CAL_TX1_VSWR
*      25           | TRACKING_CAL_TX2_VSWR
*      26           | TRACKING_CAL_TX3_VSWR
*      27           | TRACKING_CAL_TX4_VSWR
*      28           | TRACKING_CAL_RX1_HD2
*      29           | TRACKING_CAL_RX2_HD2
*      30           | TRACKING_CAL_RX3_HD2
*      31           | TRACKING_CAL_RX4_HD2
*      32           | TRACKING_CAL_SERDES
*      33-64        | Reserved
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param enableMask Pointer to single uint64_t variable that returns a bitmask
*        that shows which tracking cals are enabled.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for user ARM reset required
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Check enableMask pointer for null value
*/
int32_t adi_adrv9025_TrackingCalsEnableGet(adi_adrv9025_Device_t* device,
                                           uint64_t*              enableMask);

/**
* \brief Returns the status of the Deserializer tracking calibration
*
* The function can be called to read back the status of the Deserializer
* calibration including metrics like error codes, percentage of data
* collected for current cal, the performance of the cal and the number of
* times the cal has run and updated the hardware. The varianceMetric is not currently
* applicable for this tracking cal.
*
* \pre Before the function is called, the device must be initialized, the ARM
* loaded, and init cals run.
*
* \param device Pointer to the device settings structure
* \param desLane The deserializer lane whose status is to be read back
* \param desStatus Status of the Deserializer tracking cal, as a structure
*        of type adi_adrv9025_DesTrackingCalStatus_t is returned to this pointer address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TrackingCalDeserializerStatusGet(adi_adrv9025_Device_t*               device,
                                                      uint8_t                              desLane,
                                                      adi_adrv9025_DesTrackingCalStatus_t* desStatus);

/**
 * \brief Returns the status of the TxLOL external tracking calibration
 *
 * The function can be called to read back the status of the TxLOL external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.
 *
 * \param device Pointer to the device settings structure
 * \param txChannel The channel (Tx1/Tx2/Tx3/Tx4) whose status is to be read back
 * \param txLolStatus Status of the TxLOL external calibration, as a structure
 *        of type adi_adrv9025_TxLolStatus_t is returned to this pointer address
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_TrackingCalTxLolStatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_TxChannels_e   txChannel,
                                               adi_adrv9025_TxLolStatus_t* txLolStatus);

/**
 * \brief Returns the status of the TxQEC tracking calibration
 *
 * The function can be called to read back the status of the TxQEC
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.
 *
 * \param device Pointer to the device settings structure
 * \param txChannel The channel (Tx1/Tx2) whose status is to be read back
 * \param txQecStatus Status of the TxQEC external calibration, as a structure
 *        of type adi_adrv9025_TxQecStatus_t is returned to this pointer address
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_TrackingCalTxQecStatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_TxChannels_e   txChannel,
                                               adi_adrv9025_TxQecStatus_t* txQecStatus);

/**
 * \brief Returns the status of the RxQEC tracking calibration
 *
 * The function can be called to read back the status of the RxQEC external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, the performance of the cal and the number of
 * times the cal has run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.
 *
 * \param device Pointer to the device settings structure
 * \param rxChannel The channel (Rx1/Rx2/Rx3/Rx4) whose status is to be read back
 * \param rxQecStatus Status of the RxQEC calibration, as a structure
 *        of type adi_adrv9025_RxQecStatus_t is returned to this pointer address
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_TrackingCalRxQecStatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_RxChannels_e   rxChannel,
                                               adi_adrv9025_RxQecStatus_t* rxQecStatus);

/**
* \brief Returns the status of the ORxQEC tracking calibration
*
* The function can be called to read back the status of the ORxQEC external
* calibration including metrics like error codes, percentage of data
* collected for current cal, the performance of the cal and the number of
* times the cal has run and updated the hardware.
*
* \pre Before the function is called, the device must be initialized, the ARM
* loaded, and init cals run.
*
* \param device Pointer to the device settings structure
* \param orxChannel The channel (Orx1/Orx2/Orx3/Orx4) whose status is to be read back
* \param orxQecStatus Status of the ORxQEC external calibration, as a structure
*        of type adi_adrv9025_RxQecStatus_t is returned to this pointer address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TrackingCalOrxQecStatusGet(adi_adrv9025_Device_t*       device,
                                                adi_adrv9025_RxChannels_e    orxChannel,
                                                adi_adrv9025_OrxQecStatus_t* orxQecStatus);

/**
* \brief Returns the status of the CLGC tracking calibration
*
* The function can be called to read back the status of the CLGC tracking
* calibration including metrics like error codes, percentage of data
* collected for current cal, the performance of the cal and the number of
* times the cal has run and updated the hardware.
*
* \pre Before the function is called, the device must be initialized, the ARM
* loaded, and init cals run.
*
* \param device Pointer to the device settings structure
* \param txChannel The channel (Tx1/Tx2/Tx3/Tx4) whose status is to be read back
* \param clgcStatus Status of the CLGC tracking calibration, as a structure
*        of type adi_adrv9025_ClgcStatus_t is returned to this pointer address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TrackingCalClgcStatusGet(adi_adrv9025_Device_t*                device,
                                              adi_adrv9025_TxChannels_e             txChannel,
                                              adi_adrv9025_ClgcTrackingCalStatus_t* clgcStatus);

/**
 * \brief Returns the status of the RxHd2 tracking calibration
 *
 * The function can be called to read back the status of the RxHd2 external
 * calibration including metrics like error codes, percentage of data
 * collected for current cal, Confidence level that cal has accurately identified
 * the coefficient required to cancel HD2 and the number of times the cal has
 * run and updated the hardware.
 *
 * \pre Before the function is called, the device must be initialized, the ARM
 * loaded, and init cals run.
 *
 * \param device Pointer to the device settings structure
 * \param rxChannel The channel (Rx1/Rx2/Rx3/Rx4) whose status is to be read back
 * \param rxHd2Status Status of the RxHd2 calibration, as a structure
 *        of type adi_adrv9025_RxHd2Status_t is returned to this pointer address
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_TrackingCalRxHd2StatusGet(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_RxChannels_e   rxChannel,
                                               adi_adrv9025_RxHd2Status_t* rxHd2Status);

/**
  * \brief Allows setting configuration options for ADRV9025 Rx HD2 tracking calibration
  *
  * Set hd2CalConfig->posSideBandSel to the side (negative - lower side band or
  * positive - upper side band) that has the desired
  * signal.  Correction will be correcting the 2nd harmonic distortion on the
  * same side of the spectrum.
  *
  * \dep_begin
  * \dep{device->common.devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the ADRV9025 device data structure containing settings
  * \param hd2CalConfig Pointer to HD2 calibration config structure with desired settings
  *
  * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
  * \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action if ARM is detected to be in wrong state
  * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
  */
int32_t adi_adrv9025_TrackingCalRxHd2CfgSet(adi_adrv9025_Device_t*      device,
                                            adi_adrv9025_RxHd2Config_t* hd2CalConfig);

/**
  * \brief Allows reading configuration options for ADRV9025 Rx HD2 tracking calibration from
  * ADRV9025 ARM memory
  *
  * Config values are read from the ADRV9025 ARM memory and returned at the address of
  * the hd2CalConfig function parameter.
  *
  *
  * \dep_begin
  * \dep{device->common.devHalInfo}
  * \dep_end
  *
  * \param device Pointer to the ADRV9025 device data structure containing settings
  * \param hd2CalConfig Pointer to HD2 calibration config structure to return read settings
  *
  * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
  * \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action if ARM is detected to be in wrong state
  * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
  * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
  */
int32_t adi_adrv9025_TrackingCalRxHd2CfgGet(adi_adrv9025_Device_t*      device,
                                            adi_adrv9025_RxHd2Config_t* hd2CalConfig);

/**
* \brief Allows reading the current state of all calibrations.
*        The possible states for each calibration depending on the returned value are
*
*        BIT Number	|  STATE	   |   Description
*        ------------------------------------------------------------------------------
*           0	    |  SUSPENDED   |   Cal is temporarily suspended by the framework.
*           1	    |  RESUMED	   |   Cal is resumed for running.
*           2	    |  INACTIVE	   |   Cal's Main function is not executing
*           3	    |  RUNNING	   |   Cal's Main function is executing
*           4	    |  ENABLED	   |   Cal is enabled
*           5	    |  DISABLED	   |   Cal is disabled
*           6	    |  ERROR	   |   Cal has generated error. It may be suspended from running.
*                   |              |   BBIC should re-run the cal.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param calState Pointer to Tracking calibration state structure to store the read values.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action if ARM is detected to be in wrong state
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TrackingCalAllStateGet(adi_adrv9025_Device_t*           device,
                                            adi_adrv9025_TrackingCalState_t* calState);

/**
* \brief Configures the Tx-ORx path delay used to align Tx and ORx samples
*
* This function allows the user to manually overwrite the Tx-ORx external path delay
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \note If the Tx-ORx external routing is not the same as default Tx-ORx mapping, then the user will have to
*       call the API adi_adrv9025_TxToOrxMappingSet() in pin mode or switch the appropriate GPIO pins
*       in a GPIO Tx to ORx mapping control mode before executing this API in order to convey the
*       Tx-ORx mapping to the ADRV9025 firmware.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which the Tx-ORx path delay is required to be configured
* \param externalPathDelay Structure consisting of the path delay settings to be configured
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExternalPathDelaySet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_ExternalPathDelay_t* externalPathDelay);

/**
* \brief Retrieves the current Tx-ORx path delay value used to align Tx and ORx samples
*
* This function returns the current Tx-ORx external path delay calibrated by the ARM or written manually by the user
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the path delay init cal has run.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which the Tx-ORx path delay is required to be configured
* \param externalPathDelay Structure consisting of the path delay settings to be configured
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExternalPathDelayGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_ExternalPathDelay_t* externalPathDelay);

/**
* \brief Configures the supplied mShift value (0-31) to the supplied channel selection.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param device Pointer to the device settings structure
* \param dcOffSetMShift structure containing mShift channel and value
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DigDcOffsetMShiftSet(adi_adrv9025_Device_t*  device,
                                          adi_adrv9025_DcOffSet_t dcOffSetMShift);

/**
* \brief Retrieves the mShift value for the supplied channel selection.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param device Pointer to the device settings structure
* \param dcOffSetMShift pointer to the structure where the mShift value
*        is returned for the supplied channel selection.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DigDcOffsetMShiftGet(adi_adrv9025_Device_t*   device,
                                          adi_adrv9025_DcOffSet_t* dcOffSetMShift);

/**
* \brief Sets the Enable/Disable Digital DC Offset using the adi_adrv9025_DcOffsetChannels_e to
*  construct as channel mask.
*  The mask can be a combination of the following channel values ( ::adi_adrv9025_DcOffsetChannels_e ).
*
*           Channel                             |   Value   |  Channel description
* ----------------------------------------------|-----------|--------------------------
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH0         |   0x0001  | MShift and DcOffset RX channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH1         |   0x0002  | MShift and DcOffset RX channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH2         |   0x0004  | MShift and DcOffset RX channel2 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH3         |   0x0008  | MShift and DcOffset RX channel3 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH0        |   0x0010  | MShift and DcOffset ORX channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH1        |   0x0020  | MShift and DcOffset ORX channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH0       |   0x0040  | MShift and DcOffset ORX2 mode channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH1       |   0x0080  | MShift and DcOffset ORX2 mode channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH0 |   0x0100  | MShift and DcOffset ORX LpBack1 channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH1 |   0x0200  | MShift and DcOffset ORX LpBack1 channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH0 |   0x0400  | MShift and DcOffset ORX LpBack2 channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH1 |   0x0400  | MShift and DcOffset ORX LpBack2 channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ALL_CHANNELS   |   0x0FFF  | MShift and DcOffset All Channels selected
*
* <B>Dependencies</B>
* - device->spiSettings->chipSelectIndex
*
* \param device Pointer to the device settings structure
* \param channelDisableEnableMask with bits of channels to be enabled if set and disabled if cleared.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DigDcOffsetEnableSet(adi_adrv9025_Device_t* device,
                                          uint16_t               channelDisableEnableMask);

/**
* \brief Returns Enable/Disable Digital DC Offset mask representing the enable/disable status of each
*  digital DC Offset channel where a 0 is disabled and a 1 is disable using the following channel mapping:
*
*           Channel                             |   Value   |  Channel description
* ----------------------------------------------|-----------|--------------------------
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH0         |   0x0001  | MShift and DcOffset RX channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH1         |   0x0002  | MShift and DcOffset RX channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH2         |   0x0004  | MShift and DcOffset RX channel2 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH3         |   0x0008  | MShift and DcOffset RX channel3 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH0        |   0x0010  | MShift and DcOffset ORX channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH1        |   0x0020  | MShift and DcOffset ORX channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH0       |   0x0040  | MShift and DcOffset ORX2 mode channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH1       |   0x0080  | MShift and DcOffset ORX2 mode channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH0 |   0x0100  | MShift and DcOffset ORX LpBack1 channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH1 |   0x0200  | MShift and DcOffset ORX LpBack1 channel1 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH0 |   0x0400  | MShift and DcOffset ORX LpBack2 channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH1 |   0x0400  | MShift and DcOffset ORX LpBack2 channel0 select
*  ADI_ADRV9025_MSHIFT_DC_OFFSET_ALL_CHANNELS   |   0x0FFF  | MShift and DcOffset All Channels selected
*
* <B>Dependencies</B>
* - device->spiSettings->chipSelectIndex
*
* \param device Pointer to the device settings structure
* \param channelDisableEnableMask point with bits representing the enable/disable status of each channel.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DigDcOffsetEnableGet(adi_adrv9025_Device_t* device,
                                          uint16_t*              channelDisableEnableMask);

/**
* \brief Resets the External Tx LO Leakage estimate for the specified channel mask with two types of reset,
* Hard reset and Soft reset.
*
* Both reset types resets the perturbation scaler, clears accumulated RX and TX values, sets fast attack=1 to force a fast attack
* and applies the measurements more strongly.
*
* Hard reset erases the TxLOL look-up table.  The look-up table keeps the correction values for all 64 attenuation values.
* Soft reset restarts the TxLOL estimation without erasing the TxLOL look-up table.
*
* \param device Pointer to the device settings structure
* \param txLolReset Pointer to structure containing channel mask and reset type
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtTxLolChannelReset(adi_adrv9025_Device_t*               device,
                                          adi_adrv9025_ExtTxLolChannelReset_t* txLolReset);

/**
* \brief Sets the switching time between LO1/2 to auxLO, that occurs when dual band is enabled during tracking cals
*
* \param device Pointer to the device settings structure
* \param milliseconds variable which defines the new switching time in milliseconds
*
* Min and Max Values are defined in adi_adrv9025_cals_types.h as ADI_ADRV9025_SWITCH_TIME_MIN and ADI_ADRV9025_SWITCH_TIME_MAX
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DualBandSwitchingTimeSet(adi_adrv9025_Device_t* device,
                                              uint32_t               milliseconds);

/**
* \brief Gets the switching time between LO1/2 to auxLO, that occurs when dual band is enabled during tracking cals
*
* \param device Pointer to the device settings structure
* \param milliseconds pointer variable which we use to store the time read back from Firmware
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DualBandSwitchingTimeGet(adi_adrv9025_Device_t* device,
                                              uint32_t*              milliseconds);



/**
* \brief Configures the Tx-ORx path delay used to align Tx and ORx samples
*
* This function allows the user to manually overwrite the Tx-ORx external CLGC path delay
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \note If the Tx-ORx external routing is not the same as default Tx-ORx mapping, then the user will have to
*       call the API adi_adrv9025_TxToOrxMappingSet() in pin mode or switch the appropriate GPIO pins
*       in a GPIO Tx to ORx mapping control mode before executing this API in order to convey the
*       Tx-ORx mapping to the ADRV9025 firmware.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which the Tx-ORx external CLGC path delay is required to be configured
* \param externalPathDelay Structure consisting of the path delay settings to be configured
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ClgcExternalPathDelaySet(adi_adrv9025_Device_t*            device,
                                              adi_adrv9025_TxChannels_e         txChannel,
                                              adi_adrv9025_ExternalPathDelay_t* externalPathDelay);

/**
* \brief Retrieves the current Tx-ORx CLGC path delay value used to align Tx and ORx samples
*
* This function returns the current Tx-ORx external CLGC path delay calibrated by the ARM or written manually by the user
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the path delay init cal has run.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which the Tx-ORx CLGC external path delay is required to be configured
* \param externalPathDelay Structure consisting of the path delay settings to be configured
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ClgcExternalPathDelayGet(adi_adrv9025_Device_t*            device,
                                              adi_adrv9025_TxChannels_e         txChannel,
                                              adi_adrv9025_ExternalPathDelay_t* externalPathDelay);
#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_CALS_H_ */

// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_rx.h
 * \brief Contains Talise receive related function prototypes for
 *        talise_rx.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_RX_H_
#define TALISE_RX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_rx_types.h"
#include "talise_error_types.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Programs the gain table settings for either Rx1, Rx2, Rx1 + Rx2
 * receiver types
 *
 * The gain table for a receiver type is set with the parameters passed by
 * taliseRxGainTable_t gainTablePtr array. The array length (n) is dependent
 * upon receiver type. The (n) value is conveyed by numGainIndexesInTable.
 * All gain tables have a maximum index and a minimum index specified by
 * MAX_RX_GAIN_TABLE_NUMINDEXES and MIN_RX_GAIN_TABLE_INDEX
 * The minimum gain index is application dependent, this can be modified
 * in the user space, the absolute maximum and minimum indexes are specified
 * by MAX_GAIN_TABLE_INDEX and MIN_GAIN_TABLE_INDEX
 *
 * The Rx max gain index is always normalized to index 255. The
 * gain index to use in the setRxGain function expects 255 for max gain and
 * will automatically be offset to set the correct gain index based on the
 * MAX_RX_GAIN_TABLE_NUMINDEXES macro.
 *
 * \pre This function called automatically in TALISE_initialize() to load
 *      the default gain table. If the BBIC desires to change or update the
 *      gain table, it may call this function after initialization but before
 *      running init cals.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise device data structure
 * \param gainTablePtr containing gain table program values
 * \param numGainIndexesInTable The number of 'n' indices in array.  A range
 *  check is performed to ensure the maximum is not exceeded.
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1, Rx2,
 * Rx1 + Rx2 gain table for programming.  A channel check is performed to
 * ensure a valid selection.
 *
 * \pre This function can be called by the user to load a custom gain table or
 *  to reconfigure the gain table
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_programRxGainTable(taliseDevice_t *device,
				   taliseRxGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
				   taliseRxChannels_t rxChannel);

/**
 * \brief Programs the gain table settings for either ORx1, ORx2, ORx1 + ORx2
 * receiver types
 *
 *  * The gain table for a receiver type is set with the parameters passed by
 * taliseOrxGainTable_t gainTablePtr array. The array length (n) is dependent
 * upon receiver type. The (n) value is conveyed by numGainIndexesInTable.
 * All gain tables have a maximum index and a minimum index specified by
 * MAX_ORX_GAIN_TABLE_NUMINDEXES and MIN_ORX_GAIN_TABLE_INDEX
 * The minimum gain index is application dependent, this can be modified in
 * the user space, the absolute maximum and minimum indexes are specified by:
 * MAX_GAIN_TABLE_INDEX and MIN_GAIN_TABLE_INDEX
 *
 * The ORx max gain index is always normalized to index 255. The
 * gain index to use in the setRxGain function expects 255 for max gain and
 * will automatically be offset to set the correct gain index based on the
 * MAX_ORX_GAIN_TABLE_NUMINDEXES macro.
 *
 * \pre This function called automatically in TALISE_initialize() to load
 *      the default gain table. If the BBIC desires to change or update the
 *      gain table, it may call this function after initialization but before
 *      running init cals.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise device data structure
 * \param gainTablePtr Pointer to array of ORx gain table structures.
 * \param numGainIndexesInTable The number of 'n' indices in gainTablePtr array.  A range
 *  check is performed to ensure the maximum is not exceeded.
 * \param orxChannel taliseObsRxChannels_t enum type to select either ORx1, ORx2,
 * ORx1 + ORx2 gain table for programming.  A channel check is performed to
 * ensure a valid selection.
 *
 * \pre This function can be called by the user to load a custom gain table or
 *  to reconfigure the gain table
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_programOrxGainTable(taliseDevice_t *device,
				    taliseOrxGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
				    taliseObsRxChannels_t orxChannel);


/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */
/**
 * \brief Sets the Rx1 or Rx2 Manual Gain Index
 *
 * If the value passed in the gainIndex parameter is within range of the gain
 * table minimum and maximum indexes, the Rx1/Rx2 gain index will be written
 * to the transceiver. Else, an error will be returned. The maximum index is 255
 * and the minimum index is application specific.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 * \param rxChannel Selects the channel (Rx1 or Rx2) to update the gainIndex
 * \param gainIndex Desired Rx1 or Rx2 gain index depending on rxChannel parameter
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setRxManualGain(taliseDevice_t *device,
				taliseRxChannels_t rxChannel, uint8_t gainIndex);

/**
 * \brief Reads the Rx AGC/MGC Gain Index for the passed channel
 *
 * This function reads the gain index for the passed channel, Rx1, Rx2.
 *
 * This function reads back the AGC gain index or manual gain
 * index per channel, depending on the gain control mode.
 *
 * \pre This function may be called any time after device initialization
 *  and the ARM processor has moved to Radio ON state at least once.
 *  However, gain indices are tracked only after the device goes into a
 *  Receiver mode. So the function will return a TALACT_ERR_CHECK_PARAM
 *  recovery action if used before the part enters the receiver mode for
 *  the first time since there is no gain index history available.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param rxChannel Channel selection to read back gain index for (Valid Rx1, Rx2 only)
 * \param rxGainIndex Pointer to the specified channel gain index value
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRxGain(taliseDevice_t *device, taliseRxChannels_t rxChannel,
			  uint8_t *rxGainIndex);

/**
 * \brief Sets the ORx gain of the ObsRx channel
 *
 * The ObsRx channel can have different RF inputs (ORx1/ORx2)
 * This function sets the ObsRx gain index independently for ORx1/ORx2.
 * The maximum index is 255 and the minimum index is application specific.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end

 * \param device Pointer to the Talise device data structure
 * \param obsRxCh is an enum type taliseObsRxChannels_t to identify the desired RF input for gain change
 * \param gainIndex Desired manual gain table index to set
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setObsRxManualGain(taliseDevice_t *device,
				   taliseObsRxChannels_t obsRxCh, uint8_t gainIndex);

/**
 * \brief Reads the MGC gain index for the passed Obs Rx channel.
 *
 * This function reads the gain index for the passed channel, ORx1 or
 * ORx2.
 *
 * This function reads back the manual gain
 * index per channel, depending on the gain control mode.
 *
 * \pre This function may be called any time after device initialization.
 *  However, gain indices are tracked only after the device goes into a
 *  Receiver mode. So the function will return a TALACT_ERR_CHECK_PARAM
 *  recovery action if used before the part enters the receiver mode for
 *  the first time since there is no gain index history available.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param obsRxChannel Channel selection to read back gain index for (Valid TAL_ORX1 or TAL_ORX2 only)
 * \param obsRxGainIndex Read back gain index is returned to this pointer address.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getObsRxGain(taliseDevice_t *device,
			     taliseObsRxChannels_t obsRxChannel, uint8_t *obsRxGainIndex);

/**
 * \brief Configures the Rx gain control mode
 *
 * The Talise device defaults to Manual Gain Control (MGC) mode on power-up.
 * This API is used to reconfigure the Rx Gain Control Mode.
 *
 * \pre This function should be called before Rx operation is initiated.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 * \param mode   Desired gain mode to be configured. Parameter is of enumerated type taliseGainMode_t  where:
 * Manual Gain = MGC,
 * Automatic Gain Control Fast Attack = AGCFAST,
 * Automatic Gain Control Slow loop  = AGCSLOW ,
 * Hybrid mode = HYBRID
 *
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setRxGainControlMode(taliseDevice_t *device,
				     taliseGainMode_t mode);

/**
 * \brief Sets the Rx Data format to integer or floating point format.
 *
 * The function allows selecting the Rx data format (integer or floating point)
 * and allows configuring the various data formatting options.  If
 * this function is not called, the Talise defaults to integer format.
 * The floating point format can be set for one of several 16 bit floating
 * point formats.  The user can select the number of exponent and significand
 * bits.  It is not possible to represent every possible sample value with
 * 16bit floating point format. When using integer formatting, the number of slicer
 * bits as well as their position in the data word are selectable.  The GPIO bits are
 * also user selectable for external slicer control mode.
 *
 * The Talise data formatting should be used in conjunction with the
 * gain compensation feature.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param rxDataFormat Pointer to the Rx data format configuration structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setRxDataFormat (taliseDevice_t *device,
				 taliseRxDataFormat_t *rxDataFormat);

/**
 * \brief Retreive the Rx data path format configuration.
 *
 * Use this command to retrieve the Rx data path data formatting configuration
 * set by the TALISE_setRxDataFormat function.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param rxDataFormat Pointer to the Rx data format configuration structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRxDataFormat(taliseDevice_t *device,
				taliseRxDataFormat_t *rxDataFormat);


/**
 * \brief Get the gain slicer position bits for both Rx1 and Rx2.
 *
 * Before this information is valid, the Rx gain compensation feature should be enabled with
 * the integer gain slicer sample format enabled.  See the Talise.SetRxDataFormat() function
 * to enable gain compensation. This function is not useful in the case of using floating point
 * as the gain slicer (data scaling) is applied in the floating point scaled sample.
 *
 * The slicer position is only needed for integer 12bit and 16bit formats.  The 24bit sample format
 * already applies scaling in the 24bit sample.
 *
 * Slicer Position[2:0] | Description
 * ---------------------|---------------------------------------------
 *                0     | 0dB gain
 *                1     | Rx Data samples should be scaled by +6dB
 *                2     | Rx Data samples should be scaled by +12dB
 *                3     | Rx Data samples should be scaled by +18dB
 *                4     | Rx Data samples should be scaled by +24dB
 *                5     | Rx Data samples should be scaled by +30dB
 *                6     | Rx Data samples should be scaled by +36dB
 *                7     | Rx Data samples should be scaled by +42dB
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param rx1SlicerPosition Pointer to byte containing 3 slicer position bits for Rx1.
 * \param rx2SlicerPosition Pointer to byte containing 3 slicer position bits for Rx2.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getSlicerPosition(taliseDevice_t *device,
				  uint8_t *rx1SlicerPosition, uint8_t *rx2SlicerPosition);

/**
 * \brief Configures Gain steps and the GPIO inputs for Manual Rx gain control
 *
 * This API function configures the GPIO input pin and step size to allow the
 * BBP to control gain changes in Rx signal chain.
 * A high pulse on the 'rxGainIncPin' in pin control mode will increment the
 * gain by the value set in 'incStep'.
 * A high pulse on the 'rxGainDecPin' in pin control mode will decrement the
 * gain by the value set in 'decStep'.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 *
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1 or Rx2
 *  channel for programming.
 *
 * \param rxGainCtrlPin Pointer to taliseRxGainCtrlPin_t structure that
 * configures the Manual Rx gain pin control.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setRxGainCtrlPin(taliseDevice_t *device,
				 taliseRxChannels_t rxChannel, taliseRxGainCtrlPin_t *rxGainCtrlPin);

/**
 * \brief This API function returns the configuration (Gain steps and the GPIO
 * inputs) for Manual Rx gain control.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 *
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1 or Rx2
 *  channel for reading configuration from.
 *
 * \param rxGainCtrlPin Pointer to taliseRxGainCtrlPin_t structure that
 * would contain configuration values for Manual Rx gain pin control.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRxGainCtrlPin(taliseDevice_t *device,
				 taliseRxChannels_t rxChannel, taliseRxGainCtrlPin_t *rxGainCtrlPin);

/**
 * \brief This private helper function sets the Dual band datapath configuration.
 *
 * The dualband mode of operation allows the wideband receiver to receive two
 * adjacent bands, split them into individual paths, filter (decimation), and
 * pack them together or send them out on the JESD as separate converter data.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param init Pointer to the NCO Shifter Configuration settings.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
talRecoveryActions_t talSetDualBandSettings(taliseDevice_t *device,
		taliseInit_t *init);

/**
 * \brief Programs the Dualband external LNA control gain table settings for
 * either Rx1, Rx2, Rx1 + Rx2 receiver types
 *
 * The gain table for a receiver type is set with the parameters passed by
 * taliseDualBandLnaGainTable_t gainTablePtr array. The array length (n) is
 * conveyed by numGainIndexesInTable. The table can have a maximum of 4
 * entries. The first entry in the array corresponds to entry 0, second to
 * entry 1, and so on.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise device data structure
 * \param gainTablePtr Pointer to Dual Band LNA gain table array of taliseDualBandLnaGainTable_t structures
 * \param numGainIndexesInTable The number of 'n' indices in gainTablePtr array.  A range
 *  check is performed to ensure the maximum (4) is not exceeded.
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1, Rx2,
 * Rx1 + Rx2 gain table for programming.  A channel check is performed to
 * ensure a valid selection.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_programDualBandLnaGainTable( taliseDevice_t *device,
		taliseDualBandLnaGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
		taliseRxChannels_t rxChannel);

/**
 * \brief Private function sets the configuration for Low IF to Zero IF mode used only by
 * Talise_initialize().  Application should set the init structure prior to calling
 * Talise_initialize().
 *
 * This mode allows the receiver to downconvert the signal to Low IF, and then
 * shift it to zero IF using the NCO. The user provides the center frequency
 * of the downconverted low IF band to the API, and the API programs the NCO
 * to shift the center frequency of the band to zero.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param init Pointer to the NCO Shifter Configuration settings.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
talRecoveryActions_t talSetupNcoShifter(taliseDevice_t *device,
					taliseInit_t *init);

/**
 * \brief Programs the GPIOs to output external control word of the current gain index
 *
 * External Control Word is a 4 bit data which is unique for each gain index. It is
 * unique for channels Rx1 and Rx2. External control word corresponding to the current
 * gain index can be output on 3.3V GPIO[3:0] for Rx1 and 3.3V GPIO[7:4] for Rx2.
 * This API configures the corresponding GPIOs as output based on the rxChannel parameter
 * to output the external control word.
 *
 * \dep_begin
 * \dep{device->devStateInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1, Rx2,
 * Rx1 + Rx2 gain table external control word to be output on GPIOs
 * \param enable3p3vGpios 1=enable output of Rx Gain table external control words on 3.3v GPIOs or 0=disable
 * output of external control words on GPIOs.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setGainTableExtCtrlPins(taliseDevice_t *device,
					taliseRxChannels_t rxChannel, uint8_t enable3p3vGpios);

/**
 * \brief Performs a power measurement in the Rx digital data path for the channel specified.
 *
 * Due to interdependencies between the AGC and power measurement the power measurement duration and
 * where the measurement is taken is variable. Call TALISE_setupRxAgc() to configure power measurement.
 * The location of the power measurement is given by rxAgcCtrl->agcPower->[powerUseRfirOut/powerUseBBDC2].
 * The number of samples the power measurement uses is given by 8*2^(rxAgcCtrl->agcPower->powerMeasurementDuration) at the IQ rate,
 * if measured at RFIR output. This number of samples must be less than the agcGainUpdateCounter.
 * If the receiver is disabled during the power measurement, this function returns a 0 value for rx1DecPower_mdBFS
 *
 * The resolution of this function is 0.25dB.
 * When the measurement location is HB2 (i.e., powerUseRfirOut == powerUseBBDC2 == 0) the dynamic range of this function is 40dB.
 * Signals lower than -40dBFS may not be measured accurately. When measuring from RfirOut or BBDC2, the dynamic range of this
 * function is 60dB. Signals lower than -60 dBFS may not be measured accurately.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{rxAgcCtrl->agcPower}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure containing settings
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1 or Rx2
 * \param rxDecPower_mdBFS Pointer to store the Rx decimated power return. Value returned in mdBFS
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getRxDecPower(taliseDevice_t *device,
			      taliseRxChannels_t rxChannel, uint16_t *rxDecPower_mdBFS);

/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif /* TALISE_RX_H_ */

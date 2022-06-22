// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_jesd204.h
 * \brief Contains Talise JESD204b data path related function prototypes for
 *        talise_jesd204.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_JESD204_H_
#define TALISE_JESD204_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_jesd204_types.h"
#include "talise_tx_types.h"
#include "talise_error_types.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Sets up the JESD204B Serializers
 *
 * This function uses init->rx.framerSel and init->obsRx.framerSel to determine
 * the framerA and framerB structures settings to use to setup the correct serializer
 * lanes that are shared between the two framers. This function configures the following
 * for the serializers:
 *   1) Sets the serializer lane clock frequency
 *   2) Powers up the requested serializer lanes
 *   3) Resets the powered up serializers
 *   4) Sets the serializers preEmphasis
 *   5) Sets the serializers amplitude
 *   6) Sets the serializer PN invert setting per lane
 *
 * Since the Rx and ORx IQ data rates can be different, and the serializers are shared between
 * the two framers, the serializer lane clock is set for the faster of the two
 * framer rates.  The framers will automatically oversample or bit repeat to
 * account for the clock rate difference, depending on the oversample framer parameter. The
 * oversample mode is only needed if Rx and ORx share the same framer and have different
 * IQ sample rates.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{init->rx (most members)}
 * \dep{init->obsRx (most members)}
 * \dep{init->jesd204Settings (most members)}
 * \dep{init->clocks.clkPllVcoFreq_kHz}
 * \dep_end
 *
 * \param device Structure pointer to the device settings structure
 * \param init Pointer to the device initialization settings structure
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setupSerializers(taliseDevice_t *device, taliseInit_t *init);

/**
 * \brief Sets up the JESD204B Deserializers
 *
 * This function is called automatically by TALISE_initialize() if the Tx profile is valid.
 * This function enables the necessary deserializer lanes, sets the deserializer clocks
 * PN inversion settings, and EQ settings based on the info found in the device data structure.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{init->tx.deframerSel}
 * \dep{init->jesd204Settings (most members)}
 * \dep{init->tx.txProfile.txInputRate_kHz}
 * \dep{init->clocks.clkPllVcoFreq_kHz}
 * \dep_end
 *
 * \param device Structure pointer to the device settings structure
 * \param init Pointer to the device initialization settings data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setupDeserializers(taliseDevice_t *device, taliseInit_t *init);

/**
 * \brief Sets up the JESD204B Framer
 *
 * If the number of converters, M = 0 or no lanes are enabled, then the framer will be disabled.
 * Otherwise, the framer selected in the framerSelect parameter will be configured based on the
 * settings in the data structure.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{init->rxChannels}
 * \dep{init->jesd204Settings.framerA :All members}
 * \dep{init->jesd204Settings.framerB :All members}
 * \dep_end
 *
 * \param device Structure pointer to the device settings structure
 * \param init Pointer to init settings for this Talise instance
 * \param framerSel Select the framer to configure (TAL_FRAMER_A or TAL_FRAMER_B)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setupJesd204bFramer(taliseDevice_t *device, taliseInit_t *init,
				    taliseFramerSel_t framerSel);

/**
 * \brief Sets up the JESD204B Deframer
 *
 * This function updates the JESD204B deframer settings.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{init->jesd204Settings}
 * \dep{init->tx}
 * \dep_end
 *
 * \param device Structure pointer to device settings data structure
 * \param init Pointer to device initialization settings data structure
 * \param deframerSel Desired deframer to configure
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setupJesd204bDeframer(taliseDevice_t *device,
				      taliseInit_t *init, taliseDeframerSel_t deframerSel);

/**
 * \brief Enables/Disables the JESD204B Framer
 *
 * This function is normally not necessary.  In the event that the link needs to be reset, this
 * function allows a framer to be disabled and re-enabled.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSel Desired framer to reset.
 * \param enable 0 = Disable the selected framer, 1 = enable the selected framer link
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableFramerLink(taliseDevice_t *device,
				 taliseFramerSel_t framerSel, uint8_t enable);

/**
 * \brief Enables/Disables the JESD204B Deframer
 *
 * This function is normally not necessary.  In the event that the link needs to be reset, this
 * function allows a deframer to be disabled and re-enabled.  During disable, the lane FIFOs
 * for the selected deframer are also disabled.  When the deframer link is enabled, the
 * lane FIFOs for the selected deframer are re-enabled (reset).  The BBIC should send valid
 * serializer data before enabling the link so the Talise CDR (recovered clock) is locked.
 *
 * \pre This function may be called any time after device initialization.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talised device settings data structure
 * \param deframerSel Desired deframer to reset.
 * \param enable 0 = Disable the selected deframer, 1 = enable the selected deframer link
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableDeframerLink(taliseDevice_t *device,
				   taliseDeframerSel_t deframerSel, uint8_t enable);

/**
 * \brief Enables or disables the external SYSREF JESD204b signal to the transceiver's framers
 *
 * For the framer to retime its LMFC (local multi frame clock), a SYSREF rising edge is required.
 * The external SYSREF signal at the pin can be gated off internally so the framer does not see
 * a potential invalid SYSREF pulse before it is configured correctly.
 *
 * By default Talise has the SYSREF signal ungated, however, the Talise Multichip Sync state machine
 * still does not allow the external SYSREF to reach the framer until the other stages of multichip
 * sync have completed.  As long as the external SYSREF is correctly configured before performing MCS,
 * this function may not be needed by the BBIC, since the MCS state machine gates the SYSREF to the
 * framer.
 *
 * \pre This function is called after the device has been initialized and the JESD204B framer is enabled
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSel Select framer to enable/disable SYSREF input for (Valid TAL_FRAMER_A, TAL_FRAMER_B or TAL_FRAMER_A_AND_B)
 * \param enable = '1' enables SYSREF to deframer, '0' disables SYSREF to deframer
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableSysrefToFramer(taliseDevice_t *device,
				     taliseFramerSel_t framerSel, uint8_t enable);

/**
 * \brief Enables or disables the external SYSREF JESD204b signal to the transceiver's deframers
 *
 * \pre This function is called after the device has been initialized and the JESD204B deframer is enabled
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSel Select deframer to enable/disable SYSREF input for (Valid TAL_DEFRAMER_A, TAL_DEFRAMER_B or TAL_DEFRAMER_A_AND_B)
 * \param enable = '1' enables SYSREF to deframer, '0' disables SYSREF to deframer
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableSysrefToDeframer(taliseDevice_t *device,
				       taliseDeframerSel_t deframerSel, uint8_t enable);

/**
 * \brief Reads the Talise framer status for the framer selected by the framerSel input parameter
 *
 * \pre The Rx JESD204B link(s) needs to be configured and running to use this function
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * framerStatus |  Description
 * -------------|-----------------------------------------------------------------------------
 *         [6]  | Flag indicating that configuration parameters are not supported when set(1). Valid only on C0 or higher silicon revisions. This field is set to 0 for lower revisions.
 *         [5]  | JESD204 Negative Edge Count for SYNC  0 = not greater than zero, 1 = greater than zero
 *         [4]  | Reserved (0)
 *         [3]  | SYNCB IN select (which SYNCB IN pin is used by the requested framer) 0 = SYNCBIN0, 1 = SYNCBIN1
 *         [2]  | Current SYNCBIN level (1= high, 0 = low)
 *         [1]  | SYSREF phase error ? a new SYSREF had different timing than the first that set the LMFC timing.
 *         [0]  | SYSREF received by framer
 *
 * \param device is a pointer to the device settings structure
 * \param framerSel Read back the framer status of the selected framer (A or B)
 * \param framerStatus is the RX framer status byte read
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_readFramerStatus(taliseDevice_t *device,
				 taliseFramerSel_t framerSel, uint8_t *framerStatus);

/**
 * \brief Reads the status of the requested deframer
 *
 * \pre The Tx JESD204B link(s) needs to be configured and running to use this function
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 *   deframerStatus  |  Bit Name                |  Description
 *   ----------------|--------------------------|---------------------------------------------
 *              [8]  | Tx JESD Config Invalid   | Flag indicating that configuration parameters are not supported when set(1). Valid only on C0 or higher silicon revisions. This field is set to 0 for lower revisions.
 *              [7]  | Valid checksum           | 1 if the checksum calculated by the Talise matched the one sent in the ILAS data.
 *              [6]  | EOF Event                | This bit captures the internal status of the framer End of Frame event. Value =1 if framing error during ILAS
 *              [5]  | EOMF Event               | This bit captures the internal status of the framer End of Multi-Frame event. Value =1 if framing error during ILAS
 *              [4]  | FS Lost                  | This bit captures the internal status of the framer Frame Symbol event. Value =1 if framing error during ILAS or user data (invalid replacement characters)
 *              [3]  | LMFC out                 | Not useful to read across SPI
 *              [2]  | User Data Valid          | =1 when in user data (deframer link is up and sending valid DAC data)
 *              [1]  | SYSREF Received          | Deframer has received the external SYSREF signal
 *              [0]  | Sync Error               | A link synchronization error occurred
 *
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSel Select the deframer to read back the status of
 * \param deframerStatus 8bit deframer status word return value
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_readDeframerStatus(taliseDevice_t *device,
				   taliseDeframerSel_t deframerSel, uint16_t *deframerStatus);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
 * \brief Sets the DAC sample crossbar
 *
 * \pre This function is called during JESD204B initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param channelSel is a taliseTxChannels_t enumerated data type for DAC crossbar channel selection, where only TX1 or TX2 are valid choices
 * \param dacXbar is taliseDacSampleXbar_t enumerated data type used to map any deframer output to a specific DAC channel I or Q converter input for Tx
 *
 * In the event that less than four (4) DACs are enabled for deframing, the least significant deframer outputs are used
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setupDacSampleXbar(taliseDevice_t *device,
				   taliseTxChannels_t channelSel, taliseDacSampleXbar_t dacXbar);

/**
 * \brief Sets the ADC sample crossbar to map Rx1/Rx2/ORx1/ORx2 'I/Q' data to
 * the chosen JESD204B framer's converter
 *
 * \pre This function is called during JESD204B initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSel selects framer A or framer B to set the ADC crossbar on
 * it's input where only TAL_FRAMER_A or TAL_FRAMER_B are valid choices
 * \param adcXbar is the ADC crossbar setting for the framer of choice
 *
 * The adcXbar is a structure of crossbar settings for the eight converters
 * The settings use the taliseAdcSampleXbarSelect_t enumerated type
 *
 * \retval Returns uint32_t value of the enumerated type talRecoveryActions_t
 * TALACT_NO_ACTION indicates the API completed successfully. Any other value indicates
 * an error with a recommended Recovery action.
 */
uint32_t TALISE_setupAdcSampleXbar(taliseDevice_t *device,
				   taliseFramerSel_t framerSel, taliseAdcSampleXbar_t adcXbar);
/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

/**
 * \brief Selects the PRBS type and enables or disables RX Framer PRBS generation
 *
 * This is a debug function to be used for debug of the Rx JESD204B lanes.
 * Rx data transmission on the JESD204B link(s) is not possible
 * when the framer test data is activated.  To disable PRBS call this function
 * again with the framer data source set to FTD_ADC_DATA.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSelect select the framer of interest, FRAMER_A, FRAMER_B, or FRAMER_A_AND_B
 * \param testDataSource selects the desired test data pattern (normal data path = FTD_ADC_DATA)
 * \param injectPoint Point in data path to inject the test data.
 *                    PRBS data should be injected into serializer for physical
 *                    layer testing.
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_enableFramerTestData(taliseDevice_t *device,
				     taliseFramerSel_t framerSelect, taliseFramerDataSource_t testDataSource,
				     taliseFramerInjectPoint_t injectPoint);

/**
 * \brief Injects an error into the Framer test data by inverting the data
 *
 * This is a debug function to be used for debug of the the Rx JESD204B lanes.
 * Rx data transmission on the JESD204B link(s) is not possible
 * when the framer test data is activated.
 *
 * \pre This function is called after the framer test data is enabled.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param framerSelect Select the desired framer (TAL_FRAMER_A or TAL_FRAMER_B), TAL_FRAMER_A_AND_B not supported
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_injectFramerTestDataError(taliseDevice_t *device,
		taliseFramerSel_t framerSelect);

/**
 * \brief Configures and enables or disables the transceiver's lane/sample PRBS
 *        checker.
 *
 * This is a debug function to be used for debug of the Tx JESD204B lanes.
 * The Tx link(s) need to be configured and on to use this function. If the
 * checkerLocation is TAL_PRBSCHECK_LANEDATA, the PRBS is checked at the
 * output of the deserializer. If the checkLocation is TAL_PRBSCHECK_SAMPLEDATA
 * the PRBS data is expected to be framed JESD204b data and the PRBS is checked
 * after the JESD204b data is deframed.  For the sample data, there is only
 * a PRBS checker on deframer output 0.  The lane PRBS has a checker on each
 * deserializer lane.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param polyOrder selects the PRBS type based on enum values (TAL_PRBS_DISABLE, TAL_PRBS7, TAL_PRBS15, TAL_PRBS31)
 * \param checkerLocation Check at deserializer (framer0 input) or sample (framer 0 output).
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_enableDeframerPrbsChecker(taliseDevice_t *device,
		taliseDeframerPrbsOrder_t polyOrder, taliseDefPrbsCheckLoc_t checkerLocation);

/**
 * \brief Clears the deserializer lane and deframer sample PRBS error counters
 *
 * \pre The Tx JESD204B link(s) needs to be configured and running to use this function
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
  *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_clearDeframerPrbsCounters(taliseDevice_t *device);

/**
 * \brief Reads the deserializer lane or deframer sample PRBS counters
 *
 * In the case that the PRBS checker is set to check at the deframer output
 * sample, there is only a checker on the Deframer Sample 0 output.  In this
 * case the lane function parameter is ignored and the sample 0 PRBS counter
 * is returned.
 *
 * \pre The Tx JESD204B link(s) needs to be configured and running to use this function
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param lane Specify which lane to read the counter back for. (Valid 0-3)
 * \param prbsErrorCount Pointer to single uint8_t variable that returns the
 *        8-bit PRBS error count
 *
 * \param prbsInvertedStatus Pointer to single uint8_t variable that returns
 *        the bitmask indicating PRBS sequence is valid but inverted.
 *
 *    prbsInvertedStatus |  Description
 *    -------------------|---------------------------------------------
 *                  [3]  | Lane3 PRBS sequence is valid but inverted
 *                  [2]  | Lane2 PRBS sequence is valid but inverted
 *                  [1]  | Lane1 PRBS sequence is valid but inverted
 *                  [0]  | Lane0 PRBS sequence is valid but inverted
 *
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_readDeframerPrbsCounters(taliseDevice_t *device, uint8_t lane,
		uint8_t *prbsErrorCount, uint8_t *prbsInvertedStatus);

/**
 * \brief Compares received Lane0 ILAS configuration to Talise deframer
 *        configuration and returns 32-bit mask indicating values that
 *        mismatched.  Actual lane0 ILAS configuration and deframer
 *        configuration values can be obtained by passing a pointer to a
 *        structure of type taliseJesd204bLane0Config_t to the dfrmCfg and
 *        dfrmIlas function parameters individually or together.  Passing NULL
 *        to either of these parameters will result in no values returned for
 *        that parameter.
 *
 * \pre The Rx JESD204B link(s) needs to be configured and running to use
 *      this function
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * mismatch Mask|  Description
 * -------------|------------------------------------------
 *         [17] | Lane3 Checksum, 0 = match, 1 = mismatch
 *         [16] | Lane2 Checksum, 0 = match, 1 = mismatch
 *         [15] | Lane1 Checksum, 0 = match, 1 = mismatch
 *         [14] | Lane0 Checksum, 0 = match, 1 = mismatch
 *         [13] | HD, 0 = match, 1 = mismatch
 *         [12] | CF, 0 = match, 1 = mismatch
 *         [11] | S, 0 = match, 1 = mismatch
 *         [10] | NP, 0 = match, 1 = mismatch
 *         [9]  | CS, 0 = match, 1 = mismatch
 *         [8]  | N, 0 = match, 1 = mismatch
 *         [7]  | M, 0 = match, 1 = mismatch
 *         [6]  | K, 0 = match, 1 = mismatch
 *         [5]  | F, 0 = match, 1 = mismatch
 *         [4]  | SCR, 0 = match, 1 = mismatch
 *         [3]  | L, 0 = match, 1 = mismatch
 *         [2]  | LID0, 0 = match, 1 = mismatch
 *         [1]  | BID, 0 = match, 1 = mismatch
 *         [0]  | DID, 0 = match, 1 = mismatch
 *
 * \param device Pointer to the device settings structure
 * \param deframerSelect Enum indicating which deframer to address.
 * \param mismatch Pointer to a single uint32_t variable for reporting the ILAS
 *                 match status. Always returned.
 *
 * \param dfrmCfg Pointer to a taliseJesd204bLane0Config_t structure that
 *                returns the deframer configuration settings.  If NULL, data
 *                is not returned to the pointer.
 *
 * \param dfrmIlas Pointer to a taliseJesd204bLane0Config_t structure that
 *                 returns the received Lane0 ILAS settings.  If NULL, no data
 *                 is returned to this pointer.
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getDfrmIlasMismatch(taliseDevice_t *device,
				    taliseDeframerSel_t deframerSelect, uint32_t *mismatch,
				    taliseJesd204bLane0Config_t *dfrmCfg, taliseJesd204bLane0Config_t *dfrmIlas);

/**
 * \brief Reads the IRQ interrupt clear register of the specified deframer.
 * This function reads the contents of the deframer IRQ clear register.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to find the maskable deframer IRQ sources.  Note: Deframer IRQ sources Elastic Buffer Error Flag,
 * Sysref Buffer Error, and the four Lane FIFO Async Error IRQ sources are always enabled and can not be masked in the
 * interrupt clear register.
 *
 * \pre This function may be called any time after JESD204B link initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 * \param irqMask is a pointer to the bit mask value containing the contents of the deframer IRQ Clear Register
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [8]  | CMM - Reserved
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane synce  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getDfrmIrqMask(taliseDevice_t *device,
			       taliseDeframerSel_t deframerSelect, uint16_t *irqMask);

/**
 * \brief Writes the IRQ interrupt clear register of the specified deframer.
 * This function writes the specified IRQ mask value to the deframer IRQ clear register.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to clear the pending maskable deframer IRQ or to enable/disable deframer interrupt sources. Note: Deframer IRQ sources
 * Elastic Buffer Error Flag, Sysref Buffer Error, and the four Lane FIFO Async Error IRQ sources are always enabled and can not be masked in the
 * interrupt clear register.  This function does not read-modify-write the interrupt clear register. To manually clear the interrupt, write
 * a one (set) to disable or mask the bit of the interrupt followed by writing a zero (clear) to enable the bit of the interrupt.  However, if the interrupt condition
 * still exists after setting the mask bit, the corresponding IRQ vector bit will re-assert.
 *
 * \pre This function may be called any time after Jesd204B link initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 * \param irqMask is the bit mask value to be written to the deframer IRQ Clear Register (this is not a read-modify-write)
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [8]  | CMM - Reserved
 *                  [7]  | BD - Bad Disparity error counter  1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *                  [6]  | Not-In-Table error counter  1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *                  [5]  | Unexpected K error counter  1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *                  [4]  | ILD - Inter-lane De-skew  1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *                  [3]  | ILS - Initial lane sync  1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *                  [2]  | GCS - Good Check Sum  1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *                  [1]  | FS - Frame Sync 1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *                  [0]  | CSG - Code Group Sync  1 = Disable/Clear Interrupt, 0 = Enable Interrupt to assert corresponding bit in IRQ Vector Register
 *
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_setDfrmIrqMask(taliseDevice_t *device,
			       taliseDeframerSel_t deframerSelect, uint16_t irqMask);

/**
 * \brief Write the IRQ interrupt clear register for the specified deframer.
 * This function clears all deframer IRQ sources.  Use this function whenever a general purpose (GP)
 * deframer IRQ asserts to clear the pending deframer IRQ.
 *
 * \pre This function may be called any time after JESD204 link initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_clearDfrmIrq(taliseDevice_t *device,
			     taliseDeframerSel_t deframerSelect);

/**
 * \brief Read the IRQ interrupt source registers for the specified deframer.
 * This function fetches the contents of the deframer IRQ Vector register and other IRQ sources.  Use this function whenever
 * a general purpose (GP) deframer IRQ asserts to determine the source of the deframer IRQ.  Common IRQ sources are Bad Disparity (BD),
 * Not-In_Table (NIT), and Unexpected K-char (UEK) counters greater than the specified error threshold count value.
 *
 * \pre This function may be called any time after JESD204 link initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframerSelect selects the deframer to interrogate.
 * \param irqSourceValue is a pointer to a bit mask containing the status of the IRQ Vector source register at read time.
 *
 *                 Bit   |  Description
 *          -------------|-----------------------------------------------------------------------------
 *                  [14] | Lane3 Fifo Async Error - Set when lane pointers have come out of alignment during write or clear
 *                  [13] | Lane2 Fifo Async Error - Set when lane pointers have come out of alignment during write or clear
 *                  [12] | Lane1 Fifo Async Error - Set when lane pointers have come out of alignment during write or clear
 *                  [11] | Lane0 Fifo Async Error - Set when lane pointers have come out of alignment during write or clear
 *                  [10] | Sysref Phase Error - Set when sysref is misaligned to global LMFC
 *                  [9]  | Elastic Buffer Error Flag - Set when pointer come out of alignment during write or clear
 *                  [8]  | CMM - Configuration mismatch for lane0  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [7]  | BD - Bad Disparity error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [6]  | Not-In-Table error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [5]  | Unexpected K error counter  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [4]  | ILD - Inter-lane De-skew  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [3]  | ILS - Initial lane synce  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [2]  | GCS - Good Check Sum  0 = No Interrupt, 1 = Interrupt Asserted
 *                  [1]  | FS - Frame Sync 0 = No Interrupt, 1 = Interrupt Asserted
 *                  [0]  | CSG - Code Group Sync  0 = No Interrupt, 1 = Interrupt Asserted
 *
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getDfrmIrqSource(taliseDevice_t *device,
				 taliseDeframerSel_t deframerSelect, uint16_t *irqSourceValue);

/**
 * \brief Private function for scanning deframer error counters to report which lane number is reporting an counter overflow.
 *
 * \pre This function may be called any time a deframer is reporting an error counter terminal count reached interrupt.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param deframer selects the deframer to interrogate.
 * \param deframerInputsMask is a pointer to the deframer inputs bitmask
 *        indicating the input that is reporting an error. Note that these are
 *        not phyiscal lanes, but are inputs to the deframer after the lane
 *        cross bar.
 *
 *     bit  |  Lane
 *     -----|----------------
 *       0  |  deframer Input 0
 *       1  |  deframer Input 1
 *       2  |  deframer Input 2
 *       3  |  deframer Input 3
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
talRecoveryActions_t talFindDfrmrLaneCntErr(taliseDevice_t *device,
		taliseDeframerSel_t deframer, int32_t *deframerInputsMask);

/**
 * \brief Private function for scanning deframer error flags to report which lane number is reporting an error.
 *
 * \pre This function may be called any time a deframer is reporting an error interrupt.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param dfrmErrAddress deframer register address to interrogate.
 * \param nibbleToUse specify nibble to use
 * \param deframerInputsMask is a pointer to the deframer inputs bitmask
 *        indicating the input that is reporting an error. Note that these are
 *        not phyiscal lanes, but are inputs to the deframer after the lane
 *        cross bar.
 *
 *     bit  |  Lane
 *     -----|------------------
 *       0  |  deframer Input 0
 *       1  |  deframer Input 1
 *       2  |  deframer Input 2
 *       3  |  deframer Input 3
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
talRecoveryActions_t talFindDfrmrLaneErr(taliseDevice_t *device,
		uint32_t dfrmErrAddress, uint8_t nibbleToUse, int32_t *deframerInputsMask);

/**
 * \brief Manually toggles JESD204B framer SYNCB signal internal to framer
 *
 *  NOTE: DO NOT USE THIS FUNCTION UNLESS INSTRUCTED TO BY AN ADI APPLICATIONS
 *  ENGINEER.
 *
 *  This function is normally not needed and should only be used when required
 *  by the system hardware due to non-conformance to the JESD204B specification.
 *
 *  Normally, during initialization, the framer moves from the CGS state to ILAS
 *  state when the BBIC sends a rising edge on the SYNCB pin.  In systems that
 *  do not control the SYNCB per the JESD204B spec, this function allows a method
 *  to toggle SYNCB internal to the Talise device framer, allowing valid high
 *  speed serial data to start outputting from the Talise device.
 *
 *  This function would be used during device initialization when the JESD204B
 *  links are being initialized, after enabling the Talise framer link, and
 *  after sending a SYSREF pulse.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param framerSel Enum to select framer to apply SYNCB reset to
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_framerSyncbToggle(taliseDevice_t *device,
				  taliseFramerSel_t framerSel);

#ifdef __cplusplus
}
#endif

#endif /* TALISE_JESD204_H_ */

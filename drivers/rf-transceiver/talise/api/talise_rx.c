// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_rx.c
 * \brief Contains functions to support Talise Rx and Observation Rx data path
 *        control
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_rx.h"
#include "talise_gpio.h"
#include "talise_reg_addr_macros.h"
#include "talise_arm_macros.h"
#include "talise_hal.h"
#include "talise_user.h"
#include "talise_error.h"
#include "talise.h"
#include "talise_gpio.h"

uint32_t TALISE_programRxGainTable(taliseDevice_t *device,
				   taliseRxGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
				   taliseRxChannels_t rxChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t tableRowIndex = 0;
	uint8_t ctlReg = 0;
	uint8_t i = 0;
	uint32_t addrIndex = 0;
	uint32_t spiBufferSize = (((HAL_SPIWRITEARRAY_BUFFERSIZE / 8) - 1) * 8);
	uint16_t addrArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = {0};
	uint8_t dataArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = {0};
	static const uint8_t EN_TABLECLK = 0x05;
	uint8_t digGainSign = 0;
	uint16_t digGainMag = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_programRxGainTable()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((rxChannel == TAL_RX1) ||
	    (rxChannel == TAL_RX2) ||
	    (rxChannel == TAL_RX1RX2)) {
		/* Verify Rx profile is valid before allowing to set the Rx gain table, this
		 * ensures the device->rx structure pointers are valid
		 */
		if ((device->devStateInfo.profilesValid  & RX_PROFILE_VALID) == 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_RXGAINTABLE_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Check for proper mapping for gain table and passed table number of indexes */
		if (numGainIndexesInTable > ((START_RX_GAIN_INDEX - MIN_RX_GAIN_TABLE_INDEX) +
					     1)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_RXGAINTABLE_INV_GAIN_INDEX_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RXGAINTABLE_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (gainTablePtr == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_PROGRAM_RXGAIN_TABLE_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for channel and the logic behind it */
	if ((rxChannel == TAL_RX1) ||
	    (rxChannel == TAL_RX1RX2)) {
		/* normalize min/max index in device structure to 255 index = max gain */
		device->devStateInfo.gainIndexes.rx1MaxGainIndex = MAX_GAIN_TABLE_INDEX;
		device->devStateInfo.gainIndexes.rx1MinGainIndex = MAX_GAIN_TABLE_INDEX -
				numGainIndexesInTable + 1;
	}

	if ((rxChannel == TAL_RX2) ||
	    (rxChannel == TAL_RX1RX2)) {
		/* normalize min/max index in device structure to 255 index = max gain */
		device->devStateInfo.gainIndexes.rx2MaxGainIndex = MAX_GAIN_TABLE_INDEX;
		device->devStateInfo.gainIndexes.rx2MinGainIndex = MAX_GAIN_TABLE_INDEX -
				numGainIndexesInTable + 1;
	}

	/* starting the gain table clock and read from gain table address bits */
	ctlReg = (uint8_t)((rxChannel << 3) | EN_TABLECLK);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GAIN_TABLE_CONFIGURATION, ctlReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* programming a table selected by rxChannel enum type */
	addrIndex = 0;

	for(i = START_RX_GAIN_INDEX; i > START_RX_GAIN_INDEX - numGainIndexesInTable;
	    i--) {
		tableRowIndex = START_RX_GAIN_INDEX - i;

		/* set current gain table index (address) */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_ADDRESS;
		dataArray[addrIndex++] = i;

		/* Set Rx Front End gain[7:0] */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_RXFE;
		dataArray[addrIndex++] = gainTablePtr[tableRowIndex].rxFeGain;

		/* Set external control [3:0] */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_EXT;
		dataArray[addrIndex++] = gainTablePtr[tableRowIndex].extControl;

		digGainSign = (gainTablePtr[tableRowIndex].digGain < 0) ? 1 : 0;
		digGainMag = (uint16_t)((digGainSign > 0) ?
					(-gainTablePtr[tableRowIndex].digGain) : gainTablePtr[tableRowIndex].digGain);

		/* Set digital attenuation/gain[10:4] */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_DIG;
		dataArray[addrIndex++] = ((digGainSign & 0x01) << 6) | (((uint8_t)(
						 digGainMag >> 4 )) & 0x3F);

		/* Set digital attenuation/gain[3:0], TIA control */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_DIG2;
		dataArray[addrIndex++] = ((gainTablePtr[tableRowIndex].adcTiaGain & 0x03) << 4)
					 | ((uint8_t)(digGainMag & 0x0F));

		/* Set Phase offset if Rx gain table */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_PHASE_OFFSET;
		dataArray[addrIndex++] = (uint8_t)(gainTablePtr[tableRowIndex].phaseOffset &
						   0xFF);

		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_PHASE_OFFSET2;
		dataArray[addrIndex++] = (uint8_t)((gainTablePtr[tableRowIndex].phaseOffset >>
						    8) & 0xFF);

		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_CONFIGURATION;
		dataArray[addrIndex++] = (ctlReg | 0x02);

		if (addrIndex >= spiBufferSize) {
			halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &dataArray[0],
						    addrIndex);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			addrIndex = 0;
		}
	}

	if (addrIndex > 0) {
		halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &dataArray[0],
					    addrIndex);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* disabling the gain table clock */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GAIN_TABLE_CONFIGURATION, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_programOrxGainTable(taliseDevice_t *device,
				    taliseOrxGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
				    taliseObsRxChannels_t orxChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t tableRowIndex = 0;
	uint8_t ctlReg = 0;
	uint8_t i = 0;
	uint32_t addrIndex = 0;
	uint32_t spiBufferSize = (((HAL_SPIWRITEARRAY_BUFFERSIZE / 6) - 1) * 6);
	uint16_t addrArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = {0};
	uint8_t dataArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = {0};
	static const uint8_t EN_TABLECLK = 0x05;
	uint8_t digGainSign = 0;
	uint16_t digGainMag = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_programOrxGainTable()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((orxChannel == TAL_ORX1) ||
	    (orxChannel == TAL_ORX2) ||
	    (orxChannel == TAL_ORX1ORX2)) {
		/* Verify ORx profile is valid before allowing to set the ORx gain table, this
		 * ensures the device->obsRx structure pointers are valid
		 */
		if ((device->devStateInfo.profilesValid  & ORX_PROFILE_VALID) == 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_ORXGAINTABLE_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Check for proper mapping for gain table and passed table number of indexes */
		if (numGainIndexesInTable > ((START_ORX_GAIN_INDEX -  MIN_ORX_GAIN_TABLE_INDEX)
					     + 1)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_ORXGAINTABLE_INV_GAIN_INDEX_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ORXGAINTABLE_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (gainTablePtr == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_PROGRAM_ORXGAIN_TABLE_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for channel and the logic behind it */
	if ((orxChannel == TAL_ORX1) ||
	    (orxChannel == TAL_ORX1ORX2)) {
		/* normalize min/max index in device structure to 255 index = max gain */
		device->devStateInfo.gainIndexes.orx1MaxGainIndex = MAX_GAIN_TABLE_INDEX;
		device->devStateInfo.gainIndexes.orx1MinGainIndex = MAX_GAIN_TABLE_INDEX -
				numGainIndexesInTable + 1;
	}

	if ((orxChannel == TAL_ORX2) ||
	    (orxChannel == TAL_ORX1ORX2)) {
		/* normalize min/max index in device structure to 255 index = max gain */
		device->devStateInfo.gainIndexes.orx2MaxGainIndex = MAX_GAIN_TABLE_INDEX;
		device->devStateInfo.gainIndexes.orx2MinGainIndex = MAX_GAIN_TABLE_INDEX -
				numGainIndexesInTable + 1;
	}

	/* starting the gain table clock and read from gain table address bits */
	ctlReg = (uint8_t)((orxChannel << 3) | EN_TABLECLK);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GAIN_TABLE_CONFIGURATION, ctlReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* programming a table selected by rxChannel enum type */
	addrIndex = 0;

	for(i = START_ORX_GAIN_INDEX; i > START_ORX_GAIN_INDEX - numGainIndexesInTable;
	    i--) {
		tableRowIndex = START_ORX_GAIN_INDEX - i;

		/* set current gain table index (address) */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_ADDRESS;
		dataArray[addrIndex++] = i;

		/* Set Rx Front End gain[7:0] */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_RXFE;
		dataArray[addrIndex++] = gainTablePtr[tableRowIndex].rxFeGain;

		/* Set external control [3:0] */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_EXT;
		dataArray[addrIndex++] = gainTablePtr[tableRowIndex].extControl;

		digGainSign = (gainTablePtr[tableRowIndex].digGain < 0) ? 1 : 0;
		digGainMag = (uint16_t)((digGainSign > 0) ?
					(-gainTablePtr[tableRowIndex].digGain) : gainTablePtr[tableRowIndex].digGain);

		/* Set digital attenuation/gain[10:4] */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_DIG;
		dataArray[addrIndex++] = ((digGainSign & 0x01) << 6) | (((uint8_t)(
						 digGainMag >> 4 )) & 0x3F);

		/* Set digital attenuation/gain[3:0], TIA control */
		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_WORD_DATA_DIG2;
		dataArray[addrIndex++] = ((gainTablePtr[tableRowIndex].adcTiaGain & 0x03) << 4)
					 | ((uint8_t)(digGainMag & 0x0F));

		addrArray[addrIndex] = TALISE_ADDR_GAIN_TABLE_CONFIGURATION;
		dataArray[addrIndex++] = (ctlReg | 0x02);

		if (addrIndex >= spiBufferSize) {
			halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &dataArray[0],
						    addrIndex);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			addrIndex = 0;
		}
	}

	if (addrIndex > 0) {
		halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &dataArray[0],
					    addrIndex);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* disabling the gain table clock */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GAIN_TABLE_CONFIGURATION, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setRxManualGain(taliseDevice_t *device,
				taliseRxChannels_t rxChannel, uint8_t gainIndex)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRxManualGain()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Verify Rx profile is valid */
	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRXGAIN_RXPROFILE_INVALID, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxChannel == TAL_RX1) {
		if ((gainIndex < device->devStateInfo.gainIndexes.rx1MinGainIndex) ||
		    (gainIndex > device->devStateInfo.gainIndexes.rx1MaxGainIndex)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETRX1GAIN_INV_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_MANUAL_GAIN_INDEX_RX1,
					   (START_RX_GAIN_INDEX - (MAX_GAIN_TABLE_INDEX - gainIndex)));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else if (rxChannel == TAL_RX2) {
		if ((gainIndex < device->devStateInfo.gainIndexes.rx2MinGainIndex) ||
		    (gainIndex > device->devStateInfo.gainIndexes.rx2MaxGainIndex)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETRX2GAIN_INV_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_MANUAL_GAIN_INDEX_RX2,
					   (START_RX_GAIN_INDEX - (MAX_GAIN_TABLE_INDEX - gainIndex)));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getRxGain(taliseDevice_t *device, taliseRxChannels_t rxChannel,
			  uint8_t *rxGainIndex)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readData = 0;
	uint16_t regGainCh = TALISE_ADDR_RX1_GAIN_INDEX;
	uint8_t channelMaxGainIndex = START_RX_GAIN_INDEX;
	uint8_t channelMinGainIndex = MIN_RX_GAIN_TABLE_INDEX;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRxGain()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXGAIN_INV_RXPROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxChannel == TAL_RX1) {
		regGainCh = TALISE_ADDR_RX1_GAIN_INDEX;
	} else if (rxChannel == TAL_RX2) {
		regGainCh = TALISE_ADDR_RX2_GAIN_INDEX;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write the readback register to update the value */
	halError = talSpiWriteByte(device->devHalInfo, regGainCh, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* read back the current gain index for the required channel */
	halError = talSpiReadByte(device->devHalInfo, regGainCh, &readData);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* range check the gain against the max and min expected values */
	if ((readData > channelMaxGainIndex) ||
	    (readData < channelMinGainIndex)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXGAIN_GAIN_RANGE_EXCEEDED, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* normalize channel's gain index to 255 max gain index */
	readData = (MAX_GAIN_TABLE_INDEX - (channelMaxGainIndex - readData));

	if (rxGainIndex != NULL) {
		*rxGainIndex = readData;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setObsRxManualGain(taliseDevice_t *device,
				   taliseObsRxChannels_t obsRxCh, uint8_t gainIndex)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t manualGainIndexSpiAddr = 0;
	uint8_t orxGainIndex = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setObsRxManualGain()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Verify ORx profile is valid, also verifies device data struct pointers
	 * used below are valid
	 */
	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETORXGAIN_INV_ORXPROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* check for valid channel and assign address */
	switch(obsRxCh) {
	case TAL_ORX1:
		if ((gainIndex < device->devStateInfo.gainIndexes.orx1MinGainIndex) ||
		    (gainIndex > device->devStateInfo.gainIndexes.orx1MaxGainIndex)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETORXGAIN_INV_ORX1GAIN, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* GainIndex in data struct is normalized to 255 = max gain index */
		orxGainIndex = (START_ORX_GAIN_INDEX - (MAX_GAIN_TABLE_INDEX - gainIndex));
		manualGainIndexSpiAddr = TALISE_ADDR_MANUAL_GAIN_INDEX_ORX_RX1;
		break;

	case TAL_ORX2:
		if ((gainIndex < device->devStateInfo.gainIndexes.orx2MinGainIndex) ||
		    (gainIndex > device->devStateInfo.gainIndexes.orx2MaxGainIndex)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETORXGAIN_INV_ORX2GAIN, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* GainIndex in data struct is normalized to 255 = max gain index */
		orxGainIndex = (START_ORX_GAIN_INDEX - (MAX_GAIN_TABLE_INDEX - gainIndex));
		manualGainIndexSpiAddr = TALISE_ADDR_MANUAL_GAIN_INDEX_ORX_RX2;
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETORXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);

	}

	/* update the value in the device data structure, and write to device.*/
	halError = talSpiWriteByte(device->devHalInfo, manualGainIndexSpiAddr,
				   orxGainIndex);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getObsRxGain(taliseDevice_t *device,
			     taliseObsRxChannels_t obsRxChannel, uint8_t *obsRxGainIndex)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readData = 0;
	uint16_t regGainCh = TALISE_ADDR_ORX1_GAIN_INDEX;
	uint8_t channelMaxGainIndex = START_ORX_GAIN_INDEX;
	uint8_t channelMinGainIndex = MIN_ORX_GAIN_TABLE_INDEX;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getObsRxGain()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETOBSRXGAIN_INV_ORXPROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (obsRxChannel == TAL_ORX1) {
		regGainCh = TALISE_ADDR_ORX1_GAIN_INDEX;
	} else if (obsRxChannel == TAL_ORX2) {
		regGainCh = TALISE_ADDR_ORX2_GAIN_INDEX;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETOBSRXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write the readback register to update the value */
	halError = talSpiWriteByte(device->devHalInfo, regGainCh, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* read back the current gain index for the required channel */
	halError = talSpiReadByte(device->devHalInfo, regGainCh, &readData);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* range check the gain against the max and min expected values */
	if ((readData > channelMaxGainIndex) ||
	    (readData < channelMinGainIndex)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETOBSRXGAIN_GAIN_RANGE_EXCEEDED, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* normalize channel's gain index to 255 max gain index */
	readData = (MAX_GAIN_TABLE_INDEX - (channelMaxGainIndex - readData));

	if (obsRxGainIndex != NULL) {
		*obsRxGainIndex = readData;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setRxGainControlMode(taliseDevice_t *device,
				     taliseGainMode_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	static const uint8_t rxGainTypeBitMask = 0x03;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRxGainControlMode()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing AGC type check */
	switch (mode) {
	case TAL_MGC:
		break;
	case TAL_AGCSLOW:
		break;
	case TAL_AGCFAST: /* Fall through to next case */
	case TAL_HYBRID:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_UNSUPPORTED_RX_GAIN_MODE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_RX_GAIN_MODE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* read, modify, write AGC type for Rx1 */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG_RX1,
				    (uint8_t)mode, rxGainTypeBitMask, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* read, modify, write AGC type for Rx2 */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG_RX2,
				    (uint8_t)mode, rxGainTypeBitMask, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing mode setting to device state information container */
	device->devStateInfo.gainMode = mode;

	return (uint32_t)retVal;
}

uint32_t TALISE_setRxDataFormat (taliseDevice_t *device,
				 taliseRxDataFormat_t *rxDataFormat)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	uint8_t fpConfig = 0;
	uint8_t fpAtten = 0;
	uint8_t gainConfig = 0;
	uint8_t intConfig = 0;
	uint8_t slicerConfig = 0;
	uint8_t rx1GpioSelect = 0;
	uint8_t rx2GpioSelect = 0;
	uint8_t rx1Gpio = 0;
	uint8_t rx2Gpio = 0;
	uint8_t rx1SendGpio = 0;
	uint8_t rx2SendGpio = 0;
	uint8_t dualConfig = 0;
	uint8_t dualModeBit = 0;
	uint8_t combineDualband = 0;
	uint32_t srcCntrl = 0;
	uint32_t gpioAssignMask = 0;
	uint32_t gpioFreeMask = 0;
	uint32_t npCheck = 0;

	taliseRx1ExtSlicerGpioSelect_t rx1GpioConfig = TAL_EXTSLICER_RX1_GPIO_DISABLE;
	taliseRx2ExtSlicerGpioSelect_t rx2GpioConfig = TAL_EXTSLICER_RX2_GPIO_DISABLE;

	static const uint8_t rxChannelMask = 0x03;
	static const uint8_t GAIN_COMP_EN_BITM = 0x01;
	static const uint8_t PIN_CONTROL_BITM = 0x20;
	static const uint8_t SLICER_NUMBERBITS_BITM = 0x20;
	static const uint8_t SLICER_BIT_POSITION_BITM = 0x10;
	static const uint8_t SLICER_EMBED_EN_BITM = 0x08;
	static const uint8_t DATA_RESOLUTION_BITM = 0x06;
	static const uint8_t DATA_RESOLUTION_16BIT = 0x02;
	static const uint8_t DATA_RESOLUTION_24BIT = 0x04;
	static const uint8_t SIGNED_MAGNITUDE_BITM = 0x01;
	static const uint32_t GPIO0_1_2_BITM = 0x00000007;
	static const uint32_t GPIO5_6_7_BITM = 0x000000E0;
	static const uint32_t GPIO8_9_10_BITM = 0x00000700;
	static const uint32_t GPIO11_12_13_BITM = 0x00003800;
	static const uint32_t GPIO12_13_14_BITM = 0x00007000;
	static const uint32_t GPIO_OE_BITM = 0x7FFFF;
	static const uint32_t INTSLICER_NIBBLE_SOURCE = ((((uint32_t)
			TAL_GPIO_SLICER_OUT_MODE) << 12) | (((uint32_t)TAL_GPIO_SLICER_OUT_MODE) << 8));
	static const uint32_t INTSLICER_NIBBLE_CLEAR_BITM = 0xFFFF00FF;
	static const uint8_t DUALBANDENABLE = 1;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRxDataFormat()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Range check the configuration structure */
	if (rxDataFormat == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxDataFormat->formatSelect > TAL_GAIN_WITH_EXTERNAL_SLICER) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_FORMATSELECT_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxDataFormat->tempCompensationEnable > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_TEMPCOMP_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxDataFormat->intEmbeddedBits > TAL_EMBED_2_SLICERBITS_AT_LSB) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_INTEMBEDDEDBITS_INVPARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (rxDataFormat->intSampleResolution > TAL_INTEGER_24BIT_SIGNED) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_INTSAMPLERESOLUTION_INVPARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (rxDataFormat->extPinStepSize > TAL_EXTSLICER_STEPSIZE_4DB) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_PINSTEPSIZE_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxDataFormat->rx1GpioSelect > TAL_EXTSLICER_RX1_GPIO_DISABLE) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_RX1GPIOSELECT_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((rxDataFormat->rx2GpioSelect != TAL_EXTSLICER_RX2_GPIO11_12_13) &&
	    (rxDataFormat->rx2GpioSelect != TAL_EXTSLICER_RX2_GPIO5_6_7) &&
	    (rxDataFormat->rx2GpioSelect != TAL_EXTSLICER_RX2_GPIO_DISABLE)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_RX2GPIOSELECT_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxDataFormat->externalLnaGain > 1) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_RXDATAFRMT_EXTERNALLNAGAIN_INVPARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Calculate dualBandMode settings */
	if (device->devStateInfo.rxDualBandEnabled == DUALBANDENABLE) {

		/* if single Rx channel enabled then combine bands if M=2, separate bands if M=4 */
		if (((device->devStateInfo.initializedChannels & rxChannelMask) == 1) ||
		    ((device->devStateInfo.initializedChannels & rxChannelMask) == 2)) {
			combineDualband = (device->devStateInfo.rxTotalM == 2) ? 1 : 0;
		}

		/* if both Rx channels enabled then combine if M=4, separate if M=8 */
		else if ((device->devStateInfo.initializedChannels & rxChannelMask) == 3) {
			combineDualband = (device->devStateInfo.rxTotalM == 4) ? 1 : 0;
		}

		/* throw an error if no Rx channel enabled */
		else {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_RXCHAN_DISABLED, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* For floating point dualmode is set if combineDualband is set, cleared if combineDualband is clear */
		if (rxDataFormat->formatSelect == TAL_GAIN_WITH_FLOATING_POINT) {
			dualModeBit = combineDualband;
		}
		/* for internal slicer dualmode is set if no embedded bits, set if bits are embedded and combineDualband is set */
		else if ((rxDataFormat->formatSelect == TAL_GAIN_WITH_INTSLICER) ||
			 (rxDataFormat->formatSelect == TAL_GAIN_WITH_INTSLICER_NOGPIO)) {
			if (rxDataFormat->intEmbeddedBits ==  TAL_NO_EMBEDDED_SLICER_BITS) {
				dualModeBit = 1;
			} else {
				dualModeBit = combineDualband;
			}
		}
		/* for external slicer dualMode is always cleared */
		else {
			dualModeBit = 0;
		}
	}
	/* dualMode is cleared for TAL_RXDDC_DEC2 not selected */
	else {
		dualModeBit = 0;
	}

	dualConfig = (dualModeBit << 2) | ((rxDataFormat->externalLnaGain << 1) |
					   (rxDataFormat->tempCompensationEnable));

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GAIN_COMPENSATION_AND_SLICER_CONFIG2, dualConfig);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* read the Rx1 gpio config */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX1_SLICER_GPIO_CONFIG, &rx1SendGpio);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	rx1GpioConfig = (taliseRx1ExtSlicerGpioSelect_t)rx1SendGpio;

	/* deassign any rx1 gpios already in use */
	if (rx1GpioConfig == TAL_EXTSLICER_RX1_GPIO0_1_2) {
		gpioFreeMask = GPIO0_1_2_BITM;
	} else if (rx1GpioConfig == TAL_EXTSLICER_RX1_GPIO5_6_7) {
		gpioFreeMask = GPIO5_6_7_BITM;
	} else if (rx1GpioConfig == TAL_EXTSLICER_RX1_GPIO8_9_10) {
		gpioFreeMask = GPIO8_9_10_BITM;
	} else {
		gpioFreeMask = 0;
	}

	/* free the unused GPIOs */
	retVal = (talRecoveryActions_t)TALISE_setGpioOe(device,
			(~gpioFreeMask & GPIO_OE_BITM),  gpioFreeMask);
	IF_ERR_RETURN_U32(retVal);
	device->devStateInfo.usedGpiopins &= ~gpioFreeMask;

	rx1GpioSelect = TAL_EXTSLICER_RX1_GPIO_DISABLE;

	/* write gain configuration register */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_SLICER_GPIO_CONFIG, rx1GpioSelect);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* read the Rx2 gpio config */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX2_SLICER_GPIO_CONFIG, &rx2SendGpio);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	rx2GpioConfig = (taliseRx2ExtSlicerGpioSelect_t)rx2SendGpio;

	/* deassign any rx1 gpios already in use */
	if (rx2GpioConfig == TAL_EXTSLICER_RX2_GPIO5_6_7) {
		gpioFreeMask = GPIO5_6_7_BITM;
	} else if (rx2GpioConfig == TAL_EXTSLICER_RX2_GPIO11_12_13) {
		gpioFreeMask |= GPIO11_12_13_BITM;
	} else {
		gpioFreeMask = 0;
	}

	/* free the unused GPIOs */
	retVal = (talRecoveryActions_t)TALISE_setGpioOe(device,
			(~gpioFreeMask & GPIO_OE_BITM),  gpioFreeMask);
	IF_ERR_RETURN_U32(retVal);
	device->devStateInfo.usedGpiopins &= ~gpioFreeMask;

	rx2GpioSelect = TAL_EXTSLICER_RX2_GPIO_DISABLE;

	/* write gain configuration register */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_SLICER_GPIO_CONFIG, rx2GpioSelect);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* read the Gpio External Pin Configuration */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, &slicerConfig);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* if pin control is internal then check nibble source to see if gpios are in use */
	/* use case: integer with internal slicer outputting position on Gpios */
	if ((slicerConfig & PIN_CONTROL_BITM) == 0) {
		/* read nibble source */
		retVal = (talRecoveryActions_t)TALISE_getGpioSourceCtrl(device, &srcCntrl);
		IF_ERR_RETURN_U32(retVal);

		/* modify the nibble source control if gpios in use*/
		if ((srcCntrl & INTSLICER_NIBBLE_SOURCE) == INTSLICER_NIBBLE_SOURCE) {
			srcCntrl &= ~(INTSLICER_NIBBLE_SOURCE);

			/* write nibble source to clear nibbles */
			retVal = (talRecoveryActions_t)TALISE_setGpioSourceCtrl(device, srcCntrl);
			IF_ERR_RETURN_U32(retVal);

			/* turn unused pins to input */
			gpioAssignMask = GPIO8_9_10_BITM | GPIO12_13_14_BITM;
			retVal = (talRecoveryActions_t)TALISE_setGpioOe(device, 0x00, gpioAssignMask);
			IF_ERR_RETURN_U32(retVal);

			/* free the gpios */
			device->devStateInfo.usedGpiopins &= ~gpioAssignMask;
		}
	}

	/* set the configurations per selected mode type Floating Point */
	if (rxDataFormat->formatSelect == TAL_GAIN_WITH_FLOATING_POINT) {
		if (rxDataFormat->fpRoundMode > TAL_ROUND_FROM_EVEN) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_ROUNDMODE_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (rxDataFormat->fpDataFormat > 1) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_FPDATAFRMT_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (rxDataFormat->fpEncodeNan > 1) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_FPENCNAN_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (rxDataFormat->fpNumExpBits > TAL_5_EXPONENTBITS) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_FPEXPBITS_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (rxDataFormat->fpHideLeadingOne > 1) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_FPHIDELEADINGONE_INVPARAM, retVal,
							  TALACT_ERR_CHECK_PARAM);
		}

		if ((rxDataFormat->fpRx1Atten != TAL_FPATTEN_MINUS18DB) &&
		    (rxDataFormat->fpRx1Atten != TAL_FPATTEN_MINUS12DB) &&
		    (rxDataFormat->fpRx1Atten != TAL_FPATTEN_MINUS6DB) &&
		    (rxDataFormat->fpRx1Atten != TAL_FPATTEN_0DB) &&
		    (rxDataFormat->fpRx1Atten != TAL_FPATTEN_6DB) &&
		    (rxDataFormat->fpRx1Atten != TAL_FPATTEN_12DB) &&
		    (rxDataFormat->fpRx1Atten != TAL_FPATTEN_18DB) &&
		    (rxDataFormat->fpRx1Atten != TAL_FPATTEN_24DB)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_FPRX1ATTEN_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((rxDataFormat->fpRx2Atten != TAL_FPATTEN_MINUS18DB) &&
		    (rxDataFormat->fpRx2Atten != TAL_FPATTEN_MINUS12DB) &&
		    (rxDataFormat->fpRx2Atten != TAL_FPATTEN_MINUS6DB) &&
		    (rxDataFormat->fpRx2Atten != TAL_FPATTEN_0DB) &&
		    (rxDataFormat->fpRx2Atten != TAL_FPATTEN_6DB) &&
		    (rxDataFormat->fpRx2Atten != TAL_FPATTEN_12DB) &&
		    (rxDataFormat->fpRx2Atten != TAL_FPATTEN_18DB) &&
		    (rxDataFormat->fpRx2Atten != TAL_FPATTEN_24DB)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_FPRX2ATTEN_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* force embedded bits to zeros */
		/* write integer configuration register */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_INTEGER_FORMAT_CONFIG, 0x02);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* assemble and write float point config register */
		fpConfig = ((((((uint8_t)rxDataFormat->fpRoundMode << 5) |
			       (rxDataFormat->fpDataFormat << 4)) |
			      (rxDataFormat->fpEncodeNan << 3)) | ((uint8_t)rxDataFormat->fpNumExpBits << 1))
			    | rxDataFormat->fpHideLeadingOne);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_FLOATING_POINT_CONFIG, fpConfig);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* assemble and write rx attenuation register */
		fpAtten = (((rxDataFormat->fpRx2Atten & 0x07) << 3) | (rxDataFormat->fpRx1Atten
				& 0x07));

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_FLOATING_POINT_CONTROL, fpAtten);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* write the floating point enable to FP on */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_FLOATING_POINT_ENABLE, 0x03);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* read for gain configuration register */
		halError = talSpiReadByte(device->devHalInfo,
					  TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, &gainConfig);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		gainConfig |= GAIN_COMP_EN_BITM;  /* gain compensation on  */
		gainConfig &= ~PIN_CONTROL_BITM; /* pin control internal  */

		/* write gain configuration register */
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, gainConfig, 0xE1, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} /* end FP */

	/* Integer Mode or disabled */
	else {

		if (rxDataFormat->intEmbeddedBits == TAL_NO_EMBEDDED_SLICER_BITS) {
			intConfig &= ~SLICER_NUMBERBITS_BITM;   /* Set to default */
			intConfig &= ~SLICER_BIT_POSITION_BITM; /* Set to default */
			intConfig &=
				~SLICER_EMBED_EN_BITM;     /* Disable embedding slicer in data sample */
		} else if (rxDataFormat->intEmbeddedBits == TAL_EMBED_1_SLICERBIT_AT_MSB) {
			intConfig |= SLICER_NUMBERBITS_BITM;    /* 1 bit on I, 1 bit on Q */
			intConfig &= ~SLICER_BIT_POSITION_BITM; /* bits after sign at MSB */
			intConfig |= SLICER_EMBED_EN_BITM;      /* slicer embed enabled   */
		} else if (rxDataFormat->intEmbeddedBits == TAL_EMBED_1_SLICERBIT_AT_LSB) {
			intConfig |= SLICER_NUMBERBITS_BITM;    /* 1 bit on I, 1 bit on Q */
			intConfig |= SLICER_BIT_POSITION_BITM;  /* bits at LSB */
			intConfig |= SLICER_EMBED_EN_BITM;      /* slicer embed enabled   */
		} else if (rxDataFormat->intEmbeddedBits == TAL_EMBED_2_SLICERBITS_AT_MSB) {
			intConfig &= ~SLICER_NUMBERBITS_BITM;   /* 2 bit on I, 2 bit on Q */
			intConfig &= ~SLICER_BIT_POSITION_BITM; /* bits after sign at MSB */
			intConfig |= SLICER_EMBED_EN_BITM;      /* slicer embed enabled   */
		} else if (rxDataFormat->intEmbeddedBits == TAL_EMBED_2_SLICERBITS_AT_LSB) {
			intConfig &= ~SLICER_NUMBERBITS_BITM;   /* 2 bit on I, 2 bit on Q */
			intConfig |= SLICER_BIT_POSITION_BITM;  /* bits at LSB */
			intConfig |= SLICER_EMBED_EN_BITM;      /* slicer embed enabled   */
		}

		if (rxDataFormat->intSampleResolution == TAL_INTEGER_12BIT_2SCOMP) {
			intConfig &= ~DATA_RESOLUTION_BITM;  /* 12 bit  */
			intConfig &= ~SIGNED_MAGNITUDE_BITM; /* 2s comp */
			npCheck = 12;
		} else if (rxDataFormat->intSampleResolution == TAL_INTEGER_12BIT_SIGNED) {
			intConfig &= ~DATA_RESOLUTION_BITM;  /* 12 bit */
			intConfig |= SIGNED_MAGNITUDE_BITM;  /* signed */
			npCheck = 12;
		} else if (rxDataFormat->intSampleResolution == TAL_INTEGER_16BIT_2SCOMP) {
			intConfig |= DATA_RESOLUTION_16BIT;   /* 16 bit  */
			intConfig &= ~SIGNED_MAGNITUDE_BITM;  /* 2s comp */
			npCheck = 16;
		} else if (rxDataFormat->intSampleResolution == TAL_INTEGER_16BIT_SIGNED) {
			intConfig |= DATA_RESOLUTION_16BIT;   /* 16 bit */
			intConfig |= SIGNED_MAGNITUDE_BITM;   /* signed */
			npCheck = 16;
		} else if (rxDataFormat->intSampleResolution == TAL_INTEGER_24BIT_2SCOMP) {
			intConfig |= DATA_RESOLUTION_24BIT;   /* 16 bit  */
			intConfig &= ~SIGNED_MAGNITUDE_BITM;  /* 2s comp */
			npCheck = 24;
		} else {
			intConfig |= DATA_RESOLUTION_24BIT;   /* 16 bit */
			intConfig |= SIGNED_MAGNITUDE_BITM;   /* signed */
			npCheck = 24;
		}

		if (rxDataFormat->formatSelect == TAL_GAIN_COMPENSATION_DISABLED) {
			intConfig &= ~SLICER_NUMBERBITS_BITM;   /* 2 bit on I, 2 bit on Q */
			intConfig &= ~SLICER_BIT_POSITION_BITM; /* bits after sign at MSB */
			intConfig &= ~SLICER_EMBED_EN_BITM;     /* slicer embed disable */
		}

		/* JESD204 Framer Nprime must be greater than or equal to the intSampleResolution */
		if (((device->devStateInfo.profilesValid & RX_PROFILE_VALID) > 0) &&
		    (device->devStateInfo.rxFramerNp < npCheck)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_DATARES_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* JESD204 Framer Nprime must be greater than or equal to the intSampleResolution */
		if (((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0) &&
		    (device->devStateInfo.orxFramerNp < npCheck)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_RXDATAFRMT_DATARES_INVPARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* write the floating point enable to FP off */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_FLOATING_POINT_ENABLE, 0x00);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* write integer configuration register */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_INTEGER_FORMAT_CONFIG, intConfig);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		if (rxDataFormat->formatSelect == TAL_GAIN_WITH_INTSLICER) {
			/* verify needed GPIOs are unused */
			gpioAssignMask = GPIO8_9_10_BITM | GPIO12_13_14_BITM;
			if ((device->devStateInfo.usedGpiopins & gpioAssignMask) > 0) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SET_RXDATAFRMT_RX1GPIO_INUSE, retVal, TALACT_ERR_CHECK_PARAM);
			}

			slicerConfig = 0;
			slicerConfig |=
				GAIN_COMP_EN_BITM;                     /* gain compensation on  */
			slicerConfig &=
				~PIN_CONTROL_BITM;                     /* pin control internal  */
			slicerConfig |= ((uint8_t)rxDataFormat->extPinStepSize <<
					 6);   /* set gain step size    */

			/* write gain configuration register */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, slicerConfig, 0xE1, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			/* read nibble source */
			retVal = (talRecoveryActions_t)TALISE_getGpioSourceCtrl(device, &srcCntrl);
			IF_ERR_RETURN_U32(retVal);

			/* modify the nibble source control to allow outputs on GPIO 8,9,10, 12,13,14 */
			srcCntrl &= INTSLICER_NIBBLE_CLEAR_BITM;  /* clear bits [15:8] */
			srcCntrl |= INTSLICER_NIBBLE_SOURCE;

			/* write nibble source */
			retVal = (talRecoveryActions_t)TALISE_setGpioSourceCtrl(device, srcCntrl);
			IF_ERR_RETURN_U32(retVal);

			/* set used Gpios to output */
			retVal = (talRecoveryActions_t)TALISE_setGpioOe(device, gpioAssignMask,
					gpioAssignMask);
			IF_ERR_RETURN_U32(retVal);

			/* success so assign the gpios */
			device->devStateInfo.usedGpiopins |= gpioAssignMask;
		} /* end intslicer */
		else if (rxDataFormat->formatSelect == TAL_GAIN_WITH_INTSLICER_NOGPIO) {
			slicerConfig = 0;
			slicerConfig |=
				GAIN_COMP_EN_BITM;                    /* gain compensation on  */
			slicerConfig &=
				~PIN_CONTROL_BITM;                    /* pin control internal  */
			slicerConfig |= ((uint8_t)rxDataFormat->extPinStepSize <<
					 6);   /* set gain step size    */

			/* write gain configuration register */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, slicerConfig, 0xE1, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		} /* end nogpio */
		else if (rxDataFormat->formatSelect == TAL_GAIN_WITH_EXTERNAL_SLICER) {
			/* verify that rx1 GPIOs are not disabled */
			if (rxDataFormat->rx1GpioSelect == TAL_EXTSLICER_RX1_GPIO_DISABLE) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SET_RXDATAFRMT_EXTSLICER_RX1GPIO_INVPARAM, retVal,
								  TALACT_ERR_CHECK_PARAM);
			}

			/* verify that rx2 GPIOs are not disabled */
			if (rxDataFormat->rx2GpioSelect == TAL_EXTSLICER_RX2_GPIO_DISABLE) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SET_RXDATAFRMT_EXTSLICER_RX2GPIO_INVPARAM, retVal,
								  TALACT_ERR_CHECK_PARAM);
			}

			slicerConfig = 0;
			slicerConfig |=
				GAIN_COMP_EN_BITM;                     /* gain compensation on  */
			slicerConfig |=
				PIN_CONTROL_BITM;                     /* pin control external  */
			slicerConfig |= ((uint8_t)rxDataFormat->extPinStepSize <<
					 6);   /* set gain step size    */

			/* write gain configuration register */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, slicerConfig, 0xE1, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			if (rxDataFormat->rx1GpioSelect == TAL_EXTSLICER_RX1_GPIO0_1_2) {
				gpioAssignMask = GPIO0_1_2_BITM;
				rx1Gpio = TAL_EXTSLICER_RX1_GPIO0_1_2;
			} else if (rxDataFormat->rx1GpioSelect == TAL_EXTSLICER_RX1_GPIO5_6_7) {
				gpioAssignMask = GPIO5_6_7_BITM;
				rx1Gpio = TAL_EXTSLICER_RX1_GPIO5_6_7;
			} else if (rxDataFormat->rx1GpioSelect == TAL_EXTSLICER_RX1_GPIO8_9_10) {
				gpioAssignMask = GPIO8_9_10_BITM;
				rx1Gpio = TAL_EXTSLICER_RX1_GPIO8_9_10;
			}

			if ((device->devStateInfo.usedGpiopins & gpioAssignMask) > 0) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SET_RXDATAFRMT_RX1GPIO_INUSE, retVal, TALACT_ERR_CHECK_PARAM);
			}

			/* success so assign the gpios */
			device->devStateInfo.usedGpiopins |= gpioAssignMask;

			/* write the slicer register contents */
			halError = talSpiWriteByte(device->devHalInfo,
						   TALISE_ADDR_RX1_SLICER_GPIO_CONFIG, rx1Gpio);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
			rx1GpioConfig = (taliseRx1ExtSlicerGpioSelect_t) rx1Gpio;

			if (rxDataFormat->rx2GpioSelect == TAL_EXTSLICER_RX2_GPIO11_12_13) {
				gpioAssignMask = GPIO11_12_13_BITM;
				rx2Gpio = TAL_EXTSLICER_RX2_GPIO11_12_13;
			} else if (rxDataFormat->rx2GpioSelect == TAL_EXTSLICER_RX2_GPIO5_6_7) {
				gpioAssignMask = GPIO5_6_7_BITM;
				rx2Gpio = TAL_EXTSLICER_RX2_GPIO5_6_7;
			}

			/* check for special case of Rx1 and Rx2 slicer output on same GPIOs */
			if (((device->devStateInfo.usedGpiopins & gpioAssignMask) > 0) &&
			    (rx1GpioConfig != ((int32_t) TAL_EXTSLICER_RX1_GPIO5_6_7)) &&
			    (rxDataFormat->rx2GpioSelect == TAL_EXTSLICER_RX2_GPIO5_6_7)) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SET_RXDATAFRMT_RX2GPIO_INUSE, retVal, TALACT_ERR_CHECK_PARAM);
			} else if (((device->devStateInfo.usedGpiopins & gpioAssignMask) > 0) &&
				   (rxDataFormat->rx2GpioSelect == TAL_EXTSLICER_RX2_GPIO11_12_13)) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SET_RXDATAFRMT_RX2GPIO_INUSE, retVal, TALACT_ERR_CHECK_PARAM);
			}

			/* Insure Gpios to input */
			retVal = (talRecoveryActions_t)TALISE_setGpioOe(device,
					(~gpioAssignMask & GPIO_OE_BITM),  gpioAssignMask);
			IF_ERR_RETURN_U32(retVal);

			/* success so assign the gpios */
			device->devStateInfo.usedGpiopins |= gpioAssignMask;

			/* write the slicer register contents */
			halError = talSpiWriteByte(device->devHalInfo,
						   TALISE_ADDR_RX2_SLICER_GPIO_CONFIG, rx2Gpio);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		} /* end external */
		/* disable gain compensation */
		else if (rxDataFormat->formatSelect == TAL_GAIN_COMPENSATION_DISABLED) {
			/* read for gain configuration register */
			halError = talSpiReadByte(device->devHalInfo,
						  TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, &gainConfig);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			gainConfig &= ~GAIN_COMP_EN_BITM;         /* gain compensation off */
			gainConfig &= ~PIN_CONTROL_BITM;         /* pin control internal  */

			/* write gain configuration register */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, gainConfig, 0xE1, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	} /* end integer mode */

	return (uint32_t)retVal;
} /* end TALISE_setRxDataFormat */

uint32_t TALISE_getRxDataFormat(taliseDevice_t *device,
				taliseRxDataFormat_t *rxDataFormat)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	uint8_t config = 0;
	uint8_t fpEnable = 0;
	uint8_t intConfig = 0;
	uint8_t intResolution = 0;
	uint8_t gainEn = 0;
	uint32_t srcCntrl = 0;

	static const uint8_t GAIN_COMP_EN_BITM = 0x01;
	static const uint8_t PIN_CONTROL_BITM = 0x20;
	static const uint8_t EXT_LNA_COMP_BITM = 0x02;
	static const uint8_t TEMP_GAIN_BITM = 0x01;
	static const uint8_t ROUND_MODE_BITM = 0xE0;
	static const uint8_t MSBLSB_FORMAT_BITM = 0x10;
	static const uint8_t NUM_EXPBITS_BITM = 0x06;
	static const uint8_t FP_ENCNAN_BITM = 0x08;
	static const uint8_t FP_ENABLE_BITM = 0x03;
	static const uint8_t HIDE_LEADING_ONES_BITM = 0x01;
	static const uint8_t RX2_ATTEN_BITM = 0x38;
	static const uint8_t RX1_ATTEN_BITM = 0x07;
	static const uint8_t SLICER_NUMBER_BITM = 0x20;
	static const uint8_t SLICER_POSITION_BITM = 0x10;
	static const uint8_t EMBED_SLICER_BITM = 0x08;
	static const uint8_t SLICER_STEPSIZE_BITM = 0xC0;
	static const uint8_t DATA_RESOLUTION_BITM = 0x06;
	static const uint8_t DATA_RESOLUTION_16BIT = 0x02;
	static const uint8_t DATA_RESOLUTION_24BIT = 0x04;
	static const uint8_t SIGNED_MAGNITUDE_BITM = 0x01;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRxDataFormat()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (rxDataFormat == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GET_DATAFORMAT_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* fetch the floating point configuration */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_FLOATING_POINT_CONFIG,
				  &config);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	rxDataFormat->fpRoundMode = (taliseFpRoundModes_t) ((config & ROUND_MODE_BITM)
				    >> 5);
	rxDataFormat->fpDataFormat = ((config & MSBLSB_FORMAT_BITM) >> 4);
	rxDataFormat->fpEncodeNan = ((config & FP_ENCNAN_BITM) >> 3);
	rxDataFormat->fpNumExpBits = (taliseFpExponentModes_t) ((
					     config & NUM_EXPBITS_BITM) >> 1);
	rxDataFormat->fpHideLeadingOne = (config & HIDE_LEADING_ONES_BITM);

	/* fetch the Rx Attenuation */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_FLOATING_POINT_CONTROL, &config);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	rxDataFormat->fpRx2Atten = (taliseFpAttenSteps_t) ((config & RX2_ATTEN_BITM) >>
				   3);
	rxDataFormat->fpRx1Atten = (taliseFpAttenSteps_t) (config & RX1_ATTEN_BITM);

	/* fetch the Integer configuration */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_INTEGER_FORMAT_CONFIG,
				  &intConfig);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if ((intConfig & EMBED_SLICER_BITM) > 0) {
		if (((intConfig & SLICER_NUMBER_BITM) > 0) &&
		    ((intConfig & SLICER_POSITION_BITM) > 0)) {
			rxDataFormat->intEmbeddedBits = TAL_EMBED_1_SLICERBIT_AT_LSB;
		} else if (((intConfig & SLICER_NUMBER_BITM) == 0) &&
			   ((intConfig & SLICER_POSITION_BITM) > 0)) {
			rxDataFormat->intEmbeddedBits = TAL_EMBED_2_SLICERBITS_AT_LSB;
		} else if (((intConfig & SLICER_NUMBER_BITM) > 0) &&
			   ((intConfig & SLICER_POSITION_BITM) == 0)) {
			rxDataFormat->intEmbeddedBits = TAL_EMBED_1_SLICERBIT_AT_MSB;
		} else {
			rxDataFormat->intEmbeddedBits = TAL_EMBED_2_SLICERBITS_AT_MSB;
		}
	} else {
		rxDataFormat->intEmbeddedBits = TAL_NO_EMBEDDED_SLICER_BITS;
	}

	/* fetch the Integer data resolution */
	intResolution = intConfig;

	if (((intResolution & DATA_RESOLUTION_BITM) == 0) &&
	    ((intResolution & SIGNED_MAGNITUDE_BITM) > 0)) {
		rxDataFormat->intSampleResolution = TAL_INTEGER_12BIT_SIGNED;
	} else if (((intResolution & DATA_RESOLUTION_BITM) == 0) &&
		   ((intResolution & SIGNED_MAGNITUDE_BITM) == 0)) {
		rxDataFormat->intSampleResolution = TAL_INTEGER_12BIT_2SCOMP;
	} else if (((intResolution & DATA_RESOLUTION_BITM) == DATA_RESOLUTION_16BIT) &&
		   ((intResolution & SIGNED_MAGNITUDE_BITM) > 0)) {
		rxDataFormat->intSampleResolution = TAL_INTEGER_16BIT_SIGNED;
	} else if (((intResolution & DATA_RESOLUTION_BITM) == DATA_RESOLUTION_16BIT) &&
		   ((intResolution & SIGNED_MAGNITUDE_BITM) == 0)) {
		rxDataFormat->intSampleResolution = TAL_INTEGER_16BIT_2SCOMP;
	} else if (((intResolution & DATA_RESOLUTION_BITM) == DATA_RESOLUTION_24BIT) &&
		   ((intResolution & SIGNED_MAGNITUDE_BITM) > 0)) {
		rxDataFormat->intSampleResolution = TAL_INTEGER_24BIT_SIGNED;
	} else {
		rxDataFormat->intSampleResolution = TAL_INTEGER_24BIT_2SCOMP;
	}

	/* fetch the Floating Point enable */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_FLOATING_POINT_ENABLE,
				  &fpEnable);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* fetch the gain control configuration */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GAIN_COMPENATION_AND_SLICER_CONFIG, &gainEn);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	rxDataFormat->extPinStepSize = (taliseGainStepSize_t) ((
					       gainEn & SLICER_STEPSIZE_BITM) >> 6);

	/* fetch the rx1 gpio configuration */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX1_SLICER_GPIO_CONFIG, &config);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	rxDataFormat->rx1GpioSelect = (taliseRx1ExtSlicerGpioSelect_t) config;

	/* fetch the rx2 gpio configuration */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_RX2_SLICER_GPIO_CONFIG, &config);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	rxDataFormat->rx2GpioSelect = (taliseRx2ExtSlicerGpioSelect_t) config;

	if ((gainEn & GAIN_COMP_EN_BITM) > 0) {
		if ((fpEnable & FP_ENABLE_BITM) > 0) {
			rxDataFormat->formatSelect = TAL_GAIN_WITH_FLOATING_POINT;
		} else if ((gainEn & PIN_CONTROL_BITM) > 0) {
			rxDataFormat->formatSelect = TAL_GAIN_WITH_EXTERNAL_SLICER;
		} else {
			/* read nibble source */
			retVal = (talRecoveryActions_t)TALISE_getGpioSourceCtrl(device, &srcCntrl);
			IF_ERR_RETURN_U32(retVal);

			/* modify the nibble source control */
			if ((srcCntrl & 0xFF00) == ((TAL_GPIO_SLICER_OUT_MODE << 12) |
						    (TAL_GPIO_SLICER_OUT_MODE << 8))) {
				rxDataFormat->formatSelect = TAL_GAIN_WITH_INTSLICER;
			} else {
				rxDataFormat->formatSelect = TAL_GAIN_WITH_INTSLICER_NOGPIO;
			}
		}
	} else {
		rxDataFormat->formatSelect = TAL_GAIN_COMPENSATION_DISABLED;
	}

	/* fetch the misc gain configuration */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GAIN_COMPENSATION_AND_SLICER_CONFIG2, &config);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	rxDataFormat->externalLnaGain = ((config & EXT_LNA_COMP_BITM) >> 1);
	rxDataFormat->tempCompensationEnable = (config & TEMP_GAIN_BITM);

	return (uint32_t)retVal;
}

uint32_t TALISE_getSlicerPosition(taliseDevice_t *device,
				  uint8_t *rx1SlicerPosition, uint8_t *rx2SlicerPosition)
{

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t slicerPosition = 0;
	static const uint8_t POSITION_MASK = 0x07;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getSlicerPosition()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((rx1SlicerPosition == NULL) ||
	    (rx2SlicerPosition == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GET_SLICERPOS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* read the slicer positions */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_RXDP_SLICER_READBACK,
				  &slicerPosition);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*rx1SlicerPosition = slicerPosition & POSITION_MASK;
	*rx2SlicerPosition = (slicerPosition >> 3) & POSITION_MASK;

	return (uint32_t)retVal;
}

static talRecoveryActions_t talSetRx1GainCtrlPin (taliseDevice_t *device,
		uint8_t enable, taliseGpioPinSel_t rxGainIncPin,
		taliseGpioPinSel_t rxGainDecPin)
{
	uint8_t registerValue = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint32_t usedGpiopins = 0;
	uint32_t gpioFreeMask = 0;
	uint32_t gpioUsedMask = 0;

	static const uint16_t TAL_RX1_INC_GAIN_GPIO0   = 0x00;
	static const uint16_t TAL_RX1_INC_GAIN_GPIO10  = 0x01;
	static const uint16_t TAL_RX1_INC_GAIN_DISABLE = 0x03;
	static const uint16_t TAL_RX1_DEC_GAIN_GPIO1   = 0x00;
	static const uint16_t TAL_RX1_DEC_GAIN_GPIO11  = 0x01;
	static const uint16_t TAL_RX1_DEC_GAIN_DISABLE = 0x03;


	uint16_t rx1GainIncPinSel = TAL_RX1_INC_GAIN_DISABLE;
	uint16_t rx1GainDecPinSel = TAL_RX1_DEC_GAIN_DISABLE;

	/* Free all used GPIO's based on current configuration*/
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX1, &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);
	rx1GainIncPinSel = (uint16_t)((registerValue >> 4) & 0x03);
	rx1GainDecPinSel = (uint16_t)((registerValue >> 2) & 0x03);

	if (rx1GainIncPinSel == TAL_RX1_INC_GAIN_GPIO0) {
		gpioFreeMask |= (1 << TAL_GPIO_00);
	}

	else if (rx1GainIncPinSel == TAL_RX1_INC_GAIN_GPIO10) {
		gpioFreeMask |= (1 << TAL_GPIO_10);
	}

	if (rx1GainDecPinSel == TAL_RX1_DEC_GAIN_GPIO1) {
		gpioFreeMask |= (1 << TAL_GPIO_01);
	}

	else if (rx1GainDecPinSel == TAL_RX1_DEC_GAIN_GPIO11) {
		gpioFreeMask |= (1 << TAL_GPIO_11);
	}

	usedGpiopins = device->devStateInfo.usedGpiopins & ~gpioFreeMask;

	/* check if GPIO's are available for the new configuration*/
	if (enable > 0) {
		gpioUsedMask |= (1 << rxGainIncPin);
		gpioUsedMask |= (1 << rxGainDecPin);

		if ((usedGpiopins & gpioUsedMask) != 0) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETRXMGCPINCTRL_RX1_GPIO_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpiopins |= gpioUsedMask;
		rx1GainIncPinSel = (rxGainIncPin == TAL_GPIO_00) ? TAL_RX1_INC_GAIN_GPIO0 :
				   TAL_RX1_INC_GAIN_GPIO10;
		rx1GainDecPinSel = (rxGainDecPin == TAL_GPIO_01) ? TAL_RX1_DEC_GAIN_GPIO1 :
				   TAL_RX1_DEC_GAIN_GPIO11;
	} else {
		rx1GainIncPinSel = TAL_RX1_INC_GAIN_DISABLE;
		rx1GainDecPinSel = TAL_RX1_DEC_GAIN_DISABLE;
	}

	/* Program Pin mode configuration */
	registerValue = (((uint8_t)rx1GainIncPinSel) << 4) | (((
				uint8_t)rx1GainDecPinSel) << 2) | ((enable > 0) ? 0x01 : 0x00);
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX1, registerValue, 0x3D, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO's to 0 (input) */
	/* - Set OE for all used GPIO's to 0 (input) */
	retVal = (talRecoveryActions_t)TALISE_setGpioOe(device, 0,
			(gpioUsedMask | gpioFreeMask));
	IF_ERR_RETURN(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpiopins = usedGpiopins;

	return retVal;
}

static talRecoveryActions_t talSetRx2GainCtrlPin (taliseDevice_t *device,
		uint8_t enable, taliseGpioPinSel_t rxGainIncPin,
		taliseGpioPinSel_t rxGainDecPin)
{
	uint8_t registerValue = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint32_t usedGpiopins = 0;
	uint32_t gpioFreeMask = 0;
	uint32_t gpioUsedMask = 0;

	static const uint16_t TAL_RX2_INC_GAIN_GPIO3   = 0x00;
	static const uint16_t TAL_RX2_INC_GAIN_GPIO13  = 0x01;
	static const uint16_t TAL_RX2_INC_GAIN_DISABLE = 0x03;
	static const uint16_t TAL_RX2_DEC_GAIN_GPIO4   = 0x00;
	static const uint16_t TAL_RX2_DEC_GAIN_GPIO14  = 0x01;
	static const uint16_t TAL_RX2_DEC_GAIN_DISABLE = 0x03;

	uint16_t rx2GainIncPinSel = TAL_RX2_INC_GAIN_DISABLE;
	uint16_t rx2GainDecPinSel = TAL_RX2_DEC_GAIN_DISABLE;

	/* Free all used GPIO's based on current configuration*/
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX2, &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);
	rx2GainIncPinSel = (uint16_t)((registerValue >> 4) & 0x03);
	rx2GainDecPinSel = (uint16_t)((registerValue >> 2) & 0x03);

	if (rx2GainIncPinSel == TAL_RX2_INC_GAIN_GPIO3) {
		gpioFreeMask |= (1 << TAL_GPIO_03);
	}

	else if (rx2GainIncPinSel == TAL_RX2_INC_GAIN_GPIO13) {
		gpioFreeMask |= (1 << TAL_GPIO_13);
	}

	if (rx2GainDecPinSel == TAL_RX2_DEC_GAIN_GPIO4) {
		gpioFreeMask |= (1 << TAL_GPIO_04);
	}

	else if (rx2GainDecPinSel == TAL_RX2_DEC_GAIN_GPIO14) {
		gpioFreeMask |= (1 << TAL_GPIO_14);
	}

	usedGpiopins = device->devStateInfo.usedGpiopins & ~gpioFreeMask;

	/* check if GPIO's are available for the new configuration*/
	if (enable > 0) {
		gpioUsedMask |= (1 << rxGainIncPin);
		gpioUsedMask |= (1 << rxGainDecPin);

		if ((usedGpiopins & gpioUsedMask) != 0) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETRXMGCPINCTRL_RX2_GPIO_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpiopins |= gpioUsedMask;
		rx2GainIncPinSel = (rxGainIncPin == TAL_GPIO_03) ? TAL_RX2_INC_GAIN_GPIO3 :
				   TAL_RX2_INC_GAIN_GPIO13;
		rx2GainDecPinSel = (rxGainDecPin == TAL_GPIO_04) ? TAL_RX2_DEC_GAIN_GPIO4 :
				   TAL_RX2_DEC_GAIN_GPIO14;
	} else {
		rx2GainIncPinSel = TAL_RX2_INC_GAIN_DISABLE;
		rx2GainDecPinSel = TAL_RX2_DEC_GAIN_DISABLE;
	}

	/* Program Pin mode configuration */
	registerValue = (((uint8_t)rx2GainIncPinSel) << 4) | (((
				uint8_t)rx2GainDecPinSel) << 2) | ((enable > 0) ? 0x01 : 0x00);
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX2, registerValue, 0x3D, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO's to 0 (input) */
	/* - Set OE for all used GPIO's to 0 (input) */
	retVal = (talRecoveryActions_t)TALISE_setGpioOe(device, 0,
			(gpioUsedMask | gpioFreeMask));
	IF_ERR_RETURN(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpiopins = usedGpiopins;

	return retVal;
}

uint32_t TALISE_setRxGainCtrlPin(taliseDevice_t *device,
				 taliseRxChannels_t rxChannel, taliseRxGainCtrlPin_t *rxGainCtrlPin)
{
	uint8_t regVal = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	static const uint8_t MAX_STEP = 0x07;

#if TALISE_VERBOSE
	halError =talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				"TALISE_setRxGainCtrlPin()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	switch (rxChannel) {
	case TAL_RX1:
		if ( rxGainCtrlPin->enable > 0 ) {
			if ((rxGainCtrlPin->rxGainIncPin != TAL_GPIO_00)
			    && (rxGainCtrlPin->rxGainIncPin != TAL_GPIO_10)) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SETRXMGCPINCTRL_INV_RX1_INC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
			}

			if ((rxGainCtrlPin->rxGainDecPin != TAL_GPIO_01)
			    && (rxGainCtrlPin->rxGainDecPin != TAL_GPIO_11)) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SETRXMGCPINCTRL_INV_RX1_DEC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
			}
		}
		retVal = talSetRx1GainCtrlPin(device, rxGainCtrlPin->enable,
					      rxGainCtrlPin->rxGainIncPin, rxGainCtrlPin->rxGainDecPin);
		IF_ERR_RETURN_U32(retVal);
		break;

	case TAL_RX2:
		if (rxGainCtrlPin->enable > 0) {
			if ((rxGainCtrlPin->rxGainIncPin != TAL_GPIO_03)
			    && (rxGainCtrlPin->rxGainIncPin != TAL_GPIO_13)) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SETRXMGCPINCTRL_INV_RX2_INC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
			}

			if ((rxGainCtrlPin->rxGainDecPin != TAL_GPIO_04)
			    && (rxGainCtrlPin->rxGainDecPin != TAL_GPIO_14)) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_SETRXMGCPINCTRL_INV_RX2_DEC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
			}
		}
		retVal = talSetRx2GainCtrlPin(device, rxGainCtrlPin->enable,
					      rxGainCtrlPin->rxGainIncPin, rxGainCtrlPin->rxGainDecPin);
		IF_ERR_RETURN_U32(retVal);
		break;

	default: {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRXMGCPINCTRL_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	/* Error checking for gain steps. */
	if (rxGainCtrlPin->incStep > MAX_STEP) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRXMGCPINCTRL_INV_INC_STEP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxGainCtrlPin->decStep > MAX_STEP) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRXMGCPINCTRL_INV_DEC_STEP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write Gain step */
	regVal = (rxGainCtrlPin->incStep << 3) | rxGainCtrlPin->decStep;
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MANUAL_GAIN_CONFIG,
				   regVal);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getRxGainCtrlPin(taliseDevice_t *device,
				 taliseRxChannels_t rxChannel, taliseRxGainCtrlPin_t *rxGainCtrlPin)
{
	uint8_t regVal = 0;
	uint8_t pinSel = 0;
	uint16_t mgcConfigAddr = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError =talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				"TALISE_getRxGainCtrlPin()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Read current value of Gain inc/dec and enable */
	switch (rxChannel) {
	case TAL_RX1:
		mgcConfigAddr = TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX1;
		break;

	case TAL_RX2:
		mgcConfigAddr = TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX2;
		break;

	default: {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXMGCPINCTRL_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	/*Check passed pointers for NULL */
	if (rxGainCtrlPin == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXMGCPINCTRL_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Gain Inc, Gain dec, enable  */
	halError = talSpiReadByte(device->devHalInfo, mgcConfigAddr, &regVal);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	pinSel = (regVal >> 4) & 0x03;
	rxGainCtrlPin->rxGainIncPin = (rxChannel == TAL_RX1) ? ((
					      pinSel == 0x00) ? TAL_GPIO_00 : (( pinSel == 0x01) ? TAL_GPIO_10 :
							      TAL_GPIO_INVALID))
				      : ((pinSel == 0x00) ? TAL_GPIO_03 : (( pinSel == 0x01) ? TAL_GPIO_13 :
						      TAL_GPIO_INVALID));
	pinSel = (regVal >> 2) & 0x03;
	rxGainCtrlPin->rxGainDecPin = (rxChannel == TAL_RX1) ? ((
					      pinSel == 0x00) ? TAL_GPIO_01 : (( pinSel == 0x01) ? TAL_GPIO_11 :
							      TAL_GPIO_INVALID))
				      : ((pinSel == 0x00) ? TAL_GPIO_04 : (( pinSel == 0x01) ? TAL_GPIO_14 :
						      TAL_GPIO_INVALID));
	rxGainCtrlPin->enable = regVal & 0x01;

	/* Gain step */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MANUAL_GAIN_CONFIG,
				  &regVal);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	rxGainCtrlPin->incStep = (regVal >> 3) & 0x7;
	rxGainCtrlPin->decStep = regVal & 0x7;

	return (uint32_t)retVal;
}

talRecoveryActions_t talSetDualBandSettings(taliseDevice_t *device,
		taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	int32_t pfirFs_kHz = 0;
	int32_t pfirFsDiv2_kHz = 0;
	int32_t outFs_kHz = 0;
	int32_t outFsDiv2_kHz = 0;
	int32_t upperBandBandwidthDiv2_kHz = 0;
	int32_t lowerBandBandwidthDiv2_kHz = 0;
	int32_t bandSeparation_kHz = 0;
	int32_t maxLowerBandShiftCenterFreq_kHz = 0;
	int32_t minLowerBandShiftCenterFreq_kHz = 0;
	int32_t lowerBandShiftCenterFreq_kHz = 0;
	int32_t lowerBandNco1Freq_kHz = 0;
	int32_t maxUpperBandShiftCenterFreq_kHz = 0;
	int32_t minUpperBandShiftCenterFreq_kHz = 0;
	int32_t upperBandShiftCenterFreq_kHz = 0;
	int32_t upperBandNco1Freq_kHz = 0;
	int32_t lowerBandNco2Freq_kHz = 0;
	int32_t upperBandNco2Freq_kHz = 0;
	uint32_t lowerBandNco1FreqUnsigned_kHz = 0;
	uint32_t upperBandNco1FreqUnsigned_kHz = 0;
	uint32_t lowerBandNco2FreqUnsigned_kHz = 0;
	uint32_t upperBandNco2FreqUnsigned_kHz = 0;
	uint64_t lowerBandNco1Ftw = 0;
	uint64_t upperBandNco1Ftw = 0;
	uint64_t lowerBandNco2Ftw = 0;
	uint64_t upperBandNco2Ftw = 0;

	int32_t inUpperBandCenterFreq_kHz = 0;
	int32_t inLowerBandCenterFreq_kHz = 0;
	int32_t outUpperBandCenterFreq_kHz = 0;
	int32_t outLowerBandCenterFreq_kHz = 0;

	static const uint64_t POW_2_32 = ((uint64_t)1) << 32;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "talSetDualBandSettings()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	inUpperBandCenterFreq_kHz =
		init->rx.rxProfile.rxNcoShifterCfg.bandAInputCenterFreq_kHz;
	inLowerBandCenterFreq_kHz =
		init->rx.rxProfile.rxNcoShifterCfg.bandBInputCenterFreq_kHz;

	outUpperBandCenterFreq_kHz = inUpperBandCenterFreq_kHz +
				     init->rx.rxProfile.rxNcoShifterCfg.bandANco1Freq_kHz +
				     init->rx.rxProfile.rxNcoShifterCfg.bandANco2Freq_kHz;
	outLowerBandCenterFreq_kHz = inLowerBandCenterFreq_kHz +
				     init->rx.rxProfile.rxNcoShifterCfg.bandBNco1Freq_kHz +
				     init->rx.rxProfile.rxNcoShifterCfg.bandBNco2Freq_kHz;

	/* Fs at output of PFIR : Twice output rate due to decimation in DDC */
	pfirFsDiv2_kHz = device->devStateInfo.rxOutputRate_kHz;
	pfirFs_kHz = pfirFsDiv2_kHz << 1;

	/* Output Fs and Fs/2 */
	outFs_kHz = pfirFsDiv2_kHz;
	outFsDiv2_kHz = outFs_kHz >> 1;

	/* Bandwidth /2 */
	upperBandBandwidthDiv2_kHz =
		(init->rx.rxProfile.rxNcoShifterCfg.bandAInputBandWidth_kHz >> 1);
	lowerBandBandwidthDiv2_kHz =
		(init->rx.rxProfile.rxNcoShifterCfg.bandBInputBandWidth_kHz >> 1);

	/* Check : Center frequency of input lower band is less than input upper band */
	if (inLowerBandCenterFreq_kHz > inUpperBandCenterFreq_kHz) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_INV_CENTER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : Band should be separated by at least 1 khz */
	if (((inUpperBandCenterFreq_kHz - upperBandBandwidthDiv2_kHz) -
	     (inLowerBandCenterFreq_kHz + lowerBandBandwidthDiv2_kHz)) < 0) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_INV_BAND_SEP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : Upper end of upper band input is less than +PfirFs/2 */
	if ((inUpperBandCenterFreq_kHz + upperBandBandwidthDiv2_kHz) > pfirFsDiv2_kHz) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_INV_IN_UPPER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : Lower end of lower band input is more than -PfirFs/2 */
	if ((inLowerBandCenterFreq_kHz - lowerBandBandwidthDiv2_kHz) <
	    -pfirFsDiv2_kHz) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_INV_IN_LOWER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : upper band output is between -OutFs/2 and  +OutFs/2 */
	if (((outUpperBandCenterFreq_kHz + upperBandBandwidthDiv2_kHz) > outFsDiv2_kHz)
	    ||
	    ((outUpperBandCenterFreq_kHz - upperBandBandwidthDiv2_kHz) < -outFsDiv2_kHz)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_INV_OUT_UPPER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : lower band output is between -OutFs/2 and  +OutFs/2 */
	if (((outLowerBandCenterFreq_kHz + lowerBandBandwidthDiv2_kHz) > outFsDiv2_kHz)
	    ||
	    ((outLowerBandCenterFreq_kHz - lowerBandBandwidthDiv2_kHz) < -outFsDiv2_kHz)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_INV_OUT_LOWER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Output upper and lower bands dont overlap */
	/* Error if 'lower end of lowerBand' < 'higher end of upperBand' and 'lower end of upperBand' < 'higher end of lowerBand' */
	if (((outLowerBandCenterFreq_kHz - lowerBandBandwidthDiv2_kHz) <
	     (outUpperBandCenterFreq_kHz + upperBandBandwidthDiv2_kHz)) &&
	    ((outUpperBandCenterFreq_kHz - upperBandBandwidthDiv2_kHz) <
	     (outLowerBandCenterFreq_kHz + lowerBandBandwidthDiv2_kHz))) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_OUT_OVERLAP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Separation between the two bands */
	bandSeparation_kHz = (pfirFs_kHz - lowerBandBandwidthDiv2_kHz -
			      upperBandBandwidthDiv2_kHz -
			      (inUpperBandCenterFreq_kHz - inLowerBandCenterFreq_kHz));

	/* Move lower band up to the upper end of DEC2 passband. */
	/* This value assumes that the upper band does not alias back into the lower side of DEC2 passband */
	maxLowerBandShiftCenterFreq_kHz = (outFsDiv2_kHz - lowerBandBandwidthDiv2_kHz);

	/* This value assumes that upper band can alias back into lower side of DEC2 passband. The shift is hence limited by band separation  */
	minLowerBandShiftCenterFreq_kHz = (-outFsDiv2_kHz + bandSeparation_kHz +
					   lowerBandBandwidthDiv2_kHz);

	/* The value that the lower band can be shifted upwards is the minimum of the two values */
	lowerBandShiftCenterFreq_kHz = (maxLowerBandShiftCenterFreq_kHz <
					minLowerBandShiftCenterFreq_kHz) ? maxLowerBandShiftCenterFreq_kHz :
				       minLowerBandShiftCenterFreq_kHz;

	/* NCO1 Frequency to tune to for lower band: NCO frequency = Fout - Fin , since I and Q are swapped */
	lowerBandNco1Freq_kHz = (lowerBandShiftCenterFreq_kHz -
				 inLowerBandCenterFreq_kHz);

	/* Move upper band down to the lower end of DEC2 passband. */
	/* This value assumes that the lower band does not alias back into the upper side of DEC2 passband */
	maxUpperBandShiftCenterFreq_kHz = (-outFsDiv2_kHz + upperBandBandwidthDiv2_kHz);

	/* This value assumes that lower band can alias back into upper side of DEC2 passband. The shift is hence limited by band separation  */
	minUpperBandShiftCenterFreq_kHz = (outFsDiv2_kHz - bandSeparation_kHz -
					   upperBandBandwidthDiv2_kHz) ;

	/* The value that the Upper band can be shifted downwards is the max of the two values */
	upperBandShiftCenterFreq_kHz = (maxUpperBandShiftCenterFreq_kHz >
					minUpperBandShiftCenterFreq_kHz) ? maxUpperBandShiftCenterFreq_kHz :
				       minUpperBandShiftCenterFreq_kHz;

	/* NCO1 Frequency to tune to for upper: NCO frequency = Fout - Fin , since I and Q are swapped*/
	upperBandNco1Freq_kHz = (upperBandShiftCenterFreq_kHz -
				 inUpperBandCenterFreq_kHz);

	/* To convert from lowerBandShiftCenterFreq_kHz to outLowerBandCenterFreq_kHz */
	lowerBandNco2Freq_kHz = (outLowerBandCenterFreq_kHz -
				 lowerBandShiftCenterFreq_kHz);

	/* To convert from upperBandShiftCenterFreq_kHz to outUpperBandCenterFreq_kHz */
	upperBandNco2Freq_kHz = (outUpperBandCenterFreq_kHz -
				 upperBandShiftCenterFreq_kHz);

	/* Convert negative to positive frequencies */
	lowerBandNco1FreqUnsigned_kHz = (lowerBandNco1Freq_kHz > 0) ?
					lowerBandNco1Freq_kHz : (pfirFs_kHz + lowerBandNco1Freq_kHz);
	upperBandNco1FreqUnsigned_kHz = (upperBandNco1Freq_kHz > 0) ?
					upperBandNco1Freq_kHz : (pfirFs_kHz + upperBandNco1Freq_kHz);
	lowerBandNco2FreqUnsigned_kHz = (lowerBandNco2Freq_kHz > 0) ?
					lowerBandNco2Freq_kHz : (outFs_kHz + lowerBandNco2Freq_kHz);
	upperBandNco2FreqUnsigned_kHz = (upperBandNco2Freq_kHz > 0) ?
					upperBandNco2Freq_kHz : (outFs_kHz + upperBandNco2Freq_kHz);

	/* Convert positive frequencies to their FTW equivalents */
	lowerBandNco1Ftw = DIV_U64((((uint64_t)lowerBandNco1FreqUnsigned_kHz * POW_2_32)
				    << 1) + pfirFs_kHz, pfirFs_kHz << 1); /* integer rounding */
	upperBandNco1Ftw = DIV_U64((((uint64_t)upperBandNco1FreqUnsigned_kHz * POW_2_32)
				    << 1) + pfirFs_kHz, pfirFs_kHz << 1); /* integer rounding */
	lowerBandNco2Ftw = DIV_U64((((uint64_t)lowerBandNco2FreqUnsigned_kHz * POW_2_32)
				    << 1) + outFs_kHz, outFs_kHz << 1); /* integer rounding */
	upperBandNco2Ftw = DIV_U64((((uint64_t)upperBandNco2FreqUnsigned_kHz * POW_2_32)
				    << 1) + outFs_kHz, outFs_kHz << 1); /* integer rounding */

	/* Check FTW is not more than 32 bits */
	if ((lowerBandNco1Ftw >= POW_2_32) || (upperBandNco1Ftw >= POW_2_32) ||
	    (lowerBandNco2Ftw >= POW_2_32) || (upperBandNco2Ftw >= POW_2_32)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETDUALBANDSETTINGS_FTW_OVRG, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write to FTW - lower Band NCO 1*/
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH1_FTW_BYTE4,
				   (uint8_t)(lowerBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH1_FTW_BYTE3,
				   (uint8_t)(lowerBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH1_FTW_BYTE2,
				   (uint8_t)(lowerBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH1_FTW_BYTE1,
				   (uint8_t)(lowerBandNco1Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH2_FTW_BYTE4,
				   (uint8_t)(lowerBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH2_FTW_BYTE3,
				   (uint8_t)(lowerBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH2_FTW_BYTE2,
				   (uint8_t)(lowerBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_NCO_CH2_FTW_BYTE1,
				   (uint8_t)(lowerBandNco1Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Write to FTW - upper Band NCO 1*/
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO1_FTW_BYTE4, (uint8_t)(upperBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO1_FTW_BYTE3, (uint8_t)(upperBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO1_FTW_BYTE2, (uint8_t)(upperBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO1_FTW_BYTE1, (uint8_t)(upperBandNco1Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO1_FTW_BYTE4, (uint8_t)(upperBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO1_FTW_BYTE3, (uint8_t)(upperBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO1_FTW_BYTE2, (uint8_t)(upperBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO1_FTW_BYTE1, (uint8_t)(upperBandNco1Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Write to FTW - lower Band NCO 2*/
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_A_NCO2_FTW_BYTE4, (uint8_t)(lowerBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_A_NCO2_FTW_BYTE3, (uint8_t)(lowerBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_A_NCO2_FTW_BYTE2, (uint8_t)(lowerBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_A_NCO2_FTW_BYTE1, (uint8_t)(lowerBandNco2Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_A_NCO2_FTW_BYTE4, (uint8_t)(lowerBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_A_NCO2_FTW_BYTE3, (uint8_t)(lowerBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_A_NCO2_FTW_BYTE2, (uint8_t)(lowerBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_A_NCO2_FTW_BYTE1, (uint8_t)(lowerBandNco2Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Write to FTW - upper Band NCO 2*/
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO2_FTW_BYTE4, (uint8_t)(upperBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO2_FTW_BYTE3, (uint8_t)(upperBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO2_FTW_BYTE2, (uint8_t)(upperBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX1_BAND_B_NCO2_FTW_BYTE1, (uint8_t)(upperBandNco2Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO2_FTW_BYTE4, (uint8_t)(upperBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO2_FTW_BYTE3, (uint8_t)(upperBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO2_FTW_BYTE2, (uint8_t)(upperBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX2_BAND_B_NCO2_FTW_BYTE1, (uint8_t)(upperBandNco2Ftw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Update FTW words */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX_NCO_FTW_UPDATE_CONTROL, 0xFF);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return retVal;
}

static talRecoveryActions_t talSetRxNcoFtw(taliseDevice_t *device,
		taliseRxNcoChannel_t rxNcoChannel, uint32_t ncoFtw)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t spiAddr = 0;

	switch(rxNcoChannel) {
	case TAL_RX1_NCO1A:
		spiAddr = TALISE_ADDR_RX_NCO_CH1_FTW_BYTE4;
		break;
	case TAL_RX1_NCO1B:
		spiAddr = TALISE_ADDR_RX1_BAND_B_NCO1_FTW_BYTE4;
		break;
	case TAL_RX1_NCO2A:
		spiAddr = TALISE_ADDR_RX1_BAND_A_NCO2_FTW_BYTE4;
		break;
	case TAL_RX1_NCO2B:
		spiAddr = TALISE_ADDR_RX1_BAND_B_NCO2_FTW_BYTE4;
		break;
	case TAL_RX2_NCO1A:
		spiAddr = TALISE_ADDR_RX_NCO_CH2_FTW_BYTE4;
		break;
	case TAL_RX2_NCO1B:
		spiAddr = TALISE_ADDR_RX2_BAND_B_NCO1_FTW_BYTE4;
		break;
	case TAL_RX2_NCO2A:
		spiAddr = TALISE_ADDR_RX2_BAND_A_NCO2_FTW_BYTE4;
		break;
	case TAL_RX2_NCO2B:
		spiAddr = TALISE_ADDR_RX2_BAND_B_NCO2_FTW_BYTE4;
		break;
	default:
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_RXNCOFTW_INVNCO, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write to FTW - NCO 1*/
	halError = talSpiWriteByte(device->devHalInfo, spiAddr,
				   (uint8_t)(ncoFtw >> 24));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, (spiAddr + 1),
				   (uint8_t)(ncoFtw >> 16));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, (spiAddr + 2),
				   (uint8_t)(ncoFtw >> 8));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, (spiAddr + 3),
				   (uint8_t)(ncoFtw));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return retVal;
}

talRecoveryActions_t talSetupNcoShifter(taliseDevice_t *device,
					taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	int32_t pfirFs_kHz = 0;
	int32_t outFs_kHz = 0;
	int32_t ddcOutFsDiv2_kHz = 0;
	uint32_t nco1FreqUnsigned_kHz = 0;
	uint64_t nco1Ftw = 0;
	uint32_t nco2FreqUnsigned_kHz = 0;
	uint64_t nco2Ftw = 0;
	int32_t inputCenterFreq_kHz = 0;
	int32_t nco1OutputCenterFreq_khz = 0;
	int32_t nco2OutputCenterFreq_khz = 0;
	int32_t bandWidthDiv2 = 0;
	uint8_t ddcHbEnabled = 0;
	int32_t ddcHbCorner_kHz = 0;
	int32_t pfirCorner_kHz = 0;
	uint8_t ncoUpdateFtw = 0;

	static const uint64_t POW_2_32 = ((uint64_t)1) << 32;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "talRecoveryActions_t()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	bandWidthDiv2 = init->rx.rxProfile.rxNcoShifterCfg.bandAInputBandWidth_kHz >> 1;

	inputCenterFreq_kHz =
		init->rx.rxProfile.rxNcoShifterCfg.bandAInputCenterFreq_kHz;
	nco1OutputCenterFreq_khz = inputCenterFreq_kHz +
				   init->rx.rxProfile.rxNcoShifterCfg.bandANco1Freq_kHz;
	nco2OutputCenterFreq_khz = nco1OutputCenterFreq_khz +
				   init->rx.rxProfile.rxNcoShifterCfg.bandANco2Freq_kHz;

	/* scale the pfirFs for the selected DDC mode */
	/* Fs at output of PFIR : Twice output rate due to decimation in DDC */
	if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2) ||
	    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz << 1;
		ddcHbEnabled = 1;

		/* DDC HB corner is at 0.4 * rx output rate.  0.4 = 2/5 */
		ddcHbCorner_kHz = (int32_t)(device->devStateInfo.rxOutputRate_kHz << 1) / 5;
	} else if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2) ||
		   (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz >> 1;
		ddcHbEnabled = 1;

		/* DDC HB corner is at 0.4 * (rx output rate/2).  0.4 = 2/5 */
		ddcHbCorner_kHz = (int32_t)(device->devStateInfo.rxOutputRate_kHz) / 5;
	} else if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY) ||
		   (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz;
		ddcHbEnabled = 1;

		/* DDC HB corner is at 0.4 * (rx output rate/2).  0.4 = 2/5 */
		ddcHbCorner_kHz = (int32_t)(device->devStateInfo.rxOutputRate_kHz) / 5;
	} else if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS) ||
		   (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz;
		ddcHbEnabled = 0;

		/* DDC HB corner is at the Pfir corner since HalfBand is disabled */
		ddcHbCorner_kHz = (int32_t)((init->rx.rxProfile.rfBandwidth_Hz / 1000) >> 1);;
	}

	outFs_kHz = device->devStateInfo.rxOutputRate_kHz;
	ddcOutFsDiv2_kHz = (outFs_kHz >> 1);

	pfirCorner_kHz = ((init->rx.rxProfile.rfBandwidth_Hz / 1000) >> 1);


	/* Range check that signal passband edges are within Rx PFIR corners */
	if (((inputCenterFreq_kHz + bandWidthDiv2) > pfirCorner_kHz) ||
	    ((inputCenterFreq_kHz - bandWidthDiv2) < -pfirCorner_kHz)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETUPNCOSHIFTER_INV_PFIR_CORNER, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* If DDC HB enabled, verify that input signal after possible NCO1 shift is in passband of DDC HB */
	if (ddcHbEnabled > 0) {
		if (((nco1OutputCenterFreq_khz + bandWidthDiv2) > ddcHbCorner_kHz) ||
		    ((nco1OutputCenterFreq_khz - bandWidthDiv2) < -ddcHbCorner_kHz)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETUPNCOSHIFTER_INV_DDCHB_CORNER, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Range check NCO2 shifted output is within -Fs/2 to Fs/2 of Rx output rate */
	if (((nco2OutputCenterFreq_khz + bandWidthDiv2) > ddcOutFsDiv2_kHz) ||
	    ((nco2OutputCenterFreq_khz - bandWidthDiv2) < -ddcOutFsDiv2_kHz)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETUPNCOSHIFTER_INV_NCO2SHIFT, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* if nco1 shift */
	if (init->rx.rxProfile.rxNcoShifterCfg.bandANco1Freq_kHz != 0) {
		/* Convert from -ve frequencies to their positive equivalents */
		nco1FreqUnsigned_kHz = (init->rx.rxProfile.rxNcoShifterCfg.bandANco1Freq_kHz >=
					0)
				       ? init->rx.rxProfile.rxNcoShifterCfg.bandANco1Freq_kHz
				       : (pfirFs_kHz + init->rx.rxProfile.rxNcoShifterCfg.bandANco1Freq_kHz) ;

		/* Convert positive frequencies to their FTW equivalents */
		nco1Ftw = DIV_U64((((uint64_t)nco1FreqUnsigned_kHz * POW_2_32) << 1) +
				  pfirFs_kHz, pfirFs_kHz << 1); /* integer rounding */

		/* Check FTW is not more than 32 bits */
		if (nco1Ftw >= POW_2_32) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETUPNCOSHIFTER_FTW_OVRG, retVal, TALACT_ERR_CHECK_PARAM);
		}

		retVal = talSetRxNcoFtw(device, TAL_RX1_NCO1A, (uint32_t)nco1Ftw);
		IF_ERR_RETURN(retVal);

		retVal = talSetRxNcoFtw(device, TAL_RX2_NCO1A, (uint32_t)nco1Ftw);
		IF_ERR_RETURN(retVal);

		/* Update FTW words - write at bottom of function */
		ncoUpdateFtw |= 0x11;
	}

	/* if nco2 shift */
	if (init->rx.rxProfile.rxNcoShifterCfg.bandANco2Freq_kHz != 0) {
		/* Convert from -ve frequencies to their positive equivalents */

		nco2FreqUnsigned_kHz = (init->rx.rxProfile.rxNcoShifterCfg.bandANco2Freq_kHz >=
					0)
				       ? init->rx.rxProfile.rxNcoShifterCfg.bandANco2Freq_kHz
				       : (outFs_kHz + init->rx.rxProfile.rxNcoShifterCfg.bandANco2Freq_kHz);

		/* Convert positive frequencies to their FTW equivalents */
		nco2Ftw = DIV_U64((((uint64_t)nco2FreqUnsigned_kHz * POW_2_32) << 1) +
				  outFs_kHz, outFs_kHz << 1); /* integer rounding */

		/* Check FTW is not more than 32 bits */
		if (nco2Ftw >= POW_2_32) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETUPNCOSHIFTER_FTW_OVRG, retVal, TALACT_ERR_CHECK_PARAM);
		}

		retVal = talSetRxNcoFtw(device, TAL_RX1_NCO2A, (uint32_t)nco2Ftw);
		IF_ERR_RETURN(retVal);

		retVal = talSetRxNcoFtw(device, TAL_RX2_NCO2A, (uint32_t)nco2Ftw);
		IF_ERR_RETURN(retVal);

		/* Update NCO2 FTW words */
		ncoUpdateFtw |= 0x44;
	}

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RX_NCO_FTW_UPDATE_CONTROL, ncoUpdateFtw);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return retVal;
}

uint32_t TALISE_programDualBandLnaGainTable( taliseDevice_t *device,
		taliseDualBandLnaGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
		taliseRxChannels_t rxChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t registerValue = 0;
	uint8_t gtConfigReg = 0;
	uint8_t count = 0;

	static const uint8_t MAX_LNA_GAIN_TABLE_ENTRIES = 4;
	static const uint8_t EN_TABLECLK = 0x05;
	static const uint8_t EN_WRSTB = 0x02;

	if ((rxChannel == TAL_RX1) ||
	    (rxChannel == TAL_RX2) ||
	    (rxChannel == TAL_RX1RX2)) {
		/* Verify Rx profile is valid before allowing to set the Rx gain table, this
		 * ensures the device->rx structure pointers are valid
		 */
		if ((device->devStateInfo.profilesValid  & RX_PROFILE_VALID) == 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DUALBAND_LNA_TABLE_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Check for proper mapping for gain table and passed table number of indexes */
		if ((numGainIndexesInTable > MAX_LNA_GAIN_TABLE_ENTRIES) ||
		    (numGainIndexesInTable == 0)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_DUALBAND_LNA_TABLE_INV_INDEX, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DUALBAND_LNA_TABLE_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (gainTablePtr == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DUALBAND_LNA_TABLE_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* starting the gain table clock and read from gain table address bits */
	gtConfigReg = ((uint8_t)rxChannel << 3) | EN_TABLECLK;
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GAIN_TABLE_CONFIGURATION, gtConfigReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* The write strobe to gain table configuration register */
	gtConfigReg |= EN_WRSTB;

	/* Load gain table */
	for (count = 0; count < numGainIndexesInTable; count ++) {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_GAIN_TABLE_WORD_ADDRESS, count);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);

		registerValue = ((gainTablePtr[count].dualbandGain & 0x3F) << 2) |
				(gainTablePtr[count].dualbandControl & 0x03);
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_GAIN_TABLE_WORD_DATA_DUALBAND, registerValue);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_GAIN_TABLE_CONFIGURATION, gtConfigReg);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);
	}

	/* disabling the gain table clock */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GAIN_TABLE_CONFIGURATION, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set Max LNA indices in Rx1 AGC to match the number of gain table indices */
	if ((rxChannel == TAL_RX1) ||
	    (rxChannel == TAL_RX1RX2)) {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX1_SLOWLOOP_CONFIG,
					   (uint8_t)(numGainIndexesInTable-1));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);
	}

	/* Set Max LNA indices in Rx2 AGC to match the number of gain table indices */
	if ((rxChannel == TAL_RX2) ||
	    (rxChannel == TAL_RX1RX2)) {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX2_SLOWLOOP_CONFIG,
					   (uint8_t)(numGainIndexesInTable-1));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setGainTableExtCtrlPins(taliseDevice_t *device,
					taliseRxChannels_t rxChannel, uint8_t enable3p3vGpios)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t curr3p3SrcCtrl = 0x00;
	uint16_t currGpio3p3Oe = 0;
	uint16_t usedGpio3p3pins = 0;
	uint16_t gpio3p3FreeMask = 0;
	uint16_t gpio3p3UsedMask = 0;
	static const uint8_t GPIO3P3_RX1_EXT_CTRL_WORD_EN = 0x04;
	static const uint8_t GPIO3P3_RX2_EXT_CTRL_WORD_EN = 0x40;
	static const uint8_t GPIO3P3_RX1_EXT_CTRL_WORD_OE = 0x0F;
	static const uint8_t GPIO3P3_RX2_EXT_CTRL_WORD_OE = 0xF0;
	static const uint8_t GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK = 0x0F;
	static const uint8_t GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK = 0xF0;
	static const uint8_t ENABLE_EXT_CTRL_WORD_GPIOS = 0x01;
	static const uint32_t SWTEST_SIMULATE_RX1_GPIO3P3_INP = 0x01;
	static const uint32_t SWTEST_SIMULATE_RX2_GPIO3P3_INP = 0x02;

	/*Ensure that the requested channel is valid*/
	if ((rxChannel != TAL_RX1) && (rxChannel != TAL_RX2)
	    && (rxChannel != TAL_RX1RX2)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETEXTWORDCTRLGPIO_INV_CHANNEL,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Ensure that if channel Rx1 is requested, it is initialized*/
	if (((((uint8_t)rxChannel & (uint8_t)TAL_RX1) == (uint8_t)TAL_RX1) &&
	     ((device->devStateInfo.initializedChannels & (uint8_t)TAL_RX1) !=
	      (uint8_t)TAL_RX1))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETEXTWORDCTRLGPIO_UNINITIALIZED_RX1,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Ensure that if channel Rx2 is requested, it is initialized*/
	if (((((uint8_t)rxChannel & (uint8_t)TAL_RX2) == (uint8_t)TAL_RX2) &&
	     ((device->devStateInfo.initializedChannels & (uint8_t)TAL_RX2) !=
	      (uint8_t)TAL_RX2))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETEXTWORDCTRLGPIO_UNINITIALIZED_RX2,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Verify that the GPIO pins requested are not already in use*/

	/* Read current configuration of GPIO 3.3s */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, &curr3p3SrcCtrl);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	retVal = (talRecoveryActions_t)TALISE_getGpio3v3Oe(device, &currGpio3p3Oe);
	IF_ERR_RETURN(retVal);

	usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins;

	if(device->devStateInfo.swTest == SWTEST_SIMULATE_RX1_GPIO3P3_INP) {
		curr3p3SrcCtrl = 0;
		usedGpio3p3pins |= 0x01;
	}

	if(device->devStateInfo.swTest == SWTEST_SIMULATE_RX2_GPIO3P3_INP) {
		curr3p3SrcCtrl = 0;
		usedGpio3p3pins |= (0x01 << 4);
	}

	/*Check that if
	 * Channel requested for Ext Ctrl Word GPIO output is Rx1
	 * GPIO 3p3 [3:0] pins are already in use
	 * GPIO 3p3 [3:0] pins are not used by this feature
	 * then throw an exception
	 * */
	if ((((uint8_t)rxChannel & (uint8_t)TAL_RX1) == (uint8_t)TAL_RX1) &&
	    ((currGpio3p3Oe & GPIO3P3_RX1_EXT_CTRL_WORD_OE) == GPIO3P3_RX1_EXT_CTRL_WORD_OE)
	    &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX1_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX1,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((((uint8_t)rxChannel & (uint8_t)TAL_RX1) == (uint8_t)TAL_RX1) &&
	    ((usedGpio3p3pins & GPIO3P3_RX1_EXT_CTRL_WORD_OE) != 0) &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX1_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX1,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Check that if
	 * Channel requested for Ext Ctrl Word GPIO output is Rx2
	 * GPIO 3p3 [7:4] pins are already in use
	 * GPIO 3p3 [7:4] pins are not used by this feature
	 * then throw an exception
	 * */
	if ((((uint8_t)rxChannel & (uint8_t)TAL_RX2) == (uint8_t)TAL_RX2) &&
	    ((currGpio3p3Oe & GPIO3P3_RX2_EXT_CTRL_WORD_OE) == GPIO3P3_RX2_EXT_CTRL_WORD_OE)
	    &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX2_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX2,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((((uint8_t)rxChannel & (uint8_t)TAL_RX2) == (uint8_t)TAL_RX2) &&
	    ((usedGpio3p3pins & GPIO3P3_RX2_EXT_CTRL_WORD_OE) != 0) &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX2_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX2,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Clear current GPIO configuration and setup again if enable requested*/
	if ((rxChannel == TAL_RX1) || (rxChannel == TAL_RX1RX2)) {
		/* Free GPIO3P3[3:0] if used */
		gpio3p3FreeMask |= GPIO3P3_RX1_EXT_CTRL_WORD_OE;
		curr3p3SrcCtrl &= ~GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK;

		if (enable3p3vGpios >= ENABLE_EXT_CTRL_WORD_GPIOS) {
			/* Enable GPIO3P3[3:0] if requested */
			gpio3p3UsedMask |= GPIO3P3_RX1_EXT_CTRL_WORD_OE;
			curr3p3SrcCtrl |= GPIO3P3_RX1_EXT_CTRL_WORD_EN;
		}
	}

	if ((rxChannel == TAL_RX2) || (rxChannel == TAL_RX1RX2)) {
		/* Free GPIO3P3[7:4] if used */
		gpio3p3FreeMask |= GPIO3P3_RX2_EXT_CTRL_WORD_OE;
		curr3p3SrcCtrl &= ~GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK;

		if (enable3p3vGpios >= ENABLE_EXT_CTRL_WORD_GPIOS) {
			/* Enable GPIO3P3[7:4] if requested */
			gpio3p3UsedMask |= GPIO3P3_RX2_EXT_CTRL_WORD_OE;
			curr3p3SrcCtrl |= GPIO3P3_RX2_EXT_CTRL_WORD_EN;
		}
	}

	usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins & ~gpio3p3FreeMask;
	usedGpio3p3pins |= gpio3p3UsedMask;

	/*Update the Source Control Register*/
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, curr3p3SrcCtrl);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO3p3's to 0 (input) */
	/* - Set OE for all used GPIO3p3's to 1 (output) */
	retVal = (talRecoveryActions_t)TALISE_setGpio3v3Oe(device, gpio3p3UsedMask,
			(gpio3p3UsedMask | gpio3p3FreeMask));
	IF_ERR_RETURN(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpio3p3pins = usedGpio3p3pins;

	return (uint32_t)retVal;
}

uint32_t TALISE_getRxDecPower(taliseDevice_t *device,
			      taliseRxChannels_t rxChannel, uint16_t *rxDecPower_mdBFS)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t decPowerAddr = 0;
	uint8_t rxDecPower_dBFS = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRxDecPower()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* null pointer check */
	if (rxDecPower_mdBFS == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_RX_DEC_POWER_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxChannel == TAL_RX1) {
		decPowerAddr = TALISE_ADDR_DEC_POWER_CH1;
	} else if (rxChannel == TAL_RX2) {
		decPowerAddr = TALISE_ADDR_DEC_POWER_CH2;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXDECPOWER_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check that rx profile is valid in current config */
	if (((device->devStateInfo.profilesValid & RX_PROFILE_VALID) == 0)
	    || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXDECPOWER_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write some value to generate a strobe to latch the value */
	halError = talSpiWriteByte(device->devHalInfo, decPowerAddr, 0x01);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Read Rx1 Dec Power register */
	halError = talSpiReadByte(device->devHalInfo, decPowerAddr, &rxDecPower_dBFS);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*rxDecPower_mdBFS = ((uint16_t)rxDecPower_dBFS) * 250; /* 250 = 1000 * 0.25dB */

	return (uint32_t)retVal;
}
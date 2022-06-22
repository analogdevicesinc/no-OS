// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_tx.c
 * \brief Contains functions to support Talise Tx data path control
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_tx.h"
#include "talise_reg_addr_macros.h"
#include "talise_arm_macros.h"
#include "talise_hal.h"
#include "talise_user.h"
#include "talise_error.h"
#include "talise.h"
#include "talise_gpio.h"

uint32_t TALISE_setTxAttenuation(taliseDevice_t *device,
				 taliseTxChannels_t txChannel, uint16_t txAttenuation_mdB)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t regData = 0;

	static const uint32_t txMax = 41951;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setTxAttenuation()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check for valid txChannel */
	if ((txChannel != TAL_TX1) &&
	    (txChannel != TAL_TX2)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXATTEN_INV_TXCHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* check input parameter is in valid range */
	if (txAttenuation_mdB >= txMax) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXATTEN_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* set step size depending on tx->txAttenStepSize */
	switch (device->devStateInfo.txAttenStepSize) {
	case TAL_TXATTEN_0P05_DB:
		regData = (txAttenuation_mdB / 50);
		break;
	case TAL_TXATTEN_0P1_DB:
		regData = (txAttenuation_mdB / 100);
		break;
	case TAL_TXATTEN_0P2_DB:
		regData = (txAttenuation_mdB / 200);
		break;
	case TAL_TXATTEN_0P4_DB:
		regData = (txAttenuation_mdB / 400);
		break;
	default: {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXATTEN_INV_STEPSIZE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	if (txChannel == TAL_TX1) {
		/* Write MSB bits followed by LSB bits, TxAtten updates when [7:0] written */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_ATTENUATION_1,
					   ((regData >> 8) & 0x03));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_ATTENUATION_0,
					   (regData & 0xFF));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (txChannel == TAL_TX2) {
		/* Write MSB bits followed by LSB bits, TxAtten updates when [7:0] written */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_ATTENUATION_1,
					   ((regData >> 8) & 0x03));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_ATTENUATION_0,
					   (regData & 0xFF));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getTxAttenuation(taliseDevice_t *device,
				 taliseTxChannels_t txChannel, uint16_t *txAttenuation_mdB)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t msbData = 0;
	uint8_t lsbData = 0;

	static const uint16_t attenStepSizeDiv = 50;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getTxAttenuation()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* check txAttenuation_mdB for null pointer */
	if (txAttenuation_mdB == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXATTEN_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for a valid txChannel */
	if ((txChannel != TAL_TX1) &&
	    (txChannel != TAL_TX2)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXATTEN_INV_TXCHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		if (txChannel == TAL_TX1) {
			/* dummy write to Tx1 LSB register to 'activate' read */
			halError = talSpiWriteByte(device->devHalInfo,
						   TALISE_ADDR_TX1_ATTENUATION_0_READBACK, 0x00);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			/* reading MSB bits for Tx1 */
			halError = talSpiReadByte(device->devHalInfo,
						  TALISE_ADDR_TX1_ATTENUATION_1_READBACK, &msbData);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			/* reading LSB bits for Tx1 */
			halError = talSpiReadByte(device->devHalInfo,
						  TALISE_ADDR_TX1_ATTENUATION_0_READBACK, &lsbData);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}

		if (txChannel == TAL_TX2) {
			/* dummy write to Tx2 LSB register to 'activate' read */
			halError = talSpiWriteByte(device->devHalInfo,
						   TALISE_ADDR_TX2_ATTENUATION_0_READBACK, 0x00);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			/* reading MSB bits for Tx2 */
			halError = talSpiReadByte(device->devHalInfo,
						  TALISE_ADDR_TX2_ATTENUATION_1_READBACK, &msbData);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			/* reading LSB bits for Tx2 */
			halError = talSpiReadByte(device->devHalInfo,
						  TALISE_ADDR_TX2_ATTENUATION_0_READBACK, &lsbData);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* Readback word always reads back with 0.05dB resolution */
	*txAttenuation_mdB = (((uint16_t)(lsbData) | ((uint16_t)(
				       msbData & 0x03) << 8)) * attenStepSizeDiv);

	return (uint32_t)retVal;
}

uint32_t TALISE_setDacFullScale(taliseDevice_t *device,
				taliseDacFullScale_t dacFullScale)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t dacFsBoost = 0;
	uint8_t isArmRunning = 0;
	static const uint8_t TXDAC_GAIN_MASK = 0x1F;

	if ((dacFullScale != TAL_DACFS_0DB) &&
	    (dacFullScale != TAL_DACFS_3DB)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_DAC_FULLSCALE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_ARM_CTL_1,
				   &isArmRunning, 0x01, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (isArmRunning == 1) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_DAC_FULLSCALE_INVARMSTATE, retVal, TALACT_ERR_RESET_FULL);
	}

	dacFsBoost = ((uint8_t)dacFullScale) & TXDAC_GAIN_MASK;

	/* Tx1 I data */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TXDAC1_GAIN_I,
				   dacFsBoost );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx1 Q data */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TXDAC1_GAIN_Q,
				   dacFsBoost );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx2 I data */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TXDAC2_GAIN_I,
				   dacFsBoost );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx2 Q data */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TXDAC2_GAIN_Q,
				   dacFsBoost );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_txNcoShifterSet(taliseDevice_t *device,
				taliseTxNcoShifterCfg_t *txNcoShiftCfg)
{
	long long exponent32bit = 2147483648UL;
	int32_t exponent16bit = 65535;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint32_t txInputRateDiv2_kHz = 0;
	uint16_t tx1PhaseWord = 0;
	uint16_t tx2PhaseWord = 0;
	uint16_t tx1NcoTuneWord = 0;
	uint16_t tx2NcoTuneWord = 0;
	int32_t ch1TuneFrequency = 0;
	int32_t ch2TuneFrequency = 0;
	int32_t absValTx1Freq = 0;
	int32_t absValTx2Freq = 0;
	uint8_t ncoEnableMask =  (uint8_t )txNcoShiftCfg->enableNCO;


#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_txNcoShifterSet()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check valid Tx profile */
	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ENABLETXNCO_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for NULL pointer */
	if (txNcoShiftCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_TXNCOSHIFTER_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	txInputRateDiv2_kHz = device->devStateInfo.txInputRate_kHz >> 1;
	if ((txNcoShiftCfg->tx1ToneFreq_kHz > (int32_t)txInputRateDiv2_kHz) ||
	    (txNcoShiftCfg->tx1ToneFreq_kHz < -((int32_t)txInputRateDiv2_kHz))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_TXNCOSHIFTER_INV_TX1_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Tx2 NCO freq range between Fs/2 and -Fs/2 */
	if ((txNcoShiftCfg->tx2ToneFreq_kHz > (int32_t)txInputRateDiv2_kHz) ||
	    (txNcoShiftCfg->tx2ToneFreq_kHz < -((int32_t)txInputRateDiv2_kHz))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_TXNCOSHIFTER_INV_TX2_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Force Tx output power to max analog output power, but 6dB digital
	    * back off to prevent the NCO from clipping the Tx PFIR filter */
	/* Tx1 */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_GAIN_0, 0x78 );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_GAIN_1, 0x00 );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_GAIN_2, 0x00 );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx2 */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_GAIN_0, 0x78 );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_GAIN_1, 0x00 );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_GAIN_2, 0x00 );
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable TPC for Tx1 and Tx2 to Direct mode */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_TPC_CONFIG, 0x0A,
				    0x0F, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Calculate Tx NCO tuning words. Round the result */
	tx1NcoTuneWord = (int32_t)((DIV_S64(((int64_t)txNcoShiftCfg->tx1ToneFreq_kHz <<
					     20), txInputRateDiv2_kHz) + 1) >> 1);
	tx2NcoTuneWord = (int32_t)((DIV_S64(((int64_t)txNcoShiftCfg->tx2ToneFreq_kHz <<
					     20), txInputRateDiv2_kHz) + 1) >> 1);

	/* Write NCO tuning words - Tx1 */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_I_UPPER_NIBBLE,
				   (uint8_t)((tx1NcoTuneWord >> 16) & 0x0F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_I_MSBS,
				   (uint8_t)((tx1NcoTuneWord >> 8) & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_I_LSBS, (uint8_t)(tx1NcoTuneWord & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Write NCO tuning words - Tx2 */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_Q_UPPER_NIBBLE,
				   (uint8_t)((tx2NcoTuneWord >> 16) & 0x0F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_Q_MSBS,
				   (uint8_t)((tx2NcoTuneWord >> 8) & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_Q_LSBS, (uint8_t)(tx2NcoTuneWord & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Calculate the phase values to be wrote to registers based on formula */
	/* value = round(desiredPhaseShift*(2^16-1)/360) */
	tx1PhaseWord = (u16) DIV_ROUND_CLOSEST(txNcoShiftCfg->tx1TonePhaseDeg * ((
			exponent16bit) - 1), 360);
	tx2PhaseWord = (u16) DIV_ROUND_CLOSEST(txNcoShiftCfg->tx2TonePhaseDeg * ((
			exponent16bit) - 1), 360);

	/* write nco phase words */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH1_PHASE_OFFSET_BYTE_1, ((uint8_t)((tx1PhaseWord) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH1_PHASE_OFFSET_BYTE_2,
				   ((uint8_t)((tx1PhaseWord >> 8) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH2_PHASE_OFFSET_BYTE_1, ((uint8_t)((tx2PhaseWord) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH2_PHASE_OFFSET_BYTE_2,
				   ((uint8_t)((tx2PhaseWord >> 8) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Calculate absolute value of frequencies manually */
	absValTx1Freq = txNcoShiftCfg->tx1ToneFreq_kHz * ((
				txNcoShiftCfg->tx1ToneFreq_kHz > 0) - (txNcoShiftCfg->tx1ToneFreq_kHz < 0));
	absValTx2Freq = txNcoShiftCfg->tx2ToneFreq_kHz * ((
				txNcoShiftCfg->tx2ToneFreq_kHz > 0) - (txNcoShiftCfg->tx2ToneFreq_kHz < 0));

	/* Calculate the frequency tuning values to be wrote to registers based on formula */
	/* value = round(2^31*(abs(desiredNCOFreq / 2)/(TxSampleRate/2))); if (desiredNCOFreq<0) then add 2^31  */
	ch1TuneFrequency = (uint32_t)DIV_ROUND_CLOSEST_ULL(exponent32bit *
			   absValTx1Freq, txInputRateDiv2_kHz / 2);
	ch2TuneFrequency = (uint32_t)DIV_ROUND_CLOSEST_ULL(exponent32bit *
			   absValTx2Freq, txInputRateDiv2_kHz / 2);

	/* 2's complement to handle negative frequencies */
	if (txNcoShiftCfg->tx1ToneFreq_kHz > 0) {
		ch1TuneFrequency = ~ch1TuneFrequency + 1;
	}

	if (txNcoShiftCfg->tx2ToneFreq_kHz > 0) {
		ch2TuneFrequency = ~ch2TuneFrequency + 1;
	}

	/* check for enabled/disabled and if so zero'ing out data so it resets to default signal */
	if (txNcoShiftCfg->enableNCO == TAL_TXNCO_DISABLE) {
		ch1TuneFrequency = 0x0;
		ch2TuneFrequency = 0x0;
		/* set the mask to update all channels back to 0 value */
		ncoEnableMask = (uint8_t)TAL_TXNCO_ENABLE_ALL;
	}

	/* write nco frequency tuning values */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH1_FREQ_TUNE_BYTE_1,
				   ((uint8_t)((ch1TuneFrequency) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH1_FREQ_TUNE_BYTE_2,
				   ((uint8_t)((ch1TuneFrequency >> 8) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH1_FREQ_TUNE_BYTE_3,
				   ((uint8_t)((ch1TuneFrequency >> 16) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH1_FREQ_TUNE_BYTE_4,
				   ((uint8_t)((ch1TuneFrequency >> 24) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH2_FREQ_TUNE_BYTE_1,
				   ((uint8_t)((ch2TuneFrequency) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH2_FREQ_TUNE_BYTE_2,
				   ((uint8_t)((ch2TuneFrequency >> 8) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH2_FREQ_TUNE_BYTE_3,
				   ((uint8_t)((ch2TuneFrequency >> 16) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_NCO_CH2_FREQ_TUNE_BYTE_4,
				   ((uint8_t)((ch2TuneFrequency >> 24) & 0xFF)));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set nco frequency update ch2 */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX_NCO_FREQ_UPDATE,
				   ncoEnableMask);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_NCO_CH1_CTRL,
				    0x1, 0x01, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_NCO_CH2_CTRL,
				    0x1, 0x01, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_enableTxNco(taliseDevice_t *device,
			    taliseTxNcoTestToneCfg_t *txNcoTestToneCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	int32_t tx1NcoTuneWord = 0;
	int32_t tx2NcoTuneWord = 0;
	uint32_t txInputRateDiv2_kHz = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableTxNco()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check valid Tx profile */
	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ENABLETXNCO_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for NULL pointer */
	if ( txNcoTestToneCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ENABLETXNCO_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (txNcoTestToneCfg->enable > 0) {
		txInputRateDiv2_kHz = device->devStateInfo.txInputRate_kHz >> 1;
		/* Check Tx1 NCO freq range between Fs/2 and -Fs/2 */
		if ((txNcoTestToneCfg->tx1ToneFreq_kHz > (int32_t)txInputRateDiv2_kHz) ||
		    (txNcoTestToneCfg->tx1ToneFreq_kHz < -((int32_t)txInputRateDiv2_kHz))) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_ENABLETXNCO_INV_TX1_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Check Tx2 NCO freq range between Fs/2 and -Fs/2 */
		if ((txNcoTestToneCfg->tx2ToneFreq_kHz > (int32_t)txInputRateDiv2_kHz) ||
		    (txNcoTestToneCfg->tx2ToneFreq_kHz < -((int32_t)txInputRateDiv2_kHz))) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_ENABLETXNCO_INV_TX2_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Force Tx output power to max analog output power, but 6dB digital
		 * back off to prevent the NCO from clipping the Tx PFIR filter */
		/* Tx1 */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_GAIN_0, 0x78 );
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_GAIN_1, 0x00 );
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_GAIN_2, 0x00 );
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Tx2 */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_GAIN_0, 0x78 );
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_GAIN_1, 0x00 );
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_GAIN_2, 0x00 );
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Enable TPC for Tx1 and Tx2 to Direct mode */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_TPC_CONFIG, 0x0A,
					    0x0F, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Calculate Tx NCO tuning words. Round the result */
		tx1NcoTuneWord = (int32_t)((DIV_S64(((int64_t)txNcoTestToneCfg->tx1ToneFreq_kHz
						     << 20), txInputRateDiv2_kHz) + 1) >> 1);
		tx2NcoTuneWord = (int32_t)((DIV_S64(((int64_t)txNcoTestToneCfg->tx2ToneFreq_kHz
						     << 20), txInputRateDiv2_kHz) + 1) >> 1);

		/* Write NCO tuning words - Tx1 */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_I_UPPER_NIBBLE,
					   (uint8_t)((tx1NcoTuneWord >> 16) & 0x0F));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_I_MSBS,
					   (uint8_t)((tx1NcoTuneWord >> 8) & 0xFF));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_I_LSBS, (uint8_t)(tx1NcoTuneWord & 0xFF));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Write NCO tuning words - Tx2 */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_Q_UPPER_NIBBLE,
					   (uint8_t)((tx2NcoTuneWord >> 16) & 0x0F));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_Q_MSBS,
					   (uint8_t)((tx2NcoTuneWord >> 8) & 0xFF));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_TX_ABBF_FREQCAL_NCO_Q_LSBS, (uint8_t)(tx2NcoTuneWord & 0xFF));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Enable NCO test mode */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DIGITAL_TEST_BYTE,
					    0x01, 0x02, 1);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		/* Disable NCO test mode */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DIGITAL_TEST_BYTE,
					    0x00, 0x02, 1);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Enable TPC for Tx1 and Tx2 to SPI mode */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_TPC_CONFIG, 0x05,
					    0x0F, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

static talRecoveryActions_t talSetTx1AttenCtrlPin(taliseDevice_t *device,
		uint8_t enable, taliseGpioPinSel_t txAttenIncPin,
		taliseGpioPinSel_t txAttenDecPin)
{
	static const uint8_t TAL_TX1_INC_ATTEN_GPIO4   = 0x00;
	static const uint8_t TAL_TX1_INC_ATTEN_GPIO12  = 0x01;
	static const uint8_t TAL_TX1_INC_ATTEN_DISABLE = 0x03;
	static const uint8_t TAL_TX1_DEC_ATTEN_GPIO5   = 0x00;
	static const uint8_t TAL_TX1_DEC_ATTEN_GPIO13  = 0x01;
	static const uint8_t TAL_TX1_DEC_ATTEN_DISABLE = 0x03;

	uint8_t registerValue = 0;
	uint8_t tx1IncAttenGpioSel = TAL_TX1_INC_ATTEN_DISABLE;
	uint8_t tx1DecAttenGpioSel = TAL_TX1_DEC_ATTEN_DISABLE;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint32_t usedGpiopins = 0;
	uint32_t gpioFreeMask = 0;
	uint32_t gpioUsedMask = 0;

	/* Range Check */
	if (enable > 0) {
		if ((txAttenIncPin != TAL_GPIO_04) &&
		    (txAttenIncPin != TAL_GPIO_12)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETTXATTENCTRLPIN_INV_TX1_INC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((txAttenDecPin != TAL_GPIO_05) &&
		    (txAttenDecPin != TAL_GPIO_13)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETTXATTENCTRLPIN_INV_TX1_DEC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Free all used GPIO's based on current configuration*/
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_TX_TPC_GPIO_CONFIG,
				  &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);
	tx1IncAttenGpioSel = (registerValue & 0x03);
	tx1DecAttenGpioSel = ((registerValue >> 2) & 0x03);


	if (tx1IncAttenGpioSel == TAL_TX1_INC_ATTEN_GPIO4) {
		gpioFreeMask |= (1 << TAL_GPIO_04);
	} else if (tx1IncAttenGpioSel == TAL_TX1_INC_ATTEN_GPIO12) {
		gpioFreeMask |= (1 << TAL_GPIO_12);
	}

	if (tx1DecAttenGpioSel == TAL_TX1_DEC_ATTEN_GPIO5) {
		gpioFreeMask |= (1 << TAL_GPIO_05);

	} else if (tx1DecAttenGpioSel == TAL_TX1_DEC_ATTEN_GPIO13) {
		gpioFreeMask |= (1 << TAL_GPIO_13);
	}

	usedGpiopins = device->devStateInfo.usedGpiopins & ~gpioFreeMask;

	/* check if GPIO's are available for the new configuration*/
	if (enable > 0) {
		gpioUsedMask |= (1 << txAttenIncPin);
		gpioUsedMask |= (1 << txAttenDecPin);

		if ((usedGpiopins & gpioUsedMask) != 0) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETTXATTENCTRLPIN_TX1_GPIO_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpiopins |= gpioUsedMask;
		tx1IncAttenGpioSel = (txAttenIncPin == TAL_GPIO_04) ? TAL_TX1_INC_ATTEN_GPIO4 :
				     TAL_TX1_INC_ATTEN_GPIO12;
		tx1DecAttenGpioSel = (txAttenDecPin == TAL_GPIO_05) ? TAL_TX1_DEC_ATTEN_GPIO5 :
				     TAL_TX1_DEC_ATTEN_GPIO13;
	} else {
		tx1IncAttenGpioSel = TAL_TX1_INC_ATTEN_DISABLE;
		tx1DecAttenGpioSel = TAL_TX1_DEC_ATTEN_DISABLE;
	}

	/* Program Pin mode configuration */
	registerValue = ((registerValue & 0xF0) | (tx1DecAttenGpioSel << 2) |
			 tx1IncAttenGpioSel);
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX_TPC_GPIO_CONFIG,
				   registerValue);
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

static talRecoveryActions_t talSetTx2AttenCtrlPin(taliseDevice_t *device,
		uint8_t enable, taliseGpioPinSel_t txAttenIncPin,
		taliseGpioPinSel_t txAttenDecPin)
{
	static const uint8_t TAL_TX2_INC_ATTEN_GPIO6   = 0x00;
	static const uint8_t TAL_TX2_INC_ATTEN_GPIO14  = 0x01;
	static const uint8_t TAL_TX2_INC_ATTEN_DISABLE = 0x03;
	static const uint8_t TAL_TX2_DEC_ATTEN_GPIO7   = 0x00;
	static const uint8_t TAL_TX2_DEC_ATTEN_GPIO15  = 0x01;
	static const uint8_t TAL_TX2_DEC_ATTEN_DISABLE = 0x03;

	uint8_t registerValue = 0;
	uint8_t tx2IncAttenGpioSel = TAL_TX2_INC_ATTEN_DISABLE;
	uint8_t tx2DecAttenGpioSel = TAL_TX2_DEC_ATTEN_DISABLE;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint32_t usedGpiopins = 0;
	uint32_t gpioFreeMask = 0;
	uint32_t gpioUsedMask = 0;

	/* Range Check */
	if (enable > 0) {
		if ((txAttenIncPin != TAL_GPIO_06) &&
		    (txAttenIncPin != TAL_GPIO_14)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETTXATTENCTRLPIN_INV_TX2_INC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((txAttenDecPin != TAL_GPIO_07) &&
		    (txAttenDecPin != TAL_GPIO_15)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETTXATTENCTRLPIN_INV_TX2_DEC_PIN, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Free all used GPIO's based on current configuration*/
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_TX_TPC_GPIO_CONFIG,
				  &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);
	tx2IncAttenGpioSel = ((registerValue >> 4) & 0x03);
	tx2DecAttenGpioSel = ((registerValue >> 6) & 0x03);

	if (tx2IncAttenGpioSel == TAL_TX2_INC_ATTEN_GPIO6) {
		gpioFreeMask |= (1 << TAL_GPIO_06);

	} else if (tx2IncAttenGpioSel == TAL_TX2_INC_ATTEN_GPIO14) {
		gpioFreeMask |= (1 << TAL_GPIO_14);

	}

	if (tx2DecAttenGpioSel == TAL_TX2_DEC_ATTEN_GPIO7) {
		gpioFreeMask |= (1 << TAL_GPIO_07);
	} else if (tx2DecAttenGpioSel == TAL_TX2_DEC_ATTEN_GPIO15) {
		gpioFreeMask |= (1 << TAL_GPIO_15);

	}

	usedGpiopins = device->devStateInfo.usedGpiopins & ~gpioFreeMask;

	/* check if GPIO's are available for the new configuration*/
	if (enable > 0) {
		gpioUsedMask |= (1 << txAttenIncPin);
		gpioUsedMask |= (1 << txAttenDecPin);

		if ((usedGpiopins & gpioUsedMask) != 0) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_SETTXATTENCTRLPIN_TX2_GPIO_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpiopins |= gpioUsedMask;
		tx2IncAttenGpioSel = (txAttenIncPin == TAL_GPIO_06) ? TAL_TX2_INC_ATTEN_GPIO6 :
				     TAL_TX2_INC_ATTEN_GPIO14;
		tx2DecAttenGpioSel = (txAttenDecPin == TAL_GPIO_07) ? TAL_TX2_DEC_ATTEN_GPIO7 :
				     TAL_TX2_DEC_ATTEN_GPIO15;
	} else {
		tx2IncAttenGpioSel = TAL_TX2_INC_ATTEN_DISABLE;
		tx2DecAttenGpioSel = TAL_TX2_DEC_ATTEN_DISABLE;
	}

	/* Program Pin mode configuration */
	registerValue = ((registerValue & 0x0F) | (tx2DecAttenGpioSel << 6) |
			 (tx2IncAttenGpioSel << 4));
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX_TPC_GPIO_CONFIG,
				   registerValue);
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

uint32_t TALISE_setTxAttenCtrlPin(taliseDevice_t *device,
				  taliseTxChannels_t txChannel, taliseTxAttenCtrlPin_t *txAttenCtrlPin)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t tpcModeVal = 0;
	uint8_t tpcModeMask = 0;

	static const uint8_t TX_INCDEC_MASK = 0x1F;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setTxAttenCtrlPin()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/*Check passed pointers for NULL */
	if (txAttenCtrlPin == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXATTENCTRLPIN_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check out of range */
	if ( txAttenCtrlPin->stepSize > TX_INCDEC_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXATTENCTRLPIN_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check channel, GPIO's */
	switch (txChannel) {
	case TAL_TX1:
		retVal = talSetTx1AttenCtrlPin(device, txAttenCtrlPin->enable,
					       txAttenCtrlPin->txAttenIncPin, txAttenCtrlPin->txAttenDecPin);
		IF_ERR_RETURN_U32(retVal);
		tpcModeVal = (txAttenCtrlPin->enable == 0) ? 0x01 : 0x03;
		tpcModeMask = 0x03;
		break;

	case TAL_TX2:
		retVal = talSetTx2AttenCtrlPin(device, txAttenCtrlPin->enable,
					       txAttenCtrlPin->txAttenIncPin, txAttenCtrlPin->txAttenDecPin);
		IF_ERR_RETURN_U32(retVal);
		tpcModeVal = (txAttenCtrlPin->enable == 0) ? 0x04 : 0x0C;
		tpcModeMask = 0x0C;
		break;

	default: {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXATTENCTRLPIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	/* Write attenuation step */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX_INCR_DECR_WORD,
				   txAttenCtrlPin->stepSize);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Write Enable or disable */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_TPC_CONFIG,
				    tpcModeVal, tpcModeMask, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getTxAttenCtrlPin(taliseDevice_t *device,
				  taliseTxChannels_t txChannel, taliseTxAttenCtrlPin_t *txAttenCtrlPin)
{
	uint8_t registerValue = 0;
	uint8_t fieldValue = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getTxAttenCtrlPin()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check channel */
	if ((txChannel != TAL_TX1) &&
	    (txChannel != TAL_TX2)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXATTENCTRLPIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Check passed pointers for NULL */
	if (txAttenCtrlPin == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXATTENCTRLPIN_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Get value of GPIO selects */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_TX_TPC_GPIO_CONFIG,
				  &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Value of txAttenIncPin */
	fieldValue = (txChannel == TAL_TX1) ? (registerValue & 0x03) : ((
				registerValue >> 4) & 0x03);
	txAttenCtrlPin->txAttenIncPin = (txChannel == TAL_TX1) ? ((
						fieldValue == 0x00) ? TAL_GPIO_04 : ((fieldValue == 0x01) ? TAL_GPIO_12 :
								TAL_GPIO_INVALID))
					: ((fieldValue == 0x00) ? TAL_GPIO_06 : ((fieldValue == 0x01) ? TAL_GPIO_14 :
							TAL_GPIO_INVALID));

	/* Value of txAttenDecPin */
	fieldValue = (txChannel == TAL_TX1) ? ((registerValue >> 2) & 0x03) : ((
				registerValue >> 6) & 0x03);
	txAttenCtrlPin->txAttenDecPin = (txChannel == TAL_TX1) ? ((
						fieldValue == 0x00) ? TAL_GPIO_05 : ((fieldValue == 0x01) ? TAL_GPIO_13 :
								TAL_GPIO_INVALID))
					: ((fieldValue == 0x00) ? TAL_GPIO_07 : ((fieldValue == 0x01) ? TAL_GPIO_15 :
							TAL_GPIO_INVALID));
	/*Attenuation step */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_TX_INCR_DECR_WORD,
				  &txAttenCtrlPin->stepSize);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/*Get value of Enable */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_TX_TPC_CONFIG,
				  &registerValue);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable or disable */
	fieldValue = (txChannel == TAL_TX1) ? (registerValue & 0x03) : ((
				registerValue >> 2) & 0x03);
	txAttenCtrlPin->enable = (fieldValue == 0x03) ? 1 : 0;

	return (uint32_t)retVal;
}

uint32_t TALISE_setPaProtectionCfg(taliseDevice_t *device,
				   taliseTxPaProtectCfg_t *txPaProtectCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t peakCount = 0;
	uint8_t tx1PeakThreshold = 0;
	uint8_t tx2PeakThreshold = 0;
	uint8_t paProtectCfg = 0;
	uint8_t peakCountMax = 31;

	static const uint8_t ENABLE_PA_PROTECTION_MEASUREMENT = 0x01;
	static const uint8_t ENABLE_PEAK_MEASUREMENT = 0x01;
	static const uint8_t ERROR_FLAGS_STICKY = 0x40;
	static const uint8_t TX_ATTEN_REDUCTION_MAX = 127;
	static const uint16_t POWER_THRESHOLD_MAX = 8191;
	static const uint8_t AVG_DURATION_MAX = 14;
	static const uint8_t SILICON_REV_C0 = 0xC0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setPaProtectionCfg()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* If Silicon Revision is less than C0 - Max Peak Count is 30 */
	if (device->devStateInfo.deviceSiRev < SILICON_REV_C0) {
		/*Update the max peak count for silicon revisions lower than C0 to 30*/
		peakCountMax = 30;
	}

	if (txPaProtectCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPRO_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->avgDuration > AVG_DURATION_MAX) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPRO_INV_AVGDURATION, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->peakCount > peakCountMax) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPROT_INV_PEAKCNT, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->txAttenStep > TX_ATTEN_REDUCTION_MAX) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPROT_INV_TXATTENSTEP, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if ((txPaProtectCfg->tx1PowerThreshold == 0) ||
	    (txPaProtectCfg->tx1PowerThreshold > POWER_THRESHOLD_MAX)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPROT_INV_TX1THRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if ((txPaProtectCfg->tx2PowerThreshold == 0) ||
	    (txPaProtectCfg->tx2PowerThreshold > POWER_THRESHOLD_MAX)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPROT_INV_TX2THRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->tx1PeakThreshold == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPROT_INV_TX1PEAKTHRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->tx2PeakThreshold == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETPAPROT_INV_TX2PEAKTHRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Disable PA Protection before changing settings paProtectCfg[0]=0 */
	/* Set Tx Power average duration paProtectCfg[4:1] */
	/* Allow error flags to be stick (set until cleared by SPI clear bit paProtectCfg[6]=1 */
	paProtectCfg = ((txPaProtectCfg->avgDuration << 1) | ERROR_FLAGS_STICKY);
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_CONFIGURATION, paProtectCfg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_PA_PROTECTION_ATTEN_CONTROL, txPaProtectCfg->txAttenStep, 0xFE, 1);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx1 average power threshold[7:0] */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_THRESHOLD_0,
				   (txPaProtectCfg->tx1PowerThreshold & 0xFF));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx1 average power threshold[12:8] */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_THRESHOLD_1,
				   ((txPaProtectCfg->tx1PowerThreshold >> 8) & 0xFF));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx2 average power threshold[7:0] */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_THRESHOLD_2,
				   (txPaProtectCfg->tx2PowerThreshold & 0xFF));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx2 average power threshold[12:8] */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_THRESHOLD_3,
				   ((txPaProtectCfg->tx2PowerThreshold >> 8) & 0xFF));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Peak mode settings */
	if (txPaProtectCfg->peakCount == 0) {
		peakCount = 0;
		tx1PeakThreshold = 0;
		tx2PeakThreshold = 0;
	} else {
		peakCount = ((txPaProtectCfg->peakCount << 1) | ENABLE_PEAK_MEASUREMENT);
		tx1PeakThreshold = txPaProtectCfg->tx1PeakThreshold;
		tx2PeakThreshold = txPaProtectCfg->tx2PeakThreshold;
	}

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH1, tx1PeakThreshold);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH2, tx2PeakThreshold);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set peak count and peak enable bit after peak thresholds */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_PEAK_COUNT, peakCount);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable PA Power Protection measurement */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_PA_PROTECTION_CONFIGURATION, ENABLE_PA_PROTECTION_MEASUREMENT, 0x01,
				    0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Clear PA Protection Error flags */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_PA_PROTECTION_CONFIGURATION, 1, 0x80, 7);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getPaProtectionCfg(taliseDevice_t *device,
				   taliseTxPaProtectCfg_t *txPaProtectCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t tx1PowerThresholdLsb = 0;
	uint8_t tx1PowerThresholdMsb = 0;
	uint8_t tx2PowerThresholdLsb = 0;
	uint8_t tx2PowerThresholdMsb = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getPaProtectionCfg()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (txPaProtectCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETPAPRO_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_CONFIGURATION, &txPaProtectCfg->avgDuration, 0x1E, 1);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_ATTEN_CONTROL, &txPaProtectCfg->txAttenStep, 0xFE, 1);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_PA_PROTECTION_THRESHOLD_0, &tx1PowerThresholdLsb);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_THRESHOLD_1, &tx1PowerThresholdMsb, 0x1F, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	txPaProtectCfg->tx1PowerThreshold = ((uint16_t)tx1PowerThresholdMsb << 8) |
					    (uint16_t)tx1PowerThresholdLsb;

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_PA_PROTECTION_THRESHOLD_2, &tx2PowerThresholdLsb);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_THRESHOLD_3, &tx2PowerThresholdMsb, 0x1F, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	txPaProtectCfg->tx2PowerThreshold = ((uint16_t)tx2PowerThresholdMsb << 8) |
					    (uint16_t)tx2PowerThresholdLsb;

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_PEAK_COUNT, &txPaProtectCfg->peakCount, 0x3E, 1);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH1,
				  &txPaProtectCfg->tx1PeakThreshold);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH2,
				  &txPaProtectCfg->tx2PeakThreshold);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_enablePaProtection(taliseDevice_t *device,
				   uint8_t enableTxAttenCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t enTxAttenCtrl = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enablePaProtection()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* TODO: Currently do not allow enabling this feature due to a hardware
	 * issue found.  Calling this function will not enable control of TxAttenuation when
	 * the PA Protection error flag asserts.  BBIC will need to take action to reduce
	 * Tx sample power.
	 */

	/* enTxAttenCtrl = (enableTxAttenCtrl > 0) ? 1 : 0; */
	enTxAttenCtrl = 0;

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_PA_PROTECTION_ATTEN_CONTROL, enTxAttenCtrl, 0x01, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getTxSamplePower(taliseDevice_t *device,
				 taliseTxChannels_t txChannel, uint16_t *channelPower)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t paProtectChannelSel = 0;
	uint8_t chPowerLsb = 0;
	uint8_t chPowerMsb = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getTxSamplePower()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (channelPower == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXSAMPLEPWR_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	switch(txChannel) {
	case TAL_TX1:
		paProtectChannelSel = 0;
		break;

	case TAL_TX2:
		paProtectChannelSel = 1;
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXSAMPLEPWR_INV_TXREADCHAN, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Set channel for power read back */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_PA_PROTECTION_CONFIGURATION, paProtectChannelSel, 0x20, 5);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Write power readback reg to force the value to update (write strobe) */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PA_PROTECTION_POWER_0, 0x00);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Read back DAC power for selected channel */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_PA_PROTECTION_POWER_0,
				  &chPowerLsb);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_PA_PROTECTION_POWER_1,
				  &chPowerMsb);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*channelPower = ((uint16_t)chPowerLsb | (uint16_t)(chPowerMsb << 8));

	return (uint32_t)retVal;
}

uint32_t TALISE_getPaProtectErrorFlags(taliseDevice_t *device,
				       uint8_t *errorFlags)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getPaProtectErrorFlags()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (errorFlags == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETPAERRFLAGS_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Read back PA Protection Error flags */
	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_PA_PROTECTION_ERROR,
				   errorFlags, 0x03, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_clearPaProtectErrorFlags(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_clearPaProtectErrorFlags()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Clear PA Protection Error flags */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_PA_PROTECTION_CONFIGURATION, 1, 0x80, 7);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

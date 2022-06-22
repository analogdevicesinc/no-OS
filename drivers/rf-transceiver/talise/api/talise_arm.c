// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_arm.c
 * \brief Contains functions to support interfacing with the TALISE internal
 *          ARM processor
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_arm.h"
#include "talise_radioctrl.h"
#include "talise_reg_addr_macros.h"
#include "talise_arm_macros.h"
#include "talise_hal.h"
#include "talise_user.h"
#include "talise_error.h"
#include "talise.h"

uint32_t TALISE_initArm(taliseDevice_t *device, taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t regClkSel = 0;
	uint8_t armClkSel =  0;
	uint32_t armClkRate_kHz = 0;
	uint32_t hsDigClkDiv4or5_Hz = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_initArm()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	hsDigClkDiv4or5_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz;

	/* dividing from Hz to kHz */
	armClkRate_kHz =  hsDigClkDiv4or5_Hz / 1000;

	/* the SPI read register and write register clocks must be equal or less than 250MHz */
	/* the ARM clock should not exceed 250 Mhz */
	if (armClkRate_kHz <= 250000) {
		regClkSel = 0x00;
		armClkSel = 0x00;
	} else if ((armClkRate_kHz > 250000) &&
		   (armClkRate_kHz <= 500000)) {
		regClkSel = 0x01;
		armClkSel = (0x01 << 1);
	} else if ((armClkRate_kHz > 500000) &&
		   (armClkRate_kHz <= 1000000)) {
		regClkSel = 0x02;
		armClkSel = (0x02 << 1);
	} else if ((armClkRate_kHz > 1000000) &&
		   (armClkRate_kHz <= 2000000)) {
		regClkSel = 0x03;
		armClkSel = (0x03 << 1);
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INITARM_INV_ARMCLK_PARAM,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* auto_incr[2] = 1 */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_ARM_DMA_CTL, 0x01,
				    0x02, 1);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* arm_debug_enable[7] = 1, mem_hresp_mask[3] = 1, arm_m3_run[0] = 0 */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_CTL_1, 0x88);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* setting the ARM clock rate, resetting the program counter, and enabling the ARM clock */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_CLOCK_CONTROL,
				   armClkSel |= 0x41);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* maintaining the ARM clock rate, disabling the program counter reset, and maintaining ARM clock enable */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_CLOCK_CONTROL,
				   armClkSel &= ~0x40);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* setting the SPI read and write clock rates with default reg clk cycles wait */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_BRIDGE_CLOCK_CONTROL,
				   regClkSel |= 0x40);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* blockout_window_size[4:1] = 9, ahb_spi_bridge_enable[0] = 1 */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AHB_SPI_BRIDGE,
				   0x13);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* write the ARM profiles */
	retVal = (talRecoveryActions_t)TALISE_writeArmProfile(device, init);
	IF_ERR_RETURN_U32(retVal);

	/* Load ADC profiles */
	retVal = (talRecoveryActions_t)TALISE_loadAdcProfiles(device,
			&init->rx.rxProfile.rxAdcProfile[0],
			&init->obsRx.orxProfile.orxLowPassAdcProfile[0],
			&init->obsRx.orxProfile.orxBandPassAdcProfile[0],
			&init->tx.txProfile.loopBackAdcProfile[0],
			&init->obsRx.orxProfile.orxMergeFilter[0]);
	return (uint32_t)retVal;
}

uint32_t TALISE_writeArmProfile(taliseDevice_t *device, taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	int32_t i = 0;
	int32_t j = 0;
	uint8_t hsDiv = 0;
	uint16_t channelsEnabled = 0;
	uint8_t cfgData[84] = {0};

	static const uint8_t length = 84;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_writeArmConfig()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* reading in required taliseDevice_t structure data into array - not pretty but it works */
	for (i = 0; i < 4; i++) {
		cfgData[i] = (uint8_t)(((init->clocks.deviceClock_kHz * 1000) >>
					(i * 8)) & 0x000000FF);
	}

	for (i = 4; i < 8; i++) {
		cfgData[i] = (uint8_t)(((init->clocks.clkPllVcoFreq_kHz) >> ((
						i - 4) * 8)) & 0x000000FF);
	}

	hsDiv = (uint8_t)init->clocks.clkPllHsDiv;

	cfgData[8] =
		2; /* hardcoded for 2 because it does not exist - this was the vcoDiv */
	cfgData[9] = hsDiv; /* uint8_t hs_div */

	channelsEnabled |= init->tx.txChannels & 0x03;
	channelsEnabled |= ((init->rx.rxChannels & 0x03) << 2);

	if (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) {
		channelsEnabled |= ((init->obsRx.obsRxChannelsEnable & 0x03) <<
				    4); /* enable both ORx1/ORx2 channels */
	}

	for (i = 10; i < 12; i++) {
		cfgData[i] = (uint8_t)((channelsEnabled >> ((i - 10) * 8)) & 0xFF);
	}

	cfgData[12] = ((init->clocks.rfPllUseExternalLo > 0) ? 1 :
		       0); /* [0] = Enable RF PLL to use external LO input */
	device->devStateInfo.clocks.rfPllUseExternalLo =
		init->clocks.rfPllUseExternalLo;

	cfgData[13] = (init->clocks.rfPllPhaseSyncMode & 0x03); /* RFPLL MCS control */
	if ((init->obsRx.orxProfile.rfBandwidth_Hz > 200000000) &&
	    (device->devStateInfo.orxAdcStitchingEnabled == 0)) {
		/* Override stitching for bandwidth above 200 MHz,if all MergeFilter */
		/* values are zero */
		/* 1 - Override ADC stitching - meaning do not do ADC stitching. */
		/* 0 - Do not override ADC stitching */
		cfgData[14] = 1;
	} else {
		cfgData[14] = 0x00;
	}

	cfgData[15] = 0x00; /* Not used...padding */

	if (device->devStateInfo.profilesValid & TX_PROFILE_VALID) {
		/* start of Tx profile data */
		cfgData[16] = init->tx.txProfile.dacDiv;
		cfgData[17] = init->tx.txProfile.txFirInterpolation;
		cfgData[18] = init->tx.txProfile.thb1Interpolation;
		cfgData[19] = init->tx.txProfile.thb2Interpolation;
		cfgData[20] = init->tx.txProfile.thb3Interpolation;
		cfgData[21] = init->tx.txProfile.txInt5Interpolation;
		cfgData[22] = 0; /* padding */
		cfgData[23] = 0; /* padding */

		for (i = 24; i < 28; i++) {
			cfgData[i] = (uint8_t)(((init->tx.txProfile.txInputRate_kHz * 1000) >> ((
							i - 24) * 8)) & 0x000000FF);
		}

		for (i = 28; i < 32; i++) {
			cfgData[i] = (uint8_t)(((init->tx.txProfile.primarySigBandwidth_Hz) >> ((
							i - 28) * 8)) & 0x000000FF);
		}

		for (i = 32; i < 36; i++) {
			cfgData[i] = (uint8_t)(((init->tx.txProfile.rfBandwidth_Hz) >> ((
							i - 32) * 8)) & 0x000000FF);
		}

		for (i = 36; i < 40; i++) {
			cfgData[i] = (uint8_t)(((init->tx.txProfile.txDac3dBCorner_kHz) >> ((
							i - 36) * 8)) & 0x000000FF);
		}

		for (i = 40; i < 44; i++) {
			cfgData[i] = (uint8_t)(((init->tx.txProfile.txBbf3dBCorner_kHz * 1000) >> ((
							i - 40) * 8)) & 0x000000FF);
		}
	} else {
		/* clear cfgData[16] to cfgData[43] */
		for (j = 16; j < 44; j++) {
			cfgData[j] = 0;
		}
	}

	if (device->devStateInfo.profilesValid & RX_PROFILE_VALID) {
		/* start of Rx profile data */
		cfgData[44] = (uint8_t)(init->rx.rxProfile.rxDdcMode);
		cfgData[45] = init->rx.rxProfile.rxFirDecimation;
		cfgData[46] = init->rx.rxProfile.rxDec5Decimation;
		cfgData[47] = init->rx.rxProfile.rhb1Decimation;

		for (i = 48; i < 52; i++) {
			cfgData[i] = (uint8_t)(((init->rx.rxProfile.rxOutputRate_kHz * 1000) >> ((
							i - 48) * 8)) & 0x000000FF);
		}

		for (i = 52; i < 56; i++) {
			/* sig bw placeholder */
			cfgData[i] = (uint8_t)(((init->rx.rxProfile.rfBandwidth_Hz) >> ((
							i - 52) * 8)) & 0x000000FF);
		}

		for (i = 56; i < 60; i++) {
			cfgData[i] = (uint8_t)(((init->rx.rxProfile.rfBandwidth_Hz) >> ((
							i - 56) * 8)) & 0x000000FF);
		}

		for (i = 60; i < 64; i++) {
			cfgData[i] = (uint8_t)(((init->rx.rxProfile.rxBbf3dBCorner_kHz * 1000) >> ((
							i - 60) * 8)) & 0x000000FF);
		}
	} else {
		/* clear cfgData[44] to cfgData[63] */
		for (j = 44; j < 64; j++) {
			cfgData[j] = 0;
		}
	}

	if (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) {
		/* start of ObsRx profile data */
		cfgData[64] = (uint8_t)(init->obsRx.orxProfile.orxDdcMode);
		cfgData[65] = init->obsRx.orxProfile.rxFirDecimation;
		cfgData[66] = init->obsRx.orxProfile.rxDec5Decimation;
		cfgData[67] = init->obsRx.orxProfile.rhb1Decimation;

		for (i = 68; i < 72; i++) {
			cfgData[i] = (uint8_t)(((init->obsRx.orxProfile.orxOutputRate_kHz * 1000) >> ((
							i - 68) * 8)) & 0x000000FF);
		}

		for (i = 72; i < 76; i++) {
			/* sig bw placeholder */
			cfgData[i] = (uint8_t)(((init->obsRx.orxProfile.rfBandwidth_Hz) >> ((
							i - 72) * 8)) & 0x000000FF);
		}

		for (i = 76; i < 80; i++) {
			cfgData[i] = (uint8_t)(((init->obsRx.orxProfile.rfBandwidth_Hz) >> ((
							i - 76) * 8)) & 0x000000FF);
		}

		for (i = 80; i < 84; i++) {
			cfgData[i] = (uint8_t)(((init->obsRx.orxProfile.rxBbf3dBCorner_kHz * 1000) >> ((
							i - 80) * 8)) & 0x000000FF);
		}
	} else {
		/* clear cfgData[64] to cfgData[83] */
		for (j = 64; j < 84; j++) {
			cfgData[j] = 0;
		}
	}

	/* writing to the ARM memory with the array data */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &cfgData[0], length);
	IF_ERR_RETURN_U32(retVal);

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_loadArmFromBinary(taliseDevice_t *device, uint8_t *binary,
				  uint32_t count)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t stackPtr[4] = {0};
	uint8_t bootAddr[4] = {0};
	uint32_t radioStatus = 0;
	uint32_t waitInterval_us = 0;
	uint32_t numEventChecks = 1;
	uint32_t eventCheck = 0;
	uint8_t armFieldValue[1] = {0};

	static const uint32_t MAX_BIN_CNT = 114688;
	static const uint16_t TRCKINGCALS_GPINT_EN_BYTEOFFSET = 0x0044;


#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_loadArmFromBinary()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (binary == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_LOADBIN_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (count != MAX_BIN_CNT) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_LOADBIN_INVALID_BYTECOUNT, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* extraction of stack pointer and boot address from top of array */
		stackPtr[0] = binary[0];
		stackPtr[1] = binary[1];
		stackPtr[2] = binary[2];
		stackPtr[3] = binary[3];

		bootAddr[0] = binary[4];
		bootAddr[1] = binary[5];
		bootAddr[2] = binary[6];
		bootAddr[3] = binary[7];

		/* writing binary data to ARM memory */
		retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
				TALISE_ADDR_ARM_START_PROG_ADDR, &binary[0], count);
		IF_ERR_RETURN_U32(retVal);
	}

	/* writing stack pointer [7:0] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_STACK_PTR_BYTE_0,
				   stackPtr[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing stack pointer [15:8] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_STACK_PTR_BYTE_1,
				   stackPtr[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing stack pointer [23:16] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_STACK_PTR_BYTE_2,
				   stackPtr[2]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing stack pointer [31:24] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_STACK_PTR_BYTE_3,
				   stackPtr[3]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing boot address [7:0] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_BOOT_ADDR_BYTE0,
				   bootAddr[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing boot address [15:8] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_BOOT_ADDR_BYTE1,
				   bootAddr[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing boot address [23:16] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_BOOT_ADDR_BYTE2,
				   bootAddr[2]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing boot address [31:24] */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_BOOT_ADDR_BYTE3,
				   bootAddr[3]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* setting arm_debug_enable[7]=1, mem_hresp_mask[3]=1, arm_m3_run[0]=1 */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_CTL_1, 0x89);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* verifying ARM checksum */
	retVal = (talRecoveryActions_t)TALISE_verifyArmChecksum(device);
	IF_ERR_RETURN_U32(retVal);

	/* Wait for ARM to exit BOOTUP state */
	waitInterval_us = (GETARMBOOTUP_INTERVAL_US > GETARMBOOTUP_TIMEOUT_US) ?
			  GETARMBOOTUP_TIMEOUT_US : GETARMBOOTUP_INTERVAL_US;
	numEventChecks = (waitInterval_us == 0) ? 1 : (GETARMBOOTUP_TIMEOUT_US /
			 waitInterval_us);

	for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++) {
		retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
		IF_ERR_RETURN_U32(retVal);

		if (radioStatus == TALISE_ARM_RADIO_STATUS_POWERUP) {
			/* Evaluate first since it's the most common case. Wait interval then check again */
			halError = ADIHAL_wait_us(device->devHalInfo, waitInterval_us);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_WAIT, halError, retVal,
						  TALACT_ERR_CHECK_TIMER);
		} else if (radioStatus == TALISE_ARM_RADIO_STATUS_READY) {
			break; /* Valid case - ARM booted successfully */
		} else if (radioStatus == TALISE_ARM_RADIO_STATUS_IDLE) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_APIARM_ERR,
							  TALAPI_ARMERR_BOOTUP_IDLE, retVal, TALACT_ERR_RESET_ARM);
		} else if (radioStatus == TALISE_ARM_RADIO_STATUS_RADIO_ON) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_APIARM_ERR,
							  TALAPI_ARMERR_BOOTUP_RADIO_ON, retVal, TALACT_ERR_RESET_ARM);
		} else if (radioStatus == TALISE_ARM_RADIO_STATUS_PROFILE_ERROR) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_APIARM_ERR,
							  TALAPI_ARMERR_BOOTUP_PROFILE_ERROR, retVal, TALACT_ERR_RESET_ARM);
		} else {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_APIARM_ERR,
							  TALAPI_ARMERR_BOOTUP_UNKNOWN_ERROR, retVal, TALACT_ERR_RESET_ARM);
		}
	}

	if (radioStatus != TALISE_ARM_RADIO_STATUS_READY) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_APIARM_ERR,
						  TALAPI_ARMERR_BOOTUP_TIMEOUT_ERROR, retVal, TALACT_ERR_RESET_ARM);
	}

	/* enable tracking cals to assert GP_INT */
	armFieldValue[0] = 1;
	retVal = (talRecoveryActions_t)TALISE_writeArmConfig(device,
			TALISE_ARM_OBJECTID_ENABLE_CALS_GPINT,
			TRCKINGCALS_GPINT_EN_BYTEOFFSET,
			&armFieldValue[0], sizeof(armFieldValue));
	IF_ERR_RETURN_U32(retVal);

	device->devStateInfo.devState = (taliseStates_t)(device->devStateInfo.devState |
					TAL_STATE_ARMLOADED);

	return (uint32_t)retVal;
}

/**
 * \brief Helper function to format ARM memory byte array with the specified ADC profile
 *
 * This function is used to load one ADC profile into a byte array that
 * will be written into ARM memory.  In total, Talise requires four ADC
 * profiles be loaded before loading the ARM firmware.  This function
 * is used by TALISE_loadAdcProfiles to format the ARM byte arrays before
 * writing them to the ARM memory.
 *
 * \pre This function is called during device initialization.  It is called
 *      automatically by the Talise API before the ARM binary firmware is
 *      loaded.
 *
 * \param armMemByteArray Pointer to the byte array to format with adc Profile data
 * \param adcProfile Pointer to the uint16_t array specifying a valid ADC profile
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
static void talFormatAdcProfileByteArray(uint8_t *armMemByteArray,
		uint16_t *adcProfile)
{
	uint8_t byteOffset = 0;
	uint8_t elementOffset = 0;
	uint8_t i = 0;

	/* Load Rx ADC profile */
	/* Load Capacitance, Conductance, and Current values into byte array (uint16_t typed) */
	for (i = elementOffset;
	     i < (elementOffset + TALISE_ADCPROFILE_NUM_CAP_COND_CURR_VALUES); i++) {
		armMemByteArray[byteOffset++] = (uint8_t)(adcProfile[i] & 0xFF);
		armMemByteArray[byteOffset++] = (uint8_t)((adcProfile[i] >> 8) & 0xFF);
	}

	/* Load Bias and VBG values (uint8_t typed) */
	elementOffset = TALISE_ADCPROFILE_NUM_CAP_COND_CURR_VALUES;

	for (i = elementOffset;
	     i < (elementOffset + TALISE_ADCPROFILE_NUM_BIAS_VBG_VALUES); i++) {
		armMemByteArray[byteOffset++] = (uint8_t)(adcProfile[i] & 0xFF);
	}

	/* Load DAC_G values into byte array (uint16_t typed) */
	elementOffset = TALISE_ADCPROFILE_NUM_CAP_COND_CURR_VALUES +
			TALISE_ADCPROFILE_NUM_BIAS_VBG_VALUES;

	for (i = elementOffset; i < (elementOffset + TALISE_ADCPROFILE_NUM_DACG_VALUES);
	     i++) {
		armMemByteArray[byteOffset++] = (uint8_t)(adcProfile[i] & 0xFF);
		armMemByteArray[byteOffset++] = (uint8_t)((adcProfile[i] >> 8) & 0xFF);
	}

	/* Load ES1 values (uint8_t typed) */
	elementOffset = TALISE_ADCPROFILE_NUM_CAP_COND_CURR_VALUES +
			TALISE_ADCPROFILE_NUM_BIAS_VBG_VALUES +
			TALISE_ADCPROFILE_NUM_DACG_VALUES;

	for (i = elementOffset; i < (elementOffset + TALISE_ADCPROFILE_NUM_ES1_VALUES);
	     i++) {
		armMemByteArray[byteOffset++] = (uint8_t)(adcProfile[i] & 0xFF);
	}

	/* 1 byte structure memory padding in ARM */
	byteOffset++;

	/* Load ALT_G values into byte array (uint16_t typed) */
	elementOffset = TALISE_ADCPROFILE_NUM_CAP_COND_CURR_VALUES +
			TALISE_ADCPROFILE_NUM_BIAS_VBG_VALUES +
			TALISE_ADCPROFILE_NUM_DACG_VALUES +
			TALISE_ADCPROFILE_NUM_ES1_VALUES;

	/* Write TALISE_ADCPROFILE ALTG_VALUES  - only one 16-bit value */
	armMemByteArray[byteOffset++] = (uint8_t)(adcProfile[elementOffset] & 0xFF);
	armMemByteArray[byteOffset] = (uint8_t)((adcProfile[elementOffset] >> 8) &
						0xFF);
}

uint32_t TALISE_loadAdcProfiles(taliseDevice_t *device, uint16_t *rxAdcProfile,
				uint16_t *orxLowPassAdcProfile,
				uint16_t *orxBandPassAdcProfile, uint16_t *loopBackAdcProfile,
				int16_t *orxMergeFilter)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t i = 0;
	uint8_t bytesToLoad[62] = {0};
	uint8_t sizeOfBytesToLoad = 0;
	static const uint32_t ARM_RXADCPROFILE_OFFSET =
		84;    /* ARM memory offset for ADC profile data structure */
	static const uint32_t ARM_ORX1RXADCPROFILE_OFFSET = 146;
	static const uint32_t ARM_ORX2RXADCPROFILE_OFFSET = 208;
	static const uint32_t ARM_LBRXADCPROFILE_OFFSET = 270;
	static const uint32_t ARM_ORXMERGEFILTERPROFILE_OFFSET = 332;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_loadAdcProfiles\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/**************************************************************/
	/* check if Rx profile is valid*/
	/**************************************************************/
	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) > 0) {
		talFormatAdcProfileByteArray(&bytesToLoad[0], &rxAdcProfile[0]);
	} else {
		/* if profile is not valid, load zeros into ARM memory */
		for (i = 0; i < sizeof(bytesToLoad); i++) {
			bytesToLoad[i] = 0;
		}
	}

	/* writing to the ARM memory with Rx ADC profiles */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + ARM_RXADCPROFILE_OFFSET), &bytesToLoad[0],
			sizeof(bytesToLoad));
	IF_ERR_RETURN_U32(retVal);

	/***************************************************************/
	/* check if Tx profile is valid - load Loopback Rx ADC profile */
	/***************************************************************/
	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) > 0) {
		talFormatAdcProfileByteArray(&bytesToLoad[0], &loopBackAdcProfile[0]);
	} else {
		/* if profile is not valid, load zeros into ARM memory */
		for (i = 0; i < sizeof(bytesToLoad); i++) {
			bytesToLoad[i] = 0;
		}
	}

	/* writing to the ARM memory with Loopback Rx ADC profiles */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + ARM_LBRXADCPROFILE_OFFSET), &bytesToLoad[0],
			sizeof(bytesToLoad));
	IF_ERR_RETURN_U32(retVal);

	/***************************************************************/
	/* check if ORx profile is valid                               */
	/***************************************************************/
	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0) {
		talFormatAdcProfileByteArray(&bytesToLoad[0], &orxLowPassAdcProfile[0]);
	} else {
		/* if profile is not valid, load zeros into ARM memory */
		for (i = 0; i < sizeof(bytesToLoad); i++) {
			bytesToLoad[i] = 0;
		}
	}

	/* writing to the ARM memory with ORx low pass ADC profile */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + ARM_ORX1RXADCPROFILE_OFFSET),
			&bytesToLoad[0], sizeof(bytesToLoad));
	IF_ERR_RETURN_U32(retVal);

	/* If ORx ADC stitching enabled, load bandpass ADC profile, else load low pass profile */
	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0) {
		if (device->devStateInfo.orxAdcStitchingEnabled > 0) {
			talFormatAdcProfileByteArray(&bytesToLoad[0], &orxBandPassAdcProfile[0]);
		} else {
			talFormatAdcProfileByteArray(&bytesToLoad[0], &orxLowPassAdcProfile[0]);
		}
	} else {
		/* if profile is not valid, load zeros into ARM memory */
		for (i = 0; i < sizeof(bytesToLoad); i++) {
			bytesToLoad[i] = 0;
		}
	}

	/* writing to the ARM memory with Loopback Rx ADC profiles */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + ARM_ORX2RXADCPROFILE_OFFSET),
			&bytesToLoad[0], sizeof(bytesToLoad));
	IF_ERR_RETURN_U32(retVal);

	/***************************************************************/
	/* check if ORx merge filter is valid                          */
	/***************************************************************/
	if (((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0) &&
	    (device->devStateInfo.orxAdcStitchingEnabled > 0)) {
		bytesToLoad[0] = 0xEF;
		bytesToLoad[1] = 0xBE;

		for (i = 0; i < 12; i++) {
			bytesToLoad[(2 * i) + 2] = (uint8_t)(orxMergeFilter[i] & 0xFF);
			bytesToLoad[(2 * i) + 3] = (uint8_t)((orxMergeFilter[i] >> 8) & 0xFF);
		}

		sizeOfBytesToLoad = 26;
	} else {
		bytesToLoad[0] = 0;
		bytesToLoad[1] = 0;
		sizeOfBytesToLoad = 2;
	}

	/* writing to the ARM memory with Merge filter profiles */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + ARM_ORXMERGEFILTERPROFILE_OFFSET),
			&bytesToLoad[0], sizeOfBytesToLoad);
	IF_ERR_RETURN_U32(retVal);

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_readArmMem(taliseDevice_t *device, uint32_t address,
			   uint8_t *returnData, uint32_t bytesToRead, uint8_t autoIncrement)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t dataMem;
	uint32_t i;
	uint8_t autoIncBit = 0;

	static const uint8_t READ_MEM_BIT = 0x80;
	static const uint8_t LEGACY_MODE_BIT = 0x20;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_readArmMem()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* check that start and stop address are in valid range */
	if (!(address >= TALISE_ADDR_ARM_START_PROG_ADDR
	      && address <= TALISE_ADDR_ARM_END_PROG_ADDR ) &&
	    !(address >= TALISE_ADDR_ARM_START_DATA_ADDR
	      && address <= TALISE_ADDR_ARM_END_DATA_ADDR)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_READARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (!((address + bytesToRead - 1) >= TALISE_ADDR_ARM_START_PROG_ADDR
	      && (address + bytesToRead - 1) <= TALISE_ADDR_ARM_END_PROG_ADDR ) &&
	    !((address + bytesToRead - 1) >= TALISE_ADDR_ARM_START_DATA_ADDR
	      && (address + bytesToRead - 1) <= TALISE_ADDR_ARM_END_DATA_ADDR)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_READARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((address >= TALISE_ADDR_ARM_START_DATA_ADDR) &&
	    (address <= TALISE_ADDR_ARM_END_DATA_ADDR)) {
		dataMem = 1;
	} else {
		dataMem = 0;
	}

	/* range checking autoIncrement */
	autoIncBit = (autoIncrement > 0) ? 1: 0;

	/* setting up ARM read for legacy mode with autoincrement bit setting */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_CTL,
				   READ_MEM_BIT | (dataMem << 6)  | LEGACY_MODE_BIT | (autoIncBit <<
						   1)); /* set to read */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_ADDR0,
				   (uint8_t)((address) >> 2)); /* write address[9:2] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_ADDR1,
				   (uint8_t)(address >> 10)); /* write address[17:10] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* start read-back at correct byte offset */
	/* without address auto increment set, 0x4 must be added to the address for correct indexing */
	if (autoIncrement > 0) {
		for (i = 0; i < bytesToRead; i++) {
			halError = talSpiReadByte(device->devHalInfo,
						  (uint16_t)(TALISE_ADDR_ARM_DMA_DATA0 + (((address & 0x3) + i) % 4)),
						  &returnData[i]);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	} else {
		for (i = 0; i < bytesToRead; i++) {
			halError = talSpiReadByte(device->devHalInfo,
						  (uint16_t)(TALISE_ADDR_ARM_DMA_DATA0 + (((address & 0x3) + i) % 4)),
						  &returnData[i]);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			if ((TALISE_ADDR_ARM_DMA_DATA0 + (((address & 0x3) + i) % 4)) ==
			    TALISE_ADDR_ARM_DMA_DATA3) {
				halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_ADDR0,
							   (uint8_t)(((address += 0x4) & 0x3FF) >> 2));
				retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
							  TALACT_ERR_RESET_SPI);
				IF_ERR_RETURN_U32(retVal);

				halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_ADDR1,
							   (uint8_t)((address >> 10) & 0x7F));
				retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
							  TALACT_ERR_RESET_SPI);
				IF_ERR_RETURN_U32(retVal);
			}
		}
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_writeArmMem(taliseDevice_t *device, uint32_t address,
			    uint8_t *data, uint32_t byteCount)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t dataMem = 0;
	uint32_t i = 0;
	uint8_t regWrite = 0;

	uint32_t addrIndex = 0;
	uint32_t dataIndex = 0;
	uint32_t spiBufferSize = HAL_SPIWRITEARRAY_BUFFERSIZE;
	uint16_t addrArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = {0};

	static const uint8_t FORCE_AUTO_INC = 0x02;
	static const uint8_t LEGACY_MODE_BIT = 0x20;

#if TALISE_VERBOSE
	talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
		      "TALISE_writeArmMem()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (!(address >= TALISE_ADDR_ARM_START_PROG_ADDR
	      && address <= TALISE_ADDR_ARM_END_PROG_ADDR) &&
	    !(address >= TALISE_ADDR_ARM_START_DATA_ADDR
	      && address <= TALISE_ADDR_ARM_END_DATA_ADDR)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_WRITEARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (!((address + byteCount - 1) >= TALISE_ADDR_ARM_START_PROG_ADDR
	      && (address + byteCount-1) <= TALISE_ADDR_ARM_END_PROG_ADDR ) &&
	    !((address + byteCount-1) >= TALISE_ADDR_ARM_START_DATA_ADDR
	      && (address + byteCount-1) <= TALISE_ADDR_ARM_END_DATA_ADDR)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_WRITEARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* determining the data space and setting dataMem bit accordingly */
	dataMem = ((address >= TALISE_ADDR_ARM_START_DATA_ADDR)
		   && (address <= TALISE_ADDR_ARM_END_DATA_ADDR)) ? 1: 0;

	/* clearing write bit, setting legacy mode, forcing auto increment of address for efficiency to form dma control word */
	regWrite |= (dataMem << 6) | LEGACY_MODE_BIT | FORCE_AUTO_INC;

	/* setting up the DMA control register for a write */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_CTL,
				   regWrite);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing the starting address */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_ADDR0,
				   (uint8_t)((address) >> 2));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_DMA_ADDR1,
				   (uint8_t)(address >> 10));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* starting write at zero address offset */
	for (i = 0; i < byteCount; i++) {
		addrArray[addrIndex++] = (uint16_t)(TALISE_ADDR_ARM_DMA_DATA0 + (((
				address & 0x3) + i) % 4));

		if (addrIndex == spiBufferSize) {
			halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &data[dataIndex],
						    addrIndex);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			dataIndex = dataIndex + addrIndex;
			addrIndex = 0;
		}
	}

	/* write remaining SPI bytes that did not fit  into a multiple of the spiBufferSize */
	if (addrIndex > 0) {
		halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &data[dataIndex],
					    addrIndex);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_writeArmConfig(taliseDevice_t *device, uint8_t objectId,
			       uint16_t offset, uint8_t *data, uint8_t byteCount)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t extendedData[4] = {0};
	uint8_t cmdStatusByte = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_writeArmConfig()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &data[0], byteCount);
	IF_ERR_RETURN_U32(retVal);

	/* ARM Object id, byte offset LSB, offset MSB = 0, copy 2 bytes */
	extendedData[0] = objectId;
	extendedData[1] = (offset & 0xFF);
	extendedData[2] = ((offset >> 8) & 0xFF);
	extendedData[3] = byteCount;

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_WRITECFG_OPCODE, &extendedData[0], 4);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_WRITECFG_OPCODE, &cmdStatusByte, WRITEARMCFG_TIMEOUT_US,
			WRITEARMCFG_INTERVAL_US);
	if ((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, objectId, cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_readArmConfig(taliseDevice_t *device, uint8_t objectId,
			      uint16_t offset, uint8_t *data, uint8_t byteCount)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t extendedData[4] = {0};
	uint8_t cmdStatusByte = 0;

	static const uint8_t AUTO_INC_BIT = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_readArmConfig()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* ARM Object id, byte offset LSB, offset MSB = 0, byteCount will read that number of bytes */
	extendedData[0] = objectId;
	extendedData[1] = (offset & 0xFF);
	extendedData[2] = ((offset >> 8) & 0xFF);
	extendedData[3] = byteCount;

	/* send the read config ARM opcode */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_READCFG_OPCODE, &extendedData[0], 4);
	IF_ERR_RETURN_U32(retVal);

	/* check the command status for timeout */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_READCFG_OPCODE, &cmdStatusByte, READARMCFG_TIMEOUT_US,
			READARMCFG_INTERVAL_US);
	if ((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, objectId, cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &data[0], byteCount, AUTO_INC_BIT);
	IF_ERR_RETURN_U32(retVal);

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_readArmCmdStatus(taliseDevice_t *device, uint16_t *errorWord,
				 uint16_t *statusWord)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t i = 0;
	uint8_t bytes[8] = {0};

#if TALISE_VERBOSE
	talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
		      "TALISE_readArmCmdStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing null pointer check */
	if ((errorWord == NULL) ||
	    (statusWord == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_READARMCMDSTATUS_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* ensuring the errorWord and statusWord are cleared */
	*errorWord = 0;
	*statusWord = 0;

	/* read in the entire 64-bit status register into a byte array and parse one byte at a time */
	for (i = 0; i < 8; i++) {
		halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_ARM_CMD_STATUS_0 + i,
					  &bytes[i]);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* assigning each pending bit from every opcode to a weighted position in statusWord */
		*statusWord |= (uint16_t)(((bytes[i] & 0x10) >> 3) | (bytes[i] & 0x01)) <<
			       (i * 2);

		/* checking each 3-bit error field per nibble for > 0, then assigning to weighted bit in errorWord */
		if ((bytes[i] & 0x0E) > 0) {
			*errorWord |= (0x0001 << (i * 2));
		} else if ((bytes[i] & 0xE0) > 0) {
			*errorWord |= (0x0002 << (i * 2));
		}
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_readArmCmdStatusByte(taliseDevice_t *device, uint8_t opCode,
				     uint8_t *cmdStatByte)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t cmdByteIndex = 0;
	uint8_t cmdByte = 0;

#if TALISE_VERBOSE
	talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
		      "TALISE_readArmCmdStatusByte()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* check for even-numbered opCodes including opcode 0, but must not be greater than 30 */
	if ((opCode != 0) &&
	    ((opCode % 2) ||
	     (opCode > 30))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ARMCMDSTATUS_INV_OPCODE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* calculating command status register offset based on opcode status packing */
		cmdByteIndex = opCode / 4;

		/* reading the command status register for given opcode */
		halError = talSpiReadByte(device->devHalInfo,
					  TALISE_ADDR_ARM_CMD_STATUS_0 + cmdByteIndex, &cmdByte);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* identifying nibble location in command register for given opcode */
		if (((opCode / 2) % 2) != 0) {
			*cmdStatByte = ((cmdByte >> 4) & 0x0F);
		} else {
			*cmdStatByte = (cmdByte & 0x0F);
		}

		return (uint32_t)retVal;
	}
}

uint32_t TALISE_waitArmCmdStatus(taliseDevice_t *device, uint8_t opCode,
				 uint8_t *cmdStatByte, uint32_t timeout_us, uint32_t waitInterval_us)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint32_t eventCheck = 0;
	uint32_t numEventChecks = 0;

	static const uint8_t ARM_ERR_MASK = 0x0E;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_waitArmCmdStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* check for even-numbered opCodes including opcode 0, but must not be greater than 30 */
	if ((opCode != 0) &&
	    ((opCode % 2) ||
	     (opCode > 30))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_WAITARMCMDSTATUS_INV_OPCODE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
	numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

	/* timeout event check loop */
	for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++) {
		/* read status of opcode */
		retVal = (talRecoveryActions_t)TALISE_readArmCmdStatusByte(device, opCode,
				cmdStatByte);
		IF_ERR_RETURN_U32(retVal);

		/* If error code is non zero in [3:1], - return error */
		if ((*cmdStatByte & ARM_ERR_MASK) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
							  TAL_ERR_ARMCMDSTATUS_ARMERROR, retVal, TALACT_ERR_RESET_ARM);
		}

		/* if pending bit is set for opcode of interest and the number of events have not expired, perform wait */
		if (((*cmdStatByte & 0x01) > 0) &&
		    (eventCheck < numEventChecks)) {
			halError = ADIHAL_wait_us(device->devHalInfo, waitInterval_us);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_WAIT, halError, retVal,
						  TALACT_ERR_CHECK_TIMER);
		} else {
			break;
		}
	}

	/* if ARM Command did not complete within the timeout period */
	if ((*cmdStatByte & 0x01) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_WAITARMCMDSTATUS_TIMEOUT, retVal, TALACT_ERR_RESET_ARM);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_sendArmCommand(taliseDevice_t *device, uint8_t opCode,
			       const uint8_t *extendedData, uint8_t extendedDataNumBytes)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t armCommandBusy = 0;
	uint8_t i = 0;
	uint16_t extCmdByteStartAddr = TALISE_ADDR_ARM_EXT_CMD_BYTE_1;

	uint32_t timeout_us = SENDARMCMD_TIMEOUT_US;
	uint32_t waitInterval_us = SENDARMCMD_INTERVAL_US;
	uint32_t eventCheck = 0;
	uint32_t numEventChecks = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_sendArmCommand()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* check for even-numbered opCodes only including opcode 0, but must not be greater than opCode 30 */
	if ((opCode != 0) &&
	    ((opCode % 2) ||
	     (opCode > 30))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ARMCMD_INV_OPCODE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* the number of valid extended data bytes is from 0-4 */
	if (extendedDataNumBytes > 4) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_ARMCMD_INV_NUMBYTES_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* setting a 2 second timeout for mailbox busy bit to be clear (can't send an arm mailbox command until mailbox is ready) */

	waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
	numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

	/* timeout event loop to permit non-blocking of thread */
	for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++) {
		halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_ARM_COMMAND,
					   &armCommandBusy, 0x80, 7);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		if (armCommandBusy > 0) {
			halError = ADIHAL_wait_us(device->devHalInfo, waitInterval_us);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_WAIT, halError, retVal,
						  TALACT_ERR_CHECK_TIMER);
		} else {
			break;
		}
	}

	/* if busy bit remains set after timeout event loop function is exited, otherwise command is sent after extended bytes */
	if (armCommandBusy > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_TIMEDOUT_ARMMAILBOXBUSY, retVal, TALACT_ERR_RESET_ARM);
	} else {
		if (extendedDataNumBytes > 0) {
			for (i = 0; i < extendedDataNumBytes; i++) {
				halError = talSpiWriteByte(device->devHalInfo, (extCmdByteStartAddr + i),
							   extendedData[i]);
				retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
							  TALACT_ERR_RESET_SPI);
				IF_ERR_RETURN_U32(retVal);
			}
		}

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_COMMAND, opCode);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getArmVersion_v2(taliseDevice_t *device,
				 taliseArmVersionInfo_t *talArmVersionInfo)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t ver[5] = {0};
	uint32_t fullVersion = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getArmVersion()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (talArmVersionInfo == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETARMVER_V2_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((device->devStateInfo.devState & TAL_STATE_ARMLOADED) !=
	    TAL_STATE_ARMLOADED) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETARMVER_V2_INVALID_ARM_NOT_LOADED, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_VERSION, &ver[0], sizeof(ver), 0);
	IF_ERR_RETURN_U32(retVal);

	fullVersion = (((uint32_t)ver[0]) | (((uint32_t)ver[1]) << 8) | (((
				uint32_t)ver[2]) << 16) | (((uint32_t)ver[3]) << 24));
	talArmVersionInfo->rcVer = (uint8_t)(fullVersion % 100);
	talArmVersionInfo->minorVer = (uint8_t)((fullVersion / 100) % 100);
	talArmVersionInfo->majorVer = (uint8_t)(fullVersion / 10000);

	if(ver[4] & 0x01) {
		talArmVersionInfo->buildType = TAL_ARM_BUILD_DEBUG;
	} else if(ver[4] & 0x04) {
		talArmVersionInfo->buildType = TAL_ARM_BUILD_TEST_OBJECT;
	} else {
		talArmVersionInfo->buildType = TAL_ARM_BUILD_RELEASE;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getArmVersion(taliseDevice_t *device, uint8_t *majorVer,
			      uint8_t *minorVer, uint8_t *rcVer)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t ver[4] = {0};
	uint32_t fullVersion = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getArmVersion()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((majorVer == NULL) ||
	    (minorVer == NULL) ||
	    (rcVer == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETARMVER_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_VERSION, &ver[0], sizeof(ver), 0);
	IF_ERR_RETURN_U32(retVal);

	fullVersion = (((uint32_t)ver[0]) | (((uint32_t)ver[1]) << 8) | (((
				uint32_t)ver[2]) << 16) | (((uint32_t)ver[3]) << 24));
	*rcVer = (uint8_t)(fullVersion % 100);
	*minorVer = (uint8_t)((fullVersion / 100) % 100);
	*majorVer = (uint8_t)(fullVersion / 10000);

	return (uint32_t)retVal;
}

uint32_t TALISE_verifyArmChecksum(taliseDevice_t *device)
{
	adiHalErr_t halError = ADIHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint32_t buildTimeChecksum = 0x00000000;
	uint32_t calculatedChecksum = 0x00000000;
	uint8_t buildData[4];
	uint8_t calcData[4];
	uint32_t timeout_us = VERIFY_ARM_CHKSUM_TIMEOUT_US;
	uint32_t waitInterval_us = VERIFY_ARM_CHKSUM_INTERVAL_US;
	uint32_t numEventChecks = 1;
	uint32_t eventCheck = 0;

	static const uint8_t CHECKSUM_BYTES = 0x4;
	static const uint32_t MIN_TIMEOUT_US = 10000;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_verifyArmChecksum()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* reading four (4) bytes at ARM checksum memory location */
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_BUILD_CHKSUM_ADDR, buildData, CHECKSUM_BYTES, 0);
	IF_ERR_RETURN_U32(retVal);

	/* determining build time checksum */
	buildTimeChecksum = (((uint32_t)buildData[3] << 24) | ((
				     uint32_t)buildData[2] << 16) | ((uint32_t)buildData[1] << 8) |
			     (uint32_t)buildData[0]);

	if (timeout_us < MIN_TIMEOUT_US) {
		/* Set minimum timeout of 10 ms. Prevent divide by zero error */
		timeout_us = MIN_TIMEOUT_US;
	}

	waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
	numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

	for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++) {
		retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
				TALISE_ADDR_ARM_CALC_CHKSUM_ADDR, calcData, CHECKSUM_BYTES, 0);
		IF_ERR_RETURN_U32(retVal);

		calculatedChecksum = (((uint32_t)calcData[3] << 24) | ((
					      uint32_t)calcData[2] << 16) | ((uint32_t)calcData[1] << 8) |
				      (uint32_t)calcData[0]);

		if ((calculatedChecksum == 0) &&
		    (eventCheck < numEventChecks)) {
			/* wait */
			halError = ADIHAL_wait_us(device->devHalInfo, timeout_us);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_WAIT, halError, retVal,
						  TALACT_ERR_CHECK_TIMER);
			IF_ERR_RETURN_U32(retVal);
		} else {
			break;
		}
	}

	/* return error if timeout occurred before ARM completed calculating checksum */
	if (calculatedChecksum == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_VERIFYBIN_CHECKSUM_TIMEOUT, retVal, TALACT_ERR_RESET_ARM);
	} else {
		/* performing checksum check */
		if (buildTimeChecksum != calculatedChecksum) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
							  TAL_ERR_LOADHEX_INVALID_CHKSUM, retVal, TALACT_ERR_RESET_ARM);
		} else {
			/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
			if (retVal == TALACT_NO_ACTION) {
				retVal = retValWarn;
			}

			return (uint32_t)retVal;
		}
	}
}

const char* talGetArmErrorMessage(uint32_t errSrc, uint32_t errCode)
{
	uint32_t armErrorFlag = ((errCode >> 1) & 0x07);

#ifndef TALISE_VERBOSE
	return "";
#else

	if (errSrc == TAL_ERRSRC_TALAPIARM) {
		switch(errCode) {
		case TALAPI_ARMERR_BOOTUP_TIMEOUT_ERROR:
			return "TALISE_loadArmFromBinary(): Timed out waiting for ARM bootup to happen\n";
		case TALAPI_ARMERR_BOOTUP_IDLE:
			return "TALISE_loadArmFromBinary(): ARM in IDLE mode after bootup\n";
		case TALAPI_ARMERR_BOOTUP_RADIO_ON:
			return "TALISE_loadArmFromBinary(): ARM in RADIO_ON mode after bootup\n";
		case TALAPI_ARMERR_BOOTUP_PROFILE_ERROR:
			return "TALISE_loadArmFromBinary(): ARM Profile error during bootup\n";
		case TALAPI_ARMERR_BOOTUP_UNKNOWN_ERROR:
			return "TALISE_loadArmFromBinary(): ARM unknown error during bootup\n";
		}
	} else if (errSrc == TAL_ERRSRC_TALARMCMD) {

		errCode = ((errCode & 0xFFFFFF00) | armErrorFlag);

		if ((((errCode >> 16) & 0xFF) == TALISE_ARM_WRITECFG_OPCODE) ||
		    (((errCode >> 16) & 0xFF) == TALISE_ARM_READCFG_OPCODE)) {
			/* Zero out second byte (object ID) for Write CFG and Read CFG opcodes */
			errCode = ((errCode & 0xFFFF0000) | armErrorFlag);
		}

		if (armErrorFlag > 0) {
			switch (errCode) {
			/* ARM Error flag = 1 */
			case ARMCMD_ERRCODE(TALISE_ARM_ABORT_OPCODE, 0, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_RADIOON_OPCODE, 0, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 1): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 1):/* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FREQ_HOP, 1):
				return "Detected nested Talise ARM commands, second command ignored\n";

			/* ARM Error flag = 2 */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 2): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 2):/* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FREQ_HOP, 2):
				return "Talise ARM Command not accepted in this state\n";

			case ARMCMD_ERRCODE(TALISE_ARM_RADIOON_OPCODE, 0, 2):
				return "Talise ARM Command not accepted in READY state.  RunInit not completed\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 2):
				return "Talise ARM Command not accepted. Input parameters out of range\n";

			/* ARM Error flag = 3 */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 3): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 3)
					:
				return "Talise ARM Command not supported\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 3):
				return "Talise ARM Command not supported. Another Aux ADC conversion progress\n";

			/* ARM Error flag = 4 */
			case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 4): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 4)
					:
				return "Talise ARM Invalid Object ID\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 4):
				return "Talise ARM Aux ADC Pin Mode config command received without a valid GPIO assigned to Aux ADC start";

			case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 4):
				return "Sequence length exceeds max allowed of 32\n";

			case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 5):
				return "Talise RF PLL frequencies not set before Run Init Cals\n";

			case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 6):
				return "Talise ARM initalization sequence aborted\n";

			case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 7):
				return "Talise init calibration error encountered\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 6):
			case ARMCMD_ERRCODE(TALISE_ARM_RADIOON_OPCODE, 0, 4):
				return "Invalid Talise ARM GPIO assignments for pin mode\n";

			case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 5):
				return "Talise ARM Object does not have configuration structure\n";
				break;

			case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 6):
				return "Talise ARM Object ID is currently blocked and update is not allowed\n";

			case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 7):
				return "Specified data does not fit this Talise ARM Object's data structure\n";

			case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 5):
				return "Talise ARM Source Object does not have configuration structure\n";

			case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 7):
				return "Requested data is outside of the Talise ARM Object's data structure\n";

			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 5): /* Fall through */
			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 5):
				return "Invalid ARM Signal ID\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 5)
					:
				return "Talise setRfPllFrequency has invalid input parameters\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 6)
					:
				return "RFPLL LO computation error\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 7)
					:
				return "RFPLL Synth Lock failed\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 5)
					:
				return "Requested Talise tracking calibration is not enabled - can not reschedule cal\n";

			case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 5):
				return "Invalid channel selected for Talise ARM calibration status.\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 5)
					:
				return "RFPLL loop filter settings could not converge to a result.\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 6)
					:
				return "RFPLL loop filter input parameters out of range.\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 7)
					:
				return "RFPLL loop filter invalid LO.\n";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 5)
					:
				return "Frequency Hopping - Invalid scan range specified";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 6)
					:
				return "Frequency Hopping - Requested Hop Frequency outside scan range";

			case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 7)
					:
				return "Frequency Hopping - PLL Error. More information can be obtained by executing TALISE_getFhmStatus()";

			default: {
				return "Invalid ARM error passed, not in error list\n";
			}
			}
		} else {
			return "Talise ARM command completed successfully - no Error\n";
		}
	} else {
		return "Wrong error handler - not a Talise ARM error\n";
	}
#endif

	return "Wrong error handler - not a Talise ARM error\n";
}

talRecoveryActions_t talArmCmdErrorHandler(taliseDevice_t *device,
		taliseErrHdls_t errHdl,
		uint32_t detErr, talRecoveryActions_t retVal, talRecoveryActions_t recAction)
{
	uint8_t armOpCode = 0;
	uint8_t armObjId = 0;
	uint8_t cmdStatusByte = 0;
	uint32_t armErrorFlag = 0;

	armOpCode = (uint8_t)(detErr >> 16);
	armObjId = (uint8_t)(detErr >> 8);
	cmdStatusByte = (uint8_t)(detErr);

	/* perform object id overrides for opcodes Abort, RunInit, RadioOn */
	if ((armOpCode == TALISE_ARM_ABORT_OPCODE)
	    || (armOpCode == TALISE_ARM_RUNINIT_OPCODE)
	    || (armOpCode == TALISE_ARM_RADIOON_OPCODE)
	    || (armOpCode == TALISE_ARM_WRITECFG_OPCODE)
	    || (armOpCode == TALISE_ARM_READCFG_OPCODE)) {
		armObjId = 0x00;
	}

	armErrorFlag = ((cmdStatusByte >> 1) & 0x07);
	if (armErrorFlag > 0) {
		switch (ARMCMD_ERRCODE(armOpCode, armObjId, armErrorFlag)) {
		/* Abort Error Messages */
		case ARMCMD_ERRCODE(TALISE_ARM_ABORT_OPCODE, 0, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_RADIOON_OPCODE, 0, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 1):/* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 1): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FREQ_HOP, 1):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 2):/* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 2): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FREQ_HOP, 2):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_RADIOON_OPCODE, 0, 2):
			recAction = TALACT_ERR_RERUN_INIT_CALS;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 3): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 3):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TDD_TXATTEN_CTRL, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_UPDATE, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_ARM_EXCEPTION, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RSSI, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_INITCAL_STATUS, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 4): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 4)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GS_AUX_ADC, 4):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 4):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 5):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 6):
			recAction = TALACT_ERR_RERUN_INIT_CALS;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, 7):
			recAction = TALACT_ERR_RERUN_INIT_CALS;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 6):
		case ARMCMD_ERRCODE(TALISE_ARM_RADIOON_OPCODE, 0, 4):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 5):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 6):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, 0, 7):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 5):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_READCFG_OPCODE, 0, 7):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 5): /* Fall through */
		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_GPIO_CTRL, 5):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 5)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 6)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY, 7)
				:
			recAction = TALACT_ERR_RERUN_INIT_CALS;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING, 5)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_CAL_STATUS, 5):
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 5)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 6)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 7)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 5)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 6)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		case ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE, 7)
				:
			recAction = TALACT_ERR_CHECK_PARAM;
			break;

		default: {
			recAction = TALACT_NO_ACTION;
		}
		}

		if (recAction != TALACT_NO_ACTION) {
			talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
				      TALISE_getErrorMessage(TAL_ERRSRC_TALARMCMD, detErr));
			device->devStateInfo.errSource = TAL_ERRSRC_TALARMCMD;
			device->devStateInfo.errCode = detErr;
		}
	} else {
		recAction = retVal;
	}

	return recAction;
}

// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise.c
 * \brief Contains top level functions to support initialization and ctrol of
 *         the Talise transceiver device.
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

/**
 * \mainpage Overview
 *
 * This document is intended for use by software engineering professionals
 * and includes detailed information regarding the data types and
 * function calls which comprise the Talise ANSI C API
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

/**
 * \page Suggested Use
 * The purpose of the Talise Application Programming Interface (API) is to abstract the low-level
 * SPI configuration, control, and related calculations required for the Talise family of transceiver devices
 *
 * Add the included source code as required to your baseband processor software build. Please reference the integration document
 * as well for further instructions.
 */

/**
 * \page Architecture Talise API Software Architecture
 * \image html software_architecture.png
 */

/**
 * \page Folders Talise API Folder Structure
 * \image html folder_structure.png
 */

#include "talise.h"
#include "talise_reg_addr_macros.h"
#include "talise_arm.h"
#include "talise_hal.h"
#include "talise_user.h"
#include "talise_error.h"
#include "talise_version.h"
#include "talise_rx.h"
#include "talise_tx.h"
#include "talise_jesd204.h"
#include "talise_cals.h"
#include "talise.h"

uint32_t TALISE_openHw(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	ADIHAL_setLogLevel(device->devHalInfo, TALISE_LOGGING);
	halError = ADIHAL_openHw(device->devHalInfo, 100);
	if (halError != ADIHAL_OK) {
		switch(halError) {
		case ADIHAL_SPI_FAIL:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_HAL_SPI,
							  halError, retVal, TALACT_ERR_RESET_SPI);
		case ADIHAL_GPIO_FAIL:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_HAL_GPIO,
							  halError, retVal, TALACT_ERR_RESET_GPIO);
		case ADIHAL_TIMER_FAIL:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_HAL_WAIT, halError,
							  retVal, TALACT_ERR_CHECK_TIMER);
		case ADIHAL_GEN_SW:
		default:
			return (uint32_t)TALACT_ERR_CHECK_PARAM;
		}
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_closeHw(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_closeHw(device->devHalInfo);

	if (halError != ADIHAL_OK) {
		switch(halError) {
		case ADIHAL_SPI_FAIL:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_HAL_SPI,
							  halError, retVal, TALACT_ERR_RESET_SPI);
		case ADIHAL_GPIO_FAIL:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_HAL_GPIO,
							  halError, retVal, TALACT_ERR_RESET_GPIO);
		case ADIHAL_TIMER_FAIL:
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_HAL_WAIT,
							  halError, retVal, TALACT_ERR_CHECK_TIMER);
		case ADIHAL_GEN_SW:
		default:
			return (uint32_t)TALACT_ERR_CHECK_PARAM;
		}
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_resetDevice(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_resetDevice()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* toggle RESETB on device with matching spi chip select index */
	halError = ADIHAL_resetHw(device->devHalInfo);
	if (halError == ADIHAL_WAIT_TIMEOUT) {
		/*Retry*/
		halError = ADIHAL_resetHw(device->devHalInfo);
	}

	if (halError != ADIHAL_OK) {
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_GPIO, halError, retVal,
					  TALACT_ERR_RESET_GPIO);
	}

	/* Power down deserializers */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DES_PHY_PWR_DWN_0,
				   0xF0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DES_PHY_PWR_DWN_4,
				   0x2F);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	device->devStateInfo.devState = TAL_STATE_POWERONRESET;

	return (uint32_t)retVal;
}

static talRecoveryActions_t talInitializeDdc(taliseDevice_t *device,
		taliseInit_t *init)
{
	uint8_t totalFramerM = 0;
	uint8_t combineBands = 0;
	uint8_t pfirOutDecimation = 0;
	uint8_t ncoPhaseAccumDivideRatio = 0;
	uint8_t rxNco1PhaseAccumDivideRatio = 0;
	uint8_t rxNco2PhaseAccumDivideRatio = 0;
	uint8_t registerValue = 0;
	uint8_t rxNcoControl = 0;
	adiHalErr_t halError = ADIHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	static const uint8_t RX_DUALBAND_ENABLED = 1;
	static const uint8_t RX_DUALBAND_DISABLED = 0;
	static const uint8_t ENABLE_NCO1 = 0x01;
	static const uint8_t ENABLE_NCO2 = 0x40;
	static const uint8_t ENABLE_NCO1_REALIF = 0x20;
	static const uint8_t ENABLE_NCO2_REALIF = 0x80;
	static const uint8_t DDC_DEC2 = 0x04;    /* Set DDC Mode and enable bit */
	static const uint8_t DDC_INT2 = 0x14;    /* Set DDC Mode and enable bit */
	static const uint8_t DDC_LOWPASS = 0x0C; /* Set DDC Mode and enable bit */

	/* Verify Null pointer to *init */
	if (init == NULL) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_INITIALIZE_DDC_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) &&
	    (init->rx.rxChannels != TAL_RXOFF)) {
		/* Phase accumulator calculation for NCO : FLOGB2(Decimation at PFIR out) - 2 */
		/* The -2 is because NCO clock is divided version of the div4/5 clock */
		/* For TAL_RXDDC_ZIF_TO_RIF_INT2 mode, Decimation at PFIR should be more than 8 since it is going to be interpolated */
		pfirOutDecimation = init->rx.rxProfile.rxDec5Decimation *
				    init->rx.rxProfile.rhb1Decimation * init->rx.rxProfile.rxFirDecimation;
		if (((pfirOutDecimation < 4)
		     && (init->rx.rxProfile.rxDdcMode != TAL_RXDDC_INT2)) ||
		    ((pfirOutDecimation < 8)
		     && (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF)) ||
		    (pfirOutDecimation > 40)) {
			return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						TAL_ERR_INITIALIZE_DDC_INV_DEC_AT_PFIR, retVal, TALACT_ERR_CHECK_PARAM);
		} else {
			pfirOutDecimation >>= 3;
			for (ncoPhaseAccumDivideRatio = 0; ncoPhaseAccumDivideRatio < 3;
			     ncoPhaseAccumDivideRatio++) {
				if (pfirOutDecimation == 0) {
					break;
				} else {
					pfirOutDecimation >>= 1;
				}
			}
		}

		device->devStateInfo.rxDualBandEnabled = RX_DUALBAND_DISABLED;
		device->devStateInfo.rxOutputRate_kHz = init->rx.rxProfile.rxOutputRate_kHz;


		/* Dualband mode */
		if ((init->rx.rxProfile.rxNcoShifterCfg.bandAInputBandWidth_kHz != 0) &&
		    (init->rx.rxProfile.rxNcoShifterCfg.bandBInputBandWidth_kHz != 0)) {
			/* Total number of converters */
			totalFramerM = (((init->rx.framerSel == TAL_FRAMER_A)
					 || (init->rx.framerSel == TAL_FRAMER_A_AND_B)) ?
					init->jesd204Settings.framerA.M : 0) +
				       (((init->rx.framerSel == TAL_FRAMER_B)
					 || (init->rx.framerSel == TAL_FRAMER_A_AND_B)) ?
					init->jesd204Settings.framerB.M : 0);

			/*  If one Rx enabled and M = 2, combine two bands. If M = 4, separate them */
			/*  If two Rx enabled and M = 4, combine two bands. If M = 8, separate them */
			if ((init->rx.rxChannels == TAL_RX1) ||
			    (init->rx.rxChannels == TAL_RX2)) {
				if ((totalFramerM != 2) && (totalFramerM != 4)) {
					return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								TAL_ERR_INITIALIZE_DDC_INV_TOTAL_M_2OR4, retVal, TALACT_ERR_CHECK_PARAM);
				}

				combineBands = (totalFramerM == 2) ? 1 : 0;
			} else { /* TAL_RX1RX2 */
				if ((totalFramerM != 4) && (totalFramerM != 8)) {
					return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								TAL_ERR_INITIALIZE_DDC_INV_TOTAL_M_4OR8, retVal, TALACT_ERR_CHECK_PARAM);
				}

				combineBands = (totalFramerM == 4) ? 1 : 0;
			}

			/* Phase accumulator calculation for NCO1 : FLOGB2(Decimation at PFIR out) - 2 */
			rxNco1PhaseAccumDivideRatio = ncoPhaseAccumDivideRatio;

			/* Phase accumulator calculation for NCO2 : NCO1 + 1 */
			rxNco2PhaseAccumDivideRatio = rxNco1PhaseAccumDivideRatio + 1;

			/* Setup value of NCO1 and NCO2 */
			registerValue = (rxNco2PhaseAccumDivideRatio << 4) |
					rxNco1PhaseAccumDivideRatio;
			halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_7,
						    registerValue, 0x73, 0);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN(retVal);

			/* NCO configuration: NCO1 enable, DDC HB enable, DDC mode = Dec2, NCO2 enable */
			halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_RX_NCO_CONTROL_0,
						    0x45, 0xFD, 0);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN(retVal);

			/* Band configuration : Band A enable, Band B enable, Combine bands */
			registerValue = (combineBands << 2) | 0x3;
			halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RX_BAND_CONTROL,
						   registerValue);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN(retVal);

			/* Select Real IF output for NCO2 */
			if (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF) {
				halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_RX_NCO_CONTROL_0,
							    0x80, 0x80, 0);
				retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
							  TALACT_ERR_RESET_SPI);
				IF_ERR_RETURN(retVal);
			}

			/* Initialize Dual Band Settings */
			retVal = talSetDualBandSettings(device, init);
			IF_ERR_RETURN(retVal);

			device->devStateInfo.rxDualBandEnabled = RX_DUALBAND_ENABLED;
		}/* end dualband */

		/* Not Dualband */
		else {
			/* Phase accumulator calculation for NCO1 : FLOGB2(Decimation at PFIR out) - 2 */
			rxNco1PhaseAccumDivideRatio = ncoPhaseAccumDivideRatio;
			rxNcoControl = 0;

			switch(init->rx.rxProfile.rxDdcMode) {
			case TAL_RXDDC_BYPASS:
			case TAL_RXDDC_BYPASS_REALIF:
				rxNco2PhaseAccumDivideRatio = ncoPhaseAccumDivideRatio;
				/* Leave rxNcoControl = 0 to bypass DDC HB */
				break;

			case TAL_RXDDC_FILTERONLY:
			case TAL_RXDDC_FILTERONLY_REALIF:
				rxNco2PhaseAccumDivideRatio = ncoPhaseAccumDivideRatio;
				rxNcoControl |= DDC_LOWPASS;
				break;

			case TAL_RXDDC_DEC2:
			case TAL_RXDDC_DEC2_REALIF:
				rxNco2PhaseAccumDivideRatio = ncoPhaseAccumDivideRatio + 1;
				rxNcoControl |= DDC_DEC2;
				break;

			case TAL_RXDDC_INT2:
			case TAL_RXDDC_INT2_REALIF:
				rxNco2PhaseAccumDivideRatio = ncoPhaseAccumDivideRatio - 1;
				rxNcoControl |= DDC_INT2;
				break;

			default:

				return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							TAL_ERR_INITIALIZE_DDC_INV_RXDDCMODE, retVal, TALACT_ERR_CHECK_PARAM);
			}

			/* if nco1 shift */
			if (init->rx.rxProfile.rxNcoShifterCfg.bandANco1Freq_kHz != 0) {
				rxNcoControl |= ENABLE_NCO1;

				if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF) ||
				    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF) ||
				    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF) ||
				    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
					rxNcoControl |= ENABLE_NCO1_REALIF;
				}
			}

			/* if nco2 shift */
			if (init->rx.rxProfile.rxNcoShifterCfg.bandANco2Freq_kHz != 0) {
				rxNcoControl |= ENABLE_NCO2;

				if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF) ||
				    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF) ||
				    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF) ||
				    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
					rxNcoControl &= ~ENABLE_NCO1_REALIF;
					rxNcoControl |= ENABLE_NCO2_REALIF;
				}
			}

			/* Setup value of NCO2 phase accum clock */
			halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_7,
						    ((rxNco2PhaseAccumDivideRatio << 4) | rxNco1PhaseAccumDivideRatio), 0x73, 0);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN(retVal);

			/* Set DDC HB mode, and NCO1/2 enable/disable bits */
			halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_RX_NCO_CONTROL_0,
						    rxNcoControl, 0xFD, 0);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN(retVal);

			/* Initialize single band shift Settings */
			retVal = talSetupNcoShifter(device, init);
			IF_ERR_RETURN(retVal);
		} /* end Not Dualband */
	}

	return retVal;
}

uint32_t TALISE_initialize(taliseDevice_t *device, taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t txChannelSettings = 0;
	uint8_t rxChannelSettings = 0;
	uint8_t orxChannelSettings = 0;
	uint8_t dacClockRateSelect = 0;
	uint8_t rxFirDecBitField = 0;
	uint8_t orxFirDecBitField = 0;
	uint8_t digDeviceClockDiv = 0;
	uint32_t digRefClock_MHz = 0;
	taliseInfo_t clearInfo = {
		.devState = TAL_STATE_POWERONRESET,
		.initializedChannels = 0,
		.profilesValid = 0,
		.errSource = 0,
		.errCode = 0,
		.clocks =
		{
			.deviceClock_kHz = 0,
			.clkPllVcoFreq_kHz = 0,
			.clkPllHsDiv = TAL_HSDIV_2,
			.hsDigClkDiv2_Hz = 0,
			.hsDigClkDiv4or5_Hz = 0,
			.rfPllUseExternalLo = 0
		},
		.gainMode = TAL_MGC,
		.gainIndexes =
		{
			.rx1MinGainIndex = 0,
			.rx1MaxGainIndex = 0,
			.rx2MinGainIndex = 0,
			.rx2MaxGainIndex = 0,
			.orx1MinGainIndex = 0,
			.orx1MaxGainIndex = 0,
			.orx2MinGainIndex = 0,
			.orx2MaxGainIndex = 0
		},
		.txAttenStepSize = TAL_TXATTEN_0P05_DB,
		.orxAdcStitchingEnabled = 0,
		.usedGpiopins = 0,
		.usedGpio3p3pins = 0,
		.rxFramerNp = 0,
		.orxFramerNp = 0,
		.rxOutputRate_kHz = 0,
		.txInputRate_kHz = 0,
		.rxDdcMode = TAL_RXDDC_BYPASS,
		.rxDualBandEnabled = 0,
		.rxTotalM = 0,
		.rxBandwidth_Hz = 0,
		.txBandwidth_Hz = 0,
		.orxBandwidth_Hz = 0,
		.swTest = 0,
		.deviceSiRev = 0,
		.talErrFunctionTable =
		{
			.talErrorFunctionTable = {}
		},
		.talFhmFreqRange =
		{
			.fhmMinFreq_MHz = 0,
			.fhmMaxFreq_MHz = 0
		},
		.talFhmTriggerMode = TAL_FHM_GPIO_MODE,
		.talFhmInitHopFreq_Hz = 0,
		.talFhmMcsSync = 0
	};

	uint32_t agcClock_Hz = 0;
	uint32_t gainUpdateCount = 0;
	taliseAdcSampleXbar_t adcXbar = {(taliseAdcSampleXbarSelect_t)0,(taliseAdcSampleXbarSelect_t)0,(taliseAdcSampleXbarSelect_t)0,(taliseAdcSampleXbarSelect_t)0,(taliseAdcSampleXbarSelect_t)0,(taliseAdcSampleXbarSelect_t)0,(taliseAdcSampleXbarSelect_t)0,(taliseAdcSampleXbarSelect_t)0};
	taliseDacSampleXbar_t dacXbar = {(taliseDacSampleXbarSelect_t)0,(taliseDacSampleXbarSelect_t)0};
	uint8_t deframerSyncPadCfg = 0;
	uint16_t syncbPadCfgAddr = 0;
	uint8_t framerSyncPadCfg = 0;
	uint8_t sysrefCfg = 0;
	uint8_t regData = 0;

	static const uint8_t ULB_THRESHOLD = 0x27;
	static const uint8_t ULB2_THRESHOLD = 0x37;
	static const uint8_t LLB_THRESHOLD = 0x1b;
	static const uint8_t LLB2_THRESHOLD = 0x27;
	static const uint8_t GPINT_MASK_AUXPLL = 1;
	static const uint8_t MIN_SUPPORTED_SIREV = 0xB1;
	static const uint8_t TPC_MODE_TX1_SPI = 0x01;
	static const uint8_t TPC_MODE_TX2_SPI = 0x04;

	/* Verify pointers to *device and *init are not NULL */
	if (device == NULL) {
		/* Can not write to log since log function requires device data structure */
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	if (init == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_initialize()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* clear memory of Talise API state container */
	device->devStateInfo = clearInfo;

	device->devStateInfo.txAttenStepSize = init->tx.txAttenStepSize;

	/* Calculate digital clocks based on information in init structure */
	retVal = (talRecoveryActions_t)TALISE_calculateDigitalClocks(device,
			&init->clocks);
	IF_ERR_RETURN_U32(retVal);

	/* Verify Rx/Tx and ObsRx profiles are valid combinations
	 * and set device->devStateInfo.profilesValid member based on init struct settings
	 */
	retVal = (talRecoveryActions_t)TALISE_verifyProfiles(device, init);
	IF_ERR_RETURN_U32(retVal);

	/* Set 3 or 4-wire SPI mode, MSBFirst/LSBfirst in device, pushes CPOL=0, CPHA=0, longInstWord=1 into device->spiSettings */
	retVal = (talRecoveryActions_t)TALISE_setSpiSettings(device,
			&init->spiSettings);
	IF_ERR_RETURN_U32(retVal);

	/* Read device silicon revision */
	retVal = (talRecoveryActions_t)TALISE_getDeviceRev(device,
			&device->devStateInfo.deviceSiRev);
	IF_ERR_RETURN_U32(retVal);

	/* Make sure Silicon is at least B1 silicon. API supports B1, C0 silicon and should work with any future silicon */
	if (device->devStateInfo.deviceSiRev < MIN_SUPPORTED_SIREV) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_INV_SIREV, retVal, TALACT_ERR_RESET_FULL);
	}

	/* Disable stream processor TxEnable/RxEnable pin control mode */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_RADIO_CONTROL_CONFIG1, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable Reference clock - Set REFCLK pad common mode voltage */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_REF_PAD_CONFIG2,
				   0x07);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Master bias trim and reset bandgap */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MASTERBIAS_CONFIG1,
				   0x0F);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MASTERBIAS_CONFIG0,
				   0x30);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MASTERBIAS_CONFIG0,
				   0x10);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	sysrefCfg = (((init->jesd204Settings.sysrefLvdsMode > 0) ? 1 : 0) << 1)
		    | /* Set SYSREF PAD for LVDS mode = 1, or CMOS mode = 0 */
		    (((init->jesd204Settings.sysrefLvdsPnInvert > 0) ? 1 : 0) << 2) |
		    (((init->jesd204Settings.sysrefLvdsMode > 0) ? 1 : 0) <<
		     4);  /* Set LVDS 100 ohm input termination */

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SYSREF_PAD_CONFIG,
				   sysrefCfg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* mask the RF PLL GP Interrupt */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
				    GPINT_MASK_AUXPLL, 0x04, 2);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (init->jesd204Settings.deframerA.deserializerLanesEnabled == 0) {
		/* If no lanes used for deframer A, do not enable SYNCB pad */
	} else {
		if (init->jesd204Settings.deframerA.syncbOutSelect == 0) {
			syncbPadCfgAddr = TALISE_ADDR_TX1_SYNC_PAD_CONFIG;
		} else {
			syncbPadCfgAddr = TALISE_ADDR_TX2_SYNC_PAD_CONFIG;
		}

		/* Valid range 0-3 */
		if (init->jesd204Settings.deframerA.syncbOutCmosSlewRate > 3) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_DEFA_SLEWRATE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		deframerSyncPadCfg = (init->jesd204Settings.deframerA.syncbOutCmosSlewRate << 5)
				     | (((init->jesd204Settings.deframerA.syncbOutCmosDriveLevel > 0) ? 1 : 0) << 4)
				     | (0x01 << 3) /* Set tx_sync_pad_lvds_low_cm */
				     | (((init->jesd204Settings.deframerA.syncbOutLvdsPnInvert > 0) ? 1 : 0) << 2)
				     | (((init->jesd204Settings.deframerA.syncbOutLvdsMode > 0) ? 1 : 0) << 1);

		halError = talSpiWriteByte(device->devHalInfo, syncbPadCfgAddr,
					   deframerSyncPadCfg);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (init->jesd204Settings.deframerB.deserializerLanesEnabled == 0) {
		/* If no lanes used for deframer B, do not enable SYNCB pad */
	} else {
		if (init->jesd204Settings.deframerB.syncbOutSelect == 0) {
			syncbPadCfgAddr = TALISE_ADDR_TX1_SYNC_PAD_CONFIG;
		} else {
			syncbPadCfgAddr = TALISE_ADDR_TX2_SYNC_PAD_CONFIG;
		}

		/* Valid range 0-3 */
		if (init->jesd204Settings.deframerB.syncbOutCmosSlewRate > 3) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_INV_DEFB_SLEWRATE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		deframerSyncPadCfg = (init->jesd204Settings.deframerB.syncbOutCmosSlewRate << 5)
				     | (((init->jesd204Settings.deframerB.syncbOutCmosDriveLevel > 0) ? 1 : 0) << 4)
				     | (0x01 << 3) /* Set tx_sync_pad_lvds_low_cm */
				     | (((init->jesd204Settings.deframerB.syncbOutLvdsPnInvert > 0) ? 1 : 0) << 2)
				     | (((init->jesd204Settings.deframerB.syncbOutLvdsMode > 0) ? 1 : 0) << 1);

		halError = talSpiWriteByte(device->devHalInfo, syncbPadCfgAddr,
					   deframerSyncPadCfg);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Look at each framer and enable the RXSYNCB used for each framer. It is possible that they use the same RXSYNCB pin */
	if (init->jesd204Settings.framerA.serializerLanesEnabled == 0) {
		/* framer A disabled, do not enable SYNCB pad */
	} else {
		if (init->jesd204Settings.framerA.syncbInSelect == 0) {
			syncbPadCfgAddr = TALISE_ADDR_RX1_SYNC_PAD_CONFIG;
		} else {
			syncbPadCfgAddr = TALISE_ADDR_RX2_SYNC_PAD_CONFIG;
		}

		framerSyncPadCfg = (((init->jesd204Settings.framerA.syncbInLvdsPnInvert > 0) ?
				     1 : 0) << 2) |
				   (((init->jesd204Settings.framerA.syncbInLvdsMode > 0) ? 1 : 0) << 1) |
				   (((init->jesd204Settings.framerA.syncbInLvdsMode > 0) ? 1 : 0) <<
				    4); /* if LVDS mode, enable internal 100ohm termination */

		halError = talSpiWriteByte(device->devHalInfo, syncbPadCfgAddr,
					   framerSyncPadCfg);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (init->jesd204Settings.framerB.serializerLanesEnabled == 0) {
		/* framer B disabled, do not enable SYNCB pad */
	} else {
		if (init->jesd204Settings.framerB.syncbInSelect == 0) {
			syncbPadCfgAddr = TALISE_ADDR_RX1_SYNC_PAD_CONFIG;
		} else {
			syncbPadCfgAddr = TALISE_ADDR_RX2_SYNC_PAD_CONFIG;
		}

		framerSyncPadCfg = (((init->jesd204Settings.framerB.syncbInLvdsPnInvert > 0) ?
				     1 : 0) << 2) |
				   (((init->jesd204Settings.framerB.syncbInLvdsMode > 0) ? 1 : 0) << 1) |
				   (((init->jesd204Settings.framerB.syncbInLvdsMode > 0) ? 1 : 0) <<
				    4); /* if LVDS mode, enable internal 100ohm termination */

		halError = talSpiWriteByte(device->devHalInfo, syncbPadCfgAddr,
					   framerSyncPadCfg);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Set Digital device clock divider - provides clock to 1us timer logic, and some other blocks */
	if (init->clocks.deviceClock_kHz <= 200000) {
		digDeviceClockDiv =  0; /* Device clock / 1 */
		digRefClock_MHz = (init->clocks.deviceClock_kHz / 1000);
	} else if ((init->clocks.deviceClock_kHz <= 400000)
		   && (init->clocks.deviceClock_kHz > 200000)) {
		digDeviceClockDiv =  1; /* Device clock / 2 */
		digRefClock_MHz = (((init->clocks.deviceClock_kHz / 1000) + 1) >>
				   1); /* /2 with round to nearest */
	} else if ((init->clocks.deviceClock_kHz <= 800000)
		   && (init->clocks.deviceClock_kHz > 400000)) {
		digDeviceClockDiv =  2; /* Device clock / 4 */
		digRefClock_MHz = (((init->clocks.deviceClock_kHz / 1000) + 2) >>
				   2); /* /4 with round to nearest */
	} else if ((init->clocks.deviceClock_kHz <= 1600000)
		   && (init->clocks.deviceClock_kHz > 800000)) {
		digDeviceClockDiv =  3; /* Device clock / 8 */
		digRefClock_MHz = (((init->clocks.deviceClock_kHz / 1000) + 4) >>
				   3); /* /8 with round to nearest */
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INIT_INV_DEVCLK, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Set Digital device clock divider */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_0,
				    digDeviceClockDiv, 0xE0, 5);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set number of divided device clock cycles per microsecond */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_REFERENCE_CLOCK_CYCLES, (uint8_t)(digRefClock_MHz - 1));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Negate Rx2 so phase matches Rx1 */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_TX_RX_DATAPATH_INVERSION, 0x0C);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx Settings */
	if (device->devStateInfo.profilesValid & TX_PROFILE_VALID) {

		if (init->tx.txProfile.thb3Interpolation == 2) {
			txChannelSettings |= 0x10;
		}

		if (init->tx.txProfile.thb2Interpolation == 2) {
			txChannelSettings |= 0x04;
		}

		if (init->tx.txProfile.thb1Interpolation == 2) {
			txChannelSettings |= 0x08;
		}

		switch (init->tx.txProfile.txFirInterpolation) {
		case  1:
			txChannelSettings |= 0x01;
			break;
		case  2:
			txChannelSettings |= 0x02;
			break;
		case  4:
			txChannelSettings |= 0x03;
			break;
		}

		/* Set Rx Dec5 if Int5 is enabled */
		if (init->tx.txProfile.txInt5Interpolation == 5) {
			rxChannelSettings |= 0x02;
		}

		if (init->tx.txProfile.dacDiv == 2) {
			dacClockRateSelect = 1; /* DAC Clock = hsDigClk/2 */
		}
	}

	/* Rx settings */
	if (device->devStateInfo.profilesValid & RX_PROFILE_VALID) {
		if (init->rx.rxProfile.rxDec5Decimation == 5) {
			rxChannelSettings |= 0x02;
		}

		if ( init->rx.rxProfile.rhb1Decimation == 2) {
			rxChannelSettings |= 0x10;
		}

		switch (init->rx.rxProfile.rxFirDecimation) {
		case 1:
			rxFirDecBitField |= 0x01;
			break;
		case 2:
			rxFirDecBitField |= 0x02;
			break;
		case 4:
			rxFirDecBitField |= 0x03;
			break;
		}
	}

	/* Determine ORx channel settings */
	if (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) {
		if ( init->obsRx.orxProfile.rxDec5Decimation == 5) {
			rxChannelSettings |= 0x02;
		}

		/* Enable ORX RHB2 Wideband */
		orxChannelSettings = 0x10;

		if (init->obsRx.orxProfile.rhb1Decimation == 2) {
			orxChannelSettings |= 0x04;
		}

		switch (init->obsRx.orxProfile.rxFirDecimation) {
		case 1:
			orxFirDecBitField |= 0x01;
			break;
		case 2:
			orxFirDecBitField |= 0x02;
			break;
		case 4:
			orxFirDecBitField |= 0x03;
			break;
		}
	}

	/************************************************************************
	* Set channel config settings for Rx/Tx/ and ORx channels
	************************************************************************/
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CONFIGURATION_CONTROL_2, txChannelSettings);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CONFIGURATION_CONTROL_4, rxChannelSettings);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CONFIGURATION_CONTROL_ORX, orxChannelSettings);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_RX_PFIR_BANK_A_SETTINGS, rxFirDecBitField, 0x03, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_RX_PFIR_BANK_B_SETTINGS, orxFirDecBitField, 0x03, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_RX_PFIR_BANK_C_SETTINGS, orxFirDecBitField, 0x03, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable SPI Tx Atten mode and set Atten step size. Only write bottom 6 bits */
	regData = 0;
	regData |= TPC_MODE_TX1_SPI;
	regData |= TPC_MODE_TX2_SPI;
	regData |= (init->tx.txAttenStepSize << 4);
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_TPC_CONFIG,
				    regData, 0x3F, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set DAC divider */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_1,
				    dacClockRateSelect, 0x01, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set ADC Flash bias */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ADC_BIAS_FLASH_VISB,
				   0x7D);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set the CLKPLL with the frequency from the device data structure */
	retVal = (talRecoveryActions_t)TALISE_initDigitalClocks(device, &init->clocks);
	IF_ERR_RETURN_U32(retVal);

	halError = ADIHAL_wait_us(device->devHalInfo, CLKPLL_LOCK_US);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_CHECK_TIMER);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_0, 0,
				    0x01, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_0, 1,
				    0x01, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable digital clocks - this is gated by the CLKPLL being locked */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_0, 1,
				    0x04, 2);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Wait for CLKPLL CP Cal done and CLKPLL Lock  or throw error message */
	retVal = (talRecoveryActions_t)TALISE_waitForEvent(device, TAL_CLKPLLCP,
			CLKPLL_CPCAL_TIMEOUT_US, CLKPLL_CPCAL_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitForEvent(device, TAL_CLKPLL_LOCK,
			CLKPLL_LOCK_TIMEOUT_US, CLKPLL_LOCK_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);

	/* Toggle DEC5 divider reset bit */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_0, 0,
				    0x01, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Program FIR filters after CLKPLL locked */
	if (device->devStateInfo.profilesValid & TX_PROFILE_VALID) {
		retVal = (talRecoveryActions_t)TALISE_setTxPfirSyncClk(device,
				&init->tx.txProfile);
		IF_ERR_RETURN_U32(retVal);

		if (init->tx.txProfile.txFir.numFirCoefs > 0) {
			retVal = (talRecoveryActions_t)TALISE_programFir(device, TAL_TX1TX2_FIR,
					&init->tx.txProfile.txFir);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	if (((device->devStateInfo.profilesValid & RX_PROFILE_VALID) > 0) ||
	    ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0)) {
		retVal = (talRecoveryActions_t)TALISE_setRxPfirSyncClk(device,
				&init->rx.rxProfile, &init->obsRx.orxProfile);
		IF_ERR_RETURN_U32(retVal);
	}

	if (device->devStateInfo.profilesValid & RX_PROFILE_VALID) {
		if (init->rx.rxProfile.rxFir.numFirCoefs > 0) {
			retVal = (talRecoveryActions_t)TALISE_programFir(device, TAL_RX1RX2_FIR,
					&init->rx.rxProfile.rxFir);
			IF_ERR_RETURN_U32(retVal);

			/* Load Rx gain table */
			retVal = (talRecoveryActions_t)TALISE_programRxGainTable(device,
					&rxGainTable[0], (sizeof(rxGainTable) / sizeof(taliseRxGainTable_t)),
					TAL_RX1RX2);
			IF_ERR_RETURN_U32(retVal);

			retVal = (talRecoveryActions_t)TALISE_setRxManualGain(device, TAL_RX1,
					init->rx.rxGainCtrl.rx1GainIndex);
			IF_ERR_RETURN_U32(retVal);

			retVal = (talRecoveryActions_t)TALISE_setRxManualGain(device, TAL_RX2,
					init->rx.rxGainCtrl.rx2GainIndex);
			IF_ERR_RETURN_U32(retVal);

			/* Enable Digital gain for Rx and ObsRx gain tables */
			halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DIGITAL_GAIN_CONFIG,
						   0x80);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	if (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) {
		if (init->obsRx.orxProfile.rxFir.numFirCoefs > 0) {
			/* if pointer to orx rxFIR is valid */
			retVal = (talRecoveryActions_t)TALISE_programFir(device,
					TAL_OBSRX1RX2_LOOPBACK_FIR, &init->obsRx.orxProfile.rxFir);
			IF_ERR_RETURN_U32(retVal);

			/* Load ORx gain table */
			retVal = (talRecoveryActions_t)TALISE_programOrxGainTable(device,
					&orxGainTable[0], (sizeof(orxGainTable) / sizeof(taliseOrxGainTable_t)),
					TAL_ORX1ORX2);
			IF_ERR_RETURN_U32(retVal);

			retVal = (talRecoveryActions_t)TALISE_setObsRxManualGain(device, TAL_ORX1,
					init->obsRx.orxGainCtrl.orx1GainIndex);
			IF_ERR_RETURN_U32(retVal);

			retVal = (talRecoveryActions_t)TALISE_setObsRxManualGain(device, TAL_ORX2,
					init->obsRx.orxGainCtrl.orx2GainIndex);
			IF_ERR_RETURN_U32(retVal);

			/* Enable Digital gain for Rx and ObsRx gain tables */
			halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DIGITAL_GAIN_CONFIG,
						   0x80);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RCAL_CONTROL,
				   0x07); /* Set RCAL code to nominal */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* If Valid Rx Profile or valid ObsRx profile, setup serializers */
	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) ||
	    (device->devStateInfo.profilesValid & ORX_PROFILE_VALID)) {
		retVal = (talRecoveryActions_t)TALISE_setupSerializers(device, init);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) &&
	    (init->rx.rxChannels != TAL_RXOFF)) {

		/* save the Np states for use in slicer gain compensation management */
		if ((init->rx.framerSel == TAL_FRAMER_A) ||
		    (init->rx.framerSel == TAL_FRAMER_A_AND_B)) {
			device->devStateInfo.rxFramerNp = init->jesd204Settings.framerA.Np;
		} else {
			device->devStateInfo.rxFramerNp = init->jesd204Settings.framerB.Np;
		}

		retVal = (talRecoveryActions_t)TALISE_setupJesd204bFramer(device, init,
				init->rx.framerSel);
		IF_ERR_RETURN_U32(retVal);

		/* set the ADC crossbar: Q/I data assigned to even/odd converters to account for I/Q swap in DUT */
		if (init->rx.rxChannels == TAL_RX2) {
			/* In real IF mode, send Rx2 I data to converter 0 */
			if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
				adcXbar.conv0 = TAL_ADC_RX2_I;
				adcXbar.conv1 = TAL_ADC_RX2_Q;
				adcXbar.conv2 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
				adcXbar.conv3 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
				adcXbar.conv4 = TAL_ADC_RX1_I;
				adcXbar.conv5 = TAL_ADC_RX1_Q;
				adcXbar.conv6 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
				adcXbar.conv7 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
			} else {
				adcXbar.conv0 = TAL_ADC_RX2_Q;
				adcXbar.conv1 = TAL_ADC_RX2_I;
				adcXbar.conv2 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
				adcXbar.conv3 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
				adcXbar.conv4 = TAL_ADC_RX1_Q;
				adcXbar.conv5 = TAL_ADC_RX1_I;
				adcXbar.conv6 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
				adcXbar.conv7 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
			}
		} else if (init->rx.rxChannels == TAL_RX1) {
			/* In real IF mode, send Rx1 I data to converter 0 */
			if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
				adcXbar.conv0 = TAL_ADC_RX1_I;
				adcXbar.conv1 = TAL_ADC_RX1_Q;
				adcXbar.conv2 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
				adcXbar.conv3 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
				adcXbar.conv4 = TAL_ADC_RX2_I;
				adcXbar.conv5 = TAL_ADC_RX2_Q;
				adcXbar.conv6 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
				adcXbar.conv7 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
			} else {
				adcXbar.conv0 = TAL_ADC_RX1_Q;
				adcXbar.conv1 = TAL_ADC_RX1_I;
				adcXbar.conv2 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
				adcXbar.conv3 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
				adcXbar.conv4 = TAL_ADC_RX2_Q;
				adcXbar.conv5 = TAL_ADC_RX2_I;
				adcXbar.conv6 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
				adcXbar.conv7 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
			}
		} else {
			/* In real IF mode, send Rx1 I data to converter 0, Rx2 I data on converter 1 */
			if ((init->rx.rxProfile.rxDdcMode == TAL_RXDDC_INT2_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_DEC2_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_FILTERONLY_REALIF) ||
			    (init->rx.rxProfile.rxDdcMode == TAL_RXDDC_BYPASS_REALIF)) {
				adcXbar.conv0 = TAL_ADC_RX1_I;
				adcXbar.conv1 = TAL_ADC_RX2_I;
				adcXbar.conv2 = TAL_ADC_RX1_Q;
				adcXbar.conv3 = TAL_ADC_RX2_Q;
				adcXbar.conv4 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
				adcXbar.conv5 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
				adcXbar.conv6 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
				adcXbar.conv7 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
			} else {
				adcXbar.conv0 = TAL_ADC_RX1_Q;
				adcXbar.conv1 = TAL_ADC_RX1_I;
				adcXbar.conv2 = TAL_ADC_RX2_Q;
				adcXbar.conv3 = TAL_ADC_RX2_I;
				adcXbar.conv4 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
				adcXbar.conv5 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
				adcXbar.conv6 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
				adcXbar.conv7 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
			}
		}
		retVal = (talRecoveryActions_t)TALISE_setupAdcSampleXbar(device,
				init->rx.framerSel, adcXbar);
		IF_ERR_RETURN_U32(retVal);

		/* Initialize DDC modes */
		retVal = talInitializeDdc(device, init);
		IF_ERR_RETURN(retVal);
	}

	if (device->devStateInfo.profilesValid & ORX_PROFILE_VALID) {
		/* save the Np states for use in slicer gain compensation management */
		if ((init->obsRx.framerSel == TAL_FRAMER_A) ||
		    (init->obsRx.framerSel == TAL_FRAMER_A_AND_B)) {
			/* Only Setup JESD Framer if serializer Lanes are enabled for Framer A ORx path */
			if (init->jesd204Settings.framerA.serializerLanesEnabled > 0) {
				device->devStateInfo.orxFramerNp = init->jesd204Settings.framerA.Np;
				retVal = (talRecoveryActions_t)TALISE_setupJesd204bFramer(device, init,
						init->obsRx.framerSel);
				IF_ERR_RETURN_U32(retVal);
			}
		} else {
			/* Only Setup JESD Framer if serializer Lanes are enabled for Framer B ORx path */
			if (init->jesd204Settings.framerB.serializerLanesEnabled > 0) {
				device->devStateInfo.orxFramerNp = init->jesd204Settings.framerB.Np;
				retVal = (talRecoveryActions_t)TALISE_setupJesd204bFramer(device, init,
						init->obsRx.framerSel);
				IF_ERR_RETURN_U32(retVal);
			}
		}

		/* set the ADC crossbar: Q/I data assigned to even/odd converters to account for I/Q swap in DUT */
		if ((device->devStateInfo.orxAdcStitchingEnabled == 0 )
		    && (init->obsRx.obsRxChannelsEnable == TAL_ORX2)) {
			adcXbar.conv0 = TAL_ADC_RX2_Q;
			adcXbar.conv1 = TAL_ADC_RX2_I;
			adcXbar.conv2 = TAL_ADC_RX1_Q;
			adcXbar.conv3 = TAL_ADC_RX1_I;
			adcXbar.conv4 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
			adcXbar.conv5 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
			adcXbar.conv6 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
			adcXbar.conv7 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
		} else {
			adcXbar.conv0 = TAL_ADC_RX1_Q;
			adcXbar.conv1 = TAL_ADC_RX1_I;
			adcXbar.conv2 = TAL_ADC_RX2_Q;
			adcXbar.conv3 = TAL_ADC_RX2_I;
			adcXbar.conv4 = TAL_ADC_DUALBAND_RX1_BAND_B_Q;
			adcXbar.conv5 = TAL_ADC_DUALBAND_RX1_BAND_B_I;
			adcXbar.conv6 = TAL_ADC_DUALBAND_RX2_BAND_B_Q;
			adcXbar.conv7 = TAL_ADC_DUALBAND_RX2_BAND_B_I;
		}
		retVal = (talRecoveryActions_t)TALISE_setupAdcSampleXbar(device,
				init->obsRx.framerSel, adcXbar);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) &&
	    (init->tx.txChannels != TAL_TXOFF)) {
		retVal = (talRecoveryActions_t)TALISE_setupDeserializers(device, init);
		IF_ERR_RETURN_U32(retVal);

		/* set the DAC sample crossbars */
		if (init->tx.deframerSel == TAL_DEFRAMER_A) {
			if ((init->tx.txChannels == TAL_TX1) ||
			    (init->tx.txChannels == TAL_TX1TX2)) {
				dacXbar.dacChanI = TAL_DEFRAMERA_OUT0;
				dacXbar.dacChanQ = TAL_DEFRAMERA_OUT1;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX1,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);

				dacXbar.dacChanI = TAL_DEFRAMERA_OUT2;
				dacXbar.dacChanQ = TAL_DEFRAMERA_OUT3;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX2,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);

			}

			if (init->tx.txChannels == TAL_TX2) {
				dacXbar.dacChanI = TAL_DEFRAMERA_OUT2;
				dacXbar.dacChanQ = TAL_DEFRAMERA_OUT3;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX1,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);

				dacXbar.dacChanI = TAL_DEFRAMERA_OUT0;
				dacXbar.dacChanQ = TAL_DEFRAMERA_OUT1;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX2,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);
			}
		}

		if (init->tx.deframerSel == TAL_DEFRAMER_B) {
			if ((init->tx.txChannels == TAL_TX1) ||
			    (init->tx.txChannels == TAL_TX1TX2)) {
				dacXbar.dacChanI = TAL_DEFRAMERB_OUT0;
				dacXbar.dacChanQ = TAL_DEFRAMERB_OUT1;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX1,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);

				dacXbar.dacChanI = TAL_DEFRAMERB_OUT2;
				dacXbar.dacChanQ = TAL_DEFRAMERB_OUT3;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX2,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);
			}

			if (init->tx.txChannels == TAL_TX2) {
				dacXbar.dacChanI = TAL_DEFRAMERB_OUT2;
				dacXbar.dacChanQ = TAL_DEFRAMERB_OUT3;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX1,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);

				dacXbar.dacChanI = TAL_DEFRAMERB_OUT0;
				dacXbar.dacChanQ = TAL_DEFRAMERB_OUT1;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX2,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);
			}
		}

		if (init->tx.deframerSel == TAL_DEFRAMER_A_AND_B) {
			if ((init->jesd204Settings.deframerA.M == 4) ||
			    (init->jesd204Settings.deframerB.M == 4)) {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
								  TAL_ERR_FRAMER_A_AND_B_INV_M_PARM, retVal, TALACT_ERR_CHECK_PARAM);
			} else if ((init->tx.txChannels == TAL_TX1TX2) &&
				   (init->jesd204Settings.deframerA.M == 2) &&
				   (init->jesd204Settings.deframerB.M == 2)) {
				dacXbar.dacChanI = TAL_DEFRAMERA_OUT0;
				dacXbar.dacChanQ = TAL_DEFRAMERA_OUT1;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX1,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);

				dacXbar.dacChanI = TAL_DEFRAMERB_OUT0;
				dacXbar.dacChanQ = TAL_DEFRAMERB_OUT1;
				retVal = (talRecoveryActions_t)TALISE_setupDacSampleXbar(device, TAL_TX2,
						dacXbar);
				IF_ERR_RETURN_U32(retVal);
			}
		}

		if ((init->tx.deframerSel == TAL_DEFRAMER_A) ||
		    (init->tx.deframerSel == TAL_DEFRAMER_A_AND_B)) {
			retVal = (talRecoveryActions_t)TALISE_setupJesd204bDeframer(device, init,
					TAL_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);
		}

		if ((init->tx.deframerSel == TAL_DEFRAMER_B) ||
		    (init->tx.deframerSel == TAL_DEFRAMER_A_AND_B)) {
			retVal = (talRecoveryActions_t)TALISE_setupJesd204bDeframer(device, init,
					TAL_DEFRAMER_B);
			IF_ERR_RETURN_U32(retVal);
		}

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GAIN_VALID_OVERRIDES,
					   0x88);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		retVal = (talRecoveryActions_t)TALISE_setTxAttenuation(device, TAL_TX1,
				init->tx.tx1Atten_mdB);
		IF_ERR_RETURN_U32(retVal);

		retVal = (talRecoveryActions_t)TALISE_setTxAttenuation(device, TAL_TX2,
				init->tx.tx2Atten_mdB);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Set RxFE LO Common mode */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RXFE1_LOCM, 0x30);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_RXFE2_LOCM, 0x30);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Setup thresholds needed by ARM calibrations */
	/* ULB threshold is set to 3db below full scale of ADC */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_UPPER_LEVEL_BLOCKER_THRESHOLD, ULB_THRESHOLD);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* ULB2 is 3db above ULB threshold */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_UPPER_LEVEL_BLOCKER_THRESHOLD2, ULB2_THRESHOLD);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* LLB is 3db below ULB threshold */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_LOWER_LEVEL_BLOCKER_THRESHOLD, LLB_THRESHOLD);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* LLB2 is 3db above LLB threshold */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_LOWER_LEVEL_BLOCKER_THRESHOLD2, LLB2_THRESHOLD);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ULB_COUNT_THRESHOLD,
				   0x06);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_DEC_OVERLOAD_CONFIG1,
				   0x91);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_DEC_OVERLOAD_UPPER_THRESHOLD, 0xB5);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_ADC_HIGH_OVRG_COUNT_THRESHOLD, 0x06);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_AGC_CONFIG1, 0x04,
				    0x3F, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_DEC_OVERLOAD_CONFIG2, 0x03, 0x03, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set gain update counter for 1ms. */
	/* Calculate AGC clock rate - Use same equation as used for txAttenClock_Hz in TALISE_initDigitalClocks*/
	if (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz > 500000000) {
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 4); /* div 4 */
	} else if (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz > 250000000) {
		agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 2); /* div 2 */
	} else {
		agcClock_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz; /* div 1 */
	}

	gainUpdateCount = (agcClock_Hz / 1000);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER1,
				   (gainUpdateCount & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER2,
				   ((gainUpdateCount >> 8) & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GAIN_UPDATE_COUNTER3,
				   ((gainUpdateCount >> 16) & 0x3F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_SLOWLOOP_SETTLING_DELAY, 0x10);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SLOWLOOP_CONFIG,
				   0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_AGC_UNDERRANGE1,
				   0x02);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX1, 1, 0x02, 1);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_AGC_MANUAL_GAIN_CONTROL_CONFIG_RX2, 1, 0x02, 1);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Options to disable Tx data if PLL unlocks */
	if ((init->tx.disTxDataIfPllUnlock != TAL_TXDIS_TX_NOT_DISABLED) &&
	    (init->tx.disTxDataIfPllUnlock != TAL_TXDIS_TX_ZERO_DATA) &&
	    (init->tx.disTxDataIfPllUnlock != TAL_TXDIS_TX_RAMP_DOWN_TO_ZERO)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERTXPFILE_INV_DISONPLLUNLOCK, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_DP_MASK_CH1,
				    (uint8_t)init->tx.disTxDataIfPllUnlock, 0x06, 1);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_TX_DP_MASK_CH2,
				    (uint8_t)init->tx.disTxDataIfPllUnlock, 0x06, 1);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Talise API state container information */
	device->devStateInfo.devState = (taliseStates_t)(device->devStateInfo.devState |
					TAL_STATE_INITIALIZED);
	device->devStateInfo.initializedChannels = (init->rx.rxChannels & 3);
	device->devStateInfo.initializedChannels |= ((init->obsRx.obsRxChannelsEnable &
			3) << 2);
	device->devStateInfo.initializedChannels |= ((init->tx.txChannels & 3) << 6);
	device->devStateInfo.txInputRate_kHz = init->tx.txProfile.txInputRate_kHz;
	device->devStateInfo.rxDdcMode = init->rx.rxProfile.rxDdcMode;

	/* Find the Rx Framer and save the Rx Framer M value */
	if (init->rx.framerSel == TAL_FRAMER_A) {
		device->devStateInfo.rxTotalM = init->jesd204Settings.framerA.M;
	} else if (init->rx.framerSel == TAL_FRAMER_B) {
		device->devStateInfo.rxTotalM = init->jesd204Settings.framerB.M;
	} else if (init->rx.framerSel == TAL_FRAMER_A_AND_B) {
		device->devStateInfo.rxTotalM = init->jesd204Settings.framerA.M +
						init->jesd204Settings.framerB.M;
	}

	device->devStateInfo.rxBandwidth_Hz = init->rx.rxProfile.rfBandwidth_Hz;
	device->devStateInfo.txBandwidth_Hz = init->tx.txProfile.rfBandwidth_Hz;
	device->devStateInfo.orxBandwidth_Hz = init->obsRx.orxProfile.rfBandwidth_Hz;

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = (talRecoveryActions_t)retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_shutdown(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	retVal = (talRecoveryActions_t)TALISE_resetDevice(device);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getMultiChipSyncStatus(taliseDevice_t *device,
				       uint8_t *mcsStatus)
{
	talRecoveryActions_t retVal   = TALACT_NO_ACTION;
	adiHalErr_t          halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getMultiChipSyncStatus()\n");

	retVal   = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				    TALACT_WARN_RESET_LOG);
#endif

	/* null pointer check */
	if (mcsStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_CHECKGETMCS_STATUS_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MCS_STATUS,
					  mcsStatus);
		retVal   = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					    TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_enableMultichipSync(taliseDevice_t *device, uint8_t enableMcs,
				    uint8_t *mcsStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t clkPllSdmBypass = 0;
	uint8_t mcsEnable =
		0x9B; /* [7] Keeps RF LO divider enabled, Enable MCS[4] and reset Device Clock divider[3], Digital clocks[1], and JESD204 SYSREF[0] */

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableMultichipSync()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (enableMcs > 0) {
		/* If CLKPLL SDM not bypassed, reset CLKPLL SDM as well. */
		halError = ADIHAL_spiReadField(device->devHalInfo,
					       TALISE_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE1, &clkPllSdmBypass, 0x40, 6);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		if (clkPllSdmBypass == 0) {
			mcsEnable |= 0x04; /* enable MCS for CLKPLL SDM */
		}

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MCS_CONTROL,
					   mcsEnable);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Prime the K1 input sync (clear bit 4, then set bit 4) */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_MCS_CONTROL_2, 0,
					    0x10, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_MCS_CONTROL_2, 1,
					    0x10, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		retVal = (talRecoveryActions_t)TALISE_serializerReset(device);
		IF_ERR_RETURN_U32(retVal);
	}

	/* if mcsStatus is a valid pointer, return the MCS status */
	if (mcsStatus != NULL) {
		halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MCS_STATUS,
					  mcsStatus);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_enableMultichipRfLOPhaseSync(taliseDevice_t *device,
		uint8_t enableDigTestClk)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t mcsEnable =
		0x98; /* [7] Keeps RF LO divider enabled, Enable MCS[4] and reset Device Clock divider[3] */

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableMultichipRfLOPhaseSync()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (enableDigTestClk > 0) {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MCS_CONTROL,
					   mcsEnable);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Prime the K1 input sync (clear bit 4, then set bit 4) */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_MCS_CONTROL_2, 0,
					    0x10, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_MCS_CONTROL_2, 1,
					    0x10, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Enable Digital Test Clock  */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DIGITAL_TEST_BYTE,
					    1, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		/* Disable Digital Test Clock */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DIGITAL_TEST_BYTE,
					    0, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		mcsEnable =
			0x93; /* [7] Keeps RF LO divider enabled, Enable MCS[4] ,reset Digital clocks[1] and JESD204 SYSREF[0] */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MCS_CONTROL,
					   mcsEnable);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_serializerReset(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_SerializerReset()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* reset JESD204 serializer */
	/* Toggle Serializer SRESET */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_JESD_FRAMER_SRST_CFG,
				   0xFF);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Allow framer data to output to serializer (clear reset)*/
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_JESD_FRAMER_SRST_CFG,
				   0x0F);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_programFir(taliseDevice_t *device,
			   talisefirName_t filterToProgram, taliseFir_t *firFilter)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t firBankSel = 0;
	uint8_t i = 0;
	uint8_t numTapsReg = 0;
	uint8_t nTapMul = 24;
	uint8_t maxNumTaps = 80;
	uint8_t filterGain = 0;

	uint32_t addrIndex = 0;
	uint32_t spiBufferSize = ((HAL_SPIWRITEARRAY_BUFFERSIZE / 6) *
				  6); /* Make buffer size a multiple of 6 */
	uint16_t addrArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = {0};
	uint8_t dataArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = {0};

	static const uint8_t COEF_WRITE_EN = 0x40;
	static const uint8_t PROGRAM_CLK_EN = 0x80;
	static const uint8_t TXFIR_BANKA = 0x01;
	static const uint8_t TXFIR_BANKB = 0x02;
	static const uint8_t RXFIR_BANKA = 0x04;
	static const uint8_t RXFIR_BANKB = 0x08;
	static const uint8_t RXFIR_BANKC = 0x10;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_programFir()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Error checking, #taps less than max number of taps allowed.*/
	if (firFilter->numFirCoefs == 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_PROGRAMFIR_COEFS_NULL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* firBankSelect and assignment of maxNumTaps, nTapMul and address firAddr.*/
	switch(filterToProgram) {
	case TAL_TX1_FIR:
		maxNumTaps = 80;
		firBankSel = TXFIR_BANKA;
		nTapMul = 20;
		break;

	case TAL_TX2_FIR:
		maxNumTaps = 80;
		firBankSel = TXFIR_BANKB;
		nTapMul = 20;
		break;

	case TAL_TX1TX2_FIR:
		maxNumTaps = 80;
		firBankSel = (TXFIR_BANKA | TXFIR_BANKB);
		nTapMul = 20;
		break;

	case TAL_RX1_FIR: /*check for valid cases and shares the exact same code as case RX1RX2_FIR  */
	case TAL_RX2_FIR: /*check for valid cases and shares the exact same code as case RX1RX2_FIR  */
	case TAL_RX1RX2_FIR:
		maxNumTaps = 72;
		firBankSel = RXFIR_BANKA;
		nTapMul = 24;
		break;

	case TAL_OBSRX1_FIR: /*check for valid cases and shares the exact same code as case OBSRX1RX2_FIR  */
	case TAL_OBSRX2_FIR: /*check for valid cases and shares the exact same code as case OBSRX1RX2_FIR  */
	case TAL_OBSRX1RX2_FIR:
		maxNumTaps = 72;
		firBankSel = RXFIR_BANKB;
		nTapMul = 24;
		break;

	case TAL_LOOPBACK_FIR:
		maxNumTaps = 72;
		firBankSel = RXFIR_BANKC;
		nTapMul = 24;
		break;

	case TAL_RX1RX2_LOOPBACK_FIR:
		maxNumTaps = 72;
		firBankSel = (RXFIR_BANKA | RXFIR_BANKC);
		nTapMul = 24;
		break;

	case TAL_OBSRX1RX2_LOOPBACK_FIR:
		maxNumTaps = 72;
		firBankSel = (RXFIR_BANKB | RXFIR_BANKC);
		nTapMul = 24;
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_PROGRAMFIR_INV_FIRNAME_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Verification for the structure numFirCoefs */
	if (firFilter->numFirCoefs > maxNumTaps) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_PROGRAMFIR_INV_NUMTAPS_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Calculate register value for number of Taps */
	if (((firFilter->numFirCoefs % nTapMul) == 0) &&
	    (firFilter->numFirCoefs > 0)) {
		numTapsReg = (firFilter->numFirCoefs / nTapMul) - 1;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_PROGRAMFIR_INV_NUMTAPS_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Select which FIR filter to program coeffs for and enable the programming clock*/
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_PFIR_COEFF_CTL,
				   (PROGRAM_CLK_EN | firBankSel));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* write filter coefficients */
	addrIndex = 0;
	for (i = 0; i < firFilter->numFirCoefs; i++) {
		addrArray[addrIndex] = TALISE_ADDR_PFIR_COEFF_ADDR;
		dataArray[addrIndex++] = (i * 2);

		addrArray[addrIndex] = TALISE_ADDR_PFIR_COEFF_DATA;
		dataArray[addrIndex++] = (firFilter->coefs[i] & 0xFF);

		addrArray[addrIndex] = TALISE_ADDR_PFIR_COEFF_CTL;
		dataArray[addrIndex++] = (PROGRAM_CLK_EN | COEF_WRITE_EN | firBankSel);

		addrArray[addrIndex] = TALISE_ADDR_PFIR_COEFF_ADDR;
		dataArray[addrIndex++] = ((i * 2) + 1);

		addrArray[addrIndex] = TALISE_ADDR_PFIR_COEFF_DATA;
		dataArray[addrIndex++] = ((firFilter->coefs[i] >> 8) & 0xFF);

		addrArray[addrIndex] = TALISE_ADDR_PFIR_COEFF_CTL;
		dataArray[addrIndex++] = (PROGRAM_CLK_EN | COEF_WRITE_EN | firBankSel);

		/* Send full buffer size when possible */
		/* spiBufferSize set to multiple of 6 at top of function */
		if (addrIndex >= spiBufferSize) {
			halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &dataArray[0],
						    addrIndex);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			addrIndex = 0;
		}
	}

	if (addrIndex > 0) {
		halError = talSpiWriteBytes(device->devHalInfo, &addrArray[0], &dataArray[0],
					    addrIndex);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* clear Program clock enable */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_PFIR_COEFF_CTL,
				   firBankSel);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set filter gain to be used */
	switch (firFilter->gain_dB) {
	case TAL_FIR_GAIN_NEG12_DB:
		filterGain = 0x00;
		break;
	case TAL_FIR_GAIN_NEG6_DB:
		filterGain = 0x01;
		break;
	case TAL_FIR_GAIN_0_DB:
		filterGain = 0x02;
		break;
	case TAL_FIR_GAIN_POS6_DB:
		filterGain = 0x03;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RXFIR_INV_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* write filter gain and #taps if TX, else is RX */
	if ((filterToProgram == TAL_TX1_FIR) || (filterToProgram == TAL_TX2_FIR)
	    || (filterToProgram == TAL_TX1TX2_FIR)) {
		/* Write number of Taps , Tx Filter gain*/
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_TX_FILTER_CONFIGURATION,
					   (((numTapsReg & 0x3) << 5) | (filterGain & 0x3)));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		if ((firBankSel & RXFIR_BANKA) == RXFIR_BANKA) {
			/* Write FIR #taps and gain */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_RX_PFIR_BANK_A_SETTINGS,
						    (((numTapsReg & 0x3) << 4) | ((filterGain & 0x03) << 2)), 0x3C, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}

		if ((firBankSel & RXFIR_BANKB) == RXFIR_BANKB) {
			/* Write FIR #taps and gain */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_RX_PFIR_BANK_B_SETTINGS,
						    (((numTapsReg & 0x3) << 4) | ((filterGain & 0x03) << 2)), 0x3C, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}

		if ((firBankSel & RXFIR_BANKC) == RXFIR_BANKC) {
			/* Write FIR #taps and gain */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_RX_PFIR_BANK_C_SETTINGS,
						    (((numTapsReg & 0x3) << 4) | ((filterGain & 0x03) << 2)), 0x3C, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* Set Mode mapping for RX, OBSRX or LOOPBACK */
	if ((filterToProgram == TAL_RX1_FIR) || (filterToProgram == TAL_RX2_FIR) ||
	    (filterToProgram == TAL_RX1RX2_FIR)
	    || (filterToProgram == TAL_RX1RX2_LOOPBACK_FIR)) {
		if (filterToProgram == TAL_RX1RX2_LOOPBACK_FIR) {
			/* Set Mode mapping for RX1 Rx2 and Loopback FIR Bank A */
			halError = talSpiWriteByte(device->devHalInfo,
						   TALISE_ADDR_RX_PFIR_MODE_MAPPING1, 0x00);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		} else {
			/* Set Mode mapping for RX1 Rx2 FIR Bank A */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_RX_PFIR_MODE_MAPPING1, 0x00, 0x0F, 0);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	} else if ((filterToProgram == TAL_OBSRX1_FIR)
		   || (filterToProgram == TAL_OBSRX2_FIR) ||
		   (filterToProgram == TAL_OBSRX1RX2_FIR)
		   || (filterToProgram == TAL_OBSRX1RX2_LOOPBACK_FIR)) {
		if (filterToProgram == TAL_OBSRX1RX2_LOOPBACK_FIR) {
			/* Set Mode mapping for Loopback FIR Bank B */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_RX_PFIR_MODE_MAPPING1, 0x01, 0xC0, 6);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}

		/* Set Mode mapping for Obs RxA and Obs RxA FIR  Bank B */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_RX_PFIR_MODE_MAPPING2, 0x05);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else if (filterToProgram == TAL_LOOPBACK_FIR) {
		/* Set Mode mapping for Loopback FIR Bank C */
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_RX_PFIR_MODE_MAPPING1, 0x02, 0xC0, 6);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_calculateDigitalClocks(taliseDevice_t *device,
				       taliseDigClocks_t *digClocks)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint8_t hsDivTimes10 = 25;
	uint8_t hsClkDivHsDigClk4or5 = 20;
	uint32_t localHsDigClkDiv2_Hz = 0;
	uint32_t localHsDigClk4or5_Hz = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_calculateDigitalClocks()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	device->devStateInfo.clocks.clkPllVcoFreq_kHz = digClocks->clkPllVcoFreq_kHz;
	device->devStateInfo.clocks.clkPllHsDiv = digClocks->clkPllHsDiv;
	device->devStateInfo.clocks.deviceClock_kHz = digClocks->deviceClock_kHz;

	switch(digClocks->clkPllHsDiv) {
	case TAL_HSDIV_2:
		hsDivTimes10 = 20;
		hsClkDivHsDigClk4or5 = 10;
		break;
	case TAL_HSDIV_2P5:
		hsDivTimes10 = 25;
		hsClkDivHsDigClk4or5 = 10;
		break;
	case TAL_HSDIV_3:
		hsDivTimes10 = 30;
		hsClkDivHsDigClk4or5 = 15;
		break;
	case TAL_HSDIV_4:
		hsDivTimes10 = 40;
		hsClkDivHsDigClk4or5 = 20;
		break;
	case TAL_HSDIV_5:
		hsDivTimes10 = 50;
		hsClkDivHsDigClk4or5 = 20;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_CLKPLL_INV_HSDIV, retVal, TALACT_ERR_CHECK_PARAM );
	}

	localHsDigClkDiv2_Hz = (uint32_t)((DIV_U64((((uint64_t)(
			digClocks->clkPllVcoFreq_kHz) * 1000) >> 1), hsDivTimes10) * 10) >> 1);

	device->devStateInfo.clocks.hsDigClkDiv2_Hz = localHsDigClkDiv2_Hz;

	localHsDigClk4or5_Hz = (uint32_t)DIV_U64((((uint64_t)(
				       digClocks->clkPllVcoFreq_kHz) * 1000) >> 1), hsClkDivHsDigClk4or5);

	device->devStateInfo.clocks.hsDigClkDiv4or5_Hz = localHsDigClk4or5_Hz;

	return (uint32_t)retVal;
}

/**
 * \brief Verifies the Rx profile members are valid (in range) and calculates HS Dig Clock require for the Rx Profile
 *
 * Private helper function to verify the Rx profile members are valid (in range)
 * and calculates HS Dig Clock require for the Rx Profile
 * If the Rx profile IQ data rate = 0, it is assumed that the Rx profile is
 * not used.  If Rx IQ data rate > 0, and Rx profile members are out of range,
 * the function stores the error code describing the error, and returns a
 * Recovery action to check the configuration settings.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to Talise device data structure
 * \param rxProfile rxProfile settings to be verified
 * \param rxHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Rx profile
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
static talRecoveryActions_t talVerifyRxProfile(taliseDevice_t *device,
		taliseRxProfile_t *rxProfile, uint32_t *rxHsDigClk_kHz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t ddcMultiply = 1;
	uint8_t ddcDivide = 1;

	*rxHsDigClk_kHz = 0;

	/********************************/
	/* Check for a valid Rx profile */
	/********************************/
	if ((rxProfile->rxOutputRate_kHz < 15000) ||
	    (rxProfile->rxOutputRate_kHz > 370000)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERRXPFILE_INV_IQRATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((rxProfile->rfBandwidth_Hz < 15000000) ||
	    (rxProfile->rfBandwidth_Hz > 250000000)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERRXPFILE_INV_RFBW, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((rxProfile->rhb1Decimation != 1) &&
	    (rxProfile->rhb1Decimation != 2)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERRXPFILE_INV_RHB1, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((rxProfile->rxDec5Decimation != 4) &&
	    (rxProfile->rxDec5Decimation != 5)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERRXPFILE_INV_DEC5, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((rxProfile->rxFirDecimation != 1) &&
	    (rxProfile->rxFirDecimation != 2) &&
	    (rxProfile->rxFirDecimation != 4)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERRXPFILE_INV_FIR, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((rxProfile->rxFir.coefs == NULL) &&
	    (rxProfile->rxFirDecimation != 1)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERRXPFILE_INV_COEF, retVal, TALACT_ERR_CHECK_PARAM);
	}

	switch(rxProfile->rxDdcMode) {
	case TAL_RXDDC_BYPASS_REALIF:
	case TAL_RXDDC_BYPASS:
		ddcMultiply = 1;
		ddcDivide = 1;
		break;
	case TAL_RXDDC_DEC2_REALIF:
	case TAL_RXDDC_DEC2:
		ddcMultiply = 2;
		ddcDivide = 1;
		break;
	case TAL_RXDDC_INT2_REALIF:
	case TAL_RXDDC_INT2:
		ddcMultiply = 1;
		ddcDivide = 2;
		break;
	case TAL_RXDDC_FILTERONLY_REALIF:
	case TAL_RXDDC_FILTERONLY:
		ddcMultiply = 1;
		ddcDivide = 1;
		break;
	default:
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERRXPFILE_INV_DDC, retVal, TALACT_ERR_CHECK_PARAM );
	}

	*rxHsDigClk_kHz = ( rxProfile->rxOutputRate_kHz * rxProfile->rxFirDecimation *
			    rxProfile->rhb1Decimation * rxProfile->rxDec5Decimation * ddcMultiply ) /
			  ddcDivide;

	return retVal;
}

/**
 * \brief Verifies the ORx profile members are valid (in range) in the init structure
 *
 * If the ORx profile IQ data rate = 0, it is assumed that the ORx profile is
 * not used.  If ORx IQ data rate > 0, and ORx profile members are out of range,
 * the function stores the error code describing the error, and returns a
 * Recovery action to check the configuration settings.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to Talise device data structure
 * \param orxProfile orxProfile settings to be verified
 * \param orxHsDigClk_kHz Return value of the calculated HS Dig Clock required by the ORx profile
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
static talRecoveryActions_t talVerifyOrxProfile(taliseDevice_t *device,
		taliseORxProfile_t *orxProfile, uint32_t *orxHsDigClk_kHz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t ddcMultiply = 1;
	uint8_t ddcDivide = 1;
	int32_t sumMergeFilter = 0;
	uint8_t indexMF = 0;

	*orxHsDigClk_kHz = 0;

	/********************************/
	/* Check for a valid ORx profile */
	/********************************/
	if ((orxProfile->orxOutputRate_kHz < 15000) ||
	    (orxProfile->orxOutputRate_kHz > 500000)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERORXPFILE_INV_IQRATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((orxProfile->rfBandwidth_Hz < 15000000) ||
	    (orxProfile->rfBandwidth_Hz > 460000000)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERORXPFILE_INV_RFBW, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if(orxProfile->rfBandwidth_Hz > 200000000) {
		// Disable stitching for bandwidth above 200 MHz,if all MergeFilter values are zero
		for(indexMF = 0; indexMF < 12; ++indexMF) {
			sumMergeFilter += orxProfile->orxMergeFilter[indexMF];
		}

		device->devStateInfo.orxAdcStitchingEnabled = (sumMergeFilter == 0) ? 0 : 1;
	} else {
		device->devStateInfo.orxAdcStitchingEnabled = 0;
	}

	if ((orxProfile->rhb1Decimation != 1) &&
	    (orxProfile->rhb1Decimation != 2)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERORXPFILE_INV_RHB1, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((orxProfile->rxDec5Decimation != 4) &&
	    (orxProfile->rxDec5Decimation != 5)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERORXPFILE_INV_DEC5, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((orxProfile->rxFirDecimation != 1) &&
	    (orxProfile->rxFirDecimation != 2) &&
	    (orxProfile->rxFirDecimation != 4)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERORXPFILE_INV_FIR, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((orxProfile->rxFir.coefs == NULL) &&
	    (orxProfile->rxFirDecimation != 1)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERORXPFILE_INV_COEF, retVal, TALACT_ERR_CHECK_PARAM);
	}

	switch(orxProfile->orxDdcMode) {
	case TAL_ORXDDC_DISABLED:
		ddcMultiply = 1;
		ddcDivide = 1;
		break;
	case TAL_ORXDDC_SUBSAMPLE_BY2:
		ddcMultiply = 2;
		ddcDivide = 1;
		break;
	default:
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERORXPFILE_INV_DDC, retVal, TALACT_ERR_CHECK_PARAM );
	}

	*orxHsDigClk_kHz = (orxProfile->orxOutputRate_kHz * orxProfile->rxFirDecimation
			    *
			    orxProfile->rhb1Decimation * orxProfile->rxDec5Decimation * ddcMultiply) /
			   ddcDivide;

	return retVal;
}

/**
 * \brief Verifies the Tx profile members are valid (in range) in the init structure
 *
 * If the Tx profile IQ data rate = 0, it is assumed that the Tx profile is
 * not used.  If Tx IQ data rate > 0, and Tx profile members are out of range,
 * the function stores the error code describing the error, and returns a
 * Recovery action to check the configuration settings.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to Talise device data structure
 * \param txProfile txProfile settings to be verified
 * \param txHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Tx profile
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
static talRecoveryActions_t talVerifyTxProfile(taliseDevice_t *device,
		taliseTxProfile_t *txProfile, uint32_t *txHsDigClk_kHz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	*txHsDigClk_kHz = 0;
	/********************************/
	/* Check for a valid Tx profile */
	/********************************/

	if ((txProfile->txInputRate_kHz < 15000) ||
	    (txProfile->txInputRate_kHz > 500000)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_IQRATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((txProfile->rfBandwidth_Hz < 15000000) ||
	    (txProfile->rfBandwidth_Hz > 460000000)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_RFBW, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((txProfile->thb1Interpolation != 1) &&
	    (txProfile->thb1Interpolation != 2)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_THB1, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((txProfile->thb2Interpolation != 1) &&
	    (txProfile->thb2Interpolation != 2)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_THB2, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((txProfile->thb3Interpolation != 1) &&
	    (txProfile->thb3Interpolation != 2)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_THB3, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((txProfile->txInt5Interpolation != 1) &&
	    (txProfile->txInt5Interpolation != 5)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_INT5, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Int5 and thb1/2/3 should be mutually exclusive */
	if (((txProfile->txInt5Interpolation == 5) &&
	     ((txProfile->thb1Interpolation != 1) ||
	      (txProfile->thb2Interpolation != 1) ||
	      (txProfile->thb3Interpolation != 1))) ||
	    ((txProfile->txInt5Interpolation == 1) &&
	     (txProfile->thb1Interpolation == 1) &&
	     (txProfile->thb2Interpolation == 1) &&
	     (txProfile->thb3Interpolation == 1))) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_HBMUTEX, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((txProfile->txFirInterpolation != 1) &&
	    (txProfile->txFirInterpolation != 2) &&
	    (txProfile->txFirInterpolation != 4)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_FIRIPL, retVal, TALACT_ERR_CHECK_PARAM);

	}

	if ((txProfile->txFir.coefs == NULL) &&
	    (txProfile->txFirInterpolation != 1)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_COEF, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if((txProfile->dacDiv != 1) &&
	    (txProfile->dacDiv != 2)) {
		return talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
					TAL_ERR_VERTXPFILE_INV_DACDIV, retVal, TALACT_ERR_CHECK_PARAM);
	}

	*txHsDigClk_kHz = (txProfile->txInputRate_kHz * txProfile->txFirInterpolation *
			   txProfile->thb1Interpolation * txProfile->thb2Interpolation *
			   txProfile->thb3Interpolation * txProfile->txInt5Interpolation *
			   txProfile->dacDiv);

	return retVal;
}

uint32_t TALISE_verifyProfiles(taliseDevice_t *device, taliseInit_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t rxHsDigClk_kHz = 0;
	uint32_t orxHsDigClk_kHz = 0;
	uint32_t txHsDigClk_kHz = 0;
	uint32_t hsDigClkDiv2_Hz = 0;
	uint32_t hsDigClk_kHz = 0;
	taliseRxProfile_t *rxProfile  = NULL;
	taliseTxProfile_t *txProfile = NULL;
	taliseORxProfile_t *orxProfile  = NULL;

	if (device == NULL) {
		/* Can not write to log since log function requires device data structure */
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	if (init == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_NULL_INIT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	device->devStateInfo.profilesValid = 0;

	if(init->tx.txChannels != TAL_TXOFF) {
		txProfile = &init->tx.txProfile;
		retVal = talVerifyTxProfile(device, txProfile, &txHsDigClk_kHz);
		IF_ERR_RETURN_U32(retVal);
	}

	if(init->rx.rxChannels != TAL_RXOFF) {
		rxProfile = &init->rx.rxProfile;
		retVal = talVerifyRxProfile(device, rxProfile, &rxHsDigClk_kHz);
		IF_ERR_RETURN_U32(retVal);
	}

	if((init->obsRx.obsRxChannelsEnable != TAL_ORXOFF)
	    || (init->tx.txChannels != TAL_TXOFF)) {
		orxProfile = &init->obsRx.orxProfile;
		retVal = talVerifyOrxProfile(device, orxProfile, &orxHsDigClk_kHz);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((init->clocks.rfPllPhaseSyncMode != TAL_RFPLLMCS_NOSYNC)
	    && (init->clocks.rfPllPhaseSyncMode != TAL_RFPLLMCS_INIT_AND_SYNC)
	    && (init->clocks.rfPllPhaseSyncMode != TAL_RFPLLMCS_INIT_AND_1TRACK)
	    && (init->clocks.rfPllPhaseSyncMode != TAL_RFPLLMCS_INIT_AND_CONTTRACK)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERPFILE_INV_RFPLLMCSMODE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Verify HsDigClk needed by Rx/Tx/Orx profiles is possible based on CLKPLL
	 * settings
	 */

	hsDigClkDiv2_Hz = device->devStateInfo.clocks.hsDigClkDiv2_Hz;

	hsDigClk_kHz = (hsDigClkDiv2_Hz / 500); /* time 2 and convert from Hz to kHz */

	/* Verify Tx profile is valid */
	if (txHsDigClk_kHz > 0) {
		if ((rxHsDigClk_kHz > 0) && (txHsDigClk_kHz != rxHsDigClk_kHz)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_TXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((orxHsDigClk_kHz > 0) && (txHsDigClk_kHz != orxHsDigClk_kHz)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_TXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (hsDigClk_kHz != txHsDigClk_kHz) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_TXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		device->devStateInfo.profilesValid |= TX_PROFILE_VALID;
	}

	/* Verify Rx profile is valid */
	if (rxHsDigClk_kHz > 0) {
		if ((txHsDigClk_kHz > 0) && (rxHsDigClk_kHz != txHsDigClk_kHz)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_RXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((orxHsDigClk_kHz > 0) && (rxHsDigClk_kHz != orxHsDigClk_kHz)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_RXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (hsDigClk_kHz != rxHsDigClk_kHz) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_RXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		device->devStateInfo.profilesValid |= RX_PROFILE_VALID;
	}

	/* Verify ORx profile is valid */
	if (orxHsDigClk_kHz > 0) {
		if ((txHsDigClk_kHz > 0) && (orxHsDigClk_kHz != txHsDigClk_kHz)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_ORXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if ((rxHsDigClk_kHz > 0) && (orxHsDigClk_kHz != rxHsDigClk_kHz)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_ORXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if (hsDigClk_kHz != orxHsDigClk_kHz) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_VERPFILE_ORXHSCLK, retVal, TALACT_ERR_CHECK_PARAM);
		}

		device->devStateInfo.profilesValid |= ORX_PROFILE_VALID;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setSpiSettings(taliseDevice_t *device, taliseSpiSettings_t *spi)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t spiReg = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setSpiSettings()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (spi->MSBFirst == 0) {
		spiReg |= 0x42; /* SPI bit is 1 = LSB first */
	}

	if (spi->autoIncAddrUp > 0) {
		spiReg |= 0x24;
	}

	if (spi->fourWireMode > 0) {
		spiReg |= 0x18;
	}

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_SPI_INTERFACE_CONFIG_A, spiReg);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_1X) &&
	    ( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_2X) &&
	    ( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_3X) &&
	    ( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_4X) &&
	    ( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_5X) &&
	    ( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_6X) &&
	    ( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_8X) &&
	    ( spi->cmosPadDrvStrength != TAL_CMOSPAD_DRV_10X)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETSPI_INV_CMOS_DRV_STR, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* Extract fields from this encoding when writing: {non_gpio_drv , spi_cmos_drv_select[3:0]} */
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_GPIO_SLEW_CONTROL_BYTE3,
					    (((uint8_t)spi->cmosPadDrvStrength) & 0x0F), 0xF0, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_DIGITAL_IO_CONTROL,
					    (((uint8_t)spi->cmosPadDrvStrength) >> 4), 0x10, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	if (spi->enSpiStreaming > 0) {
		/* Allow SPI streaming mode: SPI message ends when chip select de-asserts */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_SPI_INTERFACE_CONFIG_B, 0x00);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else {
		/* Force single instruction mode */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_SPI_INTERFACE_CONFIG_B, 0x80);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Check if SPI reads and writes work after changing settings */
	retVal = (talRecoveryActions_t)TALISE_verifySpiReadWrite (device);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_verifySpiReadWrite (taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t spiReg = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_verifySpiReadWrite()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check SPI read - VENDOR_ID_0 */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_VENDOR_ID_0, &spiReg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if (spiReg != 0x56) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERIFYSPI_READ_LOW_ADDR_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	/* Check SPI read - VENDOR_ID_1 */
	spiReg = 0;
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_VENDOR_ID_1, &spiReg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if (spiReg != 0x04) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERIFYSPI_READ_LOW_ADDR_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	/* Check SPI write - SCRATCHPAD : Data = 10110110 */
	spiReg = 0;
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SCRATCH_PAD, 0xB6);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_SCRATCH_PAD, &spiReg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if (spiReg != 0xB6) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERIFYSPI_WRITE_LOW_ADDR_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	/* Check SPI write - SCRATCHPAD : Data = 01001001 */
	spiReg = 0;
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SCRATCH_PAD, 0x49);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_SCRATCH_PAD, &spiReg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if (spiReg != 0x49) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERIFYSPI_WRITE_LOW_ADDR_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	/* Check SPI read - SCRATCH_PAD_READ_ONLY_UPPER_ADDRESS_SPACE */
	spiReg = 0;
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_SCRATCH_PAD_READ_ONLY_UPPER_ADDRESS_SPACE, &spiReg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if (spiReg != 0xA5) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERIFYSPI_READ_HIGH_ADDR_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	/* Check SPI write - SCRATCH_PAD_UPPER_ADDRESS_SPACE : Data = 10110110 */
	spiReg = 0;
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, 0xB6);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, &spiReg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if (spiReg != 0xB6) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERIFYSPI_WRITE_HIGH_ADDR_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	/* Check SPI write - SCRATCH_PAD_UPPER_ADDRESS_SPACE : Data = 01001001 */
	spiReg = 0;
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, 0x49);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, &spiReg);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if (spiReg != 0x49) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_VERIFYSPI_WRITE_HIGH_ADDR_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_initDigitalClocks(taliseDevice_t *device,
				  taliseDigClocks_t *clockSettings)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	/* RF Synth variables */
	/* pull Synth and Loop filter values from a Look Up Table */
	uint8_t vcoOutLvl;
	uint8_t vcoVaractor;    /* [3:0] */
	uint8_t vcoBiasRef;     /* [2:0] */
	uint8_t vcoBiasTcf;     /* [1:0] */
	uint8_t vcoCalOffset;   /* [3:0] */
	uint8_t vcoVaractorRef; /* [3:0] */
	uint8_t loopFilterIcp;  /* Icp[5:0] */
	uint8_t loopFilterC2C1; /* C2[3:0],C1[3:0] */
	uint8_t loopFilterR1C3; /* R1[3:0],C3[3:0] */
	uint8_t loopFilterR3;   /* R3[3:0] */
	uint8_t vcoIndex;
	uint8_t i;

	/* RF PLL variables */
	uint16_t integerWord;
	uint32_t fractionalWord;
	uint32_t fractionalRemainder;
	uint32_t scaledRefClk_Hz;

	/* common */
	uint32_t hsDigClk_Hz_div2 = 0;
	uint32_t scaledRefClk_kHz = 0;
	uint8_t deviceClkDiv = 0;
	uint8_t clkPllRefClkDiv = 0;

	uint8_t clockControl2 = 0;
	uint8_t clockControl5 = 0x40;
	uint8_t sdmSettings = 0;
	uint8_t hsDivTimes10 = 25;

	uint32_t txAttenClock_Hz = 0;
	uint8_t txAttenClockDiv = 0;

	/******************RF Synth Section************************/
	static const uint8_t icp_46p08[53] = {15, 15, 15, 16, 17, 18, 19, 20, 21, 22,
					      23, 24, 25, 26, 27, 27, 27, 28, 29, 30,
					      31, 32, 33, 32, 33, 32, 33, 33, 34, 27,
					      28, 29, 29, 30, 39, 29, 30, 31, 31, 30,
					      31, 31, 32, 30, 31, 31, 32, 32, 31, 31,
					      32, 32, 33
					     };

	static const uint8_t icp_61p44[53] = {13, 13, 13, 14, 15, 16, 16, 17, 18, 19,
					      20, 21, 22, 22, 23, 24, 24, 25, 25, 26,
					      27, 28, 29, 28, 29, 28, 29, 29, 30, 24,
					      25, 25, 26, 26, 35, 33, 34, 35, 35, 26,
					      27, 27, 28, 27, 27, 27, 28, 28, 27, 27,
					      28, 28, 29
					     };

	static const uint8_t icp_76p8[53]  = { 7, 7, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
					       12, 12, 13, 13, 13, 14, 14, 15, 15, 15,
					       16, 15, 16, 15, 16, 16, 17, 13, 14, 14,
					       14, 15, 19, 14, 15, 15, 15, 15, 15, 15,
					       15, 15, 15, 15, 15, 16, 15, 15, 15, 16,
					       16
					     };

	static const uint32_t vcoFreqArray_kHz [53] = {
		12605000UL, 12245000UL, 11906000UL, 11588000UL, 11288000UL,
		11007000UL, 10742000UL, 10492000UL, 10258000UL, 10036000UL,
		9827800UL, 9631100UL, 9445300UL, 9269800UL, 9103600UL,
		8946300UL, 8797000UL, 8655300UL, 8520600UL, 8392300UL,
		8269900UL, 8153100UL, 8041400UL, 7934400UL, 7831800UL,
		7733200UL, 7638400UL, 7547100UL, 7459000UL, 7374000UL,
		7291900UL, 7212400UL, 7135500UL, 7061000UL, 6988700UL,
		6918600UL, 6850600UL, 6784600UL, 6720500UL, 6658200UL,
		6597800UL, 6539200UL, 6482300UL, 6427000UL, 6373400UL,
		6321400UL, 6270900UL, 6222000UL, 6174500UL, 6128400UL,
		6083600UL, 6040100UL, 5997700UL
	};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_initDigitalClocks()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Scaled Ref Clock at input to CLKPLL should be in range 40-80MHz.
	 * Slightly lower frequencies allowed for lab testing - not suggested for
	 * production
	 */
	if (clockSettings->deviceClock_kHz > 15000 &&
	    clockSettings->deviceClock_kHz <= 40000) {
		/* Do not use doubler because multichip sync is not valid when doubler enabled */
		scaledRefClk_kHz = clockSettings->deviceClock_kHz; /* total div 1 */
		deviceClkDiv = 0; /* div 1 */
		clkPllRefClkDiv = 0; /* div 1 */
	} else if (clockSettings->deviceClock_kHz > 40000 &&
		   clockSettings->deviceClock_kHz <= 80000) {
		scaledRefClk_kHz = clockSettings->deviceClock_kHz; /* total div 1 */
		deviceClkDiv = 0; /* div 1 */
		clkPllRefClkDiv = 0; /* div 1 */
	} else if (clockSettings->deviceClock_kHz > 80000 &&
		   clockSettings->deviceClock_kHz <= 160000) {
		scaledRefClk_kHz = clockSettings->deviceClock_kHz >> 1; /* total div 2 */
		deviceClkDiv = 0; /* div 1 */
		clkPllRefClkDiv = 1; /* div 2 */
	} else if (clockSettings->deviceClock_kHz > 160000 &&
		   clockSettings->deviceClock_kHz <= 320000) {
		scaledRefClk_kHz = clockSettings->deviceClock_kHz >> 2; /* total div 4 */
		deviceClkDiv = 0; /* div 1 */
		clkPllRefClkDiv = 2; /* div 4 */
	} else if (clockSettings->deviceClock_kHz > 320000 &&
		   clockSettings->deviceClock_kHz <= 640000) {
		scaledRefClk_kHz = clockSettings->deviceClock_kHz >> 3; /* total div 8 */
		deviceClkDiv = 1; /* div 2 */
		clkPllRefClkDiv = 2; /* div 4 */
	} else if (clockSettings->deviceClock_kHz > 640000 &&
		   clockSettings->deviceClock_kHz <= 1280000) {
		scaledRefClk_kHz = clockSettings->deviceClock_kHz >> 4; /* total div 16 */
		deviceClkDiv = 2; /* div 4 */
		clkPllRefClkDiv = 2; /* div 4 */
	} else {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_SCALEDDEVCLK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	switch(clockSettings->clkPllHsDiv) {
	case TAL_HSDIV_2:
		hsDivTimes10 = 20;
		break;
	case TAL_HSDIV_2P5:
		hsDivTimes10 = 25;
		break;
	case TAL_HSDIV_3:
		hsDivTimes10 = 30;
		break;
	case TAL_HSDIV_4:
		hsDivTimes10 = 40;
		break;
	case TAL_HSDIV_5:
		hsDivTimes10 = 50;
		break;
	default:
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_CLKPLL_INV_HSDIV, retVal, TALACT_ERR_CHECK_PARAM);
	}

	clockControl2 = ((clkPllRefClkDiv & 0x7) << 4) | (((uint8_t)
			clockSettings->clkPllHsDiv) & 0x7);

	/* Set TxAtten / AGC clock divider */
	if (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz > 500000000) {
		txAttenClockDiv = 2; /* div 4 */
		txAttenClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 4);
	} else if (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz > 250000000) {
		txAttenClockDiv = 1; /* div 2 */
		txAttenClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 2);
	} else {
		txAttenClockDiv = 0; /* div 1 */
		txAttenClock_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz;
	}

	/* TxAtten clock should be less than 250Mhz by design */
	if (txAttenClock_Hz > 250000000) {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETCLKPLL_INV_TXATTENDIV, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Tx Atten clock divider, bits [3:2], is same as AGC clock divider bits[1:0]
	 * bit [6] is resetb for device clock divider */
	clockControl5 = 0x40 | ((deviceClkDiv & 0x3) << 4) | ((txAttenClockDiv & 0x3) <<
			2) | (txAttenClockDiv & 0x3);

	/* find vco table index based on vco frequency and then break out of loop to assign value */
	for (i = 0; clockSettings->clkPllVcoFreq_kHz < vcoFreqArray_kHz[i]; i++) {}
	vcoIndex = i + 1;

	if (vcoIndex < 1 || vcoIndex > 53) {
		/* vco index out of range */
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETCLKPLL_INV_VCOINDEX, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* valid for all refclk frequencies */
	vcoOutLvl      = (vcoIndex <= 16) ? 13 : (vcoIndex <= 23) ? 12 :
			 (vcoIndex <= 25) ?
			 11 : (vcoIndex <= 35) ? 10 : (vcoIndex <= 39) ? 9 : (vcoIndex <= 43) ?
			 8 :  (vcoIndex <= 48) ? 7 : 6;
	vcoVaractor    = (vcoIndex <= 29 || vcoIndex == 35) ?  1 : 2;
	vcoBiasRef     = (vcoIndex <= 7 ) ?  4 : (vcoIndex <= 16) ? 6 : 7;
	vcoBiasTcf     = (vcoIndex <= 25 || (vcoIndex > 26 && vcoIndex <= 29) ||
			  (vcoIndex > 37 && vcoIndex <= 39)) || (vcoIndex > 40 && vcoIndex <= 43) ?
			 2 : 3;

	vcoVaractorRef = (vcoIndex <= 29) ? 12 : (vcoIndex == 35) ?
			 14 :  13;

	if (scaledRefClk_kHz >= 15000 && scaledRefClk_kHz < 53760) {
		/* scaledRefClkMhz = 46.08 MHz */
		vcoCalOffset   = (vcoIndex == 11 || (vcoIndex > 35 && vcoIndex <= 37)
				  || (vcoIndex > 29 && vcoIndex <= 34)|| vcoIndex == 40 || vcoIndex == 44) ?
				 14 :(vcoIndex <= 10 || vcoIndex > 44) ?
				 15 : ((vcoIndex > 12 && vcoIndex <= 25) || (vcoIndex > 26 && vcoIndex <= 29)
				       || vcoIndex == 35) ?
				 12 : 13;

		loopFilterIcp = icp_46p08[vcoIndex-1];
		loopFilterC2C1 = 0xF9; /* C2=0xF0 */
		loopFilterR1C3 = 0xD5;
		loopFilterR3   = 0x0E;
	} else if (scaledRefClk_kHz >= 53760 && scaledRefClk_kHz < 69120) {
		/* scaledRefClkMhz = 61.44 MHz */
		vcoCalOffset   = (vcoIndex == 11 || (vcoIndex > 29 && vcoIndex <= 34)
				  || vcoIndex == 40 || vcoIndex == 44) ?
				 14 :(vcoIndex <= 10 || vcoIndex > 44) ?
				 15 : ((vcoIndex > 12 && vcoIndex <= 22) || (vcoIndex > 23 && vcoIndex <= 25)
				       || (vcoIndex > 26 && vcoIndex <= 29) || (vcoIndex > 34 && vcoIndex <= 37)) ?
				 12 : (vcoIndex > 37 && vcoIndex <= 39) ? 11 : 13;

		vcoVaractor = (vcoIndex > 35 && vcoIndex <= 39) ? 1 : vcoVaractor;
		vcoBiasRef = (vcoIndex > 4 && vcoIndex <= 7) ? 5 : vcoBiasRef;
		vcoVaractorRef = (vcoIndex > 35 && vcoIndex <= 39) ? 14 : vcoVaractorRef;
		loopFilterIcp  = icp_61p44[vcoIndex-1];
		loopFilterC2C1 = ((vcoIndex > 2 && vcoIndex <= 6) || (vcoIndex > 8
				  && vcoIndex <= 13) || (vcoIndex > 14)) ? 0xFD : 0xFC; /* C2=0xF0 */
		loopFilterR1C3 = 0xC5;
		loopFilterR3   = (vcoIndex <= 2 || (vcoIndex > 6 && vcoIndex <= 8)
				  || vcoIndex == 14) ? 14 : 13 ;
	} else if (scaledRefClk_kHz >= 69120 && scaledRefClk_kHz < 80000) {
		/* scaledRefClkMhz = 76.8 MHz */
		vcoCalOffset   = (vcoIndex == 11 || (vcoIndex > 35 && vcoIndex <= 37)
				  || (vcoIndex > 29 && vcoIndex <= 34)|| vcoIndex == 40 || vcoIndex == 44) ?
				 14 :(vcoIndex <= 10 || vcoIndex > 44) ?
				 15 : ((vcoIndex > 12 && vcoIndex <= 25) || (vcoIndex > 26 && vcoIndex <= 29)
				       || vcoIndex == 35) ?
				 12 : 13;

		loopFilterIcp  = icp_76p8[vcoIndex-1];
		loopFilterC2C1 = (vcoIndex == 20 ) ? 0xF7 : (vcoIndex == 4 || vcoIndex == 6
				 || vcoIndex == 8 ) ? 0xE6: 0xF6; /*C2=0xF0, C1=6 or 7 */
		loopFilterR1C3 = (vcoIndex == 4 ) ? 0xE4 : 0xD5;
		loopFilterR3   = 0x0E;
	} else {
		return (uint32_t)talApiErrHandler(device,TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRFPLL_INV_REFCLK, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Hold CLKPLL digital logic in reset */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_4,
				   0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Device clock divider */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_5,
				   clockControl5);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set reference clock scaler + HSdiv */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_2,
				   clockControl2);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/*Set VCO cal time */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_CAL_CONTROL, 0x02);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Write Synth Setting regs */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_F_VCOTN_BYTE1,
				   ((vcoCalOffset & 0x0F) << 3)); /* Set VCO Cal offset[3:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_BYTE1,
				   (0xC0 | (vcoVaractor & 0x0F))); /* Init ALC[3:0], VCO varactor[3:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_BYTE2,
				   (0x40 | (vcoOutLvl & 0x0F))); /* VCO output level */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_BYTE9,
				   (((vcoBiasTcf & 0x03) << 3) | (vcoBiasRef &
						   0x07))); /* Set VCO Bias Tcf[1:0] and VCO Bias Ref[2:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_VCO_CAL_REF,
				   0x30); /* Set VCO cal time (compare length) + Set VCO Cal Ref Tcf[2:0]=0 */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_VCO_VAR_CTR1,
				   0x70); /* Set VCO Varactor Ref Tcf[2:0] and VCO Varactor Offset[3:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_VCO_VAR_CTR2,
				   (vcoVaractorRef & 0x0F)); /* Set VCO Varactor Reference[3:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Write Loop filter */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_BYTE3,
				   (loopFilterIcp & 0x03F)); /* Set Loop Filter Icp[5:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_BYTE6,
				   loopFilterC2C1); /* Set Loop Filter C2[3:0] and C1[3:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_BYTE7,
				   loopFilterR1C3); /* Set Loop Filter R1[3:0] and C3[3:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_LF_R3,
				   (loopFilterR3 & 0x0F)); /*Set Loop Filter R3[3:0] */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Calculate PLL integer and fractional words with integer math */
	scaledRefClk_Hz = scaledRefClk_kHz * 1000;
	hsDigClk_Hz_div2 = (uint32_t)((DIV_U64((((uint64_t)(
			clockSettings->clkPllVcoFreq_kHz) * 1000) >> 1), hsDivTimes10) * 10) >> 1);
	integerWord = (uint16_t)(hsDigClk_Hz_div2 / scaledRefClk_Hz);
	fractionalRemainder = hsDigClk_Hz_div2 % scaledRefClk_Hz;

	/* +1 >>1 is rounding (add .5) */
	fractionalWord = (uint32_t)(DIV_U64((uint64_t)fractionalRemainder * 4177920,
					    (uint64_t)scaledRefClk_Hz) + 1 ) >> 1;

	/* if fractionalWord rounded up and == PLL modulus, fix it */
	if (fractionalWord == 2088960) {
		fractionalWord = 0;
		integerWord = integerWord + 1;
	}

	if (fractionalWord > 0) {
		/* in normal case, the fractional word should be zero and SDM bypassed */
		sdmSettings = 0x20;

		if (integerWord < 9) {
			/* Ndivider too small for PLL design */
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETCLKPLL_INV_NDIV, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		/* Bypass SDM */
		sdmSettings = 0xE0;

		if (integerWord < 6) {
			/* Ndivider too small for PLL design */
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETCLKPLL_INV_NDIV, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Set PLL fractional word[22:0] */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE0, (fractionalWord & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE1, ((fractionalWord >> 8) & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE2, ((fractionalWord >> 16) & 0x7F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Write PLL integer word [7:0] */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE1,
				   (sdmSettings | ((integerWord >> 8) & 0x7)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE0, (integerWord & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Release PLL from reset, and set start VCO cal bit to 0 */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_4,
				   0x80);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Power up CLKPLL */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_CLK_SYNTHESIZER_POWER_DOWN, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = ADIHAL_wait_us(device->devHalInfo,
				  CLKPLL_POWERUP_US); /* Allow PLL time to power up */
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_WAIT, halError, retVal,
				  TALACT_ERR_CHECK_TIMER);
	IF_ERR_RETURN_U32(retVal);

	/* Enable Charge pump cal after Charge Pump current set (Icp[5:0]) */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLK_SYNTH_BYTE5,
				   0x84);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Start VCO Cal */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_4,
				   0x80);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_4,
				   0x81);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setTxPfirSyncClk(taliseDevice_t *device,
				 taliseTxProfile_t *txProfile)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	static const uint8_t NUM_MULT_PER_ROW = 20;
	static const uint8_t MAX_NUM_TAPS = 80;
	uint8_t effectiveRows = 0;
	uint8_t numRows = 0;
	uint32_t dpClk_kHz = 0;
	uint32_t syncClk_kHz = 0;
	uint32_t hsDigClkDiv4or5_kHz = 0;
	uint8_t syncDiv = 0;

	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) > 0) {

		/* Calculate number of FIR rows for number of Taps */
		if ((txProfile->txFir.numFirCoefs % NUM_MULT_PER_ROW == 0) &&
		    (txProfile->txFir.numFirCoefs > 0) &&
		    (txProfile->txFir.numFirCoefs <= MAX_NUM_TAPS)) {
			numRows = (txProfile->txFir.numFirCoefs / NUM_MULT_PER_ROW);
		} else {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_TXFIR_INV_NUMTAPS_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		effectiveRows = ((numRows + (txProfile->txFirInterpolation - 1)) /
				 txProfile->txFirInterpolation);

		/* round up to next power of 2 */
		switch (effectiveRows) {
		case 1:
			effectiveRows = 1;
			break;
		case 2:
			effectiveRows = 2;
			break;
		case 3: /* fall through */
		case 4:
			effectiveRows = 4;
			break;
		default: {
			/* invalid number of FIR taps */
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_TXFIR_INV_NUMROWS, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		dpClk_kHz = (txProfile->txInputRate_kHz * txProfile->txFirInterpolation *
			     effectiveRows);

		/* FIR DPCLK can only run at max of 500MHz */
		if (dpClk_kHz > 500000) {
			/* Max number of Tx PFIR taps exceeded for Tx Profile */
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_TXFIR_TAPSEXCEEDED, retVal, TALACT_ERR_CHECK_PARAM);
		}

		if(txProfile->txFirInterpolation < numRows) {
			/* SYNC CLOCK is the PFIR output rate / 2 */
			syncClk_kHz = txProfile->txInputRate_kHz * txProfile->txFirInterpolation / 2;
		} else {
			syncClk_kHz = txProfile->txInputRate_kHz;
		}

		hsDigClkDiv4or5_kHz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 1000;

		/* Select correct divider setting for SYNCCLK - must be == syncClk_kHz or slower */
		for (syncDiv = 0; syncDiv < 5; syncDiv++) {
			if ((hsDigClkDiv4or5_kHz / (4 << syncDiv)) <= syncClk_kHz) {
				break;
			}
		}

		/* Write Tx PFIR SYNC Clock divider */
		halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_6,
					    syncDiv, 0x38, 3);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setRxPfirSyncClk(taliseDevice_t *device,
				 taliseRxProfile_t *rxProfile, taliseORxProfile_t *orxProfile)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	static const uint8_t NUM_MULT_PER_ROW = 24;
	static const uint8_t MAX_NUM_TAPS = 72;
	uint8_t ddcMultiply = 1;
	uint8_t ddcDivide = 1;
	uint32_t rxOutClk_kHz = 0;
	uint8_t rxNumRows = 0;
	uint8_t rxEffectiveNumRows = 0;
	uint32_t rxDpClk_kHz = 0;

	uint8_t orxDdcDecRatio = 1;
	uint32_t orxOutClk_kHz = 0;
	uint8_t orxNumRows = 0;
	uint8_t orxEffectiveNumRows = 0;
	uint32_t orxDpClk_kHz = 0;

	uint32_t slowestOutputRate_kHz = 0;
	uint32_t syncClk_kHz = 0;
	uint32_t hsDigClkDiv4or5_kHz = 0;
	uint8_t syncDiv = 0;

	/**
	 * Calculate Rx SYNC Clock divider for Rx PFIR.  Same Rx SYNC Clock is used
	 * for Rx and ORx PFIRs, and must be slow enough to handle the slowest
	 * PFIR rate.
	 **/

	/* Calculate clocks for Rx Profile */
	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) > 0) {
		/* Calculate decimation in DDC */
		switch (rxProfile->rxDdcMode) {
		case TAL_RXDDC_BYPASS_REALIF:
		case TAL_RXDDC_BYPASS:
			ddcMultiply = 1;
			ddcDivide = 1;
			break;
		case TAL_RXDDC_DEC2_REALIF:
		case TAL_RXDDC_DEC2:
			ddcMultiply = 2;
			ddcDivide = 1;
			break;
		case TAL_RXDDC_INT2_REALIF:
		case TAL_RXDDC_INT2:
			ddcMultiply = 1;
			ddcDivide = 2;
			break;
		case TAL_RXDDC_FILTERONLY_REALIF:
		case TAL_RXDDC_FILTERONLY:
			ddcMultiply = 1;
			ddcDivide = 1;
			break;
		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_RXFIR_INV_DDC, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		/* Calculate Rx PFIR output from Rx output and decimation */
		rxOutClk_kHz = (rxProfile->rxOutputRate_kHz * ddcMultiply ) / ddcDivide;

		/* Set slowestOutputRate_kHz to the Rx PFIR output rate */
		slowestOutputRate_kHz = rxOutClk_kHz;

		/* Calculate number of rows in Rx FIR based on taps */
		if ((rxProfile->rxFir.numFirCoefs % NUM_MULT_PER_ROW == 0) &&
		    (rxProfile->rxFir.numFirCoefs > 0) &&
		    (rxProfile->rxFir.numFirCoefs <= MAX_NUM_TAPS)) {
			rxNumRows = (rxProfile->rxFir.numFirCoefs / NUM_MULT_PER_ROW);
		} else {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_RXFIR_INV_NUMTAPS_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Effective number of rows is the power of 2 that's >= rows */
		switch(rxNumRows) {
		case 1:
			rxEffectiveNumRows = 1;
			break;
		case 2:
			rxEffectiveNumRows = 2;
			break;
		case 3:
			rxEffectiveNumRows = 4;
			break;
		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_RXFIR_INV_NUMROWS, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		/* Calculate DP clock */
		rxDpClk_kHz =  rxOutClk_kHz * rxEffectiveNumRows;

		/* DP clock is a max of 500Mhz in Talise */
		if (rxDpClk_kHz > 500000) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_RXFIR_TAPSEXCEEDED, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Calculate clocks for Orx Profile */
	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0) {
		/* Calculate decimation in DDC */
		switch (orxProfile->orxDdcMode) {
		case TAL_ORXDDC_SUBSAMPLE_BY2:
			orxDdcDecRatio = 2;
			break;
		case TAL_ORXDDC_DISABLED:
			orxDdcDecRatio = 1;
			break;
		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_ORXFIR_INV_DDC, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		/* Calculate ORx PFIR output from Rx output and decimation */
		orxOutClk_kHz = orxProfile->orxOutputRate_kHz * orxDdcDecRatio;

		/* Check if the ORx PFIR output is the slowest among all output clocks */
		if ((slowestOutputRate_kHz == 0) ||
		    (slowestOutputRate_kHz > orxOutClk_kHz)) {
			slowestOutputRate_kHz = orxOutClk_kHz;
		}

		/* Calculate number of rows in ORx FIR based on taps */
		if ((orxProfile->rxFir.numFirCoefs % NUM_MULT_PER_ROW == 0) &&
		    (orxProfile->rxFir.numFirCoefs > 0) &&
		    (orxProfile->rxFir.numFirCoefs <= MAX_NUM_TAPS)) {
			orxNumRows = (orxProfile->rxFir.numFirCoefs / NUM_MULT_PER_ROW);
		} else {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_ORXFIR_INV_NUMTAPS_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Effective number of rows is the power of 2 that's >= rows */
		switch(orxNumRows) {
		case 1:
			orxEffectiveNumRows = 1;
			break;
		case 2:
			orxEffectiveNumRows = 2;
			break;
		case 3:
			orxEffectiveNumRows = 4;
			break;
		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_ORXFIR_INV_NUMROWS, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		/* Calculate DP clock */
		orxDpClk_kHz = orxOutClk_kHz * orxEffectiveNumRows;

		/* DP clock is a max of 500Mhz in Talise */
		if (orxDpClk_kHz > 500000) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TALISE_ERR_ORXFIR_TAPSEXCEEDED, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* SYNC CLOCK should be FIR output rate / 2 */
	syncClk_kHz = (slowestOutputRate_kHz / 2);

	/* Get hsDigClkDiv4or5_kHz from profile */
	hsDigClkDiv4or5_kHz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 1000;

	/* Select correct divider setting for SYNCCLK - must be == syncClk_kHz or slower */
	for (syncDiv = 0; syncDiv < 5; syncDiv++) {
		if ((hsDigClkDiv4or5_kHz / (4 << syncDiv)) <= syncClk_kHz) {
			break;
		}
	}

	/* Write Rx/Orx PFIR SYNC Clock divider */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_6,
				    syncDiv, 0x7, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getApiVersion (taliseDevice_t *device, uint32_t *siVer,
			       uint32_t *majorVer, uint32_t *minorVer, uint32_t *buildVer)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getApiVersion()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((siVer == NULL) ||
	    (majorVer == NULL) ||
	    (minorVer == NULL) ||
	    (buildVer == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETAPIVERSION_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	*siVer = TAL_CURRENT_SI_VERSION;
	*majorVer = TAL_CURRENT_MAJOR_VERSION;
	*minorVer = TAL_CURRENT_MINOR_VERSION;
	*buildVer = TAL_CURRENT_BUILD_VERSION;
	return (uint32_t)retVal;
}

uint32_t TALISE_getDeviceRev(taliseDevice_t *device, uint8_t *revision)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getDeviceRev()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (revision == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDEVICEREV_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_PRODUCT_ID_1,
				  revision);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return (uint32_t)retVal;
}

uint32_t TALISE_getProductId(taliseDevice_t *device, uint8_t *productId)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getProductId()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (productId == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETPRODUCTID_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_PRODUCT_ID_0,
				  productId);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return (uint32_t)retVal;
}

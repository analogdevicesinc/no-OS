// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_error.c
 * \brief Contains Talise API error information functions
 *        These functions are public to the customer for getting more details on
 *        errors and debugging.
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_error.h"
#include "talise_hal.h"
#include "adi_hal.h"
#include "talise_user.h"
#include "talise_gpio.h"
#include "talise_arm.h"
#include "talise_cals.h"

uint32_t talRegisterErrorMessage(taliseDevice_t *device, uint32_t errSrc,
				 const char* (*callbackFunction)(uint32_t, uint32_t))
{
#ifndef TALISE_VERBOSE
	return (0);
#else
	uint32_t i = 0;
	talErrFunctionTable_t *talFunctionTable =
		&device->devStateInfo.talErrFunctionTable;

	for (i = 0; i < TALISE_ERRORFUNCTIONTABLEMAX; i++) {
		if ((talFunctionTable->talErrorFunctionTable[i].errSrc == errSrc) &&
		    (talFunctionTable->talErrorFunctionTable[i].callbackFunction != NULL)) {
			/* Already registered */
			break;
		}

		/* Register function if space is available */
		if (talFunctionTable->talErrorFunctionTable[i].callbackFunction == NULL) {
			talFunctionTable->talErrorFunctionTable[i].callbackFunction = callbackFunction;
			talFunctionTable->talErrorFunctionTable[i].errSrc = errSrc;
			break;
		}

	}

	if (i >= TALISE_ERRORFUNCTIONTABLEMAX) {
		/* space is not available */
		return (1);
	}

	return (0);
#endif
}

static const char* talCallbackErrorMessage(taliseDevice_t *device,
		uint32_t errSrc, uint32_t errCode)
{
#ifndef TALISE_VERBOSE
	return "";
#else
	uint32_t i = 0;
	talErrFunctionTable_t *talFunctionTable =
		&device->devStateInfo.talErrFunctionTable;

	for (i = 0; i < TALISE_ERRORFUNCTIONTABLEMAX; i++) {
		if ((talFunctionTable->talErrorFunctionTable[i].errSrc == errSrc) &&
		    (talFunctionTable->talErrorFunctionTable[i].callbackFunction != NULL)) {
			return ((*talFunctionTable->talErrorFunctionTable[i].callbackFunction)(errSrc,
					errCode));
		}

	}

	return "Invalid TALISE error passed, not in error list\n";
#endif
}

uint32_t TALISE_getErrCode(taliseDevice_t *device, uint32_t* errSrc,
			   uint32_t* errCode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	if(device == NULL ||
	    errSrc == NULL ||
	    errCode == NULL) {
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	*errSrc = device->devStateInfo.errSource;
	*errCode = device->devStateInfo.errCode;

	return (uint32_t)retVal;
}

const char* TALISE_getErrorMessage(uint32_t errSrc, uint32_t errCode)
{
#ifndef TALISE_VERBOSE
	return "";
#else

	if(errSrc == TAL_ERRSRC_ADIHAL) {
		switch (errCode) {
		case ADIHAL_OK:
			return "";
		case ADIHAL_SPI_FAIL:
			return "ADIHAL: SPI Failure\n";
		case ADIHAL_GPIO_FAIL:
			return "ADIHAL: GPIO Failure\n";
		case ADIHAL_TIMER_FAIL:
			return "ADIHAL:Timing Function Failure\n";
		case ADIHAL_WAIT_TIMEOUT:
			return "ADIHAL:HAL Access Timed out\n";
		case ADIHAL_GEN_SW:
			return "ADIHAL:HAL Function Soft Error\n";
		case ADIHAL_WARNING:
			return "ADIHAL:HAL Function Soft Warning\n";

		default:
			return "Invalid ADIHAL error passed, not in error list\n";
		}
	} else if ((errSrc == TAL_ERRSRC_TALARMCMD) ||
		   (errSrc == TAL_ERRSRC_TALAPIARM)) {
		return talGetArmErrorMessage(errSrc, errCode);
	} else if (errSrc == TAL_ERRSRC_INITCALS) {
		return talGetCalErrorMessage(errSrc, errCode);
	} else if (errSrc == TAL_ERRSRC_API) {
		switch (errCode) {
		case TAL_ERR_OK:
			return "\n";
		case TAL_ERR_INV_NULL_INIT_PARAM:
			return "Talise init data structure pointer is null\n";
		case TAL_ERR_WAITFOREVENT_INV_PARM:
			return "waitForEvent had invalid parameter.\n";
		case TAL_ERR_INIT_NULLPARAM:
			return "TALISE_initialize() was passed a NULL pointer for the device or init function parameter\n";
		case TAL_ERR_INIT_INV_DEVCLK:
			return "Invalid device clock in TALISE_initialize()\n";
		case TAL_ERR_CLKPLL_INV_HSDIV:
			return "Invalid CLKPLL HSDIV. (Valid 2, 2.5, 3, 4 or 5)\n";
		case TAL_ERR_SETCLKPLL_INV_VCOINDEX:
			return "CLKPLL VCO frequency out of range.\n";
		case TAL_ERR_SETCLKPLL_INV_NDIV:
			return "CLKPLL N divider too small, try using different device clock frequency\n";
		case TAL_ERR_SETRFPLL_INV_PLLNAME:
			return "Invalid pllName requested in setRfPllFreuquency().\n";
		case TAL_ERR_INV_SCALEDDEVCLK_PARAM:
			return "Could not scale device clock into range 40MHz to 80Mhz.\n";
		case TAL_ERR_SETRFPLL_INV_REFCLK:
			return "Unsupported PLL reference clock or refclk out of range.\n";
		case TAL_ERR_SETORXGAIN_INV_ORXPROFILE:
			return "Can not set ORx gain Index, ORX profile is not valid in device data structure\n";
		case TAL_ERR_SETORXGAIN_INV_CHANNEL:
			return "Invalid ObsRx channel in setObsRxManualGain().\n";
		case TAL_ERR_SETORXGAIN_INV_ORX1GAIN:
			return "Invalid ORx1 gain index requested in setObsRxManualGain()\n";
		case TAL_ERR_SETORXGAIN_INV_ORX2GAIN:
			return "Invalid ORx2 gain index requested in setObsRxManualGain()\n";
		case TAL_ERR_SETTXATTEN_INV_STEPSIZE_PARM:
			return "Invalid TxAtten stepsize. Use enum taliseTxAttenStepSize_t\n";
		case TAL_ERR_SETTXATTEN_INV_TXCHANNEL:
			return "Invalid Tx channel selected.\n";
		case TAL_ERR_SETTXATTEN_INV_PARM:
			return "Tx attenuation is out of range (0 - 41950 mdB).\n";
		case TAL_ERR_PROGRAMFIR_INV_NUMTAPS_PARM:
			return "Invalid number of FIR taps\n";
		case TALISE_ERR_TXFIR_INV_NUMTAPS_PARM:
			return "Invalid number of taps in Tx FIR\n";
		case TALISE_ERR_TXFIR_INV_NUMROWS:
			return "Invalid number of taps for given Tx FIR interpolation\n";
		case TALISE_ERR_TXFIR_TAPSEXCEEDED:
			return "Taps exceed max possible by the Tx FIR \n";
		case TALISE_ERR_RXFIR_INV_DDC:
			return "Invalid DDC configuration when calculating Rx FIR clock rates\n";
		case TALISE_ERR_RXFIR_INV_NUMTAPS_PARM:
			return "Invalid tap sizes when calculating Rx FIR clock rates\n";
		case TALISE_ERR_RXFIR_INV_NUMROWS:
			return "Invalid Rx FIR rows when calculating Rx FIR clock rates\n";
		case TALISE_ERR_RXFIR_TAPSEXCEEDED:
			return "Invalid clock rates when calculating Rx FIR clock rates\n";
		case TALISE_ERR_ORXFIR_INV_DDC:
			return "Invalid DDC configuration when calculating ORx FIR clock rates\n";
		case TALISE_ERR_ORXFIR_INV_NUMTAPS_PARM:
			return "Invalid tap sizes when calculating ORx FIR clock rates\n";
		case TALISE_ERR_ORXFIR_INV_NUMROWS:
			return "Invalid ORx FIR rows when calculating ORx FIR clock rates\n";
		case TALISE_ERR_ORXFIR_TAPSEXCEEDED:
			return "Invalid clock rates when calculating ORx FIR clock rates\n";
		case TAL_ERR_PROGRAMFIR_INV_FIRNAME_PARM:
			return "Invalid FIR filter name requested. use enum talisefirName_t\n";
		case TAL_ERR_RXFIR_INV_GAIN_PARM:
			return "Rx FIR filter has invalid gain setting\n";
		case TAL_ERR_SETRXGAIN_RXPROFILE_INVALID:
			return "Cannot set Rx Manual Gain, Rx profile is not valid in current configuration\n";
		case TAL_ERR_SETRXGAIN_INV_CHANNEL:
			return "Invalid channel selection for TALISE_setRxManualGain()\n";
		case TAL_ERR_SETRX1GAIN_INV_GAIN_PARM:
			return "Rx1 manual gain index out of range of gain table\n";
		case TAL_ERR_SETRX2GAIN_INV_GAIN_PARM:
			return "Rx2 manual gain index out of range of gain table\n";
		case TAL_ERR_SER_INV_M_PARM:
			return "Invalid JESD Rx Framer M parameter during setupSerializers()\n";
		case TAL_ERR_SER_INV_NP_PARM:
			return "Invalid JESD Rx Framer Np parameter during setupSerializers()\n";
		case TAL_ERR_SER_INV_L_PARM:
			return "Invalid JESD Rx Framer L parameter during setupSerializers()\n";
		case TAL_ERR_SER_INV_ORX_L_PARM:
			return "Invalid JESD Framer ORx L parameter during setupSerializers()\n";
		case TAL_ERR_SER_INV_ORX_M_PARM:
			return "Invalid JESD Framer ORx M parameter during setupSerializers()\n";
		case TAL_ERR_SER_INV_ORX_NP_PARM:
			return "Invalid JESD Framer ORx Np parameter (!= 16) during setupSerializers()\n";
		case TAL_ERR_SER_INV_ORX_LANEEN_PARM:
			return "Invalid number of ORx JESD204 lanes enabled\n";
		case TAL_ERR_SER_INV_LANERATE_PARM:
			return "Invalid Rx Lanerate frequency in setupSerializer()\n";
		case TAL_ERR_SER_INV_ORX_LANERATE_PARM:
			return "Invalid ORx Lanerate frequency in setupSerializer()\n";
		case TAL_ERR_SER_INV_LANEEN_PARM:
			return "Invalid number of JESD204 lanes enabled\n";
		case TAL_ERR_SER_INV_AMP_PARM:
			return "Invalid JESD204 serializer amplitude\n";
		case TAL_ERR_SER_INV_PREEMP_PARM:
			return "Invalid JESD204 serializer preemphasis setting\n";
		case TAL_ERR_SER_INV_LANEPN_PARM:
			return "Invalid JESD204 serializer PN invert setting\n";
		case TAL_ERR_SER_LANE_CONFLICT_PARM:
			return "FramerA and FramerB are attempting to use the same physical lanes\n";
		case TAL_ERR_SER_INV_TXSER_DIV_PARM:
			return "Invalid serializer lane clock divider\n";
		case TAL_ERR_SER_LANE_RATE_CONFLICT_PARM:
			return "Rx lane rate and ObsRx lane rate do not have common serializer lane clock rate.\n";
		case TAL_ERR_SER_INV_ZIF_TO_RIF_DATA_PARM:
			return "Framer M can only =1 if DDC mode is TAL_RXDDC_ZIF_TO_RIF_INT2\n";
		case TAL_ERR_SER_INV_DUALBAND_DATA_PARM:
			return "Framer M can only =8 if DDC mode is TAL_RXDDC_DUALBAND_DEC2\n";
		case TAL_ERR_SER_INV_RXFRAMER_SEL:
			return "Invalid Rx Framer select in device data structure\n";
		case TAL_ERR_SER_INV_ORXFRAMER_SEL:
			return "Invalid ObsRx Framer select in device data structure\n";
		case TAL_ERR_SER_LANERATE_ZERO:
			return "Serializer calculated lane rate = 0, check framer data structure for invalid setting\n";
		case TAL_ERR_HS_AND_LANE_RATE_NOT_INTEGER_MULT:
			return "Serializer HSCLK and lane clock are not integer multiples\n";
		case TAL_ERR_DESES_HS_AND_LANE_RATE_NOT_INTEGER_MULT:
			return "Deserializer lane clock and desired lanerate are not integer multiples\n";
		case TAL_ERR_DESES_INV_LANE_RATE:
			return "Invalid deserializer lane rate, when Np=12, CLKPLL VCO must = lane rate\n";
		case TAL_ERR_DESES_INV_LANE_RATE_DIV:
			return "A valid Deserializer lane clock divider is not possible with current settings\n";
		case TAL_ERR_DESER_INV_M_PARM:
			return "Invalid deserializer M value. (Valid 2 or 4)\n";
		case TAL_ERR_DESER_INV_NP_PARM:
			return "Invalid deserializer Np value. (Valid 12 or 16)\n";
		case TAL_ERR_DESER_INV_L_PARM:
			return "Invalid deserializer L value. (Valid 1, 2, 4)\n";
		case TAL_ERR_DESER_INV_LANERATE_PARM:
			return "Invalid deserializer lane rate (Valid 614.4Mbps to 12288Mbps)\n";
		case TAL_ERR_DESER_INV_LANEEN_PARM:
			return "Invalid number of deserializer lanes enabled (Valid 1, 2, 4)\n";
		case TAL_ERR_DESER_INV_EQ_PARM:
			return "Invalid deserializer EQ setting";
		case TAL_ERR_DESER_INV_LANEPN_PARM:
			return "Invalid deserializer invert Lane PN setting (valid 0 - 15)\n";
		case TAL_ERR_DESER_LANECONFLICT_PARM:
			return "When using both deframers, deframerA and B can not use the same phyiscal lane\n";
		case TAL_ERR_DESER_M_CONFLICT:
			return "When using both deframers, M must match for deframer A and B\n";
		case TAL_ERR_DESER_INV_DEFAB_M:
			return "When using both deframers, M can not exceed M=2\n";
		case TAL_ERR_DESER_NP_CONFLICT:
			return "When using both deframers, Np must match for deframer A and B\n";
		case TAL_ERR_DESER_INV_DEFRAMERSEL:
			return "Invalid deframer select parameter in TALISE_setupDeserializer()\n";
		case TAL_ERR_DESER_TXPROFILE_INV:
			return "Tx Profile config is not valid, can not setup deserializer. Verify device data structure\n";
		case TAL_ERR_FRAMER_INV_M_PARM:
			return "Invalid Rx framer M setting\n";
		case TAL_ERR_FRAMER_INV_NP_PARM:
			return "Invalid Rx framer Np setting\n";
		case TAL_ERR_FRAMER_INV_S_PARM:
			return "Invalid Framer S setting\n";
		case TAL_ERR_FRAMER_INV_BANKID_PARM:
			return "Invalid Rx framer Bank ID\n";
		case TAL_ERR_FRAMER_INV_LANEID_PARM:
			return "Invalid Rx framer Lane ID\n";
		case TAL_ERR_FRAMER_INV_SYNCBIN_PARM:
			return "Invalid Rxframer SYCNB select\n";
		case TAL_ERR_FRAMER_INV_LMFC_OFFSET_PARAM:
			return "Invalid Rx framer LMFC offset\n";
		case TAL_ERR_FRAMER_INV_DUALBAND_DATA_PARM:
			return"Invalid Rx framer dualband settings\n";
		case TAL_ERR_FRAMER_INV_FRAMERSEL_PARM:
			return "Invalid framer select parameter in TALISE_setupJesd204bFramer()\n";
		case TAL_ERR_FRAMERSTATUS_INV_FRAMERSEL_PARAM:
			return "Invalid framerSel parameter in readFramerStatus(). Choose framer A or framerB\n";
		case TAL_ERR_FRAMERSTATUS_NULL_FRAMERSTATUS_PARAM:
			return "NULL framerStatus param in readFramerStatus()\n";
		case TAL_ERR_FRAMER_ERRINJECT_INV_FRAMERSEL_PARAM:
			return "Invalid framerSel parameter in TALISE_injectFramerTestDataError()\n";
		case TAL_ERR_DEFRAMER_INV_DEFSEL:
			return "Invalid deframerSel parameter (Valid TAL_DEFRAMER_A or TAL_DEFRAMER_B)\n";
		case TAL_ERR_DEFRAMER_INV_TXPROFILE:
			return "Tx Profile is invalid. Verify tx structure in device data structure\n";
		case TAL_ERR_DEFRAMER_INV_LANESEN:
			return "Invalid deserializerLanesEnabled. (Valid 1-15 in 1,2,4 lane combos)\n";
		case TAL_ERR_DEFRAMER_INV_L:
			return "Invalid deframer L. deserializerLanesEnabled must enable 1,2 or 4 lanes\n";
		case TAL_ERR_DEFRAMER_INV_M:
			return "Invalid deframer M parameter. (valid 2 or 4)\n";
		case TAL_ERR_DEFRAMER_INV_NP:
			return "Invalid deframer Np parameter. (valid 12 or 16)\n";
		case TAL_ERR_DEFRAMER_INV_F:
			return "Invalid deframer F value. Check M, S and lanes enabled\n";
		case TAL_ERR_DEFRAMER_INV_K:
			return "Invalid deframer K value. Verify K is less than or equal to 32\n";
		case TAL_ERR_DEFRAMER_INV_FK:
			return "Invalid deframer F*K value. Verify M, S, and lanes enabled\n";
		case TAL_ERR_DEFRAMER_INV_PCLK:
			return "Invalid deframer PCLK. PCLK must be an integer divide from the CLKPLL VCO frequency\n";
		case TAL_ERR_DEFRAMER_INV_PCLKDIV:
			return "Invalid PCLK divider. The CLKPLL VCO to PCLK divide ratio is not a valid option\n";
		case TAL_ERR_DEFRAMER_INV_BANKID:
			return "Invalid JESD204 deframer bank ID (valid 0-15)\n";
		case TAL_ERR_ERR_DEFRAMER_INV_LANEID:
			return "INvalid JESD204 deframer lane ID (valid 0-31)\n";
		case TAL_ERR_DEFRAMER_INV_LMFC_OFFSET:
			return "Invalid deframer LMFC offset (valid 0 to K-1)\n";
		case TAL_ERR_RSTDEFRAMER_INV_DEFSEL:
			return "Invalid Deframer select parameter in enableDeframerLink()\n";
		case TAL_ERR_RSTFRAMER_INV_FRAMERSEL:
			return "Invalid framer select parameter in enableFramerLink()\n";
		case TAL_ERR_DEFSTATUS_INV_DEFRAMERSEL_PARAM:
			return "Invalid Deframer select parameter in readDeframerStatus()\n";
		case TAL_ERR_DEFSTATUS_NULL_DEFRAMERSTATUS_PARAM:
			return "Null pointer to deframerStatus parameter in readDeframerStatus()\n";
		case TAL_ERR_GETDFRMIRQSRC_NULL_STATUS_PARAM:
			return "TALISE_getDfrmIrqSource: Null pointer in irqSourceValue parameter\n";
		case TAL_ERR_SETDFRMIRQMASK_INV_DEFRAMERSEL_PARAM:
			return "Invalid deframer selected in setDfrmIrqMask()\n";
		case TAL_ERR_GETDFRMIRQMASK_NULL_IRQMASK_PARAM:
			return "Null pointer to irqMask parameter in getDfrmIrqMask()\n";
		case TAL_ERR_GETDFRMIRQMASK_INV_DEFRAMERSELECT_PARAM:
			return "Invalid deframer selected in getDfrmIrqMask()\n";
		case TAL_ERR_CLRDFRMIRQ_INV_DEFRAMERSEL_PARAM:
			return "Invalid deframer selected in TALISE_clearDfrmIrq()\n";
		case TAL_ERR_GETDFRMIRQSRC_INV_DEFRAMERSEL_PARAM:
			return "Invalid deframer selected in TALISE_getDfrmIrqSource()\n";
		case TAL_ERR_ENDEFSYSREF_INV_DEFRAMERSEL_PARAM:
			return "Invalid deframer select parameter in TALISE_enableSysrefToDeframer()\n";
		case TAL_ERR_ENFRAMERSYSREF_INV_FRAMERSEL_PARAM:
			return "Invalid framer select parameter in TALISE_enableSysrefToFramer()\n";
		case TAL_ERR_FRAMER_INV_TESTDATA_SOURCE_PARAM:
			return "Invalid framer testDataSource parameter in TALISE_enableFramerTestData()\n";
		case TAL_ERR_FRAMER_INV_INJECTPOINT_PARAM:
			return "Invalid framer test data injection point parameter in TALISE_enableFramerTestData()\n";
		case TAL_ERR_FRAMER_INV_FRAMERSEL_PARAM:
			return "Invalid framer select parameter in TALISE_enableFramerTestData()\n";
		case TAL_ERR_EN_DEFRAMER_PRBS_INV_PARAM:
			return "Invalid polyOrder or checkerLocation parameter in enableDeframerPrbsChecker()\n";
		case TAL_ERR_READDFRMPRBS_NULL_PARAM:
			return "NULL prbsErrorCount parameter in TALISE_readDeframerPrbsCounters()\n";
		case TAL_ERR_READDFRMPRBS_INV_DEFSEL_PARAM:
			return "Invalid deframer select parameter in TALISE_readDeframerPrbsCounters()\n";
		case TAL_ERR_INITARM_INV_ARMCLK_PARAM:
			return "Invalid ARM clock rate\n";
		case TAL_ERR_LOADHEX_INVALID_CHKSUM:
			return "LoadHex() line checksum is invalid\n";
		case TAL_ERR_LOADBIN_INVALID_BYTECOUNT:
			return 0;
		case TAL_ERR_READARMMEM_INV_ADDR_PARM:
			return "ReadArmMem() was given an invalid memory address\n";
		case TAL_ERR_WRITEARMMEM_INV_ADDR_PARM:
			return "WriteArmMem() was given an invalid memory address\n";
		case TAL_ERR_ARMCMD_INV_OPCODE_PARM:
			return "Invalid ARM opcode given to sendArmCommand()\n";
		case TAL_ERR_ARMCMD_INV_NUMBYTES_PARM:
			return "Invalid number of extended data in sendArmCommand()\n";
		case TAL_ERR_ARMCMDSTATUS_INV_OPCODE_PARM:
			return "Invalid opcode given to readArmCmdStatus()\n";
		case TAL_ERR_JESD204B_ILAS_MISMATCH_NULLPARAM:
			return "Function parameter has NULL pointer in TALISE_getDfrmIlasMismatch()\n";
		case TAL_ERR_JESD204B_ILAS_MISMATCH_INVALID_DEFRAMER:
			return "Invalid deframer selection parameter in TALISE_getDfrmIlasMismatch()\n";
		case TAL_ERR_JESD204B_ILAS_MISMATCH_NO_ACTIVE_LINK:
			return "No active link detected in TALISE_getDfrmIlasMismatch()\n";
		case TAL_ERR_JESD204B_ILAS_MISMATCH_SYNC_NOT_DETECTED:
			return "Sync_n not detected in TALISE_getDfrmIlasMismatch()\n";
		case TAL_ERR_TALFINDDFRMRLANECNTERROR_INV_DEFRAMERSEL_PARAM:
			return "Invalid deframer selection parameter in talFindDfrmrLaneCntErr()\n";
		case TAL_ERR_TALFINDDFRMRLANECNTERROR_NULL_PARAM:
			return "talFindDfrmrLaneCntErr: Null deframerInputsMask function parameter\n";
		case TAL_ERR_TALFINDDFRMRLANEERROR_NULL_PARAM:
			return "talFindDfrmrLaneErr: Null deframerInputsMask function parameter\n";
		case TAL_ERR_RXGAINTABLE_INV_PROFILE:
			return "TALISE_programRxGainTable(): RX gain table channel chosen, but the Rx profile is not valid in the device data structure\n";
		case TAL_ERR_RXGAINTABLE_INV_CHANNEL:
			return "TALISE_programRxGainTable(): Invalid channel specified\n";
		case TAL_ERR_RXGAINTABLE_INV_GAIN_INDEX_RANGE:
			return "TALISE_programRxGainTable(): Invalid numGainIndicesinTable greater than possible number of gain indices for channel, max is 128 indices. \n";
		case TAL_ERR_ORXGAINTABLE_INV_PROFILE:
			return "TALISE_programOrxGainTable(): RX gain table channel chosen, but the Rx profile is not valid in the device data structure\n";
		case TAL_ERR_ORXGAINTABLE_INV_CHANNEL:
			return "TALISE_programOrxGainTable(): Invalid channel specified\n";
		case TAL_ERR_ORXGAINTABLE_INV_GAIN_INDEX_RANGE:
			return "TALISE_programOrxGainTable(): Invalid numGainIndicesinTable greater than possible number of gain indices for channel, max is 124 indices. \n";
		case TAL_ERR_RXFRAMER_INV_FK_PARAM:
			return "Invalid FK parameter detected in TALISE_setupJesd204bFramer()\n";
		case TAL_ERR_RXFRAMER_INV_L_PARAM:
			return "Invalid L parameter detected in TALISE_setupJesd204bFramer\n";
		case TAL_ERR_INV_RX_GAIN_MODE_PARM:
			return "Invalid gain control mode detected in TALISE_setRxGainControlMode()\n";
		case TAL_ERR_UNSUPPORTED_RX_GAIN_MODE_PARM:
			return "TALISE_setRxGainControlMode(): Gain control mode is unsupported by API\n";
		case TAL_ERR_INV_AGC_RX_STRUCT_INIT:
			return "Invalid RX AGC structure detected at device->rx->rxAgcCtrl in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_STRUCT_INIT:
			return "Invalid RX AGC structure detected at device->rx->rxAgcCtrl->agcPeak in TALISE_setupRxAgc() and TALISE_getagcPeakRegisters\n";
		case TAL_ERR_INV_AGC_PWR_STRUCT_INIT:
			return "Invalid RX AGC structure detected at device->rx->rxAgcCtrl->agcPower in TALISE_setupRxAgc() and TALISE_getAgcPowerRegisters()\n";
		case TAL_ERR_INV_AGC_PKK_HIGHTHRSH_PARAM:
			return "Invalid  APD high threshold at device->rx->rxAgcCtrl->agcPeak->apdHighThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_LOWGAINHIGHTHRSH_PARAM:
			return "Invalid  APD peak detect high threshold detected at device->rx->rxAgcCtrl->agcPeak->apdLowThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_LOWGAINMODEHIGHTHRSH_PARAM:
			return "Invalid APD peak detect low threshold at device->rx->rxAgcCtrl->agcPeak->apdLowGainModeHighThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_LOWGAINTHRSH_PARAM:
			return "Invalid  APD peak detect low threshold at device->rx->rxAgcCtrl->agcPeak->apdLowGainModeLowThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_GAINSTEPATTACK_PARAM :
			return "Invalid  APD peak detect attack gain step at device->rx->rxAgcCtrl->agcPeak->apdGainStepAttack in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_APD_HIGH_LOW_THRESH:
			return "APD High threshold less than APD Low threshold in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_GAINSTEPRECOVERY_PARAM:
			return "Invalid   APD gain index step size at device->rx->rxAgcCtrl->agcPeak->apdGainStepRecovery in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2OVRLD_PARAM:
			return "Invalid HB2 overload detector setting at device->rx->rxAgcCtrl->agcPeak->enableHb2Overload in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2OVRLDDURATION_PARAM:
			return "Invalid HB2 Overload Duration Cnt at device->rx->rxAgcCtrl->agcPeak->hb2OverloadDurationCnt in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2OVRLDTHRSHCNT_PARAM:
			return "Invalid HB2 Overload Threshold Cnt at device->rx->rxAgcCtrl->agcPeak->hb2OverloadThreshCnt in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2GAINSTEPRECOVERY_PARAM:
			return "Invalid HB2 gain index step size at device->rx->rxAgcCtrl->agcPeak->hb2GainStepHighRecovery in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2GAINSTEP0RECOVERY_PARAM:
			return "Invalid HB2 gain index step size ovrerange interval 0 at device->rx->rxAgcCtrl->agcPeak->hb2GainStepLowRecovery in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2GAINSTEP1RECOVERY_PARAM:
			return "Invalid HB2 gain index step size overrange interval 1 at device->rx->rxAgcCtrl->agcPeak->hb2GainStepMidRecovery in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2GAINSTEPATTACK_PARAM:
			return "Invalid HB2 decimator output attack gain step at device->rx->rxAgcCtrl->agcPeak->hb2GainStepAttack in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2OVRLDPWRMODE_PARAM:
			return "Invalid HB2 overload mode at device->rx->rxAgcCtrl->agcPeak->hb2OverloadPowerMode in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2OVRLDSEL_PARAM:
			return "Invalid HB2 overload select bit at device->rx->rxAgcCtrl->agcPeak->hb2OvrgSel in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PKK_HB2THRSHCFG_PARAM:
			return "Invalid HB2 Threshold configuration at device->rx->rxAgcCtrl->agcPeak->hb2ThreshConfig in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_LWR0THRSH_PARAM:
			return "Invalid Power measurement detect lower 0 threshold at device->rx->rxAgcCtrl->agcPower->underRangeHighPowerThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_LWR1THRSH_PARAM:
			return "Invalid Power measurement detect lower 1 threshold at device->rx->rxAgcCtrl->agcPower->underRangeLowPowerThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_LWR0PWRGAINSTEP_PARAM:
			return "Invalid Power measurement detect lower 0 recovery gain step at device->rx->rxAgcCtrl->agcPower->underRangeHighPowerGainStepRecovery in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_LWR1PWRGAINSTEP_PARAM:
			return "Invalid Power measurement detect lower 1 recovery gain step at device->rx->rxAgcCtrl->agcPower->underRangeLowPowerGainStepRecovery in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_MSR_DURATION_PARAM:
			return "Invalid Power measurement duration used by the decimated power block at device->rx->rxAgcCtrl->agcPower->powerMeasurementDuration in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_UPPWR0THRSH_PARAM:
			return "Invalid AGC  device->rx->rxAgcCtrl->agcPower->upper0PowerThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_UPPWR1THRSH_PARAM:
			return "Invalid AGC device->rx->rxAgcCtrl->agcPower->upper1PowerThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_LOGSHIFT_PARAM:
			return "Invalid dynamic range increase of the power measurement from 40dB to ~60dB at device->rx->rxAgcCtrl->agcPower in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_UPPWR0GAINSTEP_PARAM:
			return "Invalid inner upper attack gain step at device->rx->rxAgcCtrl->agcPower->overRangeLowPowerGainStepAttack in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_PWR_UPPWR1GAINSTEP_PARAM:
			return "Invalid outer high power attack gain step at device->rx->rxAgcCtrl->agcPower->overRangeHighPowerGainStepAttack in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM:
			return "device->rx->rxAgcCtrl->agcPeakWaitTime out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_MIN_MAX_GAIN_PARM:
			return "TALISE_setupRxAgc(): RX1 or RX2 min or max gain index in AGC structure is out of range of the values programmed in the gain table\n";
		case TAL_ERR_INV_AGC_RX_GAIN_UPDATE_TIME_PARM:
			return "device->rx->rxAgcCtrl->agcGainUpdateTime_us out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_MIN_GAIN_GRT_THAN_MAX_GAIN_PARM :
			return "TALISE_setRxAgcMinMaxGainIndex() : Min Gain Index cannot be greater than Max Gain Index";
		case TAL_ERR_INV_AGC_RX1ATTACKDELAY_PARAM:
			return " device->rx->rxAgcCtrl->agcRx1AttackDelay is out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX2ATTACKDELAY_PARAM:
			return " device->rx->rxAgcCtrl->agcRx2AttackDelay is out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_LOWTHRSHPREVENTGAIN_PARM:
			return "Invalid device->rx->rxAgcCtrl->agcPeakThreshGainControlMode TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_CHANGEGAINTHRSHHIGH_PARM:
			return "Invalid device->rx->rxAgcCtrl->agcChangeGainIfThreshHigh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_RESETONRXON_PARM:
			return "Invalid device->rx->rxAgcCtrl->agcResetOnRxon in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_ENSYNCHPULSECAINCNTR_PARM:
			return "Invalid device->rx->rxAgcCtrl->agcEnableSyncPulseForGainCounter in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_ENIP3OPTTHRSH_PARM:
			return "Invalid device->rx->rxAgcCtrl->agcEnableIp3OptimizationThresh in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_ENFASTRECOVERYLOOP_PARM:
			return "Invalid device->rx->rxAgcCtrl->agcPeak->agcEnableFastRecoveryLoop in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_SLOWLOOPDELAY_PARAM:
			return "Invalid AGC slow loop settling delay in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_MINAGCSLOWLOOPDELAY_PARAM:
			return "TALISE_setupRxAgc(): AGC slow loop settling delay is too small.\n";
		case TAL_ERR_INV_AGC_PWR_IP3RANGE_PARAM:
			return "device->rx->rxAgcCtrl->ip3OverRangeThresh out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_CLK_DIV_RATIO_PARM:
			return "Calculated agcClockDivRatio is not valid in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_CLK_PARM:
			return "AGC clock is 0. Parameter device->devStateInfo->clocks->hsDigClkDiv4or5_Hz is not valid in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_CLK_RATIO:
			return "AGC clock to ADC clock ratio is invalid\n";
		case TAL_ERR_INV_AGC_RX_GAIN_UNDERRANGE_UPDATE_TIME_PARM:
			return "AGC Under Range time interval is not valid. Parameter agcUnderRangeLowInterval_us is out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_GAIN_UNDERRANGE_MID_INTERVAL_PARM:
			return "AGC Under Range time interval is not valid. Parameter agcUnderRangeMidInterval is out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_AGC_RX_GAIN_UNDERRANGE_HIGH_INTERVAL_PARM:
			return "AGC Under Range time interval is not valid. Parameter agcUnderRangeHighInterval is out of range in TALISE_setupRxAgc()\n";
		case TAL_ERR_INV_RX_DEC_POWER_PARAM:
			return "TALISE_getRxDecPower: Null rxDecPower_mdBFS function parameter\n";
		case TAL_ERR_GETRXDECPOWER_INV_PROFILE:
			return "TALISE_getRxDecPower: Dec Power read requested for an Rx channel, but Rx profile is invalid or channel not initialized in device structure\n";
		case TAL_ERR_GETRXDECPOWER_INV_CHANNEL:
			return "TALISE_getRxDecPower: Invalid rxChannel parameter\n";
		case TAL_ERR_WAITFOREVENT_TIMEDOUT_CLKPLLCP:
			return "Clock PLL Charge Pump Cal event timed out in TALISE_waitForEvent()\n";
		case TAL_ERR_WAITFOREVENT_TIMEDOUT_CLKPLL_LOCK:
			return "Clock PLL Lock event timed out in TALISE_waitForEvent()\n";
		case TAL_ERR_WAITFOREVENT_TIMEDOUT_RFPLLCP:
			return "RF PLL Charge Pump Cal event timed out in TALISE_waitForEvent()\n";
		case TAL_ERR_WAITFOREVENT_TIMEDOUT_RFPLL_LOCK:
			return "RF PLL Lock event timed out in TALISE_waitForEvent()\n";
		case TAL_ERR_WAITFOREVENT_TIMEDOUT_AUXPLLCP:
			return "Aux PLL Charge Pump Cal event timed out in TALISE_waitForEvent()\n";
		case TAL_ERR_WAITFOREVENT_TIMEDOUT_AUXPLL_LOCK:
			return "Aux PLL Lock event timed out in TALISE_waitForEvent()\n";
		case TAL_ERR_WAITFOREVENT_TIMEDOUT_ARMBUSY:
			return "ARM Busy event timed out in TALISE_waitForEvent()\n";
		case TAL_ERR_TIMEDOUT_ARMMAILBOXBUSY:
			return "ARM Mailbox Busy. Command not executed in TALISE_sendArmCommand()\n";
		case TAL_ERR_EN_TRACKING_CALS_ARMSTATE_ERROR:
			return "Device not in radioOff/IDLE state. Error in TALISE_enableTrackingCals()\n";
		case TAL_ERR_GETPENDINGTRACKINGCALS_NULL_PARAM:
			return "TALISE_getPendingTrackingCals() was passed a null pointer\n";
		case TAL_ERR_TRACKINGCAL_OUTOFRANGE_PARAM:
			return "Parameter Out of Range in TALISE_rescheduleTrackingCal()\n";
		case TAL_ERR_PAUSETRACKINGCAL_INV_PARAM:
			return "Parameter out of range in TALISE_setAllTrackCalState()\n";
		case TAL_ERR_GETPAUSECALSTATE_NULL_PARAM:
			return "Null parameter in TALISE_getAllTrackCalState()\n";
		case TAL_ERR_SET_ARMGPIO_PINS_GPIO_IN_USE:
			return "TALISE_setArmGpioPins: Requested GPIO pin already assigned to another function\n";
		case TAL_ERR_SET_ARMGPIO_PINS_INV_SIGNALID:
			return "Invalid ARM GPIO Signal ID detected\n";
		case TAL_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN:
			return "Out of range ARM GPIO pin detected\n";
		case TAL_ERR_GETRADIOSTATE_NULL_PARAM:
			return "TALISE_getRadioState() has null *radioStatus parameter\n";
		case TAL_ERR_CHECKGETMCS_STATUS_NULL_PARM:
			return "TALISE_getMultiChipSyncStatus() has null *mcsStatus parameter\n";
		case TAL_ERR_WAIT_INITCALS_ARMERROR:
			return "TALISE_waitInitCals() returned an ARM error\n";
		case TAL_ERR_WAIT_INITCALS_NULL_PARAM:
			return "TALISE_waitInitCals() has one or more null parameters\n";
		case TAL_ERR_CHECK_PLL_LOCK_NULL_PARM:
			return "TALISE_checkPllsLockStatus() has a null *pllLockStatus parameter\n";
		case TAL_ERR_READGPIOSPI_NULL_PARM:
			return "TALISE_readGpioSpi() has a null *gpioSpiRead parameter\n";
		case TAL_ERR_READGPIO3V3SPI_NULL_PARM:
			return "TALISE_readGpio3v3Spi() has a null *gpio3v3SpiRead parameter\n";
		case TAL_ERR_GET_TXFILTEROVRG_NULL_PARM:
			return "TALISE_getTxFilterOverRangeStatus() has a null *txFilterStatus parameter\n";
		case TAL_ERR_PROGRAM_RXGAIN_TABLE_NULL_PARM:
			return "TALISE_programRxGainTable() has a null *gainTablePtr parameter\n";
		case TAL_ERR_PROGRAM_ORXGAIN_TABLE_NULL_PARM:
			return "TALISE_programOrxGainTable() has a null *gainTablePtr parameter\n";
		case TAL_ERR_PROGRAMFIR_NULL_PARM:
			return "TALISE_programFir() has a null *firFilter parameter\n";
		case TAL_ERR_PROGRAMFIR_COEFS_NULL:
			return "TALISE_programFir() has a null coef array in *firFilter structure\n";
		case TAL_ERR_READ_DEFRAMERSTATUS_NULL_PARAM:
			return "TALISE_readDeframerStatus() has a null *deframerStatus parameter\n";
		case TAL_ERR_READ_DEFRAMERPRBS_NULL_PARAM:
			return "TALISE_readDeframerPrbscounters() has a null *prbsErrorCount parameter\n";
		case TAL_ERR_ARMCMDSTATUS_NULL_PARM:
			return "TALISE_waitArmCmdStatus() has a null *cmdStatByte parameter\n";
		case TAL_ERR_READARMCMDSTATUS_NULL_PARM:
			return "TALISE_readArmCmdStatus() has one or more null pointer parameters\n";
		case TAL_ERR_LOADBIN_NULL_PARAM:
			return "TALISE_loadArmFromBinary() has a null *binary parameter\n";
		case TAL_ERR_GETARMVER_NULL_PARM:
			return "TALISE_getArmVersion() has a null parameter\n";
		case TAL_ERR_GETARMVER_V2_NULL_PARM:
			return "TALISE_getArmVersion_v2() has a null parameter\n";
		case TAL_ERR_GETARMVER_V2_INVALID_ARM_NOT_LOADED:
			return "ARM binary must be loaded before calling TALISE_getArmVersion_v2()\n";
		case TAL_ERR_CHECKPLLLOCK_NULL_PARM:
			return "Null pointer for paraemter in TALISE_checkPllsLockStatus()\n";
		case TAL_ERR_READEVENTSTATUS_INV_PARM:
			return "TALISE_readEventStatus() had invalid parameter\n";
		case TAL_ERR_RXFRAMER_INV_PCLKFREQ:
			return "Could not find a valid PCLK divider setting when setting up the Talise framers\n";
		case TAL_ERR_RXFRAMER_INV_OUTPUT_RATE:
			return "Output rate was 0 during framer configuration.\n";
		case TAL_ERR_BBIC_INV_CHN:
			return "Invalid channel parameter passed to TALISE_enableRxTx().\n";
		case TAL_ERR_INV_GP_INT_MASK_PARM:
			return "Invalid general purpose interrupt mask parameter passed to TALISE_setGpIntMask().\n";
		case TAL_ERR_INV_GP_INT_MASK_NULL_PARM:
			return "TALISE_getGpIntMask() has a null *gpIntMask parameter.\n";
		case TAL_ERR_GP_INT_STATUS_NULL_PARAM:
			return "TALISE_getGpIntStatus() has a null *gpIntStatus parameter\n";
		case TAL_ERR_INV_DAC_SAMP_XBAR_CHANNEL_SEL:
			return "Invalid channel selection for TALISE_setupDacSampleXbar()\n";
		case TAL_ERR_INV_ADC_SAMP_XBAR_FRAMER_SEL:
			return "Invalid framer selection for TALISE_setupAdcSampleXbar()\n";
		case TAL_ERR_INV_ADC_SAMP_XBAR_SELECT_PARAM:
			return "talCheckAdcSampleXbarSelectEnum(): Invalid ENUM for the ADC sample crossbar select\n";
		case TAL_ERR_INV_DAC_SAMP_XBAR_SELECT_PARAM:
			return "talCheckDacSampleXbarSelectEnum(): Invalid ENUM for the DAC sample crossbar select\n";
		case TAL_ERR_GETRFPLL_INV_PLLNAME:
			return "Invalid PLL selection for TALISE_getRfPllFrequency()\n";
		case TAL_ERR_GET_PLLFREQ_INV_REFCLKDIV:
			return "Invalid REFCLK divider for TALISE_getRfPllFrequency()\n";
		case TAL_ERR_GET_PLLFREQ_INV_HSDIV:
			return "Invalid HSDIV for TALISE_getRfPllFrequency()\n";
		case TAL_ERR_GETRFPLL_NULLPARAM:
			return "rfPllLoFrequency_Hz function parameter pointer is NULL in TALISE_getRfPllFrequency()\n";
		case TAL_ERR_FRAMER_A_AND_B_INV_M_PARM:
			return "M parameter cannot equal 4 when deframer A and B are selected\n";
		case TAL_ERR_INIT_CALS_COMPLETED_NULL_PTR:
			return "calsSincePowerUp parameter is null pointer in TALISE_getInitCalStatus()\n";
		case TAL_ERR_CHKINITCALS_NULL_PTR:
			return "areCalsRunning parameter is NULL pointer in TALISE_checkInitCalComplete()\n";
		case TAL_ERR_INIT_CALS_LASTRUN_NULL_PTR:
			return "calsLastRun parameter is null pointer in TALISE_getInitCalStatus()\n";
		case TAL_ERR_INIT_CALS_MIN_NULL_PTR:
			return "calsMinimum parameter is a null pointer in TALISE_getInitCalStatus()\n";
		case TAL_ERR_INIT_ERR_CAL_NULL_PTR:
			return "initErrCal parameter is a null pointer in TALISE_getInitCalStatus()\n";
		case TAL_ERR_INIT_ERR_CODE_NULL_PTR:
			return "initErrCode parameter is a null pointer in TALISE_getInitCalStatus()\n";
		case TAL_ERR_READARMCFG_ARMERRFLAG:
			return "ARM read error returned for TALISE_readArmConfig()\n";
		case TAL_ERR_GETRXGAIN_INV_RXPROFILE:
			return "Invalid Rx Profile for TALISE_getRxGain()\n";
		case TAL_ERR_GETRXGAIN_INV_CHANNEL:
			return "Invalid Rx channel for TALISE_getRxGain()\n";
		case TAL_ERR_GETRXGAIN_GAIN_RANGE_EXCEEDED:
			return "gain index exceeded expected maximum or minimum value for TALISE_getRxGain(). Please check if Receive Path is enabled\n";
		case TAL_ERR_GETOBSRXGAIN_GAIN_RANGE_EXCEEDED:
			return "Gain index exceeded expected max or min value for TALISE_getObsRxGain()\n";
		case TAL_ERR_SETUPDUALBANDRXAGC_GAIN_RANGE_MISMATCH:
			return "Upper gain index lessthan or equalto lower gain index TALISE_setupDualBandRxAgc()\n";
		case TAL_ERR_SETUPDUALBANDRXAGC_GAIN_OUT_OF_RANGE:
			return "Gain index lessthan minimum index or greaterthan maximum gain index TALISE_setupDualBandRxAgc()\n";
		case TAL_ERR_GETOBSRXGAIN_INV_ORXPROFILE:
			return "Attempted to read ObsRx gain index when the ORx profile is not valid\n";
		case TAL_ERR_GETOBSRXGAIN_INV_CHANNEL:
			return "Invalid ObsRx channel selected in TALISE_getObsRxGain()\n";
		case TAL_ERR_VERIFYBIN_CHECKSUM_TIMEOUT:
			return "Timeout occurred waiting for ARM to calculate run time checksum to verify ARM loaded correctly\n";
		case TAL_ERR_SETCLKPLL_INV_TXATTENDIV:
			return "During initialization, a valid txAtten or AGC clock divider setting could not be realized with current CLKPLL settings\n";
		case TAL_ERR_VERRXPFILE_INV_IQRATE:
			return "Rx Profile IQRate is out of range\n";
		case TAL_ERR_VERRXPFILE_INV_RFBW:
			return "Rx Profile RF bandwidth is out of range\n";
		case TAL_ERR_VERRXPFILE_INV_RHB1:
			return "Rx Profile halfband 1 FIR filter decimation out of range (1,2)\n";
		case TAL_ERR_VERRXPFILE_INV_DEC5:
			return "Rx Profile Dec5 FIR filter decimation out of range\n";
		case TAL_ERR_VERRXPFILE_INV_FIR:
			return "Rx Profile programmable FIR decimation out of range (1,2,4)\n";
		case TAL_ERR_VERRXPFILE_INV_COEF:
			return "Rx Profile PFIR coefficient array is NULL and filter is not bypassed\n";
		case TAL_ERR_VERRXPFILE_INV_DDC:
			return "Rx Profile DDC mode is invalid\n";
		case TAL_ERR_VERORXPFILE_INV_IQRATE:
			return "ORx Profile IQRate is out of range\n";
		case TAL_ERR_VERORXPFILE_INV_RFBW:
			return "ORx Profile RF bandwidth is out of range\n";
		case TAL_ERR_VERORXPFILE_INV_RHB1:
			return "ORx Profile halfband 1 FIR filter decimation is out of range (1,2)\n";
		case TAL_ERR_VERORXPFILE_INV_DEC5:
			return "ORx Profile Dec5 FIR filter decimation is out of range\n";
		case TAL_ERR_VERORXPFILE_INV_FIR:
			return "ORx Profile programmable FIR filter decimation is out of range (1,2,4)\n";
		case TAL_ERR_VERORXPFILE_INV_COEF:
			return "ORx Profile PFIR coefficient array is NULL and filter is not bypassed\n";
		case TAL_ERR_VERORXPFILE_INV_DDC:
			return "ORx Profile DDC mode is invalid\n";
		case TAL_ERR_VERTXPFILE_INV_DISONPLLUNLOCK:
			return "TALISE_initialize(): Invalid option for disTxDataIfPllUnlock\n";
		case TAL_ERR_VERTXPFILE_INV_IQRATE:
			return "Tx Profile IQRate is out of range\n";
		case TAL_ERR_VERTXPFILE_INV_RFBW:
			return "Tx Profile RF bandwidth is out of range\n";
		case TAL_ERR_VERTXPFILE_INV_THB1:
			return "Tx Profile halfband 1 FIR filter interpolation is out of range (1,2)\n";
		case TAL_ERR_VERTXPFILE_INV_THB2:
			return "Tx Profile halfband 2 FIR filter interpolation is out of range (1,2)\n";
		case TAL_ERR_VERTXPFILE_INV_THB3:
			return "Tx Profile halfband 3 FIR filter interpolation is out of range (1,2)\n";
		case TAL_ERR_VERTXPFILE_INV_INT5:
			return "Tx Profile Interpolate by 5 FIR filter interpolation is out of range (1,2)\n";
		case TAL_ERR_VERTXPFILE_INV_HBMUTEX:
			return "Tx Profile Interpolate by 5 FIR filter should be mutually exclusive to halfband 1,2,3 filters\n";
		case TAL_ERR_VERTXPFILE_INV_FIRIPL:
			return "Tx Profile programmable FIR filter interpolation is out of range(1,2,4)\n";
		case TAL_ERR_VERTXPFILE_INV_COEF:
			return "Tx Profile PFIR coef array is NULL and filter is not disabled\n";
		case TAL_ERR_VERTXPFILE_INV_DACDIV:
			return "Tx Profile DAC divider is out of range\n";
		case TAL_ERR_VERPFILE_INV_RFPLLMCSMODE:
			return "Talise init->clocks.rfPllPhaseSyncMode structure member has invalid option for RFPLL phase sync mode\n";
		case TAL_ERR_VERPFILE_TXHSCLK:
			return "Tx Profile IQrate and filter settings are not possible with current CLKPLL frequency\n";
		case TAL_ERR_VERPFILE_RXHSCLK:
			return "Rx Profile IQrate and filter settings are not possible with current CLKPLL frequency\n";
		case TAL_ERR_VERPFILE_ORXHSCLK:
			return "Orx Profile IQrate and filter settings are not possible with current CLKPLL frequency\n";
		case TAL_ERR_SETSPI_INV_CMOS_DRV_STR:
			return "SPI profile invalid CMPS pad drive strength\n";
		case TAL_ERR_SETTXTOORXMAP_INV_ORX1_MAP:
			return "Invalid Tx channel for oRx1Map function parameter in TALISE_setTxToOrxMapping()\n";
		case TAL_ERR_SETTXTOORXMAP_INV_ORX2_MAP:
			return "Invalid Tx channel for oRx2Map function parameter in TALISE_setTxToOrxMapping()\n";
		case TAL_ERR_GETRXTXENABLE_NULLPARAM:
			return "TALISE_getRxTxEnable function has NULL pointers in the function parameters\n";
		case TAL_ERR_SETRXTXENABLE_INVCHANNEL:
			return "TALISE_setRxTxEnable can not enable a Rx, Tx, ORx channel that was not initialized with the TALISE_initialize() function\n";
		case TAL_ERR_GETTXATTEN_NULL_PARM:
			return "TALISE_getTxAttenuation txAttenuation_mdB pointer has not been initialized\n";
		case TAL_ERR_GETTXATTEN_INV_TXCHANNEL:
			return "TALISE_getTxAttenuation txChannel is an invalid channel selection\n";
		case TAL_ERR_INV_RADIO_CTL_MASK_PARM:
			return "TALISE_setRadioCtlPinMode mask parameter contains invalid bit settings\n";
		case TAL_ERR_GETTEMPERATURE_NULLPARAM:
			return "temperature function parameter is a NULL pointer in TALISE_getTemperature()\n";
		case TAL_ERR_GETPINMODE_NULLPARAM:
			return "TALISE_getRadioCtlPinMode function parameters are null pointers\n";
		case TAL_ERR_SET_ARMGPIO_NULLPARAM:
			return "TALISE_setArmGpioPins function parameters are null pointers\n";
		case TAL_ERR_ARMCMDSTATUS_ARMERROR:
			return "TALISE_waitArmCmdStatus() failed due to thrown ARM error. Is device in correct state for calling command?\n";
		case TAL_ERR_WAITARMCMDSTATUS_INV_OPCODE:
			return "TALISE_waitArmCmdStatus() failed due invalid Opcode\n";
		case TAL_ERR_WAITARMCMDSTATUS_TIMEOUT:
			return "TALISE_waitArmCmdStatus() failed due to thrown ARM error. ARM time out \n";
		case TAL_ERR_GETTXLOLSTATUS_NULL_PARAM:
			return "TALISE_getTxLolStatus(): txLolStatus is a null pointer\n";
		case TAL_ERR_GETTXLOLSTATUS_INV_CHANNEL_PARM:
			return "TALISE_getTxLolStatus(): Invalid channel - TAL_TX1 or TAL_TX2 expected\n";
		case TAL_ERR_GETTXQECSTATUS_NULL_PARAM:
			return "TALISE_getTxQecStatus(): txQecStatus is a null pointer\n";
		case TAL_ERR_GETTXQECSTATUS_INV_CHANNEL_PARM:
			return "TALISE_getTxQecStatus(): Invalid channel - TAL_TX1 or TAL_TX2 expected\n";
		case TAL_ERR_GETRXQECSTATUS_NULL_PARAM:
			return "TALISE_getRxQecStatus(): rxQecStatus is a null pointer\n";
		case TAL_ERR_GETRXQECSTATUS_INV_CHANNEL_PARM:
			return "TALISE_getRxQecStatus(): Invalid channel - TAL_RX1 or TAL_RX2 expected\n";
		case TAL_ERR_GETORXQECSTATUS_NULL_PARAM:
			return "TALISE_getOrxQecStatus(): orxQecStatus is a null pointer\n";
		case TAL_ERR_GETORXQECSTATUS_INV_CHANNEL_PARM:
			return "TALISE_getOrxQecStatus(): Invalid channel - TAL_ORX1 or TAL_ORX2 expected\n";
		case TAL_ERR_GETRXHD2STATUS_NULL_PARAM:
			return "TALISE_getRxHd2Status(): rxHd2Status is a null pointer\n";
		case TAL_ERR_GETRXHD2STATUS_INV_CHANNEL_PARM:
			return "TALISE_getRxHd2Status(): Invalid channel - TAL_RX1 or TAL_RX2 expected\n";
		case TAL_ERR_VERIFYSPI_READ_LOW_ADDR_ERROR:
			return "TALISE_verifySpiReadWrite(): Cannot read from a low SPI address\n";
		case TAL_ERR_VERIFYSPI_WRITE_LOW_ADDR_ERROR:
			return "TALISE_verifySpiReadWrite(): Cannot write to a low SPI address\n";
		case TAL_ERR_VERIFYSPI_READ_HIGH_ADDR_ERROR:
			return "TALISE_verifySpiReadWrite(): Cannot read from a high SPI address\n";
		case TAL_ERR_VERIFYSPI_WRITE_HIGH_ADDR_ERROR:
			return "TALISE_verifySpiReadWrite(): Cannot write to a high SPI address\n";
		case TAL_ERR_GETAPIVERSION_NULLPARAM:
			return "TALISE_getApiVersion(): Null parameter passed to function\n";
		case TAL_ERR_GETENABLED_TRACK_CALS_NULL_PTR:
			return "Null parameter passed to TALISE_getEnabledTrackingCals()\n";
		case TAL_ERR_INV_DAC_FULLSCALE_PARM:
			return "TALISE_setDacFullScale() Invalid enumeration for dacFullScale\n";
		case TAL_ERR_DAC_FULLSCALE_INVARMSTATE:
			return "TALISE_setDacFullScale() can only be called before Talise ARM is loaded\n";
		case TAL_ERR_RESET_TXLOL_INV_CHANNEL_PARM:
			return "TALISE_resetExtTxLolChannel() Invalid channel specified\n";
		case TAL_ERR_RESET_TXLOL_ARMSTATE_ERROR:
			return "TALISE_resetExtTxLolChannel() InitCals have not been run. ARM must be in IDLE (RadioOff) or RadioOn state\n";
		case TAL_ERR_SET_SPI2_ENABLE_INVALID_TX_ATTEN_SEL:
			return "TALISE_setSpi2Enable(): Invalid Tx atten enumeration\n";
		case TAL_ERR_SET_SPI2_ENABLE_GPIO_IN_USE:
			return "TALISE_setSpi2Enable(): GPIO already used by another function\n";
		case TAL_ERR_SET_RXDATAFRMT_NULL_PARAM:
			return "TALISE_setRxDataFormat() was passed a NULL pointer for the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_FORMATSELECT_INVPARAM:
			return "TALISE_setRxDataFormat() Parameter formatSelect is invalid\n";
		case TAL_ERR_SET_RXDATAFRMT_TEMPCOMP_INVPARAM:
			return "TALISE_setRxDataFormat() Parameter tempCompensation is invalid (Currently unsupported)\n";
		case TAL_ERR_SET_RXDATAFRMT_ROUNDMODE_INVPARAM:
			return "TALISE_setRxDataFormat() fpRoundMode out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_FPDATAFRMT_INVPARAM:
			return "TALISE_setRxDataFormat() fpDataFormat out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_FPENCNAN_INVPARAM:
			return "TALISE_setRxDataFormat() fpEncodeNan out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_FPEXPBITS_INVPARAM:
			return "TALISE_setRxDataFormat() fpNumExpBits out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_FPHIDELEADINGONE_INVPARAM:
			return "TALISE_setRxDataFormat() fpHideLeadingOne out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_FPRX1ATTEN_INVPARAM:
			return "TALISE_setRxDataFormat() fpRx1Atten out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_FPRX2ATTEN_INVPARAM:
			return "TALISE_setRxDataFormat() fpRx2Atten out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_INTEMBEDDEDBITS_INVPARAM:
			return "TALISE_setRxDataFormat() intEmbeddedBits out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_INTSAMPLERESOLUTION_INVPARAM:
			return "TALISE_setRxDataFormat() intSampleResolution out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_PINSTEPSIZE_INVPARAM:
			return "TALISE_setRxDataFormat() pinStepSize out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_RX1GPIOSELECT_INVPARAM:
			return "TALISE_setRxDataFormat() rx1GpioSelect out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_RX2GPIOSELECT_INVPARAM:
			return "TALISE_setRxDataFormat() rx2GpioSelect out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_RXCHAN_DISABLED:
			return "TALISE_setRxDataFormat() Can not set Rx data format, no Rx channel was enabled during initialization.\n";
		case TAL_ERR_SET_RXDATAFRMT_EXTERNALLNAGAIN_INVPARAM:
			return "TALISE_setRxDataFormat() externalLnaGain out of range in the configuration structure.\n";
		case TAL_ERR_SET_RXDATAFRMT_RX1GPIO_INUSE:
			return "TALISE_setRxDataFormat() Rx1 requested GPIOs already inuse.\n";
		case TAL_ERR_SET_RXDATAFRMT_RX2GPIO_INUSE:
			return "TALISE_setRxDataFormat() Rx2 requested GPIOs already inuse.\n";
		case TAL_ERR_SET_RXDATAFRMT_DATARES_INVPARAM:
			return "TALISE_setRxDataFormat() Jesd204 NP is less than the select data resolution size.\n";
		case TAL_ERR_SET_RXDATAFRMT_EXTSLICER_RX1GPIO_INVPARAM:
			return "TALISE_setRxDataFormat(): can not set rx1GpioSelect to disabled in external slicer mode\n";
		case TAL_ERR_SET_RXDATAFRMT_EXTSLICER_RX2GPIO_INVPARAM:
			return "TALISE_setRxDataFormat(): can not set rx2GpioSelect to disabled in external slicer mode\n";
		case TAL_ERR_GET_SLICERPOS_NULL_PARAM:
			return "TALISE_getSlicerPosition() was passed a NULL pointer for one of the function parameters.\n";
		case TAL_ERR_GET_DATAFORMAT_NULL_PARAM:
			return "TALISE_getRxDataFormat() was passed a NULL pointer for the rxDataFormat function parameter.\n";
		case TAL_ERR_SETRXMGCPINCTRL_INV_RX1_INC_PIN:
			return "setRxGainCtrlPin(): Invalid GPIO pin for RX1 Increment gain\n";
		case TAL_ERR_SETRXMGCPINCTRL_INV_RX1_DEC_PIN:
			return "setRxGainCtrlPin(): Invalid GPIO pin for RX1 Decrement gain\n";
		case TAL_ERR_SETRXMGCPINCTRL_INV_RX2_INC_PIN:
			return "setRxGainCtrlPin(): Invalid GPIO pin for RX2 Increment gain\n";
		case TAL_ERR_SETRXMGCPINCTRL_INV_RX2_DEC_PIN:
			return "setRxGainCtrlPin(): Invalid GPIO pin for RX2 Decrement gain\n";
		case TAL_ERR_SETRXMGCPINCTRL_INV_CHANNEL:
			return "setRxGainCtrlPin(): Invalid Channel (only TAL_RX1, TAL_RX2 allowed)\n";
		case TAL_ERR_SETRXMGCPINCTRL_INV_INC_STEP:
			return "setRxGainCtrlPin(): Invalid increment gain step (only values 0-7 allowed)\n";
		case TAL_ERR_SETRXMGCPINCTRL_INV_DEC_STEP:
			return "setRxGainCtrlPin(): Invalid decrement gain step (only values 0-7 allowed)\n";
		case TAL_ERR_SETRXMGCPINCTRL_RX1_GPIO_IN_USE:
			return "setRxGainCtrlPin(): GPIO's for configuring RX1 pin control already in use\n";
		case TAL_ERR_SETRXMGCPINCTRL_RX2_GPIO_IN_USE:
			return "setRxGainCtrlPin(): GPIO's for configuring RX2 pin control already in use\n";
		case TAL_ERR_GETRXMGCPINCTRL_INV_CHANNEL:
			return "getRxGainCtrlPin(): Invalid Channel (only TAL_RX1, TAL_RX2 allowed)\n";
		case TAL_ERR_GETRXMGCPINCTRL_NULL_PARAM:
			return "getRxGainCtrlPin(): NULL pointer passed to function\n";
		case TAL_ERR_SETRFPLL_LOOPFILTER_INV_LOOPBANDWIDTH:
			return "TALISE_setPllLoopFilter(): Invalid Loop bandwidth (Valid range between 50 and 750 kHz).\n";
		case TAL_ERR_SETRFPLL_LOOPFILTER_INV_STABILITY:
			return "TALISE_setPllLoopFilter(): Invalid loop stability (Valid range between 3 and 15).\n";
		case TAL_ERR_SETRFPLL_LOOPFILTER_INV_PLLSEL:
			return "TALISE_setPllLoopFilter(): Invalid pllName parameter\n";
		case TAL_ERR_GETRFPLL_LOOPFILTER_NULLPARAM:
			return "TALISE_getPllLoopFilter(): NULL pointer passed to function\n";
		case TAL_ERR_GETRFPLL_LOOPFILTER_INV_PLLSEL:
			return "TALISE_getPllLoopFilter(): Invalid pllName parameter\n";
		case TAL_ERR_GETDEVICEREV_NULLPARAM:
			return "TALISE_getDeviceRev(): Null revision parameter passed to function\n";
		case TAL_ERR_GETPRODUCTID_NULLPARAM:
			return "TALISE_getProductId(): Null productId parameter passed to function\n";
		case TAL_ERR_ENABLETXNCO_INV_PROFILE:
			return "TALISE_enableTxNco(): Tx Profile has not been enabled \n";
		case TAL_ERR_ENABLETXNCO_NULL_PARM:
			return "TALISE_enableTxNco(): Null txNcoTestToneCfg parameter passed to function\n";
		case TAL_ERR_ENABLETXNCO_INV_TX1_FREQ:
			return "TALISE_enableTxNco(): Tx1 frequency is out of the range (valid range is -txInputRate_kHz/2 to +txInputRate_kHz/2)\n";
		case TAL_ERR_ENABLETXNCO_INV_TX2_FREQ:
			return "TALISE_enableTxNco(): Tx2 frequency is out of the range (valid range is -txInputRate_kHz/2 to +txInputRate_kHz/2)\n";
		case TAL_ERR_SETTXATTENCTRLPIN_NULL_PARAM:
			return "TALISE_setTxAttenCtrlPin(): NULL pointer passed to function\n";
		case TAL_ERR_SETTXATTENCTRLPIN_TX1_GPIO_IN_USE:
			return "TALISE_setTxAttenCtrlPin(): GPIO's for configuring TX1 pin control already in use\n";
		case TAL_ERR_SETTXATTENCTRLPIN_TX2_GPIO_IN_USE:
			return "TALISE_setTxAttenCtrlPin(): GPIO's for configuring TX2 pin control already in use\n";
		case TAL_ERR_SETTXATTENCTRLPIN_INV_PARM:
			return "TALISE_setTxAttenCtrlPin(): Invalid attenuation step size (only values 0-31 allowed)\n";
		case TAL_ERR_SETTXATTENCTRLPIN_INV_TX1_INC_PIN:
			return "TALISE_setTxAttenCtrlPin(): Invalid GPIO pin for TX1 Increment attenuation\n";
		case TAL_ERR_SETTXATTENCTRLPIN_INV_TX1_DEC_PIN:
			return "TALISE_setTxAttenCtrlPin(): Invalid GPIO pin for TX1 Decrement attenuation\n";
		case TAL_ERR_SETTXATTENCTRLPIN_INV_TX2_INC_PIN:
			return "TALISE_setTxAttenCtrlPin(): Invalid GPIO pin for TX2 Increment attenuation\n";
		case TAL_ERR_SETTXATTENCTRLPIN_INV_TX2_DEC_PIN:
			return "TALISE_setTxAttenCtrlPin(): Invalid GPIO pin for TX2 Decrement attenuation\n";
		case TAL_ERR_SETTXATTENCTRLPIN_INV_CHANNEL:
			return "TALISE_setTxAttenCtrlPin(): Invalid Channel (only TAL_TX1, TAL_TX2 allowed)\n";
		case TAL_ERR_GETTXATTENCTRLPIN_INV_CHANNEL:
			return "TALISE_getTxAttenCtrlPin(): Invalid Channel (only TAL_TX1, TAL_TX2 allowed)\n";
		case TAL_ERR_GETTXATTENCTRLPIN_NULL_PARAM:
			return "TALISE_getTxAttenCtrlPin(): NULL pointer passed to function\n";
		case TAL_ERR_SETHD2CFG_NULL_PARAM:
			return "TALISE_setRxHd2Config: NULL pointer passed to function\n";
		case TAL_ERR_SETHD2CFG_ARMSTATE_ERROR:
			return "TALISE_setRxHd2Config: Talise ARM can not be in radioOn state when the function is called.\n";
		case TAL_ERR_GETHD2CFG_NULL_PARAM:
			return "TALISE_getRxHd2Config: NULL pointer passed to function\n";
		case TAL_ERR_GETHD2CFG_ARMSTATE_ERROR:
			return "TALISE_getRxHd2Config: Talise ARM can not be in radioOn state when the function is called.\n";
		case TAL_ERR_INITIALIZE_DDC_INV_TOTAL_M_2OR4:
			return "talInitializeDdc(): For Dualband single Rx mode, M value should be 2 or 4\n";
		case TAL_ERR_INITIALIZE_DDC_INV_TOTAL_M_4OR8:
			return "talInitializeDdc(): For Dualband dual Rx mode, M value should be 4 or 8\n";
		case TAL_ERR_INITIALIZE_DDC_NULL_PARAM:
			return "talInitializeDdc(): Has a null *init parameter\n";
		case TAL_ERR_INITIALIZE_DDC_INV_RXDDCMODE:
			return "talInitializeDdc(): Unsupported rxDdcMode in init structure\n";
		case TAL_ERR_RXNCOFTW_INVNCO:
			return "talSetRxNcoFtw(): Invalid Rx NCO channel\n";
		case TAL_ERR_INITIALIZE_DDC_INV_DEC_AT_PFIR:
			return "talInitializeDdc(): Invalid total decimation at output of PFIR in Dualband mode";
		case TAL_ERR_SETDUALBANDSETTINGS_INV_CENTER_FREQ:
			return "talSetDualBandSettings(): Center frequency of input lower band is greater than input upper band\n";
		case TAL_ERR_SETDUALBANDSETTINGS_INV_BAND_SEP:
			return "talSetDualBandSettings(): Band should be separated by atleast 10Mhz to be able to split them\n";
		case TAL_ERR_SETDUALBANDSETTINGS_INV_IN_UPPER_FREQ:
			return "talSetDualBandSettings(): Upper end of upper band input is greater than +PfirFs/2\n";
		case TAL_ERR_SETDUALBANDSETTINGS_INV_IN_LOWER_FREQ:
			return "talSetDualBandSettings(): Lower end of lower band input is less than -PfirFs/2\n";
		case TAL_ERR_SETDUALBANDSETTINGS_INV_OUT_UPPER_FREQ:
			return "talSetDualBandSettings(): Upper band output is not within -OutFs/2 and +OutFs/2\n";
		case TAL_ERR_SETDUALBANDSETTINGS_INV_OUT_LOWER_FREQ:
			return "talSetDualBandSettings(): Lower band output is not within -OutFs/2 and +OutFs/2\n";
		case TAL_ERR_SETDUALBANDSETTINGS_OUT_OVERLAP:
			return "talSetDualBandSettings(): Output upper and lower bands are overlapping\n";
		case TAL_ERR_SETDUALBANDSETTINGS_FTW_OVRG:
			return "talSetDualBandSettings(): Invalid FTW for NCO\n";
		case TAL_ERR_DUALBAND_LNA_TABLE_INV_PROFILE:
			return "TALISE_programDualBandLnaGainTable(): RX gain table channel chosen, but the Rx profile is not valid in the device data structure\n";
		case TAL_ERR_DUALBAND_LNA_TABLE_INV_INDEX:
			return "TALISE_programDualBandLnaGainTable(): Invalid number of indices. Should be between 1 and 4 (both inclusive)\n ";
		case TAL_ERR_DUALBAND_LNA_TABLE_INV_CHANNEL:
			return "TALISE_programDualBandLnaGainTable(): Invalid channel specified\n";
		case TAL_ERR_DUALBAND_LNA_TABLE_NULL_PARM:
			return "TALISE_programDualBandLnaGainTable(): Has a null *gainTablePtr parameter\n";
		case TAL_ERR_SETUPNCOSHIFTER_INV_PFIR_CORNER:
			return "talSetupNcoShifter(): Specified Rx PassBand crosses a Pfir Filter Corner\n";
		case TAL_ERR_SETUPNCOSHIFTER_INV_NCO2SHIFT:
			return "talSetupNcoShifter(): NCO2 shifted out of Rx output band\n";
		case TAL_ERR_SETUPNCOSHIFTER_INV_DDCHB_CORNER:
			return "talSetupNcoShifter(): Specified input band crosses a RxDDC HalfBand Filter Corner\n";
		case TAL_ERR_SETUPNCOSHIFTER_FTW_OVRG:
			return "talSetupNcoShifter(): Invalid FTW for NCO\n";
		case TAL_ERR_SETUPDUALBANDRX1AGC_GPIO3P3_IN_USE:
			return "TALISE_setupDualBandRxAgc(): GPIO3.3 [3:0] are already in use by another function\n";
		case TAL_ERR_SETUPDUALBANDRX2AGC_GPIO3P3_IN_USE:
			return "TALISE_setupDualBandRxAgc(): GPIO3.3 [7:4] are already in use by another function\n";
		case TAL_ERR_SETUPDUALBANDRXAGC_INV_CHANNEL:
			return "TALISE_setupDualBandRxAgc(): Invalid channel (only TAL_RX1, TAL_RX2 or TAL_RX1RX2 are allowed)\n";
		case TAL_ERR_SETUPDUALBANDRXAGC_NULL_PARAM:
			return "TALISE_setupDualBandRxAgc(): Null rxAgcCtrlDualBand parameter passed to function\n";
		case TAL_ERR_SETUPDUALBANDRXAGC_INV_PWRMARGIN:
			return "TALISE_setupDualBandRxAgc(): Invalid agcDualbandPwrMargin range (should be between 0 and 31)\n";
		case TAL_ERR_SETUPDUALBANDRXAGC_INV_DECPWR:
			return "TALISE_setupDualBandRxAgc(): Invalid decPowerDdcMeasurementDuration range (should be between 0 and 31)";
		case TAL_ERR_GETDUALBANDLNA_INV_CHANNEL:
			return "TALISE_getDualBandLnaControls(): Invalid channel (Only TAL_RX1 or TAL_RX2 are allowed\n";
		case TAL_ERR_GETDUALBANDLNA_NULL_PARAM:
			return "TALISE_getDualBandLnaControls(): Null rxDualBandLnaControls parameter passed to function\n";
		case TAL_ERR_INV_DEFA_SLEWRATE:
			return "init->jesd204Settings.deframerA.syncbOutCmosSlewRate invalid (valid: 0-3)\n";
		case TAL_ERR_INV_DEFB_SLEWRATE:
			return "init->jesd204Settings.deframerB.syncbOutCmosSlewRate invalid (valid: 0-3)\n";
		case TAL_ERR_GETTXSAMPLEPWR_NULL_PARAM:
			return "TALISE_getTxSamplePower() has null parameter: channelPower\n";
		case TAL_ERR_GETTXSAMPLEPWR_INV_TXREADCHAN:
			return "TALISE_TxSamplePower(): Invalid txChannel parameter (Select Tx1 or Tx2)\n";
		case TAL_ERR_SETPAPRO_NULL_PARAM:
			return "TALISE_setPaProtectionCfg() has Null parameter: txPaProtectCfg\n";
		case TAL_ERR_SETPAPRO_INV_AVGDURATION:
			return "TALISE_setPaProtectionCfg(): Invalid avgDuration (valid 0-14)\n";
		case TAL_ERR_SETPAPROT_INV_PEAKCNT:
			return "TALISE_setPaProtectionCfg(): Invalid peakCount (valid Si Rev 0xB1: 0-30, Si Rev 0xC0: 0-31)\n";
		case TAL_ERR_SETPAPROT_INV_TXATTENSTEP:
			return "TALISE_setPaProtectionCfg(): Invalid txAttenStep (valid 0-127)\n";
		case TAL_ERR_SETPAPROT_INV_TX1THRESH:
			return "TALISE_setPaProtectionCfg(): Invalid tx1PowerThreashold\n";
		case TAL_ERR_SETPAPROT_INV_TX2THRESH:
			return "TALISE_setPaProtectionCfg(): Invalid tx2PowerThreashold\n";
		case TAL_ERR_SETPAPROT_INV_TX1PEAKTHRESH:
			return "TALISE_setPaProtectionCfg(): Invalid tx1PeakThreashold\n";
		case TAL_ERR_SETPAPROT_INV_TX2PEAKTHRESH:
			return "TALISE_setPaProtectionCfg(): Invalid tx2PeakThreashold\n";
		case TAL_ERR_GETPAERRFLAGS_NULL_PARAM:
			return "TALISE_getPaProtectErrorFlags(): NULL function parameter: errorFlags\n";
		case TAL_ERR_GETPAPRO_NULL_PARAM:
			return "TALISE_getPaProtectionCfg: NULL function parameter: txPaProtectCfg\n";
		case TAL_ERR_SETRFPLL_INITCALS_INPROGRESS:
			return "TALISE_setRfPllFrequency(): Can not set RF PLL while InitCals are running\n";
		case TAL_ERR_DEFSTATUS_INV_COUNTERERRTHRESHOLD_PARAM:
			return "TALISE_enableDfrmErrCounter(): Selected error counter threshold out of range\n";
		case TAL_ERR_RFPLLFREQ_TX_OUT_OF_RANGE :
			return "TALISE_setRfPllFrequency() : Invalid rfpllLoFreq, rfPllLoFreq - TxProfileRFBW/2 must be > 0 (DC)\n";
		case TAL_ERR_RFPLLFREQ_RX_OUT_OF_RANGE :
			return "TALISE_setRfPllFrequency() : Invalid rfpllLoFreq, rfPllLoFreq - RxProfileRFBW/2 must be > 0 (DC)\n";
		case TAL_ERR_RFPLLFREQ_ORX_OUT_OF_RANGE :
			return "TALISE_setRfPllFrequency() : Invalid rfpllLoFreq, rfPllLoFreq - ORxProfileRFBW/2 must be > 0 (DC)\n";
		case TAL_ERR_INV_SIREV:
			return "Talise API only supports Talise B1 silicon and greater.  Older Si detected.\n";
		case TAL_ERR_REGISTER_ERRORMSG_C0:
			return "talRegisterErrorMessage() failed for Talise.\n";
		case TAL_ERR_SETEXTWORDCTRLGPIO_INV_CHANNEL :
			return "TALISE_setGainTableExtCtrlPins() : Invalid channel (Only TAL_RX1 or TAL_RX2 are allowed\n";
		case TAL_ERR_SETEXTWORDCTRLGPIO_UNINITIALIZED_RX1 :
			return "TALISE_setGainTableExtCtrlPins() : Please verify that channel Rx1 is enabled and initialized\n";
		case TAL_ERR_SETEXTWORDCTRLGPIO_UNINITIALIZED_RX2 :
			return "TALISE_setGainTableExtCtrlPins() : Please verify that channel Rx2 is enabled and initialized\n";
		case TAL_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX1 :
			return "TALISE_setGainTableExtCtrlPins() : GPIO Pins Requested for Rx1 Ext Ctrl Word Output already in use\n";
		case TAL_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX2 :
			return "TALISE_setGainTableExtCtrlPins() : GPIO Pins Requested for Rx2 Ext Ctrl Word Output already in use\n";
		case TAL_ERR_READEVENTSTATUS_NULL_PARM:
			return "Null pointer for parameter in TALISE_readEventStatus()\n";
		case TAL_ERR_FRAMERSYSREFTOGGLE_INV_FRAMERSEL_PARAM:
			return "TALISE_framerSyncbToggle(): Invalid framer select parameter\n";
		case TAL_ERR_SETORXLOSRC_INVALIDPARAM:
			return "TALISE_setOrxLoSource: Invalid orxLoSource input parameter\n";
		case TAL_ERR_GETORXLOSRC_NULLPARAM:
			return "TALISE_getOrxLoSource: Function input parameters were null\n";
		case TAL_ERR_SETORXLOSRC_TIMEDOUT_ARMMAILBOXBUSY:
			return "TALISE_setOrxLoSource: failed due to timeout waiting on ARM mailbox to become idle\n";
		case TAL_ERR_SETORXLOCFG_NULL_PARAM:
			return "TALISE_setOrxLoCfg: orxLoCfg parameter is null\n";
		case TAL_ERR_SETORXLOCFG_INVALIDPARAM:
			return "TALISE_setOrxLoCfg: Invalid gpioSelect parameter\n";
		case TAL_ERR_SETORXLOCFG_INVALID_ARMSTATE:
			return "TALISE_setOrxLoCfg: ARM must be in radio Off state\n";
		case TAL_ERR_SETORXLOCFG_GPIOUSED:
			return "TALISE_setOrxLoCfg: GPIO pin is already used\n";
		case TAL_ERR_GETORXLOCFG_NULL_PARAM:
			return "TALISE_getOrxLoCfg: orxLoCfg parameter is null\n";
		case TAL_ERR_GETORXLOCFG_INVALID_ARMSTATE:
			return "TALISE_getOrxLoCfg: ARM must be in radio Off state\n";
		case TAL_ERR_SETFHMCONFIG_NULL_PARAM :
			return "TALISE_setFhmConfig: fhmConfig parameter is null\n";
		case TAL_ERR_SETFHMCONFIG_INV_FHMGPIOPIN :
			return "TALISE_setFhmConfig: Invalid fhmGpioPin selected. Valid TAL_GPIO_00 - TAL_GPIO_15 for enabling and TAL_GPIO_INVALID for disabling\n";
		case TAL_ERR_SETFHMCONFIG_INV_FHMCONFIG_FHM_MIN_FREQ :
			return "TALISE_setFhmConfig: Invalid FHM range minimum hop frequency. fhmMinFreq - ProfileRFBW/2 must be > 0 (DC)\n";
		case TAL_ERR_SETFHMCONFIG_INV_FHMCONFIG_FHM_MAX_FREQ :
			return "TALISE_setFhmConfig: FHM range maximum frequency should be greater than minimum frequency";
		case TAL_ERR_SETFHMCONFIG_FHMGPIOPIN_IN_USE :
			return "TALISE_setFhmConfig : GPIO pin requested for Frequency hopping already in use by another feature\n";
		case TAL_ERR_GETFHMCONFIG_NULL_PARAM :
			return "TALISE_getFhmConfig : fhmConfig parameter is null\n";
		case TAL_ERR_SETFHMMODE_NULL_PARAM :
			return "TALISE_setFhmMode : fhmMode parameter is null\n";
		case TAL_ERR_SETFHMMODE_INV_FHM_INIT_FREQ :
			return "TALISE_setFhmMode : Out of range fhmInitFreq requested\n";
		case TAL_ERR_SETFHMMODE_INV_FHM_TRIGGER_MODE :
			return "TALISE_setFhmMode : Invalid FHM trigger mode requested. Valid values TAL_FHM_GPIO_MODE, TAL_FHM_NON_GPIO_MODE\n";
		case TAL_ERR_SETFHMMODE_INV_FHM_EXIT_MODE :
			return "TALISE_setFhmMode : Invalid FHM exit mode requested. Valid values are TAL_FHM_QUICK_EXIT, TAL_FHM_FULL_EXIT\n";
		case TAL_ERR_GETFHMMODE_NULL_PARAM :
			return "TALISE_getFhmMode : fhmMode parameter is null\n";
		case TAL_ERR_SETFHMHOP_INV_FHM_FREQ :
			return "TALISE_setFhmHop : nextFhmRfPllFrequency is out of scan range\n";
		case TAL_ERR_GETFHMSTS_NULL_PARAM :
			return "TALISE_getFhmStatus : fhmStatus parameter is null\n";
		case TAL_ERR_SETDCMSHIFT_INV_CH_PARAM:
			return "TALISE_setDigDcOffsetMShift(): Invalid channel parameter\n";
		case TAL_ERR_SETDCMSHIFT_INV_MSHIFT_PARAM:
			return "TALISE_setDigDcOffsetMShift(): Invalid mShift parameter\n";
		case TAL_ERR_GETDCMSHIFT_INV_CH_PARAM:
			return "TALISE_getDigDcOffsetMShift(): Invalid channel parameter\n";
		case TAL_ERR_GETDCMSHIFT_NULL_MSHIFT_PARAM:
			return "TALISE_getDigDcOffsetMShift(): NULL mShift parameter\n";
		case TAL_ERR_SETEXTLOOUT_INV_DIV_PARAM:
			return "TALISE_setExtLoOutCfg(): Invalid extLoOutDivide parameter (use enum)\n";
		case TAL_ERR_SETEXTLOOUT_LO_IN_ENABLED:
			return "TALISE_setExtLoOutCfg(): Can not enable Ext LO output because Ext LO input was enabled during init.\n";
		case TAL_ERR_GETEXTLOOUT_NULL_PARAM:
			return "TALISE_getExtLoOutCfg(): Null function parameter\n";
		case TAL_ERR_DIG_DC_OFFSET_INV_ENABLE_MASK:
			return "TALISE_setDigDcOffsetEn(): Invalid enable mask parameter\n";
		case TAL_ERR_DIG_DC_OFFSET_NULL_ENABLE_MASK:
			return "TALISE_getDigDcOffsetEn(): NULL enable mask parameter\n";
		case TAL_ERR_SETTCAL_BATCH_SIZE_PARAM:
			return "TALISE_setTrackingCalsBatchSize(): Invalid batch size parameter\n";
		case TAL_ERR_SETTCAL_BATCH_SIZE_ARMSTATE_ERROR:
			return "TALISE_setTrackingCalsBatchSize: Talise ARM can not be in radioOn state when the function is called.\n";
		case TAL_ERR_GETTCAL_BATCH_SIZE_ARMSTATE_ERROR:
			return "TALISE_getTrackingCalsBatchSize: Talise ARM can not be in radioOn state when the function is called.\n";
		case TAL_ERR_GETTCAL_BATCHSIZE_NULL_PARAM:
			return "TALISE_getTrackingCalsBatchSize(): NULL pointer passed to function\n";
		case TAL_ERR_GETTCAL_BATCHSIZE_INV_VALUE:
			return "TALISE_getTrackingCalsBatchSize(): Invalid batch size stored in ARM \n";
		case TAL_ERR_TXNCOSHIFTER_INV_PROFILE:
			return "TALISE_txNcoShifterSet(): Invalid Tx profile";
		case TAL_ERR_TXNCOSHIFTER_NULL_PARM:
			return "TALISE_txNcoShifterSet(): Error txNcoShiftCfg is NULL";
		case TAL_ERR_TXNCOSHIFTER_INV_TX1_FREQ:
			return "TALISE_txNcoShifterSet(): tx1 tone frequency > divider rate KHZ or < negative divider rate KHZ";
		case TAL_ERR_TXNCOSHIFTER_INV_TX2_FREQ:
			return "TALISE_txNcoShifterSet(): tx2 tone frequency > divider rate KHZ or < negative divider rate KHZ";
		case TAL_ERR_NUMBER_OF_ERRORS:
			return "Invalid API error passed, last in error list\n"; /*Use for TestApiErrorStrings */

		default:
			return "Invalid API error passed, not in error list\n";
		}
	} else if (errSrc == TAL_ERRSRC_TAL_API_GPIO) {
		return talGetGpioErrorMessage(errSrc, errCode);
	} else {
		return "Invalid TALISE error passed, not in error list\n";
	}

#endif
}

const char* TALISE_getRegisteredErrorMessage(taliseDevice_t *device,
		uint32_t errSrc, uint32_t errCode)
{
#ifndef TALISE_VERBOSE
	return "";
#else
	/* Get core functionality error message from TALISE_getErrorMessage() */
	if ((errSrc == TAL_ERRSRC_API) ||
	    (errSrc == TAL_ERRSRC_ADIHAL) ||
	    (errSrc == TAL_ERRSRC_TALARMCMD) ||
	    (errSrc == TAL_ERRSRC_TAL_API_GPIO) ||
	    (errSrc == TAL_ERRSRC_TALAPIARM) ||
	    (errSrc == TAL_ERRSRC_INITCALS)) {
		return TALISE_getErrorMessage(errSrc, errCode);
	}
	/* Check for any addon feature error messages */
	else {
		return talCallbackErrorMessage(device, errSrc, errCode);

	}

#endif
}

talRecoveryActions_t talApiErrHandler(taliseDevice_t *device,
				      taliseErrHdls_t errHdl,
				      uint32_t detErr, talRecoveryActions_t retVal, talRecoveryActions_t recAction)
{
	switch (errHdl) {
	case TAL_ERRHDL_HAL_GPIO: /* Fall through */
	case TAL_ERRHDL_HAL_WAIT:
		if (detErr != ADIHAL_OK) {
			talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
				      TALISE_getErrorMessage(TAL_ERRSRC_ADIHAL, detErr));
			device->devStateInfo.errSource = TAL_ERRSRC_ADIHAL;
			device->devStateInfo.errCode = detErr;
			return recAction;
		}
		return retVal;

	case TAL_ERRHDL_HAL_LOG:
		if (detErr != ADIHAL_OK) {
			device->devStateInfo.errSource = TAL_ERRSRC_ADIHAL;
			device->devStateInfo.errCode = detErr;
			return recAction;
		}
		return retVal;

	case TAL_ERRHDL_HAL_SPI:
		if (detErr != ADIHAL_OK) {
			/*Ignore Error from log as lower priority than error being handled*/
			talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
				      TALISE_getErrorMessage(TAL_ERRSRC_ADIHAL, detErr));
			device->devStateInfo.errSource = TAL_ERRSRC_ADIHAL;
			device->devStateInfo.errCode = detErr;
			return recAction;
		}
		return retVal;

	case TAL_ERRHDL_INVALID_PARAM: /* Fall through */
	case TAL_ERRHDL_API_FAIL:
		/*Ignore Error from log as lower priority than error being handled*/
		talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
			      TALISE_getErrorMessage(TAL_ERRSRC_API, detErr));
		device->devStateInfo.errSource = TAL_ERRSRC_API;
		device->devStateInfo.errCode = detErr;
		return recAction;

	case TAL_ERRHDL_APIARM_ERR:
		/*Ignore Error from log as lower priority than error being handled*/
		talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
			      TALISE_getErrorMessage(TAL_ERRSRC_TALAPIARM, detErr));
		device->devStateInfo.errSource = TAL_ERRSRC_TALAPIARM;
		device->devStateInfo.errCode = detErr;
		return recAction;

	case TAL_ERRHDL_ARM_CMD_ERR:
		return talArmCmdErrorHandler(device, errHdl, detErr, retVal, recAction);

	case TAL_ERRHDL_ARM_INITCALS_ERR:
		/*Ignore Error from log as lower priority than error being handled*/
		talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
			      TALISE_getErrorMessage(TAL_ERRSRC_INITCALS, detErr));
		device->devStateInfo.errSource = TAL_ERRSRC_INITCALS;
		device->devStateInfo.errCode = detErr;
		return recAction;

	case TAL_ERRHDL_API_GPIO:
		/*Ignore Error from log as lower priority than error being handled*/
		talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
			      TALISE_getErrorMessage(TAL_ERRSRC_TAL_API_GPIO, detErr));
		device->devStateInfo.errSource = TAL_ERRSRC_TAL_API_GPIO;
		device->devStateInfo.errCode = detErr;
		return recAction;

	case TAL_ERRHDL_API_C0_PCA:
		talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
			      TALISE_getRegisteredErrorMessage(device, TAL_ERRSRC_TAL_API_C0_PCA, detErr));
		device->devStateInfo.errSource = TAL_ERRSRC_TAL_API_C0_PCA;
		device->devStateInfo.errCode = detErr;
		return recAction;

	case TAL_ERRHDL_API_C0_PHMFOVR:
		talWriteToLog(device->devHalInfo, ADIHAL_LOG_ERR, detErr,
			      TALISE_getRegisteredErrorMessage(device, TAL_ERRSRC_TAL_API_C0_PHMFOVR,
					      detErr));
		device->devStateInfo.errSource = TAL_ERRSRC_TAL_API_C0_PHMFOVR;
		device->devStateInfo.errCode = detErr;
		return recAction;

	default:
		return retVal;
	}
}

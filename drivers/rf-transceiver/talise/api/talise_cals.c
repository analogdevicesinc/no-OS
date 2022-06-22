// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_cals.c
 * \brief Contains functions to support Talise init and tracking calibrations
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_cals.h"
#include "talise_reg_addr_macros.h"
#include "talise_arm_macros.h"
#include "talise_hal.h"
#include "talise_user.h"
#include "talise_error.h"
#include "talise_radioctrl.h"
#include "talise_arm.h"
#include "talise.h"

uint32_t TALISE_runInitCals(taliseDevice_t *device, uint32_t calMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t payload[4] = {0};

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_runInitCals()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	payload[0] = (uint8_t)calMask;
	payload[1] = (uint8_t)(calMask >> 8);
	payload[2] = (uint8_t)(calMask >> 16);
	payload[3] = (uint8_t)(calMask >> 24);

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_RUNINIT_OPCODE, &payload[0], 4);
	IF_ERR_RETURN_U32(retVal);

	device->devStateInfo.devState = (taliseStates_t)(device->devStateInfo.devState |
					TAL_STATE_CALS_RUN);

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_waitInitCals(taliseDevice_t *device, uint32_t timeoutMs,
			     uint8_t *errorFlag)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t _errFlag = 0;

	static const uint16_t TIMEOUT_MS_FACTOR = 1000;
	static const uint32_t CODECHECK_PARAM_WAITINITCALS_ERR = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_waitInitCals()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (errorFlag == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_WAIT_INITCALS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retValWarn = retVal;

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_RUNINIT_OPCODE, &cmdStatusByte, (timeoutMs * TIMEOUT_MS_FACTOR),
			WAITINITCALS_INTERVAL_US);
	_errFlag = (cmdStatusByte >> 1);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_WAITINITCALS_ERR) {
		retVal = TALACT_ERR_RESET_SPI;
		cmdStatusByte = 2;
		_errFlag = (cmdStatusByte >> 1);
	}

	/* Don't update errorFlag if SPI error because errorFlag could be a random */
	/* value but update error flag for other recovery action types */
	if (retVal == TALACT_ERR_RESET_SPI) {
		*errorFlag = 0;
	} else {
		*errorFlag = _errFlag;
	}

	/* ARM error handler to provide valid recovery action based on ARM error
	 * code */
	if (_errFlag > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* if no error from higher priority calls, return possible log warning */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_checkInitCalComplete(taliseDevice_t *device,
				     uint8_t *areCalsRunning, uint8_t *errorFlag)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t armErrorFlag = 0;

	static const uint32_t CODECHECK_PARAM_CHECKINITCALCOMPLETE_ERR3 = 3;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
		      "TALISE_checkInitCalComplete()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (areCalsRunning == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_CHKINITCALS_NULL_PTR, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* read the command status register for the initialization calibration opcode */
	retVal = (talRecoveryActions_t)TALISE_readArmCmdStatusByte(device,
			TALISE_ARM_RUNINIT_OPCODE, &cmdStatusByte);
	/* don't check cmdStatusByte until return parameters are updated */

	armErrorFlag = (cmdStatusByte >> 1);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_CHECKINITCALCOMPLETE_ERR3) {
		retVal = TALACT_ERR_RESET_SPI;
		cmdStatusByte = 2;
		armErrorFlag = (cmdStatusByte >> 1);
	}

	if (retVal == TALACT_ERR_RESET_SPI) {
		*areCalsRunning = 0;
	} else {
		*areCalsRunning = (cmdStatusByte & 0x01);
	}

	/* Don't update errorFlag if SPI error because errorFlag could be a random */
	/* value but update error flag for other recovery action types */
	if (errorFlag != NULL) {
		if (retVal == TALACT_ERR_RESET_SPI) {
			*errorFlag = 0;
		} else {
			*errorFlag = armErrorFlag;
		}
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_RUNINIT_OPCODE, 0, cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_abortInitCals(taliseDevice_t *device, uint32_t *calsCompleted)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t calCompleteBitField[4] = {0};
	uint8_t extData[1] = {TALISE_ARM_OBJECTID_INITCAL_STATUS};

	static const uint8_t talArmAddrOffset = 4;
	static const uint32_t CODECHECK_PARAM_ABORTINITCALS_ERR2 = 2;
	static const uint32_t CODECHECK_PARAM_ABORTINITCALS_ERR3 = 3;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_abortInitCals()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* send ARM abort opcode */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_ABORT_OPCODE, 0, 0);
	IF_ERR_RETURN_U32(retVal);

	/* wait for command completion */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_ABORT_OPCODE, &cmdStatusByte, ABORTINITCALS_TIMEOUT_US,
			ABORTINITCALS_INTERVAL_US);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_ABORTINITCALS_ERR2) {
		cmdStatusByte = 2;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_ABORT_OPCODE, 0, cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	if (calsCompleted != NULL) {
		/* send ARM get opcode for the init cals completed */
		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_GET_OPCODE, &extData[0], sizeof(extData));
		IF_ERR_RETURN_U32(retVal);

		/* wait for command completion */
		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_GET_OPCODE, &cmdStatusByte, ABORTINITCALS_TIMEOUT_US,
				ABORTINITCALS_INTERVAL_US);

		/* SW Test */
		if (device->devStateInfo.swTest == CODECHECK_PARAM_ABORTINITCALS_ERR3) {
			cmdStatusByte = 2;
		}

		if((cmdStatusByte >> 1) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
							  TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}

		/* read the ARM memory for the init cals completed bit field */
		retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
				TALISE_ADDR_ARM_START_DATA_ADDR + talArmAddrOffset, &calCompleteBitField[0], 4,
				0);
		IF_ERR_RETURN_U32(retVal);

		/* form the calsComplted word and return if requested */
		*calsCompleted = (((uint32_t)calCompleteBitField[3]) << 24) | (((
					 uint32_t)calCompleteBitField[2]) << 16) | (((uint32_t)calCompleteBitField[1]) <<
							 8) | (((uint32_t)calCompleteBitField[0]));
	}

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getInitCalStatus(taliseDevice_t *device,
				 uint32_t *calsSincePowerUp, uint32_t *calsLastRun, uint32_t *calsMinimum,
				 uint8_t *initErrCal, uint8_t *initErrCode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t calBitField[14] = {0};
	static const uint8_t talInitCalDoneObjectID = 0x43;
	static const uint8_t byteCnt = 14;
	static const uint32_t CODECHECK_PARAM_GETINITCALSTATUS_ERR1 = 6;
	static const uint32_t CODECHECK_PARAM_GETINITCALSTATUS_ERR2 = 7;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getInitCalStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* performing null pointer checks before executing any code */
	if (calsSincePowerUp == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INIT_CALS_COMPLETED_NULL_PTR, retVal, TALACT_ERR_CHECK_PARAM);
	} else if (calsLastRun == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INIT_CALS_LASTRUN_NULL_PTR, retVal, TALACT_ERR_CHECK_PARAM);
	} else if (calsMinimum == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INIT_CALS_MIN_NULL_PTR, retVal, TALACT_ERR_CHECK_PARAM);
	} else if (initErrCal == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INIT_ERR_CAL_NULL_PTR, retVal, TALACT_ERR_CHECK_PARAM);
	} else if (initErrCode == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INIT_ERR_CODE_NULL_PTR, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* sending get command to ARM */
		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_GET_OPCODE, &talInitCalDoneObjectID, 1);
		IF_ERR_RETURN_U32(retVal);

		/* waiting for command completion */
		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETINITCALSTATUS_TIMEOUT_US,
				GETINITCALSTATUS_INTERVAL_US);

		/* SW Test */
		if (device->devStateInfo.swTest == CODECHECK_PARAM_GETINITCALSTATUS_ERR1) {
			cmdStatusByte = 2;
		}

		if((cmdStatusByte >> 1) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, talInitCalDoneObjectID, cmdStatusByte),
							  retVal, TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}

		/* reading the calibration bit field value */
		retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
				TALISE_ADDR_ARM_START_DATA_ADDR, &calBitField[0], byteCnt, 0);
		IF_ERR_RETURN_U32(retVal);

		/* getting the number of calibrations since power up */
		*calsSincePowerUp = (((uint32_t)calBitField[3]) << 24) | (((
					    uint32_t)calBitField[2]) << 16) | (((uint32_t)calBitField[1]) << 8) | (((
							    uint32_t)calBitField[0]));

		/* getting the number of calibrations since the last sequence */
		*calsLastRun = (((uint32_t)calBitField[7]) << 24) | (((uint32_t)calBitField[6])
				<< 16) | (((uint32_t)calBitField[5]) << 8) | (((uint32_t)calBitField[4]));

		/* getting the minimum number of initialization calibrations required */
		*calsMinimum = (((uint32_t)calBitField[11]) << 24) | (((
					uint32_t)calBitField[10]) << 16) | (((uint32_t)calBitField[9]) << 8) | (((
							uint32_t)calBitField[8]));

		/* getting the object ID reporting the error */
		*initErrCal = calBitField[12];

		/* getting the offending error code */
		*initErrCode = calBitField[13];

		/* SW Test */
		if (device->devStateInfo.swTest == CODECHECK_PARAM_GETINITCALSTATUS_ERR2) {
			*initErrCal = 1;
			*initErrCode = 1;
		}

		if (*initErrCode > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_INITCALS_ERR,
							  ARMINITCAL_ERRCODE(*initErrCal, *initErrCode), retVal, TALACT_ERR_RESET_ARM);
		}
	}

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_enableTrackingCals(taliseDevice_t *device, uint32_t enableMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t armData[4] = {0};
	uint8_t cmdStatusByte = 0;
	uint32_t radioStatus = 0;

	/* Target ARM Object ID, Offset LSB, Offset MSB, Length */
	static const uint8_t extData[4] = {TALISE_ARM_OBJECTID_CAL_SCHEDULER, 0x00, 0x00, 0x02};
	static const uint8_t ARM_RADIOSTATE_IDLE = 0x02;
	static const uint32_t CODECHECK_PARAM_ENABLETRACKINGCALS_ERR1 = 1;
	static const uint32_t CODECHECK_PARAM_ENABLETRACKINGCALS_ERR2 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_enableTrackingCals()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* read radio state to make sure ARM is in radioOff/IDLE mode */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_ENABLETRACKINGCALS_ERR1) {
		radioStatus = ~ARM_RADIOSTATE_IDLE;
		retVal = TALACT_WARN_RERUN_TRCK_CAL;
	}

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_ENABLETRACKINGCALS_ERR2) {
		radioStatus = ARM_RADIOSTATE_IDLE;
	}

	/* throw error if not in radioOff/IDLE state */
	if ((radioStatus & 0x07) != ARM_RADIOSTATE_IDLE) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_EN_TRACKING_CALS_ARMSTATE_ERROR, retVal, TALACT_ERR_RESET_ARM);
	}

	/* populating armData array */
	armData[0] = (uint8_t)(enableMask & 0xFF);
	armData[1] = (uint8_t)((enableMask >>  8) & 0xFF);
	armData[2] = (uint8_t)((enableMask >> 16) & 0xFF);
	armData[3] = (uint8_t)((enableMask >> 24) & 0xFF);

	/* writing armData to ARM memory */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armData[0], 4);
	IF_ERR_RETURN_U32(retVal);

	/* sending write config command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_WRITECFG_OPCODE, &extData[0], 4);
	IF_ERR_RETURN_U32(retVal);

	/* waiting for command to finish successfully */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_WRITECFG_OPCODE, &cmdStatusByte, ENTRACKINGCALS_TIMEOUT_US,
			ENTRACKINGCALS_INTERVAL_US);
	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_ENABLETRACKINGCALS_ERR2) {
		cmdStatusByte = 3;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_WRITECFG_OPCODE, TALISE_ARM_OBJECTID_CAL_SCHEDULER,
								  cmdStatusByte), retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getEnabledTrackingCals(taliseDevice_t *device,
				       uint32_t *enableMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t armData[4] = {0};

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getEnabledTrackingCals()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if(enableMask == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETENABLED_TRACK_CALS_NULL_PTR, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* sending read config command */
	retVal = (talRecoveryActions_t)TALISE_readArmConfig(device,
			TALISE_ARM_OBJECTID_CAL_SCHEDULER, 0, &armData[0], 4);
	IF_ERR_RETURN_U32(retVal);

	*enableMask = ((uint32_t)(armData[0]) | ((uint32_t)(armData[1]) << 8) | ((
				uint32_t)(armData[2]) << 16) | ((uint32_t)(armData[3]) << 24));

	return (uint32_t)retVal;
}

uint32_t TALISE_getPendingTrackingCals(taliseDevice_t *device,
				       uint32_t *pendingCalMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	uint8_t readData[4] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getPendingTrackingCals()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (pendingCalMask == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETPENDINGTRACKINGCALS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* read back the RX Tracking cal status */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_ARM_CMD_STATUS_11,
				  &readData[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* read back the TX Tracking cal status */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_ARM_CMD_STATUS_12,
				  &readData[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*pendingCalMask = ((uint32_t)readData[1] << 8) | ((uint32_t)readData[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_rescheduleTrackingCal(taliseDevice_t *device,
				      taliseTrackingCalibrations_t trackingCal)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t extData[3] = {0};
	uint8_t cmdStatusByte = 0;

	static const uint32_t CODECHECK_PARAM_RESCHEDULETRACKINGCAL_ERR1 = 1;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_rescheduleTrackingCal()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	switch (trackingCal) {
	case TAL_TRACK_RX1_QEC:
		extData[1] = TALISE_ARM_OBJECTID_RXQEC_TRACKING;
		extData[2] = 0;
		break;

	case TAL_TRACK_RX2_QEC:
		extData[1] = TALISE_ARM_OBJECTID_RXQEC_TRACKING;
		extData[2] = 1;
		break;

	case TAL_TRACK_ORX1_QEC:
		extData[1] = TALISE_ARM_OBJECTID_ORXQEC_TRACKING;
		extData[2] = 0;
		break;

	case TAL_TRACK_ORX2_QEC:
		extData[1] = TALISE_ARM_OBJECTID_ORXQEC_TRACKING;
		extData[2] = 1;
		break;

	case TAL_TRACK_TX1_LOL:
		extData[1] = TALISE_ARM_OBJECTID_TXLOL_TRACKING;
		extData[2] = 0;
		break;

	case TAL_TRACK_TX2_LOL:
		extData[1] = TALISE_ARM_OBJECTID_TXLOL_TRACKING;
		extData[2] = 1;
		break;

	case TAL_TRACK_TX1_QEC:
		extData[1] = TALISE_ARM_OBJECTID_TXQEC_TRACKING;
		extData[2] = 0;
		break;

	case TAL_TRACK_TX2_QEC:
		extData[1] = TALISE_ARM_OBJECTID_TXQEC_TRACKING;
		extData[2] = 1;
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_TRACKINGCAL_OUTOFRANGE_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	extData[0] = TALISE_ARM_OBJECTID_TRACKING_CAL_PENDING;

	/* sending ARM command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &extData[0], 3);
	IF_ERR_RETURN_U32(retVal);

	/* check for completion */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE, &cmdStatusByte, RESCHEDULETRACKINGCALS_TIMEOUT_US,
			RESCHEDULETRACKINGCALS_INTERVAL_US);
	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_RESCHEDULETRACKINGCAL_ERR1) {
		cmdStatusByte = 2;
	}
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setAllTrackCalState(taliseDevice_t *device,
				    uint32_t calSubsetMask, uint32_t resumeCalMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t cfgData[8] = {0};
	uint8_t extData[2] = {TALISE_ARM_OBJECTID_TRACKING_CAL_SUSPEND_RESUME, 0x0F};
	uint8_t cmdStatusByte = 0;

	static const uint32_t CODECHECK_PARAM_SETALLTRACKCALSTATE_ERR1 = 1;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setAllTrackCalState()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Hold possible warning and return at end of function if no other higher */
	/* priority errors */
	retValWarn = retVal;

	/* Range check that the input masks are within the valid tracking cal bit mask */
	if ((calSubsetMask > (uint32_t)TAL_TRACK_ALL) ||
	    (resumeCalMask > (uint32_t)TAL_TRACK_ALL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_PAUSETRACKINGCAL_INV_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* convert tracking mask to array of uint8_t type */
	cfgData[0] = (uint8_t)(resumeCalMask & 0xFF);
	cfgData[1] = (uint8_t)((resumeCalMask >> 8) & 0xFF);
	cfgData[2] = (uint8_t)((resumeCalMask >> 16) & 0xFF);
	cfgData[3] = (uint8_t)((resumeCalMask >> 24) & 0xFF);

	cfgData[4] = (uint8_t)(calSubsetMask & 0xFF);
	cfgData[5] = (uint8_t)((calSubsetMask >> 8) & 0xFF);
	cfgData[6] = (uint8_t)((calSubsetMask >> 16) & 0xFF);
	cfgData[7] = (uint8_t)((calSubsetMask >> 24) & 0xFF);

	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &cfgData[0], sizeof(cfgData));
	IF_ERR_RETURN_U32(retVal);

	/* sending ARM command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &extData[0], sizeof(extData));
	IF_ERR_RETURN_U32(retVal);

	/* check for completion */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE,
			&cmdStatusByte,
			PAUSERESUMETRACKINGCALS_TIMEOUT_US,
			PAUSERESUMETRACKINGCALS_INTERVAL_US);
	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_SETALLTRACKCALSTATE_ERR1) {
		cmdStatusByte = 2;
	}
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getAllTrackCalState(taliseDevice_t *device,
				    uint32_t *resumeCalMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint8_t extData[1] = {TALISE_ARM_OBJECTID_TRACKING_CAL_SUSPEND_RESUME};
	uint8_t armData[4] = {0};
	uint8_t cmdStatusByte = 0;

	static const uint32_t CODECHECK_PARAM_GETALLTRACKCALSTATE_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getAllTrackCalState()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Hold possible warning and return at end of function if no other higher */
	/* priority errors */
	retValWarn = retVal;

	if (resumeCalMask == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETPAUSECALSTATE_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* sending ARM command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &extData[0], 1);
	IF_ERR_RETURN_U32(retVal);

	/* check for completion */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE,
			&cmdStatusByte,
			PAUSERESUMETRACKINGCALS_TIMEOUT_US,
			PAUSERESUMETRACKINGCALS_INTERVAL_US);
	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETALLTRACKCALSTATE_ERR1) {
		cmdStatusByte = 2;
	}
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read the response back from ARM memory */
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armData[0], sizeof(armData), 0);
	IF_ERR_RETURN_U32(retVal);

	*resumeCalMask =  (uint32_t)armData[0] | ((uint32_t)armData[1] << 8) | ((
				  uint32_t)armData[2] << 16) | ((uint32_t)armData[3] << 24);

	/* ARM returns a 32bit value with unused bits set high, mask with valid cal
	 * bits to only show the resume bits for the valid cals. */
	*resumeCalMask &= (uint32_t)TAL_TRACK_ALL;

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getTxLolStatus(taliseDevice_t *device,
			       taliseTxChannels_t channelSel, taliseTxLolStatus_t *txLolStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t extData[3] = {TALISE_ARM_OBJECTID_CAL_STATUS, TALISE_ARM_OBJECTID_TXLOL_TRACKING, 0};
	uint8_t cmdStatusByte = 0;
	uint8_t armReadBack[20] = {0};

	static const uint32_t CODECHECK_PARAM_GETTXLOWSTATUS_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getTxLolStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check for null */
	if (txLolStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXLOLSTATUS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Channel */
	switch (channelSel) {
	case TAL_TX1 :
		extData[2] = 0;
		break;
	case TAL_TX2 :
		extData[2] = 1;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXLOLSTATUS_INV_CHANNEL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &extData[0], 3);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETTXLOLSTATUS_TIMEOUT_US,
			GETTXLOLSTATUS_INTERVAL_US);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETTXLOWSTATUS_ERR1) {
		cmdStatusByte = 2;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read the ARM's status structure (20 bits)*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armReadBack[0], sizeof(armReadBack), 0);
	IF_ERR_RETURN_U32(retVal);

	/* Assign to data structure */
	txLolStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((
					 uint32_t)armReadBack[2]) << 16) |
				 (((uint32_t)armReadBack[1]) <<  8) |  ((uint32_t)armReadBack[0]);
	txLolStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((
					       uint32_t)armReadBack[6]) << 16) |
				       (((uint32_t)armReadBack[5]) <<  8) |  ((uint32_t)armReadBack[4]);
	txLolStatus->varianceMetric = (((uint32_t)armReadBack[11]) << 24) | (((
					      uint32_t)armReadBack[10]) << 16) |
				      (((uint32_t)armReadBack[ 9]) <<  8) |  ((uint32_t)armReadBack[ 8]);
	txLolStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((
					 uint32_t)armReadBack[14]) << 16) |
				 (((uint32_t)armReadBack[13]) <<  8) |  ((uint32_t)armReadBack[12]);
	txLolStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((
					   uint32_t)armReadBack[18]) << 16) |
				   (((uint32_t)armReadBack[17]) <<  8) |  ((uint32_t)armReadBack[16]);

	return (uint32_t)retVal;
}

uint32_t TALISE_getTxQecStatus(taliseDevice_t *device,
			       taliseTxChannels_t channelSel, taliseTxQecStatus_t *txQecStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t extData[3] = {TALISE_ARM_OBJECTID_CAL_STATUS, TALISE_ARM_OBJECTID_TXQEC_TRACKING, 0};
	uint8_t cmdStatusByte = 0;
	uint8_t armReadBack[20] = {0};

	static const uint32_t CODECHECK_PARAM_GETTXQECSTATUS_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getTxQecStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check for null */
	if (txQecStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXQECSTATUS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Channel */
	switch (channelSel) {
	case TAL_TX1 :
		extData[2] = 0;
		break;
	case TAL_TX2 :
		extData[2] = 1;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTXQECSTATUS_INV_CHANNEL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &extData[0], 3);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETTXQECSTATUS_TIMEOUT_US,
			GETTXQECSTATUS_INTERVAL_US);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETTXQECSTATUS_ERR1) {
		cmdStatusByte = 2;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read the ARM's status structure (20 bits)*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armReadBack[0], sizeof(armReadBack), 0);
	IF_ERR_RETURN_U32(retVal);

	/* Assign to data structure */
	txQecStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((
					 uint32_t)armReadBack[2]) << 16) |
				 (((uint32_t)armReadBack[1]) <<  8) |  ((uint32_t)armReadBack[0]);
	txQecStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((
					       uint32_t)armReadBack[6]) << 16) |
				       (((uint32_t)armReadBack[5]) <<  8) |  ((uint32_t)armReadBack[4]);
	txQecStatus->correctionMetric = (((uint32_t)armReadBack[11]) << 24) | (((
						uint32_t)armReadBack[10]) << 16) |
					(((uint32_t)armReadBack[ 9]) <<  8) |  ((uint32_t)armReadBack[ 8]);
	txQecStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((
					 uint32_t)armReadBack[14]) << 16) |
				 (((uint32_t)armReadBack[13]) <<  8) |  ((uint32_t)armReadBack[12]);
	txQecStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((
					   uint32_t)armReadBack[18]) << 16) |
				   (((uint32_t)armReadBack[17]) <<  8) |  ((uint32_t)armReadBack[16]);

	return (uint32_t)retVal;
}

uint32_t TALISE_getRxQecStatus(taliseDevice_t *device,
			       taliseRxChannels_t channelSel, taliseRxQecStatus_t *rxQecStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t extData[3] = {TALISE_ARM_OBJECTID_CAL_STATUS, TALISE_ARM_OBJECTID_RXQEC_TRACKING, 0};
	uint8_t cmdStatusByte = 0;
	uint8_t armReadBack[20] = {0};

	static const uint32_t CODECHECK_PARAM_GETRXQECSTATUS_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRxQecStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check for null */
	if (rxQecStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXQECSTATUS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Channel */
	switch (channelSel) {
	case TAL_RX1 :
		extData[2] = 0;
		break;
	case TAL_RX2 :
		extData[2] = 1;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXQECSTATUS_INV_CHANNEL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &extData[0], 3);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETRXQECSTATUS_TIMEOUT_US,
			GETRXQECSTATUS_INTERVAL_US);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETRXQECSTATUS_ERR1) {
		cmdStatusByte = 2;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read the ARM's status structure (20 bits)*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armReadBack[0], sizeof(armReadBack), 0);
	IF_ERR_RETURN_U32(retVal);

	/* Assign to data structure */
	rxQecStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((
					 uint32_t)armReadBack[2]) << 16) |
				 (((uint32_t)armReadBack[1]) <<  8) |  ((uint32_t)armReadBack[0]);
	rxQecStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((
					       uint32_t)armReadBack[6]) << 16) |
				       (((uint32_t)armReadBack[5]) <<  8) |  ((uint32_t)armReadBack[4]);
	rxQecStatus->selfcheckIrrDb = (((uint32_t)armReadBack[11]) << 24) | (((
					      uint32_t)armReadBack[10]) << 16) |
				      (((uint32_t)armReadBack[ 9]) <<  8) |  ((uint32_t)armReadBack[ 8]);
	rxQecStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((
					 uint32_t)armReadBack[14]) << 16) |
				 (((uint32_t)armReadBack[13]) <<  8) |  ((uint32_t)armReadBack[12]);
	rxQecStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((
					   uint32_t)armReadBack[18]) << 16) |
				   (((uint32_t)armReadBack[17]) <<  8) |  ((uint32_t)armReadBack[16]);

	return (uint32_t)retVal;
}

uint32_t TALISE_getOrxQecStatus(taliseDevice_t *device,
				taliseObsRxChannels_t channelSel, taliseOrxQecStatus_t *orxQecStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t extData[3] = {TALISE_ARM_OBJECTID_CAL_STATUS, TALISE_ARM_OBJECTID_ORXQEC_TRACKING, 0};
	uint8_t cmdStatusByte = 0;
	uint8_t armReadBack[20] = {0};

	static const uint32_t CODECHECK_PARAM_GETORXQECSTATUS_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getOrxQecStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check for null */
	if (orxQecStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETORXQECSTATUS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Channel */
	switch (channelSel) {
	case TAL_ORX1:
		extData[2] = 0;
		break;
	case TAL_ORX2 :
		extData[2] = 1;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETORXQECSTATUS_INV_CHANNEL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &extData[0], 3);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETORXQECSTATUS_TIMEOUT_US,
			GETORXQECSTATUS_INTERVAL_US);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETORXQECSTATUS_ERR1) {
		cmdStatusByte = 2;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read the ARM's status structure (20 bits)*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armReadBack[0], sizeof(armReadBack), 0);
	IF_ERR_RETURN_U32(retVal);

	/* Assign to data structure */
	orxQecStatus->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((
					  uint32_t)armReadBack[2]) << 16) |
				  (((uint32_t)armReadBack[1]) <<  8) |  ((uint32_t)armReadBack[0]);
	orxQecStatus->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((
						uint32_t)armReadBack[6]) << 16) |
					(((uint32_t)armReadBack[5]) <<  8) |  ((uint32_t)armReadBack[4]);
	orxQecStatus->selfcheckIrrDb = (((uint32_t)armReadBack[11]) << 24) | (((
					       uint32_t)armReadBack[10]) << 16) |
				       (((uint32_t)armReadBack[ 9]) <<  8) |  ((uint32_t)armReadBack[ 8]);
	orxQecStatus->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((
					  uint32_t)armReadBack[14]) << 16) |
				  (((uint32_t)armReadBack[13]) <<  8) |  ((uint32_t)armReadBack[12]);
	orxQecStatus->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((
					    uint32_t)armReadBack[18]) << 16) |
				    (((uint32_t)armReadBack[17]) <<  8) |  ((uint32_t)armReadBack[16]);

	return (uint32_t)retVal;
}

uint32_t TALISE_getRxHd2Status(taliseDevice_t *device,
			       taliseRxChannels_t channelSel, taliseRxHd2Status_t *rxHd2Status)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t extData[3] = {TALISE_ARM_OBJECTID_CAL_STATUS, TALISE_ARM_OBJECTID_RXHD2_TRACKING, 0};
	uint8_t cmdStatusByte = 0;
	uint8_t armReadBack[20] = {0};

	static const uint32_t CODECHECK_PARAM_GETRXHD2STATUS_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRxHd2Status()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check for null */
	if (rxHd2Status == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXHD2STATUS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check Channel */
	switch (channelSel) {
	case TAL_RX1 :
		extData[2] = 0;
		break;
	case TAL_RX2 :
		extData[2] = 1;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXHD2STATUS_INV_CHANNEL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &extData[0], 3);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETRXHD2STATUS_TIMEOUT_US,
			GETRXHD2STATUS_INTERVAL_US);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETRXHD2STATUS_ERR1) {
		cmdStatusByte = 2;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read the ARM's status structure (20 bits)*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armReadBack[0], sizeof(armReadBack), 0);
	IF_ERR_RETURN_U32(retVal);

	/* Assign to data structure */
	rxHd2Status->errorCode = (((uint32_t)armReadBack[3]) << 24) | (((
					 uint32_t)armReadBack[2]) << 16) |
				 (((uint32_t)armReadBack[1]) <<  8) |  ((uint32_t)armReadBack[0]);
	rxHd2Status->percentComplete = (((uint32_t)armReadBack[7]) << 24) | (((
					       uint32_t)armReadBack[6]) << 16) |
				       (((uint32_t)armReadBack[5]) <<  8) |  ((uint32_t)armReadBack[4]);
	rxHd2Status->confidenceLevel = (((uint32_t)armReadBack[11]) << 24) | (((
					       uint32_t)armReadBack[10]) << 16) |
				       (((uint32_t)armReadBack[ 9]) <<  8) |  ((uint32_t)armReadBack[ 8]);
	rxHd2Status->iterCount = (((uint32_t)armReadBack[15]) << 24) | (((
					 uint32_t)armReadBack[14]) << 16) |
				 (((uint32_t)armReadBack[13]) <<  8) |  ((uint32_t)armReadBack[12]);
	rxHd2Status->updateCount = (((uint32_t)armReadBack[19]) << 24) | (((
					   uint32_t)armReadBack[18]) << 16) |
				   (((uint32_t)armReadBack[17]) <<  8) |  ((uint32_t)armReadBack[16]);

	return (uint32_t)retVal;
}

uint32_t TALISE_waitForEvent(taliseDevice_t *device,
			     taliseWaitEvent_t waitEvent, uint32_t timeout_us, uint32_t waitInterval_us)
{
	uint32_t halError = ADIHAL_OK;
	uint32_t errCode = TAL_ERR_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	talRecoveryActions_t eventTimeOutAction = TALACT_NO_ACTION;
	uint16_t spiAddr=0;
	uint8_t  spiBit=0;
	uint8_t  doneBitLevel=0;
	uint8_t  data=0;
	uint32_t numEventChecks = 1;
	uint32_t eventCheck = 0;

	static const uint32_t CODECHECK_PARAM_WAITFOREVENT_ERR1 = 1;
	static const uint32_t CODECHECK_PARAM_WAITFOREVENT_ERR2 = 2;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_waitForEvent()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	switch(waitEvent) {
	case TAL_CLKPLLCP:
		spiAddr = TALISE_ADDR_CLK_SYNTH_CAL;
		spiBit = 5;
		doneBitLevel = 1;
		errCode = TAL_ERR_WAITFOREVENT_TIMEDOUT_CLKPLLCP;
		eventTimeOutAction = TALACT_ERR_RESET_FULL;
		break;
	case TAL_CLKPLL_LOCK:
		spiAddr = TALISE_ADDR_CLK_SYNTH_VCO_BAND_BYTE1;
		spiBit = 0;
		doneBitLevel = 1;
		errCode = TAL_ERR_WAITFOREVENT_TIMEDOUT_CLKPLL_LOCK;
		eventTimeOutAction = TALACT_ERR_RESET_FULL;
		break;
	case TAL_RFPLLCP:
		spiAddr = TALISE_ADDR_RF_SYNTH_CAL;
		spiBit = 5;
		doneBitLevel = 1;
		errCode = TAL_ERR_WAITFOREVENT_TIMEDOUT_RFPLLCP;
		eventTimeOutAction = TALACT_ERR_RESET_FULL;
		break;
	case TAL_RFPLL_LOCK:
		spiAddr = TALISE_ADDR_RF_SYNTH_VCO_BAND_BYTE1;
		spiBit = 0;
		doneBitLevel = 1;
		errCode = TAL_ERR_WAITFOREVENT_TIMEDOUT_RFPLL_LOCK;
		eventTimeOutAction = TALACT_ERR_RESET_FULL;
		break;
	case TAL_AUXPLLCP:
		spiAddr = TALISE_ADDR_AUX_SYNTH_CAL;
		spiBit = 5;
		doneBitLevel = 1;
		errCode = TAL_ERR_WAITFOREVENT_TIMEDOUT_AUXPLLCP;
		eventTimeOutAction = TALACT_ERR_RESET_ARM;
		break;
	case TAL_AUXPLL_LOCK:
		spiAddr = TALISE_ADDR_AUX_SYNTH_VCO_BAND_BYTE1;
		spiBit = 0;
		doneBitLevel = 1;
		errCode = TAL_ERR_WAITFOREVENT_TIMEDOUT_AUXPLL_LOCK;
		eventTimeOutAction = TALACT_ERR_RESET_ARM;
		break;
	case TAL_ARMBUSY:
		spiAddr = TALISE_ADDR_ARM_COMMAND;
		spiBit = 7;
		doneBitLevel = 0;
		errCode = TAL_ERR_WAITFOREVENT_TIMEDOUT_ARMBUSY;
		eventTimeOutAction = TALACT_ERR_RESET_ARM;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_WAITFOREVENT_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
	numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

	for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++) {
		halError = talSpiReadByte(device->devHalInfo, spiAddr, &data);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* For SW verification tests, allow API to think all cals are complete */
#ifdef TAL_CALS_DONE_SWDEBUG
		if (doneBitLevel == 0) {
			data = 0x00;
		} else {
			data = 0xFF;
		}
#endif
		/* SW Test */
		if (device->devStateInfo.swTest == CODECHECK_PARAM_WAITFOREVENT_ERR1) {
			data  = 0;
			spiBit = 0;
			doneBitLevel = 1;
		}
		if ((((data >> spiBit) & 0x01) != doneBitLevel) &&
		    (eventCheck < numEventChecks)) {
			halError = ADIHAL_wait_us(device->devHalInfo, waitInterval_us);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_WAIT, halError, retVal,
						  TALACT_ERR_CHECK_TIMER);
			IF_ERR_RETURN_U32(retVal);
		} else {
			break;
		}
	}

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_WAITFOREVENT_ERR2) {
		data  = 0;
		spiBit = 0;
		doneBitLevel = 1;
	}
	if (((data >> spiBit) & 0x01) != doneBitLevel) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  errCode, retVal, eventTimeOutAction);
	} else {
		/* Lowest priority, return possible log warning */
		return (uint32_t)retValWarn;
	}
}

uint32_t TALISE_readEventStatus(taliseDevice_t *device,
				taliseWaitEvent_t waitEvent, uint8_t *eventDone)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t spiAddr = 0;
	uint8_t data = 0;
	uint8_t  spiBit = 0;
	uint8_t  doneBitLevel = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_readEventStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (eventDone == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_READEVENTSTATUS_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	switch(waitEvent) {
	case TAL_CLKPLLCP:
		spiAddr = TALISE_ADDR_CLK_SYNTH_CAL;
		spiBit = 5;
		doneBitLevel = 1;
		break;
	case TAL_CLKPLL_LOCK:
		spiAddr = TALISE_ADDR_CLK_SYNTH_VCO_BAND_BYTE1;
		spiBit = 0;
		doneBitLevel = 1;
		break;
	case TAL_RFPLLCP:
		spiAddr = TALISE_ADDR_RF_SYNTH_CAL;
		spiBit = 5;
		doneBitLevel = 1;
		break;
	case TAL_RFPLL_LOCK:
		spiAddr = TALISE_ADDR_RF_SYNTH_VCO_BAND_BYTE1;
		spiBit = 0;
		doneBitLevel = 1;
		break;
	case TAL_AUXPLLCP:
		spiAddr = TALISE_ADDR_AUX_SYNTH_CAL;
		spiBit = 5;
		doneBitLevel = 1;
		break;
	case TAL_AUXPLL_LOCK:
		spiAddr = TALISE_ADDR_AUX_SYNTH_VCO_BAND_BYTE1;
		spiBit = 0;
		doneBitLevel = 1;
		break;
	case TAL_ARMBUSY:
		spiAddr = TALISE_ADDR_ARM_COMMAND;
		spiBit = 7;
		doneBitLevel = 0;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_READEVENTSTATUS_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadByte(device->devHalInfo, spiAddr, &data);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (doneBitLevel) {
		*eventDone = (data >> spiBit) & 0x01;
	} else {
		*eventDone = (~(data >> spiBit)  & 0x01);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_resetExtTxLolChannel(taliseDevice_t *device,
				     taliseTxChannels_t channelSel)
{
	static const uint8_t TXLOL_RESET_CHANNEL_ESTIMATE = 0x01;
	static const uint8_t ARM_RADIOSTATE_IDLE = 0x02;
	static const uint8_t ARM_RADIOSTATE_ON = 0x03;
	static const uint32_t CODECHECK_PARAM_RESETEXTTXLOLCHANNEL_ERR1 = 1;
	static const uint32_t CODECHECK_PARAM_RESETEXTTXLOLCHANNEL_ERR2 = 2;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t extData[3] = {TALISE_ARM_OBJECTID_TRACKING_CAL_CTRL, TXLOL_RESET_CHANNEL_ESTIMATE, 0};
	uint8_t cmdStatusByte = 0;
	uint32_t radioStatus = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_resetExtTxLolChannel()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check Channel */
	switch (channelSel) {
	case TAL_TX1 :
		extData[2] = 0x01;
		break;
	case TAL_TX2 :
		extData[2] = 0x02;
		break;
	case TAL_TX1TX2 :
		extData[2] = 0x03;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RESET_TXLOL_INV_CHANNEL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* read radio state to make sure ARM is in radioOff/IDLE mode */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_RESETEXTTXLOLCHANNEL_ERR1) {
		radioStatus = 0;
	}

	/* throw error if not in radioOff/IDLE state */
	if (((radioStatus & 0x07) != ARM_RADIOSTATE_IDLE) &&
	    ((radioStatus & 0x07) != ARM_RADIOSTATE_ON)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_RESET_TXLOL_ARMSTATE_ERROR, retVal, TALACT_ERR_RERUN_INIT_CALS);
	}

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &extData[0], 3);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE, &cmdStatusByte, GETTXLOLSTATUS_TIMEOUT_US,
			GETTXLOLSTATUS_INTERVAL_US);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_RESETEXTTXLOLCHANNEL_ERR2) {
		cmdStatusByte  = 2;
	}
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setRxHd2Config(taliseDevice_t *device,
			       taliseRxHd2Config_t *hd2CalConfig)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t armFieldValue[4] = {0};
	uint32_t radioStatus = 0;
	uint8_t byteOffset = 0;

	static const uint32_t CODECHECK_PARAM_SETRXHD2CONFIG_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRxHd2Config()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	if (hd2CalConfig == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETHD2CFG_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* read radio state to make sure ARM is in radioOff /IDLE */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_SETRXHD2CONFIG_ERR1) {
		radioStatus = 0;
	}

	/* throw error if not in radioOff/IDLE state or ready state */
	if (((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_SETHD2CFG_ARMSTATE_ERROR, retVal,
						  TALACT_ERR_RESET_ARM);
	}

	/* armFieldValue[1], [2], [3] remain zero value */
	armFieldValue[0] = (hd2CalConfig->posSideBandSel > 0) ? 1 : 0;

	byteOffset = 0;
	retVal = (talRecoveryActions_t)TALISE_writeArmConfig(device,
			TALISE_ARM_OBJECTID_RXHD2_TRACKING, byteOffset,
			&armFieldValue[0], 4);
	IF_ERR_RETURN_U32(retVal);

	/* If no other higher priority errors, return possible log warning */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getRxHd2Config(taliseDevice_t *device,
			       taliseRxHd2Config_t *hd2CalConfig)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t armFieldValue[4] = {0};
	uint32_t radioStatus = 0;
	uint8_t byteOffset = 0;

	static const uint32_t CODECHECK_PARAM_GETRXHD2CONFIG_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRxHd2Config()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	if (hd2CalConfig == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETHD2CFG_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* read radio state to make sure ARM is in radioOff /IDLE */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETRXHD2CONFIG_ERR1) {
		radioStatus = 0;
	}
	/* throw error if not in radioOff/IDLE state or ready state */
	if (((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_GETHD2CFG_ARMSTATE_ERROR, retVal,
						  TALACT_ERR_RESET_ARM);
	}

	byteOffset = 0;
	retVal = (talRecoveryActions_t)TALISE_readArmConfig(device,
			TALISE_ARM_OBJECTID_RXHD2_TRACKING,
			byteOffset, &armFieldValue[0], 4);
	IF_ERR_RETURN_U32(retVal);

	hd2CalConfig->posSideBandSel = (uint32_t)armFieldValue[0];

	/* If no other higher priority errors, return possible log warning */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setDigDcOffsetMShift(taliseDevice_t *device,
				     taliseDcOffsetChannels_t channel, uint8_t mShift)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t mshiftRegAddress = 0;

	static const uint8_t MSHIFT_MIN_RANGE= 0x08;
	static const uint8_t MSHIFT_MAX_RANGE= 0x14;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setDigDcOffsetMShift()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* check channel and set the address */
	switch (channel) {
	case TAL_DC_OFFSET_RX_CHN:
		mshiftRegAddress = TALISE_ADDR_DIGITAL_DC_OFFSET_SHIFT;
		break;

	case TAL_DC_OFFSET_ORX_CHN:
		mshiftRegAddress = TALISE_ADDR_DIGITAL_DC_OFFSET_ORX_SHIFT;
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETDCMSHIFT_INV_CH_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if ((mShift < MSHIFT_MIN_RANGE) ||
	    (mShift > MSHIFT_MAX_RANGE)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETDCMSHIFT_INV_MSHIFT_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiWriteField(device->devHalInfo, mshiftRegAddress, mShift, 0x1F,
				    0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getDigDcOffsetMShift(taliseDevice_t *device,
				     taliseDcOffsetChannels_t channel, uint8_t *mShift)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t mshiftRegAddress = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getDigDcOffsetMShift()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* check channel and set the address */
	switch (channel) {
	case TAL_DC_OFFSET_RX_CHN:
		mshiftRegAddress = TALISE_ADDR_DIGITAL_DC_OFFSET_SHIFT;
		break;

	case TAL_DC_OFFSET_ORX_CHN:
		mshiftRegAddress = TALISE_ADDR_DIGITAL_DC_OFFSET_ORX_SHIFT;
		break;

	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDCMSHIFT_INV_CH_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (mShift == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETDCMSHIFT_NULL_MSHIFT_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadField(device->devHalInfo, mshiftRegAddress, mShift, 0x1F,
				   0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setDigDcOffsetEn(taliseDevice_t *device, uint8_t enableMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t REG_ADDRESS = 0x00;
	uint8_t dataToWrite = 0x00;

#if (TALISE_VERBOSE == 1)
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setDigDcOffsetEn()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (enableMask & ~((uint8_t)TAL_DC_OFFSET_ALL_ON)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DIG_DC_OFFSET_INV_ENABLE_MASK, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	REG_ADDRESS =
		TALISE_ADDR_DIGITAL_DC_OFFSET_CONFIG;                           /* Address for Rx Digital tracking Enable bits register */
	if (enableMask & ((uint8_t)TAL_DC_OFFSET_RX1 | (uint8_t)
			  TAL_DC_OFFSET_RX2)) { /* Enable / Disable  Rx1 and/or Rx2 */
		dataToWrite |= (enableMask & ((uint8_t)TAL_DC_OFFSET_RX1 |
					      (uint8_t) TAL_DC_OFFSET_RX2));
	}

	halError = talSpiWriteField(device->devHalInfo,  REG_ADDRESS, dataToWrite, 0x06,
				    1); /* Write RX enable bits to the register */
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	dataToWrite =
		0x0;                                                             /* Reset dataToWrite */
	REG_ADDRESS =
		TALISE_DIGITAL_DC_OFFSET_ORX_LOOPBACK_CONFIG;                    /* Address for ORx Digital trackingEnable bits register */
	if (enableMask & ((uint8_t)TAL_DC_OFFSET_ORX1 | (uint8_t)
			  TAL_DC_OFFSET_ORX2)) { /* Check for channel  ORx and/or Sniffer */
		dataToWrite |= ((enableMask & ((uint8_t)TAL_DC_OFFSET_ORX1 |
					       (uint8_t)TAL_DC_OFFSET_ORX2)) >> 2);
	}

	halError = talSpiWriteField(device->devHalInfo,  REG_ADDRESS, dataToWrite, 0x03,
				    0);  /* Write Loopback, ORx1 and ORx2 enable bits to the register */
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getDigDcOffsetEn(taliseDevice_t *device,uint8_t *enableMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t REG_ADDRESS = 0;
	uint8_t readbackData = 0;
	uint8_t enableMaskData = 0;

#if (TALISE_VERBOSE == 1)
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getDigDcOffsetEn()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (enableMask == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_DIG_DC_OFFSET_NULL_ENABLE_MASK, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Read back Rx1/Rx2 Dig DC offset enable setting */
	REG_ADDRESS =
		TALISE_ADDR_DIGITAL_DC_OFFSET_CONFIG;  /* register address of Rx1 and Rx2 enable bits*/
	halError = talSpiReadField(device->devHalInfo, REG_ADDRESS, &readbackData, 0x06,
				   1);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	enableMaskData |=
		readbackData;  /* adjust bits to match channel :refer enum mykonosRfDcOffsettEn_t. */

	readbackData = 0x00;
	REG_ADDRESS =
		TALISE_DIGITAL_DC_OFFSET_ORX_LOOPBACK_CONFIG;  /* register address of Orx and sniffer enable bits*/
	halError = talSpiReadField(device->devHalInfo, REG_ADDRESS, &readbackData, 0x03,
				   0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	enableMaskData |= (uint8_t)(readbackData <<
				    2);  /* adjust bits to match channel :refer enum mykonosRfDcOffsettEn_t. */

	*enableMask = enableMaskData;

	return (uint32_t)retVal;
}

uint32_t TALISE_getTrackingCalsBatchSize(taliseDevice_t *device,
		taliseTrackingCalBatchSize_t* batchsize_us)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t armFieldValue[16] = { 0 };
	uint32_t radioStatus = 0;
	uint8_t byteOffset = 0x3C;
	uint32_t batchsizetime;

	static const uint32_t CODECHECK_PARAM_GETTCALBATCHSIZE_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;

	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getTrackingCalsBatchSize()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	if (batchsize_us == NULL) {
		return (uint32_t)talApiErrHandler(device,
						  TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTCAL_BATCHSIZE_NULL_PARAM,
						  retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* read radio state to make sure ARM is in radioOff/IDLE state or ready state */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_GETTCALBATCHSIZE_ERR1) {
		radioStatus = 0;
	}
	/* throw error if not in radioOff/IDLE state or ready state */
	if (((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device,
						  TAL_ERRHDL_API_FAIL,
						  TAL_ERR_GETTCAL_BATCH_SIZE_ARMSTATE_ERROR,
						  retVal,
						  TALACT_ERR_RESET_ARM);
	}

	retVal = (talRecoveryActions_t)TALISE_readArmConfig(device,
			TALISE_ARM_OBJECTID_CAL_SCHEDULER,
			byteOffset,
			&armFieldValue[0],
			16);
	IF_ERR_RETURN_U32(retVal);
	batchsizetime = (taliseTrackingCalBatchSize_t)((armFieldValue[1] << 2) +
			(armFieldValue[0]));
	switch (batchsizetime) {
	case 500:
		*batchsize_us = TAL_TRACK_BATCH_SIZE_500_US;
		break;
	case 200:
		*batchsize_us = TAL_TRACK_BATCH_SIZE_200_US;
		break;
		return (uint32_t)talApiErrHandler(device,
						  TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTCAL_BATCHSIZE_INV_VALUE,
						  retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* If no other higher priority errors, return possible log warning */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setTrackingCalsBatchSize(taliseDevice_t *device,
		taliseTrackingCalBatchSize_t batchsize_us)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t armFieldValue[16] = { 0 };
	uint32_t radioStatus = 0;
	uint8_t byteOffset = 0x3C;

	static const uint32_t CODECHECK_PARAM_SETTCALBATCHSIZE_ERR1 = 2;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;

	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setTrackingCalsBatchSize()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* check bactch size and set the address */
	switch (batchsize_us) {
	case TAL_TRACK_BATCH_SIZE_500_US:
		armFieldValue[0] = (uint8_t)0xF4;
		armFieldValue[4] = (uint8_t)0xF4;
		armFieldValue[8] = (uint8_t)0xF4;
		armFieldValue[12] = (uint8_t)0xF4;
		armFieldValue[1] = (uint8_t)0x01;
		armFieldValue[5] = (uint8_t)0x01;
		armFieldValue[9] = (uint8_t)0x01;
		armFieldValue[13] = (uint8_t)0x01;
		break;

	case TAL_TRACK_BATCH_SIZE_200_US:
		armFieldValue[0] = (uint8_t)0xC8;
		armFieldValue[4] = (uint8_t)0xC8;
		armFieldValue[8] = (uint8_t)0xC8;
		armFieldValue[12] = (uint8_t)0xC8;
		break;

	default:
		return (uint32_t)talApiErrHandler(device,
						  TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTCAL_BATCH_SIZE_PARAM,
						  retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* read radio state to make sure ARM is in radioOff/IDLE state or ready state */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	/* SW Test */
	if (device->devStateInfo.swTest == CODECHECK_PARAM_SETTCALBATCHSIZE_ERR1) {
		radioStatus = 0;
	}

	/* throw error if not in radioOff/IDLE state or ready state */
	if (((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device,
						  TAL_ERRHDL_API_FAIL,
						  TAL_ERR_SETTCAL_BATCH_SIZE_ARMSTATE_ERROR,
						  retVal,
						  TALACT_ERR_RESET_ARM);
	}

	/* other armFieldValues remain zero */

	retVal = (talRecoveryActions_t)TALISE_writeArmConfig(device,
			TALISE_ARM_OBJECTID_CAL_SCHEDULER,
			byteOffset,
			&armFieldValue[0],
			16);
	IF_ERR_RETURN_U32(retVal);

	/* If no other higher priority errors, return possible log warning */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

const char* talGetCalErrorMessage(uint32_t errSrc, uint32_t errCode)
{
#ifndef TALISE_VERBOSE
	return "";
#else
	uint8_t initErrCode = (uint8_t)errCode;
	uint8_t initErrCal = (uint8_t)(errCode >> 8);

	if (errSrc == TAL_ERRSRC_INITCALS) {
		switch (initErrCal) {
		/* Tx BBF */
		case  TALISE_ARM_OBJECTID_TXBBF_INIT:
			switch (initErrCode) {
			case 0x02:
				return "TxBBF Init Cal: Calibration Timed Out - Orx\n";
			case 0x03:
				return "TxBBF Init Cal: DAC divider out of range\n";
			case 0x04:
				return "TxBBF Init Cal: DAC divider out of range\n";
			default:
				return  "Unknown Talise ARM TXBBF init cal error\n";
			}
			break;

		/* Adc Tuner */
		case  TALISE_ARM_OBJECTID_ADCTUNER_INIT:
			switch (initErrCode) {
			case 0x01:
				return "ADC Tuner Init Cal: Calibration Timed Out\n";
			case 0x02:
				return "ADC Tuner Init Cal: Invalid Channel Specified\n";
			default:
				return  "Unknown Talise ARM ADC Tuner init cal error\n";
			}
			break;

		/* TIA */
		case  TALISE_ARM_OBJECTID_TIA_INIT:
			switch (initErrCode) {
			case 0x01:
				return "TIA Init Cal: Error Configuring PLL - Orx\n";
			case 0x02:
				return "TIA Init Cal: Error during TIA tuner calibration - Orx\n";
			case 0x03:
				return "TIA Init Cal: Error configuring PLL - Rx\n";
			case 0x04:
				return "TIA Init Cal: Error during TIA tuner calibration - Rx\n";
			default:
				return  "Unknown Talise ARM TIA init cal error\n";
			}
			break;

		/* DC Offset */
		case  TALISE_ARM_OBJECTID_DCOFFSET_INIT:
			switch (initErrCode) {
			case 0x01:
				return "DC Offset Init Cal: Calibration Timed Out - Rx\n";
			case 0x02:
				return "DC Offset Init Cal: Calibration Timed Out - Orx\n";
			case 0x03:
				return "DC Offset Init Cal: Calibration Timed Out - LBrx\n";
			case 0x04:
				return "DC Offset Init Cal: Calibration Timed Out - Srx\n";
			default:
				return  "Unknown Talise ARM DC Offset init cal error\n";
			}
			break;

		/* Flash Cal */
		case  TALISE_ARM_OBJECTID_ADCFLASH_INIT:
			switch (initErrCode) {
			case 0x01:
				return "ADC Flash Init Cal: Calibration Aborted\n";
			case 0x02:
				return "ADC Flash Init Cal: Calibration Timed Out\n";
			case 0x03:
				return "ADC Flash Init Cal: No Channel Selected\n";
			case 0x04:
				return "ADC Flash Init Cal: Rx Disabled\n";
			case 0x05:
				return "ADC Flash Init Cal: Orx Disabled\n";
			default:
				return "Unknown Talise ARM ADC Flash init cal error\n";
			}
			break;

		/* Path Delay */
		case  TALISE_ARM_OBJECTID_PATHDELAY_INIT:
			switch (initErrCode) {
			case 0x01:
				return "PathDelay Init Cal: Rx Disabled\n";
			case 0x02:
				return "PathDelay Init Cal: Tx Disabled\n";
			case 0x03:
				return "PathDelay Init Cal: Data capture timeout due to hardware setup\n";
			case 0x04:
				return "PathDelay Init Cal: Data capture was aborted\n";
			case 0x05:
				return "PathDelay Init Cal: NCO did not lock\n";
			case 0x06:
				return "PathDelay Init Cal: Measurement error\n";
			default:
				return "Unknown Talise ARM Path Delay init cal error\n";
			}
			break;

		/* TxLO Leakage Internal Init */
		case  TALISE_ARM_OBJECTID_TXLOLINT_INIT: /* Fall through */
		/* TxLO Leakage External Init */
		case  TALISE_ARM_OBJECTID_TXLOLEXT_INIT:
			switch (initErrCode) {
			case 0x02:
				return "TxLol Init Cal: Tx Disabled\n";
			case 0x03:
				return "TxLol Init Cal: Path delay invalid\n";
			case 0x04:
				return "TxLol Init Cal: Initial calibration not completed\n";
			case 0x05:
				return "TxLol Init Cal: Internal loopback tracking disabled\n";
			case 0x06:
				return "TxLol Init Cal: Data capture timeout due to hardware setup\n";
			case 0x07:
				return "TxLol Init Cal: Stitching for Orx2 not supported\n";
			case 0x08:
				return "TxLol Init Cal: No Tx-Orx mapping\n";
			default:
				return "Unknown Talise ARM Tx LOL init cal error\n";
			}
			break;

		/* TxQEC Init */
		case  TALISE_ARM_OBJECTID_TXQEC_INIT:
			switch (initErrCode) {
			case 0x02:
				return "TxQEC Init Cal: Tx Disabled\n";
			case 0x03:
				return "TxQEC Init Cal: No path delay estimate present\n";
			case 0x04:
				return "TxQEC Init Cal: NCO failed to lock\n";
			default:
				return "Unknown Talise ARM TxQEC init cal error\n";
			}
			break;

		/* LBRx LO Delay */
		case  TALISE_ARM_OBJECTID_LBLODELAY_INIT:
			switch (initErrCode) {
			case 0x01:
				return "LB LO Delay Init Cal: Rx Disabled\n";
			case 0x02:
				return "LB LO Delay Init Cal: Tx Disabled\n";
			case 0x03:
				return "LB LO Delay Init Cal: Error configuring Cal PLL\n";
			case 0x04:
				return "LB LO Delay Init Cal: TCAL settling time\n";
			case 0x05:
				return "LB LO Delay Init Cal: TCAL data capture error\n";
			case 0x06:
				return "LB LO Delay Init Cal: Wrong frequency Bin\n";
			default:
				return "Unknown Talise ARM LB LO Delay init cal error\n";
			}
			break;

		/* LBRx Tcal */
		case  TALISE_ARM_OBJECTID_LBRXQEC_INIT:
			switch (initErrCode) {
			case 0x01:
				return "LB RxQEC Init Cal: Rx Disabled\n";
			case 0x02:
				return "LB RxQEC Init Cal: Tx Disabled\n";
			case 0x03:
				return "LB RxQEC Init Cal: Error configuring Cal PLL\n";
			case 0x04:
				return "LB RxQEC Init Cal: TCAL settling time\n";
			case 0x05:
				return "LB RxQEC Init Cal: QFIR overflow\n";
			case 0x06:
				return "LB RxQEC Init Cal: CFIR overflow\n";
			case 0x07:
				return "LB RxQEC Init Cal: TCAL data capture error\n";
			default:
				return "Unknown Talise ARM LB RxQEC init cal error\n";
			}
			break;


		/* Rx TCal */
		case  TALISE_ARM_OBJECTID_RXQEC_INIT:
			switch (initErrCode) {
			case 0x01:
				return "RxQEC Init Cal: Rx Disabled\n";
			case 0x02:
				return "RxQEC Init Cal: Tx Disabled\n";
			case 0x03:
				return "RxQEC Init Cal: Error configuring Cal PLL\n";
			case 0x04:
				return "RxQEC Init Cal: TCAL settling time\n";
			case 0x05:
				return "RxQEC Init Cal: QFIR overflow\n";
			case 0x06:
				return "RxQEC Init Cal: CFIR overflow\n";
			case 0x07:
				return "RxQEC Init Cal: TCAL data capture error\n";
			default:
				return  "Unknown Talise ARM RxQEC init cal error\n";
			}
			break;

		/* ORx TCal */
		case  TALISE_ARM_OBJECTID_ORXQEC_INIT:
			switch (initErrCode) {
			case 0x01:
				return "ORxQEC Init Cal: Rx Disabled\n";
			case 0x02:
				return "ORxQEC Init Cal: Tx Disabled\n";
			case 0x03:
				return "ORxQEC Init Cal: Error configuring Cal PLL\n";
			case 0x04:
				return "ORxQEC Init Cal: TCAL settling time\n";
			case 0x05:
				return "ORxQEC Init Cal: QFIR overflow\n";
			case 0x06:
				return "ORxQEC Init Cal: CFIR overflow\n";
			case 0x07:
				return "ORxQEC Init Cal: TCAL data capture error\n";
			default:
				return "Unknown Talise ARM ORx QEC init cal error\n";
			}
			break;

		/* Tx DAC */
		case  TALISE_ARM_OBJECTID_TXDAC_INIT:
			switch (initErrCode) {
			case 0x01:
				return "TxDAC Init Cal: Calibration time out\n";
			case 0x02:
				return "TxDAC Init Cal: Tx Disabled\n";
			default:
				return "Unknown Talise ARM TxDAC init cal error\n";
			}
			break;

		/* ADC Stitching */
		case  TALISE_ARM_OBJECTID_ADCSTITCHING_INIT:
			switch (initErrCode) {
			case 0x01:
				return "ADC Stitching Init Cal: Excess level supported by hardware\n";
			default:
				return "Unknown Talise ARM ADC stitching init cal error\n";
			}
			break;

		/* Orx LO Delay */
		case  TALISE_ARM_OBJECTID_ORXLODELAY_INIT: /* Fall through */
		/* Reserved */
		case  0x0F: /* Fall through */
		/* Rx LO Delay */
		case  TALISE_ARM_OBJECTID_RXLODELAY_INIT: /* Fall through */
		/* Tx Atten Delay */
		case  TALISE_ARM_OBJECTID_TXATTENDELAY_INIT: /* Fall through */
		/* Rx Gain Delay */
		case  TALISE_ARM_OBJECTID_RXGAINDELAY_INIT: /* Fall through */
		default:
			return "Unknown Init Calibration Error\n";
		}
	}
	return "Unknown Init Calibration Error\n";

#endif

}

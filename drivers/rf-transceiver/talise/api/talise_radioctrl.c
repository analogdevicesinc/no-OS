// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_radioctrl.c
 * \brief Contains functions to support Talise radio control and pin control
 *        functions
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_cals.h"
#include "talise_radioctrl.h"
#include "talise_gpio.h"
#include "talise_reg_addr_macros.h"
#include "talise_arm_macros.h"
#include "talise_hal.h"
#include "talise_user.h"
#include "talise_error.h"
#include "talise_arm.h"
#include "talise.h"

uint32_t TALISE_loadStreamFromBinary(taliseDevice_t *device, uint8_t *binary)
{
	adiHalErr_t halError = ADIHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t streamBinBaseAddr = 0;
	uint32_t streamBaseAddr = 0;
	uint8_t numberStreams = 0;
	uint32_t imageSize = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_loadStreamFromBinary()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* populating variables from binary array */
	streamBinBaseAddr = (((uint32_t)binary[3]) << 24) | (((uint32_t)binary[2]) <<
			    16) | (((uint32_t)binary[1]) << 8) | (uint32_t)(binary[0]);
	streamBaseAddr = (((uint32_t)binary[7]) << 24) | (((uint32_t)binary[6]) << 16)
			 | (((uint32_t)binary[5]) << 8) | (uint32_t)(binary[4]);
	numberStreams = binary[8];
	imageSize = (((uint32_t)binary[11]) << 8) | (uint32_t)(binary[10]);

	/* resetting the stream processor */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_STREAM_CONTROL,
				    0x01, 0x01, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing stream instruction and data pointers to memory */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device, streamBinBaseAddr,
			binary, imageSize);
	IF_ERR_RETURN_U32(retVal);

	/* writing lower 16 bits of stream base address */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_STREAM_BASE_BYTE0,
				   (uint8_t)streamBaseAddr);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_STREAM_BASE_BYTE1,
				   (uint8_t)(streamBaseAddr >> 8));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* writing lower byte of number of streams */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_LAST_STREAM_NUM,
				   numberStreams);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* clearing stream reset bit */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_STREAM_CONTROL,
				    0x00, 0x01, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* setting up the stream processor and clearing the debug registers */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG0, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG1, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG2, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG3, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG4, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG5, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG6, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_STREAM_PROC_DEBUG_REG7, 0x00);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	device->devStateInfo.devState = (taliseStates_t)(device->devStateInfo.devState |
					TAL_STATE_STREAMLOADED);

	return (uint32_t)retVal;
}

uint32_t TALISE_setArmGpioPins(taliseDevice_t *device,
			       taliseArmGpioConfig_t *armGpio)
{
	uint8_t i = 0;
	uint8_t cmdStatusByte = 0;
	uint8_t signalId[8] = {
		TALISE_ARM_ORX1_TX_SEL0_SIGNALID,
		TALISE_ARM_ORX1_TX_SEL1_SIGNALID,
		TALISE_ARM_ORX2_TX_SEL0_SIGNALID,
		TALISE_ARM_ORX2_TX_SEL1_SIGNALID,
		TALISE_ARM_TXCAL_ENA_SIGNALID,
		TALISE_ARM_CAL_UPDATE0_SIGNALID,
		TALISE_ARM_CAL_UPDATE1_SIGNALID,
		TALISE_ARM_CAL_UPDATE2_SIGNALID
	};
	uint8_t armGetGpioStatus = 0;
	uint8_t gpioStatus[2] = {0};
	uint8_t gpioCheck = 0;
	taliseGpioPinSel_t gpioPinSel = TAL_GPIO_00;
	uint8_t gpioEnable = 0;
	uint8_t gpioConfig[3] = {0};

	uint32_t gpioOe = 0;
	uint32_t gpioUsedMask = 0;
	uint32_t gpioFreeMask = 0;
	uint32_t usedGpiopins = 0;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setArmGpioPins()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (armGpio == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_ARMGPIO_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Free all current GPIO's */
	gpioStatus[0] = TALISE_ARM_OBJECTID_GPIO_CTRL;

	for(i = 0; i < sizeof(signalId); i++) {
		gpioStatus[1] = signalId[i];

		/* Get current GPIO pin assigned to this signal_id */
		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_GET_OPCODE, &gpioStatus[0], sizeof(gpioStatus));
		IF_ERR_RETURN_U32(retVal);

		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_GET_OPCODE, &cmdStatusByte, SETARMGPIO_TIMEOUT_US,
				SETARMGPIO_INTERVAL_US);
		if((cmdStatusByte >> 1) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, gpioStatus[0], cmdStatusByte), retVal,
							  TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}

		retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
				TALISE_ADDR_ARM_START_DATA_ADDR, &armGetGpioStatus, 1, 0);
		IF_ERR_RETURN_U32(retVal);

		if (armGetGpioStatus < 19) {
			gpioFreeMask |= (1 << (armGetGpioStatus & 0x0F));
		}
	}

	usedGpiopins = device->devStateInfo.usedGpiopins & ~gpioFreeMask;

	/* Update all used GPIO's */
	for(i = 0; i < sizeof(signalId); i++) {
		switch(signalId[i]) {
		case TALISE_ARM_ORX1_TX_SEL0_SIGNALID:
			gpioPinSel = armGpio->orx1TxSel0Pin.gpioPinSel;
			gpioEnable = armGpio->orx1TxSel0Pin.enable;
			gpioCheck = 1;
			break;

		case TALISE_ARM_ORX1_TX_SEL1_SIGNALID:
			gpioPinSel = armGpio->orx1TxSel1Pin.gpioPinSel;
			gpioEnable = armGpio->orx1TxSel1Pin.enable;
			gpioCheck = 1;
			break;

		case TALISE_ARM_ORX2_TX_SEL0_SIGNALID:
			gpioPinSel = armGpio->orx2TxSel0Pin.gpioPinSel;
			gpioEnable = armGpio->orx2TxSel0Pin.enable;
			gpioCheck = 1;
			break;

		case TALISE_ARM_ORX2_TX_SEL1_SIGNALID:
			gpioPinSel = armGpio->orx2TxSel1Pin.gpioPinSel;
			gpioEnable = armGpio->orx2TxSel1Pin.enable;
			gpioCheck = 1;
			break;

		case TALISE_ARM_TXCAL_ENA_SIGNALID:
			gpioPinSel = armGpio->enTxTrackingCals.gpioPinSel;
			gpioEnable = armGpio->enTxTrackingCals.enable;
			gpioCheck = 1;
			break;

		case TALISE_ARM_CAL_UPDATE0_SIGNALID:
			gpioCheck = 0;
			break;

		case TALISE_ARM_CAL_UPDATE1_SIGNALID:
			gpioCheck = 0;
			break;

		case TALISE_ARM_CAL_UPDATE2_SIGNALID:
			gpioCheck = 0;
			break;
		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_ARMGPIO_PINS_INV_SIGNALID, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		if ( gpioCheck > 0) {
			/* Use GPIO if pin enabled for ARM */
			if (gpioEnable) {
				if (gpioPinSel > TAL_GPIO_15) {
					return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
									  TAL_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN, retVal, TALACT_ERR_CHECK_PARAM);
				}

				/* Error if GPIO is already assigned to some other function */
				if ((usedGpiopins & (1 << (uint8_t)gpioPinSel)) != 0) {
					return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
									  TAL_ERR_SET_ARMGPIO_PINS_GPIO_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
				}

				usedGpiopins |= (1 << (uint8_t)
						 gpioPinSel); /* GPIO pins used by all GPIO functions */
				gpioUsedMask |= (1 << (uint8_t)
						 gpioPinSel); /* GPIO pins used by this function */
			}
		}
	}

	/* Write ARM set command to setup which ARM signals are using which GPIO pins */
	/* Setup input pins for ORX_MODE[2:0] and ORX_MODE_Trigger if orx is in pin mode */
	for(i = 0; i < sizeof(signalId); i++) {
		gpioConfig[0] = TALISE_ARM_OBJECTID_GPIO_CTRL;
		gpioConfig[1] = signalId[i];

		switch(signalId[i]) {
		case TALISE_ARM_ORX1_TX_SEL0_SIGNALID:
			gpioConfig[2] = (uint8_t)(armGpio->orx1TxSel0Pin.gpioPinSel |
						  (((armGpio->orx1TxSel0Pin.polarity > 0) ? 1 : 0) << 5) |
						  (((armGpio->orx1TxSel0Pin.enable > 0) ? 1 : 0) << 7));
			break;

		case TALISE_ARM_ORX1_TX_SEL1_SIGNALID:
			gpioConfig[2] = (uint8_t)(armGpio->orx1TxSel1Pin.gpioPinSel |
						  (((armGpio->orx1TxSel1Pin.polarity > 0) ? 1 : 0) << 5) |
						  (((armGpio->orx1TxSel1Pin.enable > 0) ? 1 : 0) << 7));
			break;

		case TALISE_ARM_ORX2_TX_SEL0_SIGNALID:
			gpioConfig[2] = (uint8_t)(armGpio->orx2TxSel0Pin.gpioPinSel |
						  (((armGpio->orx2TxSel0Pin.polarity > 0) ? 1 : 0) << 5) |
						  (((armGpio->orx2TxSel0Pin.enable > 0) ? 1 : 0) << 7));
			break;

		case TALISE_ARM_ORX2_TX_SEL1_SIGNALID:
			gpioConfig[2] = (uint8_t)(armGpio->orx2TxSel1Pin.gpioPinSel |
						  (((armGpio->orx2TxSel1Pin.polarity > 0) ? 1 : 0) << 5) |
						  (((armGpio->orx2TxSel1Pin.enable > 0) ? 1 : 0) << 7));
			break;

		case TALISE_ARM_TXCAL_ENA_SIGNALID:
			gpioConfig[2] = (uint8_t)(armGpio->enTxTrackingCals.gpioPinSel |
						  (((armGpio->enTxTrackingCals.polarity > 0) ? 1 : 0) << 5) |
						  (((armGpio->enTxTrackingCals.enable > 0) ? 1 : 0) << 7));
			break;

		case TALISE_ARM_CAL_UPDATE0_SIGNALID:
			gpioConfig[2] = 0;
			break;
		case TALISE_ARM_CAL_UPDATE1_SIGNALID:
			gpioConfig[2] = 0;
			break;
		case TALISE_ARM_CAL_UPDATE2_SIGNALID:
			gpioConfig[2] = 0;
			break;
		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SET_ARMGPIO_PINS_INV_SIGNALID, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_SET_OPCODE, &gpioConfig[0], sizeof(gpioConfig));
		IF_ERR_RETURN_U32(retVal);

		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETARMGPIO_TIMEOUT_US,
				SETARMGPIO_INTERVAL_US);
		if((cmdStatusByte >> 1) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							  ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, gpioConfig[0], cmdStatusByte), retVal,
							  TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* Set Talise GPIO OE direction for pins used by ARM */
	/* Currently ARM has no outputs, so gpioOe = 0 */
	retVal = (talRecoveryActions_t)TALISE_setGpioOe(device, gpioOe, gpioUsedMask);
	IF_ERR_RETURN_U32(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpiopins = usedGpiopins;

	return (uint32_t)retVal;
}

uint32_t TALISE_setRadioCtlPinMode(taliseDevice_t *device,
				   uint8_t pinOptionsMask, taliseRadioCtlCfg2_t orxEnGpioPinSel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint8_t radioCtlCfg1BitMask = 0x77;
	static const uint8_t radioCtlCfg2BitMask = 0x03;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRadioCtlPinMode()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid bit mask settings */
	if (((pinOptionsMask & ~radioCtlCfg1BitMask) > 0) ||
	    ((((uint8_t)orxEnGpioPinSel) & ~radioCtlCfg2BitMask) > 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_RADIO_CTL_MASK_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* Disable ability for partial stream processor power down from Rx to
		 * ORx or ORx to Rx  until stream processor is updated to support it
		 */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_RADIO_CONTROL_CONFIG1, pinOptionsMask);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Set which GPIO pins are used by the stream processor to control ORx enable[1:0] */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_RADIO_CONTROL_CONFIG2, ((uint8_t)orxEnGpioPinSel));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getRadioCtlPinMode(taliseDevice_t *device,
				   uint8_t *pinOptionsMask, taliseRadioCtlCfg2_t *orxEnGpioPinSel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readData = 0;

	static const uint8_t radioCtlCfg1BitMask = 0x77;
	static const uint8_t orxEnGpioPinSelFieldMask = 0x03;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRadioCtlPinMode()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for NULL pointers in function parameters */
	if ((pinOptionsMask == NULL) ||
	    (orxEnGpioPinSel == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETPINMODE_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_RADIO_CONTROL_CONFIG1, pinOptionsMask, radioCtlCfg1BitMask, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Read which GPIO pins are used by the stream processor to control ORx enable[1:0] */
	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_RADIO_CONTROL_CONFIG2, &readData, orxEnGpioPinSelFieldMask, 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*orxEnGpioPinSel = (taliseRadioCtlCfg2_t)readData;

	return (uint32_t)retVal;
}

uint32_t TALISE_setOrxLoCfg(taliseDevice_t *device,
			    const taliseOrxLoCfg_t *orxLoCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t enableRelock = 0;
	uint8_t gpio3StreamMaskValue = 0;
	uint8_t gpio3StreamMaskReadValue = 0;
	uint8_t gpioSel = 0;
	uint8_t gpioSelReadVal = 0;
	uint8_t byteOffset = 0;
	uint32_t radioStatus = 0;

	uint32_t usedGpioPins = 0;
	uint32_t freeGpioPins = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setOrxLoCfg()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Perform input paramter NULL and range checks */
	if (orxLoCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETORXLOCFG_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (orxLoCfg->gpioSelect == TAL_GPIO_INVALID) {
		gpio3StreamMaskValue = 1;
		gpioSel = 0;
	} else if ((orxLoCfg->gpioSelect >= TAL_GPIO_00)
		   && (orxLoCfg->gpioSelect <= TAL_GPIO_15)) {
		gpio3StreamMaskValue = 0;
		gpioSel = (uint8_t)orxLoCfg->gpioSelect;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETORXLOCFG_INVALIDPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* If this feature already enabled for any GPIO pin, free the pin */
	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_STREAM_CONTROL,
				   &gpio3StreamMaskReadValue, 0x80, 7);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_STREAM_GPIO_TRIGGER_PIN_SELECT_BYTE1, &gpioSelReadVal, 0xF0, 4);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (gpio3StreamMaskReadValue == 1) {
		/* TAL_GPIO_INVALID */
		/* No GPIO pins to free */
	} else {
		freeGpioPins = (uint32_t)(1 << gpioSelReadVal);
	}

	/* Verify GPIO is not already used */
	if ((orxLoCfg->gpioSelect >= TAL_GPIO_00)
	    && (orxLoCfg->gpioSelect <= TAL_GPIO_15)) {
		usedGpioPins = (uint32_t)(1 << gpioSel);

		/* Check if pin selected is already being used by another feature */
		if ((device->devStateInfo.usedGpiopins & (usedGpioPins & ~freeGpioPins)) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETORXLOCFG_GPIOUSED, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Verify ARM in radio Off state */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	if (((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETORXLOCFG_INVALID_ARMSTATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Disable/enable Auto Relocking AuxPLL in ARM */
	/* Invert value before writing to ARM memory */
	enableRelock = (orxLoCfg->disableAuxPllRelocking == 0) ? 1 : 0;
	byteOffset = 0x1C;
	retVal = (talRecoveryActions_t)TALISE_writeArmConfig(device,
			TALISE_ARM_OBJECTID_SYTEM_INFO,
			byteOffset, &enableRelock, 1);
	IF_ERR_RETURN_U32(retVal);

	/* Set Stream GPIO3 pin select and enable the GPIO pin to be able to create
	 * an interrupt to trigger the stream */
	halError = talSpiWriteField(device->devHalInfo, TALISE_ADDR_STREAM_CONTROL,
				    gpio3StreamMaskValue, 0x80, 7);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_STREAM_GPIO_TRIGGER_PIN_SELECT_BYTE1, gpioSel, 0xF0, 4);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Update used GPIO pins in device data structure */
	device->devStateInfo.usedGpiopins &= ~freeGpioPins;
	device->devStateInfo.usedGpiopins |= usedGpioPins;

	return (uint32_t)retVal;
}

uint32_t TALISE_getOrxLoCfg(taliseDevice_t *device, taliseOrxLoCfg_t *orxLoCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint32_t radioStatus = 0;
	uint16_t byteOffset = 0;
	uint8_t enableRelock = 0;
	uint8_t gpio3StreamMaskValue = 0;
	uint8_t gpioSel = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getOrxLoCfg()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Perform input paramter NULL and range checks */
	if (orxLoCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETORXLOCFG_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Verify ARM in radio Off state */
	retVal = (talRecoveryActions_t)TALISE_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	if (((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != TALISE_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETORXLOCFG_INVALID_ARMSTATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Read back if ARM is auto relocking AuxPll */
	byteOffset = 0x1C;
	retVal = (talRecoveryActions_t)TALISE_readArmConfig(device,
			TALISE_ARM_OBJECTID_SYTEM_INFO, byteOffset, &enableRelock, 1);
	IF_ERR_RETURN_U32(retVal);

	/* Invert relock value */
	orxLoCfg->disableAuxPllRelocking = (enableRelock == 1) ? 0 : 1;

	/* Read back stream GPIO3 pin setup */
	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_STREAM_CONTROL,
				   &gpio3StreamMaskValue, 0x80, 7);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_STREAM_GPIO_TRIGGER_PIN_SELECT_BYTE1, &gpioSel, 0xF0, 4);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (gpio3StreamMaskValue == 1) {
		orxLoCfg->gpioSelect = TAL_GPIO_INVALID;
	} else {
		orxLoCfg->gpioSelect = (taliseGpioPinSel_t)gpioSel;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_radioOn(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;

	/* send ARM opcode to turn radio on */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_RADIOON_OPCODE, 0, 0);
	IF_ERR_RETURN_U32(retVal);

	/* wait for command to complete */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_RADIOON_OPCODE, &cmdStatusByte, RADIOON_TIMEOUT_US,
			RADIOON_INTERVAL_US);
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_RADIOON_OPCODE, 0, cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	device->devStateInfo.devState = (taliseStates_t)(device->devStateInfo.devState |
					TAL_STATE_RADIOON);

	return (uint32_t)retVal;
}

uint32_t TALISE_radioOff(taliseDevice_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;

	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_ABORT_OPCODE, 0, 0);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_ABORT_OPCODE, &cmdStatusByte, RADIOOFF_TIMEOUT_US,
			RADIOOFF_INTERVAL_US);
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_ABORT_OPCODE, 0, cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	device->devStateInfo.devState = (taliseStates_t)(device->devStateInfo.devState &
					~TAL_STATE_RADIOON);

	return (uint32_t)retVal;
}

uint32_t TALISE_getRadioState(taliseDevice_t *device, uint32_t *radioStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t status = 0;

	/* execute only if radioStatus pointer is valid */
	if (radioStatus != NULL) {
		halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_ARM_CMD_STATUS_8,
					  &status);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		*radioStatus = (uint32_t)status;
	} else {
		/* invalid radioStatus pointer */
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRADIOSTATE_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setRxTxEnable(taliseDevice_t *device,
			      taliseRxORxChannels_t rxOrxChannel, taliseTxChannels_t txChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t chEnable = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRxTxEnable()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Only allow valid Rx / ORx cases, return error else */
	switch (rxOrxChannel) {
	case TAL_RXOFF_EN:  /* Fall through to next case */
	case TAL_RX1_EN:    /* Fall through to next case */
	case TAL_RX2_EN:    /* Fall through to next case */
	case TAL_RX1RX2_EN: /* Fall through to next case */
	case TAL_ORX1_EN:   /* Fall through to next case */
	case TAL_ORX2_EN:   /* Fall through to next case */
	case TAL_ORX1ORX2_EN:
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_BBIC_INV_CHN, retVal, TALACT_ERR_CHECK_PARAM);
	}

	switch (txChannel) {
	case TAL_TXOFF: /* Fall through to next case */
	case TAL_TX1:   /* Fall through to next case */
	case TAL_TX2:   /* Fall through to next case */
	case TAL_TX1TX2:
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_BBIC_INV_CHN, retVal, TALACT_ERR_CHECK_PARAM);
	}

	chEnable = (uint8_t)((txChannel << 6) | rxOrxChannel);

	/* Check that channels were initialized */
	if ((chEnable & device->devStateInfo.initializedChannels) != chEnable) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRXTXENABLE_INVCHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_BBIC_ENABLES,
				   chEnable);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getRxTxEnable(taliseDevice_t *device,
			      taliseRxORxChannels_t *rxOrxChannel, taliseTxChannels_t *txChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t chEnable = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRxTxEnable()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((txChannel == NULL) ||
	    (rxOrxChannel == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRXTXENABLE_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_BBIC_ENABLES,
				  &chEnable);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*txChannel = (taliseTxChannels_t)((chEnable >> 6) & 0x03);
	*rxOrxChannel = (taliseRxORxChannels_t)(chEnable & 0x0F);

	return (uint32_t)retVal;
}

uint32_t TALISE_setTxToOrxMapping(taliseDevice_t *device, uint8_t txCalEnable,
				  taliseTxToOrxMapping_t oRx1Map, taliseTxToOrxMapping_t oRx2Map)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t extData[2] = {0};
	uint8_t cmdStatusByte = 0x0;

	static const uint8_t ENABLE_TXCALS = 0x10;

	extData[0] = TALISE_ARM_OBJECTID_ORX_TXCAL_CTRL;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setTxToOrxMapping()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* cmdByte assignment from oRx1Map */
	switch (oRx1Map) {
	case TAL_MAP_NONE:
		extData[1] = TAL_MAP_NONE;
		break;
	case TAL_MAP_TX1_ORX:
		extData[1] = TAL_MAP_TX1_ORX;
		break;
	case TAL_MAP_TX2_ORX:
		extData[1] = TAL_MAP_TX2_ORX;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXTOORXMAP_INV_ORX1_MAP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* cmdByte assignment from oRx2Map */
	switch (oRx2Map) {
	case TAL_MAP_NONE:
		extData[1] &= ~0x0C; /* clear bits [3:2] */
		break;
	case TAL_MAP_TX1_ORX:
		extData[1] |= (TAL_MAP_TX1_ORX << 2);
		break;
	case TAL_MAP_TX2_ORX:
		extData[1] |= (TAL_MAP_TX2_ORX << 2);
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETTXTOORXMAP_INV_ORX2_MAP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* adding the txCalEnable bit */
	if (txCalEnable > 0) {
		extData[1] |= ENABLE_TXCALS;
	} else {
		extData[1] &= ~ENABLE_TXCALS;
	}

	/* write new setting to the command register */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &extData[0], 2);
	IF_ERR_RETURN_U32(retVal);

	/* check for completion */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETTXTOORXMAP_TIMEOUT_US,
			SETTXTOORXMAP_INTERVAL_US);
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, extData[0], cmdStatusByte), retVal,
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

uint32_t TALISE_setRfPllFrequency(taliseDevice_t *device,
				  taliseRfPllName_t pllName, uint64_t rfPllLoFrequency_Hz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	uint8_t armData[8] = {0};
	uint8_t extData[2] = {0};
	uint8_t cmdStatusByte = 0;
	uint8_t gpStatus = 0;
	uint8_t areCalsRunning = 0;
	uint8_t errorFlag = 0;
	int64_t txFreqValid = 0;
	int64_t rxFreqValid = 0;
	int64_t orxFreqValid = 0;

	/* return error if init cals are in progress */
	retVal = (talRecoveryActions_t)TALISE_checkInitCalComplete(device,
			&areCalsRunning, &errorFlag);
	IF_ERR_RETURN_U32(retVal);

	if ((areCalsRunning == 1) || (device->devStateInfo.swTest == 1)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_SETRFPLL_INITCALS_INPROGRESS, retVal, TALACT_ERR_BBIC_LOG_ERROR);
	}

	/*if Tx profile valid, (rfpllFrequency - TxProfileRFBW/2) > 0 */
	txFreqValid = (int64_t)rfPllLoFrequency_Hz - (int64_t)(
			      device->devStateInfo.txBandwidth_Hz >> 1);
	if(((device->devStateInfo.profilesValid & TX_PROFILE_VALID) > 0)
	    && (txFreqValid <= 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RFPLLFREQ_TX_OUT_OF_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*If Rx profile valid, (rfpllFrequency - RxProfileRFBW/2) > 0 */
	rxFreqValid = (int64_t)rfPllLoFrequency_Hz - (int64_t)(
			      device->devStateInfo.rxBandwidth_Hz >> 1);
	if(((device->devStateInfo.profilesValid & RX_PROFILE_VALID) > 0)
	    && (rxFreqValid <= 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RFPLLFREQ_RX_OUT_OF_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*If ORx profile valid, (rfpllFrequency - OrxProfileRFBW/2) > 0 */
	orxFreqValid = (int64_t)rfPllLoFrequency_Hz - (int64_t)(
			       device->devStateInfo.orxBandwidth_Hz >> 1);
	if(((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0)
	    && (orxFreqValid <= 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_RFPLLFREQ_ORX_OUT_OF_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* loading byte array with parsed bytes from rfPllLoFrequency_Hz word */
	armData [0] = (uint8_t)(rfPllLoFrequency_Hz & 0xFF);
	armData [1] = (uint8_t)((rfPllLoFrequency_Hz >> 8) & 0xFF);
	armData [2] = (uint8_t)((rfPllLoFrequency_Hz >> 16) & 0xFF);
	armData [3] = (uint8_t)((rfPllLoFrequency_Hz >> 24) & 0xFF);
	armData [4] = (uint8_t)((rfPllLoFrequency_Hz >> 32) & 0xFF);
	armData [5] = (uint8_t)((rfPllLoFrequency_Hz >> 40) & 0xFF);
	armData [6] = (uint8_t)((rfPllLoFrequency_Hz >> 48) & 0xFF);
	armData [7] = (uint8_t)((rfPllLoFrequency_Hz >> 56) & 0xFF);

	/* write 64-bit frequency value to ARM memory */
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armData[0], 8);
	IF_ERR_RETURN_U32(retVal);

	/* setting the object ID for RF PLL LO frequency setting */
	extData[0] = TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY;

	/* selecting the PLL of interest */
	switch (pllName) {
	case TAL_RF_PLL:
		extData[1] = 0x00;
		break;
	case TAL_AUX_PLL:
		extData[1] = 0x01;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRFPLL_INV_PLLNAME, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* save the GP Interrutpt mask */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
				  &gpStatus);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Mask GP interrupt for the PLL being set - only write if mask not already set */
	if ((pllName == TAL_AUX_PLL) &&
	    ((gpStatus & (uint8_t)TAL_GP_MASK_AUX_SYNTH_UNLOCK) == 0)) {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
					   (gpStatus | (uint8_t)TAL_GP_MASK_AUX_SYNTH_UNLOCK));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else if ((pllName == TAL_RF_PLL) &&
		   ((gpStatus & (uint8_t)TAL_GP_MASK_RF_SYNTH_UNLOCK) == 0)) {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
					   (gpStatus | (uint8_t)TAL_GP_MASK_RF_SYNTH_UNLOCK));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* executing the SET command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &extData[0], 2);
	IF_ERR_RETURN_U32(retVal);

	/* waiting for command to complete */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETRFPLL_TIMEOUT_US,
			SETRFPLL_INTERVAL_US);

	/* if PLL GP interrupt was changed, then restore original GP interrupt mask */
	if (((pllName == TAL_AUX_PLL) &&
	     ((gpStatus & (uint8_t)TAL_GP_MASK_AUX_SYNTH_UNLOCK) == 0)) ||
	    ((pllName == TAL_RF_PLL) &&
	     ((gpStatus & (uint8_t)TAL_GP_MASK_RF_SYNTH_UNLOCK) == 0))) {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
					   gpStatus);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* check previous TALISE_waitArmCmdStatus for ARM errors */
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;

}

uint32_t TALISE_getRfPllFrequency(taliseDevice_t *device,
				  taliseRfPllName_t pllName, uint64_t *rfPllLoFrequency_Hz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t armData[8] = {0};
	uint8_t extData[2] = {0};
	uint8_t cmdStatusByte = 0;
	uint8_t getClkPllFrequency = 0;
	uint32_t clkPllIntWord = 0;
	uint32_t clkPllFracWord = 0;
	uint8_t clkPllRefClkDiv = 0;
	uint8_t devClkDivideRatio = 0;
	uint8_t hsDivTimes10 = 0;
	uint8_t hsDivReg = 0;
	uint8_t clkPllIntWord7_0 = 0;
	uint8_t clkPllIntWord10_8 = 0;
	uint8_t clkPllFracWord7_0 = 0;
	uint8_t clkPllFracWord15_8 = 0;
	uint8_t clkPllFracWord22_16 = 0;
	uint64_t refclk_Hz = 0;

	static const uint32_t modDiv2 = 1044480;
	static const uint8_t vcoDiv = 2;
	static const uint8_t hsDigClkDiv2 = 2;
	static const uint16_t kHzToHz = 1000;


#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRfPllFrequency()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* first extended data byte is the object ID */
	extData[0] = TALISE_ARM_OBJECTID_RFPLL_LO_FREQUENCY;

	/* PLL select with error check */
	switch (pllName) {
	case TAL_CLK_PLL:
		getClkPllFrequency = 1;
		break;
	case TAL_RF_PLL:
		extData[1] = 0x00;
		break;
	case TAL_AUX_PLL:
		extData[1] = 0x01;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRFPLL_INV_PLLNAME, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* rfPllLoFrequency_Hz null pointer check */
	if (rfPllLoFrequency_Hz == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRFPLL_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* calculate CLK PLL frequency */
	else if (getClkPllFrequency == 1) {
		/* read in integer and fractional PLL data into variables */
		halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_2,
					   &hsDivReg, 0x07, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_5,
					   &devClkDivideRatio, 0x30, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_CLOCK_CONTROL_2,
					   &clkPllRefClkDiv, 0x70, 4);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadByte(device->devHalInfo,
					  TALISE_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE0, &clkPllIntWord7_0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadField(device->devHalInfo,
					   TALISE_ADDR_CLK_SYNTH_DIVIDER_INT_BYTE1, &clkPllIntWord10_8, 0x07, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadByte(device->devHalInfo,
					  TALISE_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE0, &clkPllFracWord7_0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadByte(device->devHalInfo,
					  TALISE_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE1, &clkPllFracWord15_8);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadField(device->devHalInfo,
					   TALISE_ADDR_CLK_SYNTH_DIVIDER_FRAC_BYTE2, &clkPllFracWord22_16, 0x7F, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* forming CLK PLL integer and fractional words */
		clkPllIntWord = ((uint32_t)(clkPllIntWord10_8) << 8) | (uint32_t)(
					clkPllIntWord7_0);
		clkPllFracWord = ((uint32_t)(clkPllFracWord22_16) << 16) | ((uint32_t)(
					 clkPllFracWord15_8) << 8) | (uint32_t)(clkPllFracWord7_0);

		/* determine CLK PLL reference clock divider */
		switch (devClkDivideRatio) {
		case 0:
			/* times 1 */
			refclk_Hz = ((uint64_t)device->devStateInfo.clocks.deviceClock_kHz * kHzToHz);
			break;
		case 1:
			/* div 2 */
			refclk_Hz = (((uint64_t)device->devStateInfo.clocks.deviceClock_kHz * kHzToHz)
				     >> 1);
			break;
		case 2:
			/* div 4 */
			refclk_Hz = (((uint64_t)device->devStateInfo.clocks.deviceClock_kHz * kHzToHz)
				     >> 2);
			break;

		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
							  TAL_ERR_GET_PLLFREQ_INV_REFCLKDIV, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		switch (clkPllRefClkDiv) {
		case 0:
			/* times 1 */
			/* refclk_Hz = (refclk_Hz); */ /* No need to assign to same value */
			break;
		case 1:
			/* div 2 */
			refclk_Hz = (refclk_Hz >> 1);
			break;
		case 2:
			/* div 4 */
			refclk_Hz = (refclk_Hz >> 2);
			break;
		case 3:
			/* times 2 */
			refclk_Hz = (refclk_Hz << 1);
			break;

		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
							  TAL_ERR_GET_PLLFREQ_INV_REFCLKDIV, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		/* HS divider scalar assignment */
		switch (hsDivReg) {
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
		default: {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
							  TAL_ERR_GET_PLLFREQ_INV_HSDIV, retVal, TALACT_ERR_CHECK_PARAM);
		}
		}

		/* calculate PLL clock frequency - round to nearest Hz for fractional word (fractional modulus = 2088960) */
		*rfPllLoFrequency_Hz = DIV_U64(((refclk_Hz * clkPllIntWord) + ((DIV_U64(
							refclk_Hz * clkPllFracWord, modDiv2) + 1) >> 1)) * vcoDiv * hsDivTimes10,
					       10) * hsDigClkDiv2;
	}
	/* else get RF PLL LO frequency from ARM memory */
	else {
		/* send get opcode to ARM */
		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_GET_OPCODE, &extData[0], 2);
		IF_ERR_RETURN_U32(retVal);

		/* wait for command to complete */
		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETRFPLL_TIMEOUT_US,
				GETRFPLL_INTERVAL_US);
		if((cmdStatusByte >> 1) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
							  TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}
		/* read 64-bit frequency from ARM memory */
		retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
				TALISE_ADDR_ARM_START_DATA_ADDR, &armData[0], 8, 0);
		IF_ERR_RETURN_U32(retVal);

		/* form RF PLL LO frequency word for read-back */
		*rfPllLoFrequency_Hz =  ((uint64_t)(armData[0])) |
					((uint64_t)(armData[1]) << 8) |
					((uint64_t)(armData[2]) << 16) |
					((uint64_t)(armData[3]) << 24) |
					((uint64_t)(armData[4]) << 32) |
					((uint64_t)(armData[5]) << 40) |
					((uint64_t)(armData[6]) << 48) |
					((uint64_t)(armData[7]) << 56);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getPllsLockStatus(taliseDevice_t *device,
				  uint8_t *pllLockStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readData = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getPllsLockStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* null pointer check */
	if (pllLockStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_CHECKPLLLOCK_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* clear the pointer contents */
		*pllLockStatus = 0;

		halError = talSpiReadField(device->devHalInfo,
					   TALISE_ADDR_CLK_SYNTH_VCO_BAND_BYTE1, &readData, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
		*pllLockStatus = readData;

		halError = talSpiReadField(device->devHalInfo,
					   TALISE_ADDR_RF_SYNTH_VCO_BAND_BYTE1, &readData, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
		*pllLockStatus = *pllLockStatus | (readData << 1);

		halError = talSpiReadField(device->devHalInfo,
					   TALISE_ADDR_AUX_SYNTH_VCO_BAND_BYTE1, &readData, 0x01, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
		*pllLockStatus = *pllLockStatus | (readData << 2);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setRfPllLoopFilter(taliseDevice_t *device,
				   uint16_t loopBandwidth_kHz, uint8_t stability)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setRfPllLoopFilter()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	retVal = (talRecoveryActions_t)TALISE_setPllLoopFilter(device, TAL_RF_PLL,
			loopBandwidth_kHz, stability);
	IF_ERR_RETURN_U32(retVal);

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getRfPllLoopFilter(taliseDevice_t *device,
				   uint16_t *loopBandwidth_kHz, uint8_t *stability)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;


#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getRfPllLoopFilter()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	retVal = (talRecoveryActions_t)TALISE_getPllLoopFilter(device, TAL_RF_PLL,
			loopBandwidth_kHz, stability);
	IF_ERR_RETURN_U32(retVal);

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setPllLoopFilter(taliseDevice_t *device,
				 taliseRfPllName_t pllName, uint16_t loopBandwidth_kHz, uint8_t stability)
{
	static const uint8_t AUXPLL_BIT = 0x80;
	static const uint8_t RFPLL_BIT = 0x00;

	uint8_t extData[4] = {0};
	uint8_t cmdStatusByte = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t auxPllSel = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setPllLoopFilter()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* Range check for loopBandwidth - Should be between 50kHz and 750kHz */
	if ((loopBandwidth_kHz < 50) ||
	    (loopBandwidth_kHz > 750)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRFPLL_LOOPFILTER_INV_LOOPBANDWIDTH, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Range check for stability - Should be between 3 and 15 */
	if ((stability < 3) ||
	    (stability > 15)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRFPLL_LOOPFILTER_INV_STABILITY, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((pllName != TAL_RF_PLL) &&
	    (pllName != TAL_AUX_PLL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETRFPLL_LOOPFILTER_INV_PLLSEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	auxPllSel = (pllName == TAL_AUX_PLL) ? AUXPLL_BIT : RFPLL_BIT;
	/* Generate payload for SET command */
	extData[0] = TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ;
	extData[1] = auxPllSel | (stability & 0x0F);
	extData[2] = (uint8_t)(loopBandwidth_kHz);
	extData[3] = (uint8_t)(loopBandwidth_kHz >> 8);

	/* executing the SET command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &extData[0], 4);
	IF_ERR_RETURN_U32(retVal);

	/* waiting for command to complete */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETRFPLL_LOOP_FREQ_TIMEOUT_US,
			SETRFPLL_LOOP_FREQ_INTERVAL_US);

	/* performing command status check */
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, extData[0], cmdStatusByte), retVal,
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

uint32_t TALISE_getPllLoopFilter(taliseDevice_t *device,
				 taliseRfPllName_t pllName, uint16_t *loopBandwidth_kHz, uint8_t *stability)
{
	static const uint8_t AUXPLL_BIT = 0x80;
	static const uint8_t RFPLL_BIT = 0x00;
	static const uint8_t STABILITY_BITMASK = 0x0F;

	uint8_t extData[2] = {TALISE_ARM_OBJECTID_RFPLL_LOOP_FREQ, 0};
	uint8_t armData[3] = {0};
	uint8_t cmdStatusByte = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint8_t auxPllSel = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getPllLoopFilter()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((pllName != TAL_RF_PLL) &&
	    (pllName != TAL_AUX_PLL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRFPLL_LOOPFILTER_INV_PLLSEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for NULL */
	if ((loopBandwidth_kHz == NULL) ||
	    (stability == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETRFPLL_LOOPFILTER_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	auxPllSel = (pllName == TAL_AUX_PLL) ? AUXPLL_BIT : RFPLL_BIT;
	extData[1] = auxPllSel;
	/* executing the GET command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &extData[0], sizeof(extData));
	IF_ERR_RETURN_U32(retVal);

	/* waiting for command to complete */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETRFPLL_LOOP_FREQ_TIMEOUT_US,
			GETRFPLL_LOOP_FREQ_INTERVAL_US);

	/* performing command status check */
	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, extData[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* Read from ARM memory */
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armData[0], 3, 0);
	IF_ERR_RETURN_U32(retVal);

	/* Assign to variables */
	*stability = (armData[2] & STABILITY_BITMASK);
	*loopBandwidth_kHz = (((uint16_t)armData[1]) << 8) | ((uint16_t)armData[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_setOrxLoSource(taliseDevice_t *device,
			       taliseObsRxLoSource_t orxLoSource)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t streamId = 0;
	uint8_t armCommandBusy = 0;

	uint32_t timeout_us      = RUNSTREAMCMD_TIMEOUT_US;
	uint32_t waitInterval_us = RUNSTREAMCMD_INTERVAL_US;
	uint32_t eventCheck      = 0;
	uint32_t numEventChecks  = 0;

	const uint8_t GPIO3_RISING_STREAMID = 25;
	const uint8_t GPIO3_FALLING_STREAMID = 29;
	const uint8_t ARMCMD_RUN_STREAM = 0x1F;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setOrxLoSource()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (orxLoSource == TAL_OBSLO_RF_PLL) {
		streamId = GPIO3_FALLING_STREAMID;
	} else if (orxLoSource == TAL_OBSLO_AUX_PLL) {
		streamId = GPIO3_RISING_STREAMID;
	} else {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETORXLOSRC_INVALIDPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* setting a timeout for ARM mailbox busy bit to be clear (can't send an arm mailbox command until mailbox is ready) */
	waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
	numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

	/* timeout event loop to permit non-blocking of thread */
	for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++) {
		halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_ARM_COMMAND,
					   &armCommandBusy, 0x80, 7);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
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
						  TAL_ERR_SETORXLOSRC_TIMEDOUT_ARMMAILBOXBUSY, retVal, TALACT_ERR_RESET_ARM);
	} else {
		/* Write stream ID to run */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_EXT_CMD_BYTE_1,
					   streamId);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Run stream */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_ARM_COMMAND,
					   ARMCMD_RUN_STREAM);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getOrxLoSource(taliseDevice_t *device,
			       taliseObsRxLoSource_t *orx1LoSource, taliseObsRxLoSource_t *orx2LoSource)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t orxLoSourceBitfieldValue = 0;
	const uint8_t ORXLOSRC_BITMASK = 0x0C;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getOrxLoSource()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check for NULL */
	if ((orx1LoSource == NULL) ||
	    (orx2LoSource == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETORXLOSRC_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_LO_MUX_CONFIG,
				   &orxLoSourceBitfieldValue, ORXLOSRC_BITMASK, 2);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*orx1LoSource = (taliseObsRxLoSource_t)(orxLoSourceBitfieldValue & 0x01);
	*orx2LoSource = (taliseObsRxLoSource_t)((orxLoSourceBitfieldValue >> 1) & 0x01);

	return (uint32_t)retVal;
}

static uint32_t talSetFhmGpio(taliseDevice_t *device,
			      taliseGpioPinSel_t fhmGpioPin)
{
	static const uint8_t GPIO_STATUS_CMD_SIZE_BYTES = 2;
	static const uint8_t GPIO_CONFIG_CMD_SIZE_BYTES = 3;
	static const uint8_t SIGNALID_RF_PLL_FREQ_HOP = 0x09;
	static const uint8_t GPIO_CONFIG_POLARITY = 0;
	static const uint8_t GPIO_CONFIG_DISABLE =  0;
	static const uint8_t GPIO_CONFIG_ENABLE = 0x80;
	static const uint8_t ARM_AUTO_INCREMENT = 0;
	static const uint8_t ARM_GPIO_READ_NUM_BYTES = 1;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t armGetGpioStatus = 0;
	uint32_t usedGpioPins = 0;
	uint32_t gpioFreeMask = 0;
	uint32_t gpioUsedMask = 0;
	uint8_t gpioStatus[GPIO_STATUS_CMD_SIZE_BYTES];
	uint8_t gpioConfig[GPIO_CONFIG_CMD_SIZE_BYTES];

	/*Initialize gpioStatus array C-99 style*/
	gpioStatus[0] = TALISE_ARM_OBJECTID_GPIO_CTRL;
	gpioStatus[1] = SIGNALID_RF_PLL_FREQ_HOP;

	/*Initialize gpioConfig array C-99 style*/
	gpioConfig[0] = TALISE_ARM_OBJECTID_GPIO_CTRL;
	gpioConfig[1] = SIGNALID_RF_PLL_FREQ_HOP;
	gpioConfig[2] = 0;

	/* Get current GPIO pin assigned to RF_PLL_FREQ_HOP signal_id */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &gpioStatus[0], sizeof(gpioStatus));
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, SETARMGPIO_TIMEOUT_US,
			SETARMGPIO_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, gpioStatus[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armGetGpioStatus, ARM_GPIO_READ_NUM_BYTES,
			ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);

	if (armGetGpioStatus <= (uint8_t)TAL_GPIO_15) {
		gpioFreeMask = (1 << (armGetGpioStatus & 0x0F));
	}

	usedGpioPins = device->devStateInfo.usedGpiopins & ~gpioFreeMask;

	if(fhmGpioPin <= TAL_GPIO_15) {
		gpioUsedMask = (1 << fhmGpioPin);
		gpioConfig[2] = ((uint8_t)fhmGpioPin | GPIO_CONFIG_POLARITY |
				 GPIO_CONFIG_ENABLE);
	}

	else if(fhmGpioPin == TAL_GPIO_INVALID) {
		gpioUsedMask = 0;
		gpioConfig[2] = ((uint8_t)TAL_GPIO_INVALID | GPIO_CONFIG_POLARITY |
				 GPIO_CONFIG_DISABLE);
	}

	/*Check if GPIO pin is already in use*/
	if ((usedGpioPins & gpioUsedMask) != 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMCONFIG_FHMGPIOPIN_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	usedGpioPins |= gpioUsedMask;

	/*Send FHM GPIO config to ARM*/
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &gpioConfig[0], sizeof(gpioConfig));
	IF_ERR_RETURN_U32(retVal);

	/* execute Set FHM GPIO config command */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETARMGPIO_TIMEOUT_US,
			SETARMGPIO_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						   ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, gpioConfig[0], cmdStatusByte), retVal,
						   TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO's to 0 (input) */
	/* - Set OE for all used GPIO's to 0 (input) */
	retVal = (talRecoveryActions_t)TALISE_setGpioOe(device, 0,
			(gpioUsedMask | gpioFreeMask));
	IF_ERR_RETURN(retVal);

	device->devStateInfo.usedGpiopins = usedGpioPins;

	return (uint32_t)retVal;
}

static uint32_t talGetFhmGpio(taliseDevice_t *device,
			      taliseGpioPinSel_t *fhmGpioPin)
{
	static const uint8_t GPIO_STATUS_CMD_SIZE_BYTES = 2;
	static const uint8_t SIGNALID_RF_PLL_FREQ_HOP = 0x09;
	static const uint8_t ARM_AUTO_INCREMENT = 0;
	static const uint8_t ARM_GPIO_READ_NUM_BYTES = 1;
	static const uint8_t GPIO_PINSEL_MASK = 0x1F;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t armGetGpioStatus = 0;
	uint8_t gpioStatus[GPIO_STATUS_CMD_SIZE_BYTES];

	/*Initialize gpioStatus array C-99 style*/
	gpioStatus[0] = TALISE_ARM_OBJECTID_GPIO_CTRL;
	gpioStatus[1] = SIGNALID_RF_PLL_FREQ_HOP;

	/* Get current GPIO pin assigned to RF_PLL_FREQ_HOP signal_id */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &gpioStatus[0], sizeof(gpioStatus));
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, SETARMGPIO_TIMEOUT_US,
			SETARMGPIO_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, gpioStatus[0], cmdStatusByte), retVal,
						  TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armGetGpioStatus, ARM_GPIO_READ_NUM_BYTES,
			ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);

	*fhmGpioPin = (taliseGpioPinSel_t)(armGetGpioStatus & GPIO_PINSEL_MASK);

	return (uint32_t)retVal;
}

uint32_t TALISE_setFhmConfig(taliseDevice_t *device,
			     taliseFhmConfig_t *fhmConfig)
{
	static const int64_t MHZ_TO_HZ_CONVERSION_FACTOR = 1000000;
	static const uint8_t ARM_CFG_BUF_SIZE = 8;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	int64_t freqValid = 0;
	uint8_t byteOffset = 0;
	uint8_t armConfigBuf[8] = {0};

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setFhmConfig()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/*Check for NULL*/
	if(fhmConfig == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMCONFIG_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Check that Frequency Hop Trigger GPIO pin is valid*/
	if((fhmConfig->fhmGpioPin > TAL_GPIO_15) &&
	    (fhmConfig->fhmGpioPin != TAL_GPIO_INVALID)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMCONFIG_INV_FHMGPIOPIN, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if(fhmConfig->fhmMinFreq_MHz >= fhmConfig->fhmMaxFreq_MHz) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMCONFIG_INV_FHMCONFIG_FHM_MAX_FREQ, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/*Check that minimum hopping range frequency is at least greater than or equal to DC + Bandwidth/2*/
	/*If Rx profile valid, (fhmMinFreq - RxProfileRFBW/2) > 0 */
	freqValid = ((int64_t)fhmConfig->fhmMinFreq_MHz * MHZ_TO_HZ_CONVERSION_FACTOR)
		    - (int64_t)(device->devStateInfo.rxBandwidth_Hz >> 1);
	if(((device->devStateInfo.profilesValid & RX_PROFILE_VALID) > 0)
	    && (freqValid <= 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMCONFIG_INV_FHMCONFIG_FHM_MIN_FREQ, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/*If Tx profile valid, (fhmMinFreq - TxProfileRFBW/2) > 0 */
	freqValid = ((int64_t)fhmConfig->fhmMinFreq_MHz * MHZ_TO_HZ_CONVERSION_FACTOR)
		    - (int64_t)(device->devStateInfo.txBandwidth_Hz >> 1);
	if(((device->devStateInfo.profilesValid & TX_PROFILE_VALID) > 0)
	    && (freqValid <= 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMCONFIG_INV_FHMCONFIG_FHM_MIN_FREQ, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/*If ORx profile valid, (fhmMinFreq - OrxProfileRFBW/2) > 0 */
	freqValid = ((int64_t)fhmConfig->fhmMinFreq_MHz * MHZ_TO_HZ_CONVERSION_FACTOR)
		    - (int64_t)(device->devStateInfo.orxBandwidth_Hz >> 1);
	if(((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) > 0)
	    && (freqValid <= 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMCONFIG_INV_FHMCONFIG_FHM_MIN_FREQ, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/*Set FHM GPIO pin*/
	retVal = (talRecoveryActions_t)talSetFhmGpio(device, fhmConfig->fhmGpioPin);
	IF_ERR_RETURN_U32(retVal);

	/*Set the FHM frequency range configuration if a valid GPIO pin is selected*/
	armConfigBuf[0] = (uint8_t)(fhmConfig->fhmMinFreq_MHz & 0xFF);
	armConfigBuf[1] = (uint8_t)((fhmConfig->fhmMinFreq_MHz >> 8) & 0xFF);
	armConfigBuf[2] = (uint8_t)((fhmConfig->fhmMinFreq_MHz >> 16) & 0xFF);
	armConfigBuf[3] = (uint8_t)((fhmConfig->fhmMinFreq_MHz >> 24) & 0xFF);

	armConfigBuf[4] = (uint8_t)(fhmConfig->fhmMaxFreq_MHz & 0xFF);
	armConfigBuf[5] = (uint8_t)((fhmConfig->fhmMaxFreq_MHz >> 8) & 0xFF);
	armConfigBuf[6] = (uint8_t)((fhmConfig->fhmMaxFreq_MHz >> 16) & 0xFF);
	armConfigBuf[7] = (uint8_t)((fhmConfig->fhmMaxFreq_MHz >> 24) & 0xFF);

	byteOffset = 0x20;
	retVal = (talRecoveryActions_t)TALISE_writeArmConfig(device,
			TALISE_ARM_OBJECTID_SYTEM_INFO,
			byteOffset, &armConfigBuf[0], ARM_CFG_BUF_SIZE);
	IF_ERR_RETURN_U32(retVal);

	/*Update device data structure with frequency hopping range*/
	device->devStateInfo.talFhmFreqRange.fhmMaxFreq_MHz = fhmConfig->fhmMaxFreq_MHz;
	device->devStateInfo.talFhmFreqRange.fhmMinFreq_MHz = fhmConfig->fhmMinFreq_MHz;

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getFhmConfig(taliseDevice_t *device,
			     taliseFhmConfig_t *fhmConfig)
{
	static const uint8_t ARM_CFG_BUF_SIZE = 8;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t armCfgBuf[8] = {0};
	uint8_t byteOffset = 0;

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getFhmConfig()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/*Check for NULL*/
	if(fhmConfig == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETFHMCONFIG_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = (talRecoveryActions_t)talGetFhmGpio(device, &fhmConfig->fhmGpioPin);
	IF_ERR_RETURN_U32(retVal);

	byteOffset = 0x20;
	retVal = (talRecoveryActions_t)TALISE_readArmConfig(device,
			TALISE_ARM_OBJECTID_SYTEM_INFO,
			byteOffset, &armCfgBuf[0], ARM_CFG_BUF_SIZE);
	IF_ERR_RETURN_U32(retVal);

	fhmConfig->fhmMinFreq_MHz = ((uint32_t)armCfgBuf[0] |
				     ((uint32_t)armCfgBuf[1] << 8) |
				     ((uint32_t)armCfgBuf[2] << 16) |
				     ((uint32_t)armCfgBuf[3] << 24));

	fhmConfig->fhmMaxFreq_MHz = ((uint32_t)armCfgBuf[4] |
				     ((uint32_t)armCfgBuf[5] << 8) |
				     ((uint32_t)armCfgBuf[6] << 16) |
				     ((uint32_t)armCfgBuf[7] << 24));

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

static uint32_t talSetFhmRfPllFreq(taliseDevice_t *device,
				   uint64_t nextRfPllFreq_Hz)
{
	static const uint8_t FHM_NEXT_RF_PLL_FREQ_BYTES = 8;
	static const uint64_t BYTE0_MASK = 0x00000000000000FF;
	static const uint64_t BYTE1_MASK = 0x000000000000FF00;
	static const uint64_t BYTE2_MASK = 0x0000000000FF0000;
	static const uint64_t BYTE3_MASK = 0x00000000FF000000;
	static const uint64_t BYTE4_MASK = 0x000000FF00000000;
	static const uint64_t BYTE5_MASK = 0x0000FF0000000000;
	static const uint64_t BYTE6_MASK = 0x00FF000000000000;
	static const uint64_t BYTE7_MASK = 0xFF00000000000000;
	static const uint8_t BYTE1_SHIFT = 8;
	static const uint8_t BYTE2_SHIFT = 16;
	static const uint8_t BYTE3_SHIFT = 24;
	static const uint8_t BYTE4_SHIFT = 32;
	static const uint8_t BYTE5_SHIFT = 40;
	static const uint8_t BYTE6_SHIFT = 48;
	static const uint8_t BYTE7_SHIFT = 56;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t fhmNextRfPllFreqConfigArr[FHM_NEXT_RF_PLL_FREQ_BYTES];

	/*FHM Frequency Range needs to be written to ARM mailbox in the form {fhmMinFreq_MHz, fhmMaxFreq_MHz}*/
	fhmNextRfPllFreqConfigArr[0] = (uint8_t)(nextRfPllFreq_Hz & BYTE0_MASK);
	fhmNextRfPllFreqConfigArr[1] = (uint8_t)((nextRfPllFreq_Hz & BYTE1_MASK) >>
				       BYTE1_SHIFT);
	fhmNextRfPllFreqConfigArr[2] = (uint8_t)((nextRfPllFreq_Hz & BYTE2_MASK) >>
				       BYTE2_SHIFT);
	fhmNextRfPllFreqConfigArr[3] = (uint8_t)((nextRfPllFreq_Hz & BYTE3_MASK) >>
				       BYTE3_SHIFT);
	fhmNextRfPllFreqConfigArr[4] = (uint8_t)((nextRfPllFreq_Hz & BYTE4_MASK) >>
				       BYTE4_SHIFT);
	fhmNextRfPllFreqConfigArr[5] = (uint8_t)((nextRfPllFreq_Hz & BYTE5_MASK) >>
				       BYTE5_SHIFT);
	fhmNextRfPllFreqConfigArr[6] = (uint8_t)((nextRfPllFreq_Hz & BYTE6_MASK) >>
				       BYTE6_SHIFT);
	fhmNextRfPllFreqConfigArr[7] = (uint8_t)((nextRfPllFreq_Hz & BYTE7_MASK) >>
				       BYTE7_SHIFT);

	/* Write 64 bit next FHM hop frequency to ARM Freq Hop mailbox*/
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			TALISE_ADDR_ARM_HOP_FREQ_MEM_ADDR, &fhmNextRfPllFreqConfigArr[0],
			FHM_NEXT_RF_PLL_FREQ_BYTES);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setFhmMode(taliseDevice_t *device, taliseFhmMode_t *fhmMode)
{
	static const uint64_t MHZ_TO_HZ_CONVERSION_FACTOR = 1000000;
	static const uint32_t ARM_FHM_MODE_CMD_NUM_BYTES = 3;
	static const uint8_t FHM_ENABLE = 1;
	static const uint8_t FHM_DISABLE = 0;
	static const uint8_t FHM_ENABLE_MCS_SYNC = 1;
	static const uint8_t FHM_DISABLE_MCS_SYNC = 0;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t cmdStatusByte = 0;
	uint8_t armFhmModeCfgCmd[3] = { 0 };

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setFhmMode()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/*Check for NULL*/
	if(fhmMode == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMMODE_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if(fhmMode->fhmEnable > 0) {
		/*Check if requested FHM init frequency on enable is valid*/
		if((fhmMode->fhmInitFrequency_Hz > ((uint64_t)
						    device->devStateInfo.talFhmFreqRange.fhmMaxFreq_MHz *
						    MHZ_TO_HZ_CONVERSION_FACTOR)) ||
		    (fhmMode->fhmInitFrequency_Hz < ((uint64_t)
						     device->devStateInfo.talFhmFreqRange.fhmMinFreq_MHz *
						     MHZ_TO_HZ_CONVERSION_FACTOR))) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETFHMMODE_INV_FHM_INIT_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/*Check if requested FHM mode is valid*/
		if((fhmMode->fhmTriggerMode != TAL_FHM_GPIO_MODE) &&
		    (fhmMode->fhmTriggerMode != TAL_FHM_NON_GPIO_MODE)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETFHMMODE_INV_FHM_TRIGGER_MODE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/*Write initial FHM hop frequency to ARM mailbox*/
		retVal = (talRecoveryActions_t)talSetFhmRfPllFreq(device,
				fhmMode->fhmInitFrequency_Hz);
		IF_ERR_RETURN_U32(retVal);

		/*Send FHM Enable command to ARM. The command comprises of the following
		  Byte0 - Fast Freq Hop Obj ID
		  Byte1 - FHM Enable
		  Byte2 - Enable MCS Synchronization*/
		armFhmModeCfgCmd[0] = (uint8_t)TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE;
		armFhmModeCfgCmd[1] = FHM_ENABLE;
		armFhmModeCfgCmd[2] = ( fhmMode->enableMcsSync > 0 ) ? FHM_ENABLE_MCS_SYNC :
				      FHM_DISABLE_MCS_SYNC;
		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_SET_OPCODE, &armFhmModeCfgCmd[0], ARM_FHM_MODE_CMD_NUM_BYTES);
		IF_ERR_RETURN_U32(retVal);

		/*Verify that command executed successfully*/
		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETFREQHOP_MODE_TIMEOUT_US,
				SETFREQHOP_MODE_INTERVAL_US);

		if((cmdStatusByte >> 1) > 0) {
			return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							   ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, armFhmModeCfgCmd[0], cmdStatusByte),
							   retVal, TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}

		/*Store the trigger mode, init FHM frequency and MCS Synch flag in the device data structure*/
		device->devStateInfo.talFhmInitHopFreq_Hz = fhmMode->fhmInitFrequency_Hz;
		device->devStateInfo.talFhmTriggerMode = fhmMode->fhmTriggerMode;
		device->devStateInfo.talFhmMcsSync  = ( fhmMode->enableMcsSync > 0 ) ?
						      FHM_ENABLE_MCS_SYNC : FHM_DISABLE_MCS_SYNC;
	} else if(fhmMode->fhmEnable == 0) {
		if((fhmMode->fhmExitMode != TAL_FHM_QUICK_EXIT) &&
		    (fhmMode->fhmExitMode != TAL_FHM_FULL_EXIT)) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
							  TAL_ERR_SETFHMMODE_INV_FHM_EXIT_MODE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/*Send FHM Enable command to ARM. The command comprises of the following
		  Byte0 - Fast Freq Hop Obj ID
		  Byte1 - FHM Disable
		  Byte2 - FHM Exit Mode*/
		armFhmModeCfgCmd[0] = (uint8_t)TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE;
		armFhmModeCfgCmd[1] = FHM_DISABLE;
		armFhmModeCfgCmd[2] = (uint8_t)fhmMode->fhmExitMode;
		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_SET_OPCODE, &armFhmModeCfgCmd[0], ARM_FHM_MODE_CMD_NUM_BYTES);
		IF_ERR_RETURN_U32(retVal);

		/*Verify that command executed successfully*/
		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETFREQHOP_MODE_TIMEOUT_US,
				SETFREQHOP_MODE_INTERVAL_US);

		if((cmdStatusByte >> 1) > 0) {
			return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							   ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, armFhmModeCfgCmd[0], cmdStatusByte),
							   retVal, TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}

		device->devStateInfo.talFhmInitHopFreq_Hz = 0;
		device->devStateInfo.talFhmTriggerMode = TAL_FHM_INVALID_TRIGGER_MODE;
		device->devStateInfo.talFhmMcsSync  = FHM_DISABLE_MCS_SYNC;
	}

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getFhmMode(taliseDevice_t *device, taliseFhmMode_t *fhmMode)
{
	static const uint8_t ARM_FHM_MODE_READ_CMD_NUM_BYTES = 1;
	static const uint32_t FHM_ENABLE_STATUS_OFFSET = 8;
	static const uint32_t FHM_EXIT_MODE_STATUS_OFFSET = 9;
	static const uint8_t ARM_AUTO_INCREMENT = 0;
	static const uint8_t ARM_MAILBOX_READ_NUM_BYTES = 1;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t cmdStatusByte = 0;
	uint8_t fhmExitMode = 0;
	uint8_t armGetFhmModeCmdArr[ARM_FHM_MODE_READ_CMD_NUM_BYTES];

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getFhmMode()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/*Check for NULL*/
	if(fhmMode == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETFHMMODE_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Send ARM command to read FHM mode*/
	armGetFhmModeCmdArr[0] = TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE;
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &armGetFhmModeCmdArr[0],
			ARM_FHM_MODE_READ_CMD_NUM_BYTES);
	IF_ERR_RETURN_U32(retVal);

	/*Verify that command executed successfully*/
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETFREQHOP_MODE_TIMEOUT_US,
			GETFREQHOP_MODE_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						   ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, armGetFhmModeCmdArr[0], cmdStatusByte),
						   retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/*Read FHM enable status and FHM exit mode status from the mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_ENABLE_STATUS_OFFSET),
			&fhmMode->fhmEnable, ARM_MAILBOX_READ_NUM_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_EXIT_MODE_STATUS_OFFSET), &fhmExitMode,
			ARM_MAILBOX_READ_NUM_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);

	/*Read FHM init frequency, FHM trigger mode, MCS sync flag from the device data structure*/
	fhmMode->fhmExitMode = (taliseFhmExitMode_t)fhmExitMode;
	fhmMode->fhmInitFrequency_Hz = device->devStateInfo.talFhmInitHopFreq_Hz;
	fhmMode->fhmTriggerMode = device->devStateInfo.talFhmTriggerMode;
	fhmMode->enableMcsSync = device->devStateInfo.talFhmMcsSync;

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setFhmHop(taliseDevice_t *device,
			  uint64_t nextRfPllFrequency_Hz)
{
	static const uint8_t ARM_FHM_HOP_SET_CMD_BYTES = 1;
	static const uint64_t MHZ_TO_HZ_CONVERSION_FACTOR = 1000000;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t cmdStatusByte = 0;
	uint8_t armSetFhmHopCmdArr[ARM_FHM_HOP_SET_CMD_BYTES];

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setFhmHop()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/*Check if requested FHM init frequency on enable is valid*/
	if((nextRfPllFrequency_Hz > ((uint64_t)
				     device->devStateInfo.talFhmFreqRange.fhmMaxFreq_MHz *
				     MHZ_TO_HZ_CONVERSION_FACTOR)) ||
	    (nextRfPllFrequency_Hz < ((uint64_t)
				      device->devStateInfo.talFhmFreqRange.fhmMinFreq_MHz *
				      MHZ_TO_HZ_CONVERSION_FACTOR))) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETFHMHOP_INV_FHM_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Write the next Rf Pll freq to ARM mailbox*/
	retVal = (talRecoveryActions_t)talSetFhmRfPllFreq(device,
			nextRfPllFrequency_Hz);
	IF_ERR_RETURN_U32(retVal);

	/*If the FHM trigger mode is non-GPIO, send an ARM command to hop frequency*/
	if(device->devStateInfo.talFhmTriggerMode == TAL_FHM_NON_GPIO_MODE) {
		/*Send ARM command to set FHM Hop*/
		armSetFhmHopCmdArr[0] =
			TALISE_ARM_OBJECTID_FREQ_HOP; /*No payload for set FHM hop cmd*/
		retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
				TALISE_ARM_SET_OPCODE, &armSetFhmHopCmdArr[0], ARM_FHM_HOP_SET_CMD_BYTES);
		IF_ERR_RETURN_U32(retVal);

		/*Verify that command executed successfully*/
		retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
				TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETFREQHOP_MODE_TIMEOUT_US,
				SETFREQHOP_MODE_INTERVAL_US);

		if((cmdStatusByte >> 1) > 0) {
			return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
							   ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, armSetFhmHopCmdArr[0], cmdStatusByte),
							   retVal, TALACT_ERR_RESET_ARM);
		} else {
			IF_ERR_RETURN_U32(retVal);
		}
	}

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getFhmRfPllFrequency(taliseDevice_t *device,
				     uint64_t *fhmRfPllFrequency_Hz)
{
	static const uint8_t ARM_FHM_MODE_READ_CMD_NUM_BYTES = 1;
	static const uint8_t ARM_FHM_MODE_FREQ_NUM_BYTES = 8;
	static const uint8_t ARM_AUTO_INCREMENT = 0;
	static const uint8_t BYTE1_SHIFT = 8;
	static const uint8_t BYTE2_SHIFT = 16;
	static const uint8_t BYTE3_SHIFT = 24;
	static const uint8_t BYTE4_SHIFT = 32;
	static const uint8_t BYTE5_SHIFT = 40;
	static const uint8_t BYTE6_SHIFT = 48;
	static const uint8_t BYTE7_SHIFT = 56;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t cmdStatusByte = 0;
	uint8_t armGetFhmModeCmdArr[1] = { 0 };
	uint8_t armGetFhmRfPllFreqArr[8] = { 0 };

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getFhmRfPllFrequency()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/*Send ARM command to read FHM mode*/
	armGetFhmModeCmdArr[0] = TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE;
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &armGetFhmModeCmdArr[0],
			ARM_FHM_MODE_READ_CMD_NUM_BYTES);
	IF_ERR_RETURN_U32(retVal);

	/*Verify that command executed successfully*/
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETFREQHOP_MODE_TIMEOUT_US,
			GETFREQHOP_MODE_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						   ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, armGetFhmModeCmdArr[0], cmdStatusByte),
						   retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/*Read FHM enable status and FHM exit mode status from the mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armGetFhmRfPllFreqArr[0],
			ARM_FHM_MODE_FREQ_NUM_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);

	/*Update pointer memory with FHM Rf Pll freq read back*/
	*fhmRfPllFrequency_Hz = ((uint64_t)armGetFhmRfPllFreqArr[0] |
				 ((uint64_t)armGetFhmRfPllFreqArr[1] << BYTE1_SHIFT) |
				 ((uint64_t)armGetFhmRfPllFreqArr[2] << BYTE2_SHIFT) |
				 ((uint64_t)armGetFhmRfPllFreqArr[3] << BYTE3_SHIFT) |
				 ((uint64_t)armGetFhmRfPllFreqArr[4] << BYTE4_SHIFT) |
				 ((uint64_t)armGetFhmRfPllFreqArr[5] << BYTE5_SHIFT) |
				 ((uint64_t)armGetFhmRfPllFreqArr[6] << BYTE6_SHIFT) |
				 ((uint64_t)armGetFhmRfPllFreqArr[7] << BYTE7_SHIFT));

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getFhmStatus(taliseDevice_t *device,
			     taliseFhmStatus_t *fhmStatus)
{
	static const uint8_t ARM_FHM_STS_READ_CMD_NUM_BYTES = 1;
	static const uint8_t ARM_FHM_STS_TWO_BYTES = 2;
	static const uint8_t ARM_FHM_STS_FOUR_BYTES = 4;
	static const uint8_t ARM_FHM_STS_EIGHT_BYTES = 8;
	static const uint8_t ARM_AUTO_INCREMENT = 0;
	static const uint32_t FHM_STS_CMD_ERROR_OFFSET = 16;
	static const uint32_t FHM_STS_HOP_ERROR_OFFSET = 18;
	static const uint32_t FHM_STS_NUM_HOPS_OFFSET = 20;
	static const uint32_t FHM_STS_NUM_ERROR_EVENTS_OFFSET = 24;
	static const uint32_t FHM_STS_FREQ_ERROR_EVENT_OFFSET = 32;
	static const uint32_t FHM_STS_NUM_HOPS_OUTSIDE_SR_OFFSET = 40;
	static const uint32_t FHM_STS_HOP_FREQ_OUTSIDE_SR_FREQ_OFFSET = 48;
	static const uint32_t FHM_STS_NUM_INVALID_HOP_FREQ_OFFSET = 56;
	static const uint32_t FHM_STS_LAST_INVALID_FREQ_OFFSET = 64;
	static const uint32_t FHM_STS_COMP_PLL_ERROR_OFFSET = 72;
	static const uint32_t FHM_STS_COMP_PLL_ERROR_FREQ_OFFSET = 80;
	static const uint32_t FHM_STS_RF_PLL_LOCK_FAILED_OFFSET = 88;
	static const uint32_t FHM_STS_RF_PLL_LOCK_FAILED_FREQ_OFFSET = 96;
	static const uint8_t BYTE1_SHIFT = 8;
	static const uint8_t BYTE2_SHIFT = 16;
	static const uint8_t BYTE3_SHIFT = 24;
	static const uint8_t BYTE4_SHIFT = 32;
	static const uint8_t BYTE5_SHIFT = 40;
	static const uint8_t BYTE6_SHIFT = 48;
	static const uint8_t BYTE7_SHIFT = 56;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

	uint8_t cmdStatusByte = 0;
	uint8_t armGetFhmModeCmdArr[ARM_FHM_STS_READ_CMD_NUM_BYTES];
	uint8_t armFhmStsData[8] = { 0 };

	/*Check for NULL*/
	if(fhmStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETFHMSTS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getFhmStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/*Send ARM command to read FHM mode*/
	armGetFhmModeCmdArr[0] = TALISE_ARM_OBJECTID_FAST_FREQ_HOP_MODE;
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &armGetFhmModeCmdArr[0], ARM_FHM_STS_READ_CMD_NUM_BYTES);
	IF_ERR_RETURN_U32(retVal);

	/*Verify that command executed successfully*/
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETFREQHOP_MODE_TIMEOUT_US,
			GETFREQHOP_MODE_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						   ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, armGetFhmModeCmdArr[0], cmdStatusByte),
						   retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/*Read FHM CMD error status from the mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_CMD_ERROR_OFFSET), &armFhmStsData[0],
			ARM_FHM_STS_TWO_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	/*Update FHM CMD error status pointer memory*/
	fhmStatus->currentFhmCmdErrorStatus = ((uint32_t)armFhmStsData[0] |
					       ((uint32_t)armFhmStsData[1] << BYTE1_SHIFT));

	/*Read FHM hop error status from the mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_HOP_ERROR_OFFSET), &armFhmStsData[0],
			ARM_FHM_STS_TWO_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	/*Update FHM hop error status pointer memory*/
	fhmStatus->currentFhmHopErrorStatus = ((uint32_t)armFhmStsData[0] |
					       ((uint32_t)armFhmStsData[1] << BYTE1_SHIFT));

	/*Read Total no. of hops in mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_NUM_HOPS_OFFSET), &armFhmStsData[0],
			ARM_FHM_STS_FOUR_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->numFhmHops = ((uint32_t)armFhmStsData[0] |
				 ((uint32_t)armFhmStsData[1] << BYTE1_SHIFT) |
				 ((uint32_t)armFhmStsData[2] << BYTE2_SHIFT) |
				 ((uint32_t)armFhmStsData[3] << BYTE3_SHIFT));

	/*Read no. of error events from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_NUM_ERROR_EVENTS_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_FOUR_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->numFhmNoErrorEvents = ((uint32_t)armFhmStsData[0] |
					  ((uint32_t)armFhmStsData[1] << BYTE1_SHIFT) |
					  ((uint32_t)armFhmStsData[2] << BYTE2_SHIFT) |
					  ((uint32_t)armFhmStsData[3] << BYTE3_SHIFT));

	/*Read the last frequency for which error occurred*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_FREQ_ERROR_EVENT_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_EIGHT_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->lastFhmNoErrorFreq_Hz = ((uint64_t)armFhmStsData[0] |
					    ((uint64_t)armFhmStsData[1] << BYTE1_SHIFT) |
					    ((uint64_t)armFhmStsData[2] << BYTE2_SHIFT) |
					    ((uint64_t)armFhmStsData[3] << BYTE3_SHIFT) |
					    ((uint64_t)armFhmStsData[4] << BYTE4_SHIFT) |
					    ((uint64_t)armFhmStsData[5] << BYTE5_SHIFT) |
					    ((uint64_t)armFhmStsData[6] << BYTE6_SHIFT) |
					    ((uint64_t)armFhmStsData[7] << BYTE7_SHIFT));

	/*Read no. of hops outside scan range from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_NUM_HOPS_OUTSIDE_SR_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_FOUR_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->numFhmHopsOutsideScanRange = ((uint32_t)armFhmStsData[0] |
						((uint32_t)armFhmStsData[1] << BYTE1_SHIFT) |
						((uint32_t)armFhmStsData[2] << BYTE2_SHIFT) |
						((uint32_t)armFhmStsData[3] << BYTE3_SHIFT));

	/*Read the last hop frequency outside scan range*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_HOP_FREQ_OUTSIDE_SR_FREQ_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_EIGHT_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->lastFreqOutsideScanRange_Hz = ((uint64_t)armFhmStsData[0] |
			((uint64_t)armFhmStsData[1] << BYTE1_SHIFT) |
			((uint64_t)armFhmStsData[2] << BYTE2_SHIFT) |
			((uint64_t)armFhmStsData[3] << BYTE3_SHIFT) |
			((uint64_t)armFhmStsData[4] << BYTE4_SHIFT) |
			((uint64_t)armFhmStsData[5] << BYTE5_SHIFT) |
			((uint64_t)armFhmStsData[6] << BYTE6_SHIFT) |
			((uint64_t)armFhmStsData[7] << BYTE7_SHIFT));

	/*Read no. of invalid hop frequencies from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_NUM_INVALID_HOP_FREQ_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_FOUR_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->numInvalidFhmHopFrequencies = ((uint32_t)armFhmStsData[0] |
			((uint32_t)armFhmStsData[1] << BYTE1_SHIFT) |
			((uint32_t)armFhmStsData[2] << BYTE2_SHIFT) |
			((uint32_t)armFhmStsData[3] << BYTE3_SHIFT));

	/*Read the last invalid hop frequency from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_LAST_INVALID_FREQ_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_EIGHT_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->lastInvalidHopFreq_Hz = ((uint64_t)armFhmStsData[0] |
					    ((uint64_t)armFhmStsData[1] << BYTE1_SHIFT) |
					    ((uint64_t)armFhmStsData[2] << BYTE2_SHIFT) |
					    ((uint64_t)armFhmStsData[3] << BYTE3_SHIFT) |
					    ((uint64_t)armFhmStsData[4] << BYTE4_SHIFT) |
					    ((uint64_t)armFhmStsData[5] << BYTE5_SHIFT) |
					    ((uint64_t)armFhmStsData[6] << BYTE6_SHIFT) |
					    ((uint64_t)armFhmStsData[7] << BYTE7_SHIFT));

	/*Read PLL LO computation error status from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_COMP_PLL_ERROR_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_FOUR_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->compPllError = ((uint32_t)armFhmStsData[0] |
				   ((uint32_t)armFhmStsData[1] << BYTE1_SHIFT) |
				   ((uint32_t)armFhmStsData[2] << BYTE2_SHIFT) |
				   ((uint32_t)armFhmStsData[3] << BYTE3_SHIFT));

	/*Read the last frequency for which PLL LO comp error occurred from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_COMP_PLL_ERROR_FREQ_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_EIGHT_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->compPllErrorFreq_Hz = ((uint64_t)armFhmStsData[0] |
					  ((uint64_t)armFhmStsData[1] << BYTE1_SHIFT) |
					  ((uint64_t)armFhmStsData[2] << BYTE2_SHIFT) |
					  ((uint64_t)armFhmStsData[3] << BYTE3_SHIFT) |
					  ((uint64_t)armFhmStsData[4] << BYTE4_SHIFT) |
					  ((uint64_t)armFhmStsData[5] << BYTE5_SHIFT) |
					  ((uint64_t)armFhmStsData[6] << BYTE6_SHIFT) |
					  ((uint64_t)armFhmStsData[7] << BYTE7_SHIFT));

	/*Read RF PLL Lock failed status from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_RF_PLL_LOCK_FAILED_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_FOUR_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->rfPllLockFailed = ((uint32_t)armFhmStsData[0] |
				      ((uint32_t)armFhmStsData[1] << BYTE1_SHIFT) |
				      ((uint32_t)armFhmStsData[2] << BYTE2_SHIFT) |
				      ((uint32_t)armFhmStsData[3] << BYTE3_SHIFT));

	/*Read the last frequency for which RF PLL lock failed from mailbox*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			(TALISE_ADDR_ARM_START_DATA_ADDR + FHM_STS_RF_PLL_LOCK_FAILED_FREQ_OFFSET),
			&armFhmStsData[0], ARM_FHM_STS_EIGHT_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);
	fhmStatus->rfPllLockFailedFreq_Hz = ((uint64_t)armFhmStsData[0] |
					     ((uint64_t)armFhmStsData[1] << BYTE1_SHIFT) |
					     ((uint64_t)armFhmStsData[2] << BYTE2_SHIFT) |
					     ((uint64_t)armFhmStsData[3] << BYTE3_SHIFT) |
					     ((uint64_t)armFhmStsData[4] << BYTE4_SHIFT) |
					     ((uint64_t)armFhmStsData[5] << BYTE5_SHIFT) |
					     ((uint64_t)armFhmStsData[6] << BYTE6_SHIFT) |
					     ((uint64_t)armFhmStsData[7] << BYTE7_SHIFT));

	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setExtLoOutCfg(taliseDevice_t *device,
			       uint8_t enableExtLoOutput, taliseExtLoDiv_t extLoOutDivide)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t extLoOutPowerDown = 0;

	static const uint8_t BIT6_BITMASK = 0x40;
	static const uint8_t BIT6_TO_5_BITMASK = 0x60;
	static const uint8_t ENABLE_EXTLO_OUT_BUFFER = 0x00;
	static const uint8_t DISABLE_EXTLO_IN_BUFFER = 0x20;
	static const uint8_t DISABLE_EXTLO_OUT_BUFFER = 0x40;
	static const uint8_t AC_TERMINATION = 0x40;
#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setExtLoOutCfg()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Verify LO input mode is not enabled */
	if ((enableExtLoOutput > 0) &&
	    (device->devStateInfo.clocks.rfPllUseExternalLo > 0)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_FAIL,
						  TAL_ERR_SETEXTLOOUT_LO_IN_ENABLED, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((extLoOutDivide != TAL_EXTLO_RFPLLVCO_DIV2) &&
	    (extLoOutDivide != TAL_EXTLO_RFPLLVCO_DIV4) &&
	    (extLoOutDivide != TAL_EXTLO_RFPLLVCO_DIV8) &&
	    (extLoOutDivide != TAL_EXTLO_RFPLLVCO_DIV16) &&
	    (extLoOutDivide != TAL_EXTLO_RFPLLVCO_DIV32) &&
	    (extLoOutDivide != TAL_EXTLO_RFPLLVCO_DIV64)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SETEXTLOOUT_INV_DIV_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Enable Ext LO output buffer */
	if (enableExtLoOutput > 0) {
		/* Power up Ext LO output buffer by clearing bit [6] */
		/* Ext LO input buffer must be disabled when enabling Ext LO out buffer  (bit[5] = 1) */
		extLoOutPowerDown = ENABLE_EXTLO_OUT_BUFFER | DISABLE_EXTLO_IN_BUFFER;

		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_RF_SYNTHESIZER_POWER_DOWN, extLoOutPowerDown, BIT6_TO_5_BITMASK, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	} else { /* Disable Ext LO Out buffer */
		extLoOutPowerDown = DISABLE_EXTLO_OUT_BUFFER;

		/* Disable Ext LO output buffer [6] = 1, do not touch Ext LO Input buffer */
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_RF_SYNTHESIZER_POWER_DOWN, extLoOutPowerDown, BIT6_BITMASK, 0);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Enable AC termination at External LO out driver outputs (Set bit[6], all other bits are unused) */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_EXTLOGEN_CONTROL_1,
				   AC_TERMINATION);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Ext LO output divider */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_EXTLO_DIVMODE,
				   (uint8_t)(extLoOutDivide));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getExtLoOutCfg(taliseDevice_t *device,
			       uint8_t *enableExtLoOutput, taliseExtLoDiv_t *extLoOutDivide)
{
	static const uint8_t EXT_DIV_BITMASK = 0x0F;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t extLoOutPowerDown = 0;
	uint8_t extDivReadback = 0;
	uint8_t BIT6_BITMASK = 0x40;
	uint8_t BIT6_SHIFT = 6;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getExtLoOutCfg()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((enableExtLoOutput == NULL) ||
	    (extLoOutDivide == NULL)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETEXTLOOUT_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Read Enable/Disable for external LO output */
	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_RF_SYNTHESIZER_POWER_DOWN, &extLoOutPowerDown, BIT6_BITMASK,
				   BIT6_SHIFT);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*enableExtLoOutput = (extLoOutPowerDown == 0) ? 1 :
			     0; /* Invert power down bit to return enable parameter */

	/* Get Ext LO output divider */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_EXTLO_DIVMODE,
				  &extDivReadback);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*extLoOutDivide = (taliseExtLoDiv_t)(extDivReadback & EXT_DIV_BITMASK);

	return (uint32_t)retVal;
}

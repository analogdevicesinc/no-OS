// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_gpio.c
 * \brief Talise GPIO functions
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_cals.h"
#include "talise_gpio.h"
#include "talise_jesd204.h"
#include "talise_reg_addr_macros.h"
#include "talise_hal.h"
#include "talise_error.h"
#include "talise_arm.h"
#include "talise_user.h" /* Defines TALISE_VERBOSE */

uint32_t TALISE_setGpioOe(taliseDevice_t *device, uint32_t gpioOutEn,
			  uint32_t gpioUsedMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint32_t GPIO_OE_MASK = 0x7FFFF;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpioOe()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing gpioOutEn range check */
	if (gpioOutEn > GPIO_OE_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPIO_OE_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Talise SPI regs to set GPIO OE direction */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_GPIO_DIRECTION_CONTROL_7DOWNTO0,
				    (gpioOutEn & 0xFF), (gpioUsedMask & 0xFF), 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_GPIO_DIRECTION_CONTROL_15DOWNTO8,
				    ((gpioOutEn >> 8) & 0xFF), ((gpioUsedMask >> 8) & 0xFF), 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_GPIO_DIRECTION_CONTROL_18DOWNTO16,
				    ((gpioOutEn >> 16) & 0x07), ((gpioUsedMask >> 16) & 0x07), 0);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpioOe(taliseDevice_t *device, uint32_t *gpioOutEn)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[3] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpioOe()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* null pointer check */
	if (gpioOutEn == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO_OE_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Reading GPIO output enable registers */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_DIRECTION_CONTROL_7DOWNTO0, &readBytes[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_DIRECTION_CONTROL_15DOWNTO8, &readBytes[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_DIRECTION_CONTROL_18DOWNTO16, &readBytes[2]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* reconstructing byte reads into gpioOutEn word */
	*gpioOutEn = ((uint32_t)(readBytes[2] & 0x07) << 16) | ((uint32_t)(
				readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_setGpioSourceCtrl(taliseDevice_t *device, uint32_t gpioSrcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint32_t GPIO_SRC_MASK = 0x000FFFFF;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpioSourceCtrl()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing gpioSrcCtrl range check */
	if (gpioSrcCtrl > GPIO_SRC_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPIO_SRC_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* writing GPIO configuration registers */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_LOWER_BYTE_SOURCE_CONTROL, (gpioSrcCtrl & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_UPPER_BYTE_SOURCE_CONTROL, ((gpioSrcCtrl >> 8) & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_EXTRA_BITS_SOURCE_CONTROL, ((gpioSrcCtrl >> 16) & 0x0F));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpioSourceCtrl(taliseDevice_t *device, uint32_t *gpioSrcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[3] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpioSourceCtrl()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (gpioSrcCtrl == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO_SRC_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Reading GPIO source control registers */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_LOWER_BYTE_SOURCE_CONTROL, &readBytes[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_UPPER_BYTE_SOURCE_CONTROL, &readBytes[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_EXTRA_BITS_SOURCE_CONTROL, &readBytes[2]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* reconstructing byte reads into gpioSrcCtrl word */
	*gpioSrcCtrl = ((uint32_t)(readBytes[2] & 0x0F) << 16) | ((uint32_t)(
				readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_setGpioPinLevel(taliseDevice_t *device, uint32_t gpioPinLevel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint32_t GPIO_PIN_MASK = 0x0007FFFF;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpioPinLevel()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing range check on gpioPinLevel */
	if (gpioPinLevel > GPIO_PIN_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPIO_LEVEL_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* writing GPIO configuration registers */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_SPI_SOURCE_7DOWNTO0, (gpioPinLevel & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_SPI_SOURCE_15DOWNTO8, ((gpioPinLevel >> 8) & 0xFF));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_SPI_SOURCE_18DOWNTO16, ((gpioPinLevel >> 16) & 0x07));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpioPinLevel(taliseDevice_t *device, uint32_t *gpioPinLevel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[3] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpioPinLevel()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (gpioPinLevel == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO_LEVEL_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* reading the registers into three-byte array */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_SPI_READ_7DOWNTO0, &readBytes[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_SPI_READ_15DOWNTO8, &readBytes[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_SPI_READ_18DOWNTO16, &readBytes[2]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* reconstructing byte reads into gpioPinLevel word */
	*gpioPinLevel = ((uint32_t)(readBytes[2] & 0x07) << 16) | ((uint32_t)(
				readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpioSetLevel(taliseDevice_t *device,
				uint32_t *gpioPinSetLevel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[3] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpioSetLevel()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (gpioPinSetLevel == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO_SETLEVEL_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* reading the registers into three-byte array */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_SPI_SOURCE_7DOWNTO0, &readBytes[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_SPI_SOURCE_15DOWNTO8, &readBytes[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_SPI_SOURCE_18DOWNTO16, &readBytes[2]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* reconstructing byte reads into gpioPinSetLevel word */
	*gpioPinSetLevel = ((uint32_t)(readBytes[2] & 0x07) << 16) | ((uint32_t)(
				   readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_setGpioMonitorOut(taliseDevice_t *device, uint8_t monitorIndex,
				  uint8_t monitorMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint8_t INDEX_MASK = 0x35;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpioMonitorOut()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing index range check */
	if (monitorIndex > INDEX_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_MONITOR_OUT_INDEX_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Set the GPIO monitor index and the required pin configuration */
	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MONITOR_0,
				   monitorIndex);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_MONITOR_1,
				   monitorMask);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpioMonitorOut(taliseDevice_t *device, uint8_t *monitorIndex,
				  uint8_t *monitorMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpioMonitorOut()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Checking for null parameters */
	if (monitorIndex == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIOMON_INDEX_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (monitorMask == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIOMON_MONITORMASK_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Get the GPIO monitor index and the required pin configuration */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MONITOR_0,
				  monitorIndex);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Get GPIO monitor out enable */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_MONITOR_1,
				  monitorMask);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setGpIntMask(taliseDevice_t *device, uint16_t gpIntMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpIntMask()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid mask setting */
	if (gpIntMask & ~(TAL_GPMASK_MSB | TAL_GPMASK_LSB)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_GP_INT_MASK_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
					   (uint8_t)(gpIntMask & TAL_GPMASK_LSB));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_0,
					   (uint8_t)((gpIntMask & TAL_GPMASK_MSB) >> 8));
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpIntMask(taliseDevice_t *device, uint16_t *gpIntMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t gpIntLsb = 0;
	uint8_t gpIntMsb = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpIntMask()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for null pointer */
	if (gpIntMask == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_INV_GP_INT_MASK_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* Read the current GpInt Mask */
		halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
					  &gpIntLsb);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_0,
					  &gpIntMsb);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		*gpIntMask = ((((uint16_t)gpIntMsb) << 8) & TAL_GPMASK_MSB) | ((
					uint16_t)gpIntLsb & TAL_GPMASK_LSB);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpIntStatus(taliseDevice_t *device, uint16_t *gpIntStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t lsbByte = 0;
	uint8_t msbByte = 0;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpIntStatus()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* checking for null pointer */
	if (gpIntStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GP_INT_STATUS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_READ_1,
					   &lsbByte, (uint8_t)TAL_GPMASK_LSB, 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_READ_0,
					   &msbByte, (uint8_t)(TAL_GPMASK_MSB >> 8), 0);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		*gpIntStatus = (((uint16_t)msbByte) << 8) | ((uint16_t)lsbByte);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getTemperature(taliseDevice_t *device, int16_t *temperatureDegC)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint8_t armExtData[1] = {TALISE_ARM_OBJECTID_TEMP_SENSOR};
	uint8_t cmdStatusByte = 0;
	uint8_t armReadBack[2] = {0};

	static const uint32_t CODECHECK_PARAM_GETTEMPERATURE_OPCODE = 2;
	static const int16_t CODE_TO_DEGREE_CELSIUS =
		359; /* Nominal 25C value  = code 384, Scale by 384-25 to normalize */

#if TALISE_VERBOSE
	adiHalErr_t halError = ADIHAL_OK;
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getTemperature()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (temperatureDegC == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_GETTEMPERATURE_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* sending GET command to ARM */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &armExtData[0], 1);
	IF_ERR_RETURN_U32(retVal);

	/* waiting for command completion */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETTEMPERATURE_TIMEOUT_US,
			GETTEMPERATURE_INTERVAL_US);
	if(device->devStateInfo.swTest == CODECHECK_PARAM_GETTEMPERATURE_OPCODE) {
		cmdStatusByte = 2;
	}

	if((cmdStatusByte >> 1) > 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_TEMP_SENSOR,
								  cmdStatusByte), retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read the device temperature field value (16 bits)*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &armReadBack[0], sizeof(armReadBack), 0);
	IF_ERR_RETURN_U32(retVal);

	*temperatureDegC = ((int16_t)((uint16_t)armReadBack[0] | ((
					      uint16_t)armReadBack[1] << 8)) - CODE_TO_DEGREE_CELSIUS);

	return (uint32_t)retVal;
}

uint32_t TALISE_setupAuxDacs(taliseDevice_t *device, taliseAuxDac_t *auxDac)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t i = 0;
	uint8_t auxDacConfig = 0x00;

	static const uint16_t DAC_10_BIT_MAX_VALUE = 1023;
	static const uint16_t DAC_12_BIT_MAX_VALUE = 4095;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setupAuxDacs()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (auxDac == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_SETUPAUXDAC_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Power down all auxiliary DACs */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PDAUXDAC_MANUAL_IN_MSB, 0x0F);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PDAUXDAC_MANUAL_IN_LSB, 0xFF);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* 10-bit DAC configuration */
	for (i = 0; i < 10; i++) {
		/* If auxDac enabled, setup auxiliary 10-bit DAC configurations */
		if ((auxDac->auxDacEnables >> i) & 0x01) {
			if (auxDac->auxDacValues[i] > DAC_10_BIT_MAX_VALUE) {
				/* clip value to max */
				auxDac->auxDacValues[i] = DAC_10_BIT_MAX_VALUE;

				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
								  TALISE_ERR_SETUPAUXDAC_INV_10BIT_AUXDACCODE, retVal, TALACT_ERR_CHECK_PARAM);
			}

			/* initialize configuration word for each DAC and begin building auxDacConfig */
			auxDacConfig = 0x00;

			/* Set the DAC resolution (10, 11, or 12 bits) */
			if (auxDac->auxDacResolution[i] == TAL_AUXDACRES_10BIT) {
				auxDacConfig = (TAL_AUXDACVREF_1P5V << 4) |
					       0x08;  /* VREF set to 1.5V setting in [5:4]=01 */
			} else if (auxDac->auxDacResolution[i] == TAL_AUXDACRES_11BIT) {
				/* 11bit mode when auxDacConfig[6] = 0  */

				/* setting the DAC reference voltage */
				auxDacConfig |= ((auxDac->auxDacVref[i] & 0x03) << 4);
			} else if (auxDac->auxDacResolution[i] == TAL_AUXDACRES_12BIT) {
				auxDacConfig = 0x40; /* Enable 12bit mode */

				/* setting the DAC reference voltage */
				auxDacConfig |= ((auxDac->auxDacVref[i] & 0x03) << 4);
			} else {
				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
								  TALISE_ERR_SETUPAUXDAC_INV_RESOLUTION, retVal, TALACT_ERR_CHECK_PARAM);
			}

			/* Write the DAC configuration word and MSB - LSB codes */
			halError = talSpiWriteByte(device->devHalInfo,
						   (TALISE_ADDR_AUX_DAC_0_WORD_MSB + (i * 2)),
						   (auxDacConfig | ((auxDac->auxDacValues[i] >> 8) & 0x03)));
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			halError = talSpiWriteByte(device->devHalInfo,
						   (TALISE_ADDR_AUX_DAC_0_WORD_LSB + (i * 2)), (auxDac->auxDacValues[i] & 0xFF));
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* 12-bit DAC configuration */
	for (i = 10; i < 12; i++) {
		/* If auxDac enabled, setup auxiliary 12-bit DAC configurations */
		if ((auxDac->auxDacEnables >> i) & 0x01) {
			if (auxDac->auxDacValues[i] > DAC_12_BIT_MAX_VALUE) {
				/* clip value to max */
				auxDac->auxDacValues[i] = DAC_12_BIT_MAX_VALUE;

				return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
								  TALISE_ERR_SETUPAUXDAC_INV_12BIT_AUXDACCODE, retVal, TALACT_ERR_CHECK_PARAM);
			}

			/* Write the 12-bit DAC MSB and LSB codes */
			halError = talSpiWriteByte(device->devHalInfo,
						   (TALISE_ADDR_AUX_DAC_12B_0_MSB + ((i - 10) * 2)),
						   ((auxDac->auxDacValues[i] >> 4) & 0xFF));
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			halError = talSpiWriteByte(device->devHalInfo,
						   (TALISE_ADDR_AUX_DAC_12B_0_LSB + ((i - 10) * 2)),
						   (auxDac->auxDacValues[i] & 0x0F));
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* Write enable bit to latch DAC codes into all DACs */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AUX_DAC_LATCH_CONTROL, 0x01);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Power up the selected auxiliary DACs */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PDAUXDAC_MANUAL_IN_MSB,
				   (uint8_t)(~((auxDac->auxDacEnables >> 8) & 0x0F)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_PDAUXDAC_MANUAL_IN_LSB, (uint8_t)(~(auxDac->auxDacEnables & 0xFF)));
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_writeAuxDac(taliseDevice_t *device, uint8_t auxDacIndex,
			    uint16_t auxDacCode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint16_t auxDacAddr = 0;

	static const uint16_t DAC_10_BIT_MAX_VALUE = 1023;
	static const uint16_t DAC_12_BIT_MAX_VALUE = 4095;
	static const uint8_t DAC_MAX_INDEX = 11;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_writeAuxDac()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (auxDacIndex > DAC_MAX_INDEX) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_WRITEAUXDAC_INV_AUXDACINDEX, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((auxDacIndex < 10) && (auxDacCode > DAC_10_BIT_MAX_VALUE)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_WRITEAUXDAC_INV_10BIT_AUXDACCODE, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((auxDacIndex > 9) && (auxDacCode > DAC_12_BIT_MAX_VALUE)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_WRITEAUXDAC_INV_12BIT_AUXDACCODE, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		/* DAC address assignment */
		auxDacAddr = TALISE_ADDR_AUX_DAC_0_WORD_MSB + (auxDacIndex * 2);

		/* Write DAC configuration and code for 10-bit DACs */
		if (auxDacIndex < 10) {
			halError = talSpiWriteField(device->devHalInfo, auxDacAddr,
						    (uint8_t)(auxDacCode >> 8), 0x03, 0);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			halError = talSpiWriteByte(device->devHalInfo, (auxDacAddr + 1),
						   (auxDacCode & 0xFF));
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}

		/* Write DAC configuration and code for 12-bit DACs */
		if (auxDacIndex >= 10) {
			halError = talSpiWriteByte(device->devHalInfo, auxDacAddr,
						   (auxDacCode >> 4) & 0xFF);
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);

			halError = talSpiWriteByte(device->devHalInfo, (auxDacAddr + 1),
						   (auxDacCode & 0x0F));
			retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* Write enable bit to latch the DAC code into the DAC */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_AUX_DAC_LATCH_CONTROL, 0x01);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setSpi2Enable(taliseDevice_t *device, uint8_t spi2Enable,
			      taliseSpi2TxAttenGpioSel_t spi2TxAttenGpioSel)
{
	uint8_t currentSpi2En = 0;
	taliseSpi2TxAttenGpioSel_t currentSpi2TxAttenSel = TAL_SPI2_TXATTEN_DISABLE;
	uint32_t usedGpiopins = 0;
	uint32_t gpioUsedMask = 0;
	uint32_t gpioFreeMask = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setSpi2Enable()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* Get current values for spi2En, Tx atten select*/
	retVal = (talRecoveryActions_t)TALISE_getSpi2Enable (device, &currentSpi2En,
			&currentSpi2TxAttenSel);
	IF_ERR_RETURN_U32(retVal);

	/* Free all used GPIO's based on current configuration*/
	switch (currentSpi2TxAttenSel) {
	case TAL_SPI2_TXATTEN_GPIO4:
		gpioFreeMask = (1 << TAL_GPIO_04);
		break;

	case TAL_SPI2_TXATTEN_GPIO8:
		gpioFreeMask = (1 << TAL_GPIO_08);
		break;

	case TAL_SPI2_TXATTEN_GPIO14:
		gpioFreeMask = (1 << TAL_GPIO_14);
		break;

	case TAL_SPI2_TXATTEN_DISABLE:
		gpioFreeMask = 0;
	}

	if (currentSpi2En > 0 ) {
		gpioFreeMask |= 0x0f;
	}

	usedGpiopins = device->devStateInfo.usedGpiopins  & ~gpioFreeMask;

	/* check if GPIO's are available for the new configuration*/
	switch (spi2TxAttenGpioSel) {
	case TAL_SPI2_TXATTEN_GPIO4:
		gpioUsedMask = (1 << TAL_GPIO_04);
		break;

	case TAL_SPI2_TXATTEN_GPIO8:
		gpioUsedMask = (1 << TAL_GPIO_08);
		break;

	case TAL_SPI2_TXATTEN_GPIO14:
		gpioUsedMask = (1 << TAL_GPIO_14);
		break;

	case TAL_SPI2_TXATTEN_DISABLE:
		break;

	default: {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_SPI2_ENABLE_INVALID_TX_ATTEN_SEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	spi2Enable = (spi2Enable > 0) ? 1 : 0;

	if (spi2Enable > 0) {
		gpioUsedMask |= 0xF;
	}

	if ((usedGpiopins & gpioUsedMask ) != 0 ) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_INVALID_PARAM,
						  TAL_ERR_SET_SPI2_ENABLE_GPIO_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	usedGpiopins |= gpioUsedMask;

	/* Update configuration settings - Tx atten GPIO select*/
	if (currentSpi2TxAttenSel != spi2TxAttenGpioSel) {
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_CONFIGURATION_CONTROL_1,
					   (((uint8_t)spi2TxAttenGpioSel & 0x03) << 1));
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Update configuration settings - SPI2 enable*/
	if ( currentSpi2En != spi2Enable) {
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_SPI2_CONFIGURATION2,
					   spi2Enable);
		retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
	}

	/* Set GPIO OE's */
	/* - Set OE for all freed GPIO's to 0 (input) */
	/* - Set OE for all used GPIO's to 0 (input) - SPI2 overrides OE for the MOSI and MISO GPIO's */
	retVal = (talRecoveryActions_t)TALISE_setGpioOe(device, 0,
			(gpioUsedMask | gpioFreeMask));
	IF_ERR_RETURN_U32(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpiopins = usedGpiopins;

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_getSpi2Enable(taliseDevice_t *device, uint8_t *spi2Enable,
			      taliseSpi2TxAttenGpioSel_t *spi2TxAttenGpioSel)
{
	uint8_t readVal = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getSpi2Enable()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Get current GPIO select for TxAtten */
	halError = talSpiReadField(device->devHalInfo,
				   TALISE_ADDR_CONFIGURATION_CONTROL_1, &readVal, 0x06, 1);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*spi2TxAttenGpioSel = (taliseSpi2TxAttenGpioSel_t)readVal;

	/* Get current value of SPI2 */
	halError = talSpiReadField(device->devHalInfo, TALISE_ADDR_SPI2_CONFIGURATION2,
				   spi2Enable, 0x01, 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_setGpio3v3Oe(taliseDevice_t *device, uint16_t gpio3v3OutEn,
			     uint16_t gpio3v3UsedMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint16_t GPIO_OE_MASK = 0xFFF;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpio3v3Oe()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing gpioOutEn range check */
	if (gpio3v3OutEn > GPIO_OE_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPIO3V3_OE_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Talise SPI regs to set GPIO OE direction */
	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_GPIO_3P3V_DIRECTION_CONTROL_0, (gpio3v3OutEn & 0xFF),
				    (gpio3v3UsedMask & 0xFF), 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteField(device->devHalInfo,
				    TALISE_ADDR_GPIO_3P3V_DIRECTION_CONTROL_1, ((gpio3v3OutEn >> 8) & 0x0F),
				    ((gpio3v3UsedMask >> 8) & 0x0F), 0);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpio3v3Oe(taliseDevice_t *device, uint16_t *gpio3v3OutEn)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[2] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpio3v3Oe()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* null pointer check */
	if (gpio3v3OutEn == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO3V3_OE_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Reading GPIO output enable registers */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_DIRECTION_CONTROL_0, &readBytes[0]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_DIRECTION_CONTROL_1, &readBytes[1]);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* reconstructing byte reads into gpioOutEn word */
	*gpio3v3OutEn = ((uint16_t)(readBytes[1] & 0x0F) << 8) | (uint16_t)(
				readBytes[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_setGpio3v3SourceCtrl(taliseDevice_t *device,
				     uint16_t gpio3v3SrcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint16_t GPIO_SRC_MASK = 0x0FFF;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpio3v3SourceCtrl()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing gpio3v3SrcCtrl range check */
	if (gpio3v3SrcCtrl > GPIO_SRC_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPIO3V3_SRC_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Writing 3.3V GPIO configuration registers */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, (gpio3v3SrcCtrl & 0xFF));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_3P3V_UPPER_BYTE_SOURCE_CONTROL,
				   ((gpio3v3SrcCtrl >> 8) & 0x0F));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpio3v3SourceCtrl(taliseDevice_t *device,
				     uint16_t *gpio3v3SrcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[2] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpio3v3SourceCtrl()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (gpio3v3SrcCtrl == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO3V3_SRC_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Reading GPIO source control registers */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, &readBytes[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_UPPER_BYTE_SOURCE_CONTROL, &readBytes[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* reconstructing byte reads into gpioSrcCtrl word */
	*gpio3v3SrcCtrl = ((uint16_t)(readBytes[1]) << 8) | (uint16_t)(readBytes[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_setGpio3v3PinLevel(taliseDevice_t *device,
				   uint16_t gpio3v3PinLevel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	static const uint16_t GPIO_PIN_MASK = 0x0FFF;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_setGpio3v3PinLevel()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* performing range check on gpioPinLevel */
	if (gpio3v3PinLevel > GPIO_PIN_MASK) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPIO3V3_LEVEL_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* writing GPIO configuration registers */
	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_3P3V_SPI_SOURCE_0, (gpio3v3PinLevel & 0xFF));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   TALISE_ADDR_GPIO_3P3V_SPI_SOURCE_1, ((gpio3v3PinLevel >> 8) & 0x0F));
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpio3v3PinLevel(taliseDevice_t *device,
				   uint16_t *gpio3v3PinLevel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[2] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpio3v3PinLevel()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (gpio3v3PinLevel == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO3V3_LEVEL_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* reading the registers into 2-byte array */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GPIO_3P3V_SPI_READ_0,
				  &readBytes[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GPIO_3P3V_SPI_READ_1,
				  &readBytes[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* reconstructing byte reads into gpioPinLevel word */
	*gpio3v3PinLevel = ((uint16_t)(readBytes[1]) << 8) | (uint16_t)(readBytes[0]);

	return (uint32_t)retVal;
}

uint32_t TALISE_getGpio3v3SetLevel(taliseDevice_t *device,
				   uint16_t *gpio3v3PinSetLevel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;
	uint8_t readBytes[2] = {0};

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_getGpio3v3SetLevel()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (gpio3v3PinSetLevel == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GETGPIO3V3_SETLEVEL_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* reading the registers into 2-byte array */
	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_SPI_SOURCE_0, &readBytes[0]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  TALISE_ADDR_GPIO_3P3V_SPI_SOURCE_1, &readBytes[1]);
	retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* reconstructing byte reads into gpioPinSetLevel word */
	*gpio3v3PinSetLevel = ((uint16_t)(readBytes[1]) << 8) | (uint16_t)(
				      readBytes[0]);

	return (uint32_t)retVal;
}

void talSimGpSources(taliseDevice_t *device, uint32_t *gpIntStatus,
		     uint32_t *gpIntDeframerSources, uint16_t *gpIntFramerSources)
{
	/* GP_INT source masks */
	static const uint32_t GPINT_FRMR_ERROR = 0x00000010;
	static const uint32_t GPINT_DEFRMR_ERROR = 0x00000020;
	static const uint32_t GPINT_PA_PROTECT_TX1_ERROR = 0x00000040;
	static const uint32_t GPINT_PA_PROTECT_TX2_ERROR = 0x00000080;
	static const uint32_t GPINT_STREAM_ERROR = 0x000001000;

	/* deframer subsystem GP_INT sources masks */
	static const uint32_t DEFRAMER_A_BD_ERROR = 0x00000001;
	static const uint32_t DEFRAMER_A_NIT_ERROR = 0x00000002;
	static const uint32_t DEFRAMER_A_UEK_ERROR = 0x00000004;
	static const uint32_t DEFRAMER_A_ILD_ERROR = 0x00000008;
	static const uint32_t DEFRAMER_A_ILS_ERROR = 0x00000010;
	static const uint32_t DEFRAMER_A_GCS_ERROR = 0x00000020;
	static const uint32_t DEFRAMER_A_FS_ERROR = 0x00000040;
	static const uint32_t DEFRAMER_A_CSG_ERROR = 0x00000080;
	static const uint32_t DEFRAMER_A_POINTER_ALIGN_ERROR = 0x00000200;
	static const uint32_t DEFRAMER_A_SYSREF_ALIGN_ERROR = 0x00000400;
	static const uint32_t DEFRAMER_B_BD_ERROR = 0x00000800;
	static const uint32_t DEFRAMER_B_NIT_ERROR = 0x00001000;
	static const uint32_t DEFRAMER_B_UEK_ERROR = 0x00002000;
	static const uint32_t DEFRAMER_B_ILD_ERROR = 0x00004000;
	static const uint32_t DEFRAMER_B_ILS_ERROR = 0x00008000;
	static const uint32_t DEFRAMER_B_GCS_ERROR = 0x00010000;
	static const uint32_t DEFRAMER_B_FS_ERROR = 0x00020000;
	static const uint32_t DEFRAMER_B_CSG_ERROR = 0x00040000;
	static const uint32_t DEFRAMER_B_POINTER_ALIGN_ERROR = 0x00100000;
	static const uint32_t DEFRAMER_B_SYSREF_ALIGN_ERROR = 0x00200000;

	/* framer subsystem GP_INT sources masks */
	static const uint16_t FRAMER_A_FIFO_POINTER_OFFSET_ERROR = 0x00000001;
	static const uint16_t FRAMER_A_LMFC_ALIGN_ERROR = 0x00000002;
	static const uint16_t FRAMER_B_FIFO_POINTER_OFFSET_ERROR = 0x00000004;
	static const uint16_t FRAMER_B_LMFC_ALIGN_ERROR = 0x00000008;

	/* swTest Flags */
	static const uint32_t SWTST_NO_PENDING = 3;

	static const uint32_t SWTST_FRAMER_A_FIFOPTR = 6;
	static const uint32_t SWTST_FRAMER_A_LMFCALIGN = 7;
	static const uint32_t SWTST_FRAMER_B_FIFOPTR = 8;
	static const uint32_t SWTST_FRAMER_B_LMFCALIGN = 9;

	static const uint32_t SWTST_DFRAMR_A_BDERR = 10;
	static const uint32_t SWTST_DFRAMR_A_NITERR = 11;
	static const uint32_t SWTST_DFRAMR_A_UEKERR = 12;
	static const uint32_t SWTST_DFRAMR_B_BDERR = 13;
	static const uint32_t SWTST_DFRAMR_B_NITERR = 14;
	static const uint32_t SWTST_DFRAMR_B_UEKERR = 15;
	static const uint32_t SWTST_DFRAMR_A_ILDERR = 16;
	static const uint32_t SWTST_DFRAMR_B_ILDERR = 17;
	static const uint32_t SWTST_DFRAMR_A_ILSERR = 18;
	static const uint32_t SWTST_DFRAMR_B_ILSERR = 19;
	static const uint32_t SWTST_DFRAMR_A_GCSERR = 20;
	static const uint32_t SWTST_DFRAMR_B_GCSERR = 21;
	static const uint32_t SWTST_DFRAMR_A_FSERR = 22;
	static const uint32_t SWTST_DFRAMR_B_FSERR = 23;
	static const uint32_t SWTST_DFRAMR_A_CSGERR = 24;
	static const uint32_t SWTST_DFRAMR_B_CSGERR = 25;
	static const uint32_t SWTST_DFRAMR_A_FIFOERR = 26;
	static const uint32_t SWTST_DFRAMR_B_FIFOERR = 27;
	static const uint32_t SWTST_DFRAMR_A_SYSREFERR = 28;
	static const uint32_t SWTST_DFRAMR_B_SYSREFERR = 29;
	static const uint32_t SWTST_PA1_PROTECT = 30;
	static const uint32_t SWTST_PA2_PROTECT = 31;
	static const uint32_t SWTST_STREAM = 32;

	if (device->devStateInfo.swTest == SWTST_NO_PENDING) {
		*gpIntStatus = 0;
		*gpIntFramerSources = 0;
	} else if (device->devStateInfo.swTest == SWTST_FRAMER_A_FIFOPTR) {
		*gpIntStatus = GPINT_FRMR_ERROR;
		*gpIntFramerSources = FRAMER_A_FIFO_POINTER_OFFSET_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_FRAMER_A_LMFCALIGN) {
		*gpIntStatus = GPINT_FRMR_ERROR;
		*gpIntFramerSources = FRAMER_A_LMFC_ALIGN_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_FRAMER_B_FIFOPTR) {
		*gpIntStatus = GPINT_FRMR_ERROR;
		*gpIntFramerSources = FRAMER_B_FIFO_POINTER_OFFSET_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_FRAMER_B_LMFCALIGN) {
		*gpIntStatus = GPINT_FRMR_ERROR;
		*gpIntFramerSources = FRAMER_B_LMFC_ALIGN_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_BDERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_BD_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_NITERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_NIT_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_UEKERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_UEK_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_BDERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_BD_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_NITERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_NIT_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_UEKERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_UEK_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_ILDERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_ILD_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_ILDERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_ILD_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_ILSERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_ILS_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_ILSERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_ILS_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_GCSERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_GCS_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_GCSERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_GCS_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_FSERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_FS_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_FSERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_FS_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_CSGERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_CSG_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_CSGERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_CSG_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_FIFOERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_POINTER_ALIGN_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_FIFOERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_POINTER_ALIGN_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_A_SYSREFERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_A_SYSREF_ALIGN_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_DFRAMR_B_SYSREFERR) {
		*gpIntStatus = GPINT_DEFRMR_ERROR;
		*gpIntDeframerSources = DEFRAMER_B_SYSREF_ALIGN_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_PA1_PROTECT) {
		*gpIntStatus = GPINT_PA_PROTECT_TX1_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_PA2_PROTECT) {
		*gpIntStatus = GPINT_PA_PROTECT_TX2_ERROR;
	} else if (device->devStateInfo.swTest == SWTST_STREAM) {
		*gpIntStatus = GPINT_STREAM_ERROR;
	}

	return;
}

uint32_t TALISE_gpIntHandler(taliseDevice_t *device, uint32_t *gpIntStatus,
			     taliseGpIntInformation_t *gpIntDiag)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	adiHalErr_t halError = ADIHAL_OK;

	uint32_t addressDefrmrA = 0;
	uint32_t addressDefrmrB = 0;
	uint32_t gpIntDeframerSources = 0;
	uint16_t gpIntFramerSources = 0;
	uint32_t gpIntMask = 0;
	uint8_t lsByte = 0;
	uint8_t msByte = 0;
	uint8_t armExtData[1] = {TALISE_ARM_OBJECTID_GO_GET_GP_INT_SOURCE};
	uint8_t cmdStatusByte = 0;
	uint8_t data[8] = {0};
	uint16_t loopCnt = 0;

	static const uint8_t HIGH = 1;
	static const uint8_t LOW = 0;

	static const uint8_t JESD_SAMPLE_DATA_MASK = 0x03;
	static const uint8_t TXPOWER_RAMP_DOWN_MASK = 0x02;
	static const uint8_t DISTAMP_RAMP_DOWN_MASK = 0x29;
	static const uint8_t UPCONVERT_RAMP_DOWN_MASK = 0x39;
	static const uint8_t ARM_ECC_MASK = 0x01;
	static const uint32_t GPINT_CLKPLL_ERROR = 0x000000008;
	static const uint32_t GPINT_RFPLL_ERROR = 0x000000002;
	static const uint32_t GPINT_AUXPLL_ERROR = 0x000000004;
	static const uint32_t GPINT_WATCHDOG_TIMEOUT = 0x000000100;
	static const uint32_t GPINT_ARM_FORCE_GP_INTERRUPT = 0x000000200;
	static const uint32_t GPINT_ARM_SYSTEM_ERROR = 0x000000400;
	static const uint32_t GPINT_ARM_CAL_ERROR = 0x000000800;
	static const uint32_t GPINT_FRMR_ERROR = 0x00000010;
	static const uint32_t GPINT_DEFRMR_ERROR = 0x00000020;
	static const uint32_t GPINT_PA_PROTECT_TX1_ERROR = 0x00000040;
	static const uint32_t GPINT_PA_PROTECT_TX2_ERROR = 0x00000080;
	static const uint32_t GPINT_STREAM_ERROR = 0x000001000;
	static const uint32_t GPINT_ARM_PARITY_ERROR = 0x000002000;

	/* deframer subsystem GP_INT sources masks */
	static const uint32_t DEFRAMER_A_BD_ERROR = 0x00000001;
	static const uint32_t DEFRAMER_A_NIT_ERROR = 0x00000002;
	static const uint32_t DEFRAMER_A_UEK_ERROR = 0x00000004;
	static const uint32_t DEFRAMER_A_ILD_ERROR = 0x00000008;
	static const uint32_t DEFRAMER_A_ILS_ERROR = 0x00000010;
	static const uint32_t DEFRAMER_A_GCS_ERROR = 0x00000020;
	static const uint32_t DEFRAMER_A_FS_ERROR = 0x00000040;
	static const uint32_t DEFRAMER_A_CSG_ERROR = 0x00000080;
	static const uint32_t DEFRAMER_A_POINTER_ALIGN_ERROR = 0x00000200;
	static const uint32_t DEFRAMER_A_SYSREF_ALIGN_ERROR = 0x00000400;
	static const uint32_t DEFRAMER_B_BD_ERROR = 0x00000800;
	static const uint32_t DEFRAMER_B_NIT_ERROR = 0x00001000;
	static const uint32_t DEFRAMER_B_UEK_ERROR = 0x00002000;
	static const uint32_t DEFRAMER_B_ILD_ERROR = 0x00004000;
	static const uint32_t DEFRAMER_B_ILS_ERROR = 0x00008000;
	static const uint32_t DEFRAMER_B_GCS_ERROR = 0x00010000;
	static const uint32_t DEFRAMER_B_FS_ERROR = 0x00020000;
	static const uint32_t DEFRAMER_B_CSG_ERROR = 0x00040000;
	static const uint32_t DEFRAMER_B_POINTER_ALIGN_ERROR = 0x00100000;
	static const uint32_t DEFRAMER_B_SYSREF_ALIGN_ERROR = 0x00200000;

	/* framer subsystem GP_INT sources masks */
	static const uint16_t FRAMER_A_FIFO_POINTER_OFFSET_ERROR = 0x00000001;
	static const uint16_t FRAMER_A_LMFC_ALIGN_ERROR = 0x00000002;
	static const uint16_t FRAMER_B_FIFO_POINTER_OFFSET_ERROR = 0x00000004;
	static const uint16_t FRAMER_B_LMFC_ALIGN_ERROR = 0x00000008;

	static const uint32_t SWTST_DISABLE = 0;
	static const uint32_t SWTST_DATA_PARITY = 4;
	static const uint32_t SWTST_PROGRAM_PARITY = 5;

	static const uint32_t SWTST_DFRAMR_A_FIFOERR = 26;
	static const uint32_t SWTST_DFRAMR_B_FIFOERR = 27;

#if TALISE_VERBOSE
	halError = talWriteToLog(device->devHalInfo, ADIHAL_LOG_MSG, TAL_ERR_OK,
				 "TALISE_gpIntHandler()\n");
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* null pointer check */
	if (gpIntStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_STATUS_NULL_PARM,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* initialize gpIntDiag to all zeors if requested */
	if (gpIntDiag != NULL) {
		for (loopCnt = 0; loopCnt < 8; loopCnt++) {
			gpIntDiag->data[loopCnt] = 0;
		}
	}

	/* Reading GP_INT source1 */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_READ_0,
				  &msByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	*gpIntStatus = (uint32_t)msByte;

	/* Reading GP_INT source0 */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_READ_1,
				  &lsByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	*gpIntStatus = (*gpIntStatus << 8) | lsByte;

	/* Reading GP_INT mask1 */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_0,
				  &msByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	gpIntMask = msByte;

	/* Reading GP_INT mask0 */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_GP_INTERRUPT_MASK_1,
				  &lsByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	gpIntMask = (gpIntMask << 8) | lsByte;

	/* mask out all blocked interrupts */
	*gpIntStatus = *gpIntStatus & ~gpIntMask;

	if (device->devStateInfo.swTest > SWTST_DISABLE) {
		talSimGpSources(device, gpIntStatus, &gpIntDeframerSources,
				&gpIntFramerSources);
	}

	/* order of GP Int Status */
	/* bit14 - 15 Reserved */
	/* bit13  - TAL_GP_MASK_ARM_PARITY_ERROR */
	/* bit12  - TAL_GP_MASK_STREAM_ERROR */
	/* bit11  - TAL_GP_MASK_ARM_CALIBRATION_ERROR */
	/* bit10  - TAL_GP_MASK_ARM_SYSTEM_ERROR */
	/* bit9   - TAL_GP_MASK_ARM_FORCE_INTERRUPT */
	/* bit8   - TAL_GP_MASK_WATCHDOG_TIMEOUT */
	/* bit7   - TAL_GP_MASK_PA_PROTECTION_TX2_ERROR */
	/* bit6   - TAL_GP_MASK_PA_PROTECTION_TX1_ERROR */
	/* bit5   - TAL_GP_MASK_JESD_DEFRMER_IRQ */
	/* bit4   - TAL_GP_MASK_JESD_FRAMER_IRQ */
	/* bit3   - TAL_GP_MASK_CLK_SYNTH_LOCK */
	/* bit2   - TAL_GP_MASK_AUX_SYNTH_LOCK */
	/* bit1   - TAL_GP_MASK_RF_SYNTH_LOCK */
	/* bit0   - Reserved */

	/* is the source the CLK SYNTH LOCK */
	if (((*gpIntStatus & GPINT_CLKPLL_ERROR) > 0) &&
	    (device->devStateInfo.swTest == SWTST_DISABLE)) {
		/* set gpIntDiag CLK PLL bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[7] = 0x04;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_CLKPLL_UNLOCKED, retVal, TALACT_ERR_RESET_FULL);
	}

	/* is the source the RF SYNTH LOCK */
	if (((*gpIntStatus & GPINT_RFPLL_ERROR) > 0) &&
	    (device->devStateInfo.swTest == SWTST_DISABLE)) {
		/* set gpIntDiag RF PLL bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[7] = 0x10;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_RFPLL_UNLOCKED, retVal, TALACT_ERR_RESET_FULL);
	}

	/* is the source the AUX SYNTH LOCK */
	if (((*gpIntStatus & GPINT_AUXPLL_ERROR) > 0) &&
	    (device->devStateInfo.swTest == SWTST_DISABLE)) {
		/* set gpIntDiag AUX PLL bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[7] = 0x08;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_AUXPLL_UNLOCKED, retVal, TALACT_ERR_RESET_FULL);
	}

	/* check for arm crash errors before issuing ARM command */
	/* is the source the WatchDog Timeout */
	if (((*gpIntStatus & GPINT_WATCHDOG_TIMEOUT) > 0) &&
	    (device->devStateInfo.swTest == SWTST_DISABLE)) {
		/* set gpIntDiag Arm WatchDog bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[7] = 0x20;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_ARM_WATCHDOG_TIMEOUT, retVal, TALACT_ERR_RESET_ARM);
	}

	/* is the source the ARM FORCE GP_INT*/
	if (((*gpIntStatus & GPINT_ARM_FORCE_GP_INTERRUPT) > 0) &&
	    (device->devStateInfo.swTest == SWTST_DISABLE)) {
		/* set gpIntDiag Arm Force GP_INT bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[7] = 0x40;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_ARM_FORCE_GPINT, retVal, TALACT_ERR_RESET_ARM);
	}

	/* is the source the ARM SYSTEM ERROR */
	if (((*gpIntStatus & GPINT_ARM_SYSTEM_ERROR) > 0) &&
	    (device->devStateInfo.swTest == SWTST_DISABLE)) {
		/* set gpIntDiag Arm System Error bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[7] = 0x80;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_ARM_SYSTEM_ERROR, retVal, TALACT_ERR_RESET_ARM);
	}

	/* is the source the ARM CALIBRATION ERROR */
	if (((*gpIntStatus & GPINT_ARM_CAL_ERROR) > 0) &&
	    (device->devStateInfo.swTest == SWTST_DISABLE)) {
		/* set gpIntDiag RF PLL bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[7] = 0x02;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_ARM_CALIBRATION_ERROR, retVal, TALACT_ERR_RERUN_INIT_CALS);
	}

	/* is the source the ARM DATA PARITY ERROR */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_ARM_ECC_DATA_READBACK,
				  &lsByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	if (((lsByte & ARM_ECC_MASK) > 0) ||
	    (device->devStateInfo.swTest == SWTST_DATA_PARITY)) {
		*gpIntStatus |= GPINT_ARM_PARITY_ERROR;
		/* set gpIntDiag Arm Data Parity Error bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[8] = 0x01;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_ARM_DATA_PARITY_ERROR, retVal, TALACT_ERR_RESET_ARM);
	}

	/* is the source the ARM PROGRAM PARITY ERROR */
	halError = talSpiReadByte(device->devHalInfo, TALISE_ADDR_ARM_ECC_PROG_READBACK,
				  &lsByte);
	retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	if (((lsByte & ARM_ECC_MASK) > 0) ||
	    (device->devStateInfo.swTest == SWTST_PROGRAM_PARITY)) {
		*gpIntStatus |= GPINT_ARM_PARITY_ERROR;

		/* set gpIntDiag Arm Program Parity Error bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[8] = 0x02;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_ARM_PROG_PARITY_ERROR, retVal, TALACT_ERR_RESET_ARM);
	}

	/* The GP_INT is not from an ARM source and clocks are ok */
	/* assume the ARM is running ok */
	/* gather all other GP_INT sources */
	/* sending GET command to ARM */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &armExtData[0], 1);
	IF_ERR_RETURN_U32(retVal);

	/* waiting for command completion */
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, GETGPINTSRC_TIMEOUT_US,
			GETGPINTSRC_INTERVAL_US);
	if((cmdStatusByte >> 1) > 0) {
		/* set gpIntDiag Arm Command Wait TimeOut bit if requested */
		if (gpIntDiag != NULL) {
			gpIntDiag->data[8] = 0x04;
		}

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						  ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, TALISE_ARM_OBJECTID_GO_GET_GP_INT_SOURCE,
								  cmdStatusByte), retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/* read out the accumulated sources */
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &data[0], 8, 0);
	IF_ERR_RETURN_U32(retVal);

	if (device->devStateInfo.swTest > 0) {
		if (gpIntDiag == NULL) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_GPINTDIAG_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		} else {
			for (loopCnt = 0; loopCnt < sizeof(data); loopCnt++) {
				data[loopCnt] = gpIntDiag->data[loopCnt];
			}
		}

	} else {
		/* save all source bits if diagnostic requested */
		if (gpIntDiag != NULL) {
			for (loopCnt = 0; loopCnt < 8; loopCnt++) {
				gpIntDiag->data[loopCnt] = data[loopCnt];
			}
		}
	}

	/* assemble sources by subsystems */
	if (device->devStateInfo.swTest == 0) {
		gpIntDeframerSources = (uint32_t)(data[3] & 0xE0) >> 5 |
				       (uint32_t)data[4] << 3 | (uint32_t) data[5] << 11 | (uint32_t) (
					       data[6] & 0x03) << 19;
		gpIntFramerSources = (uint16_t)(data[6] & 0x78) >> 3;
	}

	/* is the source the framer */
	if ((*gpIntStatus & GPINT_FRMR_ERROR) > 0) {
		/* framer handler here */
		if (((gpIntFramerSources & FRAMER_A_FIFO_POINTER_OFFSET_ERROR) > 0) ||
		    ((gpIntFramerSources & FRAMER_A_LMFC_ALIGN_ERROR) > 0)) {
			/* Clear Framer A Fifo error */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_JESD_FRAMER_ASYNC_PTR_DBG_0, 0, 0x80, 7);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN(retVal);

			if (gpIntDiag != NULL) {
				gpIntDiag->framer = TAL_FRAMER_A;
			}

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_FRAMERA, retVal, TALACT_ERR_RESET_JESD204FRAMERA);
		} else if (((gpIntFramerSources & FRAMER_B_FIFO_POINTER_OFFSET_ERROR) > 0) ||
			   ((gpIntFramerSources & FRAMER_B_LMFC_ALIGN_ERROR) > 0)) {
			/* Clear Framer A Fifo error */
			halError = talSpiWriteField(device->devHalInfo,
						    TALISE_ADDR_JESD_FRAMER_ASYNC_PTR_DBG_1, 0, 0x80, 7);
			retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
						  TALACT_ERR_RESET_SPI);
			IF_ERR_RETURN(retVal);

			if (gpIntDiag != NULL) {
				gpIntDiag->framer = TAL_FRAMER_B;
			}

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_FRAMERB, retVal, TALACT_ERR_RESET_JESD204FRAMERB);
		}
	}

	/* is the source the deframer */
	if ((*gpIntStatus & GPINT_DEFRMR_ERROR) > 0) {
		/* Deframer A */
		if (((gpIntDeframerSources & DEFRAMER_A_BD_ERROR) > 0) ||
		    ((gpIntDeframerSources & DEFRAMER_A_NIT_ERROR) > 0) ||
		    ((gpIntDeframerSources & DEFRAMER_A_UEK_ERROR) > 0)) {
			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_A;
				retVal = talFindDfrmrLaneCntErr(device, TAL_DEFRAMER_A,
								&gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_BBIC_LOG_ERROR);
		} /* end DEFRAMER_A */

		/* Deframer B */
		else if (((gpIntDeframerSources & DEFRAMER_B_BD_ERROR) > 0) ||
			 ((gpIntDeframerSources & DEFRAMER_B_NIT_ERROR) > 0) ||
			 ((gpIntDeframerSources & DEFRAMER_B_UEK_ERROR) > 0)) {
			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_B;
				retVal = talFindDfrmrLaneCntErr(device, TAL_DEFRAMER_B,
								&gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_B);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_BBIC_LOG_ERROR);
		} /* end DEFRAMER_B */

		/* Deframer A */
		else if ((gpIntDeframerSources & DEFRAMER_A_ILD_ERROR) > 0) {
			addressDefrmrA = TALISE_ADDR_JESD_DEFRAMER_IP_OBS22_0;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_A;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrA, LOW,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_RESET_JESD204DEFRAMERA);
		} /* end DEFRAMER_A */

		/* Deframer B */
		else if ((gpIntDeframerSources & DEFRAMER_B_ILD_ERROR) > 0) {
			addressDefrmrB = TALISE_ADDR_JESD_DEFRAMER_IP_OBS22_1;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_B;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrB, LOW,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_B);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_RESET_JESD204DEFRAMERB);
		} /* end DEFRAMER_B */

		/* Deframer A */
		else if ((gpIntDeframerSources & DEFRAMER_A_ILS_ERROR) > 0) {
			addressDefrmrA = TALISE_ADDR_JESD_DEFRAMER_IP_OBS22_0;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_A;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrA, HIGH,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_RESET_JESD204DEFRAMERA);
		} /* end DEFRAMER_A */

		/* Deframer B */
		else if ((gpIntDeframerSources & DEFRAMER_B_ILS_ERROR) > 0) {
			addressDefrmrB = TALISE_ADDR_JESD_DEFRAMER_IP_OBS22_1;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_B;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrB, HIGH,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_B);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_RESET_JESD204DEFRAMERB);
		} /* end DEFRAMER_B */

		/* GCS Error */
		else if ((gpIntDeframerSources & DEFRAMER_A_GCS_ERROR) > 0) {
			addressDefrmrA = TALISE_ADDR_JESD_DEFRAMER_IP_OBS21_0;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_A;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrA, LOW,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_RESET_JESD204DEFRAMERA);
		} /* end DEFRAMER_A_GCS_ERROR */

		/* GCS Error */
		else if ((gpIntDeframerSources & DEFRAMER_B_GCS_ERROR) > 0) {
			addressDefrmrB = TALISE_ADDR_JESD_DEFRAMER_IP_OBS21_1;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_B;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrB, LOW,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_B);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_RESET_JESD204DEFRAMERB);
		} /* end DEFRAMERB_GCS_ERROR */

		/* FS Error */
		else if ((gpIntDeframerSources & DEFRAMER_A_FS_ERROR) > 0) {
			addressDefrmrA = TALISE_ADDR_JESD_DEFRAMER_IP_OBS21_0;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_A;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrA, HIGH,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_RESET_JESD204DEFRAMERA);
		} /* end DEFRAMER_A_FS_ERROR */

		/* FS Error */
		else if ((gpIntDeframerSources & DEFRAMER_B_FS_ERROR) > 0) {
			addressDefrmrB = TALISE_ADDR_JESD_DEFRAMER_IP_OBS21_1;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_B;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrB, HIGH,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_B);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_RESET_JESD204DEFRAMERB);
		} /* end DEFRAMER_B_FS_ERROR */

		/* CSG Error */
		else if ((gpIntDeframerSources & DEFRAMER_A_CSG_ERROR) > 0) {
			addressDefrmrA = TALISE_ADDR_JESD_DEFRAMER_IP_OBS20_0;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_A;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrA, HIGH,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_RESET_JESD204DEFRAMERA);
		} /* end DEFRAMER_A_CSG_ERROR */

		/* CSG Error */
		else if ((gpIntDeframerSources & DEFRAMER_B_CSG_ERROR) > 0) {
			addressDefrmrB = TALISE_ADDR_JESD_DEFRAMER_IP_OBS20_1;

			if (gpIntDiag != NULL) {
				gpIntDiag->deframer = TAL_DEFRAMER_B;
				retVal = talFindDfrmrLaneErr(device, addressDefrmrB, HIGH,
							     &gpIntDiag->deframerInputsMask);
				IF_ERR_RETURN_U32(retVal);
			}

			retVal = (talRecoveryActions_t)TALISE_clearDfrmIrq(device, TAL_DEFRAMER_B);
			IF_ERR_RETURN_U32(retVal);

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_RESET_JESD204DEFRAMERB);
		} /* end DEFRAMER_A_CSG_ERROR */

		/* FIFO PTR Error */
		else if ((gpIntDeframerSources & DEFRAMER_A_POINTER_ALIGN_ERROR) > 0) {
			if (gpIntDiag != NULL) {
				gpIntDiag->deframerInputsMask = 0;
			}

			for (loopCnt = 0; loopCnt < 4; loopCnt++) {
				/* Check Lane */
				halError = talSpiReadByte(device->devHalInfo,
							  (TALISE_ADDR_JESD_DEFRAMER_IP_OBS42_1 + loopCnt), &lsByte);
				retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
							  TALACT_ERR_RESET_SPI);
				IF_ERR_RETURN(retVal);

				if (((lsByte & 0x01) > 0)
				    || (device->devStateInfo.swTest == SWTST_DFRAMR_A_FIFOERR)) {
					if (gpIntDiag != NULL) {
						gpIntDiag->deframerInputsMask |= (0x01 << loopCnt);
						gpIntDiag->deframer = TAL_DEFRAMER_A;
					}

					/* clear the pointer error interrupt */
					halError = talSpiWriteByte(device->devHalInfo,
								   (TALISE_ADDR_JESD_DEFRAMER_IP_OBS42_1 + loopCnt), 0x00);
					retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
								  TALACT_ERR_RESET_SPI);
					IF_ERR_RETURN_U32(retVal);
				}
			}

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_RESET_JESD204DEFRAMERA);
		} /* end DEFRAMER_A_FIFO_ERROR */

		/* FIFO PTR Error */
		else if ((gpIntDeframerSources & DEFRAMER_B_POINTER_ALIGN_ERROR) > 0) {
			if (gpIntDiag != NULL) {
				gpIntDiag->deframerInputsMask = 0;
			}

			for (loopCnt = 0; loopCnt < 4; loopCnt++) {
				/* Check Lane */
				halError = talSpiReadByte(device->devHalInfo,
							  (TALISE_ADDR_JESD_DEFRAMER_IP_OBS42_1 + loopCnt), &lsByte);
				retVal = talApiErrHandler(device, TAL_ERRHDL_HAL_SPI, halError, retVal,
							  TALACT_ERR_RESET_SPI);
				IF_ERR_RETURN(retVal);

				if (((lsByte & 0x01) > 0)
				    || (device->devStateInfo.swTest == SWTST_DFRAMR_B_FIFOERR)) {
					if (gpIntDiag != NULL) {
						gpIntDiag->deframerInputsMask |= (0x01 << loopCnt);
						gpIntDiag->deframer = TAL_DEFRAMER_B;
					}

					/* clear the pointer error interrupt */
					halError = talSpiWriteByte(device->devHalInfo,
								   (TALISE_ADDR_JESD_DEFRAMER_IP_OBS42_1 + loopCnt), 0x00);
					retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
								  TALACT_ERR_RESET_SPI);
					IF_ERR_RETURN_U32(retVal);
				}
			}

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_RESET_JESD204DEFRAMERB);
		} /* end DEFRAMER_B_FIFO_ERROR */

		/* SYSREF Phase Error */
		else if ((gpIntDeframerSources & DEFRAMER_A_SYSREF_ALIGN_ERROR) > 0) {
			if (gpIntDiag != NULL) {
				gpIntDiag->deframerInputsMask = 0;
				gpIntDiag->deframer = TAL_DEFRAMER_A;
			}

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERA, retVal, TALACT_ERR_RESET_JESD204DEFRAMERA);
		} /* end DEFRAMER_A_SYSREF_PHASE_ERROR */

		/* SYSREF Phase Error */
		else if ((gpIntDeframerSources & DEFRAMER_B_SYSREF_ALIGN_ERROR) > 0) {
			if (gpIntDiag != NULL) {
				gpIntDiag->deframerInputsMask = 0;
				gpIntDiag->deframer = TAL_DEFRAMER_B;
			}

			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_DEFRAMERB, retVal, TALACT_ERR_RESET_JESD204DEFRAMERB);
		} /* end DEFRAMER_A_SYSREF_PHASE_ERROR */

	} /* end deframer */

	/* is the source the PA Protection TX1 */
	/* is the source the PA Protection */
	if (((*gpIntStatus & GPINT_PA_PROTECT_TX1_ERROR) > 0) ||
	    ((*gpIntStatus & GPINT_PA_PROTECT_TX2_ERROR) > 0)) {

		/* Write PA Protect Config */
		halError = talSpiWriteField(device->devHalInfo,
					    TALISE_ADDR_PA_PROTECTION_CONFIGURATION, 0x00, 0x80, 0x80);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* is the source the PA Protection TX2 */
		if((*gpIntStatus & GPINT_PA_PROTECT_TX2_ERROR) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_PA_PROTECT_CH2, retVal, TALACT_ERR_REDUCE_TXSAMPLE_PWR);
		}

		/* is the source the PA Protection TX1 */
		else if((*gpIntStatus & GPINT_PA_PROTECT_TX1_ERROR) > 0) {
			return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
							  TALISE_ERR_GPINT_PA_PROTECT_CH1, retVal, TALACT_ERR_REDUCE_TXSAMPLE_PWR);
		}

	}

	/* is the source the stream */
	if (*gpIntStatus & GPINT_STREAM_ERROR) {
		/* disable JESD data for both channels */
		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_JESD_DEFRAMER_SAMPLE_DISABLE_CH1, JESD_SAMPLE_DATA_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   TALISE_ADDR_JESD_DEFRAMER_SAMPLE_DISABLE_CH2, JESD_SAMPLE_DATA_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Ramp down TX power for both channels */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TDD_RAMP_TX1,
					   TXPOWER_RAMP_DOWN_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TDD_RAMP_TX2,
					   TXPOWER_RAMP_DOWN_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* Ramp down distortion amp, etc */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_PD,
					   DISTAMP_RAMP_DOWN_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_PD,
					   DISTAMP_RAMP_DOWN_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		/* power down upconverter */
		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX1_PD,
					   UPCONVERT_RAMP_DOWN_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		halError = talSpiWriteByte(device->devHalInfo, TALISE_ADDR_TX2_PD,
					   UPCONVERT_RAMP_DOWN_MASK);
		retVal = talApiErrHandler(device,TAL_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);

		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_GPINT_STREAM_ERROR, retVal, TALACT_ERR_RESET_FULL);
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_setAuxAdcPinModeGpio(taliseDevice_t *device,
				     taliseGpioPinSel_t pinModeGpio)
{
	static const uint8_t GPIO_STATUS_CMD_SIZE_BYTES = 2;
	static const uint8_t GPIO_CONFIG_CMD_SIZE_BYTES = 3;
	static const uint8_t SIGNALID_AUX_ADC_START = 0x0A;
	static const uint8_t GPIO_CONFIG_POLARITY = 0;
	static const uint8_t GPIO_CONFIG_DISABLE =  0;
	static const uint8_t GPIO_CONFIG_ENABLE = 0x80;

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
	gpioStatus[1] = SIGNALID_AUX_ADC_START;

	/*Initialize gpioConfig array C-99 style*/
	gpioConfig[0] = TALISE_ARM_OBJECTID_GPIO_CTRL;
	gpioConfig[1] = SIGNALID_AUX_ADC_START;
	gpioConfig[2] = 0;

	/*Range check that 1.8v GPIO pin to be assigned for Pin mode Aux ADC start signal is valid*/
	if((pinModeGpio > TAL_GPIO_15) &&
	    (pinModeGpio != TAL_GPIO_INVALID)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_SETAUXADCPINMODEGPIO_INV_GPIO, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Get current GPIO pin assigned to AUX_ADC_START signal_id */
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

	if (armGetGpioStatus <= (uint8_t)TAL_GPIO_15) {
		gpioFreeMask = (1 << (armGetGpioStatus & 0x0F));
	}

	usedGpioPins = device->devStateInfo.usedGpiopins & ~gpioFreeMask;

	if(pinModeGpio <= TAL_GPIO_15) {
		gpioUsedMask = (1 << pinModeGpio);
		gpioConfig[2] = ((uint8_t)pinModeGpio | GPIO_CONFIG_POLARITY |
				 GPIO_CONFIG_ENABLE);
	}

	else if(pinModeGpio == TAL_GPIO_INVALID) {
		gpioUsedMask = 0;
		gpioConfig[2] = ((uint8_t)TAL_GPIO_INVALID | GPIO_CONFIG_POLARITY |
				 GPIO_CONFIG_DISABLE);
	}

	/*Check if GPIO pin is already in use*/
	if ((usedGpioPins & gpioUsedMask) != 0) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_SETAUXADCPINMODEGPIO_GPIO_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	usedGpioPins |= gpioUsedMask;

	/*Write Pin Mode GPIO config to ARM mailbox*/
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &gpioConfig[0], sizeof(gpioConfig));
	IF_ERR_RETURN_U32(retVal);

	/* execute SET Aux ADC config command */
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

	device->devStateInfo.usedGpiopins = usedGpioPins;

	return (uint32_t)retVal;
}

uint32_t TALISE_getAuxAdcPinModeGpio(taliseDevice_t *device,
				     taliseGpioPinSel_t *pinModeGpio)
{
	static const uint8_t GPIO_STATUS_CMD_SIZE_BYTES = 2;
	static const uint8_t SIGNALID_AUX_ADC_START = 0x0A;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t armGetGpioStatus = 0;
	uint8_t gpioStatus[GPIO_STATUS_CMD_SIZE_BYTES];

	/*Initialize gpioStatus array C-99 style*/
	gpioStatus[0] = TALISE_ARM_OBJECTID_GPIO_CTRL;
	gpioStatus[1] = SIGNALID_AUX_ADC_START;

	/* Get current GPIO pin assigned to AUX_ADC_START signal_id */
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

	if (armGetGpioStatus <= (uint8_t)TAL_GPIO_15) {
		*pinModeGpio = (taliseGpioPinSel_t)(armGetGpioStatus & 0x0F);
	} else {
		*pinModeGpio = TAL_GPIO_INVALID;
	}

	return (uint32_t)retVal;
}

uint32_t TALISE_startAuxAdc(taliseDevice_t *device,
			    taliseAuxAdcConfig_t *auxAdcConfig)
{
	static const uint8_t ARM_AUXADC_CFG_CMD_SIZE_BYTES = 1;
	static const uint16_t MIN_AUXADC_SAMPLES = 1;
	static const uint16_t MAX_AUXADC_SAMPLES = 1000;
	static const uint16_t MIN_AUXADC_SAMPLING_PERIOD_US = 15;
	static const uint8_t AUXADC_CONFIG_SIZE_BYTES = 6;
	static const uint32_t SWTST_NUMSAMPLES_OUT_OF_RANGE = 1;
	static const uint32_t SWTST_SAMPLING_PERIOD_OUT_OF_RANGE = 2;
	static const uint32_t SWTST_AUXADC_CH_OUT_OF_RANGE = 3;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;
	uint8_t armAuxAdcCfgCmd[ARM_AUXADC_CFG_CMD_SIZE_BYTES];
	uint8_t auxAdcConfigByteArr[AUXADC_CONFIG_SIZE_BYTES];

	/*Initialize armAuxAdcCfgCmd C-99 style*/
	armAuxAdcCfgCmd[0] = (uint8_t)TALISE_ARM_OBJECTID_GS_AUX_ADC;

	/*Null Check auxAdcConfig structure*/
	if (auxAdcConfig == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_STARTAUXADC_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Range check that AuxADC Channel selected is valid*/
	if((auxAdcConfig->auxAdcChannelSel != TAL_AUXADC_CH0) &&
	    (auxAdcConfig->auxAdcChannelSel != TAL_AUXADC_CH1) &&
	    (auxAdcConfig->auxAdcChannelSel != TAL_AUXADC_CH2) &&
	    (auxAdcConfig->auxAdcChannelSel != TAL_AUXADC_CH3)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_STARTAUXADC_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Range check that AuxADC mode is valid*/
	if((auxAdcConfig->auxAdcMode != TAL_AUXADC_NONPIN_MODE) &&
	    (auxAdcConfig->auxAdcMode != TAL_AUXADC_PIN_MODE)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_STARTAUXADC_INV_MODE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Range check that number of samples for conversion is within range*/
	if((auxAdcConfig->numSamples < MIN_AUXADC_SAMPLES) ||
	    (auxAdcConfig->numSamples > MAX_AUXADC_SAMPLES)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_STARTAUXADC_INV_NUM_SAMPLES, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Range check that sampling period is at least minimum sampling period for a non-pin mode*/
	if((auxAdcConfig->auxAdcMode == TAL_AUXADC_NONPIN_MODE) &&
	    (auxAdcConfig->samplingPeriod_us < MIN_AUXADC_SAMPLING_PERIOD_US)) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_STARTAUXADC_INV_SAMPLING_PERIOD, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*For a pin mode, sampling period member is not used. Transmit a minimum sampling period parameter to ARM*/
	if(auxAdcConfig->auxAdcMode == TAL_AUXADC_PIN_MODE) {
		auxAdcConfig->samplingPeriod_us = MIN_AUXADC_SAMPLING_PERIOD_US;
	}

	if(device->devStateInfo.swTest == SWTST_NUMSAMPLES_OUT_OF_RANGE) {
		/*Aux ADC samples should be in the range 1-1000. Passing 1200 to ARM should throw and ARM exception*/
		auxAdcConfig->numSamples = 1200;
	}

	if(device->devStateInfo.swTest == SWTST_SAMPLING_PERIOD_OUT_OF_RANGE) {
		/*Aux ADC sampling period should be at least 15us. Passing 10us to ARM should throw and ARM exception*/
		auxAdcConfig->samplingPeriod_us = 10;
	}

	if(device->devStateInfo.swTest == SWTST_AUXADC_CH_OUT_OF_RANGE) {
		/*Aux ADC channel should be in the range 0-3. Passing 8 to ARM should throw and ARM exception*/
		auxAdcConfig->auxAdcChannelSel = (taliseAuxAdcChannels_t)8;
	}

	/*Initialize auxAdcConfig Byte Array*/
	auxAdcConfigByteArr[0] = (uint8_t)auxAdcConfig->auxAdcChannelSel;
	auxAdcConfigByteArr[1] = (uint8_t)auxAdcConfig->auxAdcMode;
	auxAdcConfigByteArr[2] = (uint8_t)(auxAdcConfig->numSamples & 0x00FF);
	auxAdcConfigByteArr[3] = (uint8_t)((auxAdcConfig->numSamples >> 8) & 0x00FF);
	auxAdcConfigByteArr[4] = (uint8_t)(auxAdcConfig->samplingPeriod_us & 0x00FF);
	auxAdcConfigByteArr[5] = (uint8_t)((auxAdcConfig->samplingPeriod_us >> 8) &
					   0x00FF);

	/* Write AuxADC config to ARM mailbox*/
	retVal = (talRecoveryActions_t)TALISE_writeArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &auxAdcConfigByteArr[0],
			AUXADC_CONFIG_SIZE_BYTES);
	IF_ERR_RETURN_U32(retVal);

	/* execute SET Aux ADC config command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_SET_OPCODE, &armAuxAdcCfgCmd[0], ARM_AUXADC_CFG_CMD_SIZE_BYTES);
	IF_ERR_RETURN_U32(retVal);

	/*Verify that command executed successfully*/
	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_SET_OPCODE, &cmdStatusByte, SETARMGPIO_TIMEOUT_US,
			SETARMGPIO_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						   ARMCMD_ERRCODE(TALISE_ARM_SET_OPCODE, armAuxAdcCfgCmd[0], cmdStatusByte),
						   retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}


	return (uint32_t)retVal;
}

uint32_t TALISE_readAuxAdc(taliseDevice_t *device,
			   taliseAuxAdcResult_t *auxAdcResult)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t cmdStatusByte = 0;

	static const uint8_t ARM_AUXADC_RESULT_CMD_SIZE_BYTES = 1;
	static const uint8_t ARM_AUXADC_RESULT_SIZE_BYTES = 5;
	static const uint8_t ARM_AUTO_INCREMENT = 0;

	uint8_t armAuxAdcResultCmd[ARM_AUXADC_RESULT_CMD_SIZE_BYTES];
	uint8_t auxAdcResultByteArr[ARM_AUXADC_RESULT_SIZE_BYTES];

	/*Null check function parameters*/
	if (auxAdcResult == NULL) {
		return (uint32_t)talApiErrHandler(device, TAL_ERRHDL_API_GPIO,
						  TALISE_ERR_READAUXADC_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Initialize Aux ADC Result Command C-99 style*/
	armAuxAdcResultCmd[0] = (uint8_t)TALISE_ARM_OBJECTID_GET_AUX_ADC_RESULT;

	/* Executing the GET Aux ADC result command */
	retVal = (talRecoveryActions_t)TALISE_sendArmCommand(device,
			TALISE_ARM_GET_OPCODE, &armAuxAdcResultCmd[0], sizeof(armAuxAdcResultCmd));
	IF_ERR_RETURN_U32(retVal);

	retVal = (talRecoveryActions_t)TALISE_waitArmCmdStatus(device,
			TALISE_ARM_GET_OPCODE, &cmdStatusByte, SETARMGPIO_TIMEOUT_US,
			SETARMGPIO_INTERVAL_US);

	if((cmdStatusByte >> 1) > 0) {
		return  (uint32_t)talApiErrHandler(device, TAL_ERRHDL_ARM_CMD_ERR,
						   ARMCMD_ERRCODE(TALISE_ARM_GET_OPCODE, armAuxAdcResultCmd[0], cmdStatusByte),
						   retVal, TALACT_ERR_RESET_ARM);
	} else {
		IF_ERR_RETURN_U32(retVal);
	}

	/*Read Aux ADC conversion result*/
	retVal = (talRecoveryActions_t)TALISE_readArmMem(device,
			TALISE_ADDR_ARM_START_DATA_ADDR, &auxAdcResultByteArr[0],
			ARM_AUXADC_RESULT_SIZE_BYTES, ARM_AUTO_INCREMENT);
	IF_ERR_RETURN_U32(retVal);

	/*manually unpacking the ARM memory and creating the API structure based on the ARM memory layout*/
	auxAdcResult->auxAdcCodeAvg = (uint16_t)(((uint16_t)auxAdcResultByteArr[1] << 8)
				      | auxAdcResultByteArr[0]);
	auxAdcResult->numSamples = (uint16_t)(((uint16_t)auxAdcResultByteArr[3] << 8) |
					      auxAdcResultByteArr[2]);
	auxAdcResult->completeIndicator = auxAdcResultByteArr[4];

	return (uint32_t)retVal;
}

const char* talGetGpioErrorMessage(uint32_t errSrc, uint32_t errCode)
{
#ifndef TALISE_VERBOSE
	return "";
#else

	if (errSrc == TAL_ERRSRC_TAL_API_GPIO) {
		switch (errCode) {
		case TALISE_ERR_GPIO_OK:
			return "\n";
		case TALISE_ERR_GPIO_OE_INV_PARM:
			return "Invalid value for gpioOutEn in TALISE_setGpioOe\n";
		case TALISE_ERR_GETGPIO_OE_NULL_PARM:
			return "Invalid NULL pointer for gpioOutEn in TALISE_getGpioOe\n";
		case TALISE_ERR_GPIO_SRC_INV_PARM:
			return "Invalid value for gpioSrcCtrl in TALISE_setGpioSourceCtrl\n";
		case TALISE_ERR_GETGPIO_SRC_NULL_PARM:
			return "Invalid NULL pointer for gpioSrcCtrl in TALISE_getGpioSourceCtrl\n";
		case TALISE_ERR_GPIO_LEVEL_INV_PARM:
			return "Invalid value for gpioPinLevel in TALISE_setGpioPinLevel\n";
		case TALISE_ERR_GETGPIO_LEVEL_NULL_PARM:
			return "Invalid NULL pointer for gpioPinLevel in TALISE_getGpioPinLevel\n";
		case TALISE_ERR_GETGPIO_SETLEVEL_NULL_PARM:
			return "Invalid NULL pointer for gpioPinSetLevel in TALISE_getGpioSetLevel\n";
		case TALISE_ERR_MONITOR_OUT_INDEX_RANGE:
			return "Invalid value for monitorIndex in TALISE_setGpioMonitorOut\n";
		case TALISE_ERR_GETGPIOMON_INDEX_NULL_PARM:
			return "Invalid NULL pointer for monitorIndex in TALISE_getGpioMonitorOut\n";
		case TALISE_ERR_GETGPIOMON_MONITORMASK_NULL_PARM:
			return "Invalid NULL pointer for monitorMask in TALISE_getGpioMonitorOut\n";
		case TALISE_ERR_SETUPAUXDAC_NULL_PARM:
			return "Invalid NULL pointer for auxDac in TALISE_setupAuxDacs\n";
		case TALISE_ERR_SETUPAUXDAC_INV_10BIT_AUXDACCODE:
			return "Invalid value for aux10BitDacValue in TALISE_setupAuxDacs\n";
		case TALISE_ERR_SETUPAUXDAC_INV_12BIT_AUXDACCODE:
			return "Invalid value for aux12BitDacValue in TALISE_setupAuxDacs\n";
		case TALISE_ERR_WRITEAUXDAC_INV_10BIT_AUXDACCODE:
			return "Invalid value for auxDacCode in TALISE_writeAuxDac\n";
		case TALISE_ERR_WRITEAUXDAC_INV_12BIT_AUXDACCODE:
			return "Invalid value for auxDacCode in TALISE_writeAuxDac\n";
		case TALISE_ERR_WRITEAUXDAC_INV_AUXDACINDEX:
			return "Invalid value for auxDacIndex in TALISE_writeAuxDac\n";
		case TALISE_ERR_SETUPAUXDAC_INV_RESOLUTION:
			return "Invalid AuxDac resolution for 10bit DACs - use ENUM\n";
		case TALISE_ERR_GPIO3V3_OE_INV_PARM:
			return "TALISE_setGpio3v3Oe(): Invalid value for gpio3v3OutEn\n";
		case TALISE_ERR_GETGPIO3V3_OE_NULL_PARM:
			return "TALISE_getGpio3v3Oe(): Invalid NULL pointer for gpio3v3OutEn\n";
		case TALISE_ERR_GPIO3V3_SRC_INV_PARM:
			return "TALISE_setGpio3v3SourceCtrl(): Invalid value for gpio3v3SrcCtrl\n";
		case TALISE_ERR_GETGPIO3V3_SRC_NULL_PARM:
			return "TALISE_getGpio3v3SourceCtrl(): Invalid NULL pointer for gpio3v3SrcCtrl\n";
		case TALISE_ERR_GPIO3V3_LEVEL_INV_PARM:
			return "TALISE_setGpio3v3PinLevel(): Invalid value for gpio3v3PinLevel\n";
		case TALISE_ERR_GETGPIO3V3_LEVEL_NULL_PARM:
			return "TALISE_getGpio3v3PinLevel(): Invalid NULL pointer for gpio3v3PinLevel\n";
		case TALISE_ERR_GETGPIO3V3_SETLEVEL_NULL_PARM:
			return "TALISE_getGpio3v3SetLevel(): Invalid NULL pointer for gpio3v3PinSetLevel\n";
		case TALISE_ERR_STARTAUXADC_INV_CHANNEL:
			return "TALISE_startAuxAdc(): Invalid Aux ADC channel selected. Valid Channels Ch0,Ch1,Ch2,Ch3";
		case TALISE_ERR_STARTAUXADC_INV_MODE:
			return "TALISE_startAuxAdc(): Invalid Aux ADC mode. Valid Modes - Pin Mode, Non-Pin Mode";
		case TALISE_ERR_STARTAUXADC_INV_NUM_SAMPLES:
			return "TALISE_startAuxAdc(): Aux ADC config number of samples out of range. Valid range 1-1000";
		case TALISE_ERR_STARTAUXADC_INV_SAMPLING_PERIOD :
			return "TALISE_startAuxAdc(): Aux ADC sampling time less than minimum specified sampling time. Minimum sampling time allowed is 15us";
		case TALISE_ERR_SETAUXADCPINMODEGPIO_INV_GPIO :
			return "TALISE_setAuxAdcPinModeGpio(): Aux ADC GPIO pin for pin mode ctrl invalid. Allowed GPIO0-GPIO15";
		case TALISE_ERR_SETAUXADCPINMODEGPIO_GPIO_IN_USE :
			return "TALISE_setAuxAdcPinModeGpio(): Aux ADC pin mode GPIO already in use by another feature";
		case TALISE_ERR_STARTAUXADC_NULL_PARAM :
			return "TALISE_startAuxAdc():  Null function parameter\n";
		case TALISE_ERR_READAUXADC_NULL_PARAM :
			return "TALISE_readAuxAdc():  Null function parameter\n";

		/* GP_INT Error Messages */
		case TALISE_ERR_GPINT_CLKPLL_UNLOCKED:
			return "TALISE_gpIntHandler(): CLK PLL is not locked\n";
		case TALISE_ERR_GPINT_RFPLL_UNLOCKED:
			return "TALISE_gpIntHandler(): RFCLK PLL is not locked\n";
		case TALISE_ERR_GPINT_AUXPLL_UNLOCKED:
			return "TALISE_gpIntHandler(): AUXCLK PLL is not locked\n";
		case TALISE_ERR_GPINT_ARM_WATCHDOG_TIMEOUT:
			return "TALISE_gpIntHandler(): ARM WatchDog Timeout\n";
		case TALISE_ERR_GPINT_ARM_FORCE_GPINT:
			return "TALISE_gpIntHandler(): ARM Forced a General Purpose Interrupt\n";
		case TALISE_ERR_GPINT_ARM_SYSTEM_ERROR:
			return "TALISE_gpIntHandler(): ARM reports a system error\n";
		case TALISE_ERR_GPINT_ARM_DATA_PARITY_ERROR:
			return "TALISE_gpIntHandler(): ARM reports an unrecoverable parity error in data memory\n";
		case TALISE_ERR_GPINT_ARM_PROG_PARITY_ERROR:
			return "TALISE_gpIntHandler(): ARM reports an unrecoverable parity error in program memory\n";
		case TALISE_ERR_GPINT_ARM_CALIBRATION_ERROR:
			return "TALISE_gpIntHandler(): ARM reports an Initialization Calibration Error\n";
		case TALISE_ERR_GPINT_FRAMERA:
			return "TALISE_gpIntHandler(): FramerA reports error\n";
		case TALISE_ERR_GPINT_DEFRAMERA:
			return "TALISE_gpIntHandler(): DeframerA reports error\n";
		case TALISE_ERR_GPINT_FRAMERB:
			return "TALISE_gpIntHandler(): FramerB reports error\n";
		case TALISE_ERR_GPINT_DEFRAMERB:
			return "TALISE_gpIntHandler(): DeframerB reports error\n";
		case TALISE_ERR_GPINT_STATUS_NULL_PARM:
			return "TALISE_gpIntHandler(): gpStatus variable pointer is NULL\n";
		case TALISE_ERR_GPINT_GPINTDIAG_NULL_PARM:
			return "TALISE_gpIntHandler(): gpIntDiag structure pointer is NULL\n";
		case TALISE_ERR_GPINT_PA_PROTECT_CH1:
			return "TALISE_gpIntHandler(): PA Protection Channel1 Reports Error\n";
		case TALISE_ERR_GPINT_PA_PROTECT_CH2:
			return "TALISE_gpIntHandler(): PA Protection Channel2 Reports Error\n";
		case TALISE_ERR_GPINT_STREAM_ERROR:
			return "TALISE_gpIntHandler(): Stream Processor Reports Error\n";
		default:
			return "Invalid GPIO error passed, not in error list\n";
		}
	}
	return "Unknown GPIO Error Source\n";

#endif

}

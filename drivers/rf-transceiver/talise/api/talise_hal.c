// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_hal.c
 * \brief Contains TALISE API Hardware Abstraction Layer (HAL) functions
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "talise_hal.h"

adiHalErr_t talSpiReadByte(void *devHalInfo, uint16_t addr, uint8_t *readdata)
{
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_spiReadByte(devHalInfo, addr, readdata);
	if (halError == ADIHAL_WAIT_TIMEOUT) {
		ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = ADIHAL_spiReadByte(devHalInfo, addr, readdata);
	}

	ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

adiHalErr_t talSpiWriteByte(void *devHalInfo, uint16_t addr, uint8_t data)
{
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_spiWriteByte(devHalInfo, addr, data);
	if (halError == ADIHAL_WAIT_TIMEOUT) {
		ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = ADIHAL_spiWriteByte(devHalInfo, addr, data);
	}

	ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

adiHalErr_t talSpiWriteBytes(void *devHalInfo, uint16_t *addr, uint8_t *data,
			     uint32_t count)
{
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_spiWriteBytes(devHalInfo, addr, data, count);
	if (halError == ADIHAL_WAIT_TIMEOUT) {
		ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = ADIHAL_spiWriteBytes(devHalInfo, addr, data, count);
	}

	ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

adiHalErr_t talSpiReadBytes(void *devHalInfo, uint16_t *addr, uint8_t *readdata,
			    uint32_t count)
{
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_spiReadBytes(devHalInfo, addr, readdata, count);
	if (halError == ADIHAL_WAIT_TIMEOUT) {
		ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = ADIHAL_spiReadBytes(devHalInfo, addr, readdata, count);
	}

	ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

adiHalErr_t talSpiReadField(void *devHalInfo, uint16_t addr, uint8_t *fieldVal,
			    uint8_t mask, uint8_t startBit)
{
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_spiReadField(devHalInfo, addr, fieldVal, mask, startBit);
	if (halError == ADIHAL_WAIT_TIMEOUT) {
		ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = ADIHAL_spiReadField(devHalInfo, addr, fieldVal, mask, startBit);
	}

	ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

adiHalErr_t talSpiWriteField(void *devHalInfo, uint16_t addr, uint8_t fieldVal,
			     uint8_t mask, uint8_t startBit)
{
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_spiWriteField(devHalInfo, addr, fieldVal, mask, startBit);
	if (halError == ADIHAL_WAIT_TIMEOUT) {
		ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = ADIHAL_spiWriteField(devHalInfo, addr, fieldVal, mask, startBit);
	}

	ADIHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

adiHalErr_t talWriteToLog(void *devHalInfo, adiLogLevel_t logLevel,
			  uint32_t errorCode, const char *comment)
{
	adiHalErr_t halError = ADIHAL_OK;

	halError = ADIHAL_writeToLog(devHalInfo, logLevel, errorCode, comment);

	return halError;
}

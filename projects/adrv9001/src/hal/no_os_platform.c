/***************************************************************************//**
 *   @file   no_os_platform.c
 *   @brief  adrv9002 Hardware Abstraction Layer for no-OS.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "adi_platform.h"
#include "parameters.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "spi.h"
#include "spi_extra.h"
#include "error.h"
#include "delay.h"
#include "adi_common_error.h"
#include "Navassa_EvaluationFw.h"
#include "ORxGainTable.h"
#include "RxGainTable.h"
#include "TxAttenTable.h"

/**
 * @brief Opens all neccessary files and device drivers for a specific device
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 * @retval errors returned by other function calls.
 */
int32_t no_os_HwOpen(void *devHalCfg)
{
	int32_t ret;
	adi_hal_Cfg_t *phal = (adi_hal_Cfg_t *)devHalCfg;
	struct gpio_init_param gip_gpio_reset;
#if defined(ADRV9002_RX2TX2)
	struct gpio_init_param gip_gpio_ssi_sync;
#endif
	struct xil_gpio_init_param gip_extra = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct xil_spi_init_param sip_extra = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = SPI_DEVICE_ID,
		.flags = 0
	};

	/* Reset GPIO configuration */
	gip_gpio_reset.number = GPIO_RESET;
	gip_gpio_reset.extra = &gip_extra;
	gip_gpio_reset.platform_ops = &xil_gpio_platform_ops;
	ret = gpio_get(&phal->gpio_reset_n, &gip_gpio_reset);
	if (ret)
		return ret;

	ret = gpio_direction_output(phal->gpio_reset_n, GPIO_HIGH);
	if (ret)
		return ret;

#if defined(ADRV9002_RX2TX2)
	/* SSI Sync GPIO configuration */
	gip_gpio_ssi_sync.number = GPIO_SSI_SYNC;
	gip_gpio_ssi_sync.extra = &gip_extra;
	gip_gpio_ssi_sync.platform_ops = &xil_gpio_platform_ops;
	ret = gpio_get(&phal->gpio_ssi_sync, &gip_gpio_ssi_sync);
	if (ret < 0)
		return ret;

	ret = gpio_direction_output(phal->gpio_ssi_sync, GPIO_LOW);
	if (ret)
		return ret;
#endif
	struct spi_init_param sip = {
		.max_speed_hz = 20000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_CS,
		.platform_ops = &xil_platform_ops,
		.extra = &sip_extra
	};
	ret = spi_init(&phal->spi, &sip);
	if (ret)
		return ret;

	return ADI_HAL_OK;
}

/**
 * @brief Gracefully shuts down the the hardware closing any open resources
 *        such as log files, I2C, SPI, GPIO drivers, timer resources, etc.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 */
int32_t no_os_HwClose(void *devHalCfg)
{
	int32_t ret;
	adi_hal_Cfg_t *phal = (adi_hal_Cfg_t *)devHalCfg;
	ret = gpio_remove(phal->gpio_reset_n);
	if (ret)
		return ret;

	ret = spi_remove(phal->spi);
	if (ret)
		return ret;

#if defined(ADRV9002_RX2TX2)
	ret = gpio_remove(phal->gpio_ssi_sync);
	if (ret)
		return ret;
#endif

	return ADI_HAL_OK;
}

/**
 * @brief This function control a BBIC GPIO pin that connects to the reset pin
 *        of each device.
 *
 *  This function is called by each device API giving access to the Reset pin
 *  connected to each device.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param pinLevel The desired pin logic level 0=low, 1=high to set the GPIO pin to.
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 */
int32_t no_os_HwReset(void *devHalCfg, uint8_t pinLevel)
{
	adi_hal_Cfg_t  *phal = (adi_hal_Cfg_t *)devHalCfg;

	if (!devHalCfg)
		return ADI_HAL_NULL_PTR;

	gpio_set_value(phal->gpio_reset_n, pinLevel);

	return ADI_HAL_OK;
}

/**
 * @brief Write an array of 8-bit data to a SPI device
 *
 * The function will write numTxBytes number of bytes to the SPI device
 * selected in the devHalCfg structure.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param txData Pointer to byte array txData buffer that has numTxBytes number of bytes
 * @param numTxBytes The length of txData array
 *
 * @retval ADI_HAL_OK function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR the function has been called with a null pointer
 * @retval ADI_HAL_SPI_FAIL the data was not written successfully
 */
int32_t no_os_SpiWrite(void *devHalCfg, const uint8_t txData[],
		       uint32_t numTxBytes)
{
	static const int32_t MAX_SIZE = 4096;
	uint32_t toWrite = 0;
	int32_t remaining = numTxBytes;
	adi_hal_Cfg_t *halCfg = NULL;

	if (devHalCfg == NULL)
		return ADI_HAL_NULL_PTR;

	halCfg = (adi_hal_Cfg_t *)devHalCfg;

	if (halCfg->spiCfg.spiActionDisable == 0) {
		int32_t result = 0;
		do {
			toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
			result = spi_write_and_read(halCfg->spi, &txData[numTxBytes - remaining],
						    toWrite);
			if (result < 0)
				return ADI_HAL_SPI_FAIL;

			remaining -= toWrite;
		} while (remaining > 0);
	}

	return ADI_HAL_OK;
}

/**
 * @brief Read one or more bytes from the device specified by the devHalCfg structure
 *
 * The function will read numTxRxBytes number of bytes from the SPI device selected in
 * the devHalCfg parameter and store the resulting data sent by the device in the rxData
 * data buffer.
 *
 * For each byte in txData written to the device, a byte is read and returned by this
 * function at the pointer provided by the rxData parameter.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param txData Pointer to byte array that has numTxRxBytes number of bytes
 * @param rxData Pointer to byte array where read back data will be returned, that is at least numTxRxBytes in size.
 * @param numTxRxBytes The length of txData and rxData arrays
 *
 * @retval ADI_HAL_OK function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR the function has been called with a null pointer
 * @retval ADI_HAL_SPI_FAIL the data was not read successfully
 */
int32_t no_os_SpiRead(void *devHalCfg, const uint8_t txData[], uint8_t rxData[],
		      uint32_t numTxRxBytes)
{
	static const int32_t MAX_SIZE = 4096;
	uint32_t toWrite = 0;
	int32_t remaining = numTxRxBytes;
	int32_t halError = (int32_t)ADI_HAL_OK;
	adi_hal_Cfg_t *halCfg = NULL;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_HAL_NULL_PTR;
		return halError;
	}

	memcpy(rxData, txData, numTxRxBytes);

	halCfg = (adi_hal_Cfg_t *)devHalCfg;

	if (halCfg->spiCfg.spiActionDisable == 0) {
		int32_t result = 0;
		do {
			toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
			result = spi_write_and_read(halCfg->spi, &rxData[numTxRxBytes - remaining],
						    toWrite);
			if (result < 0)
				return ADI_HAL_SPI_FAIL;

			remaining -= toWrite;
		} while (remaining > 0);
	}

	return halError;
}

/**
 * @brief Opens a logFile. If the file is already open it will be closed and reopened.
 *
 * This function opens the file for writing and saves the resulting file
 * descriptor to the devHalCfg structure.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param filename The user provided name of the file to open.
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 * @retval ADI_HAL_LOGGING_FAIL If the function failed to open or write to the specified filename
 */
int32_t no_os_LogFileOpen(void *devHalCfg, const char *filename)
{
	return ADI_HAL_OK;
}

/**
 * @brief Flushes the logFile buffer to the currently open log file.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 */
int32_t no_os_LogFileFlush(void *devHalCfg)
{
	return ADI_HAL_OK;
}

/**
 * @brief Gracefully closes the log file(s).
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 * @retval ADI_HAL_LOGGING_FAIL Error while flushing or closing the log file.
 */
int32_t no_os_LogFileClose(void *devHalCfg)
{
	return ADI_HAL_OK;
}

/**
 * @brief Sets the log level, allowing the end user to select the granularity of
 *        what events get logged.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param logLevel A mask of valid log levels to allow to be written to the log file.
 *
 * @retval ADI_COMMON_ACT_ERR_CHECK_PARAM    Recovery action for bad parameter check
 * @retval ADI_COMMON_ACT_NO_ACTION          Function completed successfully, no action required
 */
int32_t no_os_LogLevelSet(void *devHalCfg, int32_t logLevel)
{
	adi_hal_Cfg_t *halCfg = NULL;

	if (devHalCfg == NULL) {
		return ADI_COMMON_ACT_ERR_CHECK_PARAM;
	}

	halCfg = (adi_hal_Cfg_t *)devHalCfg;

	halCfg->logCfg.logLevel = (logLevel & (int32_t)ADI_HAL_LOG_ALL);

	return ADI_COMMON_ACT_NO_ACTION;
}

/**
 * @brief Gets the currently set log level: the mask of different types of log
 *         events that are currently enabled to be logged.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param logLevel Returns the current log level mask.
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 */
int32_t no_os_LogLevelGet(void *devHalCfg, int32_t *logLevel)
{
	int32_t halError = (int32_t)ADI_HAL_OK;
	adi_hal_Cfg_t *halCfg = NULL;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_HAL_NULL_PTR;
		return halError;
	}

	halCfg = (adi_hal_Cfg_t *)devHalCfg;

	*logLevel = halCfg->logCfg.logLevel;

	return halError;
}

/**
 * @brief Writes a message to the currently open logFile specified in the
 *        adi_hal_LogCfg_t of the devHalCfg structure passed
 *
 * Uses the vfprintf functionality to allow the user to supply the format and
 * the number of aguments that will be logged.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param logLevel the log level to be written into
 * @param comment the string to include in the line added to the log.
 * @param argp variable argument list to be printed
 *
 * @retval ADI_HAL_OK Function completed successfully, no action required
 * @retval ADI_HAL_NULL_PTR The function has been called with a null pointer
 * @retval ADI_HAL_LOGGING_FAIL If the function failed to flush to write
 */
int32_t no_os_LogWrite(void *devHalCfg, int32_t logLevel, const char *comment,
		       va_list argp)
{
	int32_t halError = (int32_t)ADI_HAL_OK;
	int32_t result = 0;
	adi_hal_Cfg_t *halCfg = NULL;
	char logMessage[ADI_HAL_MAX_LOG_LINE] = { 0 };
	const char *logLevelChar = NULL;
	logMessage[0] = 0;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_HAL_NULL_PTR;
		return halError;
	}

	halCfg = (adi_hal_Cfg_t *)devHalCfg;

	if (halCfg->logCfg.logLevel == (int32_t)ADI_HAL_LOG_NONE) {
		/* If logging disabled, exit gracefully */
		halError = (int32_t)ADI_HAL_OK;
		return halError;
	}

	if(logLevel > (int32_t)ADI_HAL_LOG_ALL) {
		halError = (int32_t)ADI_HAL_LOGGGING_LEVEL_FAIL;
		return halError;
	}

	/* Print Log type */
	if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_MSG)
	    && (logLevel == (int32_t)ADI_HAL_LOG_MSG)) {
		logLevelChar = "MESSAGE:";
	} else if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_WARN)
		   && (logLevel == (int32_t)ADI_HAL_LOG_WARN)) {
		logLevelChar = "WARNING:";
	} else if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_ERR)
		   && (logLevel == (int32_t)ADI_HAL_LOG_ERR)) {
		logLevelChar = "ERROR:";
	} else if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_API)
		   && (logLevel == (int32_t)ADI_HAL_LOG_API)) {
		logLevelChar = "API_LOG:";
	} else if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_BF)
		   && (logLevel == (int32_t)ADI_HAL_LOG_BF)) {
		logLevelChar = "BF_LOG:";
	} else if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_HAL)
		   && (logLevel == (int32_t)ADI_HAL_LOG_HAL)) {
		logLevelChar = "ADI_HAL_LOG:";
	} else if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_SPI)
		   && (logLevel == (int32_t)ADI_HAL_LOG_SPI)) {
		logLevelChar = "SPI_LOG:";
	} else if ((halCfg->logCfg.logLevel & ADI_HAL_LOG_API_PRIV)
		   && (logLevel == (int32_t)ADI_HAL_LOG_API_PRIV)) {
		logLevelChar = "API_PRIV_LOG:";
	} else {
		/* Nothing to log - exit cleanly */
		return (int32_t)ADI_HAL_OK;
	}

	result = snprintf(logMessage, ADI_HAL_MAX_LOG_LINE, "%s", logLevelChar);
	if (result < 0) {
		halError = (int32_t)ADI_HAL_LOGGING_FAIL;
		return halError;
	}

	result = vsnprintf(logMessage + strlen(logMessage), ADI_HAL_MAX_LOG_LINE,
			   comment, argp) ;
	if (result < 0) {
		halError = (int32_t)ADI_HAL_LOGGING_FAIL;
		return halError;
	}

	result = printf("%s\n", logMessage);
	if (result < 0) {
		halError = (int32_t)ADI_HAL_LOGGING_FAIL;
		return halError;
	}

	return halError;
}

/**
 * @brief Provides a blocking delay of the current thread
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param time_us the time to delay in mico seconds
 *
 * @retval ADI_HAL_OK Function completed successfully
 * @retval ADI_HAL_NULL_PTR the function has been called with a null pointer
 */
int32_t no_os_TimerWait_us(void *devHalCfg, uint32_t time_us)
{
	int32_t halError = (int32_t)ADI_HAL_OK;

	udelay(time_us);

	return halError;
}

/**
 * @brief Provides a blocking delay of the current thread
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param time_ms the Time to delay in milli seconds
 *
 * @retval ADI_HAL_OK Function completed successfully
 * @retval ADI_HAL_NULL_PTR the function has been called with a null pointer
 */
int32_t no_os_TimerWait_ms(void *devHalCfg, uint32_t time_ms)
{
	int32_t halError = (int32_t)ADI_HAL_OK;

	mdelay(time_ms);

	return halError;
}

/* Not supported yet */
int32_t no_os_Mcs_Pulse(void* devHalCfg, uint8_t numberOfPulses)
{
	return ADI_HAL_FUNCTION_NOT_IMP;
}

/* Not supported yet */
int32_t no_os_ssi_Reset(void* devHalCfg)
{
	return ADI_HAL_FUNCTION_NOT_IMP;
}

int32_t no_os_ImagePageGet(void *devHalCfg, const char *ImagePath,
			   uint32_t pageIndex, uint32_t pageSize, uint8_t *rdBuff)
{
	if ((pageIndex * pageSize) > sizeof(Navassa_EvaluationFw_bin))
		return -EINVAL;

	memcpy(rdBuff, &Navassa_EvaluationFw_bin[pageIndex * pageSize], pageSize);

	return ADI_HAL_OK;
}

int32_t no_os_RxGainTableEntryGet(void *devHalCfg, const char *rxGainTablePath,
				  uint16_t lineCount, uint8_t *gainIndex, uint8_t *rxFeGain,
				  uint8_t *tiaControl, uint8_t *adcControl, uint8_t *extControl,
				  uint16_t *phaseOffset, int16_t *digGain)
{
	if (!strcmp(rxGainTablePath, "RxGainTable.csv")) {
		if (lineCount > sizeof(RxGainTable) / sizeof(struct RxGainTableEntry))
			return -EINVAL;

		*gainIndex = RxGainTable[lineCount].gainIndex;
		*rxFeGain = RxGainTable[lineCount].rxFeGain;
		*tiaControl = RxGainTable[lineCount].tiaControl;
		*adcControl = RxGainTable[lineCount].adcControl;
		*extControl = RxGainTable[lineCount].extControl;
		*phaseOffset = RxGainTable[lineCount].phaseOffset;
		*digGain = RxGainTable[lineCount].digGain;
	} else if (!strcmp(rxGainTablePath, "ORxGainTable.csv")) {
		if (lineCount > sizeof(ORxGainTable) / sizeof(struct ORxGainTableEntry))
			return -EINVAL;

		*gainIndex = ORxGainTable[lineCount].gainIndex;
		*rxFeGain = ORxGainTable[lineCount].rxFeGain;
		*tiaControl = ORxGainTable[lineCount].tiaControl;
		*adcControl = ORxGainTable[lineCount].adcControl;
		*extControl = ORxGainTable[lineCount].extControl;
		*phaseOffset = ORxGainTable[lineCount].phaseOffset;
		*digGain = ORxGainTable[lineCount].digGain;
	} else
		return -EINVAL;

	return 7; /* return the number of filled elements (emulate sscanf return value) */
}

int32_t no_os_TxAttenTableEntryGet(void *devHalCfg,
				   const char *txAttenTablePath, uint16_t lineCount, uint16_t *attenIndex,
				   uint8_t *txAttenHp, uint16_t *txAttenMult)
{
	if (lineCount > sizeof(TxAttenTable) / sizeof(struct TxAttenTableEntry))
		return -EINVAL;

	*attenIndex = TxAttenTable[lineCount].attenIndex;
	*txAttenHp = TxAttenTable[lineCount].txAttenHp;
	*txAttenMult = TxAttenTable[lineCount].txAttenMult;

	return 3; /* return the number of filled elements (emulate sscanf return value) */
}

/*
 * Function pointer assignemt for default configuration
 */

/* Initialization interface to open, init, close drivers and pointers to resources */
int32_t (*adi_hal_HwOpen)(void *devHalCfg) = no_os_HwOpen;
int32_t (*adi_hal_HwClose)(void *devHalCfg) = no_os_HwClose;
int32_t (*adi_hal_HwReset)(void *devHalCfg, uint8_t pinLevel) = no_os_HwReset;

/* SPI Interface */
int32_t (*adi_hal_SpiWrite)(void *devHalCfg, const uint8_t txData[],
			    uint32_t numTxBytes) = no_os_SpiWrite;
int32_t (*adi_hal_SpiRead)(void *devHalCfg, const uint8_t txData[],
			   uint8_t rxData[], uint32_t numRxBytes) = no_os_SpiRead;

/* Logging interface */
int32_t (*adi_hal_LogFileOpen)(void *devHalCfg,
			       const char *filename) = no_os_LogFileOpen;
int32_t(*adi_hal_LogLevelSet)(void *devHalCfg,
			      int32_t logLevel) = no_os_LogLevelSet;
int32_t(*adi_hal_LogLevelGet)(void *devHalCfg,
			      int32_t *logLevel) = no_os_LogLevelGet;
int32_t(*adi_hal_LogWrite)(void *devHalCfg, int32_t logLevel,
			   const char *comment, va_list args) = no_os_LogWrite;
int32_t(*adi_hal_LogFileClose)(void *devHalCfg) = no_os_LogFileClose;

/* Timer interface */
int32_t (*adi_hal_Wait_ms)(void *devHalCfg,
			   uint32_t time_ms) = no_os_TimerWait_ms;
int32_t (*adi_hal_Wait_us)(void *devHalCfg,
			   uint32_t time_us) = no_os_TimerWait_us;

/* Mcs interface */
int32_t(*adi_hal_Mcs_Pulse)(void *devHalCfg,
			    uint8_t numberOfPulses) = no_os_Mcs_Pulse;

/* ssi */
int32_t(*adi_hal_ssi_Reset)(void *devHalCfg) = no_os_ssi_Reset;

/* File IO abstraction */
int32_t(*adi_hal_ArmImagePageGet)(void *devHalCfg, const char *ImagePath,
				  uint32_t pageIndex, uint32_t pageSize, uint8_t *rdBuff) = no_os_ImagePageGet;
int32_t(*adi_hal_StreamImagePageGet)(void *devHalCfg, const char *ImagePath,
				     uint32_t pageIndex, uint32_t pageSize, uint8_t *rdBuff) = no_os_ImagePageGet;
int32_t(*adi_hal_RxGainTableEntryGet)(void *devHalCfg,
				      const char *rxGainTablePath, uint16_t lineCount, uint8_t *gainIndex,
				      uint8_t *rxFeGain,
				      uint8_t *tiaControl, uint8_t *adcControl, uint8_t *extControl,
				      uint16_t *phaseOffset, int16_t *digGain) = no_os_RxGainTableEntryGet;
int32_t(*adi_hal_TxAttenTableEntryGet)(void *devHalCfg,
				       const char *txAttenTablePath, uint16_t lineCount, uint16_t *attenIndex,
				       uint8_t *txAttenHp,
				       uint16_t *txAttenMult) = no_os_TxAttenTableEntryGet;

/**
 * @brief Platform setup
 *
 * @param devHalInfo void pointer to be casted to the HAL config structure
 * @param platform Platform to be assigning the function pointers
 *
 * \return
 */
int32_t adi_hal_PlatformSetup(void *devHalInfo, adi_hal_Platforms_e platform)
{
	adi_hal_Err_e error = ADI_HAL_OK;

	adi_hal_HwOpen = no_os_HwOpen;

	adi_hal_HwClose = no_os_HwClose;
	adi_hal_HwReset = no_os_HwReset;

	adi_hal_SpiWrite = no_os_SpiWrite;
	adi_hal_SpiRead = no_os_SpiRead;

	adi_hal_LogFileOpen = no_os_LogFileOpen;
	adi_hal_LogLevelSet = no_os_LogLevelSet;
	adi_hal_LogLevelGet = no_os_LogLevelGet;
	adi_hal_LogWrite = no_os_LogWrite;
	adi_hal_LogFileClose = no_os_LogFileClose;

	adi_hal_Wait_us = no_os_TimerWait_us;
	adi_hal_Wait_ms = no_os_TimerWait_ms;

	adi_hal_Mcs_Pulse = no_os_Mcs_Pulse;

	adi_hal_ssi_Reset = no_os_ssi_Reset;

	adi_hal_ArmImagePageGet = no_os_ImagePageGet;
	adi_hal_StreamImagePageGet = no_os_ImagePageGet;
	adi_hal_RxGainTableEntryGet = no_os_RxGainTableEntryGet;
	adi_hal_TxAttenTableEntryGet = no_os_TxAttenTableEntryGet;

	return error;
}

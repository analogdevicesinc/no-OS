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
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "adi_common_error.h"
#include "Navassa_EvaluationFw.h"
#include "Navassa_Stream.h"
#include "ORxGainTable.h"
#include "RxGainTable.h"
#include "TxAttenTable.h"
#include "RxGainTable_GainCompensated.h"
#include "adi_platform.h"
#include "adi_platform_types.h"
#include "no_os_platform.h"

/**
 * @brief Opens all neccessary files and device drivers for a specific device
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_COMMON_ERR_OK Function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM The function has been called with a null pointer
 * @retval errors returned by other function calls.
 */
int32_t no_os_hw_open(void *devHalCfg)
{
	int32_t ret;
	struct adrv9002_hal_cfg *phal = (struct adrv9002_hal_cfg *)devHalCfg;
	struct no_os_gpio_init_param gip_gpio_reset;
#if defined(ADRV9002_RX2TX2)
	struct no_os_gpio_init_param gip_gpio_ssi_sync;
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
		.flags = 0
	};

	/* Reset GPIO configuration */
	gip_gpio_reset.number = GPIO_RESET;
	gip_gpio_reset.extra = &gip_extra;
	gip_gpio_reset.platform_ops = &xil_gpio_ops;
	ret = no_os_gpio_get(&phal->gpio_reset_n, &gip_gpio_reset);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_output(phal->gpio_reset_n, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

#if defined(ADRV9002_RX2TX2)
	/* SSI Sync GPIO configuration */
	gip_gpio_ssi_sync.number = GPIO_SSI_SYNC;
	gip_gpio_ssi_sync.extra = &gip_extra;
	gip_gpio_ssi_sync.platform_ops = &xil_gpio_ops;
	ret = no_os_gpio_get(&phal->gpio_ssi_sync, &gip_gpio_ssi_sync);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_direction_output(phal->gpio_ssi_sync, NO_OS_GPIO_LOW);
	if (ret)
		return ret;
#endif
	struct no_os_spi_init_param sip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 20000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = SPI_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &sip_extra
	};
	ret = no_os_spi_init(&phal->spi, &sip);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(phal->gpio_reset_n, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	return ADI_COMMON_ERR_OK;
}

/**
 * @brief Gracefully shuts down the the hardware closing any open resources
 *        such as log files, I2C, SPI, GPIO drivers, timer resources, etc.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_COMMON_ERR_OK Function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM The function has been called with a null pointer
 */
int32_t no_os_hw_close(void *devHalCfg)
{
	int32_t ret;
	struct adrv9002_hal_cfg *phal = (struct adrv9002_hal_cfg *)devHalCfg;
	ret = no_os_gpio_remove(phal->gpio_reset_n);
	if (ret)
		return ret;

	ret = no_os_spi_remove(phal->spi);
	if (ret)
		return ret;

#if defined(ADRV9002_RX2TX2)
	ret = no_os_gpio_remove(phal->gpio_ssi_sync);
	if (ret)
		return ret;
#endif

	return ADI_COMMON_ERR_OK;
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
 * @retval ADI_COMMON_ERR_OK Function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM The function has been called with a null pointer
 */
int32_t no_os_hw_reset(void *devHalCfg, uint8_t pinLevel)
{
	struct adrv9002_hal_cfg  *phal = (struct adrv9002_hal_cfg *)devHalCfg;

	if (!devHalCfg)
		return ADI_COMMON_ERR_NULL_PARAM;

	no_os_gpio_set_value(phal->gpio_reset_n, pinLevel);

	return ADI_COMMON_ERR_OK;
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
 * @retval ADI_COMMON_ERR_OK function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM the function has been called with a null pointer
 * @retval ADI_COMMON_ERR_API_FAIL the data was not written successfully
 */
int32_t no_os_spi_write(void *devHalCfg, const uint8_t txData[],
			uint32_t numTxBytes)
{
	static const int32_t MAX_SIZE = 4096;
	uint32_t toWrite = 0;
	int32_t result = 0;
	int32_t remaining = numTxBytes;
	struct adrv9002_hal_cfg *halCfg = NULL;

	if (devHalCfg == NULL)
		return ADI_COMMON_ERR_NULL_PARAM;

	halCfg = (struct adrv9002_hal_cfg *)devHalCfg;

	do {
		toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
		result = no_os_spi_write_and_read(halCfg->spi, &txData[numTxBytes - remaining],
						  toWrite);
		if (result < 0)
			return ADI_COMMON_ERR_API_FAIL;

		remaining -= toWrite;
	} while (remaining > 0);

	return ADI_COMMON_ERR_OK;
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
 * @retval ADI_COMMON_ERR_OK function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM the function has been called with a null pointer
 * @retval ADI_COMMON_ERR_API_FAIL the data was not read successfully
 */
int32_t no_os_spi_read(void *devHalCfg, const uint8_t txData[],
		       uint8_t rxData[],
		       uint32_t numTxRxBytes)
{
	static const int32_t MAX_SIZE = 4096;
	uint32_t toWrite = 0;
	int32_t result = 0;
	int32_t remaining = numTxRxBytes;
	int32_t halError = (int32_t)ADI_COMMON_ERR_OK;
	struct adrv9002_hal_cfg *halCfg = NULL;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_COMMON_ERR_NULL_PARAM;
		return halError;
	}

	memcpy(rxData, txData, numTxRxBytes);

	halCfg = (struct adrv9002_hal_cfg *)devHalCfg;

	do {
		toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
		result = no_os_spi_write_and_read(halCfg->spi,
						  &rxData[numTxRxBytes - remaining],
						  toWrite);
		if (result < 0)
			return ADI_COMMON_ERR_API_FAIL;

		remaining -= toWrite;
	} while (remaining > 0);

	return ADI_COMMON_ERR_OK;
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
 * @retval ADI_COMMON_ERR_OK Function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM The function has been called with a null pointer
 * @retval ADI_COMMON_ERR_API_FAIL If the function failed to open or write to the specified filename
 */
int32_t no_os_log_file_open(void *devHalCfg, const char *filename)
{
	return ADI_COMMON_ERR_OK;
}

/**
 * @brief Flushes the logFile buffer to the currently open log file.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_COMMON_ERR_OK Function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM The function has been called with a null pointer
 */
int32_t no_os_LogFileFlush(void *devHalCfg)
{
	return ADI_COMMON_ERR_OK;
}

/**
 * @brief Gracefully closes the log file(s).
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 *
 * @retval ADI_COMMON_ERR_OK Function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM The function has been called with a null pointer
 * @retval ADI_COMMON_ERR_API_FAIL Error while flushing or closing the log file.
 */
int32_t no_os_log_file_close(void *devHalCfg)
{
	return ADI_COMMON_ERR_OK;
}

/**
 * @brief Writes a message to the currently open logFile specified in the
 *        adi_hal_LogCfg_t of the devHalCfg structure passed
 *
 * Uses the vfprintf functionality to allow the user to supply the format and
 * the number of aguments that will be logged.
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param log_level the log level to be written into
 * @param comment the string to include in the line added to the log.
 * @param argp variable argument list to be printed
 *
 * @retval ADI_COMMON_ERR_OK Function completed successfully, no action required
 * @retval ADI_COMMON_ERR_NULL_PARAM The function has been called with a null pointer
 * @retval ADI_COMMON_ERR_API_FAIL If the function failed to flush to write
 */
int32_t no_os_log_write(void *devHalCfg, uint32_t log_level,
			const char *comment,
			va_list argp)
{
	int ret;
	struct adrv9002_hal_cfg *hal_cfg = devHalCfg;
	char fmt[512] = { 0 };
	const char * const log_level_char[] = {
		[ADI_LOGLEVEL_TRACE] = "[TRACE]",
		[ADI_LOGLEVEL_DEBUG] = "[DEBUG]",
		[ADI_LOGLEVEL_INFO] = "[INFO]",
		[ADI_LOGLEVEL_WARN] = "[WARN]",
		[ADI_LOGLEVEL_ERROR] = "[ERROR]",
		[ADI_LOGLEVEL_FATAL] = "[FATAL]"
	};

	if (!hal_cfg)
		return ADI_COMMON_ERR_NULL_PARAM;

	if (log_level == ADI_LOGLEVEL_NONE)
		/* If logging disabled, exit gracefully */
		return ADI_COMMON_ERR_OK;
	else if (log_level > ADI_LOGLEVEL_FATAL)
		return ADI_COMMON_ERR_INV_PARAM;

	ret = snprintf(fmt, sizeof(fmt), "%s: %s", log_level_char[log_level], comment);
	if (ret < 0)
		return ADI_COMMON_ERR_API_FAIL;

	switch (log_level) {
	case ADI_LOGLEVEL_TRACE:
	case ADI_LOGLEVEL_DEBUG:
#ifdef DEBUG
		vprintf(fmt, argp);
#endif
		break;
	case ADI_LOGLEVEL_INFO:
	case ADI_LOGLEVEL_WARN:
	case ADI_LOGLEVEL_ERROR:
	case ADI_LOGLEVEL_FATAL:
		vprintf(fmt, argp);
		break;
	}

	return ADI_COMMON_ERR_OK;
}

/**
 * @brief Provides a blocking delay of the current thread
 *
 * @param devHalCfg Pointer to device instance specific platform settings
 * @param time_us the time to delay in mico seconds
 *
 * @retval ADI_COMMON_ERR_OK Function completed successfully
 * @retval ADI_COMMON_ERR_NULL_PARAM the function has been called with a null pointer
 */
int32_t no_os_timer_wait_us(void *devHalCfg, uint32_t time_us)
{
	int32_t halError = (int32_t)ADI_COMMON_ERR_OK;

	no_os_udelay(time_us);

	return halError;
}

/* Not supported yet */
int32_t no_os_mcs_pulse(void* devHalCfg, uint8_t numberOfPulses)
{
	return ADI_COMMON_ERR_OK;
}

/* Not supported yet */
int32_t no_os_ssi_reset(void* devHalCfg)
{
	return ADI_COMMON_ERR_OK;
}

int32_t no_os_image_page_get(void *devHalCfg, const char *ImagePath,
			     uint32_t pageIndex, uint32_t pageSize, uint8_t *rdBuff)
{
	unsigned char *bin;
	if (!strcmp(ImagePath, "Navassa_EvaluationFw.bin")) {
		if ((pageIndex * pageSize) > sizeof(Navassa_EvaluationFw_bin))
			return -EINVAL;

		bin = Navassa_EvaluationFw_bin;
	} else if (!strcmp(ImagePath, "Navassa_Stream.bin")) {
		if ((pageIndex * pageSize) > sizeof(Navassa_Stream_bin))
			return -EINVAL;
		bin = Navassa_Stream_bin;
	} else
		return ADI_COMMON_ERR_INV_PARAM;

	memcpy(rdBuff, &bin[pageIndex * pageSize], pageSize);

	return ADI_COMMON_ERR_OK;
}

int32_t no_os_rx_gain_table_entry_get(void *devHalCfg,
				      const char *rxGainTablePath,
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
	} else if (!strcmp(rxGainTablePath, "RxGainTable_GainCompensated.csv")) {
		if (lineCount > sizeof(RxGainTable_GainCompensated) / sizeof(
			    struct RxGainTable_GainCompensatedEntry))
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

int32_t no_os_tx_atten_table_entry_get(void *devHalCfg,
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
int32_t (*adi_adrv9001_hal_open)(void *devHalCfg) = no_os_hw_open;
int32_t (*adi_adrv9001_hal_close)(void *devHalCfg) = no_os_hw_close;
int32_t (*adi_adrv9001_hal_resetbPin_set)(void *devHalCfg,
		uint8_t pinLevel) = no_os_hw_reset;

/* SPI Interface */
int32_t (*adi_hal_SpiWrite)(void *devHalCfg, const uint8_t txData[],
			    uint32_t numTxBytes) = no_os_spi_write;
int32_t (*adi_hal_SpiRead)(void *devHalCfg, const uint8_t txData[],
			   uint8_t rxData[], uint32_t numRxBytes) = no_os_spi_read;

/* Logging interface */
int32_t (*adi_hal_LogFileOpen)(void *devHalCfg,
			       const char *filename) = no_os_log_file_open;
int32_t(*adi_common_hal_LogWrite)(void *devHalCfg, uint32_t logLevel,
				  const char *comment, va_list args) = no_os_log_write;
int32_t(*adi_hal_LogFileClose)(void *devHalCfg) = no_os_log_file_close;

/* Timer interface */
int32_t (*adi_common_hal_Wait_us)(void *devHalCfg,
				  uint32_t time_us) = no_os_timer_wait_us;

/* Mcs interface */
int32_t (*adi_hal_Mcs_Pulse)(void *devHalCfg,
			     uint8_t numberOfPulses) = no_os_mcs_pulse;

/* ssi */
int32_t (*adi_hal_ssi_Reset)(void *devHalCfg) = no_os_ssi_reset;

/* File IO abstraction */
int32_t (*adi_hal_ArmImagePageGet)(void *devHalCfg, const char *ImagePath,
				   uint32_t pageIndex, uint32_t pageSize, uint8_t *rdBuff) = no_os_image_page_get;
int32_t (*adi_hal_StreamImagePageGet)(void *devHalCfg, const char *ImagePath,
				      uint32_t pageIndex, uint32_t pageSize, uint8_t *rdBuff) = no_os_image_page_get;
int32_t (*adi_hal_RxGainTableEntryGet)(void *devHalCfg,
				       const char *rxGainTablePath, uint16_t lineCount, uint8_t *gainIndex,
				       uint8_t *rxFeGain,
				       uint8_t *tiaControl, uint8_t *adcControl, uint8_t *extControl,
				       uint16_t *phaseOffset, int16_t *digGain) = no_os_rx_gain_table_entry_get;
int32_t (*adi_hal_TxAttenTableEntryGet)(void *devHalCfg,
					const char *txAttenTablePath, uint16_t lineCount, uint16_t *attenIndex,
					uint8_t *txAttenHp,
					uint16_t *txAttenMult) = no_os_tx_atten_table_entry_get;

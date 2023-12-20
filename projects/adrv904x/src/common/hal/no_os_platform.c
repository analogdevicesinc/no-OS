// SPDX-License-Identifier: GPL-2.0
/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information.
* see the "LICENSE.txt" file in this zip file.
*/

#include "ADRV9040_RxGainTable.h"
#include "ADRV9040_DFE_CALS_FW.h"
#include "DeviceProfileTest.h"
#include "adi_common_error.h"
#include "no_os_print_log.h"
#include "no_os_platform.h"
#include "stream_image.h"
#include "adi_platform.h"
#include "ADRV9040_FW.h"
#include "no_os_mutex.h"
#include "no_os_delay.h"
#include "common_data.h"
#include "no_os_alloc.h"
#include "parameters.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

CUSTOM_FILE profile;

/**
 * \brief Opens a logFile. If the file is already open it will be closed and reopened.
 *
 * This function opens the file for writing and saves the resulting file
 * descriptor to the devHalCfg structure.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param filename The user provided name of the file to open.
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 * \retval ADI_HAL_LOGGING_FAIL If the function failed to open or write to the specified filename
 */
adi_hal_Err_e no_os_LogFileOpen(void *devHalCfg, const char *filename)
{
	return ADI_HAL_ERR_OK;
}

/**
 * \brief Flushes the logFile buffer to the currently open log file.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 */
int32_t no_os_LogFileFlush(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

/**
 * \brief Gracefully closes the log file(s).
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 * \retval ADI_HAL_LOGGING_FAIL Error while flushing or closing the log file.
 */
adi_hal_Err_e no_os_LogFileClose(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

/**
 * ToDo
 */
adi_hal_Err_e no_os_LogStatusGet(void* const devHalCfg,
				 adi_hal_LogStatusGet_t* const logStatus)
{
	return ADI_HAL_ERR_OK;
}

/**
 * ToDo
 */
adi_hal_Err_e no_os_LogConsoleSet(void* const devHalCfg,
				  const adi_hal_LogConsole_e logConsoleFlag)
{
	return ADI_HAL_ERR_OK;
}

/**
 * \brief Sets the log level, allowing the end user to select the granularity of
 *        what events get logged.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param logLevel A mask of valid log levels to allow to be written to the log file.
 *
 * \retval ADI_COMMON_ERR_ACT_CHECK_PARAM    Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_NO_ACTION          Function completed successfully, no action required
 */
adi_hal_Err_e no_os_LogLevelSet(void *devHalCfg, const uint32_t logMask)
{
	struct adrv904x_hal_cfg *halCfg = NULL;

	if (devHalCfg == NULL) {
		return ADI_COMMON_ERR_ACT_CHECK_PARAM;
	}

	halCfg = (struct adrv904x_hal_cfg *)devHalCfg;

	halCfg->logLevel = (logMask & (int32_t)ADI_HAL_LOG_ALL);

	return ADI_HAL_ERR_OK;
}

/**
 * \brief Gets the currently set log level: the mask of different types of log
 *         events that are currently enabled to be logged.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param logLevel Returns the current log level mask.
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 */
adi_hal_Err_e no_os_LogLevelGet(void* const devHalCfg, uint32_t *logLevel)
{
	int32_t halError = (int32_t)ADI_HAL_ERR_OK;
	struct adrv904x_hal_cfg *halCfg = NULL;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_HAL_ERR_NULL_PTR;
		return halError;
	}

	halCfg = (struct adrv904x_hal_cfg *)devHalCfg;

	*logLevel = halCfg->logLevel;

	return halError;
}

/**
 * \brief Writes a message to the currently open logFile specified in the
 *        adi_hal_LogCfg_t of the devHalCfg structure passed
 *
 * Uses the vfprintf functionality to allow the user to supply the format and
 * the number of aguments that will be logged.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param logLevel the log level to be written into
 * \param comment the string to include in the line added to the log.
 * \param argp variable argument list to be printed
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 * \retval ADI_HAL_LOGGING_FAIL If the function failed to flush to write
 */

adi_hal_Err_e no_os_LogWrite(void* const                 devHalCfg,
			     const adi_hal_LogLevel_e    logLevel,
			     const uint8_t               indent,
			     const char* const           comment,
			     va_list                     argp)
{
	int32_t halError = (int32_t)ADI_HAL_ERR_OK;
	int32_t result = 0;
	struct adrv904x_hal_cfg *halCfg = NULL;
	char logMessage[ADI_HAL_MAX_LOG_LINE] = { 0 };
	const char *logLevelChar = NULL;
	logMessage[0] = 0;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_HAL_ERR_NULL_PTR;
		return halError;
	}

	halCfg = (struct adrv904x_hal_cfg *)devHalCfg;

	if (halCfg->logLevel == (int32_t)ADI_HAL_LOG_NONE) {
		/* If logging disabled, exit gracefully */
		halError = (int32_t)ADI_HAL_ERR_OK;
		return halError;
	}

	if (logLevel > (int32_t)ADI_HAL_LOG_ALL) {
		//halError = (int32_t)ADI_HAL_LOGGGING_LEVEL_FAIL;
		halError = (int32_t)6;
		return halError;
	}

	/* Print Log type */
	if ((halCfg->logLevel & ADI_HAL_LOG_MSG) &&
	    (logLevel == (int32_t)ADI_HAL_LOG_MSG)) {
		logLevelChar = "MESSAGE:";
	} else if ((halCfg->logLevel & ADI_HAL_LOG_WARN) &&
		   (logLevel == (int32_t)ADI_HAL_LOG_WARN)) {
		logLevelChar = "WARNING:";
	} else if ((halCfg->logLevel & ADI_HAL_LOG_ERR) &&
		   (logLevel == (int32_t)ADI_HAL_LOG_ERR)) {
		logLevelChar = "ERROR:";
	} else if ((halCfg->logLevel & ADI_HAL_LOG_API) &&
		   (logLevel == (int32_t)ADI_HAL_LOG_API)) {
		logLevelChar = "API_LOG:";
	} else if ((halCfg->logLevel & ADI_HAL_LOG_HAL) &&
		   (logLevel == (int32_t)ADI_HAL_LOG_HAL)) {
		logLevelChar = "ADI_HAL_LOG:";
	} else if ((halCfg->logLevel & ADI_HAL_LOG_SPI) &&
		   (logLevel == (int32_t)ADI_HAL_LOG_SPI)) {
		logLevelChar = "SPI_LOG:";
	} else if ((halCfg->logLevel & ADI_HAL_LOG_API_PRIV) &&
		   (logLevel == (int32_t)ADI_HAL_LOG_API_PRIV)) {
		logLevelChar = "API_PRIV_LOG:";
	} else {
		/* Nothing to log - exit cleanly */
		return (int32_t)ADI_HAL_ERR_OK;
	}

	result = snprintf(logMessage, ADI_HAL_MAX_LOG_LINE, "%s", logLevelChar);
	if (result < 0) {
		//halError = (int32_t)ADI_HAL_LOGGING_FAIL;
		halError = (int32_t)5;
		return halError;
	}

	result = vsnprintf(logMessage + strlen(logMessage),
			   ADI_HAL_MAX_LOG_LINE, comment, argp);
	if (result < 0) {
		// halError = (int32_t)ADI_HAL_LOGGING_FAIL;
		halError = (int32_t)5;
		return halError;
	}

	switch (logLevel) {
	case ADI_HAL_LOG_NONE:
		break;
	case ADI_HAL_LOG_WARN:
		pr_warning("%s\n", logMessage);
		break;
	case ADI_HAL_LOG_ERR:
		pr_err("%s\n", logMessage);
		break;
	case ADI_HAL_LOG_SPI:
		pr_debug("%s\n", logMessage);
		break;
	case ADI_HAL_LOG_API:
	case ADI_HAL_LOG_API_PRIV:
	case ADI_HAL_LOG_BF:
	case ADI_HAL_LOG_HAL:
	case ADI_HAL_LOG_MSG:
		pr_debug("%s\n", logMessage);
		break;
	case ADI_HAL_LOG_ALL:
		pr_info(logMessage);
		break;
	}

	return halError;
}

/**
 * \brief Opens/allocates any necessary resources to communicate via SPI to a
 *         particular device specified in the devHalCfg structure.
 *
 * This function should perform any necessary steps to open the SPI master resource
 * on the BBIC to enable SPI communications to a particular SPI device.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR the function has been called with a null pointer
 * \retval ADI_HAL_SPI_FAIL the device driver was not opened successfully
 */
int32_t no_os_SpiOpen(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

/**
 * \brief Closes any resources open/allocated for a specific SPI device
 *
 * Any information needed to close the particular SPI device should be passed in
 * the devHalCfg structure.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR the function has been called with a null pointer
 * \retval ADI_HAL_SPI_FAIL the device driver was not closed successfully
 */
int32_t no_os_SpiClose(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

/**
 * \brief Initializes the SPI device driver mode, bits per word, and speed
 *
 * Any settings needed should be passed in the devHalCfg structure
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR the function has been called with a null pointer
 * \retval ADI_HAL_SPI_FAIL the SPI initialization failed
 */
adi_hal_Err_e no_os_SpiInit(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

/**
 * \brief Write an array of 8-bit data to a SPI device
 *
 * The function will write numTxBytes number of bytes to the SPI device
 * selected in the devHalCfg structure.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param txData Pointer to byte array txData buffer that has numTxBytes number of bytes
 * \param numTxBytes The length of txData array
 *
 * \retval ADI_HAL_ERR_OK function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR the function has been called with a null pointer
 * \retval ADI_HAL_SPI_FAIL the data was not written successfully
 */
adi_hal_Err_e no_os_SpiWrite(void *devHalCfg, const uint8_t txData[],
			     uint32_t numTxBytes)
{
	int32_t halError = (int32_t)ADI_HAL_ERR_OK;
	struct adrv904x_hal_cfg *halCfg = NULL;
	static const uint32_t MAX_SIZE = 4096;
	int32_t remaining = numTxBytes;
	uint32_t toWrite = 0;
	int32_t result = 0;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_HAL_ERR_NULL_PTR;
		return halError;
	}

	halCfg = (struct adrv904x_hal_cfg *)devHalCfg;

	do {
		toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
		result = no_os_spi_write_and_read(halCfg->spi,
						  &txData[numTxBytes - remaining],
						  toWrite);
		if (result < 0) {
			//return ADI_HAL_SPI_FAIL;
			return 2;
		}
		remaining -= toWrite;
	} while (remaining > 0);

	return halError;
}

/**
 * \brief Read one or more bytes from the device specified by the devHalCfg structure
 *
 * The function will read numTxRxBytes number of bytes from the SPI device selected in
 * the devHalCfg parameter and store the resulting data sent by the device in the rxData
 * data buffer.
 *
 * For each byte in txData written to the device, a byte is read and returned by this
 * function at the pointer provided by the rxData parameter.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param txData Pointer to byte array that has numTxRxBytes number of bytes
 * \param rxData Pointer to byte array where read back data will be returned, that is at least numTxRxBytes in size.
 * \param numTxRxBytes The length of txData and rxData arrays
 *
 * \retval ADI_HAL_ERR_OK function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR the function has been called with a null pointer
 * \retval ADI_HAL_SPI_FAIL the data was not read successfully
 */
adi_hal_Err_e no_os_SpiRead(void *devHalCfg, const uint8_t txData[],
			    uint8_t rxData[],
			    uint32_t numTxRxBytes)
{
	int32_t halError = (int32_t)ADI_HAL_ERR_OK;
	struct adrv904x_hal_cfg *halCfg = NULL;
	static const uint32_t MAX_SIZE = 4096;
	int32_t remaining = numTxRxBytes;
	uint32_t toWrite = 0;
	int32_t result = 0;

	if (devHalCfg == NULL) {
		halError = (int32_t)ADI_HAL_ERR_NULL_PTR;
		return halError;
	}

	memcpy(rxData, txData, numTxRxBytes);

	halCfg = (struct adrv904x_hal_cfg *)devHalCfg;

	do {
		toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
		result = no_os_spi_write_and_read(halCfg->spi,
						  &rxData[numTxRxBytes - remaining],
						  toWrite);
		if (result < 0)
			//return ADI_HAL_SPI_FAIL;
			return 2;
		remaining -= toWrite;
	} while (remaining > 0);

	return halError;
}

/**
 * ToDo
 */
adi_hal_Err_e no_os_RegisterWrite(void* const devHalCfg, const uint32_t addr,
				  const uint32_t data)
{
	return (int32_t)ADI_HAL_ERR_OK;
}

/**
 * ToDo
 */
adi_hal_Err_e no_os_RegisterRead(void* const devHalCfg, const uint32_t addr,
				 uint32_t* const data)
{
	return (int32_t)ADI_HAL_ERR_OK;
}

/**
 * \brief Function to open/allocate any necessary resources for the timer wait
 *        functions below.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 */
int32_t no_os_TimerOpen(void *devHalCfg)
{
	/* ADI ZC706 platform does not require any timer open /close */
	return (int32_t)ADI_HAL_ERR_OK;
}

/**
 * \brief Function to close any necessary resources for the timer wait
 *        functions below.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 */
int32_t no_os_TimerClose(void *devHalCfg)
{
	/* ADI ZC706 platform does not require any timer open /close */
	return (int32_t)ADI_HAL_ERR_OK;
}

/**
 * \brief Function to initialize any necessary resources for the timer wait
 *        functions below.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 */
int32_t no_os_TimerInit(void *devHalCfg)
{
	/* ADI ZC706 platform does not require any timer init */
	return (int32_t)ADI_HAL_ERR_OK;
}

/**pthread_mutex_t
 * \brief Provides a blocking delay of the current thread
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param time_ms the time to delay in milli seconds
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 * \retval ADI_HAL_ERR_NULL_PTR the function has been called with a null pointer
 */
adi_hal_Err_e no_os_TimerWait_ms(void *devHalCfg, uint32_t time_ms)
{
	int32_t halError = (int32_t)ADI_HAL_ERR_OK;

	no_os_mdelay(time_ms);

	return halError;
}

/**
 * \brief Provides a blocking delay of the current thread
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param time_us the time to delay in mico seconds
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully
 * \retval ADI_HAL_ERR_NULL_PTR the function has been called with a null pointer
 */
adi_hal_Err_e no_os_TimerWait_us(void *devHalCfg, uint32_t time_us)
{
	int32_t halError = (int32_t)ADI_HAL_ERR_OK;

	no_os_udelay(time_us);

	return halError;
}

/**
 * \brief ToDo
 */
adi_hal_Err_e no_os_mutex_init_wrapper(adi_hal_mutex_t* const mutex)
{
//	no_os_mutex_init(mutex);

	return ADI_HAL_ERR_OK;
}

/**
 * \brief ToDo
 */
adi_hal_Err_e no_os_mutex_lock_wrapper(adi_hal_mutex_t* const mutex)
{
//	no_os_mutex_lock(*mutex);

	return ADI_HAL_ERR_OK;
}

/**
 * \brief ToDo
 */
adi_hal_Err_e no_os_mutex_unlock_wrapper(adi_hal_mutex_t* const mutex)
{
//	no_os_mutex_unlock(*mutex);

	return ADI_HAL_ERR_OK;
}

/**
 * \brief ToDo
 */
adi_hal_Err_e no_os_mutex_remove_wrapper(adi_hal_mutex_t* const mutex)
{
//	no_os_mutex_remove(mutex);

	return ADI_HAL_ERR_OK;
}

/**
 * \brief Opens all necessary files and device drivers for a specific device
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 * \retval errors returned by other function calls.
 */
adi_hal_Err_e no_os_HwOpen(void *devHalCfg)
{
	int32_t ret;
	struct adrv904x_hal_cfg *phal = (struct adrv904x_hal_cfg *)devHalCfg;
	struct no_os_gpio_init_param gip_gpio_reset_n = { 0 };
	struct no_os_spi_init_param sip = { 0 };

	/* sysref req GPIO configuration */
	gip_gpio_reset_n.number = ADRV9040_RESET_B;
	gip_gpio_reset_n.extra = clkchip_gpio_init_param.extra;
	gip_gpio_reset_n.platform_ops = clkchip_gpio_init_param.platform_ops;
	ret = no_os_gpio_get(&phal->gpio_reset_n, &gip_gpio_reset_n);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_output(phal->gpio_reset_n, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	sip.device_id = SPI_DEVICE_ID;
	sip.max_speed_hz = 5000000u;
	sip.mode = NO_OS_SPI_MODE_0;
	sip.chip_select = ADRV9040_CS;
	sip.platform_ops = ad9528_spi_param.platform_ops;
	sip.extra = ad9528_spi_param.extra;

	ret = no_os_spi_init(&phal->spi, &sip);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(phal->gpio_reset_n, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	return ADI_HAL_ERR_OK;
}

/**
 * \brief Gracefully shuts down the the hardware closing any open resources
 *        such as log files, I2C, SPI, GPIO drivers, timer resources, etc.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 */
adi_hal_Err_e no_os_HwClose(void *devHalCfg)
{
	int32_t ret;
	struct adrv904x_hal_cfg *phal = (struct adrv904x_hal_cfg *)devHalCfg;
	ret = no_os_gpio_remove(phal->gpio_reset_n);
	if (ret)
		return ret;

	ret = no_os_spi_remove(phal->spi);
	if (ret)
		return ret;

	return ADI_HAL_ERR_OK;
}

/**
 * \brief This function control a BBIC GPIO pin that connects to the reset pin
 *        of each device.
 *
 *  This function is called by each device API giving access to the Reset pin
 *  connected to each device.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param pinLevel The desired pin logic level 0=low, 1=high to set the GPIO pin to.
 *
 * \retval ADI_HAL_ERR_OK Function completed successfully, no action required
 * \retval ADI_HAL_ERR_NULL_PTR The function has been called with a null pointer
 */
adi_hal_Err_e no_os_HwReset(void *devHalCfg, const uint8_t pinLevel)
{
	struct adrv904x_hal_cfg *phal = (struct adrv904x_hal_cfg *)devHalCfg;

	if (devHalCfg == NULL) {
		return ADI_HAL_ERR_NULL_PTR;
	}

	no_os_gpio_set_value(phal->gpio_reset_n, pinLevel);

	return ADI_HAL_ERR_OK;
}

/*
 * Function pointer assignment for default configuration
 */

/* Initialization interface to open, init, close drivers and pointers to resources */
adi_hal_Err_e (*adi_hal_HwOpen)(void *devHalCfg) = no_os_HwOpen;
adi_hal_Err_e (*adi_hal_HwClose)(void *devHalCfg) = no_os_HwClose;
adi_hal_Err_e (*adi_hal_HwReset)(void *devHalCfg,
				 const uint8_t pinLevel) = no_os_HwReset;
adi_hal_Err_e (*adi_hal_SpiInit)(void *devHalCfg) =
	no_os_SpiInit; /* TODO: remove?  called by HwOpen() */
void *(*adi_hal_DevHalCfgCreate)(uint32_t interfaceMask, uint8_t spiChipSelect,
				 const char *logFilename) = NULL;
adi_hal_Err_e (*adi_hal_DevHalCfgFree)(void *devHalCfg) = NULL;

/* SPI Interface */
adi_hal_Err_e (*adi_hal_SpiWrite)(void *devHalCfg, const uint8_t txData[],
				  uint32_t numTxBytes) = no_os_SpiWrite;

adi_hal_Err_e (*adi_hal_SpiRead)(void *devHalCfg, const uint8_t txData[],
				 uint8_t rxData[], uint32_t numRxBytes) = no_os_SpiRead;

adi_hal_Err_e (*adi_hal_BbicRegisterWrite)(void* const devHalCfg,
		const uint32_t addr, const uint32_t data) = no_os_RegisterWrite;

adi_hal_Err_e (*adi_hal_BbicRegisterRead)(void* const devHalCfg,
		const uint32_t addr, uint32_t* const data) = no_os_RegisterRead;

adi_hal_Err_e (*adi_hal_BbicRegistersRead)(  void* const     devHalCfg,
		const uint32_t  addr,
		uint32_t        data[],
		const uint32_t  numDataWords) = NULL; //ToDo

adi_hal_Err_e (*adi_hal_BbicRegistersWrite)( void* const     devHalCfg,
		const uint32_t  addr,
		const uint32_t  data[],
		const uint32_t  numDataWords) = NULL; //ToDo

/* Custom SPI streaming interface*/
int32_t (*adi_hal_CustomSpiStreamWrite)(void *devHalCfg, const uint16_t address,
					const uint8_t txData[],
					uint32_t numTxBytes,
					uint8_t numBytesofAddress,
					uint8_t numBytesOfDataPerStream) = NULL;

int32_t (*adi_hal_CustomSpiStreamRead)(void *devHalCfg, const uint16_t address,
				       uint8_t rxData[], uint32_t numRxBytes,
				       uint8_t numBytesofAddress,
				       uint8_t numBytesOfDataPerStream) = NULL;

/* Logging interface */
adi_hal_Err_e (*adi_hal_LogFileOpen)(void *devHalCfg,
				     const char *filename) = no_os_LogFileOpen;

adi_hal_Err_e (*adi_hal_LogLevelSet)(void *devHalCfg,
				     const uint32_t logMask) = no_os_LogLevelSet;

adi_hal_Err_e (*adi_hal_LogLevelGet)(void* const devHalCfg,
				     uint32_t* logMask) = no_os_LogLevelGet;

adi_hal_Err_e (*adi_hal_LogWrite)(void* const                 devHalCfg,
				  const adi_hal_LogLevel_e    logLevel,
				  const uint8_t               indent,
				  const char* const           comment,
				  va_list                     argp) = no_os_LogWrite;

adi_hal_Err_e (*adi_hal_LogFileClose)(void *devHalCfg) = no_os_LogFileClose;

adi_hal_Err_e (*adi_hal_LogStatusGet)(void* const devHalCfg,
				      adi_hal_LogStatusGet_t* const logStatus) = no_os_LogStatusGet;

adi_hal_Err_e (*adi_hal_LogConsoleSet)(void* const devHalCfg,
				       const adi_hal_LogConsole_e logConsoleFlag) = no_os_LogConsoleSet; // ToDo

/* Timer interface */
adi_hal_Err_e (*adi_hal_Wait_ms)(void *devHalCfg,
				 uint32_t time_ms) = no_os_TimerWait_ms;

adi_hal_Err_e (*adi_hal_Wait_us)(void *devHalCfg,
				 uint32_t time_us) = no_os_TimerWait_us;


/* Threads */
//ToDo
adi_hal_thread_t (*adi_hal_ThreadSelf)(void) = NULL;

void* no_os_hal_TlsGet(const adi_hal_TlsType_e tlsType)
{
	return NULL;
}

void* (*adi_hal_TlsGet)(const adi_hal_TlsType_e tlsType) = no_os_hal_TlsGet;

adi_hal_Err_e no_os_hal_TlsSet(const adi_hal_TlsType_e tlsType,
			       void* const value)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e (*adi_hal_TlsSet)(const adi_hal_TlsType_e tlsType,
				void* const value) = no_os_hal_TlsSet;

/* Mutexes */
adi_hal_Err_e (*adi_hal_MutexInit)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_init_wrapper;

adi_hal_Err_e(*adi_hal_MutexLock)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_lock_wrapper;
adi_hal_Err_e(*adi_hal_MutexUnlock)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_unlock_wrapper;
adi_hal_Err_e(*adi_hal_MutexDestroy)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_remove_wrapper;

/*
 * FileIO abstraction
 *
 */

long int ftell (FILE *stream)
{
	return profile.ptr - profile.start;
}

FILE* __fopen(const char * filename, const char *mode)
{
	FILE *stream = no_os_calloc(1, sizeof(*stream));
	unsigned int length;
	char *temp;

	if (!strcmp(filename, "DeviceProfileTest.bin")) {
		length = sizeof(DeviceProfileTest_bin);
		temp = (char *)no_os_calloc(length, sizeof(char));
		memcpy(temp, DeviceProfileTest_bin, length);
		profile.data = temp;
		profile.start = profile.ptr = profile.data;
		profile.end = profile.start + length;
	} else if (!strcmp(filename, "stream_image.bin")) {
		length = sizeof(stream_image_bin);
		temp = (char *)no_os_calloc(length, sizeof(char));
		memcpy(temp, stream_image_bin, length);
		profile.data = temp;
		profile.start = profile.ptr = profile.data;
		profile.end = profile.start + length;
	} else if (!strcmp(filename, "ADRV9040_FW.bin")) {
		length = sizeof(ADRV9040_FW_bin);
		temp = (char *)no_os_calloc(length, sizeof(char));
		memcpy(temp, ADRV9040_FW_bin, sizeof(ADRV9040_FW_bin));
		profile.data = temp;
		profile.start = profile.ptr = profile.data;
		profile.end = profile.start + length;
	} else if (!strcmp(filename, "ADRV9040_DFE_CALS_FW.bin")) {
		length = sizeof(ADRV9040_DFE_CALS_FW_bin);
		temp = (char *)no_os_calloc(length, sizeof(char));
		memcpy(temp, ADRV9040_DFE_CALS_FW_bin, sizeof(ADRV9040_DFE_CALS_FW_bin));
		profile.data = temp;
		profile.start = profile.ptr = profile.data;
		profile.end = profile.start + length;
	} else if (!strcmp(filename, "RxGainTable.csv")) {
		length = strlen(ADRV9040_RxGainTable_text);
		temp = (char *)no_os_calloc(length, sizeof(char));
		strcpy(temp, ADRV9040_RxGainTable_text);
		profile.data = temp;
		profile.start = profile.ptr = profile.data;
		profile.end = profile.start + strlen(profile.data);
	} else {
		no_os_free(stream);
	}

	return stream;
}

int fseekx(FILE * stream, long int offset, int origin)
{
	char *ptr;

	switch (origin) {
	case SEEK_END:
		ptr = profile.end + offset;
		break;
	case SEEK_SET:
		ptr = profile.start + offset;
		break;
	case SEEK_CUR:
		ptr = profile.ptr + offset;
		break;
	default:
		return -EINVAL;
	}

	if (ptr > profile.end || ptr < profile.start)
		return -1;

	profile.ptr = ptr;

	return 0;
}

int __fclose(FILE *stream)
{

	if (stream == NULL)
		return -ENODEV;

	memset(&profile, 0, sizeof(profile));
	no_os_free(stream);

	return 0;
}

char * fgets(char *dst, int num, FILE *stream)
{
	char *p;
	int c;

	for (p = dst, num--; num > 0; num--) {
		if (profile.ptr + 1 > profile.end) {
			return NULL;
		}

		c = *profile.ptr++;
		*p++ = c;

		if (c == '\n')
			break;
	}
	*p = 0;
	if (p == dst)
		return NULL;

	return p;
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	int total = size * count;

	if ((profile.ptr + total) > profile.end)
		total = profile.end - profile.ptr;

	memcpy(ptr, profile.ptr, total);
	profile.ptr += total;

	return total;
}

size_t fwrite(const void * ptr, size_t size, size_t count, FILE *stream)
{
	return 0;
}

/*
 * Iterative algorithm based on the fact that
 * multiplication (division) in the linear domain is equivalent to
 * addition (subtraction) in the log domain.
 */

#define COMPUTE(n, d) if (neg) {a *= d; a /= n;} else {a *= n; a /= d;};

long int_20db_to_mag(long a, int mdB)
{
	unsigned neg = 0;

	if (mdB < 0) {
		neg = 1;
		mdB *= -1;
	}

	while (mdB > 0) {
		if (mdB >= 20000) {
			mdB -= 20000;
			COMPUTE(10, 1); /* 10^(20/20) */
			continue;
		}
		if (mdB >= 6000) {
			mdB -= 6000;
			COMPUTE(199526, 100000); /* 10^(6/20) */
			continue;
		}
		if (mdB >= 1000) {
			mdB -= 1000;
			COMPUTE(112202, 100000); /* 10^(1/20) */
			continue;
		}
		if (mdB >= 100) {
			mdB -= 100;
			COMPUTE(101158, 100000); /* 10^(0.1/20) */
			continue;
		}
		if (mdB >= 10) {
			mdB -= 10;
			COMPUTE(100115, 100000); /* 10^(0.01/20) */
			continue;
		}
		if (mdB >= 1) {
			mdB -= 1;
			COMPUTE(100012, 100000); /* 10^(0.001/20) */
			continue;
		}
	}

	return a;
}

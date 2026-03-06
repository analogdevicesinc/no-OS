// SPDX-License-Identifier: GPL-2.0
/**
 * Copyright 2026 Analog Devices Inc.
 * Released under the ADRV903X API license, for more information.
 * see the "LICENSE.txt" file in this zip file.
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adi_common_error.h"
#include "no_os_print_log.h"
#include "no_os_platform.h"
#include "adi_platform.h"
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

/******************************************************************************/
/************************** Logging Functions *********************************/
/******************************************************************************/

adi_hal_Err_e no_os_LogFileOpen(void *devHalCfg, const char *filename)
{
	return ADI_HAL_ERR_OK;
}

int32_t no_os_LogFileFlush(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_LogFileClose(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_LogStatusGet(void* const devHalCfg,
				 adi_hal_LogStatusGet_t* const logStatus)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_LogConsoleSet(void* const devHalCfg,
				  const adi_hal_LogConsole_e logConsoleFlag)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_LogLevelSet(void *devHalCfg, const uint32_t logMask)
{
	struct adrv903x_hal_cfg *halCfg = NULL;

	if (devHalCfg == NULL)
		return ADI_HAL_ERR_NULL_PTR;

	halCfg = (struct adrv903x_hal_cfg *)devHalCfg;
	halCfg->logLevel = (logMask & (int32_t)ADI_HAL_LOG_ALL);

	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_LogLevelGet(void* const devHalCfg, uint32_t *logLevel)
{
	struct adrv903x_hal_cfg *halCfg = NULL;

	if (devHalCfg == NULL || logLevel == NULL)
		return ADI_HAL_ERR_NULL_PTR;

	halCfg = (struct adrv903x_hal_cfg *)devHalCfg;
	*logLevel = halCfg->logLevel;

	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_LogWrite(void* const devHalCfg,
			     const adi_hal_LogLevel_e logLevel,
			     const uint8_t indent,
			     const char* const comment,
			     va_list argp)
{
	int32_t halError = (int32_t)ADI_HAL_ERR_OK;
	int32_t result = 0;
	struct adrv903x_hal_cfg *halCfg = NULL;
	char logMessage[ADI_HAL_MAX_LOG_LINE] = { 0 };
	const char *logLevelChar = NULL;

	if (devHalCfg == NULL)
		return ADI_HAL_ERR_NULL_PTR;

	halCfg = (struct adrv903x_hal_cfg *)devHalCfg;

	if (halCfg->logLevel == (int32_t)ADI_HAL_LOG_NONE)
		return ADI_HAL_ERR_OK;

	if (logLevel > (int32_t)ADI_HAL_LOG_ALL)
		return (int32_t)6;

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
		return ADI_HAL_ERR_OK;
	}

	result = snprintf(logMessage, ADI_HAL_MAX_LOG_LINE, "%s", logLevelChar);
	if (result < 0)
		return (int32_t)5;

	result = vsnprintf(logMessage + strlen(logMessage),
			   ADI_HAL_MAX_LOG_LINE, comment, argp);
	if (result < 0)
		return (int32_t)5;

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

/******************************************************************************/
/**************************** SPI Functions ***********************************/
/******************************************************************************/

int32_t no_os_SpiOpen(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

int32_t no_os_SpiClose(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_SpiInit(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_SpiWrite(void *devHalCfg, const uint8_t txData[],
			     uint32_t numTxBytes)
{
	struct adrv903x_hal_cfg *halCfg = NULL;
	static const uint32_t MAX_SIZE = 4096;
	uint32_t remaining = numTxBytes;
	uint32_t toWrite = 0;
	int32_t result = 0;

	if (devHalCfg == NULL)
		return ADI_HAL_ERR_NULL_PTR;

	halCfg = (struct adrv903x_hal_cfg *)devHalCfg;

	do {
		toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
		result = no_os_spi_write_and_read(halCfg->spi,
						  (uint8_t *)&txData[numTxBytes - remaining],
						  toWrite);
		if (result < 0)
			return 2; /* ADI_HAL_SPI_FAIL */
		remaining -= toWrite;
	} while (remaining > 0);

	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_SpiRead(void *devHalCfg, const uint8_t txData[],
			    uint8_t rxData[], uint32_t numTxRxBytes)
{
	struct adrv903x_hal_cfg *halCfg = NULL;
	static const uint32_t MAX_SIZE = 4096;
	uint32_t remaining = numTxRxBytes;
	uint32_t toWrite = 0;
	int32_t result = 0;

	if (devHalCfg == NULL)
		return ADI_HAL_ERR_NULL_PTR;

	memcpy(rxData, txData, numTxRxBytes);
	halCfg = (struct adrv903x_hal_cfg *)devHalCfg;

	do {
		toWrite = (remaining > MAX_SIZE) ? MAX_SIZE : remaining;
		result = no_os_spi_write_and_read(halCfg->spi,
						  &rxData[numTxRxBytes - remaining],
						  toWrite);
		if (result < 0)
			return 2; /* ADI_HAL_SPI_FAIL */
		remaining -= toWrite;
	} while (remaining > 0);

	return ADI_HAL_ERR_OK;
}

/******************************************************************************/
/************************* Register Access Stubs ******************************/
/******************************************************************************/

adi_hal_Err_e no_os_RegisterWrite(void* const devHalCfg, const uint32_t addr,
				  const uint32_t data)
{
	return (int32_t)ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_RegisterRead(void* const devHalCfg, const uint32_t addr,
				 uint32_t* const data)
{
	return (int32_t)ADI_HAL_ERR_OK;
}

/******************************************************************************/
/*************************** Timer Functions **********************************/
/******************************************************************************/

int32_t no_os_TimerOpen(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

int32_t no_os_TimerClose(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

int32_t no_os_TimerInit(void *devHalCfg)
{
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_TimerWait_ms(void *devHalCfg, uint32_t time_ms)
{
	no_os_mdelay(time_ms);
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_TimerWait_us(void *devHalCfg, uint32_t time_us)
{
	no_os_udelay(time_us);
	return ADI_HAL_ERR_OK;
}

/******************************************************************************/
/*************************** Mutex Stubs **************************************/
/******************************************************************************/

adi_hal_Err_e no_os_mutex_init_wrapper(adi_hal_mutex_t* const mutex)
{
	(void)mutex;
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_mutex_lock_wrapper(adi_hal_mutex_t* const mutex)
{
	(void)mutex;
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_mutex_unlock_wrapper(adi_hal_mutex_t* const mutex)
{
	(void)mutex;
	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_mutex_remove_wrapper(adi_hal_mutex_t* const mutex)
{
	(void)mutex;
	return ADI_HAL_ERR_OK;
}

/******************************************************************************/
/************************ Hardware Open/Close/Reset ***************************/
/******************************************************************************/

adi_hal_Err_e no_os_HwOpen(void *devHalCfg)
{
	int32_t ret;
	struct adrv903x_hal_cfg *phal = (struct adrv903x_hal_cfg *)devHalCfg;
	struct no_os_gpio_init_param gip_gpio_reset_n = { 0 };
	struct no_os_spi_init_param sip = { 0 };

	if (!devHalCfg)
		return ADI_HAL_ERR_NULL_PTR;

	gip_gpio_reset_n.number = ADRV903X_RESET_B;
	gip_gpio_reset_n.extra = clkchip_gpio_init_param.extra;
	gip_gpio_reset_n.platform_ops = clkchip_gpio_init_param.platform_ops;
	ret = no_os_gpio_get(&phal->gpio_reset_n, &gip_gpio_reset_n);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_output(phal->gpio_reset_n, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_gpio;

	sip.device_id = SPI_DEVICE_ID;
	sip.max_speed_hz = 5000000u;
	sip.mode = NO_OS_SPI_MODE_0;
	sip.chip_select = ADRV903X_CS;
	sip.platform_ops = ad9528_spi_param.platform_ops;
	sip.extra = ad9528_spi_param.extra;

	ret = no_os_spi_init(&phal->spi, &sip);
	if (ret)
		goto error_gpio;

	ret = no_os_gpio_set_value(phal->gpio_reset_n, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_spi;

	return ADI_HAL_ERR_OK;

error_spi:
	no_os_spi_remove(phal->spi);
error_gpio:
	no_os_gpio_remove(phal->gpio_reset_n);
	return ret;
}

adi_hal_Err_e no_os_HwClose(void *devHalCfg)
{
	int32_t ret;
	struct adrv903x_hal_cfg *phal = (struct adrv903x_hal_cfg *)devHalCfg;

	if (!devHalCfg)
		return ADI_HAL_ERR_NULL_PTR;

	ret = no_os_gpio_remove(phal->gpio_reset_n);
	if (ret)
		return ret;

	ret = no_os_spi_remove(phal->spi);
	if (ret)
		return ret;

	return ADI_HAL_ERR_OK;
}

adi_hal_Err_e no_os_HwReset(void *devHalCfg, const uint8_t pinLevel)
{
	struct adrv903x_hal_cfg *phal;
	int ret;

	if (devHalCfg == NULL)
		return ADI_HAL_ERR_NULL_PTR;

	phal = (struct adrv903x_hal_cfg *)devHalCfg;
	ret = no_os_gpio_set_value(phal->gpio_reset_n, pinLevel);
	if (ret)
		return ret;

	return ADI_HAL_ERR_OK;
}

/******************************************************************************/
/****************** HAL Function Pointer Assignments **************************/
/******************************************************************************/

/* Initialization interface */
adi_hal_Err_e(*adi_hal_HwOpen)(void *devHalCfg) = no_os_HwOpen;
adi_hal_Err_e(*adi_hal_HwClose)(void *devHalCfg) = no_os_HwClose;
adi_hal_Err_e(*adi_hal_HwReset)(void *devHalCfg,
				const uint8_t pinLevel) = no_os_HwReset;
adi_hal_Err_e(*adi_hal_SpiInit)(void *devHalCfg) = no_os_SpiInit;
void *(*adi_hal_DevHalCfgCreate)(uint32_t interfaceMask, uint8_t spiChipSelect,
				 const char *logFilename) = NULL;
adi_hal_Err_e(*adi_hal_DevHalCfgFree)(void *devHalCfg) = NULL;

/* SPI Interface */
adi_hal_Err_e(*adi_hal_SpiWrite)(void *devHalCfg, const uint8_t txData[],
				 uint32_t numTxBytes) = no_os_SpiWrite;

adi_hal_Err_e(*adi_hal_SpiRead)(void *devHalCfg, const uint8_t txData[],
				uint8_t rxData[], uint32_t numRxBytes) = no_os_SpiRead;

adi_hal_Err_e(*adi_hal_BbicRegisterWrite)(void* const devHalCfg,
		const uint32_t addr, const uint32_t data) = no_os_RegisterWrite;

adi_hal_Err_e(*adi_hal_BbicRegisterRead)(void* const devHalCfg,
		const uint32_t addr, uint32_t* const data) = no_os_RegisterRead;

adi_hal_Err_e(*adi_hal_BbicRegistersRead)(void* const devHalCfg,
		const uint32_t addr,
		uint32_t data[],
		const uint32_t numDataWords) = NULL;

adi_hal_Err_e(*adi_hal_BbicRegistersWrite)(void* const devHalCfg,
		const uint32_t addr,
		const uint32_t data[],
		const uint32_t numDataWords) = NULL;

/* I2C (not used - NULL) */
adi_hal_Err_e(*adi_hal_I2cWrite)(void* const devHalCfg,
				 const uint8_t txData[],
				 const uint32_t numTxBytes) = NULL;

adi_hal_Err_e(*adi_hal_I2cRead)(void* const devHalCfg,
				const uint8_t txData[],
				const uint32_t numTxBytes,
				uint8_t rxData[],
				const uint32_t numRxBytes) = NULL;

/* Logging interface */
adi_hal_Err_e(*adi_hal_LogFileOpen)(void *devHalCfg,
				    const char *filename) = no_os_LogFileOpen;

adi_hal_Err_e(*adi_hal_LogLevelSet)(void *devHalCfg,
				    const uint32_t logMask) = no_os_LogLevelSet;

adi_hal_Err_e(*adi_hal_LogLevelGet)(void* const devHalCfg,
				    uint32_t* logMask) = no_os_LogLevelGet;

adi_hal_Err_e(*adi_hal_LogWrite)(void* const devHalCfg,
				 const adi_hal_LogLevel_e logLevel,
				 const uint8_t indent,
				 const char* const comment,
				 va_list argp) = no_os_LogWrite;

adi_hal_Err_e(*adi_hal_LogFileClose)(void *devHalCfg) = no_os_LogFileClose;

adi_hal_Err_e(*adi_hal_LogStatusGet)(void* const devHalCfg,
				     adi_hal_LogStatusGet_t* const logStatus) = no_os_LogStatusGet;

adi_hal_Err_e(*adi_hal_LogConsoleSet)(void* const devHalCfg,
				      const adi_hal_LogConsole_e logConsoleFlag) = no_os_LogConsoleSet;

/* Timer interface */
adi_hal_Err_e(*adi_hal_Wait_ms)(void *devHalCfg,
				uint32_t time_ms) = no_os_TimerWait_ms;

adi_hal_Err_e(*adi_hal_Wait_us)(void *devHalCfg,
				uint32_t time_us) = no_os_TimerWait_us;

/* Threads (stubs) */
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

adi_hal_Err_e(*adi_hal_TlsSet)(const adi_hal_TlsType_e tlsType,
			       void* const value) = no_os_hal_TlsSet;

/* Mutexes */
adi_hal_Err_e(*adi_hal_MutexInit)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_init_wrapper;

adi_hal_Err_e(*adi_hal_MutexLock)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_lock_wrapper;

adi_hal_Err_e(*adi_hal_MutexUnlock)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_unlock_wrapper;

adi_hal_Err_e(*adi_hal_MutexDestroy)(adi_hal_mutex_t* const mutex) =
	no_os_mutex_remove_wrapper;

/* Board identify (stub) */
adi_hal_Err_e(*adi_hal_BoardIdentify)(char** boardNames,
				      int32_t* numBoards) = NULL;

/******************************************************************************/
/*************************** File I/O Abstraction *****************************/
/*                                                                            */
/* Firmware and profile files are not yet embedded. fopen() returns NULL for  */
/* all filenames, which will cause the API to report an error if it tries to  */
/* load firmware. Full file I/O will be added in Step 2.                      */
/******************************************************************************/

long int ftell(FILE *stream)
{
	/* Placeholder - profile not loaded yet */
	return 0;
}

FILE* fopen(const char *filename, const char *mode)
{
	/* Firmware not yet embedded - return NULL */
	pr_warning("ADRV903X HAL: fopen(\"%s\") - firmware not embedded yet\n",
		   filename);
	return NULL;
}

int __wrap_fseek(FILE *stream, long int offset, int origin)
{
	return -EINVAL;
}

int __wrap_fclose(FILE *stream)
{
	if (stream == NULL)
		return -ENODEV;
	no_os_free(stream);
	return 0;
}

char *fgets(char *dst, int num, FILE *stream)
{
	return NULL;
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
	return 0;
}

int _gettimeofday(struct timeval *tv, void *tzvp)
{
	uint64_t t = 0;
	tv->tv_sec = t / 1000000000;
	tv->tv_usec = (t % 1000000000) / 1000;
	return 0;
}

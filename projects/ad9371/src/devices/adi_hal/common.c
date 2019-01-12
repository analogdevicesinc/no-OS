/**
 * \file common.c
 * \brief Contains Mykonos API common wrapper functions for user hardware platform drivers
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include "common.h"

#include "platform_drivers.h"

ADI_LOGLEVEL CMB_LOGLEVEL = ADIHAL_LOG_NONE;

static uint32_t _desired_time_to_elapse_us = 0;
extern spi_device spi_dev;

commonErr_t CMB_closeHardware(void)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_setGPIO(uint32_t GPIO)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_hardReset(uint8_t spiChipSelectIndex)
{
	uint32_t resetGPIO = 0;

	switch (spiChipSelectIndex) {
	case AD9371_CHIP_SELECT:
		resetGPIO = AD9371_RESET_B;
		break;
	case AD9528_CHIP_SELECT:
		resetGPIO = AD9528_RESET_B;
		break;
	default:
		return(COMMONERR_FAILED);
	}

	gpio_set_value(resetGPIO, 0x1);
	CMB_wait_ms(1);
	gpio_set_value(resetGPIO, 0x0);
	CMB_wait_ms(1);
	gpio_set_value(resetGPIO, 0x1);
	CMB_wait_ms(1);

	return(COMMONERR_OK);
}

commonErr_t CMB_setSPIOptions(spiSettings_t *spiSettings)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_setSPIChannel(uint16_t chipSelectIndex )
{
	return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteByte(spiSettings_t *spiSettings, uint16_t addr, uint8_t data)
{
	uint8_t buf[3];

	spi_dev.chip_select = spiSettings->chipSelectIndex;

	buf[0] = (uint8_t) ((addr >> 8) & 0x7f);
	buf[1] = (uint8_t) (addr & 0xff);
	buf[2] = (uint8_t) data;

	spi_write_and_read(&spi_dev, buf, 3);

	return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteBytes(spiSettings_t *spiSettings, uint16_t *addr, uint8_t *data, uint32_t count)
{
	uint32_t index;

	for (index = 0; index < count; index++)
		if (CMB_SPIWriteByte(spiSettings, *(addr + index), *(data + index)) != COMMONERR_OK)
			return(COMMONERR_FAILED);

	return(COMMONERR_OK);
}

commonErr_t CMB_SPIReadByte(spiSettings_t *spiSettings, uint16_t addr, uint8_t *readdata)
{
	uint8_t buf[3];

	spi_dev.chip_select = spiSettings->chipSelectIndex;

	buf[0] = (uint8_t) ((addr >> 8) | 0x80);
	buf[1] = (uint8_t) (addr & 0xff);
	buf[2] = (uint8_t) 0x00;

	spi_write_and_read(&spi_dev, buf, 3);
	*readdata = buf[2];

	return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteField(spiSettings_t *spiSettings, uint16_t addr, uint8_t field_val, uint8_t mask, uint8_t start_bit)
{
	uint8_t data;

	if (CMB_SPIReadByte(spiSettings, addr, &data) != COMMONERR_OK)
		return(COMMONERR_FAILED);

	data = (data & ~mask) | ((field_val << start_bit) & mask);

	if (CMB_SPIWriteByte(spiSettings, addr, data) != COMMONERR_OK)
		return(COMMONERR_FAILED);

	return(COMMONERR_OK);
}

/* read a field in a single register space (not multibyte fields) */
commonErr_t CMB_SPIReadField(spiSettings_t *spiSettings, uint16_t addr, uint8_t *field_val, uint8_t mask, uint8_t start_bit)
{
	uint8_t data;

	if (CMB_SPIReadByte(spiSettings, addr, &data) != COMMONERR_OK)
		return(COMMONERR_FAILED);

	data = (data & mask) >> start_bit;
	*field_val = data;

	return(COMMONERR_OK);
}

commonErr_t CMB_writeToLog(ADI_LOGLEVEL level, uint8_t deviceIndex, uint32_t errorCode, const char *comment){

	if((CMB_LOGLEVEL & ADIHAL_LOG_ERROR) && (level == ADIHAL_LOG_ERROR))
	{
		printf("ERROR: %d: %s", (int)errorCode, comment);
	}
	else if((CMB_LOGLEVEL & ADIHAL_LOG_WARNING) && (level == ADIHAL_LOG_WARNING))
	{
		printf("WARNING: %d: %s",(int)errorCode, comment);
	}
	else if((CMB_LOGLEVEL & ADIHAL_LOG_MESSAGE) && (level == ADIHAL_LOG_MESSAGE))
	{
		printf("MESSAGE: %d: %s",(int)errorCode, comment);
	}
	else if(CMB_LOGLEVEL == ADIHAL_LOG_NONE )
	{
		//printf("MESSAGE: %d: %s",(int)errorCode, comment);
	}
	else
	{
		printf("Undefined Log Level: 0x%X", level);
	}

	return(COMMONERR_OK);
}

/* if filename null, a default path will be used in logging.c */
commonErr_t CMB_openLog(const char *filename)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_closeLog(void)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_flushLog(void)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_wait_ms(uint32_t time_ms)
{
	mdelay(time_ms);

	return(COMMONERR_OK);
}

commonErr_t CMB_wait_us(uint32_t time_us)
{
	udelay(time_us);

	return(COMMONERR_OK);
}

commonErr_t CMB_setTimeout_ms(uint32_t timeOut_ms)
{
#ifdef _XPARAMETERS_PS_H_
	_desired_time_to_elapse_us = timeOut_ms * 1000;
#else
	_desired_time_to_elapse_us = timeOut_ms * 50;	// FIXME
#endif

	return(COMMONERR_OK);
}

commonErr_t CMB_setTimeout_us(uint32_t timeOut_us)
{
	_desired_time_to_elapse_us = timeOut_us;

	return(COMMONERR_OK);
}

commonErr_t CMB_hasTimeoutExpired()
{
	udelay(1);

	_desired_time_to_elapse_us--;
	if (_desired_time_to_elapse_us > 0)
		return(COMMONERR_OK);

	return(COMMONERR_FAILED);
}

commonErr_t CMB_regRead(uint32_t offset, uint32_t *data)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_regWrite(uint32_t offset, uint32_t data)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_memRead(uint32_t offset, uint32_t *data, uint32_t len)
{
	return(COMMONERR_OK);
}

commonErr_t CMB_memWrite(uint32_t offset, uint32_t *data, uint32_t len)
{
	return(COMMONERR_OK);
}

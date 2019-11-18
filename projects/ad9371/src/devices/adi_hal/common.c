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
#include "spi.h"
#include "spi_extra.h"
#include "gpio.h"
#include "delay.h"

#include "parameters.h"

#ifndef ALTERA_PLATFORM
#include <xparameters.h>
#include "xilinx_platform_drivers.h"
#else
#include "altera_platform_drivers.h"
#endif

ADI_LOGLEVEL CMB_LOGLEVEL = ADIHAL_LOG_NONE;

static uint32_t _desired_time_to_elapse_us = 0;
struct spi_desc 	*spi_ad_desc;
struct gpio_desc	*gpio_ad9371_resetb;
struct gpio_desc	*gpio_ad9528_resetb;
struct gpio_desc	*gpio_ad9528_sysref_req;

int32_t platform_init(void)
{
	struct spi_init_param spi_param;
	int32_t status = 0;

	status = gpio_get(&gpio_ad9371_resetb, AD9371_RESET_B);
	status = gpio_get(&gpio_ad9528_resetb, AD9528_RESET_B);
	status = gpio_get(&gpio_ad9528_sysref_req, AD9528_SYSREF_REQ);

#ifndef ALTERA_PLATFORM
	struct xil_spi_init_param xilinx_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = SPI_DEVICE_ID,
		.flags = SPI_CS_DECODE
	};
	spi_param.extra = &xilinx_spi_param;
#else
	struct altera_spi_init_param altera_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.type = NIOS_II_SPI,
		.base_address = SPI_BASEADDR
	};
	spi_param.extra = &altera_spi_param;
#endif
	spi_param.mode = SPI_MODE_0;
	spi_param.chip_select = AD9371_CS;
	status |= spi_init(&spi_ad_desc, &spi_param);

	return status;
}

int32_t platform_remove(void)
{
	int32_t status;

	status = gpio_remove(gpio_ad9371_resetb);
	status |= gpio_remove(gpio_ad9528_resetb);
	status |= gpio_remove(gpio_ad9528_sysref_req);

	status |= spi_remove(spi_ad_desc);

	return status;
}

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
	struct gpio_desc *reset_gpio;

	switch (spiChipSelectIndex) {
	case AD9371_CS:
		reset_gpio = gpio_ad9371_resetb;
		break;
	case AD9528_CS:
		reset_gpio = gpio_ad9528_resetb;
		break;
	default:
		return(COMMONERR_FAILED);
	}

	gpio_direction_output(reset_gpio, 1);
	CMB_wait_ms(1);
	gpio_direction_output(reset_gpio, 0);
	CMB_wait_ms(1);
	gpio_direction_output(reset_gpio, 1);
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

commonErr_t CMB_SPIWriteByte(spiSettings_t *spiSettings, uint16_t addr,
			     uint8_t data)
{
	uint8_t buf[3];

	spi_ad_desc->chip_select = spiSettings->chipSelectIndex - 1;

	buf[0] = (uint8_t) ((addr >> 8) & 0x7f);
	buf[1] = (uint8_t) (addr & 0xff);
	buf[2] = (uint8_t) data;

	spi_write_and_read(spi_ad_desc, buf, 3);

	return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteBytes(spiSettings_t *spiSettings, uint16_t *addr,
			      uint8_t *data, uint32_t count)
{
	uint32_t index;

	for (index = 0; index < count; index++)
		if (CMB_SPIWriteByte(spiSettings, *(addr + index),
				     *(data + index)) != COMMONERR_OK)
			return(COMMONERR_FAILED);

	return(COMMONERR_OK);
}

commonErr_t CMB_SPIReadByte(spiSettings_t *spiSettings, uint16_t addr,
			    uint8_t *readdata)
{
	uint8_t buf[3];

	spi_ad_desc->chip_select = spiSettings->chipSelectIndex - 1;

	buf[0] = (uint8_t) ((addr >> 8) | 0x80);
	buf[1] = (uint8_t) (addr & 0xff);
	buf[2] = (uint8_t) 0x00;

	spi_write_and_read(spi_ad_desc, buf, 3);
	*readdata = buf[2];

	return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteField(spiSettings_t *spiSettings, uint16_t addr,
			      uint8_t field_val, uint8_t mask, uint8_t start_bit)
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
commonErr_t CMB_SPIReadField(spiSettings_t *spiSettings, uint16_t addr,
			     uint8_t *field_val, uint8_t mask, uint8_t start_bit)
{
	uint8_t data;

	if (CMB_SPIReadByte(spiSettings, addr, &data) != COMMONERR_OK)
		return(COMMONERR_FAILED);

	data = (data & mask) >> start_bit;
	*field_val = data;

	return(COMMONERR_OK);
}

commonErr_t CMB_writeToLog(ADI_LOGLEVEL level, uint8_t deviceIndex,
			   uint32_t errorCode, const char *comment)
{

	if((CMB_LOGLEVEL & ADIHAL_LOG_ERROR) && (level == ADIHAL_LOG_ERROR)) {
		printf("ERROR: %d: %s", (int)errorCode, comment);
	} else if((CMB_LOGLEVEL & ADIHAL_LOG_WARNING)
		  && (level == ADIHAL_LOG_WARNING)) {
		printf("WARNING: %d: %s",(int)errorCode, comment);
	} else if((CMB_LOGLEVEL & ADIHAL_LOG_MESSAGE)
		  && (level == ADIHAL_LOG_MESSAGE)) {
		printf("MESSAGE: %d: %s",(int)errorCode, comment);
	} else if(CMB_LOGLEVEL == ADIHAL_LOG_NONE ) {
		//printf("MESSAGE: %d: %s",(int)errorCode, comment);
	} else {
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

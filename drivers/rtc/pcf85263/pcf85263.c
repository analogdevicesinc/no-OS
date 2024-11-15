/***************************************************************************//**
 *   @file   pcf85263.c
 *   @brief  Implementation of PCF85263 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include "pcf85263.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int pcf85263_read(struct pcf85263_dev *dev, uint8_t reg_addr, uint8_t *reg_data)
{
	int ret;

	ret = no_os_i2c_write(dev->i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, reg_data, 1, 1);
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int pcf85263_write(struct pcf85263_dev *dev, uint8_t reg_addr, uint8_t reg_data)
{
	uint8_t buff[2] = { 0 };

	buff[0] = reg_addr;
	buff[1] = reg_data;

	return no_os_i2c_write(dev->i2c_desc, buff, 2, 1);
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int pcf85263_update_bits(struct pcf85263_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data)
{
	int ret;
	uint32_t data;

	ret = pcf85263_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data;

	return pcf85263_write(dev, reg_addr, data);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int pcf85263_init(struct pcf85263_dev **device,
		  struct pcf85263_init_param init_param)
{
	struct pcf85263_dev *dev;
	int ret;

	dev = (struct pcf85263_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* I2C Initialization*/
	ret = no_os_i2c_init(&dev->i2c_desc, init_param.i2c_init);
	if (ret)
		goto error_dev;

	dev->battery_en = init_param.battery_en;

	ret = pcf85263_update_bits(dev, PCF85263_REG_BATTERY_SWITCH,
				   PCF85263_BATTERY_SW_MSK,
				   no_os_field_prep(PCF85263_BATTERY_SW_MSK, dev->battery_en));
	if (ret)
		goto error_i2c;

	*device = dev;

	return 0;

error_i2c:
	no_os_i2c_remove(dev->i2c_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Set date.
 * @param dev - The device structure.
 * @param date - Structure holding the date to be set.
 * @return 0 in case of success, negative error code otherwise.
 */
int pcf85263_set_date(struct pcf85263_dev *dev, struct pcf85263_date date)
{
	int ret;

	ret = pcf85263_write(dev, PCF85263_REG_STOP_ENABLE, 0x1);
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_RESETS, PCF85263_CPR);
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_100TH_SECONDS, 0x00);
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_SECONDS, no_os_bin2bcd(date.sec));
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_MINUTES, no_os_bin2bcd(date.min));
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_HOURS, no_os_bin2bcd(date.hr));
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_DAYS, no_os_bin2bcd(date.day));
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_MONTHS, no_os_bin2bcd(date.mon));
	if (ret)
		return ret;

	ret = pcf85263_write(dev, PCF85263_REG_YEARS, no_os_bin2bcd(date.year));
	if (ret)
		return ret;

	return pcf85263_write(dev, PCF85263_REG_STOP_ENABLE, 0x0);
}

/**
 * @brief Read time stamp.
 * @param dev - The device structure.
 * @param ts - Structure holding the time stamp read.
 * @return 0 in case of success, negative error code otherwise.
 */
int pcf85263_read_ts(struct pcf85263_dev *dev, struct pcf85263_date *ts)
{
	int ret;

	ret = pcf85263_read(dev, PCF85263_REG_SECONDS, &ts->sec);
	if (ret)
		return ret;

	ts->sec = no_os_bcd2bin(ts->sec & 0x7f);

	ret = pcf85263_read(dev, PCF85263_REG_MINUTES, &ts->min);
	if (ret)
		return ret;

	ts->min = no_os_bcd2bin(ts->min & 0x7f);

	ret = pcf85263_read(dev, PCF85263_REG_HOURS, &ts->hr);
	if (ret)
		return ret;

	ts->hr = no_os_bcd2bin((ts->hr) & 0x3f);

	ret = pcf85263_read(dev, PCF85263_REG_DAYS, &ts->day);
	if (ret)
		return ret;

	ts->day = no_os_bcd2bin((ts->day) & 0x3f);

	ret = pcf85263_read(dev, PCF85263_REG_MONTHS, &ts->mon);
	if (ret)
		return ret;

	ts->mon = no_os_bcd2bin((ts->mon) & 0x1f);

	ret = pcf85263_read(dev, PCF85263_REG_YEARS, &ts->year);
	if (ret)
		return ret;

	ts->year = no_os_bcd2bin(ts->year);

	return 0;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int pcf85263_remove(struct pcf85263_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

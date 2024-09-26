/*******************************************************************************
 *   @file   max31343.c
 *   @brief  Max31343 Real Time Clock functionalities
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "max31343.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int max31343_reg_read(struct max31343_dev *dev, uint8_t reg_addr,
		      uint8_t *reg_data)
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
int max31343_reg_write(struct max31343_dev *dev, uint8_t reg_addr,
		       uint8_t reg_data)
{
	uint8_t buff[2];

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
int max31343_update_bits(struct max31343_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data)
{
	int ret;
	uint32_t data;

	ret = max31343_reg_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data;

	return max31343_reg_write(dev, reg_addr, data);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int max31343_init(struct max31343_dev **device,
		  struct max31343_init_param init_param)
{
	struct max31343_dev *dev;
	int ret;

	dev = (struct max31343_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;
	*device = dev;

	/* I2C Initialization */
	ret = no_os_i2c_init(&dev->i2c_desc, init_param.i2c_init);
	if (ret)
		goto error_dev;

	dev->battery_en = init_param.battery_en;

	/* Reset GPIO pin deassert */
	ret = max31343_update_bits(dev, MAX31343_R_RTC_RESET,
				   MAX31343_F_RTC_RESET_SWRST, 0);
	if (ret)
		goto error;

	/* Enable RTC */
	ret = max31343_update_bits(dev, MAX31343_R_CFG1, MAX31343_F_CFG1_ENOSC,
				   no_os_field_prep(MAX31343_F_CFG1_ENOSC, 1));
	if (ret)
		goto error;

	/* Disable IRQs */
	ret = max31343_reg_write(dev, MAX31343_R_INT_EN, 0);
	if (ret)
		goto error;

	return 0;

error:
	no_os_i2c_remove(dev->i2c_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Set time stamp.
 * @param dev - The device structure.
 * @param ts - Structure holding the time stamp to be set.
 * @return 0 in case of success, negative error code otherwise.
 */
int max31343_set_time_stamp(struct max31343_dev *dev,
			    struct max31343_time_stamp ts)
{
	uint8_t data;
	int ret;
	uint8_t century = (ts.year >= 2100) ? 1 : 0;

	data = no_os_bin2bcd(ts.sec);
	ret = max31343_reg_write(dev, MAX31343_R_SECONDS, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.min);
	ret = max31343_reg_write(dev, MAX31343_R_MINUTES, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.hr);
	ret = max31343_reg_write(dev, MAX31343_R_HOURS, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.day);
	ret = max31343_reg_write(dev, MAX31343_R_DATE, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.mon);
	if (century)
		data |= NO_OS_BIT(7);
	ret = max31343_reg_write(dev, MAX31343_R_MONTH, data);
	if (ret)
		return ret;

	data = no_os_bin2bcd(ts.year);
	if (century)
		data -= 2100;
	else
		data -= 2000;

	ret = max31343_reg_write(dev, MAX31343_R_YEAR, data);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read time stamp.
 * @param dev - The device structure.
 * @param ts - Structure holding the time stamp to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int max31343_reg_read_time_stamp(struct max31343_dev *dev,
				 struct max31343_time_stamp *ts)
{
	uint8_t data;
	int ret;
	uint8_t century = 0;

	ret = max31343_reg_read(dev, MAX31343_R_SECONDS, &data);
	if (ret)
		return ret;
	ts->sec = no_os_bcd2bin(data);

	ret = max31343_reg_read(dev, MAX31343_R_MINUTES, &data);
	if (ret)
		return ret;
	ts->min = no_os_bcd2bin(data);

	ret = max31343_reg_read(dev, MAX31343_R_HOURS, &data);
	if(ret)
		return ret;
	ts->hr = no_os_bcd2bin(data);

	ret = max31343_reg_read(dev, MAX31343_R_DATE, &data);
	if (ret)
		return(ret);
	ts->day = no_os_bcd2bin(data);

	ret = max31343_reg_read(dev, MAX31343_R_MONTH, &data);
	if (ret)
		return ret;

	/* 7th bit stores if current year is century */
	century = data & NO_OS_BIT(7);
	data &= ~NO_OS_BIT(7);
	ts->mon = no_os_bcd2bin(data);

	ret = max31343_reg_read(dev, MAX31343_R_YEAR, &data);
	if (ret)
		return ret;
	if(century)
		ts->year = no_os_bcd2bin(data) + 2100;
	else
		ts->year = no_os_bcd2bin(data) + 2000;

	return 0;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int max31343_remove(struct max31343_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

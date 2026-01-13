/**
 *   @file   ltm3360b.c
 *   @brief  Source code of the LTM3360B driver
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"
#include "no_os_error.h"
#include "ltm3360b.h"

/**
 * @brief Read register from LTM3360B device
 * @param dev - LTM3360B device structure
 * @param reg - Register address
 * @param data - Pointer to store read data
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_reg_read(struct ltm3360b_dev *dev, uint8_t reg, uint8_t *data)
{
	int ret;

	if (!dev || !data)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg, 1, 1);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, data, 1, 1);
}

/**
 * @brief Write register to LTM3360B device
 * @param dev - LTM3360B device structure
 * @param reg - Register address
 * @param data - Data to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_reg_write(struct ltm3360b_dev *dev, uint8_t reg, uint8_t data)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = reg;
	buf[1] = data;

	return no_os_i2c_write(dev->i2c_desc, buf, sizeof(buf), 1);
}

/**
 * @brief Update register bits using mask - modifies only specified bits
 * @param dev - LTM3360B device structure
 * @param reg - Register address
 * @param mask - Bit mask for bits to modify
 * @param val - New value for masked bits
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_reg_update(struct ltm3360b_dev *dev, uint8_t reg,
			uint8_t mask, uint8_t val)
{
	uint8_t reg_val;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltm3360b_reg_read(dev, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= (val & mask);

	return ltm3360b_reg_write(dev, reg, reg_val);
}

/**
 * @brief Enable or disable LTM3360B device
 * @param dev - LTM3360B device structure
 * @param enable - true to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_enable_device(struct ltm3360b_dev *dev, bool enable)
{
	uint8_t mask, val;

	if (!dev)
		return -EINVAL;

	if (enable) {
		mask = LTM3360B_EN_I2C_BIT | LTM3360B_IGNORE_EN_IN_BIT;
		val = LTM3360B_EN_I2C_BIT | LTM3360B_IGNORE_EN_IN_BIT;
	} else {
		mask = LTM3360B_EN_I2C_BIT;
		val = 0;
	}

	return ltm3360b_reg_update(dev, LTM3360B_EN_IMON_CTRL, mask, val);
}

/**
 * @brief Set output voltage
 * @param dev - LTM3360B device structure
 * @param vout_mv - Output voltage in millivolts.
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_set_vout_mv(struct ltm3360b_dev *dev, uint16_t vout_mv)
{
	uint8_t vout_code;

	if (!dev)
		return -EINVAL;

	if (vout_mv < LTM3360B_VOUT_MIN_MV || vout_mv > LTM3360B_VOUT_MAX_MV)
		return -EINVAL;

	vout_code = LTM3360B_VOUT_MV_TO_CODE(vout_mv);

	return ltm3360b_reg_write(dev, LTM3360B_VOUT_SETTING, vout_code);
}

/**
 * @brief Get current output voltage setting
 * @param dev - LTM3360B device structure
 * @param vout_mv - Pointer to store output voltage in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_get_vout_mv(struct ltm3360b_dev *dev, uint16_t *vout_mv)
{
	uint8_t vout_code;
	int ret;

	if (!dev || !vout_mv)
		return -EINVAL;

	ret = ltm3360b_reg_read(dev, LTM3360B_VOUT_SETTING, &vout_code);
	if (ret)
		return ret;

	*vout_mv = LTM3360B_VOUT_CODE_TO_MV(vout_code);

	return 0;
}

/**
 * @brief Set IMON threshold for overcurrent detection
 * @param dev - LTM3360B device structure
 * @param threshold - IMON threshold setting
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_set_imon_threshold(struct ltm3360b_dev *dev,
				enum ltm3360b_imon_threshold threshold)
{
	if (!dev || (threshold > LTM3360B_IMON_TH_69_PERCENT))
		return -EINVAL;

	return ltm3360b_reg_update(dev, LTM3360B_EN_IMON_CTRL,
				   LTM3360B_OPT_IMON_TH_MSK,
				   LTM3360B_OPT_IMON_TH(threshold));
}

/**
 * @brief Enable or disable ADC
 * @param dev - LTM3360B device structure
 * @param enable - true to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_adc_enable(struct ltm3360b_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return ltm3360b_reg_write(dev, LTM3360B_ADC_CTRL,
				  enable ? LTM3360B_EN_ADC_BIT : 0);
}

/**
 * @brief Read raw ADC value
 * @param dev - LTM3360B device structure
 * @param channel - ADC channel to read
 * @param result - Pointer to store ADC result
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_adc_read_raw(struct ltm3360b_dev *dev,
			  enum ltm3360b_adc_channel channel, uint8_t *result)
{
	int ret;

	if (!dev || !result || (channel > LTM3360B_ADC_VIN))
		return -EINVAL;

	ret = ltm3360b_reg_write(dev, LTM3360B_ADC_SELECT,
				 LTM3360B_ADC_SEL_IN(channel));
	if (ret)
		return ret;

	no_os_mdelay(LTM3360B_ADC_CONV_TIME_MS);

	return ltm3360b_reg_read(dev, LTM3360B_ADC_OUTPUT, result);
}

/**
 * @brief Initialize LTM3360B device
 * @param device - Pointer to the device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_init(struct ltm3360b_dev **device,
		  struct ltm3360b_init_param *init_param)
{
	struct ltm3360b_dev *dev;
	int ret;

	if (!device || !init_param || !init_param->i2c_init)
		return -EINVAL;

	dev = (struct ltm3360b_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto error_free;

	dev->auto_enable = init_param->auto_enable;

	ret = ltm3360b_set_imon_threshold(dev, init_param->imon_threshold);
	if (ret)
		goto error_i2c;

	if (init_param->vout_mv > 0) {
		ret = ltm3360b_set_vout_mv(dev, init_param->vout_mv);
		if (ret)
			goto error_i2c;
	}

	if (init_param->auto_enable) {
		ret = ltm3360b_enable_device(dev, true);
		if (ret)
			goto error_i2c;
	}

	*device = dev;
	return 0;

error_i2c:
	no_os_i2c_remove(dev->i2c_desc);
error_free:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Remove LTM3360B device instance
 * @param dev - LTM3360B device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm3360b_remove(struct ltm3360b_dev *dev)
{
	if (!dev)
		return -EINVAL;

	/* Disable the device before cleanup */
	ltm3360b_enable_device(dev, false);

	no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return 0;
}

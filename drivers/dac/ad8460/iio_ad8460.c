/***************************************************************************//**
 *   @file   iio_ad8460.c
 *   @brief  Implementation of IIO AD8460 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <errno.h>
#include "iio_ad8460.h"
#include "ad8460.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define AD8460_CHAN_EXT_INFO(_name, _what, _read, _write) {	\
	.name = (_name),					\
	.show = (int (*)()) (_read),				\
	.store = (int (*)()) (_write),				\
	.priv = (_what),					\
	.shared = IIO_SEPARATE,					\
}

#define AD8460_VOLTAGE_CHAN {					\
	.ch_type = IIO_VOLTAGE,					\
	.ch_out = 1,						\
	.indexed = 1,						\
	.channel = 0,						\
	.scan_index = 0,					\
	.scan_type = &ad8460_iio_scan_type,			\
	.attributes = ad8460_voltage_attrs,			\
}

#define AD8460_CURRENT_CHAN {					\
	.ch_type = IIO_CURRENT,					\
	.ch_out = 1,						\
	.indexed = 1,						\
	.channel = 0,						\
	.scan_index = -1,					\
	.attributes = ad8460_current_attrs,			\
}

static int ad8460_iio_reg_read(struct ad8460_iio_device *dev, uint32_t reg,
			       uint32_t *readval)
{
	return ad8460_reg_read(dev->dev, (uint8_t)reg, (uint8_t*)readval);
}

static int ad8460_iio_reg_write(struct ad8460_iio_device *dev, uint32_t reg,
				uint32_t writeval)
{
	return ad8460_reg_write(dev->dev, (uint8_t)reg, (uint8_t)writeval);
}

static int ad8460_iio_reg_update_bits(struct ad8460_iio_device *dev,
				      uint32_t reg, uint32_t mask, uint32_t val)
{
	return ad8460_reg_update_bits(dev->dev, (uint8_t)reg, (uint8_t)mask,
				      (uint8_t)val);
}

static int ad8460_buffer_preenable(void *dev, uint32_t mask)
{
	struct ad8460_iio_device *iio_dev = dev;

	return ad8460_enable_apg_mode(iio_dev->dev, 0);
}

static int ad8460_buffer_postdisable(void *dev)
{
	struct ad8460_iio_device *iio_dev = dev;

	return ad8460_enable_apg_mode(iio_dev->dev, 1);
}

static int ad8460_read_raw(void *ddev, char *buf, uint32_t len,
			   const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint32_t uval32;
	uint16_t uval16;
	int32_t val;
	int ret;

	switch (channel->type) {
	case IIO_VOLTAGE:
		ret = ad8460_get_hvdac_word(dev->dev, priv, &uval16);
		if (ret)
			return ret;

		val = uval16;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case IIO_CURRENT:
		ret = ad8460_iio_reg_read(dev, 0x04, &uval32);
		if (ret)
			return ret;

		val = uval32;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

static int ad8460_write_raw(void *ddev, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint16_t data;
	int32_t val;
	int ret;

	switch (channel->type) {
	case IIO_VOLTAGE:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		data = val;

		return ad8460_set_sample(dev->dev, data);
	case IIO_CURRENT:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		data = no_os_field_prep(AD8460_QUIESCENT_CURRENT_MSK, val);

		return ad8460_iio_reg_write(dev, 0x04, data);
	default:
		return -EINVAL;
	}
}

static int ad8460_read_scale(void *ddev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	int32_t val[2];

	/*
	* vCONV = vNOMINAL_SPAN * (DAC_CODE / 2**14) - 40V
	* vMAX  = vNOMINAL_SPAN * (2**14 / 2**14) - 40V
	* vMIN  = vNOMINAL_SPAN * (0 / 2**14) - 40V
	* vADJ  = vCONV * (2000 / rSET) * (vREF / 1.2)
	* vSPAN = vADJ_MAX - vADJ_MIN
	* See datasheet page 49, section FULL-SCALE REDUCTION
	*/
	val[0] = AD8460_NOMINAL_VOLTAGE_SPAN * 2000 * dev->dev->refio_1p2v_mv;
	val[1] = dev->dev->ext_resistor_ohms * 1200;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, val);
}

static int ad8460_dac_input_read(void *ddev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint16_t reg;
	int32_t val;
	int ret;

	ret = ad8460_get_hvdac_word(dev->dev, priv, &reg);
	if (ret)
		return ret;

	val = reg;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int ad8460_dac_input_write(void *ddev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint16_t reg;
	int32_t val;
	int ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	reg = val;

	return ad8460_set_hvdac_word(dev->dev, priv, reg);
}

static int ad8460_read_toggle_en(void *ddev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint32_t reg;
	int32_t val;
	int ret;

	ret = ad8460_iio_reg_read(dev, 0x02, &reg);
	if (ret)
		return ret;

	val = no_os_field_get(AD8460_APG_MODE_ENABLE_MSK, reg);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int ad8460_write_toggle_en(void *ddev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	bool toggle_en;
	int32_t val;
	int ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	toggle_en = !!val;

	return ad8460_enable_apg_mode(dev->dev, toggle_en);
}

static int ad8460_read_symbol(void *ddev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint32_t reg;
	int32_t val;
	int ret;

	ret = ad8460_iio_reg_read(dev, 0x02, &reg);
	if (ret)
		return ret;

	val = no_os_field_get(AD8460_PATTERN_DEPTH_MSK, reg);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int ad8460_write_symbol(void *ddev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint8_t sym;
	int32_t val;
	int ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	sym = val;

	return ad8460_iio_reg_update_bits(dev, 0x02, AD8460_PATTERN_DEPTH_MSK, sym);
}

static int ad8460_read_powerdown(void *ddev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	uint32_t reg;
	int32_t val;
	int ret;

	ret = ad8460_iio_reg_read(dev, 0x01, &reg);
	if (ret)
		return ret;

	val = no_os_field_get(AD8460_HVDAC_SLEEP_MSK, reg);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int ad8460_write_powerdown(void *ddev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad8460_iio_device *dev = ddev;
	bool pwr_down;
	uint8_t sdn_flag;
	int32_t val;
	int ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	pwr_down = !!val;

	/*
	 * If powerdown is set, HVDAC is enabled and the HV driver is
	 * enabled via HV_RESET in case it is in shutdown mode,
	 * If powerdown is cleared, HVDAC is set to shutdown state
	 * as well as the HV driver. Quiescent current decreases and ouput is
	 * floating (high impedance).
	 */

	ret = ad8460_iio_reg_update_bits(dev, 0x01, AD8460_HVDAC_SLEEP_MSK, pwr_down);
	if (ret)
		return ret;

	if (!pwr_down) {
		ret = ad8460_read_shutdown_flag(dev->dev, &sdn_flag);
		if (ret)
			return ret;

		if (sdn_flag) {
			ret = ad8460_hv_reset(dev->dev);
			if (ret)
				return ret;
		}
	}

	return ad8460_iio_reg_update_bits(dev, 0x00, AD8460_HV_SLEEP_MSK, !pwr_down);
}

static struct iio_attribute ad8460_voltage_attrs[] = {
	AD8460_CHAN_EXT_INFO("raw", 0, ad8460_read_raw, ad8460_write_raw),
	AD8460_CHAN_EXT_INFO("scale", 0, ad8460_read_scale, NULL),
	AD8460_CHAN_EXT_INFO("raw0", 0, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw1", 1, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw2", 2, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw3", 3, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw4", 4, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw5", 5, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw6", 6, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw7", 7, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw8", 8, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw9", 9, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw10", 10, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw11", 11, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw12", 12, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw13", 13, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw14", 14, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("raw15", 15, ad8460_dac_input_read,
			     ad8460_dac_input_write),
	AD8460_CHAN_EXT_INFO("toggle_en", 0, ad8460_read_toggle_en,
			     ad8460_write_toggle_en),
	AD8460_CHAN_EXT_INFO("symbol", 0, ad8460_read_symbol,
			     ad8460_write_symbol),
	AD8460_CHAN_EXT_INFO("powerdown", 0, ad8460_read_powerdown,
			     ad8460_write_powerdown),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad8460_current_attrs[] = {
	AD8460_CHAN_EXT_INFO("raw", 0, ad8460_read_raw, ad8460_write_raw),
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad8460_iio_scan_type = {
	.sign = 'u',
	.realbits = 14,
	.storagebits = 16,
};

static struct iio_channel ad8460_channels[] = {
	AD8460_VOLTAGE_CHAN,
	AD8460_CURRENT_CHAN,
};

static struct iio_device ad8460_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ad8460_channels),
	.channels = ad8460_channels,
	.debug_reg_read = (int32_t (*)()) ad8460_iio_reg_read,
	.debug_reg_write = (int32_t (*)()) ad8460_iio_reg_write,
	.pre_enable = (int32_t (*)()) ad8460_buffer_preenable,
	.post_disable = (int32_t (*)()) ad8460_buffer_postdisable,
};

/**
 * @brief Initializes the AD8460 IIO driver
 * @param iio_device - The iio device structure.
 * @param iio_init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int ad8460_iio_init(struct ad8460_iio_device **iio_device,
		    struct ad8460_iio_init_param *iio_init_param)
{
	struct ad8460_iio_device *iio_device_temp;
	int ret;

	if (!iio_init_param || !iio_init_param->init_param)
		return -EINVAL;

	iio_device_temp = no_os_calloc(1, sizeof(*iio_device_temp));
	if (!iio_device_temp)
		return -ENOMEM;

	ret = ad8460_init(&iio_device_temp->dev, iio_init_param->init_param);
	if (ret) {
		no_os_free(iio_device_temp);
		return ret;
	}

	iio_device_temp->iio_dev = &ad8460_iio_dev;

	ret = ad8460_reset(iio_device_temp->dev);

	/* Enables DAC by default */
	ret = ad8460_iio_reg_update_bits(iio_device_temp, 0x01, AD8460_HVDAC_SLEEP_MSK,
					 0);
	if (ret)
		return ret;

	*iio_device = iio_device_temp;

	return 0;
}

/**
 * @brief Free resources allocated by the init function
 * @param iio_device - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int ad8460_iio_remove(struct ad8460_iio_device *iio_device)
{
	int ret;

	ret = ad8460_remove(iio_device->dev);
	if (ret)
		return ret;

	no_os_free(iio_device);

	return 0;
}

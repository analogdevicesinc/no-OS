/***************************************************************************//**
 *   @file   ad2s1210.c
 *   @brief  Source file for the ad2s1210 driver
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 * Copyright (c) 2023 BayLibre, SAS.
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
#include <stdlib.h>
#include "ad2s1210.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

static int ad2s1210_set_mode_pins(struct ad2s1210_dev *dev,
				  enum ad2s1210_mode mode);

/*******************************************************************************
* @brief Write to device register
*
* @param dev - Device descriptor.
* @param addr - Address of the register to write
* @param val - value to write
*
* @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int ad2s1210_reg_write(struct ad2s1210_dev *dev, uint8_t addr, uint8_t val)
{
	uint8_t buf;
	int ret;

	if (addr < AD2S1210_REG_MIN)
		return -EINVAL;

	ret = ad2s1210_set_mode_pins(dev, MODE_CONFIG);
	if (ret)
		return ret;

	buf = addr;
	ret = no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
	if (ret)
		return ret;

	buf = val;
	return no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
}

/*******************************************************************************
* @brief Read a device register
*
* @param dev - Device descriptor.
* @param addr - Address of the register to read
* @param val - pointer to store register value
*
* @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int ad2s1210_reg_read(struct ad2s1210_dev *dev, uint8_t addr, uint8_t *val)
{
	uint8_t buf;
	int32_t ret;

	if (addr < AD2S1210_REG_MIN)
		return -EINVAL;

	ret = ad2s1210_set_mode_pins(dev, MODE_CONFIG);
	if (ret)
		return ret;

	buf = addr;
	ret = no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
	if (ret)
		return ret;

	/*
	 * While 2s1210 will present data of the previous cycle on the SDO pins
	 * it will try to read from the address currently on the SDI pins.
	 * An invalid address might cause udefined behavior so better to have
	 * some valid address in buf while we read the result.
	 */
	buf = addr;
	ret = no_os_spi_write_and_read(dev->spi_desc, &buf, 1);
	if (ret)
		return ret;

	*val = buf;
	return 0;
}

/*******************************************************************************
* @brief Sets the excitation frequency and performs software reset.
*
* @param dev - Device descriptor.
* @param fexcit - excitation frequency
*
* @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int ad2s1210_reinit_excitation_frequency(struct ad2s1210_dev *dev,
		uint16_t fexcit)
{
	int ret;
	int8_t fcw;

	fcw = fexcit * (1 << 15) / dev->clkin_hz;
	if (fcw < AD2S1210_MIN_FCW || fcw > AD2S1210_MAX_FCW)
		return -ERANGE;

	ret = ad2s1210_reg_write(dev, AD2S1210_REG_EXCIT_FREQ, fcw);
	if (ret)
		return ret;

	/*
	 * Software reset reinitializes the excitation frequency output.
	 * It does not reset any of the configuration registers.
	 */
	return ad2s1210_reg_write(dev, AD2S1210_REG_SOFT_RESET, 0);
}

/*******************************************************************************
* @brief Gets the excitation frequency.
*
* @param dev - Device descriptor.
* @param fexcit - pointer to store the excitation frequency
*
* @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int ad2s1210_get_excitation_frequency(struct ad2s1210_dev *dev,
				      uint16_t *fexcit)
{
	uint8_t val;
	int ret;

	ret = ad2s1210_reg_read(dev, AD2S1210_REG_EXCIT_FREQ, &val);
	if (ret)
		return ret;

	*fexcit = val * dev->clkin_hz / (1 << 15);
	return 0;
}

/*******************************************************************************
* @brief Gets the the hysteresis enable value.
*
* @param dev - Device descriptor.
*
* @return Returns 1 in case of enabled 0 if disabled or negative error code
*******************************************************************************/
int ad2s1210_hysteresis_is_enabled(struct ad2s1210_dev *dev)
{
	int ret;
	uint8_t control;

	ret = ad2s1210_reg_read(dev,  AD2S1210_REG_CONTROL, &control);
	if (ret)
		return ret;

	if (control & AD2S1210_ENABLE_HYSTERESIS)
		return 1;

	return 0;
}

/*******************************************************************************
* @brief Sets the hysteresis enable value
*
* @param dev - Device descriptor.
* @param fexcit - pointer to store the excitation frequency
*
* @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int ad2s1210_set_hysteresis(struct ad2s1210_dev *dev, bool enable)
{
	int ret;
	uint8_t control;

	ret = ad2s1210_reg_read(dev,  AD2S1210_REG_CONTROL, &control);
	if (ret)
		return ret;

	control &= ~AD2S1210_ENABLE_HYSTERESIS;
	if (enable)
		control |= AD2S1210_ENABLE_HYSTERESIS;

	return ad2s1210_reg_write(dev,  AD2S1210_REG_CONTROL, control);
}

/*******************************************************************************
* @brief Read a device register
*
* @param dev - Device descriptor.
* @param mode - enum that contains the mode to set the device
*
* @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int ad2s1210_set_mode_pins(struct ad2s1210_dev *dev,
				  enum ad2s1210_mode mode)
{
	int32_t ret;

	if (dev->mode == mode)
		return 0;

	ret = no_os_gpio_set_value(dev->gpio_a0, mode & NO_OS_BIT(0));
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_a1, !!(mode & NO_OS_BIT(1)));
	if (ret)
		return ret;

	dev->mode = mode;
	return 0;
}

/*******************************************************************************
* @brief Set device bit resolution 10, 12, 14, or 16 bits
*
* @param dev - Device descriptor.
* @param resolution - resolution value
*
* @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int ad2s1210_set_resolution(struct ad2s1210_dev *dev,
				   enum ad2s1210_res resolution)
{
	int32_t ret;
	uint8_t control;

	ret = ad2s1210_reg_read(dev, AD2S1210_REG_CONTROL, &control);
	if (ret)
		return ret;

	control &= ~(AD2S1210_CONTROL_RES_MASK);

	switch (resolution) {
	case AD2S1210_RES_10BIT:
		break;
	case AD2S1210_RES_12BIT:
		control |= AD2S1210_CONTROL_RES0_MASK;
		break;
	case AD2S1210_RES_14BIT:
		control |= AD2S1210_CONTROL_RES1_MASK;
		break;
	case AD2S1210_RES_16BIT:
		control |= AD2S1210_CONTROL_RES1_MASK | AD2S1210_CONTROL_RES0_MASK;
		break;
	default:
		pr_err("Invalid resolution: %d\n", resolution);
		return -EINVAL;
	}

	ret = ad2s1210_reg_write(dev, AD2S1210_REG_CONTROL, control);
	if (ret)
		return ret;

	if (!dev->have_resolution_pins)
		return 0;

	ret = no_os_gpio_set_value(dev->gpio_res0, !!(control & NO_OS_BIT(0)));
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_res1, !!(control & NO_OS_BIT(1)));
}

/*******************************************************************************
 * @brief Initialize the ad2s1210 driver and create a descriptor.
 *
 * @param dev - Device descriptor to create.
 * @param init_param - Initialization parameters.
 *
 * @return Returns negative error code or 0 in case of success.
 *         Example: -EINVAL - Bad input parameters.
 *                  -ENOMEM - Failed to allocate memory.
 *                  0 - No errors encountered.
*******************************************************************************/
int ad2s1210_init(struct ad2s1210_dev **dev,
		  struct ad2s1210_init_param *init_param)
{
	struct ad2s1210_dev *d;
	int32_t ret;

	if (init_param->clkin_hz < AD2S1210_MIN_CLKIN
	    || init_param->clkin_hz > AD2S1210_MAX_CLKIN) {
		pr_err("Clock frequency out of range: %lu\n",
		       init_param->clkin_hz);
		return -EINVAL;
	}

	d = (struct ad2s1210_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->name = "AD2S1210";
	d->mode = MODE_CONFIG;
	d->resolution = init_param->resolution;
	d->clkin_hz = init_param->clkin_hz;

	ret = no_os_gpio_get(&d->gpio_sample, &init_param->gpio_sample);
	if (ret)
		goto err_sample;

	ret = no_os_gpio_get_optional(&d->gpio_a0, &init_param->gpio_a0);
	if (ret)
		goto err_a0;

	ret = no_os_gpio_get_optional(&d->gpio_a1, &init_param->gpio_a1);
	if (ret)
		goto err_a1;

	ret = no_os_gpio_get_optional(&d->gpio_res0, &init_param->gpio_res0);
	if (ret)
		goto err_res0;

	ret = no_os_gpio_get_optional(&d->gpio_res1, &init_param->gpio_res1);
	if (ret)
		goto err_res1;

	ret = no_os_gpio_direction_output(d->gpio_a0, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_output;

	ret = no_os_gpio_direction_output(d->gpio_a1, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_output;

	ret = no_os_gpio_direction_output(d->gpio_res0, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_output;

	ret = no_os_gpio_direction_output(d->gpio_res1, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_output;

	ret = no_os_gpio_direction_output(d->gpio_sample, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_output;

	ret = no_os_spi_init(&d->spi_desc, &init_param->spi_init);
	if (ret)
		goto err_output;

	if (d->gpio_a0 && d->gpio_a1)
		d->have_mode_pins = true;

	if (d->gpio_res0 && d->gpio_res1)
		d->have_resolution_pins = true;

	ret = ad2s1210_set_resolution(d, d->resolution);
	if (ret)
		goto err_spi;

	*dev = d;
	return 0;

err_spi:
	no_os_spi_remove(d->spi_desc);
err_output:
	no_os_gpio_remove(d->gpio_res1);
err_res1:
	no_os_gpio_remove(d->gpio_res0);
err_res0:
	no_os_gpio_remove(d->gpio_a1);
err_a1:
	no_os_gpio_remove(d->gpio_a0);
err_a0:
	no_os_gpio_remove(d->gpio_sample);
err_sample:
	no_os_free(d);
	pr_err("%s initialization failed with status %ld\n", d->name, ret);

	return ret;
}

/***************************************************************************//**
 * @brief Returns the result of a single channel.
 *
 * @param dev - The device structure.
 * @param chn - Channel to get data.
 * @param data - Buffer to store sampled register data
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
static int ad2s1210_get_channel_data(struct ad2s1210_dev *dev,
				     enum ad2s1210_channel chn,
				     uint16_t *data)
{
	int32_t ret;
	uint8_t addr = AD2S1210_REG_POSITION;
	enum ad2s1210_mode mode = MODE_POS;

	if (dev->have_mode_pins) {
		if (chn == AD2S1210_VEL)
			mode = MODE_VEL;

		ret = ad2s1210_set_mode_pins(dev, mode);
		if (ret)
			return ret;

		return no_os_spi_write_and_read(dev->spi_desc, (uint8_t *)data, 2);
	}

	if (chn == AD2S1210_VEL)
		addr = AD2S1210_REG_VELOCITY;

	ret = ad2s1210_reg_read(dev, addr, (uint8_t *)data);
	if (ret)
		return ret;

	return ad2s1210_reg_read(dev, addr + 1, (uint8_t *)data + 1);
}

/***************************************************************************//**
 * @brief Returns the result of a conversion.
 *
 * @param dev - The device structure.
 * @param active_mask - mask of active channels angle = bit 0, velocity = bit 1
 * @param data - Buffer to store sampled register data.
 * @param size - size in bytes of the data buffer
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad2s1210_spi_single_conversion(struct ad2s1210_dev *dev,
				   uint32_t active_mask,
				   void *data, uint32_t size)
{
	int32_t ret;
	uint16_t *data_p = (uint16_t *)data;

	if (size < 2)
		return -EINVAL;

	if ((size < 4) && (active_mask & AD2S1210_POS_MASK)
	    && (active_mask & AD2S1210_POS_MASK))
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_sample, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_sample, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	if (active_mask & AD2S1210_POS_MASK) {
		ret = ad2s1210_get_channel_data(dev, AD2S1210_POS, data_p++);
		if (ret)
			return ret;
	}

	if (active_mask & AD2S1210_VEL_MASK) {
		ret = ad2s1210_get_channel_data(dev, AD2S1210_VEL, data_p);
		if (ret)
			return ret;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Remove the driver's descriptor by freeing the associated resources.
 *
 * @param dev - Device descriptor.
 *
 * @return Returns negative error code or 0 in case of success.
 *         Example: -EINVAL - Bad input parameters.
 *                  0 - No errors encountered.
*******************************************************************************/
int ad2s1210_remove(struct ad2s1210_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	if (dev->gpio_res1) {
		ret = no_os_gpio_remove(dev->gpio_res1);
		if (ret)
			return ret;
	}

	if (dev->gpio_res0) {
		ret = no_os_gpio_remove(dev->gpio_res0);
		if (ret)
			return ret;
	}

	if (dev->gpio_a0) {
		ret = no_os_gpio_remove(dev->gpio_a0);
		if (ret)
			return ret;
	}

	if (dev->gpio_a1) {
		ret = no_os_gpio_remove(dev->gpio_a1);
		if (ret)
			return ret;
	}

	if (dev->gpio_sample) {
		ret = no_os_gpio_remove(dev->gpio_sample);
		if (ret)
			return ret;
	}

	no_os_free(dev);

	return 0;
}

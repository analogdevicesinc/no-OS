/***************************************************************************//**
 *   @file   ltc2378.c
 *   @brief  Source file of LTC2378 driver.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
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

#include "ltc2378.h"
#include <stdlib.h>
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"

/**
 * @brief Initialize the LTC2378-20 device
 * @param device - Pointer to device structure pointer
 * @param init_param - Pointer to initialization parameters structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2378_init(struct ltc2378_dev **device,
		 struct ltc2378_init_param *init_param)
{
	struct ltc2378_dev *dev;
	int ret;

	dev = (struct ltc2378_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->input_mode = init_param->input_mode;
	dev->vref_uv = init_param->vref_uv;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error;

	ret = no_os_gpio_get(&dev->gpio_cnv, init_param->gpio_cnv_init);
	if (ret)
		goto error_spi;

	ret = no_os_gpio_direction_output(dev->gpio_cnv, NO_OS_GPIO_LOW);
	if (ret)
		goto error_cnv;

	ret = no_os_gpio_get_optional(&dev->gpio_busy, init_param->gpio_busy_init);
	if (ret)
		goto error_cnv;

	if (dev->gpio_busy) {
		ret = no_os_gpio_direction_input(dev->gpio_busy);
		if (ret)
			goto error_busy;
	}

	*device = dev;
	return 0;

error_busy:
	no_os_gpio_remove(dev->gpio_busy);
error_cnv:
	no_os_gpio_remove(dev->gpio_cnv);
error_spi:
	no_os_spi_remove(dev->spi_desc);
error:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Remove the LTC2378-20 device and free allocated resources
 * @param dev - Device structure pointer
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2378_remove(struct ltc2378_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_spi_remove(dev->spi_desc);
	no_os_gpio_remove(dev->gpio_cnv);
	if (dev->gpio_busy)
		no_os_gpio_remove(dev->gpio_busy);
	no_os_free(dev);

	return 0;
}

/**
 * @brief Start a single ADC conversion
 * @param dev - Device structure pointer
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2378_start_conversion(struct ltc2378_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_cnv, 1);
	if (ret)
		return ret;

	no_os_udelay(LTC2378_CNV_PULSE_WIDTH_US);

	ret = no_os_gpio_set_value(dev->gpio_cnv, 0);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read a single raw ADC conversion result
 * @param dev - Device structure pointer
 * @param data - Pointer to store the raw ADC data (20-bit value)
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2378_read_raw(struct ltc2378_dev *dev, uint32_t *data)
{
	int ret;
	uint8_t val;
	int timeout;
	uint8_t buf[3] = {0};

	if (!dev || !data)
		return -EINVAL;

	ret = ltc2378_start_conversion(dev);
	if (ret)
		return ret;

	if (dev->gpio_busy) {
		timeout = LTC2378_BUSY_TIMEOUT_US;
		do {
			ret = no_os_gpio_get_value(dev->gpio_busy, &val);
			if (ret)
				return ret;
			no_os_udelay(1);
		} while (val && --timeout);

		if (timeout == 0)
			return -ETIMEDOUT;
	} else {
		no_os_udelay(LTC2378_CONVERSION_DELAY_US);
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*data = (((uint32_t)buf[0] << LTC2378_DATA_SHIFT_BYTE0) |
		 ((uint32_t)buf[1] << LTC2378_DATA_SHIFT_BYTE1) |
		 ((uint32_t)buf[2] >> LTC2378_DATA_SHIFT_BYTE2)) & LTC2378_DATA_MASK;

	return 0;
}

/**
 * @brief Read and average multiple ADC conversion results
 * @param dev - Device structure pointer
 * @param avg_data - Pointer to store the averaged raw ADC data
 * @param samples - Number of samples to average (1-65535)
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2378_read_avg(struct ltc2378_dev *dev, uint32_t *avg_data,
		     uint16_t samples)
{
	uint64_t sum = 0;
	uint32_t raw;
	uint16_t i;
	int ret;

	if (!dev || !avg_data || samples == 0)
		return -EINVAL;

	for (i = 0; i < samples; i++) {
		ret = ltc2378_read_raw(dev, &raw);
		if (ret)
			return ret;
		sum += raw;
	}

	*avg_data = (uint32_t)(sum / samples);
	return 0;
}

/**
 * @brief Convert raw ADC value to voltage in microvolts
 * @param dev - Device structure pointer
 * @param raw - Raw ADC value (20-bit)
 * @param voltage_uv - Pointer to store the voltage in microvolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2378_raw_to_uv(struct ltc2378_dev *dev, uint32_t raw,
		      int32_t *voltage_uv)
{
	int64_t uv;
	int32_t signed_val;

	if (!dev || !voltage_uv)
		return -EINVAL;

	if (dev->input_mode == LTC2378_UNIPOLAR) {
		/* Unipolar mode: raw value is unsigned (0 to VREF) */
		uv = (int64_t)raw * dev->vref_uv;
		uv /= LTC2378_RESOLUTION_UNIPOLAR;
	} else if (dev->input_mode == LTC2378_BIPOLAR) {
		/* Bipolar mode: sign-extend the 20-bit raw value (-VREF to +VREF) */
		signed_val = no_os_sign_extend32(raw, LTC2378_SIGN_BIT_POSITION);
		uv = (int64_t)signed_val * dev->vref_uv;
		uv /= LTC2378_RESOLUTION_BIPOLAR;
	} else {
		return -EINVAL;
	}

	*voltage_uv = (int32_t)uv;
	return 0;
}

/**
 * @brief Put the LTC2378-20 into power-down mode
 * @param dev - Device structure pointer
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2378_power_down(struct ltc2378_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_cnv, 0);
	if (ret)
		return ret;

	no_os_udelay(LTC2378_POWERDOWN_DELAY_US);
	return 0;
}

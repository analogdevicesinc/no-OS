/***************************************************************************//**
 *   @file   adxrs290.c
 *   @brief  Implementation of ADXRS290 Driver.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "adxrs290.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - Device handler.
 * @param address - Register address.
 * @param data - Pointer to the register value container.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_reg_read(struct adxrs290_dev *dev, uint8_t address,
			  uint8_t *data)
{
	int32_t ret = 0;
	uint8_t buff[] = {address | 0x80, 0};

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, 2);
	if (NO_OS_IS_ERR_VALUE(ret))
		return -1;

	*data = buff[1];

	return 0;
}

/**
 * @brief Write device register.
 * @param dev - Device handler.
 * @param address - Register address.
 * @param data - New register value.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_reg_write(struct adxrs290_dev *dev, uint8_t address,
			   uint8_t data)
{
	uint8_t buff[] = {address & 0x7F, data};

	return no_os_spi_write_and_read(dev->spi_desc, buff, 2);
}

/**
 * @brief Set device operation mode.
 * @param dev - Device handler.
 * @param mode - mode of operation.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_set_op_mode(struct adxrs290_dev *dev, enum adxrs290_mode mode)
{
	int32_t ret = 0;
	uint8_t val = 0;

	ret = adxrs290_reg_read(dev, ADXRS290_REG_POWER_CTL, &val);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	switch(mode) {
	case ADXRS290_MODE_STANDBY:
		val &= ~ADXRS290_MEASUREMENT;
		break;
	case ADXRS290_MODE_MEASUREMENT:
		val |= ADXRS290_MEASUREMENT;
		break;
	default:
		return -1;
	}

	return adxrs290_reg_write(dev, ADXRS290_REG_POWER_CTL, val);
}

/**
 * @brief Get the low-pass filter pole location.
 * @param dev - Device handler.
 * @param lpf - Pointer to Low-pass pole location container.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_get_lpf(struct adxrs290_dev *dev, enum adxrs290_lpf *lpf)
{
	int32_t ret = 0;
	uint8_t data;

	ret = adxrs290_reg_read(dev, ADXRS290_REG_FILTER, &data);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	*lpf = ADXRS290_LPF(data);

	return ret;
}

/**
 * @brief Set the low-pass filter pole location.
 * @param dev - Device handler.
 * @param lpf - Low-pass pole location.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_set_lpf(struct adxrs290_dev *dev, enum adxrs290_lpf lpf)
{
	int32_t ret = 0;
	uint8_t reg_val;

	ret = adxrs290_reg_read(dev, ADXRS290_REG_FILTER, &reg_val);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	reg_val &= ~ADXRS290_LPF_MASK;
	reg_val |= lpf & ADXRS290_LPF_MASK;

	return adxrs290_reg_write(dev, ADXRS290_REG_FILTER, reg_val);
}

/**
 * @brief Get the high-pass filter pole location.
 * @param dev - Device handler.
 * @param hpf - Pointer to high-pass pole location container.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_get_hpf(struct adxrs290_dev *dev, enum adxrs290_hpf *hpf)
{
	int32_t ret = 0;
	uint8_t data;

	ret = adxrs290_reg_read(dev, ADXRS290_REG_FILTER, &data);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	*hpf = ADXRS290_HPF(data);

	return ret;
}

/**
 * @brief Set the low-pass filter pole location.
 * @param dev - Device handler.
 * @param hpf - High-pass pole location.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_set_hpf(struct adxrs290_dev *dev, enum adxrs290_hpf hpf)
{
	int32_t ret = 0;
	uint8_t reg_val;

	ret = adxrs290_reg_read(dev, ADXRS290_REG_FILTER, &reg_val);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	reg_val &= ~ADXRS290_HPF_MASK;
	reg_val |= (hpf << 4) & ADXRS290_HPF_MASK;

	return adxrs290_reg_write(dev, ADXRS290_REG_FILTER, reg_val);
}

/**
 * @brief Get the Gyro data channels.
 * @param dev - Device handler.
 * @param ch - Channel to read.
 * @param rate - Pointer to rate value.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_get_rate_data(struct adxrs290_dev *dev,
			       enum adxrs290_channel ch, int16_t *rate)
{
	int32_t ret = 0;
	uint8_t data[3];

	data[0] = 0x80 | (ADXRS290_REG_DATAX0+ch*2);
	data[1] = 0x80 | (ADXRS290_REG_DATAX1+ch*2);
	data[2] = 0;
	ret = no_os_spi_write_and_read(dev->spi_desc, data, 3);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	*rate = (((int16_t)data[2]) << 8) | data[1];

	return 0;
}

/**
 * @brief Get the temperature data.
 * @param dev - Device handler.
 * @param temp - Pointer to temperature value.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_get_temp_data(struct adxrs290_dev *dev, int16_t *temp)
{
	int32_t ret = 0;
	uint8_t data[3];

	data[0] = 0x80 | ADXRS290_REG_TEMP0;
	data[1] = 0x80 | ADXRS290_REG_TEMP1;
	data[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, data, 3);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	*temp = ((((int16_t)data[2]) << 8) | data[1]) & 0x0FFF;
	*temp = (*temp << 4) >> 4;

	return ret;
}

/**
 * @brief Get the burst data.
 * @param dev - Device handler.
 * @param burst_data - Pointer to data value.
 * @param ch_cnt - Number of active channels.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_get_burst_data(struct adxrs290_dev *dev, int16_t *burst_data,
				uint8_t *ch_cnt)
{
	int32_t		ret = 0;
	uint8_t		data_bytes = ADXRS290_CHANNEL_COUNT*2;
	uint8_t		data[data_bytes + 1];
	int16_t		result;
	uint8_t		ch_idx;
	uint8_t		i;

	for (i = 0; i < data_bytes; i++)
		data[i] = 0x80 | (ADXRS290_REG_DATAX0 + i);

	data[data_bytes] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, data, data_bytes + 1);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	i = 1;
	*ch_cnt = 0;
	for (ch_idx = 0; ch_idx < ADXRS290_CHANNEL_COUNT; ch_idx++) {
		result = (((int16_t)data[i+1]) << 8) | data[i];
		i += 2;
		if (ch_idx == ADXRS290_CHANNEL_TEMP)
			result = (result << 4) >> 4;

		if ((1 << ch_idx) & dev->ch_mask)
			burst_data[(*ch_cnt)++] = result;
	}

	return ret;
}

/**
 * @brief Set the ADXRS290 active channels.
 * @param dev - Device handler.
 * @param mask - Mask for active channels.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_set_active_channels(struct adxrs290_dev *dev, uint32_t mask)
{
	dev->ch_mask = mask & ADXRS290_CHANNEL_MASK;

	return 0;
}

/**
 * @brief Get the state of data ready.
 * @param dev - Device handler.
 * @param rdy - Pointer to state of data.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_get_data_ready(struct adxrs290_dev *dev, bool *rdy)
{
	int32_t ret = 0;
	uint8_t value;

	if (!dev->gpio_sync)
		return -1;

	ret = no_os_gpio_get_value(dev->gpio_sync, &value);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	if (value == NO_OS_GPIO_HIGH)
		*rdy = true;
	else
		*rdy = false;

	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxrs290_init(struct adxrs290_dev **device,
		      const struct adxrs290_init_param *init_param)
{
	struct adxrs290_dev *dev;
	int32_t ret = 0;
	uint8_t val = 0;

	dev = (struct adxrs290_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_dev;

	ret = adxrs290_reg_read(dev, ADXRS290_REG_DEV_ID, &val);
	if (NO_OS_IS_ERR_VALUE(ret) || (val != ADXRS290_DEV_ID)) {
		ret = -ENODEV;
		goto error_spi;
	}

	// Enable measurement mode.
	if (init_param->mode == ADXRS290_MODE_MEASUREMENT) {
		ret = adxrs290_reg_write(dev, ADXRS290_REG_POWER_CTL,
					 ADXRS290_MEASUREMENT | ADXRS290_TSM);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error_spi;

	}
	// Set initial Band pass filter poles.
	ret = adxrs290_set_lpf(dev, init_param->lpf);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_spi;

	ret = adxrs290_set_hpf(dev, init_param->hpf);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_spi;

	// Set GPIO sync pin.
	ret = no_os_gpio_get_optional(&dev->gpio_sync, init_param->gpio_sync);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_spi;

	if (dev->gpio_sync) {
		ret = no_os_gpio_direction_input(dev->gpio_sync);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto error_gpio;
	}

	// Set adxrs290 to output on sync pin.
	ret = adxrs290_reg_write(dev, ADXRS290_REG_DATA_READY,
				 ADXRS290_DATA_RDY_OUT);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error_gpio;

	// Enable all channels by default
	dev->ch_mask = ADXRS290_CHANNEL_MASK;

	*device = dev;

	return ret;

error_gpio:
	no_os_gpio_remove(dev->gpio_sync);

error_spi:
	no_os_spi_remove(dev->spi_desc);

error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free memory allocated by adxrs290_setup().
 * @param dev - Device handler.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adxrs290_remove(struct adxrs290_dev *dev)
{
	no_os_spi_remove(dev->spi_desc);
	no_os_gpio_remove(dev->gpio_sync);
	no_os_free(dev);

	return 0;
}



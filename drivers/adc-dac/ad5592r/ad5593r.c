/***************************************************************************//**
 *   @file   ad5593r.c
 *   @brief  Implementation of AD5593R driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020, 2025(c) Analog Devices, Inc.
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

#include "ad5592r-base.h"
#include "no_os_error.h"
#include "ad5593r.h"

const struct ad5592r_rw_ops ad5593r_rw_ops = {
	.write_dac = ad5593r_write_dac,
	.read_adc = ad5593r_read_adc,
	.multi_read_adc = ad5593r_multi_read_adc,
	.reg_write = ad5593r_reg_write,
	.reg_read = ad5593r_reg_read,
	.gpio_read = ad5593r_gpio_read,
};

/**
 * Write DAC channel.
 *
 * @param dev - The device structure.
 * @param chan - The channel number.
 * @param value - DAC value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5593r_write_dac(struct ad5592r_dev *dev, uint8_t chan,
			  uint16_t value)
{
	uint8_t data[3];
	int ret;

	if (!dev)
		return -1;

	data[0] = AD5593R_MODE_DAC_WRITE | chan;
	data[1] = (value >> 8) & 0xF ;
	data[2] = value & 0xFF;

	ret = no_os_i2c_write(dev->i2c, data, sizeof(data), AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	dev->cached_dac[chan] = value;

	return 0;
}

/**
 * Read ADC channel.
 *
 * @param dev - The device structure.
 * @param chan - The channel number.
 * @param value - ADC value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5593r_read_adc(struct ad5592r_dev *dev, uint8_t chan,
			 uint16_t *value)
{
	int32_t ret;
	uint8_t data[3];
	uint16_t temp;

	if (!dev)
		return -1;

	temp = NO_OS_BIT(chan);

	data[0] = AD5593R_MODE_CONF | AD5592R_REG_ADC_SEQ;
	data[1] = temp >> 8;
	data[2] = temp & 0xFF;

	ret = no_os_i2c_write(dev->i2c, data, sizeof(data), AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	data[0] = AD5593R_MODE_ADC_READBACK;
	ret = no_os_i2c_write(dev->i2c, data, 1, AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	ret = no_os_i2c_read(dev->i2c, data, 2, AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	*value = (uint16_t)((data[0] & 0x0F) << 8) + data[1];

	return 0;
}

/**
 * Read Multiple ADC Channels.
 *
 * @param dev - The device structure.
 * @param chans - The ADC channels to be readback
 * @param values - ADC value array
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5593r_multi_read_adc(struct ad5592r_dev *dev, uint16_t chans,
			       uint16_t *values)
{
	uint8_t data[AD5593R_ADC_VALUES_BUFF_SIZE], i;
	int32_t ret;
	uint8_t samples;

	if (!dev)
		return -1;

	samples = no_os_hweight16(chans);

	data[0] = AD5593R_MODE_CONF | AD5592R_REG_ADC_SEQ;
	data[1] = chans >> 8;
	data[2] = chans & 0xFF;

	ret = no_os_i2c_write(dev->i2c, data, 3, AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	data[0] = AD5593R_MODE_ADC_READBACK;
	ret = no_os_i2c_write(dev->i2c, data, 1, AD5593R_RESTART_BIT);
	if (ret < 0)
		return ret;

	ret = no_os_i2c_read(dev->i2c, data, (2 * samples), AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	for (i = 0; i < samples; i++) {
		values[i] = ((uint16_t)(((data[2 * i] & 0xFF) << 8) + data[(2 * i) + 1]));
	}

	return 0;
}

/**
 * Write register.
 *
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param value - register value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5593r_reg_write(struct ad5592r_dev *dev, uint8_t reg,
			  uint16_t value)
{
	int32_t ret;
	uint8_t data[3];

	if (!dev)
		return -1;

	data[0] = AD5593R_MODE_CONF | reg;
	data[1] = value >> 8;
	data[2] = value;

	ret = no_os_i2c_write(dev->i2c, data, sizeof(data), AD5593R_STOP_BIT);

	if (reg == AD5592R_REG_RESET && ret < 0) {
		return 0;
	}

	return ret;
}

/**
 * Read register.
 *
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param value - register value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5593r_reg_read(struct ad5592r_dev *dev, uint8_t reg,
			 uint16_t *value)
{
	int32_t ret;
	uint8_t data[2];

	if (!dev)
		return -1;

	data[0] = AD5593R_MODE_REG_READBACK | reg;

	ret = no_os_i2c_write(dev->i2c, data, 1, AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	ret = no_os_i2c_read(dev->i2c, data, sizeof(data), AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	*value = (uint16_t)(data[0] << 8) + data[1];

	return 0;
}

/**
 * Read GPIOs.
 *
 * @param dev - The device structure.
 * @param value - GPIOs value.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5593r_gpio_read(struct ad5592r_dev *dev, uint8_t *value)
{
	int32_t ret;
	uint8_t data[2];

	if (!dev)
		return -1;

	data[0] = AD5593R_MODE_GPIO_READBACK;
	ret = no_os_i2c_write(dev->i2c, data, 1, AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	ret = no_os_i2c_read(dev->i2c, data, sizeof(data), AD5593R_STOP_BIT);
	if (ret < 0)
		return ret;

	*value = data[1];

	return 0;
}

/**
 * Initialize AD5593r device.
 *
 * @param device - The device structure.
 * @param init_param - The initial parameters of the device.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5593r_init(struct ad5592r_dev **device,
		     struct ad5592r_init_param *init_param)
{
	int32_t ret;
	uint8_t i;
	struct ad5592r_dev *dev;

	dev = (struct ad5592r_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	/* Initialize the SPI communication. */
	ret = no_os_i2c_init(&dev->i2c, init_param->i2c_init);
	if (ret < 0)
		return ret;

	dev->ops = &ad5593r_rw_ops;
	dev->ldac_mode = 0;
	dev->num_channels = NUM_OF_CHANNELS;

	ret = ad5592r_software_reset(dev);
	if (ret < 0)
		return ret;

	for (i = 0; i < NUM_OF_CHANNELS; i++) {
		dev->channel_modes[i] = init_param->channel_modes[i];
		dev->channel_offstate[i] = init_param->channel_offstate[i];
	}

	ret = ad5592r_set_adc_range(dev, init_param->adc_range);
	if (ret < 0)
		return ret;

	ret = ad5592r_set_dac_range(dev, init_param->dac_range);
	if (ret < 0)
		return ret;

	ret = ad5592r_set_adc_buffer(dev, init_param->adc_buf);
	if (ret < 0)
		return ret;

	ret = ad5592r_set_channel_modes(dev);
	if (ret < 0)
		return ret;

	ret = ad5592r_set_int_ref(dev, init_param->int_ref);
	if (ret < 0)
		return ret;

	for (i = 0; i < NUM_OF_CHANNELS ; i++) {
		ret = ad5592r_power_down(dev, i, init_param->power_down[i]);
		if (ret < 0)
			return ret;
	}

	*device = dev;

	return ret;
}

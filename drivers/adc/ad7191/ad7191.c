/***************************************************************************//**
 *   @file   ad7191.c
 *   @brief  Implementation of AD7191 driver.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <errno.h>
#include <stdint.h>
#include "ad7191.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/************************ Variable Definitions ********************************/
/******************************************************************************/
/* Output data rates in Hz, see Table 5. Indexed by enum ad7191_odr. */
const uint32_t ad7191_odr_table[AD7191_ODR_NR] = { 120, 60, 50, 10 };

/* PGA gains, see Table 7. Indexed by enum ad7191_pga_gain. */
const uint32_t ad7191_gain_table[AD7191_PGA_GAIN_NR] = { 1, 8, 64, 128 };

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief Drive a two-pin (LSB, MSB) configuration value.
 * @param lsb - Descriptor of the least significant pin.
 * @param msb - Descriptor of the most significant pin.
 * @param value - 2-bit value to drive.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad7191_set_2bit(struct no_os_gpio_desc *lsb,
			   struct no_os_gpio_desc *msb, uint8_t value)
{
	int ret;

	ret = no_os_gpio_set_value(lsb, value & 0x1);
	if (ret)
		return ret;

	return no_os_gpio_set_value(msb, (value >> 1) & 0x1);
}

/**
 * @brief Select the clock source (CLKSEL pin).
 * @param dev - The device structure.
 * @param clock_sel - Clock source selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_set_clock(struct ad7191_dev *dev, enum ad7191_clock_sel clock_sel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_clksel,
				   clock_sel == AD7191_INT_CLK ?
				   NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	dev->clock_sel = clock_sel;

	return 0;
}

/**
 * @brief Select the input channel (CHAN/TEMP pins).
 * @param dev - The device structure.
 * @param channel - Channel selection.
 * @return 0 in case of success, negative error code otherwise.
 *
 * TEMP overrides CHAN: when the temperature sensor is selected the CHAN pin is
 * driven low and TEMP high.
 */
int ad7191_set_channel(struct ad7191_dev *dev, enum ad7191_channel channel)
{
	uint8_t chan_val, temp_val;
	int ret;

	if (!dev)
		return -EINVAL;

	switch (channel) {
	case AD7191_CH_AIN1_AIN2:
		chan_val = NO_OS_GPIO_LOW;
		temp_val = NO_OS_GPIO_LOW;
		break;
	case AD7191_CH_AIN3_AIN4:
		chan_val = NO_OS_GPIO_HIGH;
		temp_val = NO_OS_GPIO_LOW;
		break;
	case AD7191_CH_TEMP:
		chan_val = NO_OS_GPIO_LOW;
		temp_val = NO_OS_GPIO_HIGH;
		break;
	default:
		return -EINVAL;
	}

	ret = no_os_gpio_set_value(dev->gpio_chan, chan_val);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_temp, temp_val);
	if (ret)
		return ret;

	dev->channel = channel;

	return 0;
}

/**
 * @brief Select the PGA gain (PGA1/PGA2 pins).
 * @param dev - The device structure.
 * @param gain - Gain selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_set_gain(struct ad7191_dev *dev, enum ad7191_pga_gain gain)
{
	int ret;

	if (!dev || gain >= AD7191_PGA_GAIN_NR)
		return -EINVAL;

	ret = ad7191_set_2bit(dev->gpio_pga1, dev->gpio_pga2, gain);
	if (ret)
		return ret;

	dev->gain = gain;

	return 0;
}

/**
 * @brief Select the output data rate (ODR1/ODR2 pins).
 * @param dev - The device structure.
 * @param odr - Output data rate selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_set_odr(struct ad7191_dev *dev, enum ad7191_odr odr)
{
	int ret;

	if (!dev || odr >= AD7191_ODR_NR)
		return -EINVAL;

	ret = ad7191_set_2bit(dev->gpio_odr1, dev->gpio_odr2, odr);
	if (ret)
		return ret;

	dev->odr = odr;

	return 0;
}

/**
 * @brief Power up the device and start conversions (PDOWN low).
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_power_up(struct ad7191_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_pdown, NO_OS_GPIO_LOW);
}

/**
 * @brief Power down and reset the device (PDOWN high).
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_power_down(struct ad7191_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_pdown, NO_OS_GPIO_HIGH);
}

/**
 * @brief Wait for a conversion to become ready (DOUT/RDY low).
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 *
 * The Linux platform has no GPIO interrupt support, so the DOUT/RDY line is
 * polled through a second GPIO. The timeout is scaled to a few conversion
 * periods of the currently selected output data rate.
 */
int ad7191_wait_ready(struct ad7191_dev *dev)
{
	uint32_t timeout_ms;
	uint8_t value;
	int ret;

	if (!dev)
		return -EINVAL;

	/* Allow up to four conversion periods plus a fixed margin. */
	timeout_ms = 4000 / ad7191_odr_table[dev->odr] + 100;

	while (timeout_ms--) {
		ret = no_os_gpio_get_value(dev->gpio_rdy, &value);
		if (ret)
			return ret;

		if (value == NO_OS_GPIO_LOW)
			return 0;

		no_os_mdelay(1);
	}

	return -ETIMEDOUT;
}

/**
 * @brief Read one 24-bit conversion result, assuming the device is powered up.
 * @param dev - The device structure.
 * @param data - Pointer to store the 24-bit offset-binary result.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_read_data(struct ad7191_dev *dev, uint32_t *data)
{
	uint8_t buf[3] = { 0, 0, 0 };
	int ret;

	if (!dev || !data)
		return -EINVAL;

	ret = ad7191_wait_ready(dev);
	if (ret)
		return ret;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
	if (ret)
		return ret;

	*data = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];

	return 0;
}

/**
 * @brief Configure the channel, power up, discard settling data and read one
 *        sample.
 * @param dev - The device structure.
 * @param channel - Channel to sample.
 * @param data - Pointer to store the 24-bit offset-binary result.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_single_conversion(struct ad7191_dev *dev,
			     enum ad7191_channel channel, uint32_t *data)
{
	uint32_t dummy;
	int ret;
	int i;

	if (!dev || !data)
		return -EINVAL;

	ret = ad7191_set_channel(dev, channel);
	if (ret)
		return ret;

	ret = ad7191_power_up(dev);
	if (ret)
		return ret;

	/* Discard the settling conversions that follow the configuration change. */
	for (i = 0; i < AD7191_SETTLING_CONVERSIONS; i++) {
		ret = ad7191_read_data(dev, &dummy);
		if (ret)
			goto power_down;
	}

	ret = ad7191_read_data(dev, data);

power_down:
	ad7191_power_down(dev);

	return ret;
}

/**
 * @brief Initialize the AD7191 device.
 * @param device - Pointer to the device structure to allocate.
 * @param init_param - Initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_init(struct ad7191_dev **device,
		struct ad7191_init_param *init_param)
{
	struct ad7191_dev *dev;
	struct no_os_gpio_desc **descs[9];
	struct no_os_gpio_init_param *params[9];
	int ret;
	int i;

	if (!device || !init_param)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->vref_mv = init_param->vref_mv;

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret)
		goto error_dev;

	descs[0] = &dev->gpio_pdown;
	descs[1] = &dev->gpio_rdy;
	descs[2] = &dev->gpio_clksel;
	descs[3] = &dev->gpio_temp;
	descs[4] = &dev->gpio_chan;
	descs[5] = &dev->gpio_pga1;
	descs[6] = &dev->gpio_pga2;
	descs[7] = &dev->gpio_odr1;
	descs[8] = &dev->gpio_odr2;

	params[0] = init_param->gpio_pdown;
	params[1] = init_param->gpio_rdy;
	params[2] = init_param->gpio_clksel;
	params[3] = init_param->gpio_temp;
	params[4] = init_param->gpio_chan;
	params[5] = init_param->gpio_pga1;
	params[6] = init_param->gpio_pga2;
	params[7] = init_param->gpio_odr1;
	params[8] = init_param->gpio_odr2;

	for (i = 0; i < 9; i++) {
		ret = no_os_gpio_get(descs[i], params[i]);
		if (ret)
			goto error_gpio;
	}

	/* PDOWN idles high, keeping the device powered down and reset. */
	ret = no_os_gpio_direction_output(dev->gpio_pdown, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_gpio;

	/* DOUT/RDY is sampled as an input for data-ready polling. */
	ret = no_os_gpio_direction_input(dev->gpio_rdy);
	if (ret)
		goto error_gpio;

	/* All configuration pins are outputs; start them low before applying. */
	for (i = 2; i < 9; i++) {
		ret = no_os_gpio_direction_output(*descs[i], NO_OS_GPIO_LOW);
		if (ret)
			goto error_gpio;
	}

	ret = ad7191_set_clock(dev, init_param->clock_sel);
	if (ret)
		goto error_gpio;

	ret = ad7191_set_channel(dev, init_param->channel);
	if (ret)
		goto error_gpio;

	ret = ad7191_set_gain(dev, init_param->gain);
	if (ret)
		goto error_gpio;

	ret = ad7191_set_odr(dev, init_param->odr);
	if (ret)
		goto error_gpio;

	*device = dev;

	return 0;

error_gpio:
	for (i = 0; i < 9; i++)
		no_os_gpio_remove(*descs[i]);
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by ad7191_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7191_remove(struct ad7191_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_pdown);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_rdy);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_clksel);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_temp);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_chan);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_pga1);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_pga2);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_odr1);
	if (ret)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_odr2);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

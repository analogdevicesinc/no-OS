/***************************************************************************//**
 *   @file
 *   @brief  Implementation of AD7768 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include "ad7768.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

const uint8_t standard_pin_ctrl_mode_sel[3][4] = {
//		MCLK/1,	MCLK/2,	MCLK/4,	MCLK/8
	{0x0,	0x1,	0x2,	0x3},	// Eco
	{0x4,	0x5,	0x6,	0x7},	// Median
	{0x8,	0x9,	0xA,	0xB},	// Fast
};

const uint8_t one_shot_pin_ctrl_mode_sel[3][4] = {
//		MCLK/1,	MCLK/2,	MCLK/4,	MCLK/8
	{0xC,	0xFF,	0xFF,	0xFF},	// Eco
	{0xD,	0xFF,	0xFF,	0xFF},	// Median
	{0xF,	0xE,	0xFF,	0xFF},	// Fast
};

/**
 * SPI read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_spi_read(ad7768_dev *dev,
			uint8_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = 0x80 | (reg_addr & 0x7F);
	buf[1] = 0x00;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

	buf[0] = 0x80 | (reg_addr & 0x7F);
	buf[1] = 0x00;
	ret |= no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	*reg_data = buf[1];

	return ret;
}

/**
 * SPI write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_spi_write(ad7768_dev *dev,
			 uint8_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = (reg_addr & 0x7F);
	buf[1] = reg_data;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

	return ret;
}

/**
 * SPI read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_spi_read_mask(ad7768_dev *dev,
			     uint8_t reg_addr,
			     uint8_t mask,
			     uint8_t *data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad7768_spi_read(dev, reg_addr, &reg_data);
	*data = (reg_data & mask);

	return ret;
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_spi_write_mask(ad7768_dev *dev,
			      uint8_t reg_addr,
			      uint8_t mask,
			      uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad7768_spi_read(dev, reg_addr, &reg_data);
	reg_data &= ~mask;
	reg_data |= data;
	ret |= ad7768_spi_write(dev, reg_addr, reg_data);

	return ret;
}

/**
 * Get the device sleep mode.
 * @param dev - The device structure.
 * @param mode - The device sleep mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_sleep_mode(ad7768_dev *dev,
			      ad7768_sleep_mode *mode)
{
	*mode = dev->sleep_mode;

	return 0;
}

/**
 * Internal set MODEx pins function.
 * @param dev - The device structure.
 * @param state - The state of the MODEx pins.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_mode_pins(ad7768_dev *dev,
			     uint8_t state)
{
	int32_t ret;

	if (dev->gpio_mode0 && dev->gpio_mode1 &&
	    dev->gpio_mode2 && dev->gpio_mode3) {
		ret = no_os_gpio_set_value(dev->gpio_mode0,
					   ((state & 0x01) >> 0));
		ret |= no_os_gpio_set_value(dev->gpio_mode1,
					    ((state & 0x02) >> 1));
		ret |= no_os_gpio_set_value(dev->gpio_mode2,
					    ((state & 0x04) >> 2));
		ret |= no_os_gpio_set_value(dev->gpio_mode3,
					    ((state & 0x08) >> 3));
	} else {
		printf ("MODE GPIOs are not defined.");
		ret = -1;
	}

	return ret;
}

static int ad7768_set_clk_divs(ad7768_dev *dev,
			       unsigned int freq)
{
	unsigned int mclk, dclk, dclk_div, i;
	struct ad7768_freq_config f_cfg;
	unsigned int chan_per_doutx;
	int ret = 0;

	mclk = 32768000;

	//chan_per_doutx = st->chip_info->num_channels / st->datalines;
	chan_per_doutx = 1;

	for (i = 0; i < dev->avail_freq[dev->power_mode].n_freqs; i++) {
		f_cfg = dev->avail_freq[dev->power_mode].freq_cfg[i];
		if (freq == f_cfg.freq)
			break;
	}
	if (i == dev->avail_freq[dev->power_mode].n_freqs)
		return -EINVAL;

	dclk = f_cfg.freq * SAMPLE_SIZE * chan_per_doutx;
	if (dclk > mclk)
		return -EINVAL;

	/* Set dclk_div to the nearest power of 2 less than the original value */
	dclk_div = NO_OS_DIV_ROUND_CLOSEST_ULL(mclk, dclk);
	if (dclk_div > AD7768_MAX_DCLK_DIV)
		dclk_div = AD7768_MAX_DCLK_DIV;
	else if (no_os_hweight32(dclk_div) != 1)
		dclk_div = 1 << (no_os_find_last_set_bit(dclk_div) - 1);

	ret = ad7768_spi_write_mask(dev, AD7768_REG_INTERFACE_CFG,
			AD7768_INTERFACE_CFG_DCLK_DIV_MSK,
			AD7768_INTERFACE_CFG_DCLK_DIV_MODE(dclk_div));
	if (ret < 0)
		return ret;

	return ad7768_spi_write_mask(dev, AD7768_REG_CH_MODE_A,
				     AD7768_CH_MODE_DEC_RATE_MSK,
				     AD7768_CH_MODE_DEC_RATE_MODE(f_cfg.dec_rate));
}

static int ad7768_set_sampling_freq(ad7768_dev *dev,
				    unsigned int freq)
{
	int ret = 0;

	if (!freq)
		return -EINVAL;

	//mutex_lock(&st->lock);

	ret = ad7768_set_clk_divs(dev, freq);
	if (ret)
		goto freq_err;

	dev->sampling_freq = freq;

freq_err:
	//mutex_unlock(&st->lock);

	return ret;
}

static int ad7768_sync(ad7768_dev *dev)
{
	int ret;

	ret = ad7768_spi_write_mask(dev, AD7768_REG_DATA_CTRL,
				    AD7768_DATA_CONTROL_SPI_SYNC_MSK,
				    AD7768_DATA_CONTROL_SPI_SYNC_CLEAR);
	if (ret < 0)
		return ret;

	return ad7768_spi_write_mask(dev,  AD7768_REG_DATA_CTRL,
				    AD7768_DATA_CONTROL_SPI_SYNC_MSK,
				    AD7768_DATA_CONTROL_SPI_SYNC);
}

/**
 * Set the device power mode.
 * @param dev - The device structure.
 * @param mode - The device power mode.
 * 				 Accepted values: AD7768_ECO
 *								  AD7768_MEDIAN
 *								  AD7768_FAST
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_power_mode(ad7768_dev *dev,
			      ad7768_power_mode mode)
{
	struct ad7768_avail_freq avail_freq;
	int max_mode_freq;
	unsigned int regval;
	int ret;

	dev->power_mode = mode;

	regval = ad7768_map_power_mode_to_regval(mode);
	ret = ad7768_spi_write_mask(dev, AD7768_REG_PWR_MODE,
				    AD7768_POWER_MODE_POWER_MODE_MSK,
				    AD7768_POWER_MODE_POWER_MODE(regval));
	if (ret < 0)
		return ret;

	/* The values for the powermode correspond for mclk div */
	ret = ad7768_spi_write_mask(dev, AD7768_REG_PWR_MODE,
				    AD7768_POWER_MODE_MCLK_DIV_MSK,
				    AD7768_POWER_MODE_MCLK_DIV_MODE(regval));
	if (ret < 0)
		return ret;

	/* Set the max freq of the selected power mode */
	avail_freq = dev->avail_freq[mode];
	max_mode_freq = avail_freq.freq_cfg[avail_freq.n_freqs - 1].freq;
	ret = ad7768_set_sampling_freq(dev, max_mode_freq);
	if (ret < 0)
		return ret;

	ret = ad7768_sync(dev);
	if (ret < 0)
		return ret;

	return ret;
}

/**
 * Get the device power mode.
 * @param dev - The device structure.
 * @param mode - The device power mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_power_mode(ad7768_dev *dev,
			      ad7768_power_mode *mode)
{
	*mode = dev->power_mode;

	return 0;
}

/**
 * Set the DCLK divider.
 * @param dev - The device structure.
 * @param clk_div - The DCLK divider.
 * 					Accepted values: AD7768_DCLK_DIV_1
 *									 AD7768_DCLK_DIV_2
 *									 AD7768_DCLK_DIV_4
 *									 AD7768_DCLK_DIV_8
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_dclk_div(ad7768_dev *dev,
			    ad7768_dclk_div clk_div)
{
//	uint8_t mode_pins_state;

	if (dev->pin_spi_ctrl == AD7768_SPI_CTRL) {
		ad7768_spi_write_mask(dev,
				      AD7768_REG_INTERFACE_CFG,
				      AD7768_INTERFACE_CFG_DCLK_DIV(0x3),
				      AD7768_INTERFACE_CFG_DCLK_DIV(clk_div));
		dev->dclk_div = clk_div;
	} else {
			printf("Invalid DCLK_DIV for the current configuration.");

			return -1;
	}

	return 0;
}

/**
 * Get the DCLK divider.
 * @param dev - The device structure.
 * @param clk_div - The DCLK divider.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_dclk_div(ad7768_dev *dev,
			    ad7768_dclk_div *clk_div)
{
	*clk_div = dev->dclk_div;

	return 0;
}


/**
 * Set the conversion operation mode.
 * @param dev - The device structure.
 * @param conv_op - The conversion operation mode.
 * 					Accepted values: AD7768_STANDARD_CONV
 * 									 AD7768_ONE_SHOT_CONV
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_conv_op(ad7768_dev *dev,
			   ad7768_conv_op conv_op)
{
//	uint8_t mode_pins_state;

	if (dev->pin_spi_ctrl == AD7768_SPI_CTRL) {
		ad7768_spi_write_mask(dev,
				      AD7768_REG_DATA_CTRL,
				      AD7768_DATA_CTRL_SINGLE_SHOT_EN,
				      conv_op ? AD7768_DATA_CTRL_SINGLE_SHOT_EN : 0);
		dev->conv_op = conv_op;
	} else {
			printf("Invalid Conversion Operation for the current configuration.");

			return -1;
	}

	return 0;
}

/**
 * Get the conversion operation mode.
 * @param dev - The device structure.
 * @param conv_op - The conversion operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_conv_op(ad7768_dev *dev,
			   ad7768_conv_op *conv_op)
{
	*conv_op = dev->conv_op;

	return 0;
}

/**
 * Set the CRC selection.
 * @param dev - The device structure.
 * @param crc_sel - The CRC selection.
 * 					Accepted values: AD7768_NO_CRC
 * 									 AD7768_CRC_4
 * 									 AD7768_CRC_16
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_crc_sel(ad7768_dev *dev,
			   ad7768_crc_sel crc_sel)
{
	ad7768_spi_write_mask(dev,
			      AD7768_REG_INTERFACE_CFG,
			      AD7768_INTERFACE_CFG_CRC_SEL(0x3),
			      AD7768_INTERFACE_CFG_CRC_SEL(crc_sel));
	dev->crc_sel = crc_sel;

	return 0;
}

/**
 * Get the CRC selection.
 * @param dev - The device structure.
 * @param crc_sel - The CRC selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_crc_sel(ad7768_dev *dev,
			   ad7768_crc_sel *crc_sel)
{
	*crc_sel = dev->crc_sel;

	return 0;
}

/**
 * Set the channel state.
 * @param dev - The device structure.
 * @param ch - The channel number.
 * 			   Accepted values: AD7768_CH0
 * 			   					AD7768_CH1
 * 			   					AD7768_CH2
 * 			   					AD7768_CH3
 * 			   					AD7768_CH4
 * 			   					AD7768_CH5
 * 			   					AD7768_CH6
 * 			   					AD7768_CH7
 * @param state - The channel state.
 * 				  Accepted values: AD7768_ENABLED
 * 								   AD7768_STANDBY
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_ch_state(ad7768_dev *dev,
			    ad7768_ch ch,
			    ad7768_ch_state state)
{
	ad7768_spi_write_mask(dev,
			      AD7768_REG_CH_STANDBY,
			      AD7768_CH_STANDBY(ch),
			      state ? AD7768_CH_STANDBY(ch) : 0);
	dev->ch_state[ch] = state;

	return 0;
}

/**
 * Get the channel state.
 * @param dev - The device structure.
 * @param ch - The channel number.
 * 			   Accepted values: AD7768_CH0
 * 			   					AD7768_CH1
 * 			   					AD7768_CH2
 * 			   					AD7768_CH3
 * 			   					AD7768_CH4
 * 			   					AD7768_CH5
 * 			   					AD7768_CH6
 * 			   					AD7768_CH7
 * @param state - The channel state.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_ch_state(ad7768_dev *dev,
			    ad7768_ch ch,
			    ad7768_ch_state *state)
{
	*state = dev->ch_state[ch];

	return 0;
}

/**
 * Set the channel mode.
 * @param dev - The device structure.
 * @param ch - The channel number.
 * 			   Accepted values: AD7768_CH0
 * 			   					AD7768_CH1
 * 			   					AD7768_CH2
 * 			   					AD7768_CH3
 * 			   					AD7768_CH4
 * 			   					AD7768_CH5
 * 			   					AD7768_CH6
 * 			   					AD7768_CH7
 * @param mode - The channel mode.
 * 				 Accepted values: AD7768_MODE_A
 * 								  AD7768_MODE_B
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_ch_mode(ad7768_dev *dev,
			   ad7768_ch ch,
			   ad7768_ch_mode mode)
{
	ad7768_spi_write_mask(dev,
			      AD7768_REG_CH_MODE_SEL,
			      AD7768_CH_MODE(ch),
			      mode ? AD7768_CH_MODE(ch) : 0);
	dev->ch_mode[ch] = mode;

	return 0;
}

/**
 * Get the channel mode.
 * @param dev - The device structure.
 * @param ch - The channel number.
 * 			   Accepted values: AD7768_CH0
 * 			   					AD7768_CH1
 * 			   					AD7768_CH2
 * 			   					AD7768_CH3
 * 			   					AD7768_CH4
 * 			   					AD7768_CH5
 * 			   					AD7768_CH6
 * 			   					AD7768_CH7
 * @param mode - The channel mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_ch_mode(ad7768_dev *dev,
			   ad7768_ch ch,
			   ad7768_ch_mode *mode)
{
	*mode = dev->ch_mode[ch];

	return 0;
}

static void ad7768_set_available_sampl_freq(ad7768_dev *dev)
{
	unsigned int mode;
	unsigned int dec;
	unsigned int mclk = 32768000;
	struct ad7768_avail_freq *avail_freq;

	for (mode = 0; mode < AD7768_NUM_POWER_MODES; mode++) {
		avail_freq = &dev->avail_freq[mode];
		for (dec = NO_OS_ARRAY_SIZE(ad7768_dec_rate); dec > 0; dec--) {
			struct ad7768_freq_config freq_cfg;

			freq_cfg.dec_rate = dec - 1;
			freq_cfg.freq = mclk / (ad7768_dec_rate[dec - 1] *
					ad7768_mclk_div[mode]);

			avail_freq->freq_cfg[avail_freq->n_freqs++] = freq_cfg;
		}
	}

	/* The max frequency is not supported in one data line configuration */
	if (dev->datalines == 1)
		dev->avail_freq[AD7768_FAST].n_freqs--;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_setup(ad7768_dev **device,
		     ad7768_init_param init_param)
{
	ad7768_dev *dev;
	int32_t ret;

	dev = (ad7768_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev) {
		return -1;
	}

	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret)
		goto error;

	dev->pin_spi_input_value = init_param.pin_spi_input_value;

	dev->pin_spi_ctrl = dev->pin_spi_input_value ?
			    AD7768_SPI_CTRL : AD7768_PIN_CTRL;

	/* get out of reset state */
	ret = no_os_gpio_get_optional(&dev->gpio_reset, &init_param.gpio_reset);
	if (ret)
		goto error;

	dev->gpio_reset_value = init_param.gpio_reset_value;

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, dev->gpio_reset_value);
		if (ret)
			goto error;
	}

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		goto error;
	no_os_udelay(1660);
	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		goto error;

	dev->datalines = 8;

	ad7768_set_available_sampl_freq(dev);

	ret = ad7768_set_power_mode(dev, AD7768_FAST);
	if (ret)
		goto error;

	*device = dev;

	if (!ret)
		printf("AD7768 successfully initialized\n");

	return ret;

error:
	no_os_free(dev);

	return ret;
}

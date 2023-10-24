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
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

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
 * Set the device sleep mode.
 * @param dev - The device structure.
 * @param mode - The device sleep mode.
 * 				 Accepted values: AD7768_ACTIVE
 * 								  AD7768_SLEEP
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_sleep_mode(ad7768_dev *dev,
			      ad7768_sleep_mode mode)
{
	ad7768_spi_write_mask(dev,
			      AD7768_REG_PWR_MODE,
			      AD7768_PWR_MODE_SLEEP_MODE,
			      (mode ? AD7768_PWR_MODE_SLEEP_MODE : 0));
	dev->sleep_mode = mode;

	return 0;
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
	uint8_t mode_pins_state;

	if (dev->pin_spi_ctrl == AD7768_SPI_CTRL) {
		ad7768_spi_write_mask(dev,
				      AD7768_REG_PWR_MODE,
				      AD7768_PWR_MODE_POWER_MODE(0x3),
				      AD7768_PWR_MODE_POWER_MODE(mode));
		dev->power_mode = mode;
	} else {
		if (dev->conv_op == AD7768_STANDARD_CONV)
			mode_pins_state =
				standard_pin_ctrl_mode_sel[mode][dev->dclk_div];
		else
			mode_pins_state =
				one_shot_pin_ctrl_mode_sel[mode][dev->dclk_div];
		if (mode_pins_state != 0xFF) {
			dev->power_mode = mode;
			ad7768_set_mode_pins(dev, mode_pins_state);
		} else {
			printf("Invalid Power Mode for the current configuration.");

			return -1;
		}
	}

	return 0;
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
 * Set the MCLK divider.
 * @param dev - The device structure.
 * @param clk_div - The MCLK divider.
 * 					Accepted values: AD7768_MCLK_DIV_32
 *									 AD7768_MCLK_DIV_8
 *									 AD7768_MCLK_DIV_4
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_mclk_div(ad7768_dev *dev,
			    ad7768_mclk_div clk_div)
{
	ad7768_spi_write_mask(dev,
			      AD7768_REG_PWR_MODE,
			      AD7768_PWR_MODE_MCLK_DIV(0x3),
			      AD7768_PWR_MODE_MCLK_DIV(clk_div));
	dev->mclk_div = clk_div;

	return 0;
}

/**
 * Get the MCLK divider.
 * @param dev - The device structure.
 * @param clk_div - The MCLK divider.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_mclk_div(ad7768_dev *dev,
			    ad7768_mclk_div *clk_div)
{
	*clk_div = dev->mclk_div;

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
	uint8_t mode_pins_state;

	if (dev->pin_spi_ctrl == AD7768_SPI_CTRL) {
		ad7768_spi_write_mask(dev,
				      AD7768_REG_INTERFACE_CFG,
				      AD7768_INTERFACE_CFG_DCLK_DIV(0x3),
				      AD7768_INTERFACE_CFG_DCLK_DIV(clk_div));
		dev->dclk_div = clk_div;
	} else {
		if (dev->conv_op == AD7768_STANDARD_CONV)
			mode_pins_state =
				standard_pin_ctrl_mode_sel[dev->power_mode][clk_div];
		else
			mode_pins_state =
				one_shot_pin_ctrl_mode_sel[dev->power_mode][clk_div];
		if (mode_pins_state != 0xFF) {
			dev->dclk_div = clk_div;
			ad7768_set_mode_pins(dev, mode_pins_state);
		} else {
			printf("Invalid DCLK_DIV for the current configuration.");

			return -1;
		}
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
	uint8_t mode_pins_state;

	if (dev->pin_spi_ctrl == AD7768_SPI_CTRL) {
		ad7768_spi_write_mask(dev,
				      AD7768_REG_DATA_CTRL,
				      AD7768_DATA_CTRL_SINGLE_SHOT_EN,
				      conv_op ? AD7768_DATA_CTRL_SINGLE_SHOT_EN : 0);
		dev->conv_op = conv_op;
	} else {
		if (conv_op == AD7768_STANDARD_CONV)
			mode_pins_state =
				standard_pin_ctrl_mode_sel[dev->power_mode][dev->dclk_div];
		else
			mode_pins_state =
				one_shot_pin_ctrl_mode_sel[dev->power_mode][dev->dclk_div];
		if (mode_pins_state != 0xFF) {
			dev->conv_op = conv_op;
			ad7768_set_mode_pins(dev, mode_pins_state);
		} else {
			printf("Invalid Conversion Operation for the current configuration.");

			return -1;
		}
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
 * Set the mode configuration.
 * @param dev - The device structure.
 * @param mode - The channel mode.
 * 				 Accepted values: AD7768_MODE_A
 * 								  AD7768_MODE_B
 * @param filt_type - The filter type.
 * 					  Accepted values: AD7768_FILTER_WIDEBAND
 * 					  				   AD7768_FILTER_SINC,
 * @param dec_rate - The decimation rate.
 * 					 Accepted values: AD7768_DEC_X32
 * 					 				  AD7768_DEC_X64
 * 					 				  AD7768_DEC_X128
 * 					 				  AD7768_DEC_X256
 * 					 				  AD7768_DEC_X512
 * 					 				  AD7768_DEC_X1024
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_set_mode_config(ad7768_dev *dev,
			       ad7768_ch_mode mode,
			       ad7768_filt_type filt_type,
			       ad7768_dec_rate dec_rate)
{
	uint8_t reg_val;

	reg_val = ((filt_type == AD7768_FILTER_SINC) ? AD7768_CH_MODE_FILTER_TYPE : 0) |
		  AD7768_CH_MODE_DEC_RATE(dec_rate);
	if (mode == AD7768_MODE_A) {
		ad7768_spi_write(dev, AD7768_REG_CH_MODE_A, reg_val);
	} else {
		ad7768_spi_write(dev, AD7768_REG_CH_MODE_B, reg_val);
	}
	dev->filt_type[mode] = filt_type;
	dev->dec_rate[mode] = dec_rate;

	return 0;
}

/**
 * Get the mode configuration.
 * @param dev - The device structure.
 * @param mode - The channel mode.
 * @param filt_type - The filter type.
 * @param dec_rate - The decimation rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_get_mode_config(ad7768_dev *dev,
			       ad7768_ch_mode mode,
			       ad7768_filt_type *filt_type,
			       ad7768_dec_rate *dec_rate)
{
	*filt_type = dev->filt_type[mode];
	*dec_rate = dev->dec_rate[mode];

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

	ret = ad7768_setup_begin(&dev, init_param);
	if (ret)
		return ret;

	ret = ad7768_setup_finish(dev, init_param);
	if (ret)
		goto error;

	*device = dev;

	return 0;
error:
	no_os_spi_remove(dev->spi_desc);
	no_os_gpio_remove(dev->gpio_reset);
	no_os_free(dev);

	return ret;
}

/**
 * Begin initializing the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_setup_begin(ad7768_dev **device,
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

	ret = no_os_gpio_get(&dev->gpio_reset, &init_param.gpio_reset);
	if (ret)
		goto error_1;
	dev->gpio_reset_value = init_param.gpio_reset_value;

	ret = no_os_gpio_direction_output(dev->gpio_reset, dev->gpio_reset_value);
	if (ret)
		goto error_1;

	dev->mclk = init_param.mclk;
	dev->datalines = init_param.datalines;

	*device = dev;

	return 0;
error_1:
	no_os_spi_remove(dev->spi_desc);
error:
	no_os_free(dev);

	return ret;
}

/**
 * Finish initializing the device.
 * @param dev - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7768_setup_finish(ad7768_dev *dev,
			    ad7768_init_param init_param)
{
	int32_t ret = 0;

	ret |= no_os_gpio_get(&dev->gpio_mode0, &init_param.gpio_mode0);
	ret |= no_os_gpio_get(&dev->gpio_mode1, &init_param.gpio_mode1);
	ret |= no_os_gpio_get(&dev->gpio_mode2, &init_param.gpio_mode2);
	ret |= no_os_gpio_get(&dev->gpio_mode3, &init_param.gpio_mode3);

	if (dev->gpio_reset)
		ret |= no_os_gpio_direction_output(dev->gpio_reset, dev->gpio_reset_value);

	if (dev->gpio_mode0)
		ret |= no_os_gpio_direction_output(dev->gpio_mode0, NO_OS_GPIO_LOW);
	if (dev->gpio_mode1)
		ret |= no_os_gpio_direction_output(dev->gpio_mode1, NO_OS_GPIO_LOW);
	if (dev->gpio_mode2)
		ret |= no_os_gpio_direction_output(dev->gpio_mode2, NO_OS_GPIO_LOW);
	if (dev->gpio_mode3)
		ret |= no_os_gpio_direction_output(dev->gpio_mode3, NO_OS_GPIO_LOW);

	dev->sleep_mode = init_param.sleep_mode;
	dev->mclk_div = init_param.mclk_div;
	dev->crc_sel = init_param.crc_sel;
	if (dev->pin_spi_ctrl == AD7768_SPI_CTRL) {
		ad7768_set_sleep_mode(dev, dev->sleep_mode);
		ad7768_set_mclk_div(dev, dev->mclk_div);
		ad7768_set_crc_sel(dev, dev->crc_sel);
	}

	dev->power_mode = init_param.power_mode;
	dev->dclk_div = init_param.dclk_div;
	dev->conv_op = init_param.conv_op;
	ad7768_set_power_mode(dev, dev->power_mode);
	ad7768_set_dclk_div(dev, dev->dclk_div);
	ad7768_set_conv_op(dev, dev->conv_op);

	if (ret) {
		no_os_gpio_remove(dev->gpio_mode0);
		no_os_gpio_remove(dev->gpio_mode1);
		no_os_gpio_remove(dev->gpio_mode2);
		no_os_gpio_remove(dev->gpio_mode3);
	} else
		printf("AD7768 successfully initialized\n");

	return ret;
}

/**
 * Set clock divisors.
 * @param dev - The device structure.
 * @param freq - The required frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad7768_set_clk_divs(ad7768_dev *dev,
			       unsigned int freq)
{
	unsigned int mclk, dclk, dclk_div, i;
	struct ad7768_freq_config f_cfg;
	unsigned int chan_per_doutx;
	int ret;

	mclk = dev->mclk;

	chan_per_doutx = AD7768_NUM_CHANNELS / dev->datalines;

	for (i = 0; i < dev->avail_freq[dev->power_mode_raw].n_freqs; i++) {
		f_cfg = dev->avail_freq[dev->power_mode_raw].freq_cfg[i];
		if (freq == f_cfg.freq)
			break;
	}
	if (i == dev->avail_freq[dev->power_mode_raw].n_freqs)
		return -EINVAL;

	dclk = f_cfg.freq * AD7768_SAMPLE_SIZE * chan_per_doutx;
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
	if (ret)
		return ret;

	return ad7768_spi_write_mask(dev, AD7768_REG_CH_MODE_A,
				     AD7768_CH_MODE_DEC_RATE_MSK,
				     AD7768_CH_MODE_DEC_RATE(f_cfg.dec_rate));
}

/**
 * Set sampling frequency.
 * @param dev - The device structure.
 * @param freq - The required sampling frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad7768_set_sampling_freq(ad7768_dev *dev,
				    unsigned int freq)
{
	int ret;

	if (!freq)
		return -EINVAL;

	ret = ad7768_set_clk_divs(dev, freq);
	if (ret)
		return ret;

	dev->sampling_freq = freq;

	return 0;
}

/**
 * Sync.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad7768_sync(ad7768_dev *dev)
{
	int ret;

	ret = ad7768_spi_write_mask(dev, AD7768_REG_DATA_CTRL,
				    AD7768_DATA_CONTROL_SPI_SYNC_MSK,
				    AD7768_DATA_CONTROL_SPI_SYNC_CLEAR);
	if (ret)
		return ret;

	return ad7768_spi_write_mask(dev, AD7768_REG_DATA_CTRL,
				     AD7768_DATA_CONTROL_SPI_SYNC_MSK,
				     AD7768_DATA_CONTROL_SPI_SYNC);
}

/**
 * Set available sampling frequency.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
void ad7768_set_available_sampl_freq(ad7768_dev *dev)
{
	unsigned int mode;
	unsigned int dec;
	unsigned int mclk = dev->mclk;
	struct ad7768_avail_freq *avail_freq;

	for (mode = 0; mode < AD7768_NUM_POWER_MODES; mode++) {
		avail_freq = &dev->avail_freq[mode];
		for (dec = NO_OS_ARRAY_SIZE(ad7768_dec_rate_vals); dec > 0; dec--) {
			struct ad7768_freq_config freq_cfg;

			freq_cfg.dec_rate = dec - 1;
			freq_cfg.freq = mclk / (ad7768_dec_rate_vals[dec - 1] *
						ad7768_mclk_div_vals[mode]);

			avail_freq->freq_cfg[avail_freq->n_freqs++] = freq_cfg;
		}
	}

	/* The max frequency is not supported in one data line configuration */
	if (dev->datalines == 1)
		dev->avail_freq[AD7768_FAST_MODE].n_freqs--;
}

/**
 * Set power mode and sampling frequency.
 * @param dev - The device structure.
 * @param mode - The required power mode (raw value).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7768_set_power_mode_and_sampling_freq(ad7768_dev *dev,
		enum ad7768_power_modes_raw mode)
{
	struct ad7768_avail_freq avail_freq;
	int max_mode_freq;
	uint8_t regval;
	int ret;

	dev->power_mode_raw = mode;

	regval = ad7768_map_power_mode_to_regval(mode);
	ret = ad7768_spi_write_mask(dev, AD7768_REG_PWR_MODE,
				    AD7768_PWR_MODE_POWER_MODE_MSK,
				    AD7768_PWR_MODE_POWER_MODE(regval));
	if (ret)
		return ret;

	/* The values for the powermode correspond for mclk div */
	ret = ad7768_spi_write_mask(dev, AD7768_REG_PWR_MODE,
				    AD7768_PWR_MODE_MCLK_DIV_MSK,
				    AD7768_PWR_MODE_MCLK_DIV(regval));
	if (ret)
		return ret;

	/* Set the max freq of the selected power mode */
	avail_freq = dev->avail_freq[mode];
	max_mode_freq = avail_freq.freq_cfg[avail_freq.n_freqs - 1].freq;
	ret = ad7768_set_sampling_freq(dev, max_mode_freq);
	if (ret)
		return ret;

	return ad7768_sync(dev);
}

/**
 * Free the resources allocated by ad7768_setup().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7768_remove(ad7768_dev *dev)
{
	no_os_spi_remove(dev->spi_desc);

	no_os_gpio_remove(dev->gpio_reset);
	no_os_gpio_remove(dev->gpio_mode0);
	no_os_gpio_remove(dev->gpio_mode1);
	no_os_gpio_remove(dev->gpio_mode2);
	no_os_gpio_remove(dev->gpio_mode3);

	no_os_free(dev);

	return 0;
}

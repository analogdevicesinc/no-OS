/***************************************************************************//**
 *   @file   ad7779.c
 *   @brief  Implementation of AD7779 Driver.
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
#include "ad7779.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/*************************** Constants Definitions ****************************/
/******************************************************************************/
const uint8_t pin_mode_options[16][4] = {
	/*GAIN_1, GAIN_2, GAIN_4, GAIN_8 */
	{0x03,	0xFF,	0x07,	0xFF},	// DEC_RATE_128, HIGH_RES, EXT_REF
	{0x0A,	0xFF,	0xFF,	0xFF},	// DEC_RATE_128, HIGH_RES, INT_REF
	{0x0D,	0xFF,	0xFF,	0xFF},	// DEC_RATE_128, LOW_PWR, EXT_REF
	{0x0E,	0xFF,	0xFF,	0xFF},	// DEC_RATE_128, LOW_PWR, INT_REF
	{0x02,	0x04,	0x06,	0xFF},	// DEC_RATE_256, HIGH_RES, EXT_REF
	{0x09,	0xFF,	0xFF,	0xFF},	// DEC_RATE_256, HIGH_RES, INT_REF
	{0x0C,	0xFF,	0xFF,	0xFF},	// DEC_RATE_256, LOW_PWR, EXT_REF
	{0x0F,	0xFF,	0xFF,	0xFF},	// DEC_RATE_256, LOW_PWR, INT_REF
	{0x01,	0xFF,	0x05,	0xFF},	// DEC_RATE_512, HIGH_RES, EXT_REF
	{0x08,	0xFF,	0xFF,	0xFF},	// DEC_RATE_512, HIGH_RES, INT_REF
	{0x08,	0xFF,	0xFF,	0xFF},	// DEC_RATE_512, LOW_PWR, EXT_REF
	{0xFF,	0xFF,	0xFF,	0xFF},	// DEC_RATE_512, LOW_PWR, INT_REF
	{0x00,	0xFF,	0xFF,	0xFF},	// DEC_RATE_1024, HIGH_RES, EXT_REF
	{0xFF,	0xFF,	0xFF,	0xFF},	// DEC_RATE_1024, HIGH_RES, INT_REF
	{0xFF,	0xFF,	0xFF,	0xFF},	// DEC_RATE_1024, LOW_PWR, EXT_REF
	{0xFF,	0xFF,	0xFF,	0xFF},	// DEC_RATE_1024, LOW_PWR, INT_REF
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * Compute CRC8 checksum.
 * @param data - The data buffer.
 * @param data_size - The size of the data buffer.
 * @return CRC8 checksum.
 */
uint8_t ad7779_compute_crc8(uint8_t *data,
			    uint8_t data_size)
{
	uint8_t i;
	uint8_t crc = 0;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= AD7779_CRC8_POLY;
			} else
				crc <<= 1;
		}
		data++;
		data_size--;
	}

	return crc;
}

/**
 * SPI internal register read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_spi_int_reg_read(ad7779_dev *dev,
				uint8_t reg_addr,
				uint8_t *reg_data)
{
	uint8_t buf[3];
	uint8_t buf_size = 2;
	uint8_t crc;
	int32_t ret;

	buf[0] = 0x80 | (reg_addr & 0x7F);
	buf[1] = 0x00;
	buf[2] = 0x00;
	if (dev->spi_crc_en == AD7779_ENABLE)
		buf_size = 3;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, buf_size);

	*reg_data = buf[1];
	if (dev->spi_crc_en == AD7779_ENABLE) {
		buf[0] = 0x80 | (reg_addr & 0x7F);
		crc = ad7779_compute_crc8(&buf[0], 2);
		if (crc != buf[2]) {
			printf("%s: CRC Error.\n", __func__);
			ret = -1;
		}
	}

	return ret;
}

/**
 * SPI internal register write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_spi_int_reg_write(ad7779_dev *dev,
				 uint8_t reg_addr,
				 uint8_t reg_data)
{
	uint8_t buf[3];
	uint8_t buf_size = 2;
	int32_t ret;

	buf[0] = 0x00 | (reg_addr & 0x7F);
	buf[1] = reg_data;
	if (dev->spi_crc_en == AD7779_ENABLE) {
		buf[2] = ad7779_compute_crc8(&buf[0], 2);
		buf_size = 3;
	}
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, buf_size);
	dev->cached_reg_val[reg_addr] = reg_data;

	return ret;
}

/**
 * SPI internal register read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_spi_int_reg_read_mask(ad7779_dev *dev,
				     uint8_t reg_addr,
				     uint8_t mask,
				     uint8_t *data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad7779_spi_int_reg_read(dev, reg_addr, &reg_data);
	*data = (reg_data & mask);

	return ret;
}

/**
 * SPI internal register write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_spi_int_reg_write_mask(ad7779_dev *dev,
				      uint8_t reg_addr,
				      uint8_t mask,
				      uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	reg_data = dev->cached_reg_val[reg_addr];
	reg_data &= ~mask;
	reg_data |= data;
	ret = ad7779_spi_int_reg_write(dev, reg_addr, reg_data);

	return ret;
}

/**
 * SPI SAR conversion code read.
 * @param dev - The device structure.
 * @param mux_next_conv - The SAR mux input configuration for the next
 *			  conversion.
 *			  Accepted values: AD7779_AUXAINP_AUXAINN
 *					   AD7779_DVBE_AVSSX
 *					   AD7779_REF1P_REF1N
 *					   AD7779_REF2P_REF2N
 *					   AD7779_REF_OUT_AVSSX
 *					   AD7779_VCM_AVSSX
 *					   AD7779_AREG1CAP_AVSSX_ATT
 *					   AD7779_AREG2CAP_AVSSX_ATT
 *					   AD7779_DREGCAP_DGND_ATT
 *					   AD7779_AVDD1A_AVSSX_ATT
 *					   AD7779_AVDD1B_AVSSX_ATT
 *					   AD7779_AVDD2A_AVSSX_ATT
 *					   AD7779_AVDD2B_AVSSX_ATT
 *					   AD7779_IOVDD_DGND_ATT
 *					   AD7779_AVDD4_AVSSX
 *					   AD7779_DGND_AVSS1A_ATT
 *					   AD7779_DGND_AVSS1B_ATT
 *					   AD7779_DGND_AVSSX_ATT
 *					   AD7779_AVDD4_AVSSX_ATT
 *					   AD7779_REF1P_AVSSX
 *					   AD7779_REF2P_AVSSX
 *					   AD7779_AVSSX_AVDD4_ATT
 * @param sar_code - SAR conversion code.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_spi_sar_read_code(ad7779_dev *dev,
				 ad7779_sar_mux mux_next_conv,
				 uint16_t *sar_code)
{
	uint8_t buf[3];
	uint8_t buf_size = 2;
	uint8_t crc;
	int32_t ret;

	buf[0] = 0x00 | (AD7779_REG_GLOBAL_MUX_CONFIG & 0x7F);
	buf[1] = AD7779_GLOBAL_MUX_CTRL(mux_next_conv);
	if (dev->spi_crc_en == AD7779_ENABLE) {
		buf[2] = ad7779_compute_crc8(&buf[0], 2);
		buf_size = 3;
	}
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, buf_size);
	dev->cached_reg_val[AD7779_REG_GLOBAL_MUX_CONFIG] =
		AD7779_GLOBAL_MUX_CTRL(mux_next_conv);
	buf[0] = buf[0] & 0x0F;
	*sar_code = (buf[0] << 8) | buf[1];
	if (dev->spi_crc_en == AD7779_ENABLE) {
		crc = ad7779_compute_crc8(&buf[0], 2);
		if (crc != buf[2]) {
			printf("%s: CRC Error.\n", __func__);
			ret = -1;
		}
	}

	return ret;
}

/**
 * Set SPI operation mode.
 * @param dev - The device structure.
 * @param mode - The SPI operation mode.
 *		 Accepted values: AD7779_INT_REG
 *				  AD7779_SD_CONV
 *				  AD7779_SAR_CONV
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_spi_op_mode(ad7779_dev *dev,
			       ad7779_spi_op_mode mode)
{
	int32_t ret;
	uint8_t cfg_2;
	uint8_t cfg_3;

	switch (mode) {
	case AD7779_SD_CONV:
		cfg_2 = 0;
		cfg_3 = AD7779_SPI_SLAVE_MODE_EN;
		break;
	case AD7779_SAR_CONV:
		cfg_2 = AD7779_SAR_DIAG_MODE_EN;
		cfg_3 = 0;
		break;
	default:	// AD7779_INT_REG
		cfg_2 = 0;
		cfg_3 = 0;
	}
	ret = ad7779_spi_int_reg_write_mask(dev,
					    AD7779_REG_GENERAL_USER_CONFIG_2,
					    AD7779_SAR_DIAG_MODE_EN,
					    cfg_2);
	ret |= ad7779_spi_int_reg_write_mask(dev,
					     AD7779_REG_GENERAL_USER_CONFIG_3,
					     AD7779_SPI_SLAVE_MODE_EN,
					     cfg_3);
	dev->spi_op_mode = mode;

	return ret;
}

/**
 * Get SPI operation mode.
 * @param dev - The device structure.
 * @param mode - The SPI operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_spi_op_mode(ad7779_dev *dev,
			       ad7779_spi_op_mode *mode)
{
	int32_t ret;
	uint8_t cfg_2;
	uint8_t cfg_3;
	uint8_t reg_data;

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_GENERAL_USER_CONFIG_2, &reg_data);
		if (ret)
			return ret;

		cfg_2 = no_os_field_get(AD7779_SAR_DIAG_MODE_EN, reg_data);

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_GENERAL_USER_CONFIG_3,
					      &reg_data);
		if (ret)
			return ret;

		cfg_3 = no_os_field_get(AD7779_SPI_SLAVE_MODE_EN, reg_data);

		if ((cfg_2 == 0) && (cfg_3 == 1))
			*mode = AD7779_SD_CONV;
		else if ((cfg_3 == 0) && (cfg_2 == 1))
			*mode = AD7779_SAR_CONV;
		else
			*mode = AD7779_INT_REG;
	} else {
		*mode = dev->spi_op_mode;
	}

	return 0;
}

/**
 * Update the state of the MODEx pins according to the settings specified in
 * the device structure.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_do_update_mode_pins(ad7779_dev *dev)
{
	int32_t ret;
	uint8_t option_index;
	uint8_t mode;

	if (!(dev->gain[AD7779_CH0] == dev->gain[AD7779_CH1] &&
	      dev->gain[AD7779_CH1] == dev->gain[AD7779_CH2] &&
	      dev->gain[AD7779_CH2] == dev->gain[AD7779_CH3] &&
	      dev->gain[AD7779_CH3] == AD7779_GAIN_1))
		goto error;

	if (!(dev->gain[AD7779_CH4] == dev->gain[AD7779_CH5] &&
	      dev->gain[AD7779_CH5] == dev->gain[AD7779_CH6] &&
	      dev->gain[AD7779_CH6] == dev->gain[AD7779_CH7]))
		goto error;

	switch (dev->dec_rate_int) {
	case 128:
		option_index = 0;
		break;
	case 256:
		option_index = 4;
		break;
	case 512:
		option_index = 8;
		break;
	case 1024:
		option_index = 12;
		break;
	default:
		goto error;
	}

	if (dev->pwr_mode == AD7779_HIGH_RES)
		if (dev->ref_type == AD7779_EXT_REF)
			mode = pin_mode_options[option_index + 0][dev->gain[AD7779_CH4]];
		else
			mode = pin_mode_options[option_index + 1][dev->gain[AD7779_CH4]];
	else if (dev->ref_type == AD7779_EXT_REF)
		mode = pin_mode_options[option_index + 2][dev->gain[AD7779_CH4]];
	else
		mode = pin_mode_options[option_index + 3][dev->gain[AD7779_CH4]];

	if (mode == 0xFF)
		goto error;

	ret = no_os_gpio_set_value(dev->gpio_mode0,
				   ((mode & 0x01) >> 0));
	ret |= no_os_gpio_set_value(dev->gpio_mode1,
				    ((mode & 0x02) >> 1));
	ret |= no_os_gpio_set_value(dev->gpio_mode2,
				    ((mode & 0x04) >> 2));
	ret |= no_os_gpio_set_value(dev->gpio_mode3,
				    ((mode & 0x08) >> 3));

	/* All the pins that define the AD7779 configuration mode are re-evaluated
	 * every time SYNC_IN pin is pulsed. */
	ret |= no_os_gpio_set_value(dev->gpio_sync_in, NO_OS_GPIO_LOW);
	no_os_mdelay(10);
	ret |= no_os_gpio_set_value(dev->gpio_sync_in, NO_OS_GPIO_HIGH);

	return ret;

error:
	printf("%s: This setting can't be set in PIN control mode.\n",
	       __func__);
	return -1;
}

/**
 * Set the state (enable, disable) of the channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param state - The channel state.
 *		  Accepted values: AD7779_ENABLE
 *				   AD7779_DISABLE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_state(ad7779_dev *dev,
			 ad7779_ch ch,
			 ad7779_state state)
{
	int32_t ret;

	ret = ad7779_spi_int_reg_write_mask(dev,
					    AD7779_REG_CH_DISABLE,
					    AD7779_CH_DISABLE(ch),
					    state ? AD7779_CH_DISABLE(ch) : 0);
	dev->state[ch] = state;

	return ret;
}

/**
 * Get the state (enable, disable) of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param state - The channel state.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_state(ad7779_dev *dev,
			 ad7779_ch ch,
			 ad7779_state *state)
{
	int32_t ret;
	uint8_t reg_data;

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_DISABLE, &reg_data);
		if (ret)
			return ret;

		*state = no_os_field_get(AD7779_CH_DISABLE(ch), reg_data);
	} else {
		*state = dev->state[ch];
	}

	return 0;
}

/**
 * Set the gain of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param gain - The gain value.
 *		 Accepted values: AD7779_GAIN_1
 *				  AD7779_GAIN_2
 *				  AD7779_GAIN_4
 *				  AD7779_GAIN_8
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_gain(ad7779_dev *dev,
			ad7779_ch ch,
			ad7779_gain gain)
{
	int32_t ret;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		if (ch <= AD7779_CH3) {
			dev->gain[AD7779_CH0] = gain;
			dev->gain[AD7779_CH1] = gain;
			dev->gain[AD7779_CH2] = gain;
			dev->gain[AD7779_CH3] = gain;
		} else {
			dev->gain[AD7779_CH4] = gain;
			dev->gain[AD7779_CH5] = gain;
			dev->gain[AD7779_CH6] = gain;
			dev->gain[AD7779_CH7] = gain;
		}
		ret = ad7779_do_update_mode_pins(dev);
	} else {
		dev->gain[ch] = gain;
		ret = ad7779_spi_int_reg_write_mask(dev,
						    AD7779_REG_CH_CONFIG(ch),
						    AD7779_CH_GAIN(0x3),
						    AD7779_CH_GAIN(gain));
	}

	return ret;
}

/**
 * Get the gain of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param gain - The gain value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_gain(ad7779_dev *dev,
			ad7779_ch ch,
			ad7779_gain *gain)
{
	int32_t ret;
	uint8_t reg_data;

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_CONFIG(ch), &reg_data);
		if (ret)
			return ret;
		*gain = no_os_field_get(AD7779_CH_GAIN(0x3), reg_data);
	} else {
		*gain = dev->gain[ch];
	}

	return 0;
}

/**
 * Set the decimation rate.
 * @param dev - The device structure.
 * @param int_val - The integer value.
 * @param dec_val - The decimal value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_dec_rate(ad7779_dev *dev,
			    uint16_t int_val,
			    uint16_t dec_val)
{
	int32_t ret;
	uint8_t msb;
	uint8_t lsb;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		switch (int_val) {
		case 128:
			break;
		case 256:
			break;
		case 512:
			break;
		case 1024:
			break;
		default:
			printf("%s: This setting can't be set in PIN control mode.\n",
			       __func__);
			return -1;
		}
		dev->dec_rate_int = int_val;
		dev->dec_rate_int = dec_val;
		ret = ad7779_do_update_mode_pins(dev);
	} else {
		msb = (int_val & 0x0F00) >> 8;
		lsb = (int_val & 0x00FF) >> 0;
		ret = ad7779_spi_int_reg_write(dev,
					       AD7779_REG_SRC_N_MSB,
					       msb);
		ret |= ad7779_spi_int_reg_write(dev,
						AD7779_REG_SRC_N_LSB,
						lsb);
		dec_val = (dec_val * 65536) / 1000;
		msb = (dec_val & 0xFF00) >> 8;
		lsb = (dec_val & 0x00FF) >> 0;
		ret |= ad7779_spi_int_reg_write(dev,
						AD7779_REG_SRC_IF_MSB,
						msb);
		ret |= ad7779_spi_int_reg_write(dev,
						AD7779_REG_SRC_IF_LSB,
						lsb);
		dev->dec_rate_int = int_val;
		dev->dec_rate_int = dec_val;
	}

	return ret;
}

/**
 * Get the decimation rate.
 * @param dev - The device structure.
 * @param int_val - The integer value.
 * @param dec_val - The decimal value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_dec_rate(ad7779_dev *dev,
			    uint16_t *int_val,
			    uint16_t *dec_val)
{
	int32_t ret;
	uint8_t reg_data;

	if (!dev->read_from_cache) {

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_SRC_IF_LSB, &reg_data);
		if (ret)
			return ret;
		*dec_val = reg_data;
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_SRC_IF_MSB, &reg_data);
		if (ret)
			return ret;
		*dec_val |= reg_data << 8;

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_SRC_N_LSB, &reg_data);
		if (ret)
			return ret;
		*int_val = reg_data;
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_SRC_N_MSB, &reg_data);
		if (ret)
			return ret;
		*int_val |= reg_data << 8;
		*dec_val = *dec_val * 1000 / 65536;
	} else {
		*int_val = dev->dec_rate_int;
		*dec_val = dev->dec_rate_int;
	}

	return 0;
}

/**
 * Set the power mode.
 * @param dev - The device structure.
 * @param pwr_mode - The power mode.
 *		     Accepted values: AD7779_HIGH_RES
 *				      AD7779_LOW_PWR
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_power_mode(ad7779_dev *dev,
			      ad7779_pwr_mode pwr_mode)
{
	int32_t ret;

	ret = ad7779_spi_int_reg_write_mask(dev,
					    AD7779_REG_GENERAL_USER_CONFIG_1,
					    AD7779_MOD_POWERMODE,
					    pwr_mode ? AD7779_MOD_POWERMODE : 0);
	dev->pwr_mode = pwr_mode;

	return ret;
}

/**
 * Get the power mode.
 * @param dev - The device structure.
 * @param pwr_mode - The power mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_power_mode(ad7779_dev *dev,
			      ad7779_pwr_mode *pwr_mode)
{
	int32_t ret;
	uint8_t reg_data;

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_GENERAL_USER_CONFIG_1, &reg_data);
		if (ret)
			return ret;
		*pwr_mode = no_os_field_get(AD7779_MOD_POWERMODE, reg_data);
	} else {
		*pwr_mode = dev->pwr_mode;
	}

	return 0;
}

/**
 * Set the reference type.
 * @param dev - The device structure.
 * @param ref_type - The reference type.
 *		     Accepted values: AD7779_EXT_REF
 *				      AD7779_INT_REF
 *				      AD7779_EXT_SUPPLY
 *				      AD7779_EXT_REF_INV
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_reference_type(ad7779_dev *dev,
				  ad7779_ref_type ref_type)
{
	int32_t ret;

	if (ref_type == AD7779_INT_REF)
		ret = ad7779_spi_int_reg_write_mask(dev,
						    AD7779_REG_GENERAL_USER_CONFIG_1,
						    AD7779_PDB_REFOUT_BUF,
						    AD7779_PDB_REFOUT_BUF);
	else
		ret = ad7779_spi_int_reg_write_mask(dev,
						    AD7779_REG_GENERAL_USER_CONFIG_1,
						    AD7779_PDB_REFOUT_BUF,
						    0);
	if (ret)
		return ret;

	ret = ad7779_spi_int_reg_write_mask(dev,
					    AD7779_REG_ADC_MUX_CONFIG,
					    AD7779_REF_MUX_CTRL(0x3),
					    AD7779_REF_MUX_CTRL(ref_type));
	if (ret)
		return ret;

	dev->ref_type = ref_type;

	return 0;
}

/**
 * Get the reference type.
 * @param dev - The device structure.
 * @param ref_type - The reference type.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_reference_type(ad7779_dev *dev,
				  ad7779_ref_type *ref_type)
{
	int32_t ret;
	uint8_t reg_data;

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_ADC_MUX_CONFIG, &reg_data);
		if (ret)
			return ret;
		*ref_type = no_os_field_get(AD7779_REF_MUX_CTRL(0x3), reg_data);
	} else {
		*ref_type = dev->ref_type;
	}

	return 0;
}

/**
 * Set the DCLK divider.
 * @param dev - The device structure.
 * @param div - The DCLK divider.
 *		Accepted values: AD7779_DCLK_DIV_1
 *				 AD7779_DCLK_DIV_2
 *				 AD7779_DCLK_DIV_4
 *				 AD7779_DCLK_DIV_8
 *				 AD7779_DCLK_DIV_16
 *				 AD7779_DCLK_DIV_32
 *				 AD7779_DCLK_DIV_64
 *				 AD7779_DCLK_DIV_128
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_dclk_div(ad7779_dev *dev,
			    ad7779_dclk_div div)
{
	int32_t ret;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		ret = no_os_gpio_set_value(dev->gpio_dclk0,
					   ((div & 0x01) >> 0));
		ret |= no_os_gpio_set_value(dev->gpio_dclk1,
					    ((div & 0x02) >> 1));
		ret |= no_os_gpio_set_value(dev->gpio_dclk2,
					    ((div & 0x04) >> 2));
	} else {
		ret = ad7779_spi_int_reg_write_mask(dev,
						    AD7779_REG_DOUT_FORMAT,
						    AD7779_DCLK_CLK_DIV(0x7),
						    AD7779_DCLK_CLK_DIV(div));
	}
	dev->dclk_div = div;

	return ret;
}

/**
 * Get the DCLK divider.
 * @param dev - The device structure.
 * @param div - The DCLK divider.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_dclk_div(ad7779_dev *dev,
			    ad7779_dclk_div *div)
{
	int32_t ret;
	uint8_t reg_data;

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_DOUT_FORMAT, &reg_data);
		if (ret)
			return ret;
		*div = no_os_field_get(AD7779_DCLK_CLK_DIV(0x7), reg_data);
	} else {
		*div = dev->dclk_div;
	}

	return 0;
}

/**
 * Set the synchronization offset of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param sync_offset - The synchronization offset value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_sync_offset(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint8_t sync_offset)
{
	int32_t ret;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	ret = ad7779_spi_int_reg_write(dev,
				       AD7779_REG_CH_SYNC_OFFSET(ch),
				       sync_offset);
	dev->sync_offset[ch] = sync_offset;

	return ret;
}

/**
 * Get the synchronization offset of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param sync_offset - The synchronization offset value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_sync_offset(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint8_t *sync_offset)
{
	int32_t ret;
	uint8_t reg_data;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_SYNC_OFFSET(ch), &reg_data);
		if (ret)
			return ret;
		*sync_offset = reg_data;
	} else {
		*sync_offset = dev->sync_offset[ch];
	}

	return 0;
}

/**
 * Set the offset correction of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param offset - The offset value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_offset_corr(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint32_t offset)
{
	int32_t ret;
	uint8_t upper_byte;
	uint8_t mid_byte;
	uint8_t lower_byte;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	upper_byte = (offset & 0xFF0000) >> 16;
	mid_byte = (offset & 0x00FF00) >> 8;
	lower_byte = (offset & 0x0000FF) >> 0;
	ret = ad7779_spi_int_reg_write(dev,
				       AD7779_REG_CH_OFFSET_UPPER_BYTE(ch),
				       upper_byte);
	ret |= ad7779_spi_int_reg_write(dev,
					AD7779_REG_CH_OFFSET_MID_BYTE(ch),
					mid_byte);
	ret |= ad7779_spi_int_reg_write(dev,
					AD7779_REG_CH_OFFSET_LOWER_BYTE(ch),
					lower_byte);
	dev->offset_corr[ch] = offset;

	return ret;
}

/**
 * Get the offset correction of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param offset - The offset value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_offset_corr(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint32_t *offset)
{
	int32_t ret;
	uint8_t reg_data;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_OFFSET_LOWER_BYTE(ch),
					      &reg_data);
		if (ret)
			return ret;
		*offset = reg_data;

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_OFFSET_MID_BYTE(ch),
					      &reg_data);
		if (ret)
			return ret;
		*offset |= reg_data << 8;

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_OFFSET_UPPER_BYTE(ch),
					      &reg_data);
		if (ret)
			return ret;
		*offset |= reg_data << 16;
	} else {
		*offset = dev->offset_corr[ch];
	}

	return 0;
}

/**
 * Set the gain correction of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param gain - The gain value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_gain_corr(ad7779_dev *dev,
			     ad7779_ch ch,
			     uint32_t gain)
{
	int32_t ret;
	uint8_t upper_byte;
	uint8_t mid_byte;
	uint8_t lower_byte;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	gain &= 0xFFFFFF;
	upper_byte = (gain & 0xff0000) >> 16;
	mid_byte = (gain & 0x00ff00) >> 8;
	lower_byte = (gain & 0x0000ff) >> 0;
	ret = ad7779_spi_int_reg_write(dev,
				       AD7779_REG_CH_GAIN_UPPER_BYTE(ch),
				       upper_byte);
	ret |= ad7779_spi_int_reg_write(dev,
					AD7779_REG_CH_GAIN_MID_BYTE(ch),
					mid_byte);
	ret |= ad7779_spi_int_reg_write(dev,
					AD7779_REG_CH_GAIN_LOWER_BYTE(ch),
					lower_byte);
	dev->gain_corr[ch] = gain;

	return ret;
}

/**
 * Get the gain correction of the selected channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 *	       Accepted values: AD7779_CH0
 *				AD7779_CH1
 *				AD7779_CH2
 *				AD7779_CH3
 *				AD7779_CH4
 *				AD7779_CH5
 *				AD7779_CH6
 *				AD7779_CH7
 * @param gain - The gain value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_gain_corr(ad7779_dev *dev,
			     ad7779_ch ch,
			     uint32_t *gain)
{
	int32_t ret;
	uint8_t reg_data;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_GAIN_LOWER_BYTE(ch),
					      &reg_data);
		if (ret)
			return ret;
		*gain  = reg_data;

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_GAIN_MID_BYTE(ch), &reg_data);
		if (ret)
			return ret;
		*gain |= reg_data << 8;

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_CH_GAIN_UPPER_BYTE(ch),
					      &reg_data);
		if (ret)
			return ret;
		*gain |= reg_data << 16;
	} else {
		*gain = dev->gain_corr[ch];
	}

	return 0;
}

/**
 * Set the reference buffer operation mode of the selected pin.
 * @param dev - The device structure.
 * @param refx_pin - The selected pin.
 *		     Accepted values: AD7779_REFX_P
 *				      AD7779_REFX_N
 * @param mode - The reference buffer operation mode.
 *		 Accepted values: AD7779_REF_BUF_ENABLED
 *				  AD7779_REF_BUF_PRECHARGED
 *				  AD7779_REF_BUF_DISABLED
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_ref_buf_op_mode(ad7779_dev *dev,
				   ad7779_refx_pin refx_pin,
				   ad7779_ref_buf_op_mode mode)
{
	int32_t ret;
	uint8_t config_1;
	uint8_t config_2;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	if (refx_pin == AD7779_REFX_P) {
		switch (mode) {
		case AD7779_REF_BUF_ENABLED:
			config_1 = AD7779_REF_BUF_POS_EN;
			config_2 = 0;
			break;
		case AD7779_REF_BUF_PRECHARGED:
			config_1 = AD7779_REF_BUF_POS_EN;
			config_2 = AD7779_REFBUFP_PREQ;
			break;
		default:
			config_1 = 0;
			config_2 = 0;
		}
		ret = ad7779_spi_int_reg_write_mask(dev,
						    AD7779_REG_BUFFER_CONFIG_1,
						    AD7779_REF_BUF_POS_EN,
						    config_1);
		ret |= ad7779_spi_int_reg_write_mask(dev,
						     AD7779_REG_BUFFER_CONFIG_2,
						     AD7779_REFBUFP_PREQ,
						     config_2);
	} else {
		switch (mode) {
		case AD7779_REF_BUF_ENABLED:
			config_1 = AD7779_REF_BUF_NEG_EN;
			config_2 = 0;
			break;
		case AD7779_REF_BUF_PRECHARGED:
			config_1 = AD7779_REF_BUF_NEG_EN;
			config_2 = AD7779_REFBUFN_PREQ;
			break;
		default:
			config_1 = 0;
			config_2 = 0;
		}
		ret = ad7779_spi_int_reg_write_mask(dev,
						    AD7779_REG_BUFFER_CONFIG_1,
						    AD7779_REF_BUF_NEG_EN,
						    config_1);
		ret |= ad7779_spi_int_reg_write_mask(dev,
						     AD7779_REG_BUFFER_CONFIG_2,
						     AD7779_REFBUFN_PREQ,
						     config_2);
	}
	dev->ref_buf_op_mode[refx_pin] = mode;

	return ret;
}

/**
 * Get the reference buffer operation mode of the selected pin.
 * @param dev - The device structure.
 * @param refx_pin - The selected pin.
 *		     Accepted values: AD7779_REFX_P
 *				      AD7779_REFX_N
 * @param mode - The reference buffer operation mode.
 *		 Accepted values: AD7779_REF_BUF_ENABLED
 *				  AD7779_REF_BUF_PRECHARGED
 *				  AD7779_REF_BUF_DISABLED
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_ref_buf_op_mode(ad7779_dev *dev,
				   ad7779_refx_pin refx_pin,
				   ad7779_ref_buf_op_mode *mode)
{
	int32_t ret;
	uint8_t cfg_1;
	uint8_t cfg_2;
	uint8_t reg_data;
	uint8_t cfg_1_mask;
	uint8_t cfg_2_mask;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	if (refx_pin == AD7779_REFX_P) {
		cfg_1_mask = AD7779_REF_BUF_POS_EN;
		cfg_2_mask = AD7779_REFBUFP_PREQ;
	} else {
		cfg_1_mask = AD7779_REF_BUF_NEG_EN;
		cfg_2_mask = AD7779_REFBUFN_PREQ;
	}

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_BUFFER_CONFIG_1, &reg_data);
		if (ret)
			return ret;
		cfg_1 = no_os_field_get(cfg_1_mask, reg_data);

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_BUFFER_CONFIG_2, &reg_data);
		if (ret)
			return ret;
		cfg_2 = no_os_field_get(cfg_2_mask, reg_data);

		if ((cfg_1 == 1) && (cfg_2 == 0))
			*mode = AD7779_REF_BUF_ENABLED;
		else if ((cfg_1 == 1) && (cfg_2 == 1))
			*mode = AD7779_REF_BUF_PRECHARGED;
		else if (cfg_1 == 0 && cfg_2 == 0)
			*mode = AD7779_REF_BUF_DISABLED;
		else
			return -1;
	} else {
		*mode = dev->ref_buf_op_mode[refx_pin];
	}

	return 0;
}

/**
 * Set the SAR ADC configuration.
 * @param dev - The device structure.
 * @param state - The SAR ADC state.
 *		  Accepted values: AD7779_ENABLE
 *				   AD7779_DISABLE
 * @param mux - The SAR mux input configuration.
 *		Accepted values: AD7779_AUXAINP_AUXAINN
 *				 AD7779_DVBE_AVSSX
 *				 AD7779_REF1P_REF1N
 *				 AD7779_REF2P_REF2N
 *				 AD7779_REF_OUT_AVSSX
 *				 AD7779_VCM_AVSSX
 *				 AD7779_AREG1CAP_AVSSX_ATT
 *				 AD7779_AREG2CAP_AVSSX_ATT
 *				 AD7779_DREGCAP_DGND_ATT
 *				 AD7779_AVDD1A_AVSSX_ATT
 *				 AD7779_AVDD1B_AVSSX_ATT
 *				 AD7779_AVDD2A_AVSSX_ATT
 *				 AD7779_AVDD2B_AVSSX_ATT
 *				 AD7779_IOVDD_DGND_ATT
 *				 AD7779_AVDD4_AVSSX
 *				 AD7779_DGND_AVSS1A_ATT
 *				 AD7779_DGND_AVSS1B_ATT
 *				 AD7779_DGND_AVSSX_ATT
 *				 AD7779_AVDD4_AVSSX_ATT
 *				 AD7779_REF1P_AVSSX
 *				 AD7779_REF2P_AVSSX
 *				 AD7779_AVSSX_AVDD4_ATT
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_set_sar_cfg(ad7779_dev *dev,
			   ad7779_state state,
			   ad7779_sar_mux mux)
{
	int32_t ret;

	ret = ad7779_spi_int_reg_write_mask(dev,
					    AD7779_REG_GENERAL_USER_CONFIG_1,
					    AD7779_PDB_SAR,
					    (state == AD7779_ENABLE) ?
					    AD7779_PDB_SAR : 0);
	ret |= ad7779_spi_int_reg_write(dev,
					AD7779_REG_GLOBAL_MUX_CONFIG,
					AD7779_GLOBAL_MUX_CTRL(mux));
	dev->sar_state = state;
	dev->sar_mux = mux;

	return ret;
}

/**
 * Get the SAR ADC configuration.
 * @param dev - The device structure.
 * @param state - The SAR ADC state.
 * @param mux - The SAR mux input configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_get_sar_cfg(ad7779_dev *dev,
			   ad7779_state *state,
			   ad7779_sar_mux *mux)
{
	int32_t ret;
	uint8_t reg_data;
	uint8_t pdb_sar_val;

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_GENERAL_USER_CONFIG_1,
					      &reg_data);
		if (ret)
			return ret;
		pdb_sar_val = no_os_field_get(AD7779_PDB_SAR, reg_data);
		*state = (pdb_sar_val == 1 ? AD7779_ENABLE : AD7779_DISABLE);

		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_GLOBAL_MUX_CONFIG, &reg_data);
		if (ret)
			return ret;
		*mux = no_os_field_get(AD7779_GLOBAL_MUX_CTRL(0x1F), reg_data);
	} else {
		*state = dev->sar_state;
		*mux = dev->sar_mux;
	}

	return 0;
}

/**
 * Do a single SAR conversion.
 * @param dev - The device structure.
 * @param mux - The SAR mux input configuration.
 *		Accepted values: AD7779_AUXAINP_AUXAINN
 *				 AD7779_DVBE_AVSSX
 *				 AD7779_REF1P_REF1N
 *				 AD7779_REF2P_REF2N
 *				 AD7779_REF_OUT_AVSSX
 *				 AD7779_VCM_AVSSX
 *				 AD7779_AREG1CAP_AVSSX_ATT
 *				 AD7779_AREG2CAP_AVSSX_ATT
 *				 AD7779_DREGCAP_DGND_ATT
 *				 AD7779_AVDD1A_AVSSX_ATT
 *				 AD7779_AVDD1B_AVSSX_ATT
 *				 AD7779_AVDD2A_AVSSX_ATT
 *				 AD7779_AVDD2B_AVSSX_ATT
 *				 AD7779_IOVDD_DGND_ATT
 *				 AD7779_AVDD4_AVSSX
 *				 AD7779_DGND_AVSS1A_ATT
 *				 AD7779_DGND_AVSS1B_ATT
 *				 AD7779_DGND_AVSSX_ATT
 *				 AD7779_AVDD4_AVSSX_ATT
 *				 AD7779_REF1P_AVSSX
 *				 AD7779_REF2P_AVSSX
 *				 AD7779_AVSSX_AVDD4_ATT
 * @param sar_code - SAR conversion code.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_do_single_sar_conv(ad7779_dev *dev,
				  ad7779_sar_mux mux,
				  uint16_t *sar_code)
{
	ad7779_spi_op_mode restore_spi_op_mode;
	ad7779_state restore_sar_state;
	int32_t ret;

	restore_spi_op_mode = dev->spi_op_mode;
	restore_sar_state = dev->sar_state;
	ret = ad7779_set_sar_cfg(dev, AD7779_ENABLE, mux);
	ret |= ad7779_set_spi_op_mode(dev, AD7779_SAR_CONV);
	ret |= no_os_gpio_set_value(dev->gpio_convst_sar, NO_OS_GPIO_LOW);
	no_os_mdelay(10);	// Acquisition Time = min 500 ns
	ret |= no_os_gpio_set_value(dev->gpio_convst_sar, NO_OS_GPIO_HIGH);
	no_os_mdelay(10);	// Conversion Time = max 3.4 us
	ad7779_spi_sar_read_code(dev, mux, sar_code);
	ret |= ad7779_set_sar_cfg(dev, restore_sar_state, mux);
	ret |= ad7779_set_spi_op_mode(dev, restore_spi_op_mode);

	return ret;
}

/**
 * Do a SPI software reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_do_spi_soft_reset(ad7779_dev *dev)
{
	uint8_t buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	int32_t ret;

	/* Keeping the SDI pin high during 64 consecutives clocks generates a
	   software reset */
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 8);

	return ret;
}

/**
 * Set the state (enable, disable) of the SINC5 filter.
 * @param dev - The device structure.
 * @param state - The SINC5 filter state.
 *		  Accepted values: AD7779_ENABLE
 *				   AD7779_DISABLE
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7771_set_sinc5_filter_state(ad7779_dev *dev,
				      ad7779_state state)
{
	int32_t ret;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	ret = ad7779_spi_int_reg_write_mask(dev,
					    AD7779_REG_GENERAL_USER_CONFIG_2,
					    AD7771_FILTER_MODE,
					    (state == AD7779_ENABLE) ?
					    AD7771_FILTER_MODE : 0);
	dev->sinc5_state = state;

	return ret;
}

/**
 * Get the state (enable, disable) of the SINC5 filter.
 * @param dev - The device structure.
 * @param state - The SINC5 filter state.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7771_get_sinc5_filter_state(ad7779_dev *dev,
				      ad7779_state *state)
{
	int32_t ret;
	uint8_t reg_data;

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		printf("%s: This feature is not available in PIN control mode.\n",
		       __func__);
		return -1;
	}

	if (!dev->read_from_cache) {
		ret = ad7779_spi_int_reg_read(dev, AD7779_REG_GENERAL_USER_CONFIG_2, &reg_data);
		if (ret)
			return ret;
		*state = no_os_field_get(AD7771_FILTER_MODE, reg_data);
	} else
		*state = dev->sinc5_state;

	return 0;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7779_init(ad7779_dev **device,
		    ad7779_init_param init_param)
{
	ad7779_dev *dev;
	uint8_t i;
	int32_t ret;

	dev = (ad7779_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	ret |= no_os_gpio_get(&dev->gpio_reset,
			      &init_param.gpio_reset);
	ret |= no_os_gpio_get(&dev->gpio_mode0,
			      &init_param.gpio_mode0);
	ret |= no_os_gpio_get(&dev->gpio_mode1,
			      &init_param.gpio_mode1);
	ret |= no_os_gpio_get(&dev->gpio_mode2,
			      &init_param.gpio_mode2);
	ret |= no_os_gpio_get(&dev->gpio_mode3,
			      &init_param.gpio_mode3);
	ret |= no_os_gpio_get(&dev->gpio_dclk0,
			      &init_param.gpio_dclk0);
	ret |= no_os_gpio_get(&dev->gpio_dclk1,
			      &init_param.gpio_dclk1);
	ret |= no_os_gpio_get(&dev->gpio_dclk2,
			      &init_param.gpio_dclk2);
	ret |= no_os_gpio_get(&dev->gpio_sync_in,
			      &init_param.gpio_sync_in);
	ret |= no_os_gpio_get(&dev->gpio_convst_sar,
			      &init_param.gpio_convst_sar);

	ret |= no_os_gpio_direction_output(dev->gpio_reset,
					   NO_OS_GPIO_LOW);
	no_os_mdelay(10);	// RESET Hold Time = min 2 x MCLK
	ret |= no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	no_os_mdelay(10);	// RESET Rising Edge to First DRDY = min 225 us
	ret |= no_os_gpio_direction_output(dev->gpio_mode0,
					   NO_OS_GPIO_LOW);
	ret |= no_os_gpio_direction_output(dev->gpio_mode1,
					   NO_OS_GPIO_LOW);
	ret |= no_os_gpio_direction_output(dev->gpio_mode2,
					   NO_OS_GPIO_LOW);
	ret |= no_os_gpio_direction_output(dev->gpio_mode3,
					   NO_OS_GPIO_LOW);
	ret |= no_os_gpio_direction_output(dev->gpio_dclk0,
					   NO_OS_GPIO_LOW);
	ret |= no_os_gpio_direction_output(dev->gpio_dclk1,
					   NO_OS_GPIO_LOW);
	ret |= no_os_gpio_direction_output(dev->gpio_dclk2,
					   NO_OS_GPIO_LOW);
	ret |= no_os_gpio_direction_output(dev->gpio_sync_in,
					   NO_OS_GPIO_HIGH);
	ret |= no_os_gpio_direction_output(dev->gpio_convst_sar,
					   NO_OS_GPIO_HIGH);
	/* Device Settings */
	dev->ctrl_mode = init_param.ctrl_mode;
	dev->spi_crc_en = AD7779_DISABLE;
	dev->spi_op_mode = AD7779_INT_REG;
	dev->sar_state = AD7779_DISABLE;
	dev->sar_mux = AD7779_AUXAINP_AUXAINN;
	dev->read_from_cache = init_param.read_from_cache;

	if ((dev->ctrl_mode == AD7779_SPI_CTRL) &&
	    (init_param.spi_crc_en == AD7779_ENABLE)) {
		ret |= ad7779_spi_int_reg_read(dev,
					       AD7779_REG_GEN_ERR_REG_1_EN,
					       &dev->cached_reg_val[AD7779_REG_GEN_ERR_REG_1_EN]);
		ret |= ad7779_spi_int_reg_write_mask(dev,
						     AD7779_REG_GEN_ERR_REG_1_EN,
						     AD7779_SPI_CRC_TEST_EN,
						     AD7779_SPI_CRC_TEST_EN);
		dev->spi_crc_en = AD7779_ENABLE;
	}

	if (dev->ctrl_mode == AD7779_SPI_CTRL)
		for (i = AD7779_REG_CH_CONFIG(0); i <= AD7779_REG_SRC_UPDATE; i++)
			ret |= ad7779_spi_int_reg_read(dev, i, &dev->cached_reg_val[i]);

	for (i = AD7779_CH0; i <= AD7779_CH7; i++) {
		dev->state[i] = init_param.state[i];
		if (dev->ctrl_mode == AD7779_SPI_CTRL)
			ret |= ad7779_set_state(dev, (ad7779_ch)i, dev->state[i]);
	}

	no_os_mdelay(50);

	for (i = AD7779_CH0; i <= AD7779_CH7; i++) {
		dev->gain[i] = init_param.gain[i];
		if (dev->ctrl_mode == AD7779_SPI_CTRL)
			ret |= ad7779_set_gain(dev, (ad7779_ch)i, dev->gain[i]);
	}

	dev->dec_rate_int = init_param.dec_rate_int;
	dev->dec_rate_dec = init_param.dec_rate_dec;
	if (dev->ctrl_mode == AD7779_SPI_CTRL)
		ret |= ad7779_set_dec_rate(dev, dev->dec_rate_int, dev->dec_rate_dec);

	ret |= ad7779_spi_int_reg_write(dev, AD7779_REG_SRC_UPDATE, 0x01);
	ret |= ad7779_spi_int_reg_write(dev, AD7779_REG_SRC_UPDATE, 0x00);

	dev->ref_type = init_param.ref_type;
	if (dev->ctrl_mode == AD7779_SPI_CTRL)
		ret |= ad7779_set_reference_type(dev, dev->ref_type);

	dev->pwr_mode = init_param.pwr_mode;
	if (dev->ctrl_mode == AD7779_SPI_CTRL)
		ret |= ad7779_set_power_mode(dev, dev->pwr_mode);

	if (dev->ctrl_mode == AD7779_PIN_CTRL) {
		ret |= ad7779_do_update_mode_pins(dev);
	}

	dev->dclk_div = init_param.dclk_div;
	ad7779_set_dclk_div(dev, dev->dclk_div);

	for (i = AD7779_CH0; i <= AD7779_CH7; i++) {
		dev->sync_offset[i] = init_param.sync_offset[i];
		dev->offset_corr[i] = init_param.offset_corr[i];
		dev->gain_corr[i] = init_param.gain_corr[i];
		if (dev->ctrl_mode == AD7779_SPI_CTRL) {
			ret |= ad7779_set_sync_offset(dev, (ad7779_ch)i,
						      dev->sync_offset[i]);
			ret |= ad7779_set_offset_corr(dev, (ad7779_ch)i,
						      dev->offset_corr[i]);
			ret |= ad7779_set_gain_corr(dev, (ad7779_ch)i,
						    dev->gain_corr[i]);
		}
	}

	*device = dev;

	if (!ret)
		printf("AD7779 successfully initialized\n");
	else
		printf("AD7779 initialization error (%d)\n", ret);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7779_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad7779_remove(ad7779_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_reset);
	ret |= no_os_gpio_remove(dev->gpio_mode0);
	ret |= no_os_gpio_remove(dev->gpio_mode1);
	ret |= no_os_gpio_remove(dev->gpio_mode2);
	ret |= no_os_gpio_remove(dev->gpio_mode3);
	ret |= no_os_gpio_remove(dev->gpio_dclk0);
	ret |= no_os_gpio_remove(dev->gpio_dclk1);
	ret |= no_os_gpio_remove(dev->gpio_dclk2);
	ret |= no_os_gpio_remove(dev->gpio_sync_in);
	ret |= no_os_gpio_remove(dev->gpio_convst_sar);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 *   @file   ad7293.c
 *   @brief  Implementation of ad7293 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <malloc.h>
#include "ad7293.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Set specific AD7293 page.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
static int ad7293_page_select(struct ad7293_dev *dev, unsigned int reg)
{
	int ret;
	uint8_t data[2];

	if (dev->page_select != no_os_field_get(AD7293_PAGE_ADDR_MSK, reg)) {
		data[0] = no_os_field_get(AD7293_REG_ADDR_MSK, AD7293_REG_PAGE_SELECT);
		data[1] = no_os_field_get(AD7293_PAGE_ADDR_MSK, reg);

		ret = no_os_spi_write_and_read(dev->spi_desc, data, 2);
		if (ret)
			return ret;

		dev->page_select = no_os_field_get(AD7293_PAGE_ADDR_MSK, reg);
	}

	return 0;
}

/**
 * @brief Reads data from AD7293 over SPI.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ad7293_spi_read(struct ad7293_dev *dev, unsigned int reg, uint16_t *val)
{
	uint8_t buff[AD7293_BUFF_SIZE_BYTES];
	unsigned int length;
	int ret;

	length = no_os_field_get(AD7293_TRANSF_LEN_MSK, reg);

	ret = ad7293_page_select(dev, reg);
	if (ret)
		return ret;

	buff[0] = AD7293_READ | no_os_field_get(AD7293_REG_ADDR_MSK, reg);
	buff[1] = 0x0;
	buff[2] = 0x0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, length + 1);
	if (ret)
		return ret;

	if (length == 1)
		*val = buff[1];
	else
		*val = no_os_get_unaligned_be16(&buff[1]);

	return 0;
}

/**
 * @brief Writes data to AD7293 over SPI.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ad7293_spi_write(struct ad7293_dev *dev, unsigned int reg, uint16_t val)
{
	uint8_t buff[AD7293_BUFF_SIZE_BYTES];
	unsigned int length;
	int ret;

	length = no_os_field_get(AD7293_TRANSF_LEN_MSK, reg);

	ret = ad7293_page_select(dev, reg);
	if (ret)
		return ret;

	buff[0] = no_os_field_get(AD7293_REG_ADDR_MSK, reg);

	if (length == 1)
		buff[1] = val;
	else
		no_os_put_unaligned_be16(val, &buff[1]);

	return no_os_spi_write_and_read(dev->spi_desc, buff, length + 1);
}

/**
 * @brief Update AD7293 register.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param val - Data written to the device (requires prior bit shifting).
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ad7293_spi_update_bits(struct ad7293_dev *dev, unsigned int reg,
			   uint16_t mask, uint16_t val)
{
	int ret;
	uint16_t data, temp;

	ret = ad7293_spi_read(dev, reg, &data);
	if (ret)
		return ret;

	temp = (data & ~mask) | (val & mask);

	return ad7293_spi_write(dev, reg, temp);
}

/**
 * @brief Get the range value for ADC channels.
 * @param dev - The device structure.
 * @param ch - the channel number.
 * @param range - the range value.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_adc_get_range(struct ad7293_dev *dev, unsigned int ch,
			 uint16_t *range)
{
	int ret;
	uint16_t data;

	ret = ad7293_spi_read(dev, AD7293_REG_VINX_RANGE1, &data);
	if (ret)
		return ret;

	*range = AD7293_REG_VINX_RANGE_GET_CH_MSK(data, ch);

	ret = ad7293_spi_read(dev, AD7293_REG_VINX_RANGE0, &data);
	if (ret)
		return ret;

	*range |= AD7293_REG_VINX_RANGE_GET_CH_MSK(data, ch) << 1;

	return 0;
}

/**
 * @brief Set the range value for ADC channels.
 * @param dev - The device structure.
 * @param ch - the channel number.
 * @param range - the range value.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_adc_set_range(struct ad7293_dev *dev, unsigned int ch,
			 uint16_t range)
{
	int ret;
	unsigned int ch_msk = NO_OS_BIT(ch);

	ret = ad7293_spi_update_bits(dev, AD7293_REG_VINX_RANGE1, ch_msk,
				     AD7293_REG_VINX_RANGE_SET_CH_MSK(range, ch));
	if (ret)
		return ret;

	return ad7293_spi_update_bits(dev, AD7293_REG_VINX_RANGE0, ch_msk,
				      AD7293_REG_VINX_RANGE_SET_CH_MSK((range >> 1), ch));
}

/**
 * @brief Set the gain value for ISENSE channels.
 * @param dev - The device structure.
 * @param ch - the channel number.
 * @param gain - the range value.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_isense_set_gain(struct ad7293_dev *dev, unsigned int ch,
			   uint16_t gain)
{
	unsigned int ch_msk = (0xf << (4 * ch));

	return ad7293_spi_update_bits(dev, AD7293_REG_ISENSE_GAIN, ch_msk,
				      gain << (4 * ch));
}

/**
 * @brief Get the gain value for ISENSE channels.
 * @param dev - The device structure.
 * @param ch - the channel number.
 * @param gain - the gain read.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_isense_get_gain(struct ad7293_dev *dev, unsigned int ch,
			   uint16_t *gain)
{
	int ret;

	ret = ad7293_spi_read(dev, AD7293_REG_ISENSE_GAIN, gain);
	if (ret)
		return ret;

	*gain = (*gain >> (4 * ch)) & 0xf;

	return 0;
}

/**
 * @brief Set offset value for specific channel and channel type.
 * @param dev - The device structure.
 * @param type - The channel type.
 * @param ch - the channel number.
 * @param offset - the raw value to be written.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_get_offset(struct ad7293_dev *dev,  enum ad7293_ch_type type,
		      unsigned int ch, uint16_t *offset)
{
	unsigned int reg_rd;

	switch (type) {
	case AD7293_ADC_VINX:
		reg_rd = AD7293_REG_VIN0_OFFSET;

		break;
	case AD7293_ADC_TSENSE:
		reg_rd = AD7293_REG_TSENSE_INT_OFFSET;

		break;
	case AD7293_ADC_ISENSE:
		reg_rd = AD7293_REG_ISENSE0_OFFSET;

		break;
	case AD7293_DAC:
		reg_rd = AD7293_REG_UNI_VOUT0_OFFSET;

		break;
	default:
		return -EINVAL;
	}

	return ad7293_spi_read(dev, reg_rd + ch, offset);
}

/**
 * @brief Set offset value for specific channel and channel type.
 * @param dev - The device structure.
 * @param type - The channel type.
 * @param ch - the channel number.
 * @param offset - the raw value read.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_set_offset(struct ad7293_dev *dev,  enum ad7293_ch_type type,
		      unsigned int ch, uint16_t offset)
{
	unsigned int reg_wr;

	switch (type) {
	case AD7293_ADC_VINX:
		reg_wr = AD7293_REG_VIN0_OFFSET;

		break;
	case AD7293_ADC_TSENSE:
		reg_wr = AD7293_REG_TSENSE_INT_OFFSET;

		break;
	case AD7293_ADC_ISENSE:
		reg_wr = AD7293_REG_ISENSE0_OFFSET;

		break;
	case AD7293_DAC:
		reg_wr = AD7293_REG_UNI_VOUT0_OFFSET;

		break;
	default:
		return -EINVAL;
	}

	return ad7293_spi_write(dev, reg_wr + ch, offset);
}

/**
 * @brief Set the DAC output raw value.
 * @param dev - The device structure.
 * @param ch - the channel number.
 * @param raw - the raw value to be written.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_dac_write_raw(struct ad7293_dev *dev, unsigned int ch,
			 uint16_t raw)
{
	int ret;

	ret = ad7293_spi_update_bits(dev, AD7293_REG_DAC_EN, NO_OS_BIT(ch),
				     NO_OS_BIT(ch));
	if (ret)
		return ret;

	return ad7293_spi_write(dev, AD7293_REG_UNI_VOUT0 + ch,
				no_os_field_prep(AD7293_REG_DATA_RAW_MSK, raw));
}

/**
 * @brief Read raw value for specific channel and channel type.
 * @param dev - The device structure.
 * @param type - The channel type.
 * @param ch - the channel number.
 * @param raw - the raw value read.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_ch_read_raw(struct ad7293_dev *dev, enum ad7293_ch_type type,
		       unsigned int ch, uint16_t *raw)
{
	int ret;
	unsigned int reg_wr, reg_rd, data_wr;

	switch (type) {
	case AD7293_ADC_VINX:
		reg_wr = AD7293_REG_VINX_SEQ;
		reg_rd = AD7293_REG_VIN0 + ch;
		data_wr = NO_OS_BIT(ch);

		break;
	case AD7293_ADC_TSENSE:
		reg_wr = AD7293_REG_ISENSEX_TSENSEX_SEQ;
		reg_rd = AD7293_REG_TSENSE_INT + ch;
		data_wr = NO_OS_BIT(ch);

		break;
	case AD7293_ADC_ISENSE:
		reg_wr = AD7293_REG_ISENSEX_TSENSEX_SEQ;
		reg_rd = AD7293_REG_ISENSE_0 + ch;
		data_wr = NO_OS_BIT(ch) << 8;

		break;
	case AD7293_DAC:
		reg_rd = AD7293_REG_UNI_VOUT0 + ch;

		break;
	default:
		return -EINVAL;
	}

	if (type != AD7293_DAC) {
		if (type == AD7293_ADC_TSENSE) {
			ret = ad7293_spi_write(dev, AD7293_REG_TSENSE_BG_EN,
					       NO_OS_BIT(ch));
			if (ret)
				return ret;

			no_os_mdelay(9);
		} else if (type == AD7293_ADC_ISENSE) {
			ret = ad7293_spi_write(dev, AD7293_REG_ISENSE_BG_EN,
					       NO_OS_BIT(ch));
			if (ret)
				return ret;

			no_os_mdelay(9);
		}

		ret = ad7293_spi_write(dev, reg_wr, data_wr);
		if (ret)
			return ret;

		ret = ad7293_spi_write(dev, AD7293_REG_CONV_CMD, AD7293_CONV_CMD_VAL);
		if (ret)
			return ret;
	}

	ret = ad7293_spi_read(dev, reg_rd, raw);
	if (ret)
		return ret;

	*raw = no_os_field_get(AD7293_REG_DATA_RAW_MSK, *raw);

	return 0;
}

/**
 * @brief Perform software reset.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_soft_reset(struct ad7293_dev *dev)
{
	int ret;

	ret = ad7293_spi_write(dev, AD7293_REG_SOFT_RESET, AD7293_SOFT_RESET_VAL);
	if (ret)
		return ret;

	return ad7293_spi_write(dev, AD7293_REG_SOFT_RESET, AD7293_SOFT_RESET_CLR_VAL);
}

/**
 * @brief Perform both hardware and software reset.
 * @param dev- The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_reset(struct ad7293_dev *dev)
{
	if (dev->gpio_reset) {
		no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_LOW);
		/* Datasheet: Minimum Reset pulse width: 90ns */
		no_os_udelay(1);
		no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);
		/* Datasheet: Minimum Reset pulse width: 90ns */
		no_os_udelay(1);

		return 0;
	}

	/* Perform a software reset */
	return ad7293_soft_reset(dev);
}

/**
 * @brief Initializes the ad7293.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int ad7293_init(struct ad7293_dev **device,
		struct ad7293_init_param *init_param)
{
	struct ad7293_dev *dev;
	uint16_t chip_id;
	int ret;

	dev = (struct ad7293_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_get_optional(&dev->gpio_reset, init_param->gpio_reset);
	if (ret)
		goto error_spi;

	dev->page_select = 0;

	ret = ad7293_reset(dev);
	if (ret)
		goto error_gpio;

	/* Check Chip ID */
	ret = ad7293_spi_read(dev, AD7293_REG_DEVICE_ID, &chip_id);
	if (ret)
		goto error_gpio;

	if (chip_id != AD7293_CHIP_ID) {
		ret = -EINVAL;
		goto error_gpio;
	}

	*device = dev;

	return 0;

error_gpio:
	no_os_gpio_remove(dev->gpio_reset);
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief AD7293 Resources Deallocation.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ad7293_remove(struct ad7293_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

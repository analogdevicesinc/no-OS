/***************************************************************************//**
 *   @file   ad4858.c
 *   @brief  Source file for the ad4858 drivers
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ad4858.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 * @note Multibyte write with streaming mode is supported by default.
 */
int ad4858_reg_write(struct ad4858_dev *dev, uint32_t reg_addr,
		     uint32_t reg_val)
{
	int32_t ret;
	uint32_t addr;
	uint8_t cnt = 0;
	uint8_t reg_size = AD4858_LEN(reg_addr);
	uint8_t buf[6] = {0};

	if (!dev)
		return -EINVAL;

	if (!dev->addr_ascension_enable)
		addr = AD4858_ADDR(reg_addr) + reg_size - 1;
	else
		addr = AD4858_ADDR(reg_addr);

	buf[cnt++] = (uint8_t)(addr >> 8);
	buf[cnt++] = (uint8_t)addr;

	if (dev->big_endian) {
		if (dev->addr_ascension_enable)
			no_os_memswap64((uint8_t *)(&reg_val + 1) - reg_size, reg_size, reg_size);
		memcpy(&buf[cnt], (uint8_t *)(&reg_val + 1) - reg_size, reg_size);
	} else {
		if (!dev->addr_ascension_enable)
			no_os_memswap64(&reg_val, reg_size, reg_size);
		memcpy(&buf[cnt], &reg_val, reg_size);
	}

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_size;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, cnt);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 * @note Multibyte read with streaming mode is supported by default.
 */
int ad4858_reg_read(struct ad4858_dev *dev, uint32_t reg_addr,
		    uint32_t *reg_val)
{
	int32_t ret;
	uint32_t addr;
	uint8_t cnt = 0;
	uint8_t dindx = 0;
	uint8_t reg_size = AD4858_LEN(reg_addr);
	uint8_t buf[6] = {0};

	if (!dev || !reg_val)
		return -EINVAL;

	if (!dev->addr_ascension_enable)
		addr = AD4858_ADDR(reg_addr) + reg_size - 1;
	else
		addr = AD4858_ADDR(reg_addr);

	/* Store the address (16-bit instruction phase with R/W=1) */
	buf[cnt++] = (uint8_t)(AD4858_REG_RD_BIT_MSK | (addr >> 8));
	buf[cnt++] = (uint8_t)addr;

	/* Save the data phase start index */
	dindx = cnt;

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_size;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, cnt);
	if (ret)
		return ret;

	if (dev->big_endian) {
		memcpy((uint8_t *)(reg_val + 1) - reg_size, &buf[dindx], reg_size);
		if (!dev->addr_ascension_enable) {
			no_os_memswap64((uint8_t *)(reg_val + 1) - reg_size, reg_size, reg_size);
		}
	} else {
		memcpy(reg_val, &buf[dindx], reg_size);
		if (!dev->addr_ascension_enable) {
			no_os_memswap64(reg_val, reg_size, reg_size);
		}
	}

	return 0;
}

/**
 * @brief Update specific register bits of an input register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 * @note Multibyte read/write with streaming mode is supported by default.
 */
int ad4858_reg_mask(struct ad4858_dev *dev,
		    uint32_t reg_addr,
		    uint32_t mask,
		    uint32_t reg_val)
{
	int ret;
	uint32_t data;

	if (!dev)
		return -EINVAL;

	ret = ad4858_reg_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_val;

	return ad4858_reg_write(dev, reg_addr, data);
}

/**
 * @brief Set the device operating mode.
 * @param dev - Pointer to the device structure.
 * @param mode - Operating mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_operating_mode(struct ad4858_dev *dev,
			      enum ad4858_operating_mode mode)
{
	int ret;
	uint32_t val;

	if (!dev || (mode >= AD4858_NUM_OF_OP_MODES))
		return -EINVAL;

	val = no_os_field_prep(AD4858_OPERATING_MODES_MSK, mode);
	ret = ad4858_reg_mask(dev, AD4858_REG_DEVICE_CONFIG, AD4858_OPERATING_MODES_MSK,
			      val);
	if (ret)
		return ret;

	dev->operating_mode = mode;

	return 0;
}

/**
 * @brief Set the SPI data mode.
 * @param dev - Pointer to the device structure.
 * @param mode - SPI data mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_spi_data_mode(struct ad4858_dev *dev,
			     enum ad4858_spi_data_mode mode)
{
	int ret;
	uint32_t val;

	if (!dev || (mode >= AD4858_NUM_OF_SPI_DATA_MODES))
		return -EINVAL;

	val = no_os_field_prep(AD4858_SINGLE_INST_MSK, mode);
	ret = ad4858_reg_mask(dev, AD4858_REG_INTERFACE_CONFIG_B,
			      AD4858_SINGLE_INST_MSK,
			      val);
	if (ret)
		return ret;

	dev->spi_data_mode = mode;

	return 0;
}

/**
 * @brief Set device config interface mode.
 * @param dev - Pointer to the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_config_interface_mode(struct ad4858_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Use SDO0 as DOUT line (over CSDO).
	 * Note: This is done to use the same SDO0 line as output data line in 4-wire
	 * SPI mode for both config interface and data interface. Without this,
	 * two separate SPI interfaces are required each for register config and data
	 * interface in standard 3/4-wire SPI mode.
	 * */
	ret = ad4858_reg_write(dev, AD4858_REG_SPI_CONFIG_D, AD4858_CSDO_ON_SDO_MSK);
	if (ret)
		return ret;

	/* Enable SDO line for 4-wire SPI config interface mode */
	ret = ad4858_reg_write(dev, AD4858_REG_INTERFACE_CONFIG_A,
			       AD4858_SDO_ENABLE_MSK);
	if (ret)
		return ret;

	/* Set SPI data mode to streaming for multibyte register access */
	return ad4858_set_spi_data_mode(dev, AD4858_STREAMING_MODE);
}

/**
 * @brief Set device data interface mode.
 * @param dev - Pointer to the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_data_interface_mode(struct ad4858_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Set SPI data mode to single instruction mode*/
	ret = ad4858_set_spi_data_mode(dev, AD4858_SINGLE_INSTRUCTION_MODE);
	if (ret)
		return ret;

	/* Disable SDO line to get 3-wire SPI config interface mode */
	return ad4858_reg_mask(dev, AD4858_REG_INTERFACE_CONFIG_A,
			       AD4858_SDO_ENABLE_MSK, 0);
}

/**
 * @brief Enable OSR.
 * @param dev - Pointer to the device structure.
 * @param osr_status - OSR enable/disable status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_enable_osr(struct ad4858_dev *dev, bool osr_status)
{
	int ret;
	uint32_t val;

	if (!dev)
		return -EINVAL;

	val = no_os_field_prep(AD4858_OS_ENABLE_MSK, osr_status);
	ret = ad4858_reg_mask(dev, AD4858_REG_OVERSAMPLE, AD4858_OS_ENABLE_MSK,
			      val);
	if (ret)
		return ret;

	dev->osr_enable = osr_status;

	return 0;
}

/**
 * @brief Set OSR ratio.
 * @param dev - Pointer to the device structure.
 * @param osr_ratio - OSR ratio.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_osr_ratio(struct ad4858_dev *dev,
			 enum ad4858_osr_ratio osr_ratio)
{
	int ret;
	uint32_t val;

	if (!dev || (osr_ratio >= AD4858_NUM_OF_OSR_RATIO))
		return -EINVAL;

	val = no_os_field_prep(AD4858_OS_RATIO_MSK, osr_ratio);
	ret = ad4858_reg_mask(dev, AD4858_REG_OVERSAMPLE, AD4858_OS_RATIO_MSK,
			      val);
	if (ret)
		return ret;

	dev->osr_ratio = osr_ratio;

	return 0;
}

/**
 * @brief Set packet format.
 * @param dev - Pointer to the device structure.
 * @param packet_format - Packet format.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_packet_format(struct ad4858_dev *dev,
			     enum ad4858_packet_format packet_format)
{
	int ret;
	uint32_t val;

	if (!dev || (packet_format >= AD4858_NUM_OF_PACKETS))
		return -EINVAL;

	val = no_os_field_prep(AD4858_PACKET_FORMAT_MSK, packet_format);
	ret = ad4858_reg_mask(dev, AD4858_REG_PACKET, AD4858_PACKET_FORMAT_MSK,
			      val);
	if (ret)
		return ret;

	dev->packet_format = packet_format;

	return 0;
}

/**
 * @brief Enable/Disable test pattern on ADC data output.
 * @param dev - Pointer to the device structure.
 * @param test_pattern - Test pattern enable status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_enable_test_pattern(struct ad4858_dev *dev, bool test_pattern)
{
	int ret;
	uint32_t val;

	if (!dev)
		return -EINVAL;

	val = no_os_field_prep(AD4858_TEST_PATTERN_MSK, test_pattern);
	ret = ad4858_reg_mask(dev, AD4858_REG_PACKET, AD4858_TEST_PATTERN_MSK,
			      val);
	if (ret)
		return ret;

	dev->test_pattern = test_pattern;

	return 0;
}
/**
 * @brief Enable/Disable channel sleep.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param sleep_status - Sleep status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_enable_ch_sleep(struct ad4858_dev* dev, uint8_t chn,
			   enum ad4858_ch_sleep_value sleep_status)
{
	int ret;
	uint32_t mask;
	uint32_t val;
	uint32_t data;

	if (!dev || (chn >= AD4858_NUM_CHANNELS))
		return -EINVAL;

	ret = ad4858_reg_read(dev, AD4858_REG_CH_SLEEP, &data);
	if (ret)
		return ret;

	dev->chn_sleep_value[chn] = sleep_status;
	mask = 1 << chn;
	val = no_os_field_prep(mask, sleep_status);
	data &= ~mask;
	data |= val;

	return ad4858_reg_write(dev, AD4858_REG_CH_SLEEP, data);
}

/**
 * @brief Enable/Disable seamless hdr.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param seamless_hdr_status - seamless hdr status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_enable_ch_seamless_hdr(struct ad4858_dev* dev, uint8_t chn,
				  enum ad4858_ch_seamless_hdr seamless_hdr_status)
{
	int ret;
	uint32_t val;
	uint32_t mask;
	uint32_t data;

	if (!dev || (chn >= AD4858_NUM_CHANNELS))
		return -EINVAL;

	ret = ad4858_reg_read(dev, AD4858_REG_SEAMLESS_HDR, &data);
	if (ret)
		return ret;

	dev->chn_seamless_hdr[chn] = seamless_hdr_status;
	mask = 1 << chn;
	val = no_os_field_prep(mask, seamless_hdr_status);
	data &= ~mask;
	data |= val;

	return ad4858_reg_write(dev, AD4858_REG_SEAMLESS_HDR, data);
}

/**
 * @brief Set channel softspan.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param chn_softspan - Softspan value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_chn_softspan(struct ad4858_dev *dev, uint8_t chn,
			    enum ad4858_chn_softspan chn_softspan)
{
	int ret;
	uint32_t val;

	if (!dev || (chn >= AD4858_NUM_CHANNELS)
	    || (chn_softspan >= AD4858_NUM_OF_SOFTSPAN))
		return -EINVAL;

	val = no_os_field_prep(AD4858_SOFTSPAN_MSK, chn_softspan);
	ret = ad4858_reg_mask(dev, AD4858_REG_CH_SOFTSPAN(chn), AD4858_SOFTSPAN_MSK,
			      chn_softspan);
	if (ret)
		return ret;

	dev->chn_softspan[chn] = chn_softspan;

	return 0;
}

/**
 * @brief Set channel offset.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param offset - Offset value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_chn_offset(struct ad4858_dev *dev, uint8_t chn, uint32_t offset)
{
	int ret;

	if (!dev || (chn >= AD4858_NUM_CHANNELS))
		return -EINVAL;

	/* Offset value is spanned over bits [24:4] */
	ret = ad4858_reg_write(dev, AD4858_REG_CH_OFFSET(chn), (offset << 4));
	if (ret)
		return ret;

	dev->chn_offset[chn] = offset;

	return 0;
}

/**
 * @brief Set channel gain.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param gain - Gain value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_chn_gain(struct ad4858_dev *dev, uint8_t chn, uint16_t gain)
{
	int ret;

	if (!dev || (chn >= AD4858_NUM_CHANNELS))
		return -EINVAL;

	ret = ad4858_reg_write(dev, AD4858_REG_CH_GAIN(chn), gain);
	if (ret)
		return ret;

	dev->chn_gain[chn] = gain;

	return 0;
}

/**
 * @brief Set channel phase.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param phase - Phase value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_chn_phase(struct ad4858_dev *dev, uint8_t chn, uint16_t phase)
{
	int ret;

	if (!dev || (chn >= AD4858_NUM_CHANNELS))
		return -EINVAL;

	ret = ad4858_reg_write(dev, AD4858_REG_CH_PHASE(chn), phase);
	if (ret)
		return ret;

	dev->chn_phase[chn] = phase;

	return 0;
}

/**
 * @brief Set channel overrange (OR) limit.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param or_limit - Overrange limit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_chn_or_limit(struct ad4858_dev *dev, uint8_t chn,
			    uint32_t or_limit)
{
	int ret;

	if (!dev || (chn >= AD4858_NUM_CHANNELS))
		return -EINVAL;

	/* OR limit value is spanned over bits [24:4] */
	ret = ad4858_reg_write(dev, AD4858_REG_CH_OR(chn), (or_limit << 4));
	if (ret)
		return ret;

	dev->chn_or[chn] = or_limit;

	return 0;
}

/**
 * @brief Set channel underrange (UR) limit.
 * @param dev - Pointer to the device structure.
 * @param chn - Input channel.
 * @param ur_limit - Underrange limit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_set_chn_ur_limit(struct ad4858_dev *dev, uint8_t chn,
			    uint32_t ur_limit)
{
	int ret;

	if (!dev || (chn >= AD4858_NUM_CHANNELS))
		return -EINVAL;

	/* UR limit value is spanned over bits [24:4] */
	ret = ad4858_reg_write(dev, AD4858_REG_CH_UR(chn), (ur_limit << 4));
	if (ret)
		return ret;

	dev->chn_ur[chn] = ur_limit;

	return 0;
}

/**
 * @brief Toggle the CNV pin to start a conversion.
 * @param dev - Pointer to the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_convst(struct ad4858_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_cnv, 1);
	if (ret)
		return ret;

	/* Conversion high time (tcnvh) is 40nsec (min) */
	/* nsec delay function not available in the no-os, so
	 * not adding delay to reduce time spent in conversion cycle */

	return no_os_gpio_set_value(dev->gpio_cnv, 0);
}

/**
 * @brief Perform ADC conversion.
 * @param dev - Pointer to the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_perform_conv(struct ad4858_dev *dev)
{
	int ret;
	uint32_t timeout = 10000;
	uint8_t gpio_val;

	ret = ad4858_convst(dev);
	if (ret)
		return ret;

	/* Monitor BUSY GPIO (low state) for conversion end */
	do {
		ret = no_os_gpio_get_value(dev->gpio_busy, &gpio_val);
		if (ret)
			return ret;

		if (gpio_val == NO_OS_GPIO_LOW)
			break;
		timeout--;
	} while (timeout);

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Read ADC conversion data over SPI.
 * @param dev - Pointer to the device structure.
 * @param data - Pointer to adc conversion data structure.
 * @return 0 in case of success, negative error code otherwise.
 * @note As this is a simultaneously sampling ADC, data for all channels
 * is acquired/read in a single SPI read function call.
 */
int ad4858_spi_data_read(struct ad4858_dev *dev, struct ad4858_conv_data *data)
{
	int ret;
	uint16_t nb_bytes;
	uint8_t indx;
	uint8_t chn;
	uint8_t buff[32] = {0};
	// Buffer index offset for each channel while reading 20-bit conversion result
	uint8_t buff_chn_offset[] = { 0, 2, 5, 7, 10, 12, 15, 17 };

	if (!dev || !data)
		return -EINVAL;

	switch (dev->packet_format) {
	case AD4858_PACKET_20_BIT:
		nb_bytes = (20 * AD4858_NUM_CHANNELS) >> 3;
		break;

	case AD4858_PACKET_24_BIT:
		nb_bytes = (24 * AD4858_NUM_CHANNELS) >> 3;
		break;

	case AD4858_PACKET_32_BIT:
		nb_bytes = (32 * AD4858_NUM_CHANNELS) >> 3;
		break;

	default:
		return -EINVAL;
	}

	/* Read SPI data */
	ret = no_os_spi_write_and_read(dev->spi_desc, buff, nb_bytes);
	if (ret)
		return ret;

	switch (dev->packet_format) {
	case AD4858_PACKET_20_BIT:
		/* 20-bit conversion result */
		for (chn = 0; chn < AD4858_NUM_CHANNELS; chn++) {
			indx = buff_chn_offset[chn];
			if (!(chn % 2))
				data->raw[chn] = (((uint32_t)buff[indx] << 16) |
						  ((uint32_t)buff[indx + 1] << 8) |
						  (buff[indx + 2])) >> 4;
			else
				data->raw[chn] = ((uint32_t)(buff[indx] & 0x0f) << 16) |
						 ((uint32_t)buff[indx + 1] << 8) |
						 (buff[indx + 2]);
		}
		break;

	case AD4858_PACKET_24_BIT:
		/* 20-bit conversion result + 1-bit OR/UR + 3-bit channel ID */
		for (chn = 0; chn < AD4858_NUM_CHANNELS; chn++) {
			indx = chn * 3;
			data->raw[chn] = (((uint32_t)buff[indx] << 16) |
					  ((uint32_t)buff[indx + 1] << 8) |
					  (buff[indx + 2])) >> 4;
			data->or_ur_status[chn] = (buff[indx + 2] >> 3) & 0x1;
			data->chn_id[chn] = buff[indx + 2] & 0x7;
		}
		chn = 0;
		break;

	case AD4858_PACKET_32_BIT:
		/* 20-bit conversion result + 1-bit OR/UR + 3-bit channel ID + 4-bit softspan ID + 4 0's */
		for (chn = 0; chn < AD4858_NUM_CHANNELS; chn++) {
			indx = chn * 4;
			data->raw[chn] = (((uint32_t)buff[indx] << 16) |
					  ((uint32_t)buff[indx + 1] << 8) |
					  (buff[indx + 2])) >> 4;
			data->or_ur_status[chn] = (buff[indx + 2] >> 3) & 0x1;
			data->chn_id[chn] = buff[indx + 2] & 0x7;
			data->softspan_id[chn] = (buff[indx + 3] >> 4) & 0xf;
		}
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Read ADC data (for all channels).
 * @param dev - Pointer to the device structure.
 * @param data - Pointer to adc conversion data structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_read_data(struct ad4858_dev *dev, struct ad4858_conv_data *data)
{
	int ret;

	if (!dev || !data)
		return -EINVAL;

	ret = ad4858_perform_conv(dev);
	if (ret)
		return ret;

	return ad4858_spi_data_read(dev, data);
}

/**
 * @brief Perform an AD4858 software reset.
 * @param dev - Pointer to the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_soft_reset(struct ad4858_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Reset device and set SPI mode to 4-wire */
	ret = ad4858_reg_write(dev,
			       AD4858_REG_INTERFACE_CONFIG_A,
			       AD4858_SW_RESET_MSK | AD4858_SDO_ENABLE_MSK);
	if (ret)
		return ret;

	/* Reset delay */
	no_os_mdelay(10);

	return 0;
}

/**
 * @brief Configure an AD4858 dev.
 * @param device - Pointer to the dev structure.
 * @param init_param - Pointer to the initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad4858_config(struct ad4858_dev *dev,
			 struct ad4858_init_param *init_param)
{
	int ret;
	uint8_t chn_cnt;
	uint32_t temp;

	if (!dev || !init_param)
		return -EINVAL;

	ret = ad4858_enable_osr(dev, init_param->osr_enable);
	if (ret)
		return ret;

	ret = ad4858_set_osr_ratio(dev, init_param->osr_ratio);
	if (ret)
		return ret;

	ret = ad4858_set_packet_format(dev, init_param->packet_format);
	if (ret)
		return ret;

	ret = ad4858_enable_test_pattern(dev, init_param->test_pattern);
	if (ret)
		return ret;

	for (chn_cnt = 0; chn_cnt < AD4858_NUM_CHANNELS; chn_cnt++) {
		if (init_param->use_default_chn_configs)
			temp = AD4858_DEF_CHN_SOFTSPAN;
		else
			temp = init_param->chn_softspan[chn_cnt];

		ret = ad4858_set_chn_softspan(dev, chn_cnt, temp);
		if (ret)
			return ret;

		if (init_param->use_default_chn_configs)
			temp = AD4858_DEF_CHN_OFFSET;
		else
			temp = init_param->chn_offset[chn_cnt];

		ret = ad4858_set_chn_offset(dev, chn_cnt, temp);
		if (ret)
			return ret;

		if (init_param->use_default_chn_configs)
			temp = AD4858_DEF_CHN_GAIN;
		else
			temp = init_param->chn_gain[chn_cnt];

		ret = ad4858_set_chn_gain(dev, chn_cnt, temp);
		if (ret)
			return ret;

		if (init_param->use_default_chn_configs)
			temp = AD4858_DEF_CHN_PHASE;
		else
			temp = init_param->chn_phase[chn_cnt];

		ret = ad4858_set_chn_phase(dev, chn_cnt, temp);
		if (ret)
			return ret;

		if (init_param->use_default_chn_configs)
			temp = AD4858_DEF_CHN_OR;
		else
			temp = init_param->chn_or[chn_cnt];

		ret = ad4858_set_chn_or_limit(dev, chn_cnt, temp);
		if (ret)
			return ret;

		if (init_param->use_default_chn_configs)
			temp = AD4858_DEF_CHN_UR;
		else
			temp = init_param->chn_ur[chn_cnt];

		ret = ad4858_set_chn_ur_limit(dev, chn_cnt, temp);
		if (ret)
			return ret;

		if (init_param->use_default_chn_configs)
			temp = AD4858_SLEEP_DISABLE;
		else
			temp = init_param->chn_sleep_value[chn_cnt];

		ret = ad4858_enable_ch_sleep(dev, chn_cnt, temp);
		if (ret)
			return ret;

		if (init_param->use_default_chn_configs)
			temp = AD4858_SEAMLESS_HDR_ENABLE;
		else
			temp = init_param->chn_seamless_hdr[chn_cnt];

		ret = ad4858_enable_ch_seamless_hdr(dev, chn_cnt, temp);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Initialize an AD4858 GPIOs.
 * @param dev - Pointer to the device structure.
 * @param init_param - Pointer to the initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad4858_gpio_config(struct ad4858_dev *dev,
			      struct ad4858_init_param *init_param)
{
	int ret;

	if (!dev || !init_param)
		return -EINVAL;

	/* PD GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_pd, init_param->gpio_pd);
	if (ret)
		return ret;

	if (dev->gpio_pd) {
		/* Set PD low for the normal operation of device */
		ret = no_os_gpio_direction_output(dev->gpio_pd, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	/* CMOS_LVDS GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_cmos_lvds, init_param->gpio_cmos_lvds);
	if (ret)
		return ret;

	if (dev->gpio_cmos_lvds) {
		/* Set low for the CMOS data interface */
		ret = no_os_gpio_direction_output(dev->gpio_cmos_lvds, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	/* CNV GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_cnv, init_param->gpio_cnv);
	if (ret)
		return ret;

	if (dev->gpio_cnv) {
		ret = no_os_gpio_direction_output(dev->gpio_cnv, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	/* BUSY GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_busy, init_param->gpio_busy);
	if (ret)
		return ret;

	if (dev->gpio_busy) {
		return no_os_gpio_direction_input(dev->gpio_busy);
	}

	return 0;
}

/**
 * @brief Remove an AD4858 GPIOs (free memory allocated by ad4858_gpio_config function).
 * @param dev - Pointer to the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad4858_gpio_remove(struct ad4858_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_remove(dev->gpio_pd);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_cmos_lvds);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_cnv);
	if (ret)
		return ret;

	return no_os_gpio_remove(dev->gpio_busy);
}

/**
 * @brief Initialize an AD4858 device structure.
 * @param device - Pointer to the device structure (memory is allocated within this function).
 * @param init_param - Pointer to the initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_init(struct ad4858_dev **device,
		struct ad4858_init_param *init_param)
{
	struct ad4858_dev *dev;
	int32_t ret;
	uint32_t product_id_l = 0;
	uint32_t product_id_h = 0;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct ad4858_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->big_endian = no_os_is_big_endian();

	/* Configure GPIOs */
	ret = ad4858_gpio_config(dev, init_param);
	if (ret)
		goto error_dev;

	/* Init SPI digital interface */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_gpio;

	/* Perform software Reset */
	ret = ad4858_soft_reset(dev);
	if (ret)
		goto error_spi;

	/* Enable 4-wire SPI config interface mode */
	ret = ad4858_set_config_interface_mode(dev);
	if (ret)
		goto error_spi;

	/* Enable address ascension.
	 * Note: By default ascension is disabled. If enabled, address is incremented
	 * by 1 for each data byte in streaming mode.
	 * */
	if (init_param->addr_ascension_enable) {
		ret = ad4858_reg_mask(dev, AD4858_REG_INTERFACE_CONFIG_A,
				      AD4858_ADDR_ASCENSION_MSK,
				      AD4858_ADDR_ASCENSION_MSK);
		if (ret)
			goto error_spi;

		dev->addr_ascension_enable = init_param->addr_ascension_enable;
	}

	/* Validate product IDs */
	ret = ad4858_reg_read(dev, AD4858_REG_PRODUCT_ID_L, &product_id_l);
	if (ret)
		goto error_spi;

	ret = ad4858_reg_read(dev, AD4858_REG_PRODUCT_ID_H, &product_id_h);
	if (ret)
		goto error_spi;

	if ((product_id_l != AD4858_PRODUCT_ID_L)
	    || (product_id_h != AD4858_PRODUCT_ID_H)) {
		pr_err("Product ID mismatch \r\n");
		return -EFAULT;
	}

	/* Configure rest of the device parameters */
	ret = ad4858_config(dev, init_param);
	if (ret)
		goto error_spi;

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_gpio:
	ad4858_gpio_remove(dev);
error_dev:
	free(dev);

	return ret;
}

/**
 * @brief Remove an AD4858 device (free memory allocated by ad4858_init function).
 * @param dev - Pointer to the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4858_remove(struct ad4858_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4858_gpio_remove(dev);
	if (ret)
		return ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	free(dev);

	return 0;
}

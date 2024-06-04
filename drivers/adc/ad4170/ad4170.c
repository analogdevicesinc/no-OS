/***************************************************************************//**
 *   @file   ad4170.c
 *   @brief  Source file for the ad4170 driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
 *   @author Janani Sunil (janani.sunil@analog.com)
 *   @author Mahesh Phalke (mahesh.phalke@analog.com)
********************************************************************************
 * Copyright 2020, 2023(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_crc8.h"
#include "ad4170.h"

NO_OS_DECLARE_CRC8_TABLE(ad4170_crc8);

struct ad4170_config ad4170_config_reset = {
	.pin_muxing = {
		.sync_ctrl = AD4170_SYNC_STANDARD,
	},
	.channel_en = 0x1,
	.map = {
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1},
		{.ainm = AD4170_AIN1}
	},
	.setups = {
		{ /* setup 0 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
		{ /* setup 1 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
		{ /* setup 2 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
		{ /* setup 3 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
		{ /* setup 4 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
		{ /* setup 5 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
		{ /* setup 6 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
		{ /* setup 7 */
			.afe = {
				.bipolar = true,
				.ref_select = AD4170_REFIN_REFOUT,
			},
			.filter_fs = 0x4,
			.gain = 0x555555,
		},
	},
	.ref_control = {
		.ref_en = true,
	},
	.fir_control = {
		.fir_length = 1,
	}
};

static inline bool _is_addr_valid(struct ad4170_dev *dev, uint32_t reg_addr)
{
	uint8_t reg_size = AD4170_TRANSF_LEN(reg_addr);
	uint32_t addr = AD4170_ADDR(reg_addr);

	/* max readable address in short instruction mode */
	if (dev->spi_settings.short_instruction && addr > 0x7F)
		return false;

	/* max valid address */
#ifndef AD4170_TEST_AREA
	if (addr > 0x195)
		return false;
#endif

	if (reg_size > 4 || reg_size == 0)
		return false;

	return true;
}

static inline bool _is_big_endian()
{
	uint16_t a = 0x0100;
	return (bool) *(uint8_t *) &a;
}

/* Swap bytes in a buffer with a given step
 * Swap with step of 2:
 * 	AA BB CC DD EE FF 00 11 becomes
 *	BB AA DD CC FF EE 11 00
 * Swap with step of 3:
 * 	AA BB CC DD EE FF 00 11 22 becomes
 * 	CC BB AA FF EE DD 22 11 00
 * etc.
 */
static void _memswap64(void * buf, uint32_t bytes, size_t step)
{
	uint8_t * p = buf;
	uint32_t i, j;
	uint8_t temp[8];
	if (step < 2 || step > 8 || bytes < step || bytes % step != 0)
		return;
	for(i = 0; i < bytes; i += step) {
		memcpy(temp, p, step);
		for(j = step; j > 0; j--) {
			*p++ = temp[j-1];
		}
	}
}

static inline void _spi_resync(struct ad4170_dev *dev)
{
	uint8_t flush_buf[8];
	memset(flush_buf, 0xff, sizeof(flush_buf));
	no_os_spi_write_and_read(dev->spi_desc, flush_buf, sizeof(flush_buf));
}

static int32_t _verify_sync_pattern(struct ad4170_dev *dev, uint8_t *buf)
{
	uint16_t sync_pattern;

	if (!dev->spi_settings.sync_loss_detect ||
	    dev->config.adc_ctrl.cont_read == AD4170_CONT_TRANSMIT_ON)
		return 0;

	sync_pattern = (uint16_t)(buf[0] & 0x7F) << 8;
	if (dev->spi_settings.short_instruction)
		sync_pattern |= (uint8_t)AD4170_SPI_SYNC_PATTERN;
	else
		sync_pattern |= buf[1];

	if (sync_pattern != AD4170_SPI_SYNC_PATTERN) {
		_spi_resync(dev);
		return -ECOMM;
	}

	return 0;
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_spi_reg_read(struct ad4170_dev *dev,
			uint32_t reg_addr,
			uint32_t *reg_data)
{
	uint8_t buf[7];
	uint8_t icrc = 0;
	uint8_t ocrc = 0;
	uint16_t sz;
	uint16_t i = 0;
	uint16_t di;
	uint8_t reg_size = AD4170_TRANSF_LEN(reg_addr);
	uint32_t addr = AD4170_ADDR(reg_addr) + reg_size - 1;
	int ret;

	if (!dev || !reg_data || !_is_addr_valid(dev, reg_addr))
		return -EINVAL;

	if (dev->config.adc_ctrl.cont_read)
		return -EACCES;

	memset(buf, 0, sizeof(buf));
	if (dev->spi_settings.short_instruction) {
		buf[i++] = (uint8_t)(AD4170_REG_READ_6(addr));
	} else {
		buf[i++] = (uint8_t)(AD4170_REG_READ_14(addr) >> 8);
		buf[i++] = (uint8_t)(addr);
	}

	di = i; // save the data index

	if (dev->spi_settings.crc_enabled) {
		ocrc = no_os_crc8(ad4170_crc8, buf, i,
				  AD4170_CRC8_INITIAL_VALUE); // compute instruction crc
		icrc = ocrc;
	}

	i += reg_size; // padding bytes on MOSI (zeroes, due to memset above)
	sz = i;

	if (dev->spi_settings.crc_enabled) {
		ocrc = no_os_crc8(ad4170_crc8, &buf[di], reg_size, ocrc);
		buf[i++] = ocrc;
		sz = i;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sz);
	if (ret)
		return ret;

	ret = _verify_sync_pattern(dev, buf);
	if (ret)
		return ret;

	if (dev->spi_settings.crc_enabled) {
		icrc = no_os_crc8(ad4170_crc8, &buf[di], reg_size, icrc);
		if (icrc != buf[sz-1])
			return -EBADMSG;
	}

	*reg_data = 0;
	if (dev->big_endian)
		memcpy((uint8_t *)(reg_data + 1) - reg_size, &buf[di], reg_size);
	else {
		memcpy(reg_data, &buf[di], reg_size);
		_memswap64(reg_data, reg_size, reg_size);
	}

	return 0;
}

/**
 * @brief Write device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_spi_reg_write(struct ad4170_dev *dev,
			 uint32_t reg_addr,
			 uint32_t reg_data)
{
	uint8_t buf[7];
	uint8_t ocrc = 0;
	uint16_t sz;
	uint16_t i = 0;
	uint8_t reg_size = AD4170_TRANSF_LEN(reg_addr);
	uint32_t addr = AD4170_ADDR(reg_addr) + reg_size - 1;
	int ret;
	uint32_t data = reg_data;

	if (!dev || !_is_addr_valid(dev, reg_addr))
		return -EINVAL;

	// allow only soft reset while in continuous read mode
	if ((dev->config.adc_ctrl.cont_read == AD4170_CONT_READ_ON) &&
	    (reg_addr != AD4170_REG_INTERFACE_CONFIG_A))
		return -EACCES;

	// allow only soft reset or continuous mode change while in continuous transmit mode
	if ((dev->config.adc_ctrl.cont_read == AD4170_CONT_TRANSMIT_ON) &&
	    !((reg_addr == AD4170_REG_INTERFACE_CONFIG_A)
	      || (reg_addr == AD4170_REG_ADC_CTRL)))
		return -EACCES;

	if (dev->spi_settings.short_instruction) {
		buf[i++] = (uint8_t)(AD4170_REG_WRITE_6(addr));
	} else {
		buf[i++] = (uint8_t)(AD4170_REG_WRITE_14(addr) >> 8);
		buf[i++] = (uint8_t)(addr);
	}

	if (dev->big_endian)
		memcpy(&buf[i], (uint8_t *)(&data + 1) - reg_size, reg_size);
	else {
		_memswap64(&data, reg_size, reg_size);
		memcpy(&buf[i], &data, reg_size);
	}
	i += reg_size;

	sz = i;

	if (dev->spi_settings.crc_enabled) {
		ocrc = no_os_crc8(ad4170_crc8, buf, i, AD4170_CRC8_INITIAL_VALUE);
		buf[i++] = ocrc;
		sz = i;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sz);
	if (ret)
		return ret;

	ret = _verify_sync_pattern(dev, buf);
	if (ret)
		return ret;

	if (dev->spi_settings.crc_enabled) {
		if (ocrc != buf[sz - 1])
			return -EBADMSG;
	}

	return 0;
}

/**
 * @brief SPI write device register using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_spi_reg_write_mask(struct ad4170_dev *dev,
			      uint32_t reg_addr,
			      uint8_t mask,
			      uint32_t reg_data)
{
	uint32_t reg;
	int ret;

	ret = ad4170_spi_reg_read(dev, reg_addr, &reg);
	if (ret)
		return ret;

	reg &= ~mask;
	reg |= reg_data;

	return ad4170_spi_reg_write(dev, reg_addr, reg);
}

/**
 * @brief Reset the SPI interface by sending reset sequence to device
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_reset_spi_interface(struct ad4170_dev *dev)
{
	int ret;

	/* Data sequence to reset AD4170 SPI interface (71*1 + 1*0) * 3
	 * Note: CS must be low for entire sequence. */
	uint8_t buf[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
			 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
			 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe
			};

	if (!dev)
		return -EINVAL;

	return no_os_spi_write_and_read(dev->spi_desc, buf, sizeof(buf));
}

/**
 * @brief Get data from register Data_16b.
 * @param dev - The device structure.
 * @param data - Output pointer where to store the data value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_data16(struct ad4170_dev *dev, uint16_t *data)
{
	uint32_t reg;
	int ret;

	if (!data)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_DATA_16b, &reg);
	if (ret)
		return ret;

	*data = (uint16_t)reg;

	return 0;
}

/**
 * @brief Get data and status from register Data_16b_Status.
 * @param dev - The device structure.
 * @param data - Output pointer where to store the data value.
 * @param status - Output pointer where to store the status value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_data16s(struct ad4170_dev *dev, uint16_t *data,
		       uint8_t *status)
{
	uint32_t reg;
	int ret;

	if (!data || !status)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_DATA_16b_STATUS, &reg);
	if (ret)
		return ret;

	*status = (uint8_t)reg;
	*data = (uint16_t)(reg >> 8);

	return 0;
}

/**
 * @brief Get data from register Data_24b.
 * @param dev - The device structure.
 * @param data - Output pointer where to store the data value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_data24(struct ad4170_dev *dev, uint32_t *data)
{
	uint32_t reg;
	int ret;

	if (!data)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_DATA_24b, &reg);
	if (ret)
		return ret;

	*data = reg & 0xFFFFFF;

	return 0;
}

/**
 * @brief Get data and status from register Data_24b_Status.
 * @param dev - The device structure.
 * @param data - Output pointer where to store the data value.
 * @param status - Output pointer where to store the status value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_data24s(struct ad4170_dev *dev, uint32_t *data,
		       uint8_t *status)
{
	uint32_t reg;
	int ret;

	if (!data || !status)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_DATA_24b_STATUS, &reg);
	if (ret)
		return ret;

	*status = (uint8_t)reg;
	*data = reg >> 8;

	return 0;
}

/**
 * @brief Get data from register Data_32b.
 * @param dev - The device structure.
 * @param data - Output pointer where to store the data value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_data32(struct ad4170_dev *dev, uint32_t *data)
{
	uint32_t reg;
	int ret;

	if (!data)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_DATA_32b, &reg);
	if (ret)
		return ret;

	*data = reg;

	return 0;
}

/**
 * @brief Get data from register Data_Per_Channel[n].
 *
 * This function assumes Per-Channel ADC Data is enabled, otherwise the
 * output data may contain zeroes.
 *
 * @param dev - The device structure.
 * @param ch - The channel number (0 to 15)
 * @param data - Output pointer where to store the data value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_ch_data(struct ad4170_dev *dev, uint8_t ch, uint32_t *data)
{
	uint32_t reg;
	int ret;

	if (ch >= AD4170_NUM_CHANNELS || !data)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_DATA_PER_CHANNEL(ch), &reg);
	if (ret)
		return ret;

	*data = reg;

	return 0;
}

/**
 * @brief Get status from register Data_Status.
 * @param dev - The device structure.
 * @param status - Output pointer where to store the status value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_status(struct ad4170_dev *dev, uint16_t *status)
{
	int ret;
	uint32_t reg;

	if (!dev || !status)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_DATA_STATUS, &reg);
	if (ret)
		return ret;

	*status = (uint16_t)reg;

	return 0;
}

static int32_t _ad4170_read(struct ad4170_dev *dev, uint32_t *pbuf,
			    uint16_t nb_samples,
			    uint32_t reg)
{
	int ret;
	uint16_t ch, repeat;
	uint32_t timeout;
	uint8_t rdyb;
	uint32_t *p = pbuf;

	if (!dev || !pbuf)
		return -EINVAL;

	if (!nb_samples)
		return 0;

	// this function relies on dig_aux1 pin being configured as RDY signal
	if (!dev->gpio_dig_aux1
	    || (dev->config.pin_muxing.dig_aux1_ctrl != AD4170_DIG_AUX1_RDY))
		return -ENOTSUP;

	while(nb_samples) {
		for (ch = 0; ch < AD4170_NUM_CHANNELS; ch++) {
			repeat = dev->config.setup[ch].repeat_n;
			/* if only one channel is enabled, the repeat setting is ignored */
			if (no_os_hweight8(dev->config.channel_en) == 1)
				repeat = 0;

			do {
				if(!(dev->config.channel_en & NO_OS_BIT(ch)))
					continue;

				rdyb = NO_OS_GPIO_HIGH;
				timeout = dev->rdy_conv_timeout;
				while (rdyb == NO_OS_GPIO_HIGH && timeout--) {
					no_os_gpio_get_value(dev->gpio_dig_aux1, &rdyb);
				};
				if (!timeout)
					return -ETIMEDOUT;

				ret = ad4170_spi_reg_read(dev, reg, p);
				if (ret)
					return ret;

				timeout = dev->rdy_conv_timeout;
				while (rdyb == NO_OS_GPIO_LOW && timeout--) {
					no_os_gpio_get_value(dev->gpio_dig_aux1, &rdyb);
				};
				if (!timeout)
					return -ETIMEDOUT;

				p++;
				nb_samples--;
			} while(repeat-- && nb_samples);

			if (!nb_samples)
				break;
		}
	}

	return 0;
}

/**
 * @brief Read multiple samples using direct register access from Data_16b
 * @param dev - The device structure.
 * @param pbuf - Output pointer where to store the data values.
 * @param nb_samples - The number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_read16(struct ad4170_dev *dev, uint32_t *pbuf,
		  uint16_t nb_samples)
{
	return _ad4170_read(dev, pbuf, nb_samples, AD4170_REG_DATA_16b);
}

/**
 * @brief Read multiple samples using direct register access from Data_16b_Status
 * @param dev - The device structure.
 * @param pbuf - Output pointer where to store the data values.
 * @param nb_samples - The number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_read16s(struct ad4170_dev *dev, uint32_t *pbuf,
		   uint16_t nb_samples)
{
	return _ad4170_read(dev, pbuf, nb_samples, AD4170_REG_DATA_16b_STATUS);
}

/**
 * @brief Read multiple samples using direct register access from Data_24
 * @param dev - The device structure.
 * @param pbuf - Output pointer where to store the data values.
 * @param nb_samples - The number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_read24(struct ad4170_dev *dev, uint32_t *pbuf,
		  uint16_t nb_samples)
{
	return _ad4170_read(dev, pbuf, nb_samples, AD4170_REG_DATA_24b);
}

/**
 * @brief Read multiple samples using direct register access from Data_24_Status
 * @param dev - The device structure.
 * @param pbuf - Output pointer where to store the data values.
 * @param nb_samples - The number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_read24s(struct ad4170_dev *dev, uint32_t *pbuf,
		   uint16_t nb_samples)
{
	return _ad4170_read(dev, pbuf, nb_samples, AD4170_REG_DATA_24b_STATUS);
}

/**
 * @brief Read multiple samples using direct register access from Data_32b
 * @param dev - The device structure.
 * @param pbuf - Output pointer where to store the data values.
 * @param nb_samples - The number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_read32(struct ad4170_dev *dev, uint32_t *pbuf,
		  uint16_t nb_samples)
{
	return _ad4170_read(dev, pbuf, nb_samples, AD4170_REG_DATA_32b);
}

static int32_t _scratchpad_test(struct ad4170_dev *dev, uint8_t pattern)
{
	int ret;
	uint32_t reg;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_SCRATCH_PAD, pattern);
	if (ret)
		return ret;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_SCRATCH_PAD, &reg);
	if (ret)
		return ret;
	if (reg != pattern)
		ret = -EFAULT;

	return 0;
}

static int32_t _ad4170_continuous_read_one(struct ad4170_dev *dev,
		uint32_t *data_out, uint8_t *status_out, bool exit)
{
	int ret;
	uint8_t rdyb, icrc;
	uint32_t timeout;
	uint8_t buf[5];
	uint16_t sz = 3;
	bool status_en = dev->config.adc_ctrl.cont_read_status_en;
	sz += (uint16_t)status_en;
	sz += (uint16_t)dev->spi_settings.crc_enabled;

	if (!status_out && status_en)
		return -EINVAL;

	// this function relies on dig_aux1 pin being configured as RDY signal
	if (!dev->gpio_dig_aux1
	    || (dev->config.pin_muxing.dig_aux1_ctrl != AD4170_DIG_AUX1_RDY))
		return -ENOTSUP;

	memset(buf, 0, sz);
	if (exit)
		buf[0] = 0xa5;

	rdyb = NO_OS_GPIO_HIGH;
	timeout = dev->rdy_conv_timeout;
	while (rdyb == NO_OS_GPIO_HIGH && timeout--) {
		no_os_gpio_get_value(dev->gpio_dig_aux1, &rdyb);
	};
	if (!timeout)
		return -ETIMEDOUT;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sz);
	if (ret)
		return ret;

	*data_out = (buf[0] << 16) | (buf[1] << 8) | buf[2];
	if (status_en)
		*status_out = buf[3];

	if (dev->spi_settings.crc_enabled && !exit) {
		icrc = no_os_crc8(ad4170_crc8, buf, sz-1, AD4170_CRC8_INITIAL_VALUE);
		if (icrc != buf[sz - 1])
			return -EBADMSG;
	}

	timeout = dev->rdy_conv_timeout;
	while (rdyb == NO_OS_GPIO_LOW && timeout--) {
		no_os_gpio_get_value(dev->gpio_dig_aux1, &rdyb);
	};
	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Read multiple samples in continuous read mode
 * @param dev - The device structure.
 * @param data_out - Output pointer where to store the data values.
 * @param status_out - Output pointer where to store the status values.
 * @param nb_samples - The number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_continuous_read(struct ad4170_dev *dev, uint32_t *data_out,
			   uint8_t *status_out, uint16_t nb_samples)
{
	int ret;
	uint16_t ch, repeat, i = 0;

	if (!data_out || !status_out)
		return -EINVAL;

	if (!nb_samples)
		return 0;

	while(nb_samples) {
		for (ch = 0; ch < AD4170_NUM_CHANNELS; ch++) {
			repeat = dev->config.setup[ch].repeat_n;
			// if only one channel is enabled, the repeat setting is ignored
			if (no_os_hweight8(dev->config.channel_en) == 1)
				repeat = 0;

			do {
				if(!(dev->config.channel_en & NO_OS_BIT(ch)))
					continue;

				ret = _ad4170_continuous_read_one(dev, &data_out[i], &status_out[i], false);
				if (ret)
					return ret;

				i++;
				nb_samples--;
			} while(repeat-- && nb_samples);

			if (!nb_samples)
				break;
		}
	}

	return 0;
}

static int32_t _ad4170_digif_init(struct ad4170_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_get_optional(&dev->gpio_dig_aux1, dev->gpio_dig_aux1_init);
	if (ret)
		return ret;

	if (dev->gpio_dig_aux1) {
		// regardless of pin_muxing settings, dig_aux1 is always an input for the host
		ret = no_os_gpio_direction_input(dev->gpio_dig_aux1);
		if (ret)
			return ret;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_dig_aux2, dev->gpio_dig_aux2_init);
	if (ret)
		return ret;

	if (dev->gpio_dig_aux2) {
		if (dev->dig_aux2_output) {
			// when dig_aux2 is used as LDAC or SYNC
			ret = no_os_gpio_direction_output(dev->gpio_dig_aux2, NO_OS_GPIO_HIGH);
			if (ret)
				return ret;
		} else {
			// when dig_aux2 is used as MODOUT or disabled
			ret = no_os_gpio_direction_input(dev->gpio_dig_aux2);
			if (ret)
				return ret;
		}
	}

	ret = no_os_gpio_get_optional(&dev->gpio_sync_inb, dev->gpio_sync_inb_init);
	if (ret)
		return ret;

	if (dev->gpio_sync_inb) {
		ret = no_os_gpio_direction_output(dev->gpio_sync_inb, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	ret = no_os_spi_init(&dev->spi_desc, &dev->spi_init);
	if (ret)
		return ret;

	dev->digif = true;

	return ret;
}

static void _ad4170_digif_remove(struct ad4170_dev *dev)
{
	if (!dev)
		return;
	no_os_spi_remove(dev->spi_desc);
	no_os_gpio_remove(dev->gpio_dig_aux1);
	no_os_gpio_remove(dev->gpio_dig_aux2);
	no_os_gpio_remove(dev->gpio_sync_inb);
	dev->digif = false;
}

/**
 * @brief Exit continuous read mode
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_continuous_read_exit(struct ad4170_dev *dev)
{
	int ret;
	uint32_t data;
	uint8_t status;

	if (!dev)
		return -EINVAL;

	if (dev->config.adc_ctrl.cont_read != AD4170_CONT_READ_ON)
		return -EACCES;

	ret = _ad4170_continuous_read_one(dev, &data, &status, true);

	if (!ret)
		dev->config.adc_ctrl.cont_read = AD4170_CONT_READ_OFF;

	return ret;
}

/**
 * @brief Exit continuous transmit mode
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_continuous_transmit_exit(struct ad4170_dev *dev)
{
	int ret;
	struct ad4170_adc_ctrl adc_ctrl = dev->config.adc_ctrl;

	if (!dev)
		return -EINVAL;

	if (dev->config.adc_ctrl.cont_read != AD4170_CONT_TRANSMIT_ON)
		return -EACCES;

	if (!dev->digif) {
		ret = _ad4170_digif_init(dev);
		if (ret)
			return 0;
	}

	adc_ctrl.cont_read = AD4170_CONT_READ_OFF;
	ret = ad4170_set_adc_ctrl(dev, adc_ctrl);
	// Being in continuous transmit (TDM), the slave may put TDM data
	// on SPI MISO which will make the CRC check fail, if enabled.
	if (ret && ret != -EBADMSG)
		return ret;

	dev->config.adc_ctrl.cont_read = AD4170_CONT_READ_OFF;

	return 0;
}

/**
 * @brief Perform a software reset
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_reset(struct ad4170_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write_mask(dev,
					AD4170_REG_INTERFACE_CONFIG_A,
					AD4170_SW_RESET_MSK | AD4170_SW_RESETX_MSK,
					AD4170_SW_RESET_MSK | AD4170_SW_RESETX_MSK);
	if (ret)
		return ret;

	/* Turn-on settling time */
	no_os_udelay(200);

	// Reset the driver representation of the chip to a default state
	// that matches the chip configuration right after reset.
	dev->config = ad4170_config_reset;

	return 0;
}

/**
 * @brief Set the AD4170 SPI settings.
 * @param dev - The device structure.
 * @param spi_settings - AD4170 SPI configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_spi_settings(struct ad4170_dev *dev,
			    const struct ad4170_spi_settings *spi_settings)
{
	int ret;

	if (!dev || !spi_settings)
		return -EINVAL;

	dev->spi_settings.sync_loss_detect =
		spi_settings->sync_loss_detect;

	// enable crc already to be sure next config instructions are correct.
	ret = ad4170_spi_reg_write_mask(dev,
					AD4170_REG_INTERFACE_CONFIG_C,
					AD4170_INTERFACE_CONFIG_C_CRC_MSK,
					AD4170_INTERFACE_CONFIG_C_CRC((uint8_t)spi_settings->crc_enabled));
	if (ret)
		return ret;
	else
		dev->spi_settings.crc_enabled =
			spi_settings->crc_enabled;

	ret = ad4170_spi_reg_write_mask(dev,
					AD4170_REG_INTERFACE_CONFIG_B,
					AD4170_INTERFACE_CONFIG_B_SHORT_INSTRUCTION_MSK,
					AD4170_INTERFACE_CONFIG_B_SHORT_INSTRUCTION(spi_settings->short_instruction));
	if (ret)
		return ret;

	dev->spi_settings.short_instruction =
		spi_settings->short_instruction;

	return 0;
}

/**
 * @brief Set the AD4170 Pin Muxing settings.
 * @param dev - The device structure.
 * @param pin_muxing - AD4170 Pin Muxing configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_pin_muxing(struct ad4170_dev *dev,
			  struct ad4170_pin_muxing pin_muxing)
{
	uint32_t reg;
	int ret;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_PIN_MUXING_CHAN_TO_GPIO_MSK,
			       pin_muxing.chan_to_gpio);
	reg |= no_os_field_prep(AD4170_PIN_MUXING_DIG_AUX2_CTRL_MSK,
				pin_muxing.dig_aux2_ctrl);
	reg |= no_os_field_prep(AD4170_PIN_MUXING_DIG_AUX1_CTRL_MSK,
				pin_muxing.dig_aux1_ctrl);
	reg |= no_os_field_prep(AD4170_PIN_MUXING_SYNC_CTRL_MSK, pin_muxing.sync_ctrl);
	reg |= no_os_field_prep(AD4170_PIN_MUXING_DIG_OUT_STR_MSK,
				pin_muxing.dig_out_str);
	reg |= no_os_field_prep(AD4170_PIN_MUXING_SDO_RDBY_DLY_MSK,
				pin_muxing.sdo_rdby_dly);

	ret = ad4170_spi_reg_write(dev, AD4170_REG_PIN_MUXING, reg);
	if (ret)
		return ret;

	dev->config.pin_muxing = pin_muxing;

	return 0;
}

/**
 * @brief Set the AD4170 DCLK configuration.
 * @param dev - The device structure.
 * @param div - AD4170 DCLK configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_dclk_div(struct ad4170_dev *dev,
			enum ad4170_dclk_div div)
{
	int ret;
	uint16_t reg;

	if (!dev)
		return -EINVAL;
	if (dev->config.clock_ctrl.dclk_divide == div)
		return 0;

	reg = no_os_field_prep(AD4170_CLOCK_CTRL_DCLK_DIVIDE_MSK, div);
	ret = ad4170_spi_reg_write_mask(dev,
					AD4170_REG_CLOCK_CTRL,
					AD4170_CLOCK_CTRL_DCLK_DIVIDE_MSK,
					reg);
	if (ret)
		return ret;

	dev->config.clock_ctrl.dclk_divide = div;

	return 0;
}

/**
 * @brief Set the AD4170 MCLK configuration.
 * @param dev - The device structure.
 * @param div - AD4170 MCLK configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_mclk_div(struct ad4170_dev *dev,
			enum ad4170_mclk_div div)
{
	int ret;
	uint16_t reg;

	if (!dev)
		return -EINVAL;
	if (dev->config.clock_ctrl.clockdiv == div)
		return 0;

	reg = no_os_field_prep(AD4170_CLOCK_CTRL_CLOCKDIV_MSK, div);
	ret = ad4170_spi_reg_write_mask(dev,
					AD4170_REG_CLOCK_CTRL,
					AD4170_CLOCK_CTRL_CLOCKDIV_MSK,
					reg);
	if (ret)
		return ret;

	dev->config.clock_ctrl.clockdiv = div;

	return 0;
}

/**
 * @brief Set the AD4170 clock selection configuration.
 * @param dev - The device structure.
 * @param sel - AD4170 clock selection configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_clocksel(struct ad4170_dev *dev,
			enum ad4170_clocksel sel)
{
	int ret;
	uint16_t reg;

	if (!dev)
		return -EINVAL;
	if (dev->config.clock_ctrl.clocksel == sel)
		return 0;

	reg = no_os_field_prep(AD4170_CLOCK_CTRL_CLOCKSEL_MSK, sel);
	ret = ad4170_spi_reg_write_mask(dev,
					AD4170_REG_CLOCK_CTRL,
					AD4170_CLOCK_CTRL_CLOCKSEL_MSK,
					reg);
	if (ret)
		return ret;

	dev->config.clock_ctrl.clocksel = sel;

	return 0;
}

/**
 * @brief Set the AD4170 Standby control settings.
 * @param dev - The device structure.
 * @param standby_ctrl - AD4170 Standby control configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_standby_ctrl(struct ad4170_dev *dev, uint16_t standby_ctrl)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_STANDBY_CTRL, standby_ctrl);
	if (ret)
		return ret;

	dev->config.standby_ctrl = standby_ctrl;

	return 0;
}

/**
 * @brief Set the AD4170 Powerdown switches settings.
 * @param dev - The device structure.
 * @param powerdown_sw - AD4170 Powerdown switches configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_powerdown_sw(struct ad4170_dev *dev, uint16_t powerdown_sw)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_POWER_DOWN_SW, powerdown_sw);
	if (ret)
		return ret;

	dev->config.powerdown_sw = powerdown_sw;

	return 0;
}

/**
 * @brief Set the AD4170 Error enable settings.
 * @param dev - The device structure.
 * @param error_en - AD4170 Error enable configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_error_en(struct ad4170_dev *dev, uint16_t error_en)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_ERROR_EN, error_en);
	if (ret)
		return ret;

	dev->config.error_en = error_en;

	return 0;
}

/**
 * @brief Set the Error register with a value to clear specific errors
 * @param dev - The device structure.
 * @param error - Error mask (set bits will be cleared from the Error register).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_error(struct ad4170_dev *dev, uint16_t error)
{
	return ad4170_spi_reg_write(dev, AD4170_REG_ERROR, error);
}

/**
 * @brief Get the Error register
 * @param dev - The device structure.
 * @param error - Pointer to where Error register content should be stored.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_get_error(struct ad4170_dev *dev, uint16_t *error)
{
	int ret;
	uint32_t reg;

	if (!dev || !error)
		return -EINVAL;

	ret = ad4170_spi_reg_read(dev, AD4170_REG_ERROR, &reg);
	if (ret)
		return ret;

	*error = (uint16_t)reg;

	return 0;
}

/**
 * @brief Set the AD4170 ADC Control
 * @param dev - The device structure.
 * @param adc_ctrl - AD4170 ADC Control configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_adc_ctrl(struct ad4170_dev *dev,
			struct ad4170_adc_ctrl adc_ctrl)
{
	uint32_t reg;
	int ret;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_ADC_CTRL_PARALLEL_FILT_EN_MSK,
			       adc_ctrl.parallel_filt_en);
	reg |= no_os_field_prep(AD4170_ADC_CTRL_MULTI_DATA_REG_SEL_MSK,
				adc_ctrl.multi_data_reg_sel);
	reg |= no_os_field_prep(AD4170_ADC_CTRL_CONT_READ_STATUS_EN_MSK,
				adc_ctrl.cont_read_status_en);
	reg |= no_os_field_prep(AD4170_REG_CTRL_CONT_READ_MSK, adc_ctrl.cont_read);
	reg |= no_os_field_prep(AD4170_REG_CTRL_MODE_MSK, adc_ctrl.mode);

	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_CTRL, reg);
	if (ret)
		return ret;

	if ((dev->config.adc_ctrl.cont_read != adc_ctrl.cont_read) &&
	    (adc_ctrl.cont_read == AD4170_CONT_TRANSMIT_ON)) {
		_ad4170_digif_remove(dev);
	}

	dev->config.adc_ctrl = adc_ctrl;

	return 0;
}

/**
 * @brief Enable AD4170 channels
 * @param dev - The device structure.
 * @param channel_en - Channel enable mask.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_channel_en(struct ad4170_dev *dev, uint16_t channel_en)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_CHANNEL_EN, channel_en);
	if (ret)
		return ret;

	dev->config.channel_en = channel_en;

	return 0;
}

/**
 * @brief Set the AD4170 Channel specific setup
 * @param dev - The device structure.
 * @param ch - The channel number (0 to 15).
 * @param setup - AD4170 Channel specific setup configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_channel_setup(struct ad4170_dev *dev, uint8_t ch,
			     struct ad4170_channel_setup setup)
{
	int ret;
	uint32_t reg;

	if (!dev || ch >= AD4170_NUM_CHANNELS)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_CHANNEL_SETUPN_REPEAT_N_MSK, setup.repeat_n);
	reg |= no_os_field_prep(AD4170_CHANNEL_SETUPN_DELAY_N_MSK, setup.delay_n);
	reg |= no_os_field_prep(AD4170_CHANNEL_SETUPN_SETUP_N_MSK, setup.setup_n);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_CHANNEL_SETUP(ch), reg);
	if (ret)
		return ret;

	dev->config.setup[ch] = setup;

	return 0;
}

/**
 * @brief Set the AD4170 Channel specific map
 * @param dev - The device structure.
 * @param ch - The channel number (0 to 15).
 * @param map - AD4170 Channel specific map configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_channel_map(struct ad4170_dev *dev, uint8_t ch,
			   struct ad4170_channel_map map)
{
	int ret;
	uint32_t reg;

	if (!dev || ch >= AD4170_NUM_CHANNELS)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_CHANNEL_MAPN_AINP_MSK, map.ainp);
	reg |= no_os_field_prep(AD4170_CHANNEL_MAPN_AINM_MSK, map.ainm);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_CHANNEL_MAP(ch), reg);
	if (ret)
		return ret;

	dev->config.map[ch] = map;

	return 0;
}

/**
 * @brief Configure an ADC setup
 * @param dev - The device structure.
 * @param n - The setup number (0 to 8).
 * @param setup - AD4170 setup configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_setup(struct ad4170_dev *dev, uint8_t n,
		     struct ad4170_setup setup)
{
	int ret;
	uint32_t reg;

	if (!dev || n >= AD4170_NUM_SETUPS)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_ADC_SETUPS_MISC_CHOP_IEXC_MSK,
			       setup.misc.chop_iexc);
	reg |= no_os_field_prep(AD4170_ADC_SETUPS_MISC_CHOP_ADC_MSK,
				setup.misc.chop_adc);
	reg |= no_os_field_prep(AD4170_ADC_SETUPS_MISC_BURNOUT_MSK, setup.misc.burnout);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_SETUPS_MISC(n), reg);
	if (ret)
		return ret;

	dev->config.setups[n].misc = setup.misc;

	reg = no_os_field_prep(AD4170_ADC_SETUPS_AFE_REF_BUF_M_MSK,
			       setup.afe.ref_buf_m);
	reg |= no_os_field_prep(AD4170_ADC_SETUPS_AFE_REF_BUF_P_MSK,
				setup.afe.ref_buf_p);
	reg |= no_os_field_prep(AD4170_ADC_SETUPS_AFE_REF_SELECT_MSK,
				setup.afe.ref_select);
	reg |= no_os_field_prep(AD4170_ADC_SETUPS_AFE_BIPOLAR_MSK, setup.afe.bipolar);
	reg |= no_os_field_prep(AD4170_ADC_SETUPS_AFE_PGA_GAIN_MSK, setup.afe.pga_gain);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_SETUPS_AFE(n), reg);
	if (ret)
		return ret;

	dev->config.setups[n].afe = setup.afe;

	reg = no_os_field_prep(AD4170_ADC_SETUPS_POST_FILTER_SEL_MSK,
			       setup.filter.post_filter_sel);
	reg |= no_os_field_prep(AD4170_ADC_SETUPS_FILTER_TYPE_MSK,
				setup.filter.filter_type);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_SETUPS_FILTER(n), reg);
	if (ret)
		return ret;

	dev->config.setups[n].filter = setup.filter;

	reg = setup.filter_fs;
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_SETUPS_FILTER_FS(n), reg);
	if (ret)
		return ret;

	dev->config.setups[n].filter_fs = setup.filter_fs;

	reg = setup.offset;
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_SETUPS_OFFSET(n), reg);
	if (ret)
		return ret;

	dev->config.setups[n].offset = setup.offset;

	reg = setup.gain;
	ret = ad4170_spi_reg_write(dev, AD4170_REG_ADC_SETUPS_GAIN(n), reg);
	if (ret)
		return ret;

	dev->config.setups[n].gain = setup.gain;

	return 0;
}

/**
 * @brief Set the AD4170 Ref Control
 * @param dev - The device structure.
 * @param enable - Enable/Disable Ref Control.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_ref_control(struct ad4170_dev *dev, bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_REF_CONTROL, enable);
	if (ret)
		return ret;

	dev->config.ref_control.ref_en = enable;

	return 0;
}

/**
 * @brief Set the AD4170 Voltage bias
 * @param dev - The device structure.
 * @param ch_mask - Voltage bias enable mask.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_v_bias(struct ad4170_dev *dev, uint16_t ch_mask)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_V_BIAS, ch_mask);
	if (ret)
		return ret;

	dev->config.v_bias = ch_mask;

	return 0;
}

/**
 * @brief Set the AD4170 Input pullup
 * @param dev - The device structure.
 * @param ch_mask - Input pullup enable mask.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_i_pullup(struct ad4170_dev *dev, uint16_t ch_mask)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4170_spi_reg_write(dev, AD4170_REG_I_PULLUP, ch_mask);
	if (ret)
		return ret;

	dev->config.i_pullup = ch_mask;

	return 0;
}

/**
 * @brief Set the AD4170 Excitation Current
 * @param dev - The device structure.
 * @param n - Current source number (0 to 3).
 * @param current_source - Excitation current confguration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_current_source(struct ad4170_dev *dev, uint8_t n,
			      struct ad4170_current_source current_source)
{
	int ret;
	uint32_t reg;

	if (!dev || n >= AD4170_NUM_CURRENT_SOURCE)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_CURRENT_SOURCE_I_OUT_PIN_MSK,
			       current_source.i_out_pin);
	reg |= no_os_field_prep(AD4170_CURRENT_SOURCE_I_OUT_VAL_MSK,
				current_source.i_out_val);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_CURRENT_SOURCE(n), reg);
	if (ret)
		return ret;

	dev->config.current_source[n] = current_source;

	return 0;
}

/**
 * @brief Set the AD4170 FIR control settings
 * @param dev - The device structure.
 * @param fir_control - FIR control configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_fir_control(struct ad4170_dev *dev,
			   struct ad4170_fir_control fir_control)
{
	int ret;
	uint32_t reg;
	uint8_t fir_coeff_wr_start_addr;
	uint8_t fir_coeff_wr_end_addr;
	uint8_t fir_coeff_wr_addr_indx;
	uint8_t fir_coeff_data_indx = 0;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_FIR_CONTROL_FIR_MODE_MSK, fir_control.fir_mode);
	reg |= no_os_field_prep(AD4170_FIR_CONTROL_COEFF_SET_MSK,
				fir_control.coeff_set);
	reg |= no_os_field_prep(AD4170_FIR_CONTROL_FIR_LENGTH_MSK,
				fir_control.fir_length);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_FIR_CONTROL, reg);
	if (ret)
		return ret;

	/* Write the FIR coefficients */
	if ((fir_control.fir_length > 0)
	    && (fir_control.fir_length <= AD4170_FIR_COEFF_MAX_LENGTH)
	    && (fir_control.fir_mode != AD4170_FIR_DEFAULT)) {
		/* Load the FIR coefficient start and end address based on the coeff set */
		if (fir_control.coeff_set == AD4170_FIR_COEFF_SET0) {
			fir_coeff_wr_start_addr = 0;
			fir_coeff_wr_end_addr = fir_control.fir_length;
		} else if (fir_control.coeff_set == AD4170_FIR_COEFF_SET1) {
			fir_coeff_wr_start_addr = AD4170_FIR_COEFF_MAX_LENGTH;
			fir_coeff_wr_end_addr = AD4170_FIR_COEFF_MAX_LENGTH + fir_control.fir_length;
		} else {
			return -EINVAL;
		}

		for (fir_coeff_wr_addr_indx = fir_coeff_wr_start_addr;
		     fir_coeff_wr_addr_indx < fir_coeff_wr_end_addr;
		     fir_coeff_wr_addr_indx++, fir_coeff_data_indx++) {
			/* Set the FIR coefficient address */
			ret = ad4170_spi_reg_write(dev, AD4170_REG_COEFF_ADDRESS,
						   fir_coeff_wr_addr_indx);
			if (ret)
				return ret;

			/* Set the FIR coefficient value */
			ret = ad4170_spi_reg_write(dev, AD4170_REG_COEFF_WRITE_DATA,
						   fir_control.fir_coefficients[fir_coeff_data_indx]);
			if (ret)
				return ret;
		}
	}

	dev->config.fir_control = fir_control;

	return 0;
}

/**
 * @brief Set the AD4170 DAC settings
 * @param dev - The device structure.
 * @param config - The DAC configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_dac_config(struct ad4170_dev *dev,
			  struct ad4170_dac_config config)
{
	int ret;
	uint32_t reg;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD4170_REG_DAC_SPAN_DAC_GAIN_MSK, config.gain);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_DAC_SPAN, reg);
	if (ret)
		return ret;

	dev->config.dac.gain = config.gain;

	reg = no_os_field_prep(AD4170_REG_DAC_HW_LDAC_MASK_HW_LDAC_EN_MSK,
			       config.hw_ldac);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_DAC_HW_LDAC_MASK, reg);
	if (ret)
		return ret;

	dev->config.dac.hw_ldac = config.hw_ldac;

	if (config.hw_ldac) {
		struct ad4170_pin_muxing pin_muxing = dev->config.pin_muxing;
		pin_muxing.dig_aux2_ctrl = AD4170_DIG_AUX2_LDAC;
		ret = ad4170_set_pin_muxing(dev, pin_muxing);
		if (ret)
			return ret;
	}

	reg = no_os_field_prep(AD4170_REG_DAC_HW_TOGGLE_MASK_HW_TOGGLE_EN_MSK,
			       config.hw_toggle);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_DAC_HW_TOGGLE_MASK, reg);
	if (ret)
		return ret;

	dev->config.dac.hw_toggle = config.hw_toggle;

	reg = no_os_field_prep(AD4170_REG_DAC_CHANNEL_EN_DAC_EN_MSK, config.enabled);
	ret = ad4170_spi_reg_write(dev, AD4170_REG_DAC_CHANNEL_EN, reg);
	if (ret)
		return ret;

	dev->config.dac.enabled = config.enabled;

	return 0;
}

/**
 * @brief Set the AD4170 DAC data by directly writing the DAT_DATA register.
 * @param dev - The device structure.
 * @param code - The DAC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_dac_data(struct ad4170_dev *dev, uint16_t code)
{
	return ad4170_spi_reg_write(dev, AD4170_REG_DAC_DATA(0),
				    no_os_field_prep(AD4170_REG_DAC_DATA_MSK, code));
}

/**
 * @brief Set the AD4170 DAC data by writing the INPUTA register to be loaded separately by LDAC or Toggle operations.
 * @param dev - The device structure.
 * @param code - The DAC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_dac_inputa(struct ad4170_dev *dev, uint16_t code)
{
	return ad4170_spi_reg_write(dev, AD4170_REG_DAC_INPUTA(0),
				    no_os_field_prep(AD4170_REG_DAC_DATA_MSK, code));
}

/**
 * @brief Set the AD4170 DAC data by writing the INPUTB register to be loaded separately by LDAC or Toggle operations.
 * @param dev - The device structure.
 * @param code - The DAC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_set_dac_inputb(struct ad4170_dev *dev, uint16_t code)
{
	return ad4170_spi_reg_write(dev, AD4170_REG_DAC_INPUTB(0),
				    no_os_field_prep(AD4170_REG_DAC_DATA_MSK, code));
}

/**
 * @brief Perform a software LDAC
 * @param dev - The device structure.
 * @param polarity - Polarity of the LDAC operation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_dac_sw_ldac(struct ad4170_dev *dev, bool polarity)
{
	return ad4170_spi_reg_write(dev, AD4170_REG_DAC_SW_LDAC_TRIGGERS,
				    no_os_field_prep(AD4170_REG_DAC_SW_LDAC_TRIGGERS_SW_LDAC_EN_MSK, polarity));
}

/**
 * @brief Perform a software toggle
 * @param dev - The device structure.
 * @param polarity - Polarity of the toggle operation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_dac_sw_toggle(struct ad4170_dev *dev, bool polarity)
{
	return ad4170_spi_reg_write(dev, AD4170_REG_DAC_SW_TOGGLE_TRIGGERS,
				    no_os_field_prep(AD4170_REG_DAC_SW_TOGGLE_TRIGGERS_SW_TOGGLE_MSK, polarity));
}

/**
 * @brief Perform a hardware toggle using the DIG_AUX2 pin, assumed to be pre-configured for this.
 * @param dev - The device structure.
 * @param polarity - Polarity of the toggle operation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_dac_hw_toggle(struct ad4170_dev *dev, bool polarity)
{
	if (!dev)
		return -EINVAL;

	if (!dev->gpio_dig_aux2
	    || (dev->config.pin_muxing.dig_aux2_ctrl != AD4170_DIG_AUX2_LDAC))
		return -ENOTSUP;

	return no_os_gpio_set_value(dev->gpio_dig_aux2,
				    polarity ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

/**
 * @brief Initialize an AD4170 device structure.
 * @param device - Pointer to the device structure (memory is allocated within this function).
 * @param init_param - Pointer to the initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_init(struct ad4170_dev **device,
		struct ad4170_init_param *init_param)
{
	struct ad4170_dev *dev;
	int32_t ret, ch;
	uint32_t reg;
	uint16_t status;
	uint8_t prod_id_h_expected;
	uint8_t prod_id_l_expected;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct ad4170_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->big_endian = _is_big_endian();
	no_os_crc8_populate_msb(ad4170_crc8, 0x7);

	dev->id = init_param->id;
	dev->spi_init = init_param->spi_init;
	dev->rdy_conv_timeout = init_param->rdy_conv_timeout;
	dev->gpio_dig_aux1_init = init_param->gpio_dig_aux1;
	dev->gpio_dig_aux2_init = init_param->gpio_dig_aux2;
	dev->dig_aux2_output = init_param->config.pin_muxing.dig_aux2_ctrl ==
			       AD4170_DIG_AUX2_LDAC ||
			       init_param->config.pin_muxing.dig_aux2_ctrl == AD4170_DIG_AUX2_SYNC;
	dev->gpio_sync_inb_init = init_param->gpio_sync_inb;

	ret = _ad4170_digif_init(dev);
	if (ret)
		goto error;

	no_os_mdelay(100);

	ret = ad4170_reset(dev);
	if (ret)
		goto error;

	ret = ad4170_set_spi_settings(dev, &init_param->spi_settings);
	if (ret)
		goto error;

	switch (dev->id) {
	case ID_AD4170:
	case ID_AD4171:
	case ID_AD4172:
		prod_id_l_expected = AD4170_PRODUCT_ID_L_VALUE;
		prod_id_h_expected = AD4170_PRODUCT_ID_H_VALUE;
		break;

	case ID_AD4190:
		prod_id_l_expected = AD4190_PRODUCT_ID_L_VALUE;
		prod_id_h_expected = AD4190_PRODUCT_ID_H_VALUE;
		break;

	default:
		goto error;
	}

	ret = ad4170_spi_reg_read(dev, AD4170_REG_PRODUCT_ID_L, &reg);
	if (ret)
		goto error;
	if (reg != prod_id_l_expected) {
		ret = -EFAULT;
		goto error;
	}

	ret = ad4170_spi_reg_read(dev, AD4170_REG_PRODUCT_ID_H, &reg);
	if (ret)
		goto error;
	if (reg != prod_id_h_expected) {
		ret = -EFAULT;
		goto error;
	}

	ret = ad4170_get_status(dev, &status);
	if (ret)
		goto error;
	if (!(status & AD4170_DATA_STATUS_POR_FLAG_S_MSK)) {
		/* If poweron reset bit is not set, the sw reset didn't work. */
		ret = -EFAULT;
		goto error;
	}

	ret = ad4170_spi_reg_write(dev, AD4170_REG_DATA_STATUS,
				   AD4170_DATA_STATUS_POR_FLAG_S_MSK);
	if (ret)
		goto error;

	ret = _scratchpad_test(dev, 0xa5);
	if (ret)
		goto error;
	ret = _scratchpad_test(dev, 0x5a);
	if (ret)
		goto error;

	ret = ad4170_set_pin_muxing(dev, init_param->config.pin_muxing);
	if (ret)
		goto error;

	ret = ad4170_set_dclk_div(dev, init_param->config.clock_ctrl.dclk_divide);
	if (ret)
		goto error;

	ret = ad4170_set_mclk_div(dev, init_param->config.clock_ctrl.clockdiv);
	if (ret)
		goto error;

	ret = ad4170_set_clocksel(dev, init_param->config.clock_ctrl.clocksel);
	if (ret)
		goto error;

	ret = ad4170_set_standby_ctrl(dev, init_param->config.standby_ctrl);
	if (ret)
		goto error;

	ret = ad4170_set_powerdown_sw(dev, init_param->config.powerdown_sw);
	if (ret)
		goto error;

	ret = ad4170_set_error_en(dev, init_param->config.error_en);
	if (ret)
		goto error;

	ret = ad4170_set_adc_ctrl(dev, init_param->config.adc_ctrl);
	if (ret)
		goto error;

	for (ch = 0; ch < AD4170_NUM_SETUPS; ch++) {
		ret = ad4170_set_setup(dev, ch, init_param->config.setups[ch]);
		if (ret)
			goto error;
	}

	for(ch = 0; ch < AD4170_NUM_CHANNELS; ch++) {
		if (!(init_param->config.channel_en & AD4170_CHANNEL(ch)))
			continue;

		ret = ad4170_set_channel_setup(dev, ch, init_param->config.setup[ch]);
		if (ret)
			goto error;

		ret = ad4170_set_channel_map(dev, ch, init_param->config.map[ch]);
		if (ret)
			goto error;
	}

	ret = ad4170_set_channel_en(dev, init_param->config.channel_en);
	if (ret)
		goto error;

	ret = ad4170_set_ref_control(dev, init_param->config.ref_control.ref_en);
	if (ret)
		goto error;

	ret = ad4170_set_v_bias(dev, init_param->config.v_bias);
	if (ret)
		goto error;

	ret = ad4170_set_i_pullup(dev, init_param->config.i_pullup);
	if (ret)
		goto error;

	for (ch = 0; ch < AD4170_NUM_CURRENT_SOURCE; ch++) {
		ret = ad4170_set_current_source(dev, ch, init_param->config.current_source[ch]);
		if (ret)
			goto error;
	}

	ret = ad4170_set_fir_control(dev, init_param->config.fir_control);
	if (ret)
		goto error;

	ret = ad4170_set_dac_config(dev, init_param->config.dac);
	if (ret)
		goto error;

	*device = dev;

	return 0;
error:
	printf("ad4170 initialization failed with status %d\n", ret);
	ad4170_remove(dev);

	return ret;
}

/**
 * @brief Remove the device and free al the resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_remove(struct ad4170_dev *dev)
{
	_ad4170_digif_remove(dev);
	free(dev);

	return 0;
}

/**
 * @brief Debugging function to print the register map to console.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4170_regmap(struct ad4170_dev *dev)
{
	uint32_t reg;
	int ret;
	uint32_t regs[] = {
		AD4170_REG_INTERFACE_CONFIG_A,
		AD4170_REG_INTERFACE_CONFIG_B,
		AD4170_REG_DEVICE_CONFIG,
		AD4170_REG_CHIP_TYPE,
		AD4170_REG_PRODUCT_ID_L,
		AD4170_REG_PRODUCT_ID_H,
		AD4170_REG_CHIP_GRADE,
		AD4170_REG_SCRATCH_PAD,
		AD4170_REG_SPI_REVISION,
		AD4170_REG_VENDOR_L,
		AD4170_REG_VENDOR_H,
		AD4170_REG_INTERFACE_CONFIG_C,
		AD4170_REG_INTERFACE_STATUS_A,
		AD4170_REG_DATA_STATUS,
		AD4170_REG_DATA_16b,
		AD4170_REG_DATA_16b_STATUS,
		AD4170_REG_DATA_24b,
		AD4170_REG_DATA_24b_STATUS,
		AD4170_REG_DATA_32b,
		AD4170_REG_PIN_MUXING,
		AD4170_REG_CLOCK_CTRL,
		AD4170_REG_STANDBY_CTRL,
		AD4170_REG_POWER_DOWN_SW,
		AD4170_REG_ADC_CTRL,
		AD4170_REG_ERROR_EN,
		AD4170_REG_ERROR,
		AD4170_REG_CHANNEL_EN,
		AD4170_REG_ADC_CHANNEL_SETUP(0),
		AD4170_REG_ADC_CHANNEL_MAP(0),
		AD4170_REG_ADC_CHANNEL_SETUP(1),
		AD4170_REG_ADC_CHANNEL_MAP(1),
		AD4170_REG_ADC_CHANNEL_SETUP(2),
		AD4170_REG_ADC_CHANNEL_MAP(2),
		AD4170_REG_ADC_CHANNEL_SETUP(3),
		AD4170_REG_ADC_CHANNEL_MAP(3),
		AD4170_REG_ADC_CHANNEL_SETUP(4),
		AD4170_REG_ADC_CHANNEL_MAP(4),
		AD4170_REG_ADC_CHANNEL_SETUP(5),
		AD4170_REG_ADC_CHANNEL_MAP(5),
		AD4170_REG_ADC_CHANNEL_SETUP(6),
		AD4170_REG_ADC_CHANNEL_MAP(6),
		AD4170_REG_ADC_CHANNEL_SETUP(7),
		AD4170_REG_ADC_CHANNEL_MAP(7),
		AD4170_REG_ADC_CHANNEL_SETUP(8),
		AD4170_REG_ADC_CHANNEL_MAP(8),
		AD4170_REG_ADC_CHANNEL_SETUP(9),
		AD4170_REG_ADC_CHANNEL_MAP(9),
		AD4170_REG_ADC_CHANNEL_SETUP(10),
		AD4170_REG_ADC_CHANNEL_MAP(10),
		AD4170_REG_ADC_CHANNEL_SETUP(11),
		AD4170_REG_ADC_CHANNEL_MAP(11),
		AD4170_REG_ADC_CHANNEL_SETUP(12),
		AD4170_REG_ADC_CHANNEL_MAP(12),
		AD4170_REG_ADC_CHANNEL_SETUP(13),
		AD4170_REG_ADC_CHANNEL_MAP(13),
		AD4170_REG_ADC_CHANNEL_SETUP(14),
		AD4170_REG_ADC_CHANNEL_MAP(14),
		AD4170_REG_ADC_CHANNEL_SETUP(15),
		AD4170_REG_ADC_CHANNEL_MAP(15),
		AD4170_REG_ADC_SETUPS_MISC(0),
		AD4170_REG_ADC_SETUPS_AFE(0),
		AD4170_REG_ADC_SETUPS_FILTER(0),
		AD4170_REG_ADC_SETUPS_FILTER_FS(0),
		AD4170_REG_ADC_SETUPS_OFFSET(0),
		AD4170_REG_ADC_SETUPS_GAIN(0),
		AD4170_REG_ADC_SETUPS_MISC(1),
		AD4170_REG_ADC_SETUPS_AFE(1),
		AD4170_REG_ADC_SETUPS_FILTER(1),
		AD4170_REG_ADC_SETUPS_FILTER_FS(1),
		AD4170_REG_ADC_SETUPS_OFFSET(1),
		AD4170_REG_ADC_SETUPS_GAIN(1),
		AD4170_REG_ADC_SETUPS_MISC(2),
		AD4170_REG_ADC_SETUPS_AFE(2),
		AD4170_REG_ADC_SETUPS_FILTER(2),
		AD4170_REG_ADC_SETUPS_FILTER_FS(2),
		AD4170_REG_ADC_SETUPS_OFFSET(2),
		AD4170_REG_ADC_SETUPS_GAIN(2),
		AD4170_REG_ADC_SETUPS_MISC(3),
		AD4170_REG_ADC_SETUPS_AFE(3),
		AD4170_REG_ADC_SETUPS_FILTER(3),
		AD4170_REG_ADC_SETUPS_FILTER_FS(3),
		AD4170_REG_ADC_SETUPS_OFFSET(3),
		AD4170_REG_ADC_SETUPS_GAIN(3),
		AD4170_REG_ADC_SETUPS_MISC(4),
		AD4170_REG_ADC_SETUPS_AFE(4),
		AD4170_REG_ADC_SETUPS_FILTER(4),
		AD4170_REG_ADC_SETUPS_FILTER_FS(4),
		AD4170_REG_ADC_SETUPS_OFFSET(4),
		AD4170_REG_ADC_SETUPS_GAIN(4),
		AD4170_REG_ADC_SETUPS_MISC(5),
		AD4170_REG_ADC_SETUPS_AFE(5),
		AD4170_REG_ADC_SETUPS_FILTER(5),
		AD4170_REG_ADC_SETUPS_FILTER_FS(5),
		AD4170_REG_ADC_SETUPS_OFFSET(5),
		AD4170_REG_ADC_SETUPS_GAIN(5),
		AD4170_REG_ADC_SETUPS_MISC(6),
		AD4170_REG_ADC_SETUPS_AFE(6),
		AD4170_REG_ADC_SETUPS_FILTER(6),
		AD4170_REG_ADC_SETUPS_FILTER_FS(6),
		AD4170_REG_ADC_SETUPS_OFFSET(6),
		AD4170_REG_ADC_SETUPS_GAIN(6),
		AD4170_REG_ADC_SETUPS_MISC(7),
		AD4170_REG_ADC_SETUPS_AFE(7),
		AD4170_REG_ADC_SETUPS_FILTER(7),
		AD4170_REG_ADC_SETUPS_FILTER_FS(7),
		AD4170_REG_ADC_SETUPS_OFFSET(7),
		AD4170_REG_ADC_SETUPS_GAIN(7),
		AD4170_REG_REF_CONTROL,
		AD4170_REG_V_BIAS,
		AD4170_REG_I_PULLUP,
		AD4170_REG_CURRENT_SOURCE(0),
		AD4170_REG_CURRENT_SOURCE(1),
		AD4170_REG_CURRENT_SOURCE(2),
		AD4170_REG_CURRENT_SOURCE(3),
		AD4170_REG_FIR_CONTROL,
		AD4170_REG_COEFF_WRITE_DATA,
		AD4170_REG_COEFF_READ_DATA,
		AD4170_REG_COEFF_ADDRESS,
		AD4170_REG_COEFF_WRRD_STB,
		AD4170_REG_DAC_SPAN,
		AD4170_REG_DAC_CHANNEL_EN,
		AD4170_REG_DAC_HW_TOGGLE_MASK,
		AD4170_REG_DAC_HW_LDAC_MASK,
		AD4170_REG_DAC_DATA(0),
		AD4170_REG_DAC_SW_TOGGLE_TRIGGERS,
		AD4170_REG_DAC_SW_LDAC_TRIGGERS,
		AD4170_REG_DAC_INPUTA(0),
		AD4170_REG_DAC_INPUTB(0),
	};

	uint32_t ri;
	printf("[register]: [value]\n");
	for(ri = 0; ri < NO_OS_ARRAY_SIZE(regs); ri++) {
		ret = ad4170_spi_reg_read(dev, regs[ri], &reg);
		if (ret) {
			printf("error %d while reading reg 0x%x", ret, AD4170_ADDR(regs[ri]));
			return ret;
		}

		printf("0x%.4x: 0x%x\n", AD4170_ADDR(regs[ri]), reg);
	}

	return 0;
}


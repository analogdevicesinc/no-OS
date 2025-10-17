/***************************************************************************//**
 *   @file   max22007.c
 *   @brief  Source file of MAX22007 Driver.
 *   @author Janani Sunil (janani.sunil@analog.com)
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
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_crc8.h"
#include "no_os_spi.h"
#include "max22007.h"
#include <string.h>

#define MAX22007_CRC8_POLYNOMIAL   0x8C
NO_OS_DECLARE_CRC8_TABLE(max22007_crc8_table);

/**
 * @brief Read from a register
 * @param dev - MAX22007 device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Register data.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_reg_read(struct max22007_dev *dev, uint8_t reg_addr,
		      uint16_t *reg_data)
{
	int ret;
	uint8_t crc;
	uint8_t cmd_byte = no_os_field_prep(MAX22007_ADRR_MASK,
					    reg_addr) | no_os_field_prep(MAX22007_RW_MASK, 1);
	uint8_t crc_data[3] = {cmd_byte, 0, 0};
	struct no_os_spi_msg xfer = {
		.tx_buff = dev->buff,
		.rx_buff = dev->buff,
		.bytes_number = MAX22007_FRAME_SIZE,
		.cs_change = 1,
	};

	if (!dev || !reg_data)
		return -EINVAL;

	if (dev->crc_en)
		xfer.bytes_number++;

	dev->buff[0] = cmd_byte;

	if (dev->crc_en)
		dev->buff[3] = no_os_crc8(max22007_crc8_table, dev->buff, xfer.bytes_number,
					  0x00);

	ret = no_os_spi_transfer(dev->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (dev->crc_en) {
		crc_data[1]	= dev->buff[1];
		crc_data[2]	= dev->buff[2];
		crc = no_os_crc8(max22007_crc8_table, crc_data, MAX22007_FRAME_SIZE, 0x00);

		if (crc != dev->buff[3]) {
			pr_info("CRC mismatch: calculated 0x%02X, received 0x%02X\n",
				crc, dev->buff[3]);
			return -EINVAL;
		}
	}

	*reg_data = (dev->buff[1] << 8) | dev->buff[2];

	return 0;
}

/**
 * @brief Write to a register
 * @param dev - MAX22007 device descriptor.
 * @param reg_addr - Register address.
 * @param reg_data - Register data.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_reg_write(struct max22007_dev *dev, uint8_t reg_addr,
		       uint16_t reg_data)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = dev->buff,
		.rx_buff = dev->buff,
		.bytes_number = MAX22007_FRAME_SIZE,
		.cs_change = 1,
	};

	if (!dev)
		return -EINVAL;

	if (dev->crc_en)
		xfer.bytes_number++;

	dev->buff[0] = no_os_field_prep(MAX22007_ADRR_MASK, reg_addr) |
		       no_os_field_prep(MAX22007_RW_MASK, 0);
	dev->buff[1] = (reg_data >> 8) & 0xFF;
	dev->buff[2] = reg_data & 0xFF;

	if (dev->crc_en)
		dev->buff[3] = no_os_crc8(max22007_crc8_table, dev->buff, 3, 0x00);

	return no_os_spi_transfer(dev->comm_desc, &xfer, 1);
}

/**
 * @brief Register write with mask
 * @param dev - MAX22007 device descriptor.
 * @param reg_addr - Register address.
 * @param data - Register data.
 * @param mask - Register mask.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_reg_write_msk(struct max22007_dev *dev,
			   uint8_t reg_addr,
			   uint16_t data,
			   uint16_t mask)
{
	int ret;
	uint16_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = max22007_reg_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return max22007_reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Set DAC latch mode
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number.
 * @param mode - Latch mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_set_latch_mode(struct max22007_dev *dev, uint8_t ch,
			    enum max22007_dac_latch_mode mode)
{
	uint16_t reg_val;

	if (!dev || (ch > (MAX22007_NUM_CHANNELS - 1)))
		return -EINVAL;

	reg_val = no_os_field_prep(LD_CTRL_CH_MASK(ch), mode);

	return max22007_reg_write_msk(dev, MAX22007_CONFIG_REG, reg_val,
				      LD_CTRL_CH_MASK(ch));
}

/**
 * @brief Get DAC latch mode
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number.
 * @param mode - Latch mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_get_latch_mode(struct max22007_dev *dev, uint8_t ch,
			    enum max22007_dac_latch_mode *mode)
{
	uint16_t reg_data;
	int ret;

	if (!dev || (ch > (MAX22007_NUM_CHANNELS - 1)) || !mode)
		return -EINVAL;

	ret = max22007_reg_read(dev, MAX22007_CONFIG_REG, &reg_data);
	if (ret)
		return ret;

	*mode = no_os_field_get(LD_CTRL_CH_MASK(ch), reg_data);

	return 0;
}

/**
 * @brief Set reference source
 * @param dev - MAX22007 device descriptor.
 * @param mode - Reference mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_set_reference(struct max22007_dev *dev,
			   enum max22007_ref_mode mode)
{
	uint16_t reg_val;

	if (!dev)
		return -EINVAL;

	reg_val = no_os_field_prep(MAX22007_CONFIG_REF_SEL_MASK, mode);

	return max22007_reg_write_msk(dev, MAX22007_CONFIG_REG, reg_val,
				      MAX22007_CONFIG_REF_SEL_MASK);
}

/**
 * @brief Get reference source
 * @param dev - MAX22007 device descriptor.
 * @param mode - Reference mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_get_reference(struct max22007_dev *dev,
			   enum max22007_ref_mode *mode)
{
	uint16_t reg_data;
	int ret;

	if (!dev || !mode)
		return -EINVAL;

	ret = max22007_reg_read(dev, MAX22007_CONFIG_REG, &reg_data);
	if (ret)
		return ret;

	*mode = no_os_field_get(MAX22007_CONFIG_REF_SEL_MASK, reg_data);

	return 0;
}

/**
 * @brief Set timeout configuration
 * @param dev - MAX22007 device descriptor.
 * @param timeout_en - Timeout enable/disable.
 * @param timeout_sel - Timeout selection.
 * @param timeout_cnfg - Timeout configuration.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_set_timeout(struct max22007_dev *dev, bool timeout_en,
			 enum max22007_timeout_sel timeout_sel, enum max22007_timeout_cnfg timeout_cnfg)
{
	uint16_t reg_val;

	if (!dev)
		return -EINVAL;

	reg_val = no_os_field_prep(MAX22007_TIMEOUT_SEL_MASK, timeout_sel) |
		  no_os_field_prep(MAX22007_TIMEOUT_CNFG_MASK, timeout_cnfg) |
		  no_os_field_prep(MAX22007_TIMEOUT_EN_MASK, timeout_en);

	return max22007_reg_write_msk(dev, MAX22007_CONFIG_REG, reg_val,
				      MAX22007_TIMEOUT_SEL_MASK | MAX22007_TIMEOUT_CNFG_MASK |
				      MAX22007_TIMEOUT_EN_MASK);
}

/**
 * @brief Get timeout configuration
 * @param dev - MAX22007 device descriptor.
 * @param timeout_en - Timeout Enable.
 * @param timeout_sel - Timeout Selection.
 * @param timeout_cnfg - Timeout Configuration.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_get_timeout(struct max22007_dev *dev,
			 bool *timeout_en,
			 enum max22007_timeout_sel *timeout_sel,
			 enum max22007_timeout_cnfg *timeout_cnfg)
{
	uint16_t reg_data;
	int ret;

	if (!dev || !timeout_en || !timeout_sel || !timeout_cnfg)
		return -EINVAL;

	ret = max22007_reg_read(dev, MAX22007_CONFIG_REG, &reg_data);
	if (ret)
		return ret;

	*timeout_en = no_os_field_get(MAX22007_TIMEOUT_EN_MASK, reg_data);
	*timeout_sel = no_os_field_get(MAX22007_TIMEOUT_SEL_MASK, reg_data);
	*timeout_cnfg = no_os_field_get(MAX22007_TIMEOUT_CNFG_MASK, reg_data);

	return 0;
}

/**
 * @brief Set channel mode
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number.
 * @param mode - Channel mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_set_channel_mode(struct max22007_dev *dev, uint8_t ch,
			      enum max22007_channel_mode mode)
{
	uint16_t reg_val;

	if (!dev || (ch > (MAX22007_NUM_CHANNELS - 1)))
		return -EINVAL;

	reg_val = no_os_field_prep(MAX22007_CH_MODE_CH_MASK(ch), mode);

	return max22007_reg_write_msk(dev, MAX22007_CHANNEL_MODE_REG, reg_val,
				      MAX22007_CH_MODE_CH_MASK(ch));
}

/**
 * @brief Get channel mode
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number.
 * @param mode - Channel mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_get_channel_mode(struct max22007_dev *dev, uint8_t ch,
			      enum max22007_channel_mode *mode)
{
	uint16_t reg_data;
	int ret;

	if (!dev || (ch > (MAX22007_NUM_CHANNELS - 1)) || !mode)
		return -EINVAL;

	ret = max22007_reg_read(dev, MAX22007_CHANNEL_MODE_REG, &reg_data);
	if (ret)
		return ret;

	*mode = no_os_field_get(MAX22007_CH_MODE_CH_MASK(ch), reg_data);

	return 0;
}

/**
 * @brief Set channel power
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number.
 * @param mode - Power mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_set_channel_power(struct max22007_dev *dev, uint8_t ch,
			       enum max22007_channel_power mode)
{
	uint16_t reg_val;

	if (!dev || (ch > (MAX22007_NUM_CHANNELS - 1)))
		return -EINVAL;

	reg_val = no_os_field_prep(MAX22007_CH_PWR_CH_MASK(ch), mode);

	return max22007_reg_write_msk(dev, MAX22007_CHANNEL_MODE_REG, reg_val,
				      MAX22007_CH_PWR_CH_MASK(ch));
}

/**
 * @brief Get channel power
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number.
 * @param mode - Power mode.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_get_channel_power(struct max22007_dev *dev, uint8_t ch,
			       enum max22007_channel_power *mode)
{
	uint16_t reg_data;
	int ret;

	if (!dev || (ch > (MAX22007_NUM_CHANNELS - 1)) || !mode)
		return -EINVAL;

	ret = max22007_reg_read(dev, MAX22007_CHANNEL_MODE_REG, &reg_data);
	if (ret)
		return ret;

	*mode = no_os_field_get(MAX22007_CH_PWR_CH_MASK(ch), reg_data);

	return 0;
}

/**
 * @brief Configure CRC
 * @param dev - MAX22007 device descriptor.
 * @param crc_en - CRC enable/disable.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_configure_crc(struct max22007_dev *dev, bool crc_en)
{
	int ret;
	uint16_t reg_val;

	if (!dev)
		return -EINVAL;

	reg_val = no_os_field_prep(MAX22007_CRC_EN_MASK, crc_en);

	ret = max22007_reg_write_msk(dev, MAX22007_CONFIG_REG, reg_val,
				     MAX22007_CRC_EN_MASK);
	if (ret)
		return ret;

	dev->crc_en = crc_en;

	return 0;
}

/**
 * @brief Write data to a DAC channel data register
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number (0-3).
 * @param data - 16-bit data to write to the channel.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_write_channel_data(struct max22007_dev *dev, uint8_t ch,
				uint16_t data)
{
	uint16_t reg_val;

	if (!dev || (ch > (MAX22007_NUM_CHANNELS - 1)))
		return -EINVAL;

	reg_val = no_os_field_prep(DAC_CH_DATA_MASK, data);

	return max22007_reg_write_msk(dev, MAX22007_CHANNEL_DATA_REG(ch), reg_val,
				      DAC_CH_DATA_MASK);
}

/**
 * @brief Read data from a DAC channel data register
 * @param dev - MAX22007 device descriptor.
 * @param ch - Channel number (0-3).
 * @param data - Pointer to store the 16-bit data read from the channel.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_read_channel_data(struct max22007_dev *dev, uint8_t ch,
			       uint16_t *data)
{
	if (!dev || !data || (ch > (MAX22007_NUM_CHANNELS - 1)))
		return -EINVAL;

	return max22007_reg_read(dev, MAX22007_CHANNEL_DATA_REG(ch), data);
}

/**
 * @brief Write to LDAC register to update DAC output
 * @param dev - MAX22007 device descriptor.
 * @param ch_mask - Channel mask.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_write_ldac(struct max22007_dev *dev, uint8_t ch_mask)
{
	uint16_t reg_val;

	if (!dev || (ch_mask > 0xF))
		return -EINVAL;

	reg_val = no_os_field_prep(MAX22007_LD_CTRL_MASK, ch_mask);

	return max22007_reg_write_msk(dev, MAX22007_CTRL_REG, reg_val,
				      MAX22007_LD_CTRL_MASK);
}

/**
 * @brief Write to Soft Reset Register
 * @param dev - MAX22007 device descriptor.
 * @param sw_clear - Data register reset control.
 * @param sw_reset - Software reset control.
 * @return 0 in case of success, negative error code otherwise.
*/
static int max22007_soft_reset(struct max22007_dev *dev, bool sw_clear,
			       bool sw_reset)
{
	uint16_t reg_val = 0;

	if (!dev)
		return -EINVAL;

	reg_val = no_os_field_prep(MAX22007_SW_CLR_MASK, sw_clear) |
		  no_os_field_prep(MAX22007_SW_RST_MASK, sw_reset);

	return max22007_reg_write(dev, MAX22007_SOFT_RESET_REG, reg_val);
}

/**
 * @brief MAX22007 descriptor initialization function.
 * @param device - MAX22007 device descriptor.
 * @param init_param - Initialization parameter containing data about the device
 * 		       descriptor to be initialized.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_init(struct max22007_dev **device,
		  struct max22007_init_param init_param)
{
	struct max22007_dev *dev;
	uint16_t reg_val;
	uint8_t ch;
	int ret;

	dev = (struct max22007_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	no_os_crc8_populate_lsb(max22007_crc8_table, MAX22007_CRC8_POLYNOMIAL);

	ret = no_os_spi_init(&dev->comm_desc, init_param.comm_param);
	if (ret)
		goto err_spi;

	ret = max22007_soft_reset(dev, true, true);
	if (ret)
		goto err_spi;

	ret = max22007_configure_crc(dev, init_param.crc_en);
	if (ret)
		goto err_spi;

	ret = max22007_reg_read(dev, MAX22007_REV_ID_REG, &reg_val);
	if (ret)
		goto err_spi;

	if ((reg_val != MAX22007_REV_ID_REV0) && (reg_val != MAX22007_REV_ID_REV1)) {
		pr_info("Invalid device ID: 0x%04X\n", reg_val);
		ret = -EINVAL;
		goto err_spi;
	}

	/* Configure device with initialization parameters */
	ret = max22007_set_reference(dev, init_param.ref_mode);
	if (ret)
		goto err_spi;

	for (ch = 0; ch < MAX22007_NUM_CHANNELS; ch++) {
		ret = max22007_set_channel_mode(dev, ch,
						init_param.channel_config[ch].channel_mode);
		if (ret)
			goto err_spi;

		ret = max22007_set_latch_mode(dev, ch,
					      init_param.channel_config[ch].latch_mode);
		if (ret)
			goto err_spi;

		ret = max22007_set_channel_power(dev, ch,
						 init_param.channel_config[ch].channel_power);
		if (ret)
			goto err_spi;
	}

	ret = max22007_set_timeout(dev, init_param.timeout_config.timeout_en,
				   init_param.timeout_config.timeout_sel,
				   init_param.timeout_config.timeout_cnfg);
	if (ret)
		goto err_spi;

	*device = dev;
	return 0;

err_spi:
	max22007_remove(dev);
	return ret;
}

/**
 * @brief Deallocates all the resources used at initialization.
 * @param dev - MAX22007 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22007_remove(struct max22007_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_spi_remove(dev->comm_desc);

	no_os_free(dev);

	return 0;
}


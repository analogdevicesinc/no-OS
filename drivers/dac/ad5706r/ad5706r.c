/***************************************************************************//**
 *   @file   ad5706r.c
 *   @brief  Implementation of AD5706R Driver.
 *   @author NAlteza (nathaniel.alteza@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ad5706r.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Transfer spi read/write messages.
 * @param dev - The device structure.
 * @param data - The transfer data structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5706r_transfer_with_crc(struct ad5706r_dev *dev,
				     struct ad5706r_transfer_data *data)
{
	int ret;
	int32_t addr;
	uint8_t cnt = 0;
	uint8_t crc_cnt = 0;
	uint8_t i, crc_seed;
	uint8_t crc_buf[4] = {0};
	uint8_t byte_cnt = 0;
	int inc;

	/* Initialize output buffer for SPI streaming. */
	uint8_t out_buff[512];
	uint8_t crc_cal[256];

	if (!dev || !data)
		return -EINVAL;

	/* Short instruction mode is not supported in Multi-Drop config. */
	if (dev->dev_addr > 0)
		data->spi_cfg->short_instr = 0;

	ret = ad5706r_set_device_spi(dev, data->spi_cfg);
	if (ret)
		return ret;

	inc = dev->spi_cfg.addr_asc ? 1 : -1;

	if (data->is_read)
		out_buff[cnt] = AD5706R_READ_BIT;
	else
		out_buff[cnt] = AD5706R_WRITE_BIT_LONG_INSTR;

	if (dev->spi_cfg.short_instr) {
		out_buff[cnt++] |= (uint8_t)(data->addr);
	} else {
		out_buff[cnt++] |= AD5706R_MD_ADDR(dev->dev_addr);
		out_buff[cnt++] = (uint8_t)(data->addr);
	}

	memcpy(crc_buf, out_buff, cnt + 1);

	for (i = 0; i < data->len; i++) {
		addr = data->addr + inc * i;
		if (addr > AD5706R_REG_ADDR_MAX)
			addr %= (AD5706R_REG_ADDR_MAX + 1);
		if (addr < 0)
			addr += (AD5706R_REG_ADDR_MAX + 1);

		if (data->is_read)
			crc_buf[cnt++] = 0x00;
		else
			crc_buf[cnt++] = data->data[i];

		if (i == 0)
			crc_seed = AD5706R_CRC_SEED;
		else
			crc_seed = addr;

		if ((addr >= AD5706R_MULTIBYTE_REG_START) &&
		    (addr <= AD5706R_MULTIBYTE_REG_END)) {
			i++;
			if (data->is_read)
				crc_buf[cnt++] = 0x00;
			else
				crc_buf[cnt++] = data->data[i];
		}

		memcpy(out_buff + byte_cnt, crc_buf, cnt);
		/* CRC byte */
		if (data->is_read && byte_cnt > 0) {
			out_buff[byte_cnt + cnt] = 0x00;
		} else {
			out_buff[byte_cnt + cnt] =
				no_os_crc8(dev->crc_table, crc_buf, cnt, crc_seed);
		}

		/* Expected CRC from device during SPI write mode. */
		if (!data->is_read) {
			crc_cal[crc_cnt] = out_buff[byte_cnt + cnt];
		}

		crc_cnt++;
		byte_cnt += cnt + 1;
		cnt = 0;
		memset(crc_buf, 0, sizeof(crc_buf));
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, out_buff, byte_cnt);
	if (ret)
		return ret;

	crc_cnt = 0;

	/* Check CRC. */
	for (i = 0; i < data->len; i++) {
		addr = data->addr + inc * i;
		if (addr > AD5706R_REG_ADDR_MAX)
			addr %= (AD5706R_REG_ADDR_MAX + 1);
		if (addr < 0)
			addr += (AD5706R_REG_ADDR_MAX + 1);

		if (data->is_read) {
			if (i == 0) {
				crc_buf[cnt] = AD5706R_READ_BIT;

				if (dev->spi_cfg.short_instr) {
					crc_buf[cnt++] |= (uint8_t)(data->addr);
				} else {
					crc_buf[cnt++] |= AD5706R_MD_ADDR(dev->dev_addr);
					crc_buf[cnt++] = (uint8_t)(data->addr);
				}
				byte_cnt = cnt;
				crc_seed = AD5706R_CRC_SEED;
			} else
				crc_seed = addr;

			crc_buf[cnt++] = out_buff[byte_cnt];
			data->data[i] = out_buff[byte_cnt];
			byte_cnt++;

			/* Check if current address is a 2-byte register. */
			if ((addr >= AD5706R_MULTIBYTE_REG_START) &&
			    (addr <= AD5706R_MULTIBYTE_REG_END)) {
				i++;
				crc_buf[cnt++] = out_buff[byte_cnt];
				data->data[i] = out_buff[byte_cnt];
				byte_cnt++;
			}
			crc_cal[crc_cnt] = no_os_crc8(dev->crc_table, crc_buf, cnt, crc_seed);
		} else {
			if (i == 0)
				byte_cnt = (dev->spi_cfg.short_instr) ? 1 : 2;

			if ((addr >= AD5706R_MULTIBYTE_REG_START) &&
			    (addr <= AD5706R_MULTIBYTE_REG_END)) {
				i++;
				byte_cnt++;
			}

			byte_cnt++;
		}

		if (crc_cal[crc_cnt] != out_buff[byte_cnt])
			return -EBADMSG;

		byte_cnt++;
		crc_cnt++;
		cnt = 0;
		memset(crc_buf, 0, sizeof(crc_buf));
	}

	return 0;
}

/**
 * @brief Transfer spi read/write messages.
 * @param dev - The device structure.
 * @param data - The transfer data structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5706r_transfer(struct ad5706r_dev *dev,
			    struct ad5706r_transfer_data *data)
{
	int ret;
	struct no_os_spi_msg msgs[2] = { 0 };
	uint8_t instr[2];

	if (!dev || !data)
		return -EINVAL;

	/* Short instruction mode is not supported in Multi-Drop config. */
	if (dev->dev_addr > 0)
		data->spi_cfg->short_instr = 0;

	ret = ad5706r_set_device_spi(dev, data->spi_cfg);
	if (ret)
		return ret;

	if (data->spi_cfg->short_instr) {
		instr[0] = data->is_read ? data->addr | AD5706R_READ_BIT :
			   (data->addr & AD5706R_ADDR_MASK);
	} else {
		instr[0] = data->is_read ? AD5706R_READ_BIT : 0;
		instr[0] = instr[0] | AD5706R_MD_ADDR(dev->dev_addr);

		instr[1] = data->addr;
	}

	msgs[0].tx_buff = instr;
	msgs[0].bytes_number = data->spi_cfg->short_instr ? 1 : 2;
	msgs[1].bytes_number = data->len;
	if (data->is_read)
		msgs[1].rx_buff = data->data;
	else
		msgs[1].tx_buff = data->data;

	return no_os_spi_transfer(dev->spi_desc, msgs, NO_OS_ARRAY_SIZE(msgs));
}

/**
 * @brief Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_spi_reg_read(struct ad5706r_dev *dev,
			 uint32_t reg_addr,
			 uint16_t *reg_data)
{
	int ret;
	uint32_t addr;
	uint8_t cnt = 0;
	uint8_t dindx = 0;
	uint8_t reg_len;
	uint8_t buf[5] = {0};
	uint8_t crc_buf[4] = {0};
	uint8_t crc_out = 0;
	uint8_t crc_cal = 0;

	if (!dev || !reg_data)
		return -EINVAL;

	/* Get the register length */
	reg_len = AD5706R_LEN(reg_addr);

	if (!dev->spi_cfg.addr_asc)
		addr = AD5706R_ADDR(reg_addr) + reg_len - 1;
	else
		addr = AD5706R_ADDR(reg_addr);

	/* Short instruction mode is not supported in Multi-drop config. */
	if (dev->spi_cfg.short_instr && !(dev->dev_addr > 0)) {
		buf[cnt++] = (uint8_t)(AD5706R_READ_BIT | addr);
	} else {
		buf[cnt++] = AD5706R_READ_BIT | AD5706R_MD_ADDR(dev->dev_addr);
		buf[cnt++] = (uint8_t)addr;
	}

	/* Save the data phase start index */
	dindx = cnt;

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_len;

	if (dev->crc_en) {
		memcpy(crc_buf, buf, cnt);

		/* CRC is computed with address + zero padding on the host device */
		buf[cnt] = no_os_crc8(dev->crc_table, buf, cnt,
				      AD5706R_CRC_SEED);

		/* For read transaction, crc is computed with received data */
		ret = no_os_spi_write_and_read(dev->spi_desc,
					       buf,
					       cnt + 1);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
		crc_out = buf[cnt];

		/* CRC is computed with the register read data on the slave device */
		memcpy(&crc_buf[dindx], &buf[dindx], reg_len);
		crc_cal = no_os_crc8(dev->crc_table, crc_buf, cnt,
				     AD5706R_CRC_SEED);

		if (crc_out != crc_cal)
			return -EBADMSG;
	} else {
		ret = no_os_spi_write_and_read(dev->spi_desc, buf, cnt);
		if (ret)
			return ret;
	}

	if (reg_len == 1)
		*reg_data = buf[dindx];
	else
		*reg_data = no_os_get_unaligned_be16(&buf[dindx]);

	return 0;
}

/**
 * @brief Read multiple register's values.
 * @param dev - The device structure.
 * @param start_addr - Starting register address to read from.
 * @param count - Number of bytes to read.
 * @param buff - Buffer to store data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_spi_reg_read_multiple(struct ad5706r_dev *dev,
				  uint32_t start_addr,
				  uint8_t count,
				  uint8_t *buff)
{
	struct ad5706r_transfer_data msg = { 0 };
	struct ad5706r_device_spi_settings multi_cfg = { 0 };
	uint8_t reg_len;

	if (!dev || !buff ||
	    (AD5706R_ADDR_MD(start_addr) > AD5706R_REG_ADDR_MAX))
		return -EINVAL;

	multi_cfg.single_instr = 0; /* Sets the streaming mode. */
	multi_cfg.stream_length_keep_value = 1;
	/* Prevents the stream length value from automatically resetting to zero. */
	multi_cfg.short_instr = dev->spi_cfg.short_instr;
	multi_cfg.stream_mode_length = count;
	multi_cfg.addr_asc = dev->spi_cfg.addr_asc;

	msg.is_read = 1;
	msg.addr = (uint16_t)AD5706R_ADDR(start_addr);
	msg.data = buff;
	msg.len = count;
	msg.spi_cfg = &multi_cfg;

	/* Get the register length */
	reg_len = AD5706R_LEN(start_addr);

	/** Offset register address when in descending address mode
	 * 	with respect to register length. */
	if (!multi_cfg.addr_asc)
		msg.addr += reg_len - 1;

	if (dev->crc_en)
		return ad5706r_transfer_with_crc(dev, &msg);
	else
		return ad5706r_transfer(dev, &msg);
}

/**
 * @brief Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_spi_reg_write(struct ad5706r_dev *dev,
			  uint32_t reg_addr,
			  uint16_t reg_data)
{
	int ret;
	uint8_t reg_len;
	uint32_t addr;
	uint8_t cnt = 0;
	uint8_t buf[5] = {0};
	uint8_t crc_cal = 0;

	if (!dev)
		return -EINVAL;

	/* Get the register length */
	reg_len = AD5706R_LEN(reg_addr);

	if (!dev->spi_cfg.addr_asc)
		addr = AD5706R_ADDR(reg_addr) + reg_len - 1;
	else
		addr = AD5706R_ADDR(reg_addr);

	/* Short instruction mode is not supported in Multi-drop config. */
	if (dev->spi_cfg.short_instr && !(dev->dev_addr > 0)) {
		buf[cnt++] = (uint8_t)(addr & AD5706R_ADDR_MASK);
	} else {
		buf[cnt++] = AD5706R_MD_ADDR(dev->dev_addr);
		buf[cnt++] = (uint8_t)addr;
	}

	no_os_memswap64(&reg_data, reg_len, reg_len);
	memcpy(&buf[cnt], &reg_data, reg_len);

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_len;

	if (dev->crc_en) {
		/* For write transaction,  CRC is computed on
		 * address + register data both on host and slave device. */
		crc_cal = no_os_crc8(dev->crc_table, buf, cnt,
				     AD5706R_CRC_SEED);
		buf[cnt] = crc_cal;

		ret = no_os_spi_write_and_read(dev->spi_desc, buf, cnt + 1);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		if (crc_cal != buf[cnt])
			return -EBADMSG;
	} else {
		ret = no_os_spi_write_and_read(dev->spi_desc, buf, cnt);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Write multiple register's values.
 * @param dev - The device structure.
 * @param start_addr - Starting register address to write into.
 * @param count - Number of bytes to write.
 * @param buff - Buffer to write data from.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_spi_reg_write_multiple(struct ad5706r_dev *dev,
				   uint32_t start_addr,
				   uint8_t count,
				   uint8_t *buff)
{
	struct ad5706r_transfer_data msg = { 0 };
	struct ad5706r_device_spi_settings multi_cfg = { 0 };
	uint8_t reg_len;

	if (!dev || !buff || (AD5706R_ADDR_MD(start_addr) > AD5706R_REG_ADDR_MAX))
		return -EINVAL;

	multi_cfg.single_instr = 0; /* Sets the streaming mode. */
	multi_cfg.stream_length_keep_value = 1;
	/* Prevents the stream length value from automatically resetting to zero. */
	multi_cfg.short_instr = dev->spi_cfg.short_instr;
	multi_cfg.stream_mode_length = count;
	multi_cfg.addr_asc = dev->spi_cfg.addr_asc;

	msg.is_read = 0;
	msg.addr = (uint16_t)AD5706R_ADDR(start_addr);
	msg.data = buff;
	msg.len = count;
	msg.spi_cfg = &multi_cfg;

	/* Get the register length */
	reg_len = AD5706R_LEN(start_addr);

	/** Offset register address when in descending address mode
	 * 	with respect to register length. */
	if (!multi_cfg.addr_asc)
		msg.addr += reg_len - 1;

	if (dev->crc_en)
		return ad5706r_transfer_with_crc(dev, &msg);
	else
		return ad5706r_transfer(dev, &msg);
}

/**
 * @brief SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_spi_write_mask(struct ad5706r_dev *dev,
			   uint32_t reg_addr,
			   uint32_t mask,
			   uint16_t data)
{
	int ret;
	uint8_t reg_len;
	uint16_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = ad5706r_spi_reg_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;

	if (dev->spi_cfg.addr_asc) {
		reg_len = AD5706R_LEN(reg_addr);
		no_os_memswap64(&mask, reg_len, reg_len);
	}

	reg_data = (reg_data & ~mask) | no_os_field_prep(mask, data);

	return ad5706r_spi_reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Set reference configuration.
 * @param dev - The device structure.
 * @param reference_selector - The reference to be selected.
 * 			Accepted values: AD5706R_EXTERNAL_VREF_PIN_INPUT (default)
 * 					 AD5706R_INTERNAL_VREF_PIN_2P5V
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_reference(struct ad5706r_dev *dev,
			  enum ad5706r_vref_select reference_selector)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_BANDGAP_CONTROL,
				     AD5706R_BANDGAP_CONTROL_EN,
				     reference_selector);
	if (ret)
		return ret;

	dev->vref_enable = reference_selector;

	return 0;
}

/**
 * @brief Set output operating mode of a channel.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param op_mode - Output operating mode to be selected.
 * 				Accepted values: AD5706R_SHUTDOWN_SW (default)
 * 						 AD5706R_SHUTDOWN_SW_W_GND
 * 						 AD5706R_NORMAL_SW
 * 						 AD5706R_SHUTDOWN_HW
 * 						 AD5706R_SHUTDOWN_HW_W_GND
 * 						 AD5706R_NORMAL_HW
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_operating_mode(struct ad5706r_dev *dev,
			       enum ad5706r_channels ch,
			       enum ad5706r_output_op_mode op_mode)
{
	int ret;
	bool gnd_switch_update = false;
	bool hw_sw_shdn_update = false;
	uint16_t data;

	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	/* Only SW operating modes are supported by the device.*/
	switch (op_mode) {
	case AD5706R_NORMAL_SW:
		hw_sw_shdn_update = true;
		break;
	case AD5706R_SHUTDOWN_SW:
		gnd_switch_update = true;
		break;
	case AD5706R_SHUTDOWN_SW_W_GND:
		gnd_switch_update = true;
		break;
	case AD5706R_NORMAL_HW:
		hw_sw_shdn_update = true;
		break;
	case AD5706R_SHUTDOWN_HW:
		gnd_switch_update = true;
		hw_sw_shdn_update = true;
		break;
	case AD5706R_SHUTDOWN_HW_W_GND:
		gnd_switch_update = true;
		hw_sw_shdn_update = true;
		break;
	default:
		break;
	}

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_OUT_OPERATING_MODE,
				     NO_OS_BIT(ch),
				     0);
	if (ret)
		return ret;

	if (gnd_switch_update) {
		data = (op_mode == AD5706R_SHUTDOWN_SW_W_GND ||
			op_mode == AD5706R_SHUTDOWN_HW_W_GND) ? 1 : 0;
		ret = ad5706r_spi_write_mask(dev,
					     AD5706R_REG_OUT_SWITCH_EN,
					     NO_OS_BIT(ch),
					     data);
		if (ret)
			return ret;
	}

	if (hw_sw_shdn_update) {
		data = (op_mode == AD5706R_SHUTDOWN_HW ||
			op_mode == AD5706R_SHUTDOWN_HW_W_GND ||
			op_mode == AD5706R_NORMAL_HW) ? 1 : 0;
		ret = ad5706r_spi_write_mask(dev,
					     AD5706R_REG_HW_SHUTDOWN_EN,
					     NO_OS_BIT(ch),
					     data);
		if (ret)
			return ret;
	}

	if (op_mode == AD5706R_NORMAL_SW ||
	    op_mode == AD5706R_SHUTDOWN_HW ||
	    op_mode == AD5706R_SHUTDOWN_HW_W_GND ||
	    op_mode == AD5706R_NORMAL_HW) {
		ret = ad5706r_spi_write_mask(dev,
					     AD5706R_REG_OUT_OPERATING_MODE,
					     NO_OS_BIT(ch),
					     1);
		if (ret)
			return ret;
	}

	dev->op_mode[ch] = op_mode;

	return 0;
}

/**
 * @brief Set output range for a channel.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param range_sel - Output range to be selected.
 * 				Accepted values: AD5706R_50mA (default)
 * 						 AD5706R_150mA
 * 						 AD5706R_200mA
 * 						 AD5706R_300mA
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_ch_output_range(struct ad5706r_dev *dev,
				enum ad5706r_channels ch,
				enum ad5706r_output_range range_sel)
{
	int ret;

	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_OUT_RANGE_CH(ch),
				     AD5706R_OUT_RANGE_CHn_MASK,
				     range_sel);
	if (ret)
		return ret;

	dev->range[ch] = range_sel;

	return 0;
}

/**
 * @brief Set the function generator enable configuration.
 * @param dev - The device structure.
 * @param en_mask - Mask to enable function generator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_func_en(struct ad5706r_dev *dev, uint16_t en_mask)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_FUNC_EN,
				     AD5706R_FUNC_EN_MASK,
				     en_mask);
	if (ret)
		return ret;

	dev->ldac_cfg.func_en_mask = en_mask;

	return 0;
}

/**
 * @brief Set function generator mode for a channel.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param func_mode_sel - Function generator mode to be selected.
 * 				Accepted values: AD5706R_TOGGLE (default)
 * 						 AD5706R_DITHER
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_func_mode_select(struct ad5706r_dev *dev,
			     enum ad5706r_channels ch,
			     enum ad5706r_function_mode func_mode_sel)
{
	int ret;

	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_FUNC_MODE_SEL_CH(ch),
				     AD5706R_FUNC_MODE_SEL_CHn,
				     func_mode_sel);
	if (ret)
		return ret;

	dev->ldac_cfg.func_mode[ch] = func_mode_sel;

	return 0;
}

/**
 * @brief Set period for dither function generator.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param period - Dither period to be selected.
 * 				Accepted values: AD5706R_SAMPLES_4 (default)
 * 						 AD5706R_SAMPLES_8
 * 						 AD5706R_SAMPLES_16
 * 						 AD5706R_SAMPLES_32
 * 						 AD5706R_SAMPLES_64
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_dither_period(struct ad5706r_dev *dev,
			      enum ad5706r_channels ch,
			      enum ad5706r_dither_period period)
{
	int ret;

	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	ret = ad5706r_spi_reg_write(dev,
				    AD5706R_REG_FUNC_DITHER_PERIOD_CH(ch),
				    period);
	if (ret)
		return ret;

	dev->ldac_cfg.dither_period[ch] = period;

	return 0;
}

/**
 * @brief Set phase for dither function generator.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param phase - Dither phase to be selected.
 * 				Accepted values: AD5706R_DEGREES_0 (default)
 * 						 AD5706R_DEGREES_90
 * 						 AD5706R_DEGREES_180
 * 						 AD5706R_DEGREES_270
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_dither_phase(struct ad5706r_dev *dev,
			     enum ad5706r_channels ch,
			     enum ad5706r_dither_phase phase)
{
	int ret;

	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	ret = ad5706r_spi_reg_write(dev,
				    AD5706R_REG_FUNC_DITHER_PHASE_CH(ch),
				    phase);
	if (ret)
		return ret;

	dev->ldac_cfg.dither_phase[ch] = phase;

	return 0;
}

/**
 * @brief Set the hardware and software ldac configuration.
 * @param dev - The device structure.
 * @param mask_hw_sw_ldac - The array contains HW/SW LDAC channel masks.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_hw_sw_ldac(struct ad5706r_dev *dev, uint16_t mask_hw_sw_ldac)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_LDAC_HW_SW,
				     AD5706R_LDAC_HW_SW_MASK,
				     mask_hw_sw_ldac);
	if (ret)
		return ret;

	dev->ldac_cfg.ldac_hw_sw_mask = mask_hw_sw_ldac;

	return 0;
}

/**
 * @brief Set sync/async ldac configuration.
 * @param dev - The device structure.
 * @param mask_ldac_sync_async - The array contains SYNC/ASYNC mode channel masks.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_sync_async_ldac(struct ad5706r_dev *dev,
				uint16_t mask_ldac_sync_async)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_LDAC_SYNC_ASYNC,
				     AD5706R_REG_LDAC_SYNC_ASYNC_MASK,
				     mask_ldac_sync_async);
	if (ret)
		return ret;

	dev->ldac_cfg.ldac_sync_async_mask = mask_ldac_sync_async;

	return 0;
}

/**
 * @brief Set ldac config for single channel.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param dac_mode - DAC Controller Mode to be configured for selected channel.
 * 				Accepted values: AD5706R_DIRECT_WRITE_REG (default)
 * 						AD5706R_SW_LDAC
 * 						AD5706R_HW_LDAC
 * 						AD5706R_SW_TOGGLE
 * 						AD5706R_HW_TOGGLE
 * 						AD5706R_SW_DITHER
 * 						AD5706R_HW_DITHER
 * @param ldac_settings - LDAC mode config Structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_ldac_config(struct ad5706r_dev *dev,
			    enum ad5706r_channels ch,
			    enum ad5706r_dac_mode dac_mode,
			    struct ad5706r_ldac_settings *ldac_settings)
{
	int ret;
	bool func_mode_update = false;
	bool sync_async_update = false;
	bool hw_sw_ldac_update = true;
	uint16_t data, masked_data;

	if (!dev || ch >= AD5706R_CH_MAX || !ldac_settings)
		return -EINVAL;

	switch (dac_mode) {
	case AD5706R_DIRECT_WRITE_REG:
		sync_async_update = true;
		hw_sw_ldac_update = false;
		break;
	case AD5706R_SW_LDAC:
	case AD5706R_HW_LDAC:
		sync_async_update = true;
		break;
	case AD5706R_SW_TOGGLE:
	case AD5706R_HW_TOGGLE:
	case AD5706R_SW_DITHER:
	case AD5706R_HW_DITHER:
		func_mode_update = true;
		break;
	default:
		hw_sw_ldac_update = false;
		break;
	}

	if (func_mode_update) {
		/* Select function generator mode*/
		masked_data = dev->ldac_cfg.func_en_mask;
		masked_data &= ~NO_OS_BIT(ch);
		data = (dac_mode == AD5706R_SW_DITHER ||
			dac_mode == AD5706R_HW_DITHER) ? 1 : 0;

		ret = ad5706r_func_en(dev, masked_data);
		if (ret)
			return ret;

		ret = ad5706r_func_mode_select(dev, ch, data);
		if (ret)
			return ret;
	}

	/* Enabling function generator */
	data = (dac_mode == AD5706R_SW_TOGGLE ||
		dac_mode == AD5706R_HW_TOGGLE ||
		dac_mode == AD5706R_SW_DITHER ||
		dac_mode == AD5706R_HW_DITHER) ? 1 : 0;
	masked_data = dev->ldac_cfg.func_en_mask;
	masked_data &= ~NO_OS_BIT(ch);
	masked_data |= no_os_field_prep(NO_OS_BIT(ch), data);

	ret = ad5706r_func_en(dev, masked_data);
	if (ret)
		return ret;

	if (sync_async_update) {
		/* Set LDAC mode as ASYNC or SYNC*/
		data = (dac_mode != AD5706R_DIRECT_WRITE_REG) ? 1 : 0;
		masked_data = dev->ldac_cfg.ldac_sync_async_mask;
		masked_data &= ~NO_OS_BIT(ch);
		masked_data |= no_os_field_prep(NO_OS_BIT(ch), data);

		ret = ad5706r_set_sync_async_ldac(dev, masked_data);
		if (ret)
			return ret;
	}

	if (hw_sw_ldac_update) {
		/* Set channel as HW or SW LDAC triggered*/
		masked_data = dev->ldac_cfg.ldac_hw_sw_mask;

		if (dac_mode == AD5706R_HW_LDAC ||
		    dac_mode == AD5706R_HW_TOGGLE ||
		    dac_mode == AD5706R_HW_DITHER) {

			/* HW LDAC */
			masked_data &= ~NO_OS_BIT(ch);

			ret = ad5706r_set_hw_sw_ldac(dev, masked_data);
			if (ret)
				return ret;

			ret = ad5706r_set_edge_trigger(dev, ch,
						       ldac_settings->edge_trig[ch]);
			if (ret)
				return ret;
		} else {
			/* SW LDAC */
			data = NO_OS_BIT(ch);
			masked_data &= ~NO_OS_BIT(ch);
			masked_data |= data;

			ret = ad5706r_set_hw_sw_ldac(dev, masked_data);
			if (ret)
				return ret;
		}
	}

	if (dac_mode == AD5706R_SW_DITHER || dac_mode == AD5706R_HW_DITHER) {
		ret = ad5706r_set_dither_phase(dev, ch,
					       ldac_settings->dither_phase[ch]);
		if (ret)
			return ret;

		ret = ad5706r_set_dither_period(dev, ch,
						ldac_settings->dither_period[ch]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Trigger Multi DAC SW LDAC.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_multi_dac_sw_ldac_trigger(struct ad5706r_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad5706r_spi_write_mask(dev,
				      AD5706R_REG_MULTI_DAC_SW_LDAC,
				      AD5706R_MULTI_DAC_SW_LDAC_TRIG,
				      AD5706R_MULTI_DAC_SW_LDAC_TRIG);
}

/**
 * @brief Set ldac config for multiple channels.
 * @param dev - The device structure.
 * @param ch_mask - Selected channels mask.
 * @param dac_mode - DAC Controller Mode to be configured for selected channel.
 * 				Accepted values: AD5706R_DIRECT_WRITE_REG (default)
 * 						AD5706R_SW_LDAC
 * 						AD5706R_HW_LDAC
 * 						AD5706R_SW_TOGGLE
 * 						AD5706R_HW_TOGGLE
 * 						AD5706R_SW_DITHER
 * 						AD5706R_HW_DITHER
 * @param ldac_settings - LDAC mode config Structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_multi_ldac_config(struct ad5706r_dev *dev,
				  uint16_t ch_mask,
				  enum ad5706r_dac_mode dac_mode,
				  struct ad5706r_ldac_settings *ldac_settings)
{
	int ret, ch;
	bool func_mode_update = false;
	bool sync_async_update = false;
	bool hw_sw_ldac_update = true;
	uint16_t data, masked_data;

	if (!dev)
		return -EINVAL;

	switch (dac_mode) {
	case AD5706R_DIRECT_WRITE_REG:
		sync_async_update = true;
		hw_sw_ldac_update = false;
		break;
	case AD5706R_SW_LDAC:
	case AD5706R_HW_LDAC:
		sync_async_update = true;
		break;
	case AD5706R_SW_TOGGLE:
	case AD5706R_HW_TOGGLE:
	case AD5706R_SW_DITHER:
	case AD5706R_HW_DITHER:
		func_mode_update = true;
		break;
	default:
		hw_sw_ldac_update = false;
		break;
	}

	/* Select channels for Multi DAC update*/
	masked_data = ch_mask;
	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_MULTI_DAC_CH_SEL,
				     AD5706R_MULTI_DAC_CH_SEL_MASK,
				     masked_data);
	if (ret)
		return ret;

	if (func_mode_update) {
		/* Select function generator mode*/
		data = (dac_mode == AD5706R_SW_DITHER ||
			dac_mode == AD5706R_HW_DITHER) ? 1 : 0;

		masked_data = ch_mask;

		ret = ad5706r_func_en(dev, 0);
		if (ret)
			return ret;

		for (ch = 0; ch < AD5706R_CH_MAX; ch++) {
			if ((ch_mask >> ch) & 1) {
				ret = ad5706r_func_mode_select(dev, ch, data);
				if (ret)
					return ret;
			}
		}
	}

	/* Enabling function generator */
	data = (dac_mode == AD5706R_SW_TOGGLE ||
		dac_mode == AD5706R_HW_TOGGLE ||
		dac_mode == AD5706R_SW_DITHER ||
		dac_mode == AD5706R_HW_DITHER) ? AD5706R_FUNC_EN_MASK : 0;
	data &= ch_mask;
	masked_data = dev->ldac_cfg.func_en_mask;
	masked_data &= ~ch_mask;
	masked_data |= data;

	ret = ad5706r_func_en(dev, masked_data);
	if (ret)
		return ret;

	if (sync_async_update) {
		/* Set Load DAC mode as ASYNC or SYNC*/
		data = (dac_mode != AD5706R_DIRECT_WRITE_REG) ?
		       AD5706R_REG_LDAC_SYNC_ASYNC_MASK : 0;
		data &= ch_mask;
		masked_data = dev->ldac_cfg.ldac_sync_async_mask;
		masked_data &= ~ch_mask;
		masked_data |= data;

		ret = ad5706r_set_sync_async_ldac(dev, masked_data);
		if (ret)
			return ret;
	}

	if (hw_sw_ldac_update) {
		/* Set channel as HW or SW LDAC triggered*/
		masked_data = dev->ldac_cfg.ldac_hw_sw_mask;
		masked_data &= ~AD5706R_LDAC_HW_SW_MASK;

		if (dac_mode == AD5706R_HW_LDAC ||
		    dac_mode == AD5706R_HW_TOGGLE ||
		    dac_mode == AD5706R_HW_DITHER) {

			/* HW LDAC */
			data = 0;
			data &= ch_mask;
			masked_data |= data;

			ret = ad5706r_set_hw_sw_ldac(dev, masked_data);
			if (ret)
				return ret;
		} else {
			/* SW LDAC */
			data = AD5706R_LDAC_HW_SW_MASK;
			data &= ch_mask;
			masked_data |= data;

			ret = ad5706r_set_hw_sw_ldac(dev, masked_data);
			if (ret)
				return ret;

			for (ch = 0; ch < AD5706R_CH_MAX; ch++) {
				if ((ch_mask >> ch) & 1) {
					ret = ad5706r_set_edge_trigger(dev, ch,
								       ldac_settings->edge_trig[ch]);
					if (ret)
						return ret;
				}
			}
		}
	}

	if (dac_mode == AD5706R_SW_DITHER || dac_mode == AD5706R_HW_DITHER) {
		for (ch = 0; ch < AD5706R_CH_MAX; ch++) {
			if ((ch_mask >> ch) & 1) {
				ret = ad5706r_set_dither_phase(dev, ch,
							       ldac_settings->dither_phase[ch]);
				if (ret)
					return ret;

				ret = ad5706r_set_dither_period(dev, ch,
								ldac_settings->dither_period[ch]);
				if (ret)
					return ret;
			}
		}
	}

	dev->ldac_cfg.multi_dac_ch_mask = ch_mask;

	return 0;
}

/**
 * @brief Set dac value on DAC input register A.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param dac_value - value that will be set in DAC input register A.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_dac_a_value(struct ad5706r_dev *dev,
			    enum ad5706r_channels ch,
			    uint16_t dac_value)
{
	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	return ad5706r_spi_reg_write(dev,
				     AD5706R_REG_DAC_INPUT_A_CH(ch),
				     dac_value);
}

/**
 * @brief Set dac value on DAC input register B.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param dac_value - value that will be set in DAC input register B.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_dac_b_value(struct ad5706r_dev *dev,
			    enum ad5706r_channels ch,
			    uint16_t dac_value)
{
	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	/* During dither mode 2 MSB bits are unused but writable */
	return ad5706r_spi_reg_write(dev,
				     AD5706R_REG_FUNC_DAC_INPUT_B_CH(ch),
				     dac_value);
}

/**
 * @brief Set dac value on Multi DAC input register A.
 * @param dev - The device structure.
 * @param dac_value - value that will be set in MULTI DAC input register A.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_multi_dac_a_value(struct ad5706r_dev *dev,
				  uint16_t dac_value)
{
	if (!dev)
		return -EINVAL;

	return ad5706r_spi_reg_write(dev,
				     AD5706R_REG_MULTI_DAC_INPUT_A,
				     dac_value);
}

/**
 * @brief Set the multiplexer output mode.
 * @param dev - The device structure.
 * @param mux_output_mode_sel - Select mux output state.
 * 				Accepted values: AD5706R_HIGH_Z (default)
 * 						 AD5706R_BYPASS
 * 						 AD5706R_BUFFER
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_mux_out_mode(struct ad5706r_dev *dev,
			     enum ad5706r_mux_out_state mux_output_mode_sel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	switch (mux_output_mode_sel) {
	case AD5706R_HIGH_Z:
		ret = ad5706r_spi_write_mask(dev, AD5706R_REG_MUX_OUT_SEL,
					     AD5706R_MUX_OUT_EN, 0);
		if (ret)
			return ret;

		break;
	case AD5706R_BYPASS:
		ret = ad5706r_spi_write_mask(dev, AD5706R_REG_MUX_OUT_SEL,
					     AD5706R_MUX_OUT_EN, 1);
		if (ret)
			return ret;

		ret = ad5706r_spi_write_mask(dev, AD5706R_REG_MUX_OUT_CONTROL,
					     AD5706R_MUX_OUT_BUFFER_EN, 0);
		if (ret)
			return ret;

		break;
	case AD5706R_BUFFER:
		ret = ad5706r_spi_write_mask(dev, AD5706R_REG_MUX_OUT_SEL,
					     AD5706R_MUX_OUT_EN, 1);
		if (ret)
			return ret;

		ret = ad5706r_spi_write_mask(dev, AD5706R_REG_MUX_OUT_CONTROL,
					     AD5706R_MUX_OUT_BUFFER_EN, 1);
		if (ret)
			return ret;

		break;
	default:
		ret = ad5706r_spi_write_mask(dev, AD5706R_REG_MUX_OUT_SEL,
					     AD5706R_MUX_OUT_EN, 0);
		if (ret)
			return ret;

		break;
	}

	dev->mux_out_mode = mux_output_mode_sel;

	return 0;
}

/**
 * @brief Set the multiplexer output select register.
 * @param dev - The device structure.
 * @param mux_output_sel - Select signal to be monitored on the mux_out pin.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_mux_out_select(struct ad5706r_dev *dev,
			       enum ad5706r_mux_out_select mux_output_sel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev, AD5706R_REG_MUX_OUT_SEL,
				     AD5706R_MUX_PARAM_SEL_MASK,
				     mux_output_sel);
	if (ret)
		return ret;

	dev->mux_out_sel = mux_output_sel;

	return 0;
}

/**
 * @brief Trigger the LDAC using hardware.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_hw_ldac_trigger(struct ad5706r_dev *dev)
{
	int ret;

	if (!dev || !dev->gpio_ldac_tgp)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_ldac_tgp, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_udelay(AD5706R_LDAC_PULSE_US);

	return no_os_gpio_set_value(dev->gpio_ldac_tgp, NO_OS_GPIO_LOW);
}

/**
 * @brief Set LDAC/TGP/DCLK edge trigger.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param trig_edge - Edge trigger to be selected.
 * 		      Accepted values: AD5706R_RISING_EDGE_TRIG (default)
 * 				       AD5706R_FALLING_EDGE_TRIG
 * 				       AD5706R_ANY_EDGE_TRIG
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_edge_trigger(struct ad5706r_dev *dev,
			     enum ad5706r_channels ch,
			     enum ad5706r_ldac_edge_trig trig_edge)
{
	int ret;

	if (!dev || ch >= AD5706R_CH_MAX)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_LDAC_EDGE_SEL_CH(ch),
				     AD5706R_LDAC_EDGE_SEL_CH_MASK,
				     trig_edge);
	if (ret)
		return ret;

	dev->ldac_cfg.edge_trig[ch] = trig_edge;

	return 0;
}

/**
 * @brief Trigger the LDAC using software.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_sw_ldac_trigger(struct ad5706r_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad5706r_spi_write_mask(dev,
				      AD5706R_REG_DAC_SW_LDAC,
				      AD5706R_DAC_SW_LDAC_MASK,
				      dev->ldac_cfg.ldac_hw_sw_mask);
}

/**
 * @brief Device configuration reset.
 * @param dev - The device structure.
 * @param sw_reset - If true, soft reset, otherwise, hard reset of device.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5706r_cfg_reset(struct ad5706r_dev *dev, bool sw_reset)
{
	bool tmp = false;
	no_os_mdelay(1);

	/* For SW reset, address ascension mode is retained. */
	if (sw_reset)
		tmp = dev->spi_cfg.addr_asc;

	memset(&dev->spi_cfg, 0, sizeof(dev->spi_cfg));
	dev->spi_cfg.addr_asc = tmp;
	dev->crc_en = 0;
	dev->vref_enable = AD5706R_EXTERNAL_VREF_PIN_INPUT;

	memset(dev->op_mode, AD5706R_SHUTDOWN_SW, sizeof(dev->op_mode));
	memset(&dev->ldac_cfg, 0, sizeof(dev->ldac_cfg));
	memset(dev->range, 0, sizeof(dev->range));
	dev->mux_out_sel = 0;

	return 0;
}

/**
 * @brief Perform soft reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_sw_reset(struct ad5706r_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_INTERFACE_CONFIG_A,
				     AD5706R_INT_CONFIG_A_SW_RESET_MASK,
				     AD5706R_INT_CONFIG_A_SW_RESET_MASK);
	if (ret)
		return ret;

	/* Default interface configuration after reset */
	return ad5706r_cfg_reset(dev, true);
}

/**
 * @brief Perform hard reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_hw_reset(struct ad5706r_dev *dev)
{
	int ret;

	if (!dev || !dev->gpio_reset)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1);

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* Default interface configuration after reset */
	return ad5706r_cfg_reset(dev, false);
}

/**
 * @brief Perform a write operation followed by a read
 *        operation from the scratch pad register.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5706r_check_scratch_pad(struct ad5706r_dev *dev)
{
	int ret;
	uint16_t val;

	ret = ad5706r_spi_reg_write(dev, AD5706R_REG_SCRATCH_PAD,
				    AD5706R_SCRATCH_PAD_TEST_VAL);
	if (ret)
		return ret;

	ret = ad5706r_spi_reg_read(dev, AD5706R_REG_SCRATCH_PAD, &val);
	if (ret)
		return ret;

	if (val != AD5706R_SCRATCH_PAD_TEST_VAL)
		return -EINVAL;

	return 0;
}

/**
 * @brief Configures the device.
 * @param dev - The device structure to be initialized.
 * @param init_param - Configuration parameters for the device.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_device_config(struct ad5706r_dev *dev,
			  struct ad5706r_init_param *init_param)
{
	int ret;
	uint8_t ch;

	if (!dev || !init_param)
		return -EINVAL;

	ret = ad5706r_set_reference(dev, init_param->vref_enable);
	if (ret)
		return ret;

	/* LDAC Function Configurations */
	for (ch = 0; ch < AD5706R_NUM_CH; ch++) {
		ret = ad5706r_set_operating_mode(dev, ch, init_param->op_mode[ch]);
		if (ret)
			return ret;

		ret = ad5706r_set_ch_output_range(dev, ch, init_param->range[ch]);
		if (ret)
			return ret;

		ret = ad5706r_set_ldac_config(dev, ch,
					      init_param->dac_mode[ch],
					      &init_param->ldac_cfg);
		if (ret)
			return ret;
	}

	if (init_param->crc_en) {
		ret = ad5706r_crc_enable(dev, init_param->crc_en);
		if (ret)
			return ret;
	}

	ret = ad5706r_set_mux_out_mode(dev, init_param->mux_out_mode);
	if (ret)
		return ret;

	ret = ad5706r_set_mux_out_select(dev, init_param->mux_out_sel);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Set device spi settings.
 * @param dev - The device structure.
 * @param spi_settings - The structure that contains the device spi parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_set_device_spi(struct ad5706r_dev *dev,
			   struct ad5706r_device_spi_settings *spi_settings)
{
	int ret;

	if (!dev || !spi_settings)
		return -EINVAL;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_INTERFACE_CONFIG_A,
				     AD5706R_INT_CONFIG_A_ADDR_ASC_MASK,
				     spi_settings->addr_asc);
	if (ret)
		return ret;

	dev->spi_cfg.addr_asc = spi_settings->addr_asc;

	/* Short instruction mode is not supported in Multi-drop config. */
	if (spi_settings->short_instr && dev->dev_addr > 0)
		spi_settings->short_instr = false;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_INTERFACE_CONFIG_B,
				     AD5706R_INT_CONFIG_B_SHORT_INSTR_MASK,
				     spi_settings->short_instr);
	if (ret)
		return ret;

	dev->spi_cfg.short_instr = spi_settings->short_instr;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_INTERFACE_CONFIG_B,
				     AD5706R_INT_CONFIG_B_SINGLE_INSTR_MASK,
				     spi_settings->single_instr);
	if (ret)
		return ret;

	dev->spi_cfg.single_instr = spi_settings->single_instr;

	ret = ad5706r_spi_write_mask(dev,
				     AD5706R_REG_TRANSFER_CONFIG,
				     AD5706R_TRANSFER_CONFIG_MASK,
				     spi_settings->stream_length_keep_value);

	if (ret)
		return ret;

	dev->spi_cfg.stream_length_keep_value =
		spi_settings->stream_length_keep_value;

	ret = ad5706r_spi_reg_write(dev,
				    AD5706R_REG_STREAM_MODE,
				    spi_settings->stream_mode_length);
	if (ret)
		return ret;

	dev->spi_cfg.stream_mode_length = spi_settings->stream_mode_length;

	return 0;
}

/**
 * @brief Configures the CRC selection bit .
 * @param dev - The device structure.
 * @param crc_enable - boolean selection for crc bit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_crc_enable(struct ad5706r_dev *dev, bool crc_enable)
{
	int ret;
	uint16_t value;

	if (!dev)
		return -EINVAL;

	value = crc_enable ? AD5706R_CRC_ENABLE_VALUE : AD5706R_CRC_DISABLE_VALUE;
	ret = ad5706r_spi_write_mask(dev, AD5706R_REG_INTERFACE_CONFIG_C,
				     AD5706R_CRC_ENABLE_MASK, value);
	if (ret)
		return ret;

	dev->crc_en = crc_enable;

	if (dev->crc_en)
		no_os_crc8_populate_msb(dev->crc_table, AD5706R_CRC_POLY);

	return 0;
}

/**
 * @brief Set SHDN pin state.
 * @param dev - The device structure.
 * @param shdn_state - Boolean selection for SHDN state.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_shdn_trigger(struct ad5706r_dev *dev, bool shdn_state)
{
	if (!dev || !dev->gpio_shdn)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_shdn, shdn_state);
}

/**
 * @brief Deallocate memory for the GPIOs assigned.
 * @param dev - Device driver handler.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5706r_remove_gpio(struct ad5706r_dev *dev)
{
	int ret;

	if (dev->gpio_shdn) {
		ret = no_os_gpio_remove(dev->gpio_shdn);
		if (ret)
			return ret;
	}

	if (dev->gpio_ldac_tgp) {
		ret = no_os_gpio_remove(dev->gpio_ldac_tgp);
		if (ret)
			return ret;
	}

	if (dev->gpio_reset) {
		ret = no_os_gpio_remove(dev->gpio_reset);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_init(struct ad5706r_dev **device,
		 struct ad5706r_init_param *init_param)
{
	struct ad5706r_dev *dev;
	uint16_t prod_id_h, prod_id_l;
	int ret;

	if (!device)
		return -ENODEV;

	if (!init_param)
		return -EINVAL;

	dev = (struct ad5706r_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init_prm);
	if (ret)
		goto error_dev;

	if (init_param->dev_addr > AD5706R_MAX_MD_ADDR) {
		pr_info("Selected device address is invalid.\n");
		ret = -EINVAL;
		goto error_spi;
	} else
		dev->dev_addr = init_param->dev_addr;

	pr_info("Device Selected: %d \n", dev->dev_addr);

	ret = no_os_gpio_get_optional(&dev->gpio_reset, init_param->gpio_reset);
	if (ret)
		goto error_spi;

	/* Validate product IDs */
	ret = ad5706r_spi_reg_read(dev, AD5706R_REG_PRODUCT_ID_H, &prod_id_h);
	if (ret)
		goto error_gpio;

	ret = ad5706r_spi_reg_read(dev, AD5706R_REG_PRODUCT_ID_L, &prod_id_l);
	if (ret)
		goto error_gpio;

	if ((prod_id_h != AD5706R_PRODUCT_ID_H) ||
	    (prod_id_l != AD5706R_PRODUCT_ID_L)) {
		ret = -ENODEV;
		pr_info("Device ID validation failed: %d \n", ret);
		goto error_gpio;
	}

	/* Reset */
	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);
		if (ret)
			goto error_gpio;

		ret = ad5706r_hw_reset(dev);
		if (ret) {
			pr_info("Reset failed: %d \n", ret);
			goto error_gpio;
		}
	} else {
		ret = ad5706r_sw_reset(dev);
		if (ret) {
			pr_info("Reset failed: %d \n", ret);
			goto error_gpio;
		}

		/* Emulate hard reset by writing default value on
			Interface Configuration A register
			in addition to SW reset */
		ret = ad5706r_spi_reg_write(dev,
					    AD5706R_REG_INTERFACE_CONFIG_A,
					    AD5706R_INTF_CFG_A_DEFAULT);
		if (ret)
			goto error_gpio;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_ldac_tgp, init_param->gpio_ldac_tgp);
	if (ret)
		goto error_gpio;

	if (dev->gpio_ldac_tgp) {

		ret = no_os_gpio_direction_output(dev->gpio_ldac_tgp, NO_OS_GPIO_HIGH);
		if (ret)
			goto error_gpio;

		ret = no_os_gpio_set_value(dev->gpio_ldac_tgp, NO_OS_GPIO_LOW);
		if (ret)
			goto error_gpio;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_shdn, init_param->gpio_shdn);
	if (ret)
		goto error_gpio;

	if (dev->gpio_shdn) {

		ret = no_os_gpio_direction_output(dev->gpio_shdn, NO_OS_GPIO_HIGH);
		if (ret)
			goto error_gpio;

		ret = no_os_gpio_set_value(dev->gpio_shdn, NO_OS_GPIO_LOW);
		if (ret)
			goto error_gpio;
	}

	/* Set SPI interface configuration */
	ret = ad5706r_set_device_spi(dev, &init_param->spi_cfg);
	if (ret) {
		pr_info("SPI interface configuration failed: %d \n", ret);
		goto error_gpio;
	}

	ret = ad5706r_check_scratch_pad(dev);
	if (ret) {
		pr_info("Scratch pad test failed: %d \n", ret);
		goto error_gpio;
	}

	/* Set device configuration */
	ret = ad5706r_device_config(dev, init_param);
	if (ret) {
		pr_info("Device Configuration failed: %d \n", ret);
		goto error_gpio;
	}

	*device = dev;

	return 0;

error_gpio:
	ad5706r_remove_gpio(dev);

error_spi:
	no_os_spi_remove(dev->spi_desc);

error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the device descriptor.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5706r_remove(struct ad5706r_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = ad5706r_remove_gpio(dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

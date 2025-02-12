/***************************************************************************//**
 *   @file   ad3530r.c
 *   @author Sai Kiran Gudla (Saikiran.Gudla@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
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

#include "ad3530r.h"
#include <stdlib.h>
#include <string.h>
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD3530R_DATA_INDEX(x)               (x) ? 1 : 2
#define AD3530R_INSTR_LEN(x)                AD3530R_DATA_INDEX(x)
#define AD3530R_ADDR_INDEX(x)               (x) ? 0 : 1
#define AD3530R_BUFF_LEN(x)                 (x) ? 2 : 3
#define AD3530R_CRC_BUFF_LEN(x)             (x) ? 3 : 4
#define AD3530R_CRC_INDEX(x)                AD3530R_ADDR_INDEX(x) + 2

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/
static struct ad3530r_transfer_config multi_cfg = {
	.single_instr = 0, /* Sets the streaming mode. */
	.stream_length_keep_value = 1 /* Prevents the stream length value from
	automatically resetting to zero. */
};

/**
 * @brief Updates the interface configuration.
 * @param desc - The device structure.
 * @param cfg - Updated interface configuration structure.
 * @return 0 in case of success, negative error otherwise.
 */
int ad3530r_update_interface_cfg(struct ad3530r_desc *desc,
				 struct ad3530r_transfer_config *cfg)
{
	int ret ;

	if (!desc || !cfg)
		return -EINVAL;

	ret = ad3530r_spi_write_mask(desc,
				     AD3530R_REG_ADDR_INTERFACE_CONFIG_A,
				     AD3530R_MASK_ADDR_ASCENSION,
				     cfg->addr_asc);
	if (ret)
		return ret;
	desc->spi_cfg.addr_asc = cfg->addr_asc;

	ret = ad3530r_spi_write_mask(desc,
				     AD3530R_REG_ADDR_INTERFACE_CONFIG_B,
				     AD3530R_MASK_SHORT_INSTRUCTION,
				     cfg->short_instr);
	if (ret)
		return ret;
	desc->spi_cfg.short_instr = cfg->short_instr;

	ret = ad3530r_spi_write_mask(desc,
				     AD3530R_REG_ADDR_INTERFACE_CONFIG_B,
				     AD3530R_MASK_SINGLE_INST,
				     cfg->single_instr);
	if (ret)
		return ret;
	desc->spi_cfg.single_instr = cfg->single_instr;

	ret = ad3530r_spi_write_mask(desc,
				     AD3530R_REG_ADDR_TRANSFER_REGISTER,
				     AD3530R_MASK_STREAM_LENGTH_KEEP_VALUE,
				     cfg->stream_length_keep_value);
	if (ret)
		return ret;
	desc->spi_cfg.stream_length_keep_value =
		cfg->stream_length_keep_value;

	ret = ad3530r_spi_write_mask(desc,
				     AD3530R_REG_ADDR_STREAM_MODE,
				     AD3530R_MASK_LENGTH,
				     cfg->stream_mode_length);
	if (ret)
		return ret;
	desc->spi_cfg.stream_mode_length = cfg->stream_mode_length;

	return 0;
}

/**
 * @brief Transfer spi read/write messages using CRC.
 * @param desc - The device structure.
 * @param data - The transfer data structure.
 * @return 0 in case of success, negative error otherwise.
 */
static int ad3530r_transfer_with_crc(struct ad3530r_desc *desc,
				     struct ad3530r_transfer_data *data)
{
	struct no_os_spi_msg msg;
	uint32_t count;
	int8_t sign;
	int inc, ret;
	uint8_t out_buff[4] = { 0 };
	uint8_t in_buff[4] = { 0 };
	uint8_t addr[2];
	bool short_instr;

	if (!desc || !data)
		return -EINVAL;

	/* Currently streaming mode is not supported with CRC */
	data->spi_cfg->single_instr = 1;
	data->spi_cfg->stream_mode_length = 0;

	if (data->spi_cfg) {
		ret = ad3530r_update_interface_cfg(desc, data->spi_cfg);
		if (ret)
			return ret;
	}

	short_instr = data->spi_cfg->short_instr;
	sign = desc->spi_cfg.addr_asc ? 1 : -1;
	inc = 0 ;
	count = 0;

	msg.cs_change = true;
	msg.bytes_number = AD3530R_CRC_BUFF_LEN(short_instr);
	msg.tx_buff = out_buff;
	msg.rx_buff = in_buff;

	do {
		/* Get the adddress for which CRC value will be calculated */
		if (short_instr) {
			if (data->is_read)
				addr[0] = (data->addr + inc) | AD3530R_READ_BIT;
			else
				addr[0] = (data->addr + inc) & AD3530R_ADDR_MASK;
		} else {
			if (data->is_read)
				addr[0] = AD3530R_READ_BIT;
			else
				addr[0] = AD3530R_WRITE_BIT_LONG_INSTR;
			addr[1] = data->addr + inc ;
		}

		addr[AD3530R_ADDR_INDEX(short_instr)] %= AD3530R_REG_ADDR_MAX;

		/* Take in consideration register address for CRC calculation */
		memcpy(out_buff, addr, AD3530R_INSTR_LEN(short_instr));
		if (data->is_read) {
			/* For read transaction, crc is computed with zero padding */
			out_buff[AD3530R_DATA_INDEX(short_instr)] = 0x00;
		} else {
			/* For write transaction, crc is computed with data to be sent. */
			out_buff[AD3530R_DATA_INDEX(short_instr)] = data->data[count];
		}

		out_buff[AD3530R_CRC_INDEX(short_instr)] =
			no_os_crc8(desc->crc_table, out_buff, AD3530R_CRC_INDEX(short_instr),
				   AD3530R_CRC_SEED);

		ret = no_os_spi_transfer(desc->spi, &msg, 1);
		if (ret)
			return ret;

		/* Check received CRC */
		if (data->is_read) {
			/* Save received data after comparing CRC */
			out_buff[AD3530R_CRC_INDEX(short_instr)] = in_buff[AD3530R_CRC_INDEX(
						short_instr)];
			if (in_buff[AD3530R_CRC_INDEX(short_instr)] !=
			    no_os_crc8(desc->crc_table, out_buff, AD3530R_CRC_INDEX(short_instr),
				       AD3530R_CRC_SEED))
				return -EBADMSG;
			data->data[count] = in_buff[AD3530R_DATA_INDEX(short_instr)];
		} else {
			if (in_buff[AD3530R_CRC_INDEX(short_instr)] != out_buff[AD3530R_CRC_INDEX(
						short_instr)])
				return -EBADMSG;
		}

		inc += sign;
		count++;
	} while (count < data->len);

	return 0;
}

/**
 * @brief Transfer spi read/write messages.
 * @param desc - The device structure.
 * @param data - The transfer data structure.
 * @return 0 in case of success, negative error otherwise.
 */
static int ad3530r_transfer(struct ad3530r_desc *desc,
			    struct ad3530r_transfer_data *data)
{
	struct no_os_spi_msg msgs[2] = { 0 };
	uint8_t instr[2];

	if (!desc || !data)
		return -EINVAL;

	if (data->spi_cfg)
		ad3530r_update_interface_cfg(desc, data->spi_cfg);

	if (data->spi_cfg->short_instr) {
		instr[0] = data->is_read ? data->addr | AD3530R_READ_BIT : data->addr &
			   AD3530R_ADDR_MASK;
	} else {
		instr[0] = data->is_read ? (data->addr >> 8) | AD3530R_READ_BIT :
			   (data->addr >> 8) & AD3530R_ADDR_MASK;
		instr[1] = data->addr;
	}

	msgs[0].tx_buff = instr;
	msgs[0].bytes_number = data->spi_cfg->short_instr ? 1 : 2;
	msgs[1].bytes_number = data->len;
	msgs[1].cs_change = true;
	if (data->is_read)
		msgs[1].rx_buff = data->data;
	else
		msgs[1].tx_buff = data->data;

	return no_os_spi_transfer(desc->spi, msgs, NO_OS_ARRAY_SIZE(msgs));
}

/**
 * @brief Write into device register.
 * @param desc - The device structure.
 * @param reg_addr - The register's address.
 * @param reg_val - The register's value.
 * @return 0 in case of success, negative error otherwise.
 */
int ad3530r_reg_write(struct ad3530r_desc *desc,
		      uint32_t reg_addr,
		      uint16_t reg_val)
{
	int ret;
	uint8_t reg_len;
	uint32_t addr;
	uint8_t cnt = 0;
	uint8_t buf[5] = {0};
	uint8_t crc_cal = 0;

	if (!desc)
		return -EINVAL;

	/* Get the register length */
	reg_len = AD3530R_LEN(reg_addr);

	if (!desc->spi_cfg.addr_asc)
		addr = AD3530R_ADDR(reg_addr) + reg_len - 1;
	else
		addr = AD3530R_ADDR(reg_addr);

	if (desc->spi_cfg.short_instr  && addr < AD3530R_MAX_SHORT_REG_ADDR) {
		buf[cnt++] = (uint8_t)(addr & AD3530R_ADDR_MASK);
	} else {
		buf[cnt++] = (uint8_t)(AD3530R_WRITE_BIT_LONG_INSTR | (addr >> 8));
		buf[cnt++] = (uint8_t)addr;
	}

	if (!desc->spi_cfg.addr_asc)
		no_os_memswap64(&reg_val, reg_len, reg_len);

	memcpy(&buf[cnt], &reg_val, reg_len);

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_len;

	if (desc->crc_en) {
		/* For write transaction,  CRC is computed on
		 * address + register data both on host and slave device. */
		crc_cal = no_os_crc8(desc->crc_table,
				     buf,
				     cnt,
				     AD3530R_CRC_SEED);
		buf[cnt] = crc_cal;

		ret = no_os_spi_write_and_read(desc->spi,
					       buf,
					       cnt + 1);
		if (ret)
			return ret;

		if (crc_cal != buf[cnt])
			return -EBADMSG;
	} else {
		ret = no_os_spi_write_and_read(desc->spi, buf, cnt);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Write multiple register's values.
 * @param desc - The device structure.
 * @param start_addr - Starting register address to write into.
 * @param count - Number of bytes to write.
 * @param buff - Buffer to write data from.
 * @return 0 in case of success, negative error otherwise.
 */
int ad3530r_multiple_reg_write(struct ad3530r_desc *desc,
			       uint32_t start_addr,
			       uint8_t count,
			       uint8_t *buff)
{
	struct ad3530r_transfer_data msg = { 0 };

	if (!desc || !buff || (start_addr > AD3530R_REG_ADDR_MAX))
		return -EINVAL;

	/* Short instruction only works below reg address 0x80 */
	if (start_addr < AD3530R_MAX_SHORT_REG_ADDR)
		multi_cfg.short_instr = desc->spi_cfg.short_instr;
	else
		multi_cfg.short_instr = 0;

	multi_cfg.stream_mode_length = count;
	multi_cfg.addr_asc = desc->spi_cfg.addr_asc;
	multi_cfg.single_instr = 0;

	msg.is_read = 0;
	msg.addr = AD3530R_ADDR(start_addr);
	msg.data = buff;
	msg.len = count;
	msg.spi_cfg = &multi_cfg;

	if (desc->crc_en)
		return ad3530r_transfer_with_crc(desc, &msg);
	else
		return ad3530r_transfer(desc, &msg);
}

/**
 * @brief Read from device register.
 * @param desc - The device structure.
 * @param reg_addr - The register's address.
 * @param reg_val - The register's read value.
 * @return 0 in case of success, negative error otherwise.
 */
int ad3530r_reg_read(struct ad3530r_desc *desc,
		     uint32_t reg_addr,
		     uint16_t *reg_val)
{
	int32_t ret;
	uint32_t addr;
	uint8_t cnt = 0;
	uint8_t dindx = 0;
	uint8_t reg_len;
	uint8_t buf[5] = {0};
	uint8_t crc_buf[4] = {0};
	uint8_t crc_out = 0;
	uint8_t crc_cal = 0;

	if (!desc || !reg_val)
		return -EINVAL;

	/* Get the register length */
	reg_len = AD3530R_LEN(reg_addr);

	if (!desc->spi_cfg.addr_asc)
		addr = AD3530R_ADDR(reg_addr) + reg_len - 1;
	else
		addr = AD3530R_ADDR(reg_addr);

	if (desc->spi_cfg.short_instr && addr < AD3530R_MAX_SHORT_REG_ADDR) {
		buf[cnt++] = (uint8_t)(AD3530R_READ_BIT | addr);
	} else {
		buf[cnt++] = (uint8_t)(AD3530R_READ_BIT | (addr >> 8));
		buf[cnt++] = (uint8_t)addr;
	}

	/* Save the data phase start index */
	dindx = cnt;

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_len;

	if (desc->crc_en) {
		memcpy(crc_buf, buf, cnt);

		/* CRC is computed with address + zero padding on the host device */
		buf[cnt] = no_os_crc8(desc->crc_table,
				      buf,
				      cnt,
				      AD3530R_CRC_SEED);

		/* For read transaction, crc is computed with received data */
		ret = no_os_spi_write_and_read(desc->spi,
					       buf,
					       cnt + 1);
		if (ret)
			return ret;
		crc_out = buf[cnt];

		/* CRC is computed with the register read data on the slave device */
		memcpy(&crc_buf[dindx], &buf[dindx], reg_len);
		crc_cal = no_os_crc8(desc->crc_table,
				     crc_buf,
				     cnt,
				     AD3530R_CRC_SEED);
		if (crc_out != crc_cal)
			return -EBADMSG;
	} else {
		ret = no_os_spi_write_and_read(desc->spi, buf, cnt);
		if (ret)
			return ret;
	}

	if (reg_len == 1)
		*reg_val = buf[dindx];
	else {
		if (desc->spi_cfg.addr_asc)
			no_os_swap(buf[dindx], buf[dindx + 1]);
		*reg_val = no_os_get_unaligned_be16(&buf[dindx]);
	}

	return 0;
}

/**
 * @brief Read multiple register's values.
 * @param desc - The device structure.
 * @param addr - Starting register address to read from.
 * @param count - Number of bytes to read.
 * @param buff - Buffer to store data.
 * @return 0 in case of success, negative error otherwise.
 */
int ad3530r_multiple_reg_read(struct ad3530r_desc *desc,
			      uint32_t addr, uint8_t count,
			      uint8_t *buff)
{
	struct ad3530r_transfer_data msg = { 0 };

	if (!desc || !buff)
		return -EINVAL;

	/* Short instruction only works below reg address 0x80 */
	if (addr < AD3530R_MAX_SHORT_REG_ADDR)
		multi_cfg.short_instr = desc->spi_cfg.short_instr;
	else
		multi_cfg.short_instr = 0;

	multi_cfg.stream_mode_length = count;
	multi_cfg.addr_asc = desc->spi_cfg.addr_asc;

	msg.is_read = 1;
	msg.addr = AD3530R_ADDR(addr);
	msg.data = buff;
	msg.len = count;

	msg.spi_cfg = &multi_cfg;

	if (desc->crc_en)
		return ad3530r_transfer_with_crc(desc, &msg);
	else
		return ad3530r_transfer(desc, &msg);
}

/**
 * @brief SPI write to device using a mask.
 * @param desc - The device structure.
 * @param addr - The register's address.
 * @param mask - The mask for a specific register field.
 * @param val - The register's value.
 * @return 0 in case of success, negative error otherwise.
 */
int ad3530r_spi_write_mask(struct ad3530r_desc *desc,
			   uint32_t addr, uint32_t mask, uint16_t val)
{
	int ret;
	uint16_t data;

	ret = ad3530r_reg_read(desc, addr, &data);
	if (ret)
		return ret;

	data = (data & ~mask) | no_os_field_prep(mask, val);

	return ad3530r_reg_write(desc, addr, data);
}

/**
 * Set reference configuration.
 * @param desc - The device structure.
 * @param reference_selector - The reference to be selected.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_reference(struct ad3530r_desc *desc,
			  enum ad3530r_ch_vref_select reference_selector)
{
	int ret;

	ret = ad3530r_spi_write_mask(desc,
				     AD3530R_REG_ADDR_REF_CONTROL_0,
				     AD3530R_MASK_REERENCE_SELECT,
				     reference_selector);
	if (ret)
		return ret;

	desc->vref_enable = reference_selector;
	return 0;
}

/**
 * Set operating mode for each channel.
 * @param desc - The device structure.
 * @param chn_num - Channel number to be configured.
 * @param chn_op_mode - Operation mode to be set.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_operating_mode(struct ad3530r_desc *desc, uint8_t chn_num,
			       enum ad3530r_operating_mode chn_op_mode)
{
	int ret;

	ret = ad3530r_spi_write_mask(desc,
				     chn_num < AD3530R_MAX_CHANNEL_OP_MODE_0  ? AD3530R_REG_ADDR_OPERATING_MODE_0 :
				     AD3530R_REG_ADDR_OPERATING_MODE_1,
				     AD3530R_MASK_OPERATING_MODE(chn_num),
				     chn_op_mode);
	if (ret)
		return ret;

	desc->chn_op_mode[chn_num] = chn_op_mode;

	return 0;
}

/**
 * Set output range for all channels.
 * @param desc - The device structure.
 * @param range_sel - Output range to be selected.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_output_range(struct ad3530r_desc *desc,
			     enum ad3530r_ch_output_range range_sel)
{
	int ret;

	ret = ad3530r_spi_write_mask(desc,
				     AD3530R_REG_ADDR_OUTPUT_CONTROL_0,
				     AD3530R_MASK_OUTPUT_RANGE, range_sel);
	if (ret)
		return ret;

	desc->range = range_sel;
	return 0;
}

/**
 * Configures the CRC selection bit .
 * @param desc - The device structure.
 * @param en_di - boolean selection for crc bit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_crc_enable(struct ad3530r_desc *desc, bool en_di)
{
	int err;
	uint16_t reg;


	reg = en_di ? AD3530R_CRC_ENABLE_VALUE : AD3530R_CRC_DISABLE_VALUE;
	err = ad3530r_spi_write_mask(desc, AD3530R_REG_ADDR_INTERFACE_CONFIG_C,
				     AD3530R_MASK_CRC_ENABLE, reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	desc->crc_en = en_di;

	return 0;
}

/**
 * Set the multiplexer output select register.
 * @param desc - The device structure.
 * @param mux_output_sel - signal to be monitored on the mux_out pin.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_mux_out_select(struct ad3530r_desc *desc,
			       enum ad3530r_mux_out_select mux_output_sel)
{
	int ret;

	ret = ad3530r_spi_write_mask(desc, AD3530R_REG_ADDR_MUX_OUT_SELECT,
				     AD3530R_MASK_MUX_SELECT, mux_output_sel);
	if (ret)
		return ret;

	desc->mux_out_sel = mux_output_sel;

	return 0;
}

/**
 * Set the hardware ldac configuration.
 * @param desc - The device structure.
 * @param mask_hw_ldac - The array contains HW LDAC channel masks.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_hw_ldac(struct ad3530r_desc *desc, uint16_t mask_hw_ldac)
{
	int ret;

	ret = ad3530r_reg_write(desc, AD3530R_REG_ADDR_HW_LDAC_EN_0,
				mask_hw_ldac);
	if (ret)
		return ret;

	desc->hw_ldac_mask = mask_hw_ldac;

	return 0;
}

/**
 * Set the software ldac configuration.
 * @param desc - The device structure.
 * @param mask_sw_ldac - The array contains SW LDAC channel masks.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_sw_ldac(struct ad3530r_desc *desc, uint16_t mask_sw_ldac)
{
	int ret;

	ret = ad3530r_reg_write(desc, AD3530R_REG_ADDR_SW_LDAC_EN_0,
				mask_sw_ldac);
	if (ret)
		return ret;

	desc->sw_ldac_mask = mask_sw_ldac;

	return 0;
}

/**
 * Set dac value based on the user selected DAC update mode.
 * @param desc - The device structure.
 * @param dac_value - value that will be set in the register.
 * @param dac_channel - selected channel.
 * @param write_mode - selected write mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_dac_value(struct ad3530r_desc *desc,
			  uint16_t dac_value,
			  uint8_t dac_channel,
			  enum ad3530r_write_mode write_mode)
{
	int ret;
	bool trig_ldac = false;

	switch (write_mode) {
	case AD3530R_WRITE_DAC_REGS:
		ret = ad3530r_reg_write(desc,
					AD3530R_REG_ADDR_DAC_CHN(dac_channel),
					dac_value);
		break;

	case AD3530R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC:
		trig_ldac = true;

	case AD3530R_WRITE_INPUT_REGS:
		ret = ad3530r_reg_write(desc,
					AD3530R_REG_ADDR_INPUT_CHN(dac_channel),
					dac_value);
		break;

	default:
		break;
	}

	if (ret)
		return ret;

	if (trig_ldac) {
		if (desc->ldac) {
			ret = ad3530r_hw_ldac_trigger(desc);
			if (ret)
				return ret;
		} else {
			ret = ad3530r_sw_ldac_trigger(desc);
			if (ret)
				return ret;
		}
	}

	return 0;
}

/**
 * Sets dac value for multiple channels.
 * @param desc - The device structure.
 * @param dac_value - value that will be set in the register.
 * @param dac_chn_mask - mask for selected channels.
 * @param write_mode - selected write mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_set_multidac_value(struct ad3530r_desc *desc,
			       uint16_t dac_value,
			       uint16_t dac_chn_mask,
			       enum ad3530r_write_mode write_mode)
{
	int ret;
	bool trig_ldac = false;

	switch (write_mode) {
	case AD3530R_WRITE_DAC_REGS:
		ret = ad3530r_reg_write(desc,
					AD3530R_REG_ADDR_MULTI_DAC_SEL_0,
					dac_chn_mask);
		if (ret)
			return ret;

		ret = ad3530r_reg_write(desc,
					AD3530R_REG_ADDR_MULTI_DAC_CH,
					dac_value);
		if (ret)
			return ret;

		break;

	case AD3530R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC:
		trig_ldac = true;

	case AD3530R_WRITE_INPUT_REGS:
		ret = ad3530r_reg_write(desc,
					AD3530R_REG_ADDR_MULTI_INPUT_SEL_0,
					dac_chn_mask);
		if (ret)
			return ret;

		ret = ad3530r_reg_write(desc,
					AD3530R_REG_ADDR_MULTI_INPUT_CH,
					dac_value);
		if (ret)
			return ret;

		break;

	default:
		break;
	}

	if (trig_ldac) {
		if (desc->ldac) {
			ret = ad3530r_hw_ldac_trigger(desc);
			if (ret)
				return ret;
		} else {
			ret = ad3530r_sw_ldac_trigger(desc);
			if (ret)
				return ret;
		}
	}

	return 0;
}

/**
 * @brief Trigger the LDAC using software.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_sw_ldac_trigger(struct ad3530r_desc *desc)
{
	if (!desc)
		return -EINVAL;

	return ad3530r_spi_write_mask(desc,
				      AD3530R_REG_ADDR_SW_LDAC_TRIG_B,
				      AD3530R_MASK_SW_LDAC_TRIG_B,
				      1);

}

/**
 * @brief Trigger the LDAC using hardware.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_hw_ldac_trigger(struct ad3530r_desc *desc)
{
	int ret;

	if (!desc || !desc->ldac)
		return -EINVAL;

	ret = no_os_gpio_set_value(desc->ldac, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(AD3530R_LDAC_PULSE_US);

	ret = no_os_gpio_set_value(desc->ldac, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Perform a hard/soft reset.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_reset(struct ad3530r_desc *desc)
{
	int ret;
	uint16_t val;

	if (!desc)
		return -EINVAL;

	if (desc->reset) {
		ret = no_os_gpio_set_value(desc->reset, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		no_os_mdelay(1);
		ret = no_os_gpio_set_value(desc->reset, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	} else {
		ret = ad3530r_spi_write_mask(desc,
					     AD3530R_REG_ADDR_INTERFACE_CONFIG_A,
					     AD3530R_MASK_SOFTWARE_RESET,
					     AD3530R_MASK_SOFTWARE_RESET);
		if (ret)
			return ret;
	}

	no_os_mdelay(1);

	/* Default interface configuration after reset */
	memset(&desc->spi_cfg, 0, sizeof(desc->spi_cfg));

	desc->crc_en = 0;
	desc->vref_enable = 0;
	desc->range = 0;
	memset(desc->chn_op_mode, AD3530R_CH_OPERATING_MODE_3,
	       sizeof(desc->chn_op_mode));

	ret = ad3530r_reg_read(desc,
			       AD3530R_REG_ADDR_STATUS_CONTROL,
			       &val);
	if (ret)
		return ret;

	if (val != AD3530R_DEFAULT_STATUS_REG_VAL)
		return -ENODEV;

	return 0;
}

/**
 * @brief Perform a write operation followed by a read
 *        operation from the scratch pad register.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad3530r_check_scratch_pad(struct ad3530r_desc *desc)
{
	int ret;
	uint16_t val;

	ret = ad3530r_reg_write(desc, AD3530R_REG_ADDR_SCRATCH_PAD,
				AD3530R_SCRATCH_PAD_TEST_VAL);
	if (ret)
		return ret;

	ret = ad3530r_reg_read(desc, AD3530R_REG_ADDR_SCRATCH_PAD, &val);
	if (ret)
		return ret;

	if (val != AD3530R_SCRATCH_PAD_TEST_VAL)
		return -EINVAL;

	return 0;
}

/**
 * @brief Configures the device.
 * @param desc - The device structure to be initialized.
 * @param dev_param - Configuration parameters for the device.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_device_config(struct ad3530r_desc *desc,
			  struct ad3530r_init_param *dev_param)
{
	int ret;
	uint8_t chn;

	ret = ad3530r_set_reference(desc, dev_param->vref_enable);
	if (ret)
		return ret;

	for (chn = 0; chn < AD3530R_NUM_CH; chn++) {
		ret = ad3530r_set_operating_mode(desc, chn, dev_param->chn_op_mode[chn]);
		if (ret)
			return ret;
	}

	ret = ad3530r_set_output_range(desc, dev_param->range);
	if (ret)
		return ret;

	ret = ad3530r_set_hw_ldac(desc, dev_param->hw_ldac_mask);
	if (ret)
		return ret;

	ret = ad3530r_set_sw_ldac(desc, dev_param->sw_ldac_mask);
	if (ret)
		return ret;

	ret = ad3530r_set_crc_enable(desc, dev_param->crc_en);
	if (ret)
		return ret;

	ret = ad3530r_set_mux_out_select(desc, dev_param->mux_out_sel);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_init(struct ad3530r_desc **desc,
		 struct ad3530r_init_param *init_param)
{
	struct ad3530r_desc *descriptor;
	int ret;

	if (!desc || !init_param)
		return -EINVAL;

	descriptor = (struct ad3530r_desc*)calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->spi, init_param->spi_param);
	if (ret)
		goto err;

	ret = no_os_gpio_get_optional(&descriptor->reset,
				      init_param->reset_gpio_param_optional);
	if (ret)
		goto err_spi;

	if (descriptor->reset) {
		ret = no_os_gpio_direction_output(descriptor->reset, NO_OS_GPIO_HIGH);
		if (ret)
			goto err_reset;
	}

	if (init_param->ldac_gpio_param_optional) {
		ret = no_os_gpio_get_optional(&descriptor->ldac,
					      init_param->ldac_gpio_param_optional);
		if (ret)
			goto err_reset;

		if (descriptor->ldac) {
			ret = no_os_gpio_direction_output(descriptor->ldac, NO_OS_GPIO_HIGH);
			if (ret)
				goto err_ldac;
		}
	}

	ret = ad3530r_reset(descriptor);
	if (ret) {
		pr_err("Reset failed: %d \n", ret);
		goto err_ldac;
	}

	ret = ad3530r_update_interface_cfg(descriptor, &init_param->spi_cfg);
	if (ret) {
		pr_err("Interface configuration failed: %d \n", ret);
		goto err_ldac;
	}

	ret = ad3530r_check_scratch_pad(descriptor);
	if (ret) {
		pr_err("Scratch pad test failed: %d \n", ret);
		goto err_ldac;
	}

	ret = ad3530r_device_config(descriptor, init_param);
	if (ret) {
		pr_err("Device Configuration failed: %d \n", ret);
		goto err_ldac;
	}

	if (descriptor->crc_en)
		no_os_crc8_populate_msb(descriptor->crc_table, AD3530R_CRC_POLY);

	descriptor->chip_id = init_param->chip_id;

	*desc = descriptor;

	return 0;
err_ldac:
	no_os_gpio_remove(descriptor->ldac);
err_reset:
	no_os_gpio_remove(descriptor->reset);
err_spi:
	no_os_spi_remove(descriptor->spi);
err:
	free(descriptor);

	return ret;
}

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad3530r_remove(struct ad3530r_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_gpio_remove(desc->ldac);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(desc->reset);
	if (ret)
		return ret;

	ret = no_os_spi_remove(desc->spi);
	if (ret)
		return ret;

	free(desc);

	return 0;
}

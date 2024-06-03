/**************************************************************************//**
*   @file   ad3552r.c
*   @brief  Implementation of ad3552r Driver
*   @author Mihail Chindris (Mihail.Chindris@analog.com)
*
*******************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
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
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
******************************************************************************/

#include "ad3552r.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_timer.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/* Private attributes */
enum ad3552r_spi_attributes {
	/* Read registers in ascending order if set. Else descending */
#ifdef AD3552R_QSPI_IMPLEMENTED
	AD3552R_ADDR_ASCENSION = AD3552R_SPI_SYNCHRONOUS_ENABLE + 1,
#else
	AD3552R_ADDR_ASCENSION = AD3552R_CRC_ENABLE + 1,
#endif
	/* Single instruction mode if set. Else, stream mode */
	AD3552R_SINGLE_INST,
	/* Number of addresses to loop on when stream writing. */
	AD3552R_STREAM_MODE,
	/* Keep stream value if set. */
	AD3552R_STREAM_LENGTH_KEEP_VALUE,
};

#define SEC_TO_10NS(x)		((x) * 100000000)

#define AD3552R_ATTR_REG(attr) addr_mask_map[attr][0]
#define AD3552R_ATTR_MASK(attr) addr_mask_map[attr][1]
#define AD3552R_CH_ATTR_REG(attr) addr_mask_map_ch[attr][0]
#define AD3552R_CH_ATTR_MASK(ch, attr) addr_mask_map_ch[attr][(ch) + 1]

#define AD3552R_MAX_REG_SIZE				3
#define AD3552R_READ_BIT				(1 << 7)
#define AD3552R_ADDR_MASK				(~AD3552R_READ_BIT)
#define AD3552R_CRC_ENABLE_VALUE			(NO_OS_BIT(6) | NO_OS_BIT(1))
#define AD3552R_CRC_DISABLE_VALUE			(NO_OS_BIT(1) | NO_OS_BIT(0))
#define AD3552R_EXTERNAL_VREF_MASK			NO_OS_BIT(1)
#define AD3552R_CRC_POLY				0x07
#define AD3552R_CRC_SEED				0xA5
#define AD3552R_SECONDARY_REGION_ADDR			0x28
#define AD3552R_DEFAULT_CONFIG_B_VALUE			0x8
#define AD3552R_DATA_IDX(x)				(1 + (x))
#define AD3552R_DEFAULT_DAC_UPDATE_PERIOD		1000
#define AD3552R_SCRATCH_PAD_TEST_VAL1			0x34
#define AD3552R_SCRATCH_PAD_TEST_VAL2			0xB2

#define AD3552R_MAX_CH_NUM(id) ((id) == AD3542R_ID || (id) == AD3552R_ID ?\
					AD3552R_NUM_CHANNELS : AD3551R_NUM_CHANNELS)

#define REG_DATA_LEN(is_fast) ((is_fast) ? AD3552R_STORAGE_BITS_FAST_MODE / 8 :\
					   AD3552R_STORAGE_BITS_PREC_MODE / 8)

#define AD3552R_GAIN_SCALE				1000

#ifdef AD3552R_DEBUG
#define _CHECK_STATUS(_status, reg, new_reg, bit_name, clr_err)  do {\
	if (reg & AD3552R_MASK_ ## bit_name) {\
		new_reg |= clr_err ? AD3552R_MASK_ ## bit_name : 0;\
		_status |= AD3552R_ ## bit_name;\
		pr_debug(" %s\n", # bit_name);\
	}\
} while (0)
#endif

static const uint16_t addr_mask_map[][2] = {
	[AD3552R_ADDR_ASCENSION]	= {
		AD3552R_REG_ADDR_INTERFACE_CONFIG_A,
		AD3552R_MASK_ADDR_ASCENSION
	},
	[AD3552R_SINGLE_INST] = {
		AD3552R_REG_ADDR_INTERFACE_CONFIG_B,
		AD3552R_MASK_SINGLE_INST
	},
	[AD3552R_STREAM_MODE] = {
		AD3552R_REG_ADDR_STREAM_MODE,
		AD3552R_MASK_LENGTH
	},
	[AD3552R_STREAM_LENGTH_KEEP_VALUE] = {
		AD3552R_REG_ADDR_TRANSFER_REGISTER,
		AD3552R_MASK_STREAM_LENGTH_KEEP_VALUE
	},
	[AD3552R_SDO_DRIVE_STRENGTH] = {
		AD3552R_REG_ADDR_INTERFACE_CONFIG_D,
		AD3552R_MASK_SDO_DRIVE_STRENGTH
	},
	[AD3552R_VREF_SELECT] = {
		AD3552R_REG_ADDR_SH_REFERENCE_CONFIG,
		AD3552R_MASK_REFERENCE_VOLTAGE_SEL
	},
	[AD3552R_CRC_ENABLE] = {
		AD3552R_REG_ADDR_INTERFACE_CONFIG_C,
		AD3552R_MASK_CRC_ENABLE
	},
#if AD3552R_QSPI_IMPLEMENTED
	[AD3552R_SPI_MULTI_IO_MODE] = {
		AD3552R_REG_ADDR_TRANSFER_REGISTER,
		AD3552R_MASK_MULTI_IO_MODE
	},
	[AD3552R_SPI_DATA_RATE] = {
		AD3552R_REG_ADDR_INTERFACE_CONFIG_D,
		AD3552R_MASK_SPI_CONFIG_DDR
	},
	[AD3552R_SPI_SYNCHRONOUS_ENABLE] = {
		AD3552R_REG_ADDR_INTERFACE_CONFIG_D,
		AD3552R_MASK_DUAL_SPI_SYNCHROUNOUS_EN
	},
#endif
};

/* 0 -> reg addr, 1->ch0 mask, 2->ch1 mask */
static const uint16_t addr_mask_map_ch[][3] = {
	[AD3552R_CH_DAC_POWERDOWN] = {
		AD3552R_REG_ADDR_POWERDOWN_CONFIG,
		AD3552R_MASK_CH_DAC_POWERDOWN(0),
		AD3552R_MASK_CH_DAC_POWERDOWN(1)
	},
	[AD3552R_CH_AMPLIFIER_POWERDOWN] = {
		AD3552R_REG_ADDR_POWERDOWN_CONFIG,
		AD3552R_MASK_CH_AMPLIFIER_POWERDOWN(0),
		AD3552R_MASK_CH_AMPLIFIER_POWERDOWN(1)
	},
	[AD3552R_CH_OUTPUT_RANGE_SEL] = {
		AD3552R_REG_ADDR_CH0_CH1_OUTPUT_RANGE,
		AD3552R_MASK_CH_OUTPUT_RANGE_SEL(0),
		AD3552R_MASK_CH_OUTPUT_RANGE_SEL(1)
	},
	[AD3552R_CH_TRIGGER_SOFTWARE_LDAC] = {
		AD3552R_REG_ADDR_SW_LDAC_16B,
		AD3552R_MASK_CH(0),
		AD3552R_MASK_CH(1)
	},
	[AD3552R_CH_HW_LDAC_MASK] = {
		AD3552R_REG_ADDR_HW_LDAC_16B,
		AD3552R_MASK_CH(0),
		AD3552R_MASK_CH(1)
	},
	[AD3552R_CH_SELECT] = {
		AD3552R_REG_ADDR_CH_SELECT_16B,
		AD3552R_MASK_CH(0),
		AD3552R_MASK_CH(1)
	},
};

static const int32_t ad3542r_ch_ranges[][2] = {
	[AD3542R_CH_OUTPUT_RANGE_0__2P5V]	= {0, 2500},
	[AD3542R_CH_OUTPUT_RANGE_0__5V]		= {0, 5000},
	[AD3542R_CH_OUTPUT_RANGE_0__10V]	= {0, 10000},
	[AD3542R_CH_OUTPUT_RANGE_NEG_5__5V]	= {-5000, 5000},
	[AD3542R_CH_OUTPUT_RANGE_NEG_2P5__7P5V]	= {-2500, 7500}
};

static const int32_t ad3552r_ch_ranges[][2] = {
	[AD3552R_CH_OUTPUT_RANGE_0__2P5V]	= {0, 2500},
	[AD3552R_CH_OUTPUT_RANGE_0__5V]		= {0, 5000},
	[AD3552R_CH_OUTPUT_RANGE_0__10V]	= {0, 10000},
	[AD3552R_CH_OUTPUT_RANGE_NEG_5__5V]	= {-5000, 5000},
	[AD3552R_CH_OUTPUT_RANGE_NEG_10__10V]	= {-10000, 10000}
};

static const int32_t gains_scaling_table[] = {
	[AD3552R_CH_GAIN_SCALING_1]		= 1000,
	[AD3552R_CH_GAIN_SCALING_0_5]		= 500,
	[AD3552R_CH_GAIN_SCALING_0_25]		= 250,
	[AD3552R_CH_GAIN_SCALING_0_125]		= 125
};

static const uint16_t ad3552r_chip_ids[] = {
	[AD3541R_ID] = 0x400B,
	[AD3542R_ID] = 0x4009,
	[AD3551R_ID] = 0x400A,
	[AD3552R_ID] = 0x4008,
};

uint8_t ad3552r_reg_len(uint8_t addr)
{
	switch (addr) {
	case AD3552R_REG_ADDR_HW_LDAC_16B:
	case AD3552R_REG_ADDR_CH_SELECT_16B:
	case AD3552R_REG_ADDR_SW_LDAC_16B:
	case AD3552R_REG_ADDR_HW_LDAC_24B:
	case AD3552R_REG_ADDR_CH_SELECT_24B:
	case AD3552R_REG_ADDR_SW_LDAC_24B:
		return 1;
	default:
		break;
	}

	if (addr > AD3552R_REG_ADDR_HW_LDAC_24B)
		return 3;
	if (addr > AD3552R_REG_ADDR_HW_LDAC_16B)
		return 2;

	return 1;
}

static inline int32_t _ad3552r_get_reg_attr(struct ad3552r_desc *desc,
		uint32_t attr,
		uint16_t *val)
{
	int32_t err;

	err = ad3552r_read_reg(desc, AD3552R_ATTR_REG(attr), val);
	*val = no_os_field_get(AD3552R_ATTR_MASK(attr), *val);

	return err;
}

static int32_t _ad3552r_update_reg_field(struct ad3552r_desc *desc,
		uint8_t addr, uint32_t mask,
		uint32_t val)
{
	int32_t err;
	uint16_t reg;

	err = ad3552r_read_reg(desc, addr, &reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	reg = (reg & ~mask) | no_os_field_prep(mask, val);

	return ad3552r_write_reg(desc, addr, reg);
}

static inline int32_t _ad3552r_set_reg_attr(struct ad3552r_desc *desc,
		uint32_t attr, uint16_t val)
{
	return _ad3552r_update_reg_field(desc, AD3552R_ATTR_REG(attr),
					 AD3552R_ATTR_MASK(attr), val);
}

/* Update spi interface configuration if needed */
static int32_t _update_spi_cfg(struct ad3552r_desc *desc,
			       struct ad3552_transfer_config *cfg)
{
	int32_t err;
	uint8_t old_keep;

	err = 0;
	if (desc->spi_cfg.addr_asc != cfg->addr_asc) {
		err = _ad3552r_set_reg_attr(desc, AD3552R_ADDR_ASCENSION,
					    cfg->addr_asc);
		desc->spi_cfg.addr_asc = cfg->addr_asc;
	}
	if (desc->spi_cfg.single_instr != cfg->single_instr) {
		err = _ad3552r_set_reg_attr(desc, AD3552R_SINGLE_INST,
					    cfg->single_instr);
		desc->spi_cfg.single_instr = cfg->single_instr;
	}
	old_keep = desc->spi_cfg.stream_length_keep_value;
	if (desc->spi_cfg.stream_length_keep_value !=
	    cfg->stream_length_keep_value) {
		err = _ad3552r_set_reg_attr(desc,
					    AD3552R_STREAM_LENGTH_KEEP_VALUE,
					    cfg->stream_length_keep_value);
		desc->spi_cfg.stream_length_keep_value =
			cfg->stream_length_keep_value;
	}
	if (desc->spi_cfg.stream_mode_length != cfg->stream_mode_length ||
	    old_keep == 0) {
		if (!(old_keep == 0 && cfg->stream_mode_length == 0))
			err = _ad3552r_set_reg_attr(desc, AD3552R_STREAM_MODE,
						    cfg->stream_mode_length);
		desc->spi_cfg.stream_mode_length = cfg->stream_mode_length;
	}
#ifdef AD3552R_QSPI_IMPLEMENTED
	if (desc->spi_cfg.multi_io_mode != cfg->multi_io_mode) {
		err = _ad3552r_set_reg_attr(desc, AD3552R_SPI_MULTI_IO_MODE,
					    cfg->multi_io_mode);
		desc->spi_cfg.multi_io_mode = cfg->multi_io_mode;
	}
	if (desc->spi_cfg.ddr != cfg->ddr) {
		err = _ad3552r_set_reg_attr(desc, AD3552R_SPI_DATA_RATE,
					    cfg->ddr);
		desc->spi_cfg.ddr = cfg->ddr;
	}
	if (desc->spi_cfg.synchronous != cfg->synchronous) {
		err = _ad3552r_set_reg_attr(desc,
					    AD3552R_SPI_SYNCHRONOUS_ENABLE,
					    cfg->synchronous);
		desc->spi_cfg.synchronous = cfg->synchronous;
	}
#endif
	return err;
}

static int32_t _ad3552r_single_transfer_with_crc(struct ad3552r_desc *desc,
		struct ad3552_transfer_data *data,
		uint8_t instr)
{
	struct no_os_spi_msg msg;
	uint32_t i;
	int32_t inc, err;
	uint8_t out[AD3552R_MAX_REG_SIZE + 4];
	uint8_t addr, reg_len, crc_init, crc = 0;
	uint8_t *pbuf;
	int8_t sign;

	msg.rx_buff = out;
	msg.tx_buff = out;
	sign = desc->spi_cfg.addr_asc ? 1 : -1;

	inc = 0;
	i = 0;
	do {
		/* Get next address to for which CRC value will be calculated */
		if (desc->spi_cfg.stream_mode_length) {
			addr = data->addr
			       + (inc % desc->spi_cfg.stream_mode_length);
		} else
			addr = data->addr + inc;

		addr %= (AD3552R_REG_ADDR_MAX + 1);
		reg_len = ad3552r_reg_len(addr);
		addr |= (instr & AD3552R_READ_BIT);

		/* Prepare CRC to send */
		msg.bytes_number = reg_len + 1;
		crc_init = no_os_crc8(desc->crc_table, &addr, 1,
				      AD3552R_CRC_SEED);

		if (data->is_read && i > 0) {
			/* CRC is not needed for continuous read transaction */
			memset(out, 0xFF, reg_len + 1);
		} else {
			pbuf = out;
			/* Take in consideration instruction for CRC */
			pbuf[0] = addr;
			++pbuf;
			++msg.bytes_number;

			memcpy(pbuf, data->data + i, reg_len);
			crc = no_os_crc8(desc->crc_table, pbuf,
					 reg_len, crc_init);
			pbuf[reg_len] = crc;
		}

		err = no_os_spi_transfer(desc->spi, &msg, 1);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		/* Check received CRC */
		if (data->is_read) {
			pbuf = out;
			if (i == 0)
				pbuf++;
			/* Save received data */
			memcpy(data->data + i, pbuf, reg_len);
			if (pbuf[reg_len] !=
			    no_os_crc8(desc->crc_table,
				       pbuf, reg_len, crc_init))
				return -EBADMSG;
		} else {
			if (crc != out[reg_len + 1])
				return -EBADMSG;
		}
		inc += sign * reg_len;
		i += reg_len;
	} while (i < data->len);

	return 0;
}

static int32_t _ad3552r_transfer_with_crc(struct ad3552r_desc *desc,
		struct ad3552_transfer_data *data,
		uint8_t instr)
{
	struct no_os_spi_msg msg;
	uint32_t i;
	int32_t inc, err;
	uint8_t out[AD3552R_MAX_REG_SIZE + 2], in[AD3552R_MAX_REG_SIZE + 2];
	uint8_t addr, reg_len, crc_init;
	uint8_t *pbuf;
	int8_t sign;

	msg.rx_buff = in;
	msg.tx_buff = out;
	sign = desc->spi_cfg.addr_asc ? 1 : -1;
	inc = 0;
	i = 0;
	do {
		/* Get next address to for which CRC value will be calculated */
		if (desc->spi_cfg.stream_mode_length)
			addr = data->addr
			       + (inc % desc->spi_cfg.stream_mode_length);
		else
			addr = data->addr + inc;
		addr %= AD3552R_REG_ADDR_MAX;
		reg_len = ad3552r_reg_len(addr);

		/* Prepare CRC to send */
		msg.bytes_number = reg_len + 1;
		if (i > 0)
			crc_init = addr;
		else
			crc_init = no_os_crc8(desc->crc_table, &instr, 1,
					      AD3552R_CRC_SEED);

		if (data->is_read && i > 0) {
			/* CRC is not needed for continuous read transaction */
			memset(out, 0xFF, reg_len + 1);
		} else {
			pbuf = out;
			if (i == 0) {
				/* Take in consideration instruction for CRC */
				pbuf[0] = instr;
				++pbuf;
				++msg.bytes_number;
			}
			memcpy(pbuf, data->data + i, reg_len);
			pbuf[reg_len] = no_os_crc8(desc->crc_table, pbuf, reg_len,
						   crc_init);
		}

		/* Send message */
		msg.cs_change = !(i + reg_len == data->len);
		err = no_os_spi_transfer(desc->spi, &msg, 1);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		/* Check received CRC */
		if (data->is_read) {
			pbuf = in;
			if (i == 0)
				pbuf++;
			/* Save received data */
			memcpy(data->data + i, pbuf, reg_len);
			if (pbuf[reg_len] !=
			    no_os_crc8(desc->crc_table, pbuf, reg_len, crc_init))
				return -EBADMSG;
		} else {
			if (in[reg_len + (i == 0)] != out[reg_len + (i == 0)])
				return -EBADMSG;
		}
		inc += sign * reg_len;
		i += reg_len;
	} while (i < data->len);

	return 0;
}

/*
 * The xilinx_spi (PS) driver does not handle cs_change, that should be off
 * between address (msg 0) and data (msg 1),
 * so, converting data transfer in a single spi message.
 */
static inline void ad3552r_convert_to_single_msg(uint8_t *data, int len)
{
	int i;

	for (i = len; i; i--)
		data[i] = data[i - 1];
}

int32_t ad3552r_single_transfer(struct ad3552r_desc *desc,
				struct ad3552_transfer_data *data)
{
	struct no_os_spi_msg msgs[2] = { 0 };
	uint8_t instr;
	int err;

	if (!desc || !data)
		return -EINVAL;

	if (data->spi_cfg)
		_update_spi_cfg(desc, data->spi_cfg);

	instr = data->addr & AD3552R_ADDR_MASK;
	instr |= data->is_read ? AD3552R_READ_BIT : 0;

	if (desc->crc_en) {
		if (desc->single_transfer) {
			return _ad3552r_single_transfer_with_crc(desc,
					data, instr);
		} else {
			return _ad3552r_transfer_with_crc(desc, data, instr);
		}
	}
	/*
	 * Xilinx PS spi controller driver is actually not supporting the
	 * cs_change between messages.
	 * Also, Xilinx spi controller driver pretend same buffer for
	 * out and in.
	 */
	ad3552r_convert_to_single_msg(data->data,  data->len);

	data->data[0] = instr;
	msgs[0].bytes_number = data->len + 1;
	msgs[0].rx_buff = data->data;
	msgs[0].tx_buff = data->data;

	err= no_os_spi_transfer(desc->spi, &msgs[0], 1);

	if (data->is_read)
		data->data[0] = data->data[1];

	return err;
}

/* SPI transfer to device */
int32_t ad3552r_transfer(struct ad3552r_desc *desc,
			 struct ad3552_transfer_data *data)
{
	struct no_os_spi_msg msgs[2] = { 0 };
	uint8_t instr;

	if (!desc || !data)
		return -EINVAL;

	if (data->spi_cfg)
		_update_spi_cfg(desc, data->spi_cfg);

	instr = data->addr & AD3552R_ADDR_MASK;
	instr |= data->is_read ? AD3552R_READ_BIT : 0;

	if (desc->crc_en)
		return _ad3552r_transfer_with_crc(desc, data, instr);

	msgs[0].tx_buff = &instr;
	msgs[0].bytes_number = 1;
	msgs[1].bytes_number = data->len;
	msgs[1].cs_change = true;
	if (data->is_read)
		msgs[1].rx_buff = data->data;
	else
		msgs[1].tx_buff = data->data;

	return no_os_spi_transfer(desc->spi, msgs, NO_OS_ARRAY_SIZE(msgs));
}

int32_t ad3552r_write_reg(struct ad3552r_desc *desc, uint8_t addr,
			  uint16_t val)
{
	struct ad3552_transfer_data msg = { 0 };
	uint8_t buf[AD3552R_MAX_REG_SIZE] = { 0 };
	uint8_t reg_len;
	int32_t err;

	if (!desc)
		return -ENODEV;

	reg_len = ad3552r_reg_len(addr);
	if (reg_len == 0 ||
	    (addr >= AD3552R_SECONDARY_REGION_ADDR && desc->spi_cfg.addr_asc))
		return -EINVAL;

	msg.is_read = 0;
	msg.addr = addr;
	msg.data = buf;
	msg.len = reg_len;
	if (reg_len == 1)
		buf[0] = val & 0xFF;
	else
		/* reg_len can be 2 or 3, but 3rd bytes needs to be set to 0 */
		no_os_put_unaligned_be16(val, buf);

	if (desc->single_transfer)
		err = ad3552r_single_transfer(desc, &msg);
	else
		err = ad3552r_transfer(desc, &msg);

	return err;
}

int32_t ad3552r_read_reg(struct ad3552r_desc *desc, uint8_t addr, uint16_t *val)
{
	struct ad3552_transfer_data msg = { 0 };
	uint8_t buf[AD3552R_MAX_REG_SIZE] = { 0 };
	uint8_t reg_len;
	int32_t err;

	if (!desc || !val)
		return -ENODEV;

	reg_len = ad3552r_reg_len(addr);
	if (reg_len == 0 ||
	    (addr >= AD3552R_SECONDARY_REGION_ADDR && desc->spi_cfg.addr_asc))
		return -EINVAL;

	msg.is_read = 1;
	msg.addr = addr;
	msg.data = buf;
	msg.len = reg_len;
	if (desc->single_transfer)
		err = ad3552r_single_transfer(desc, &msg);
	else
		err = ad3552r_transfer(desc, &msg);

	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	if (reg_len == 1)
		*val = buf[0];
	else
		*val = no_os_get_unaligned_be16(buf);

	return 0;
}

static int32_t _ad3552r_get_crc_enable(struct ad3552r_desc *desc, uint16_t *en)
{
	int32_t err;
	uint16_t reg;

	err = _ad3552r_get_reg_attr(desc, AD3552R_CRC_ENABLE, &reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	if (reg == AD3552R_CRC_ENABLE_VALUE)
		*en = 1;
	else if (reg == AD3552R_CRC_DISABLE_VALUE)
		*en = 0;
	else
		/* Unexpected value */
		return -1;

	return 0;
}

static int32_t _ad3552r_set_crc_enable(struct ad3552r_desc *desc, uint16_t en)
{
	int32_t err;
	uint16_t reg;

	reg = en ? AD3552R_CRC_ENABLE_VALUE : AD3552R_CRC_DISABLE_VALUE;
	err = ad3552r_write_reg(desc, AD3552R_ATTR_REG(AD3552R_CRC_ENABLE),
				reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	desc->crc_en = en;

	return 0;
}

int32_t ad3552r_get_dev_value(struct ad3552r_desc *desc,
			      enum ad3552r_dev_attributes attr,
			      uint16_t *val)
{
	if (!desc || !val)
		return -EINVAL;

	if (attr == AD3552R_CRC_ENABLE)
		return  _ad3552r_get_crc_enable(desc, val);

	return _ad3552r_get_reg_attr(desc, attr, val);

#if AD3552R_QSPI_IMPLEMENTED
	switch (attr) {
	case AD3552R_SPI_MULTI_IO_MODE:
	case AD3552R_SPI_DATA_RATE:
	case AD3552R_SPI_SYNCHRONOUS_ENABLE:
		/* TODO. Only standard spi implemented for the moment */
		//return -EINVAL;
		*val = 0;
		return -EINVAL;
	}
	return 0;
#endif
}

int32_t ad3552r_set_dev_value(struct ad3552r_desc *desc,
			      enum ad3552r_dev_attributes attr,
			      uint16_t val)
{
	if (!desc)
		return -EINVAL;

	switch (attr) {
#if AD3552R_QSPI_IMPLEMENTED
	case AD3552R_SPI_MULTI_IO_MODE:
	case AD3552R_SPI_DATA_RATE:
	case AD3552R_SPI_SYNCHRONOUS_ENABLE:
		/* TODO. Only standard SPI implemented for the moment */
		return -EINVAL;
#endif
	case AD3552R_CRC_ENABLE:
		return _ad3552r_set_crc_enable(desc, val);
	default:
		return _ad3552r_set_reg_attr(desc, attr, val);
	}

	return 0;
}

uint8_t ad3552r_get_code_reg_addr(uint8_t ch, uint8_t is_dac,
				  uint8_t is_fast)
{
	if (is_dac) {
		if (is_fast)
			return AD3552R_REG_ADDR_CH_DAC_16B(ch);

		return AD3552R_REG_ADDR_CH_DAC_24B(ch);
	}

	/* Input regs */
	if (is_fast)
		return AD3552R_REG_ADDR_CH_INPUT_16B(ch);

	return AD3552R_REG_ADDR_CH_INPUT_24B(ch);
}

static int32_t _ad3552r_set_code_value(struct ad3552r_desc *desc,
				       uint8_t ch,
				       uint16_t val)
{
	uint8_t addr;

	addr = ad3552r_get_code_reg_addr(ch, 1, desc->ch_data[ch].fast_en);
	return ad3552r_write_reg(desc, addr, val);
}

int32_t ad3552r_simulatneous_update_enable(struct ad3552r_desc *desc)
{
	if (desc->ch_data[0].fast_en != desc->ch_data[1].fast_en)
		return -EINVAL;
	if (desc->is_simultaneous) {
		if (desc->ch_data[0].fast_en)
			return ad3552r_write_reg(desc, AD3552R_REG_ADDR_CH_SELECT_16B,
						 AD3552R_BOTH_CH_SELECT);
		return ad3552r_write_reg(desc, AD3552R_REG_ADDR_CH_SELECT_24B,
					 AD3552R_BOTH_CH_SELECT);
	}
	if (desc->ch_data[0].fast_en)
		return ad3552r_write_reg(desc, AD3552R_REG_ADDR_CH_SELECT_16B,
					 AD3552R_BOTH_CH_DESELECT);
	return ad3552r_write_reg(desc, AD3552R_REG_ADDR_CH_SELECT_24B,
				 AD3552R_BOTH_CH_DESELECT);
}

static int32_t ad3552r_write_simulatneously(struct ad3552r_desc *desc,
		uint8_t ch,
		uint16_t val)
{
	if (desc->ch_data[0].fast_en != desc->ch_data[1].fast_en)
		return -EINVAL;
	if (desc->ch_data[ch].fast_en)
		return ad3552r_write_reg(desc, AD3552R_REG_ADDR_DAC_PAGE_MASK_16B, val);
	return ad3552r_write_reg(desc, AD3552R_REG_ADDR_DAC_PAGE_MASK_24B, val);
}

static int32_t _ad3552r_get_code_value(struct ad3552r_desc *desc,
				       uint8_t ch,
				       uint16_t *val)
{
	int32_t err;
	uint8_t addr;

	addr = ad3552r_get_code_reg_addr(ch, 1, desc->ch_data[ch].fast_en);
	err = ad3552r_read_reg(desc, addr, val);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	return 0;
}


static void ad3552r_get_custom_range(struct ad3552r_desc *dac, uint8_t i,
				     int32_t *v_min, int32_t *v_max)
{
	int64_t vref, tmp, common, offset, gn, gp, offset_polarity=0;
	/*
	 * From datasheet formula (In Volts):
	 *	Vmin = 2.5 + [((CH_Offset * Offset_polarity) / 1024 - GainP) * 1.6 * Rfb]
	 *	Vmax = 2.5 + [((CH_Offset * Offset_polarity) / 1024 + GainN) * 1.6 * Rfb]
	 * Calculus are converted to milivolts
	 */
	if (dac->ch_data[i].offset_polarity)
		offset_polarity = -1;
	else
		offset_polarity = 1;

	vref = 2500;
	common = 1.6 * dac->ch_data[i].rfb;
	offset = dac->ch_data[i].offset;

	gn = gains_scaling_table[dac->ch_data[i].n];
	tmp = (1024 * gn + AD3552R_GAIN_SCALE * offset * offset_polarity) * common;
	tmp = tmp / (1024 * AD3552R_GAIN_SCALE);
	*v_max = vref + tmp;

	gp = gains_scaling_table[dac->ch_data[i].p];
	tmp = (1024 * gp - AD3552R_GAIN_SCALE * offset * offset_polarity) * common;
	tmp = tmp / (1024 * AD3552R_GAIN_SCALE);
	*v_min = vref - tmp;
}

static void ad3552r_calc_gain_and_offset(struct ad3552r_desc *dac, uint8_t ch)
{
	int32_t idx, v_max, v_min, span;
	int64_t tmp, rem;

	if (dac->ch_data[ch].range_override) {
		ad3552r_get_custom_range(dac, ch, &v_min, &v_max);
	} else {
		/* Normal range */
		idx = dac->ch_data[ch].range;
		if (dac->chip_id == AD3542R_ID || dac->chip_id == AD3541R_ID) {
			v_min = ad3542r_ch_ranges[idx][0];
			v_max = ad3542r_ch_ranges[idx][1];
		} else {
			v_min = ad3552r_ch_ranges[idx][0];
			v_max = ad3552r_ch_ranges[idx][1];
		}
	}

	/*
	 * From datasheet formula:
	 *	Vout = Span * (D / 65536) + Vmin
	 * Converted to scale and offset:
	 *	Scale = Span / 65536
	 *	Offset = 65536 * Vmin / Span
	 *
	 * Reminders are in micros in order to be printed as
	 * IIO_VAL_INT_PLUS_MICRO
	 */
	span = v_max - v_min;
	rem = span % 65536;
	dac->ch_data[ch].scale_int = span / 65536;
	/* Do operations in microvolts */
	dac->ch_data[ch].scale_dec = NO_OS_DIV_ROUND_CLOSEST(rem * 1000000, 65536);

	tmp = v_min * 65536;
	rem = tmp % span;
	dac->ch_data[ch].offset_int = tmp / span;
	dac->ch_data[ch].offset_dec = (rem * 1000000) / span;
}

static int32_t _ad3552r_set_gain_value(struct ad3552r_desc *desc,
				       enum ad3552r_ch_attributes attr,
				       uint8_t ch,
				       uint16_t val)
{
	uint16_t reg_mask, reg;
	int32_t err;

	err = ad3552r_read_reg(desc,  AD3552R_REG_ADDR_CH_GAIN(ch), &reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	switch (attr) {
	case AD3552R_CH_GAIN_OFFSET:
		if (val > 0x1FF)
			return -EINVAL;
		desc->ch_data[ch].offset = val;
		err = ad3552r_write_reg(desc, AD3552R_REG_ADDR_CH_OFFSET(ch),
					val & AD3552R_MASK_CH_OFFSET_BITS_0_7);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
		val >>= 8;
		reg_mask = AD3552R_MASK_CH_OFFSET_BIT_8;
		break;
	case AD3552R_CH_RANGE_OVERRIDE:
		desc->ch_data[ch].range_override = !!val;
		reg_mask = AD3552R_MASK_CH_RANGE_OVERRIDE;
		break;
	case AD3552R_CH_GAIN_OFFSET_POLARITY:
		desc->ch_data[ch].offset_polarity = !!val;
		reg_mask = AD3552R_MASK_CH_OFFSET_POLARITY;
		break;
	case AD3552R_CH_GAIN_SCALING_P:
		if (val > 3)
			return -EINVAL;
		desc->ch_data[ch].p = val;
		reg_mask = AD3552R_MASK_CH_GAIN_SCALING_P;
		break;
	case AD3552R_CH_GAIN_SCALING_N:
		if (val > 3)
			return -EINVAL;
		desc->ch_data[ch].n = val;
		reg_mask = AD3552R_MASK_CH_GAIN_SCALING_N;
		break;
	default:
		return -EINVAL;
	}
	reg = (reg & ~reg_mask) | no_os_field_prep(reg_mask, val);

	err = ad3552r_write_reg(desc, AD3552R_REG_ADDR_CH_GAIN(ch), reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	ad3552r_calc_gain_and_offset(desc, ch);

	return 0;
}

static int32_t _ad3552r_get_gain_value(struct ad3552r_desc *desc,
				       enum ad3552r_ch_attributes attr,
				       uint8_t ch,
				       uint16_t *val)
{
	int32_t err;
	uint16_t reg_mask, reg;

	err = ad3552r_read_reg(desc, AD3552R_REG_ADDR_CH_GAIN(ch), &reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	switch (attr) {
	case AD3552R_CH_GAIN_OFFSET:
		*val = no_os_field_get(AD3552R_MASK_CH_OFFSET_BIT_8, reg);
		*val = *val << 8;
		err = ad3552r_read_reg(desc, AD3552R_REG_ADDR_CH_OFFSET(ch), &reg);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
		*val |= reg;
		*val &= 0x1FF;
		return 0;
	case AD3552R_CH_RANGE_OVERRIDE:
		reg_mask = AD3552R_MASK_CH_RANGE_OVERRIDE;
		break;
	case AD3552R_CH_GAIN_OFFSET_POLARITY:
		reg_mask = AD3552R_MASK_CH_OFFSET_POLARITY;
		break;
	case AD3552R_CH_GAIN_SCALING_P:
		reg_mask = AD3552R_MASK_CH_GAIN_SCALING_P;
		break;
	case AD3552R_CH_GAIN_SCALING_N:
		reg_mask = AD3552R_MASK_CH_GAIN_SCALING_N;
		break;
	default:
		return -EINVAL;
	}
	*val = no_os_field_get(reg_mask, reg);

	return 0;
}

int32_t ad3552r_get_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t *val)
{
	int32_t err;
	uint16_t reg;
	uint8_t addr;

	if (!desc || !val)
		return -EINVAL;

	/* Attributes not defined in addr_mask_map_ch */
	switch (attr) {
	case AD3552R_CH_FAST_EN:
		*val = desc->ch_data[ch].fast_en;
		return 0;
	case AD3552R_CH_CODE:
		return _ad3552r_get_code_value(desc, ch, val);
	case AD3552R_CH_RFB:
		*val = desc->ch_data[ch].rfb;
		return 0;
	default:
		break;
	}

	if (attr >= AD3552R_CH_RANGE_OVERRIDE &&
	    attr <= AD3552R_CH_GAIN_SCALING_N)
		return _ad3552r_get_gain_value(desc, attr, ch, val);

	addr = AD3552R_CH_ATTR_REG(attr);
	if (addr == AD3552R_REG_ADDR_SW_LDAC_24B ||
	    addr == AD3552R_REG_ADDR_SW_LDAC_16B) {
		pr_debug("Write only registers\n");
		/* LDAC are write only registers */
		return -EINVAL;
	}

	err = ad3552r_read_reg(desc, addr, &reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	*val = no_os_field_get(AD3552R_CH_ATTR_MASK(ch, attr), reg);

	return 0;
}

int32_t ad3552r_set_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t val)
{
	int32_t err;

	if (!desc)
		return -EINVAL;

	/* Attributes not defined in addr_mask_map_ch */
	switch (attr) {
	case AD3552R_CH_FAST_EN:
		desc->ch_data[ch].fast_en = !!val;
		return 0;
	case AD3552R_CH_CODE:
		if (desc->is_simultaneous)
			return ad3552r_write_simulatneously(desc, ch,val);
		return _ad3552r_set_code_value(desc, ch, val);
	case AD3552R_CH_RFB:
		desc->ch_data[ch].rfb = val;
		ad3552r_calc_gain_and_offset(desc, ch);
		return 0;
	default:
		break;
	}

	if (attr >= AD3552R_CH_RANGE_OVERRIDE &&
	    attr <= AD3552R_CH_GAIN_SCALING_N)
		return _ad3552r_set_gain_value(desc, attr, ch, val);

	/* Update register related to attributes in chip */
	err = _ad3552r_update_reg_field(desc, AD3552R_CH_ATTR_REG(attr),
					AD3552R_CH_ATTR_MASK(ch, attr), val);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	/* Update software structures */
	if (attr == AD3552R_CH_OUTPUT_RANGE_SEL) {
		val %= AD3552R_CH_OUTPUT_RANGE_NEG_10__10V + 1;
		desc->ch_data[ch].range = val;
		ad3552r_calc_gain_and_offset(desc, ch);
	}

	return err;
}

static int32_t ad3552r_check_scratch_pad(struct ad3552r_desc *desc)
{
	int32_t err;
	const uint16_t val1 = AD3552R_SCRATCH_PAD_TEST_VAL1;
	const uint16_t val2 = AD3552R_SCRATCH_PAD_TEST_VAL2;
	uint16_t val;

	err = ad3552r_write_reg(desc, AD3552R_REG_ADDR_SCRATCH_PAD, val1);
	if (err < 0)
		return err;

	err = ad3552r_read_reg(desc, AD3552R_REG_ADDR_SCRATCH_PAD, &val);
	if (err < 0)
		return err;

	if (val1 != val)
		return -ENODEV;

	err = ad3552r_write_reg(desc, AD3552R_REG_ADDR_SCRATCH_PAD, val2);
	if (err < 0)
		return err;

	err = ad3552r_read_reg(desc, AD3552R_REG_ADDR_SCRATCH_PAD, &val);
	if (err < 0)
		return err;

	if (val2 != val)
		return -ENODEV;

	return 0;
}

int32_t ad3552r_get_scale(struct ad3552r_desc *desc, uint8_t ch,
			  int32_t *integer, int32_t *dec)
{
	if (!integer || !desc || ch >= AD3552R_MAX_NUM_CH)
		return -EINVAL;

	*integer = desc->ch_data[ch].scale_int;
	*dec = desc->ch_data[ch].scale_dec;

	return 0;
}

int32_t ad3552r_get_offset(struct ad3552r_desc *desc, uint8_t ch,
			   int32_t *integer, int32_t *dec)
{
	if (!integer || !desc || ch >= AD3552R_MAX_NUM_CH)
		return -EINVAL;

	*integer = desc->ch_data[ch].offset_int;
	*dec = desc->ch_data[ch].offset_dec;

	return 0;
}

static int32_t ad3552r_config_custom_gain(struct ad3552r_desc *desc,
		struct ad3552r_custom_output_range_cfg *cfg, uint8_t channel_num)
{
	int32_t err;

	err = ad3552r_set_ch_value(desc, AD3552R_CH_RANGE_OVERRIDE, channel_num, 1);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	err = ad3552r_set_ch_value(desc, AD3552R_CH_GAIN_OFFSET_POLARITY, channel_num,
				   cfg->gain_offset < 0);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Error setting gain offset\n");
		return err;
	}

	err = ad3552r_set_ch_value(desc, AD3552R_CH_GAIN_OFFSET, channel_num,
				   abs(cfg->gain_offset));
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Error setting scaling_p\n");
		return err;
	}

	err = ad3552r_set_ch_value(desc, AD3552R_CH_GAIN_SCALING_P, channel_num,
				   cfg->gain_scaling_p_inv_log2);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Error setting scaling p\n");
		return err;
	}

	err = ad3552r_set_ch_value(desc, AD3552R_CH_GAIN_SCALING_N, channel_num,
				   cfg->gain_scaling_n_inv_log2);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Error setting scaling n\n");
		return err;
	}

	err = ad3552r_set_ch_value(desc, AD3552R_CH_RFB, channel_num, cfg->rfb_ohms);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Error setting RFB\n");
		return err;
	}

	return 0;
}

static int32_t ad3552r_configure_device(struct ad3552r_desc *desc,
					struct ad3552r_init_param *param)
{
	int32_t err;
	uint16_t val;
	uint8_t range, i;

	if (param->use_external_vref)
		val = AD3552R_EXTERNAL_VREF_PIN_INPUT;
	else if (param->vref_out_enable)
		val = AD3552R_INTERNAL_VREF_PIN_2P5V;
	else
		val = AD3552R_INTERNAL_VREF_PIN_FLOATING;

	err = ad3552r_set_dev_value(desc, AD3552R_VREF_SELECT, val);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	if (param->sdo_drive_strength > 3) {
		pr_err("sdo_drive_strength should be less than 4");
		return -EINVAL;
	}
	err = ad3552r_set_dev_value(desc, AD3552R_SDO_DRIVE_STRENGTH,
				    param->sdo_drive_strength);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	for (i = 0; i < AD3552R_MAX_CH_NUM(desc->chip_id); ++i) {
		range = param->channels[i].range;
		if (param->channels[i].en) {
			desc->ch_data[i].fast_en = param->channels[i].fast_en;
			if (range != AD3552R_CH_OUTPUT_RANGE_CUSTOM) {
				if (range > AD3552R_CH_OUTPUT_RANGE_NEG_10__10V) {
					pr_err("Invalid range for channel %"PRIu16"\n", i);
					return -EINVAL;
				}
				desc->ch_data[i].range = range;
				err = ad3552r_set_ch_value(desc, AD3552R_CH_OUTPUT_RANGE_SEL,
							   i, range);
				if (NO_OS_IS_ERR_VALUE(err))
					return err;
			} else {
				err = ad3552r_config_custom_gain(desc,
								 &param->channels[i].custom_range,i);
				if (NO_OS_IS_ERR_VALUE(err)) {
					pr_err("Custom gain configuration failed for channel %"PRIu16"\n", i);
					return err;
				}
			}
		} else {
			if (desc->chip_id == AD3541R_ID || desc->chip_id == AD3542R_ID) {
				err = ad3552r_set_ch_value(desc, AD3552R_CH_AMPLIFIER_POWERDOWN, i, 1);
				if (NO_OS_IS_ERR_VALUE(err))
					return err;
				err = ad3552r_set_ch_value(desc, AD3552R_CH_DAC_POWERDOWN, i, 1);
				if (NO_OS_IS_ERR_VALUE(err))
					return err;
			} else {
				err = ad3552r_set_ch_value(desc, AD3552R_CH_DAC_POWERDOWN, i, 1);
				if (NO_OS_IS_ERR_VALUE(err))
					return err;
			}
		}
	}

	err = no_os_gpio_get_optional(&desc->ldac, param->ldac_gpio_param_optional);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	if (desc->ldac) {
		err = no_os_gpio_direction_output(desc->ldac, NO_OS_GPIO_HIGH);
		if (NO_OS_IS_ERR_VALUE(err)) {
			no_os_gpio_remove(desc->ldac);
			return err;
		}
	}
	return 0;
}

int32_t ad3552r_init(struct ad3552r_desc **desc,
		     struct ad3552r_init_param *param)
{
	struct ad3552r_desc *ldesc;
	int32_t err;
	uint16_t id, val;

	if (!desc || !param)
		return -EINVAL;

	ldesc = (struct ad3552r_desc*)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	err = no_os_spi_init(&ldesc->spi, &param->spi_param);
	if (NO_OS_IS_ERR_VALUE(err))
		goto err;

	no_os_crc8_populate_msb(ldesc->crc_table, AD3552R_CRC_POLY);

	err = no_os_gpio_get_optional(&ldesc->reset,
				      param->reset_gpio_param_optional);
	if (NO_OS_IS_ERR_VALUE(err))
		goto err_spi;

	ldesc->single_transfer = param->single_transfer;

	if (ldesc->reset) {
		err = no_os_gpio_direction_output(ldesc->reset, NO_OS_GPIO_HIGH);
		if (NO_OS_IS_ERR_VALUE(err))
			goto err_reset;
	}

	err = ad3552r_reset(ldesc);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Reset failed: %"PRIi32"\n", err);
		goto err_reset;
	}

	err = ad3552r_set_dev_value(ldesc, AD3552R_CRC_ENABLE, param->crc_en);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Error enabling CRC: %"PRIi32"\n", err);
		goto err_reset;
	}

	err = ad3552r_check_scratch_pad(ldesc);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Scratch pad test failed: %"PRIi32"\n", err);
		goto err_reset;
	}

	/* Check chip ID */
	err = ad3552r_read_reg(ldesc, AD3552R_REG_ADDR_PRODUCT_ID_L, &val);
	if (err) {
		pr_err("Fail read PRODUCT_ID_L: %"PRIi32"\n", err);
		goto err_reset;
	}

	id = val;
	err = ad3552r_read_reg(ldesc, AD3552R_REG_ADDR_PRODUCT_ID_H, &val);
	if (err) {
		pr_err("Fail read PRODUCT_ID_H: %"PRIi32"\n", err);
		goto err_reset;
	}

	id |= val << 8;
	if (id != ad3552r_chip_ids[param->chip_id]) {
		pr_err("Product id not matching\n");
		err = -ENODEV;
		goto err_reset;
	}
	ldesc->chip_id = param->chip_id;
	ldesc->is_simultaneous = param->is_simultaneous;
	err = ad3552r_configure_device(ldesc, param);
	if (NO_OS_IS_ERR_VALUE(err)) {
		err = -ENODEV;
		goto err_reset;
	}

	*desc = ldesc;

	return 0;
err_reset:
	no_os_gpio_remove(ldesc->reset);
err_spi:
	no_os_spi_remove(ldesc->spi);
err:
	no_os_free(ldesc);

	return err;
}

int32_t ad3552r_remove(struct ad3552r_desc *desc)
{
	if (desc->ldac)
		no_os_gpio_remove(desc->ldac);
	if (desc->reset)
		no_os_gpio_remove(desc->reset);
	no_os_spi_remove(desc->spi);
	no_os_free(desc);

	return 0;
}

int32_t ad3552r_reset(struct ad3552r_desc *desc)
{
	uint32_t timeout = 5000;
	int32_t err;
	uint16_t val;
	uint8_t first_check;

	if (desc->reset) {
		no_os_gpio_set_value(desc->reset, NO_OS_GPIO_LOW);
		no_os_mdelay(1);
		no_os_gpio_set_value(desc->reset, NO_OS_GPIO_HIGH);
	} else {
		err = _ad3552r_update_reg_field(desc,
						AD3552R_REG_ADDR_INTERFACE_CONFIG_A,
						AD3552R_MASK_SOFTWARE_RESET,
						AD3552R_MASK_SOFTWARE_RESET);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
	}

	first_check = 0;
	while (true) {
		err = ad3552r_read_reg(desc,
				       AD3552R_REG_ADDR_INTERFACE_CONFIG_B,
				       &val);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		if (!first_check) {
			if (val == AD3552R_DEFAULT_CONFIG_B_VALUE)
				first_check = 1;
		} else if (!(val & AD3552R_MASK_INTERFACE_NOT_READY)) {
			break;
		}

		--timeout;
		if (timeout == 0)
			return -EIO;
	}

	return _ad3552r_set_reg_attr(desc, AD3552R_ADDR_ASCENSION, 0);
}

int32_t ad3552r_ldac_trigger(struct ad3552r_desc *desc, uint16_t mask,
			     uint8_t is_fast)
{
	int32_t err;

	if (!desc->ldac) {
		if (is_fast)
			return ad3552r_write_reg(desc, AD3552R_REG_ADDR_SW_LDAC_16B,
						 mask);
		return ad3552r_write_reg(desc, AD3552R_REG_ADDR_SW_LDAC_24B,
					 mask);
	}
	err = no_os_gpio_set_value(desc->ldac, NO_OS_GPIO_LOW);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	no_os_udelay(AD3552R_LDAC_PULSE_US);
	err = no_os_gpio_set_value(desc->ldac, NO_OS_GPIO_HIGH);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	return 0;
}

int32_t ad3552r_set_asynchronous(struct ad3552r_desc *desc, uint8_t enable)
{
	int err;

	err = no_os_gpio_set_value(desc->ldac,
				   enable ? NO_OS_GPIO_LOW : NO_OS_GPIO_HIGH);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	return 0;
}

static int32_t ad3552r_write_all_channels(struct ad3552r_desc *desc,
		uint16_t *data,
		enum ad3552r_write_mode mode)
{
	struct ad3552_transfer_data msg = {0};
	int32_t err;
	uint8_t buff[AD3552R_MAX_NUM_CH * AD3552R_MAX_REG_SIZE + 1] = { 0 };
	uint8_t len, is_fast;

	is_fast = desc->ch_data[0].fast_en;
	no_os_put_unaligned_be16(data[0], buff);
	len = 2;
	if (!is_fast)
		++len;
	no_os_put_unaligned_be16(data[1], buff + len);
	len += 2;
	if (!is_fast)
		++len;
	if (mode == AD3552R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC)
		if (!desc->ldac)
			buff[len++] = AD3552R_MASK_ALL_CH;

	msg.addr = ad3552r_get_code_reg_addr(1, mode == AD3552R_WRITE_DAC_REGS,
					     is_fast);
	msg.len = len;
	msg.data = buff;

	if (desc->single_transfer)
		err = ad3552r_single_transfer(desc, &msg);
	else
		err = ad3552r_transfer(desc, &msg);

	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	if (mode == AD3552R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC)
		return ad3552r_ldac_trigger(desc, AD3552R_MASK_ALL_CH, is_fast);

	return 0;
}

/*
 *
 * samples: nb of samples per channel
 * ch_mask: mask of channels to enable. Ex. 0b11 (Enable both channels)
 */
int32_t ad3552r_write_samples(struct ad3552r_desc *desc, uint16_t *data,
			      uint32_t samples, uint32_t ch_mask,
			      enum ad3552r_write_mode mode)
{
	uint32_t i;
	int32_t err;
	uint8_t addr, is_dac, ch;

	if (ch_mask == AD3552R_MASK_ALL_CH &&
	    desc->ch_data[0].fast_en != desc->ch_data[1].fast_en)
		/* Unhandled case */
		return -EINVAL;

	if (ch_mask == AD3552R_MASK_ALL_CH) {
		for (i = 0; i < samples; ++i) {
			err = ad3552r_write_all_channels(desc, data + i * 2,
							 mode);
			if (NO_OS_IS_ERR_VALUE(err))
				return err;
		}
		return 0;
	}

	ch = no_os_find_first_set_bit(ch_mask);
	is_dac = (mode == AD3552R_WRITE_DAC_REGS);
	addr = ad3552r_get_code_reg_addr(ch, is_dac, desc->ch_data[ch].fast_en);
	for (i = 0; i < samples; ++i) {
		err = ad3552r_write_reg(desc, addr, data[i]);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		if (mode == AD3552R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC) {
			err = ad3552r_ldac_trigger(desc, ch_mask, desc->ch_data[ch].fast_en);
			if (NO_OS_IS_ERR_VALUE(err))
				return err;
		}
	}

	return 0;
}

#ifdef AD3552R_DEBUG

int32_t ad3552r_get_status(struct ad3552r_desc *desc, uint32_t *status,
			   uint8_t clr_err)
{
	uint32_t _st, new_reg, reg;
	int32_t err;

	err = ad3552r_read_reg(desc, AD3552R_REG_ADDR_INTERFACE_STATUS_A, &reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	_st = 0;
	pr_debug("Status bits:\n");
	new_reg = 0;
	_CHECK_STATUS(_st, reg, new_reg, INTERFACE_NOT_READY, 0);
	_CHECK_STATUS(_st, reg, new_reg, CLOCK_COUNTING_ERROR, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, INVALID_OR_NO_CRC, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, WRITE_TO_READ_ONLY_REGISTER, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, PARTIAL_REGISTER_ACCESS, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, REGISTER_ADDRESS_INVALID, clr_err);
	if (new_reg) {
		err = ad3552r_write_reg(desc,
					AD3552R_REG_ADDR_INTERFACE_STATUS_A,
					reg);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
	}

	err = ad3552r_read_reg(desc, AD3552R_REG_ADDR_ERR_STATUS, &reg);
	if (NO_OS_IS_ERR_VALUE(err))
		return err;

	new_reg = 0;
	_CHECK_STATUS(_st, reg, new_reg, REF_RANGE_ERR_STATUS, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, DUAL_SPI_STREAM_EXCEEDS_DAC_ERR_STATUS,
		      clr_err);
	_CHECK_STATUS(_st, reg, new_reg, MEM_CRC_ERR_STATUS, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, RESET_STATUS, clr_err);
	if (new_reg) {
		err = ad3552r_write_reg(desc, AD3552R_REG_ADDR_ERR_STATUS, reg);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
	}

	return 0;
}

#endif

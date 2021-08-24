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

#include <stdlib.h>
#include <string.h>
#include "ad3552r.h"
#include "error.h"
#include "print_log.h"
#include "util.h"
#include "spi.h"
//#include "irq.h"
//#include "timer.h"
#include "gpio.h"
#include "delay.h"

/* Private attributes */
enum ad3552r_spi_attributes {
	/* Read registers in ascending order if set. Else descending */
	AD3552R_ADDR_ASCENSION = AD3552R_SPI_SYNCHRONOUS_ENABLE + 1,
	/* Single instruction mode if set. Else, stream mode */
	AD3552R_SINGLE_INST,
	/* Number of addresses to loop on when stream writing. */
	AD3552R_STREAM_MODE,
	/* Keep stream value if set. */
	AD3552R_STREAM_LENGTH_KEEP_VALUE,
};

struct ad3552r_ch_data {
	uint16_t	offset : 9;
	uint16_t	range_override : 1;
	uint16_t	n : 2;
	uint16_t	p : 2;
	uint16_t	offset_polarity : 1;
	uint16_t	rfb;
	uint8_t		range;
	uint16_t	code_min;
	uint16_t	code_max;
};

struct ad3552r_desc {
	struct ad3552_transfer_config	spi_cfg;
	struct spi_desc			*spi;
	struct irq_ctrl_desc		*irq_ctrl;
	//struct timer_desc		*tmr;
	struct gpio_desc		*ldac;
	struct ad3552r_ch_data		ch_data[AD3552R_NUM_CH];
	enum ad3552r_update_mode	update_mode;
	enum ad3552r_input_trigger_mode trigger_mode;
	//uint16_t			timer_id;
	uint16_t			timer_intr_nb;
	void				*tmr_extra;
	volatile uint8_t		is_rise_edge;
	uint8_t				(*is_hw_trigger_ready)(void *ctx);
	void				*hw_trigger_ctx;
	uint32_t	active_ch;
	uint32_t	update_period_ns;
	uint8_t		crc_table[CRC8_TABLE_SIZE];
	uint8_t		ldac_val : 1;
	uint8_t		precision_en : 1;
	uint8_t		crc_en : 1;
	uint8_t		vref_sel : 2;
};

#define SEC_TO_10NS(x)		((x) * 100000000)

#define AD3552R_ATTR_REG(attr) addr_mask_map[attr][0]
#define AD3552R_ATTR_MASK(attr) addr_mask_map[attr][1]
#define AD3552R_CH_ATTR_REG(attr) addr_mask_map_ch[attr][0]
#define AD3552R_CH_ATTR_MASK(ch, attr) addr_mask_map_ch[attr][(ch) + 1]

#define AD3552R_MAX_REG_SIZE				3
#define AD3552R_READ_BIT				(1 << 7)
#define AD3552R_ADDR_MASK				(~AD3552R_READ_BIT)
#define AD3552R_CRC_ENABLE_VALUE			(BIT(6) | BIT(1))
#define AD3552R_CRC_DISABLE_VALUE			(BIT(1) | BIT(0))
#define AD3552R_EXTERNAL_VREF_MASK			BIT(1)
#define AD3552R_CRC_POLY				0x07
#define AD3552R_CRC_SEED				0xA5
#define AD3552R_MASK_DAC_12B				0xFFF0
#define AD3552R_SECONDARY_REGION_ADDR			0x28
#define AD3552R_DEFAULT_CONFIG_B_VALUE			0x8
#define AD3552R_DATA_IDX(x)				(1 + (x))
#define AD3552R_DEFAULT_DAC_UPDATE_PERIOD		1000

#define _CHECK_STATUS(_status, reg, new_reg, bit_name, clr_err)  do {\
	if (reg & AD3552R_MASK_ ## bit_name) {\
		new_reg |= clr_err ? AD3552R_MASK_ ## bit_name : 0;\
		_status |= AD3552R_ ## bit_name;\
		pr_debug(" %s\n", # bit_name);\
	}\
} while (0)

/* Repeat multiple byte registers in order to have reg length both on high
and low address */
static const uint8_t reg_addr_size[] = {
	[AD3552R_REG_ADDR_INTERFACE_CONFIG_A]		= 1,
	[AD3552R_REG_ADDR_INTERFACE_CONFIG_B]		= 1,
	[AD3552R_REG_ADDR_DEVICE_CONFIG]		= 1,
	[AD3552R_REG_ADDR_CHIP_TYPE]			= 1,
	[AD3552R_REG_ADDR_PRODUCT_ID_L]			= 1,
	[AD3552R_REG_ADDR_PRODUCT_ID_H]			= 1,
	[AD3552R_REG_ADDR_CHIP_GRADE]			= 1,
	[AD3552R_REG_ADDR_SCRATCH_PAD]			= 1,
	[AD3552R_REG_ADDR_SPI_REVISION]			= 1,
	[AD3552R_REG_ADDR_VENDOR_L]			= 1,
	[AD3552R_REG_ADDR_VENDOR_H]			= 1,
	[AD3552R_REG_ADDR_STREAM_MODE]			= 1,
	[AD3552R_REG_ADDR_TRANSFER_REGISTER]		= 1,
	[AD3552R_REG_ADDR_INTERFACE_CONFIG_C]		= 1,
	[AD3552R_REG_ADDR_INTERFACE_STATUS_A]		= 1,
	[AD3552R_REG_ADDR_INTERFACE_CONFIG_D]		= 1,
	[AD3552R_REG_ADDR_SH_REFERENCE_CONFIG]		= 1,
	[AD3552R_REG_ADDR_ERR_ALARM_MASK]		= 1,
	[AD3552R_REG_ADDR_ERR_STATUS]			= 1,
	[AD3552R_REG_ADDR_POWERDOWN_CONFIG]		= 1,
	[AD3552R_REG_ADDR_CH0_CH1_OUTPUT_RANGE]		= 1,
	[AD3552R_REG_ADDR_CH_OFFSET(0)]			= 1,
	[AD3552R_REG_ADDR_CH_GAIN(0)]			= 1,
	[AD3552R_REG_ADDR_CH_OFFSET(1)]			= 1,
	[AD3552R_REG_ADDR_CH_GAIN(1)]			= 1,
	[AD3552R_REG_ADDR_HW_LDAC_16B]			= 1,
	[AD3552R_REG_ADDR_CH_DAC_16B(0)]		= 2,
	[AD3552R_REG_ADDR_CH_DAC_16B(0) + 1]		= 2,
	[AD3552R_REG_ADDR_CH_DAC_16B(1)]		= 2,
	[AD3552R_REG_ADDR_CH_DAC_16B(1) + 1]		= 2,
	[AD3552R_REG_ADDR_DAC_PAGE_MASK_16B]		= 2,
	[AD3552R_REG_ADDR_DAC_PAGE_MASK_16B + 1]	= 2,
	[AD3552R_REG_ADDR_CH_SELECT_16B]		= 1,
	[AD3552R_REG_ADDR_INPUT_PAGE_MASK_16B]		= 2,
	[AD3552R_REG_ADDR_INPUT_PAGE_MASK_16B + 1]	= 2,
	[AD3552R_REG_ADDR_SW_LDAC_16B]			= 1,
	[AD3552R_REG_ADDR_CH_INPUT_16B(0)]		= 2,
	[AD3552R_REG_ADDR_CH_INPUT_16B(0) + 1]		= 2,
	[AD3552R_REG_ADDR_CH_INPUT_16B(1)]		= 2,
	[AD3552R_REG_ADDR_CH_INPUT_16B(1) + 1]		= 2,
	[AD3552R_REG_ADDR_HW_LDAC_24B]			= 1,
	[AD3552R_REG_ADDR_CH_DAC_24B(0)]		= 3,
	[AD3552R_REG_ADDR_CH_DAC_24B(0) + 2]		= 3,
	[AD3552R_REG_ADDR_CH_DAC_24B(1)]		= 3,
	[AD3552R_REG_ADDR_CH_DAC_24B(1) + 2]		= 3,
	[AD3552R_REG_ADDR_DAC_PAGE_MASK_24B]		= 3,
	[AD3552R_REG_ADDR_DAC_PAGE_MASK_24B + 2]	= 3,
	[AD3552R_REG_ADDR_CH_SELECT_24B]		= 1,
	[AD3552R_REG_ADDR_INPUT_PAGE_MASK_24B]		= 3,
	[AD3552R_REG_ADDR_INPUT_PAGE_MASK_24B + 2]	= 3,
	[AD3552R_REG_ADDR_SW_LDAC_24B]			= 1,
	[AD3552R_REG_ADDR_CH_INPUT_24B(0)]		= 3,
	[AD3552R_REG_ADDR_CH_INPUT_24B(0) + 2]		= 3,
	[AD3552R_REG_ADDR_CH_INPUT_24B(1)]		= 3,
	[AD3552R_REG_ADDR_CH_INPUT_24B(1) + 2]		= 3,
};

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

static const float ch_ranges[][2] = {
	[AD3552R_CH_OUTPUT_RANGE_0__2_5V]	= {0, 2.5},
	[AD3552R_CH_OUTPUT_RANGE_0__5V]		= {0, 5},
	[AD3552R_CH_OUTPUT_RANGE_0__10V]	= {0, 10},
	[AD3552R_CH_OUTPUT_RANGE_NEG_5__5V]	= {-5, 5},
	[AD3552R_CH_OUTPUT_RANGE_NEG_10__10V]	= {-10, 10}
};

static const uint32_t default_rfbs[] = {
	[AD3552R_CH_OUTPUT_RANGE_0__2_5V]	= 1,
	[AD3552R_CH_OUTPUT_RANGE_0__5V]		= 1,
	[AD3552R_CH_OUTPUT_RANGE_0__10V]	= 2,
	[AD3552R_CH_OUTPUT_RANGE_NEG_5__5V]	= 2,
	[AD3552R_CH_OUTPUT_RANGE_NEG_10__10V]	= 4
};

static const float gains_scaling_table[] = {
	[AD3552R_CH_GAIN_SCALING_1]		= 1.0,
	[AD3552R_CH_GAIN_SCALING_0_5]		= 0.5,
	[AD3552R_CH_GAIN_SCALING_0_25]		= 0.25,
	[AD3552R_CH_GAIN_SCALING_0_125]		= 0.125
};

static inline uint8_t _ad3552r_reg_len(uint8_t addr)
{
	if (addr > AD3552R_REG_ADDR_MAX)
		return 0;
	
	return reg_addr_size[addr];
}

static inline int32_t _ad3552r_write_reg_mask(struct ad3552r_desc *desc,
						uint8_t addr, uint32_t mask,
						uint32_t val)
{
	int32_t ret;
	uint32_t reg;
	uint32_t reg_full_mask;

	reg_full_mask = (1 << (8 * _ad3552r_reg_len(addr))) - 1;
	if (mask == reg_full_mask) {
		reg = val;
	} else {
		ret = ad3552r_read_reg(desc, addr, &reg);
		if (IS_ERR_VALUE(ret))
			return ret;

		reg = (reg & ~mask) | field_prep(mask, val);
	}

	return ad3552r_write_reg(desc, addr, reg);
}

static inline int32_t _ad3552r_read_reg_mask(struct ad3552r_desc *desc,
					uint8_t addr, uint32_t mask,
					uint32_t *val)
{
	int32_t ret;

	ret = ad3552r_read_reg(desc, addr, val);
	*val = field_get(mask, *val);

	return ret;
}

static inline int32_t _ad3552r_get_reg_attr(struct ad3552r_desc *desc,
					    uint32_t attr,
					    uint32_t *val)
{
	return _ad3552r_read_reg_mask(desc, AD3552R_ATTR_REG(attr),
			AD3552R_ATTR_MASK(attr), val);
}

static inline int32_t _ad3552r_set_reg_attr(struct ad3552r_desc *desc,
		    	    	    	     uint32_t attr,
					     uint32_t val)
{
	return _ad3552r_write_reg_mask(desc, AD3552R_ATTR_REG(attr),
			AD3552R_ATTR_MASK(attr), val);
}

/* Update spi interface configuration if needed */
static int32_t _update_spi_cfg(struct ad3552r_desc *desc,
			       struct ad3552_transfer_config *cfg)
{
	int32_t ret;
	uint8_t old_keep;

	ret = SUCCESS;
	if (desc->spi_cfg.addr_asc != cfg->addr_asc) {
		ret = _ad3552r_set_reg_attr(desc, AD3552R_ADDR_ASCENSION,
				      cfg->addr_asc);
		desc->spi_cfg.addr_asc = cfg->addr_asc;
	}
	if (desc->spi_cfg.single_instr != cfg->single_instr) {
		ret = _ad3552r_set_reg_attr(desc, AD3552R_SINGLE_INST,
				      cfg->single_instr);
		desc->spi_cfg.single_instr = cfg->single_instr;
	}
	old_keep = desc->spi_cfg.stream_length_keep_value;
	if (desc->spi_cfg.stream_length_keep_value !=
	    cfg->stream_length_keep_value) {
		ret = _ad3552r_set_reg_attr(desc,
					    AD3552R_STREAM_LENGTH_KEEP_VALUE,
					    cfg->stream_length_keep_value);
		desc->spi_cfg.stream_length_keep_value =
					cfg->stream_length_keep_value;
	}
	if (desc->spi_cfg.stream_mode_length != cfg->stream_mode_length ||
	    old_keep == 0) {
		if (!(old_keep == 0 && cfg->stream_mode_length == 0))
			ret = _ad3552r_set_reg_attr(desc, AD3552R_STREAM_MODE,
						    cfg->stream_mode_length);
		desc->spi_cfg.stream_mode_length = cfg->stream_mode_length;
	}
	if (desc->spi_cfg.multi_io_mode != cfg->multi_io_mode) {
		ret = _ad3552r_set_reg_attr(desc, AD3552R_SPI_MULTI_IO_MODE,
				      cfg->multi_io_mode);
		desc->spi_cfg.multi_io_mode = cfg->multi_io_mode;
	}
	if (desc->spi_cfg.ddr != cfg->ddr) {
		ret = _ad3552r_set_reg_attr(desc, AD3552R_SPI_DATA_RATE,
				      cfg->ddr);
		desc->spi_cfg.ddr = cfg->ddr;
	}
	if (desc->spi_cfg.synchronous != cfg->synchronous) {
		ret = _ad3552r_set_reg_attr(desc,
					    AD3552R_SPI_SYNCHRONOUS_ENABLE,
					    cfg->synchronous);
		desc->spi_cfg.synchronous = cfg->synchronous;
	}

	return ret;
}

/* Transfer data using CRC */
static int32_t _ad3552r_transfer_with_crc(struct ad3552r_desc *desc,
				   struct ad3552_transfer_data *data,
				   uint8_t instr)
{
	struct spi_msg	msg;
	uint8_t		out[AD3552R_MAX_REG_SIZE + 2];
	uint8_t		in[AD3552R_MAX_REG_SIZE + 2];
	uint8_t		*pbuf;
	uint8_t		addr;
	int8_t		sign;
	uint8_t		reg_len;
	int32_t		inc;
	uint32_t	i;
	uint8_t		crc_init;
	int32_t		ret;

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
		reg_len = _ad3552r_reg_len(addr);

		/* Prepare CRC to send */
		msg.bytes_number = reg_len + 1;
		if (i > 0)
			crc_init = addr;
		else
			crc_init = crc8(desc->crc_table, &instr, 1,
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
			pbuf[reg_len] = crc8(desc->crc_table, pbuf, reg_len,
					     crc_init);
		}

		/* Send message */
		msg.cs_change = !(i + reg_len == data->len);
		ret = spi_transfer(desc->spi, &msg, 1);
		if (IS_ERR_VALUE(ret))
			return ret;

		/* Check received CRC */
		if (data->is_read) {
			pbuf = in;
			if (i == 0)
				pbuf++;
			/* Save received data */
			memcpy(data->data + i, pbuf, reg_len);
			if (pbuf[reg_len] !=
			    crc8(desc->crc_table, pbuf, reg_len, crc_init))
				return -EBADMSG;
		} else {
			if (in[reg_len + (i == 0)] != out[reg_len + (i == 0)])
				return -EBADMSG;
		}
		inc += sign * reg_len;
		i += reg_len;
	} while (i < data->len);

	return SUCCESS;
}

/* SPI transfer to device */
int32_t ad3552r_transfer(struct ad3552r_desc *desc,
			     struct ad3552_transfer_data *data)
{
	struct spi_msg	msgs[2] = { 0 };
	uint8_t		instr;

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
	if (data->is_read)
		msgs[1].rx_buff = data->data;
	else
		msgs[1].tx_buff = data->data;

	return spi_transfer(desc->spi, msgs, ARRAY_SIZE(msgs));
}

static uint32_t _swap_bytes(uint32_t val, uint32_t nb_bytes)
{
	switch (nb_bytes) {
	case 4:
		return bswap_constant_32(val);
	case 3:
		return ((val >> 16) & 0xFF) |
		       (val & 0xFF00) |
		       ((val & 0xFF) << 16);
	case 2:
		return ((val >> 8) & 0xFF) | ((val & 0xFF) << 8);
	case 1:
		return val;
	default:
		return -1;
	}
}

static int32_t ad3552r_access_reg(struct ad3552r_desc *desc, uint8_t addr,
			   uint32_t in, uint32_t *out)
{
	uint8_t	 			reg_len;
	int32_t  			ret;
	uint32_t			tmp;
	struct ad3552_transfer_data	msg = { 0 };
	struct ad3552_transfer_config	spi_cfg = { 0 };

	if (!desc)
		return -ENODEV;

	reg_len = _ad3552r_reg_len(addr);
	if (reg_len == 0 ||
	    (addr >= AD3552R_SECONDARY_REGION_ADDR && desc->spi_cfg.addr_asc))
		return -EINVAL;

	if (reg_len > 1) {
		if (desc->spi_cfg.single_instr) {
			spi_cfg.single_instr = 0;
			msg.spi_cfg = &spi_cfg;
		}

		if (!desc->spi_cfg.addr_asc)
			addr += reg_len - 1;
	}

	if (out) {
		tmp = 0;
		msg.is_read = 1;
	} else {
		if (desc->spi_cfg.addr_asc)
			tmp = in;
		else
			tmp = _swap_bytes(in, reg_len);
		msg.is_read = 0;

	}
	msg.addr = addr;
	msg.data = (uint8_t *)&tmp;
	msg.len = reg_len;
	ret = ad3552r_transfer(desc, &msg);

	if (out) {
		if (desc->spi_cfg.addr_asc)
			*out = tmp;
		else
			*out = _swap_bytes(tmp, reg_len);
	}

	return ret;
}

int32_t ad3552r_write_reg(struct ad3552r_desc *desc, uint8_t addr,
			  uint32_t val)
{
	return ad3552r_access_reg(desc, addr, val, NULL);
}

int32_t ad3552r_read_reg(struct ad3552r_desc *desc, uint8_t addr,
			 uint32_t *val)
{
	return ad3552r_access_reg(desc, addr, 0, val);
}

static int32_t _ad3552r_get_crc_enable(struct ad3552r_desc *desc, uint16_t *en)
{
	int32_t		ret;
	uint32_t	reg;

	ret = _ad3552r_read_reg_mask(desc, AD3552R_ATTR_REG(AD3552R_CRC_ENABLE),
				    AD3552R_ATTR_MASK(AD3552R_CRC_ENABLE),
				    &reg);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (reg == AD3552R_CRC_ENABLE_VALUE)
		*en = 1;
	else if (reg == AD3552R_CRC_DISABLE_VALUE)
		*en = 0;
	else
		/* Unexpected value */
		return FAILURE;

	return SUCCESS;
}

static int32_t _ad3552r_set_crc_enable(struct ad3552r_desc *desc, uint16_t en)
{
	int32_t		ret;
	uint16_t	reg;


	reg = en ? AD3552R_CRC_ENABLE_VALUE : AD3552R_CRC_DISABLE_VALUE;
	ret = ad3552r_write_reg(desc, AD3552R_ATTR_REG(AD3552R_CRC_ENABLE), reg);
	if (IS_ERR_VALUE(ret))
		return ret;

	desc->crc_en = en;

	return SUCCESS;
}

// static void _tmr_callback(struct ad3552r_desc *desc, uint32_t event,
// 			  void *extra)
// {
// 	int32_t ret;

// 	/* Enter here when timer expiers */
// 	if (desc->trigger_mode == AD3552R_TRIGGER_HW_LDAC_INTERNAL) {
// 		ret = gpio_set_value(desc->ldac, !desc->ldac_val);
// 		if (IS_ERR_VALUE(ret))
// 			return ;
// 		if (desc->ldac_val == 0)
// 			desc->is_rise_edge = 1;
// 		desc->ldac_val = !desc->ldac_val;
// 	}
// }

// static int32_t _remove_timer_config(struct ad3552r_desc *desc)
// {
// 	int32_t ret;

// 	/* Clean old resources */
// 	ret = irq_disable(desc->irq_ctrl, desc->timer_intr_nb);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;
// 	ret = irq_unregister(desc->irq_ctrl, desc->timer_intr_nb);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;
// 	ret = timer_stop(desc->tmr);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;
// 	ret = timer_remove(desc->tmr);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;
// 	desc->tmr = NULL;

// 	return SUCCESS;
// }


// static int32_t _update_timer_config(struct ad3552r_desc *desc)
// {
// 	struct callback_desc	call;
// 	struct timer_init_param	tmr_param;
// 	uint32_t		freq;
// 	uint32_t		new_load;
// 	int32_t			ret;
// 	float			scale;
// 	float			desired_period;

// 	if (!desc->tmr) {
// 		tmr_param.id = desc->timer_id;
// 		tmr_param.freq_hz = SEC_TO_10NS(1);
// 		tmr_param.load_value = 0xFFFFFFFF;
// 		tmr_param.extra = desc->tmr_extra;
// 		ret = timer_init(&desc->tmr, &tmr_param);
// 		if (IS_ERR_VALUE(ret))
// 			return ret;
// 	}
// 	ret = timer_stop(desc->tmr);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;

// 	ret = timer_count_clk_get(desc->tmr, &freq);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;

// 	new_load = desc->update_period_ns / 2.0;
// 	desired_period = (float)new_load / (SEC_TO_10NS(1) * 10);
// 	new_load = freq * desired_period;
// 	ret = timer_counter_set(desc->tmr, new_load);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;

// 	call.callback = (void (*)(void *, uint32_t, void *))_tmr_callback;
// 	call.ctx = desc;
// 	call.config = NULL;
// 	ret = irq_register_callback(desc->irq_ctrl,
// 				    desc->timer_intr_nb,
// 				    &call);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;

// 	ret = irq_enable(desc->irq_ctrl, desc->timer_intr_nb);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;

// 	ret = timer_start(desc->tmr);
// 	if (IS_ERR_VALUE(ret))
// 		return ret;

// 	return SUCCESS;
// }

// static int32_t _config_trigger_mode(struct ad3552r_desc *desc, uint16_t val)
// {
// 	int32_t ret;

// 	/* Change to timer init_param and do check here */
// 	if (!desc->irq_ctrl)
// 		return -EINVAL;

// 	if (desc->trigger_mode == val)
// 		return SUCCESS;
	
// 	if (desc->trigger_mode != AD3552R_TRIGGER_NONE) {
// 		/* Release current timer config */
// 		ret = _remove_timer_config(desc);
// 		if (IS_ERR_VALUE(ret))
// 			return ret;
// 	}

// 	if (val == AD3552R_TRIGGER_NONE) {
// 		desc->trigger_mode = val;
// 		return SUCCESS;
// 	}

// 	/* Init timer */

// 	switch (val) {
// 	case AD3552R_TRIGGER_HW_LDAC_INTERNAL:
// 		if (!desc->ldac)
// 			return -EINVAL;
// 		desc->ldac_val = 0;
// 		//No break;
// 	case AD3552R_TRIGGER_DELAY_DAC_UPDATE:
// 	case AD3552R_TRIGGER_SW_LDAC:
// 		ret = _update_timer_config(desc);
// 		if (IS_ERR_VALUE(ret))
// 			return ret;
// 		break;
// 	case AD3552R_TRIGGER_HW_LDAC_EXTERNAL:
// 		if (!desc->is_hw_trigger_ready)
// 			return -EINVAL;
// 		break;
// 	default:
// 		val = AD3552R_TRIGGER_NONE;
// 	}
// 	desc->trigger_mode = val;

// 	return SUCCESS;
// }

int32_t ad3552r_get_dev_value(struct ad3552r_desc *desc,
			  enum ad3552r_dev_attributes attr,
			  uint32_t *val)
{
	uint32_t tmp;
	int32_t  ret;
	uint16_t en;

	if (!desc || !val)
		return -EINVAL;

	switch (attr) {
	case AD3552R_SPI_MULTI_IO_MODE:
	case AD3552R_SPI_DATA_RATE:
	case AD3552R_SPI_SYNCHRONOUS_ENABLE:
		/* TODO. Only standard spi implemented for the moment */
		//return -EINVAL;
		*val = 0;
		return 0;
	case AD3552R_CRC_ENABLE:
		ret =  _ad3552r_get_crc_enable(desc, &en);
		if (IS_ERR_VALUE(ret))
			return ret;
		*val = en;
		return ret;
	case AD3552R_PRECISION_MODE_ENABLED:
		*val = desc->precision_en;
		break;
	case AD3552R_UPDATE_MODE:
		*val = desc->update_mode;
		break;
	case AD3552R_INPUT_TRIGGER_MODE:
		*val = desc->trigger_mode;
		break;
	case AD3552R_DAC_UPDATE_PERIOD_NS:
		*val = desc->update_period_ns;
		break;
	case AD3552R_VREF_SELECT:
		*val = desc->vref_sel;
		break;
	default:
		ret = _ad3552r_get_reg_attr(desc, attr, &tmp);
		*val = tmp;

		return ret;
	}

	return SUCCESS;
}

int32_t ad3552r_set_dev_value(struct ad3552r_desc *desc,
			  enum ad3552r_dev_attributes attr,
			  uint32_t val)
{
	if (!desc)
		return -EINVAL;

	switch (attr) {
	case AD3552R_SPI_MULTI_IO_MODE:
	case AD3552R_SPI_DATA_RATE:
	case AD3552R_SPI_SYNCHRONOUS_ENABLE:
		/* TODO. Only standard spi implemented for the moment */
		//return -EINVAL;
		return 0;
	case AD3552R_CRC_ENABLE:
		return _ad3552r_set_crc_enable(desc, val);
	case AD3552R_PRECISION_MODE_ENABLED:
		desc->precision_en = !!val;
		break;
	case AD3552R_UPDATE_MODE:
		desc->update_mode = val;
		break;
	case AD3552R_INPUT_TRIGGER_MODE:
		return -EINVAL;
		//return _config_trigger_mode(desc, val);
	case AD3552R_DAC_UPDATE_PERIOD_NS:
		return -EINVAL;
		// if (val < 10)
		// 	return -EINVAL;
		// desc->update_period_ns = val;
		// if (desc->tmr)
		// 	return _update_timer_config(desc);
		// break;
	case AD3552R_VREF_SELECT:
		desc->vref_sel = val & AD3552R_MASK_REFERENCE_VOLTAGE_SEL;
		//No break
	default:
		return _ad3552r_set_reg_attr(desc, attr, val);
	}

	return SUCCESS;
}

static inline uint8_t _get_code_reg_addr(uint8_t ch, uint8_t mode, uint8_t prec)
{
	switch (mode) {
	case AD3552R_UPDATE_DAC:
		return prec ? AD3552R_REG_ADDR_CH_DAC_24B(ch) :
				AD3552R_REG_ADDR_CH_DAC_16B(ch);
	case AD3552R_UPDATE_DAC_MASK:
		return prec ? AD3552R_REG_ADDR_DAC_PAGE_MASK_24B :
				AD3552R_REG_ADDR_DAC_PAGE_MASK_16B;
	case AD3552R_UPDATE_INPUT:
		return prec ? AD3552R_REG_ADDR_CH_INPUT_24B(ch) :
				AD3552R_REG_ADDR_CH_INPUT_16B(ch);
	case AD3552R_UPDATE_INPUT_MASK:
		return prec ? AD3552R_REG_ADDR_INPUT_PAGE_MASK_24B :
				AD3552R_REG_ADDR_INPUT_PAGE_MASK_16B;
	default:
		return FAILURE;
	}
}

static int32_t _ad3552r_set_code_value(struct ad3552r_desc *desc,
			  	       uint8_t ch,
				       uint16_t val)
{
	uint32_t	code;
	uint8_t		addr;
	
	addr = _get_code_reg_addr(ch, desc->update_mode, desc->precision_en);
	if (desc->precision_en)
		code = ((uint32_t)val) << 8;
	else
		code = val & AD3552R_MASK_DAC_12B;

	return ad3552r_write_reg(desc, addr, code);
}

static int32_t _ad3552r_get_code_value(struct ad3552r_desc *desc,
			  	       uint8_t ch,
				       uint16_t *val)
{
	uint32_t	code;
	uint8_t		addr;
	int32_t		ret;

	addr = _get_code_reg_addr(ch, desc->update_mode, desc->precision_en);
	ret = ad3552r_read_reg(desc, addr, &code);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (desc->precision_en)
		*val = (uint16_t)code >> 8;
	else
		*val = (uint16_t)code;

	return SUCCESS;;
}

static int32_t _ad3552r_set_gain_value(struct ad3552r_desc *desc,
		  	  	       enum ad3552r_ch_attributes attr,
				       uint8_t ch,
				       uint16_t val)
{
	struct ad3552_transfer_data	msg = {0};
	struct ad3552_transfer_config	spi_cfg = {0};
	uint16_t reg_mask;
	uint16_t regs;
	int32_t  ret;

	msg.addr = AD3552R_REG_ADDR_CH_GAIN(ch);
	msg.data = (uint8_t *)&regs;
	if (attr == AD3552R_CH_GAIN_OFFSET)
		msg.len = 2;
	else
		msg.len = 1;
	/* Use default spi configuration with addr_asc = 0 */
	msg.spi_cfg = &spi_cfg;

	msg.is_read = 1;
	ret = ad3552r_transfer(desc, &msg);
	if (IS_ERR_VALUE(ret))
		return ret;

	switch (attr) {
	case AD3552R_CH_GAIN_OFFSET:
		reg_mask = 0xFF01;
		desc->ch_data[ch].offset = val;
		val = _swap_bytes(val, sizeof(uint16_t));
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
		desc->ch_data[ch].p = val;
		reg_mask = AD3552R_MASK_CH_GAIN_SCALING_P;
		break;
	case AD3552R_CH_GAIN_SCALING_N:
		desc->ch_data[ch].n = val;
		reg_mask = AD3552R_MASK_CH_GAIN_SCALING_N;
		break;
	default:
		return -EINVAL;
	}
	regs = (regs & ~reg_mask) | field_prep(reg_mask, val);

	msg.is_read = 0;
	return ad3552r_transfer(desc, &msg);
}

static int32_t _ad3552r_get_gain_value(struct ad3552r_desc *desc,
		  	  	       enum ad3552r_ch_attributes attr,
				       uint8_t ch,
				       uint16_t *val)
{
	struct ad3552_transfer_data	msg = {0};
	struct ad3552_transfer_config	spi_cfg = {0};
	uint16_t reg_mask;
	uint16_t regs;
	int32_t  ret;

	msg.addr = AD3552R_REG_ADDR_CH_GAIN(ch);
	msg.data = (uint8_t *)&regs;
	if (attr == AD3552R_CH_GAIN_OFFSET)
		msg.len = 2;
	else
		msg.len = 1;
	/* Use default spi configuration with addr_asc = 0 */
	msg.spi_cfg = &spi_cfg;

	msg.is_read = 1;
	ret = ad3552r_transfer(desc, &msg);
	if (IS_ERR_VALUE(ret))
		return ret;

	switch (attr) {
	case AD3552R_CH_GAIN_OFFSET:
		reg_mask = 0xFF01;
		break;
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
	*val = field_get(reg_mask, regs);
	if (attr == AD3552R_CH_GAIN_OFFSET)
		*val = _swap_bytes(*val, sizeof(regs));

	return SUCCESS;
}


static inline uint8_t _get_ch_attr_reg_addr(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr)
{
	uint8_t addr;

	addr = AD3552R_CH_ATTR_REG(attr);
	if (desc->precision_en)
	{
		if (attr == AD3552R_CH_SELECT)
			return AD3552R_REG_ADDR_CH_SELECT_24B;
		if (attr == AD3552R_CH_TRIGGER_SOFTWARE_LDAC)
			return AD3552R_REG_ADDR_SW_LDAC_24B;
		if (attr == AD3552R_CH_HW_LDAC_MASK)
			return AD3552R_REG_ADDR_HW_LDAC_24B;
	}

	return addr;
}

int32_t ad3552r_get_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t *val)
{
	uint32_t	reg;
	int32_t		ret;
	uint8_t		addr;

	if (!desc || !val)
		return -EINVAL;

	/* Attributes not defined in addr_mask_map_ch */
	switch (attr) {
	case AD3552R_CH_CODE:
		return _ad3552r_get_code_value(desc, ch, val);
	case AD3552R_CH_ENABLE:
		*val = !!(desc->active_ch & AD3552R_MASK_CH(ch));
		return SUCCESS;
	case AD3552R_CH_RFB:
		*val = desc->ch_data[ch].rfb;
		return SUCCESS;
	case AD3552R_CH_CODE_AT_MIN:
		*val = desc->ch_data[ch].code_min;
		return SUCCESS;
	case AD3552R_CH_CODE_AT_MAX:
		*val = desc->ch_data[ch].code_max;
		return SUCCESS;
	default:
		break;
	}

	if (attr >= AD3552R_CH_RANGE_OVERRIDE &&
	    attr <= AD3552R_CH_GAIN_SCALING_N)
		return _ad3552r_get_gain_value(desc, attr, ch, val);
 
	addr = _get_ch_attr_reg_addr(desc, attr);
	if (addr == AD3552R_REG_ADDR_SW_LDAC_24B ||
	    addr == AD3552R_REG_ADDR_SW_LDAC_16B) {
		pr_debug("Write only registers\n");
	    	/* LDAC are write only registers */
		return -EINVAL;
	}

	ret = ad3552r_read_reg(desc, addr, &reg);
	if (IS_ERR_VALUE(ret))
		return ret;

	*val = field_get(AD3552R_CH_ATTR_MASK(ch, attr), reg);

	return SUCCESS;
}

int32_t ad3552r_set_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t val)
{
	int32_t		ret;

	if (!desc)
		return -EINVAL;


	/* Attributes not defined in addr_mask_map_ch */
	switch (attr) {
	case AD3552R_CH_CODE:
		return _ad3552r_set_code_value(desc, ch, val);
	case AD3552R_CH_ENABLE:
		if (val)
			desc->active_ch |= AD3552R_MASK_CH(ch);
		else
			desc->active_ch &= ~AD3552R_MASK_CH(ch);

		return SUCCESS;
	case AD3552R_CH_RFB:
		desc->ch_data[ch].rfb = val;
		return SUCCESS;
	case AD3552R_CH_CODE_AT_MIN:
		desc->ch_data[ch].code_min = val;
		return SUCCESS;
	case AD3552R_CH_CODE_AT_MAX:
		desc->ch_data[ch].code_max = val;
		return SUCCESS;
	default:
		break;
	}

	if (attr >= AD3552R_CH_RANGE_OVERRIDE &&
	    attr <= AD3552R_CH_GAIN_SCALING_N)
		return _ad3552r_set_gain_value(desc, attr, ch, val);

	/* Update register related to attributes in chip */
	ret = _ad3552r_write_reg_mask(desc,
				      _get_ch_attr_reg_addr(desc, attr),
				      AD3552R_CH_ATTR_MASK(ch, attr),
				      val);
	if (IS_ERR_VALUE(ret))
		return ret;

	/* Update software structures */
	if (attr == AD3552R_CH_OUTPUT_RANGE_SEL) {
		val %= AD3552R_CH_OUTPUT_RANGE_NEG_10__10V + 1;
		desc->ch_data[ch].range = val;
		desc->ch_data[ch].rfb = default_rfbs[val];
		
	}

	return ret;
}

int32_t ad3552r_init(struct ad3552r_desc **desc,
		     struct ad3552r_init_param *init_param)
{
	struct ad3552r_desc	*ldesc;
	int32_t			ret;

	if (!desc || !init_param)
		return -EINVAL;

	ldesc = (struct ad3552r_desc*)calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	ret = spi_init(&ldesc->spi, &init_param->spi_param);
	if (IS_ERR_VALUE(ret))
		goto err;

	crc8_populate_msb(ldesc->crc_table, AD3552R_CRC_POLY);

	ldesc->spi_cfg = (struct ad3552_transfer_config) { 0 };
	ret = ad3552r_software_reset(ldesc);
	if (IS_ERR_VALUE(ret))
		goto err_spi;

	ldesc->update_period_ns = AD3552R_DEFAULT_DAC_UPDATE_PERIOD;
	ldesc->ch_data[0].code_max = UINT16_MAX;
	ldesc->ch_data[1].code_max = UINT16_MAX;
	ldesc->active_ch = 0;
	ldesc->irq_ctrl = init_param->irq_crtl;
	// ldesc->timer_id = init_param->timer_id;
	// ldesc->timer_intr_nb = init_param->timer_intr_nb;
	// ldesc->tmr_extra = init_param->tmr_extra;
	ldesc->is_hw_trigger_ready = init_param->is_hw_trigger_ready;
	ldesc->hw_trigger_ctx = init_param->hw_trigger_ctx;

	ret = gpio_get_optional(&ldesc->ldac, init_param->ldac_param);
	if (IS_ERR_VALUE(ret))
		goto err_spi;

	ret = gpio_direction_output(ldesc->ldac, 0);
	if (IS_ERR_VALUE(ret))
		goto err_spi;

	*desc = ldesc;

	return SUCCESS;
err_spi:
	spi_remove(ldesc->spi);
err:
	free(ldesc);

	return ret;
}

int32_t ad3552r_remove(struct ad3552r_desc *desc)
{
	spi_remove(desc->spi);
	free(desc);

	return SUCCESS;
}

int32_t ad3552r_software_reset(struct ad3552r_desc *desc)
{
	int32_t ret;
	uint32_t val;

	ret = _ad3552r_write_reg_mask(desc,
				     AD3552R_REG_ADDR_INTERFACE_CONFIG_A,
				     AD3552R_MASK_SOFTWARE_RESET,
				     AD3552R_MASK_SOFTWARE_RESET);
	if (IS_ERR_VALUE(ret))
		return ret;

	do {
		ret = ad3552r_read_reg(desc,
				       AD3552R_REG_ADDR_INTERFACE_CONFIG_B,
				       &val);
		if (IS_ERR_VALUE(ret))
			return ret;
	} while (val != AD3552R_DEFAULT_CONFIG_B_VALUE);

	ret = ad3552r_read_reg(desc, AD3552R_REG_ADDR_INTERFACE_CONFIG_A, &val);
	if (IS_ERR_VALUE(ret))
		return ret;

	desc->spi_cfg.addr_asc = field_get(AD3552R_MASK_ADDR_ASCENSION, val);

	/* Wait for interface to be read and clear reset status bit */
	do {
		ret = ad3552r_get_status(desc, &val, 1);
		if (IS_ERR_VALUE(ret))
			return ret;
	} while (val & AD3552R_INTERFACE_NOT_READY);

	return SUCCESS;
}

int32_t ad3552r_get_status(struct ad3552r_desc *desc, uint32_t *status,
			   uint8_t clr_err)
{
	uint32_t	_st;
	uint32_t	new_reg;;
	uint32_t	reg;
	int32_t		ret;

	ret = ad3552r_read_reg(desc, AD3552R_REG_ADDR_INTERFACE_STATUS_A, &reg);
	if (IS_ERR_VALUE(ret))
		return ret;

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
		ret = ad3552r_write_reg(desc,
					AD3552R_REG_ADDR_INTERFACE_STATUS_A,
					reg);
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	ret = ad3552r_read_reg(desc, AD3552R_REG_ADDR_ERR_STATUS, &reg);
	if (IS_ERR_VALUE(ret))
		return ret;

	new_reg = 0;
	_CHECK_STATUS(_st, reg, new_reg, REF_RANGE_ERR_STATUS, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, DUAL_SPI_STREAM_EXCEEDS_DAC_ERR_STATUS,
		      clr_err);
	_CHECK_STATUS(_st, reg, new_reg, MEM_CRC_ERR_STATUS, clr_err);
	_CHECK_STATUS(_st, reg, new_reg, RESET_STATUS, clr_err);
	if (new_reg) {
		ret = ad3552r_write_reg(desc, AD3552R_REG_ADDR_ERR_STATUS, reg);
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	return SUCCESS;
}

static inline float _ad3552r_calc_span(struct ad3552r_desc *desc,
				       uint8_t ch, float *v_min)
{
	float	v_max;
	float	tmp;
	float	common;
	uint8_t idx;

	v_max = 0;
	if (desc->ch_data[ch].range_override) {
		/* Vout_fs */
		idx = desc->ch_data[ch].n;
		common = 2.5 * desc->ch_data[ch].rfb * 1.03;
		tmp = gains_scaling_table[idx] + desc->ch_data[ch].offset / 1024.0;
		v_max = 2.5 + (tmp * common);
		/* Vout_zs */
		idx = desc->ch_data[ch].p;
		tmp = gains_scaling_table[idx] - desc->ch_data[ch].offset / 1024.0;
		*v_min = 2.5 - (tmp * common);

	} else {
		idx = desc->ch_data[ch].range;
		v_max = ch_ranges[idx][1];
		*v_min = ch_ranges[idx][0];
	}

	return v_max - *v_min;
}

int32_t ad3552r_get_ch_voltage(struct ad3552r_desc *desc, uint8_t ch,
			       float *val)
{
	uint16_t	code;
	uint16_t	code_span;
	int32_t		ret;
	float		span;
	float		v_min;

	if (!desc || !val)
		return -EINVAL;

	ret = _ad3552r_get_code_value(desc, ch, &code);
	if (IS_ERR_VALUE(ret))
		return ret;

	span = _ad3552r_calc_span(desc, ch, &v_min);
	code_span = desc->ch_data[ch].code_max - desc->ch_data[ch].code_min;
	*val = (float)code - desc->ch_data[ch].code_min;
	*val /= (float)code_span;
	*val *= span;
	*val += v_min;

	return SUCCESS;
}

int32_t ad3552r_set_ch_voltage(struct ad3552r_desc *desc, uint8_t ch,
			       const float val)
{
	uint16_t	code;
	uint16_t	code_span;
	float		span;
	float		tmp;
	float		v_min;

	if (!desc)
		return -EINVAL;

	span = _ad3552r_calc_span(desc, ch, &v_min);
	code_span = desc->ch_data[ch].code_max - desc->ch_data[ch].code_min;
	tmp = val - v_min;
	tmp /= span;
	tmp *= code_span;
	tmp += desc->ch_data[ch].code_min;
	if (tmp > UINT16_MAX)
		tmp = UINT16_MAX;
	code = (uint16_t)tmp;

	return _ad3552r_set_code_value(desc, ch, code);
}

int32_t ad3552r_prepare_write(struct ad3552r_desc *desc, uint32_t mask)
{
	uint16_t i;
	int32_t  ret;

	if (!desc || !mask)
		return -EINVAL;

	/* Enable activated channels */
	mask &= AD3552R_MASK_ALL_CH;

	if (desc->update_mode == AD3552R_UPDATE_DAC_MASK ||
	    desc->update_mode == AD3552R_UPDATE_INPUT_MASK)
		/*
		 * Mask will be updated, therefore only one channel should
		 * be set to active. Otherwise will affect math in write_dev
		 */
		mask = 1;

	if (mask == desc->active_ch)
		return SUCCESS;

	for (i = 0; i < AD3552R_NUM_CH; i++) {
		ret = ad3552r_set_ch_value(desc, AD3552R_CH_ENABLE, i,
					   !!(mask & AD3552R_MASK_CH(i)));
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	return SUCCESS;
}

static int32_t _ad3552r_write_dev_direct(struct ad3552r_desc *desc,
					 uint8_t *data,
					 uint32_t samples)
{
	struct ad3552_transfer_data	msg = {0};
	struct ad3552_transfer_config	spi_cfg = {0};
	uint8_t				ch;
	uint8_t				nb_ch;
	uint8_t				reg_len;
	uint8_t				bytes_per_sample;

	if (desc->precision_en)
		bytes_per_sample = AD3552R_STORAGE_BITS_PREC_MODE / 8;
	else
		bytes_per_sample = AD3552R_STORAGE_BITS_FAST_MODE / 8;
	ch = find_last_set_bit(desc->active_ch);
	nb_ch = hweight8(desc->active_ch);
	msg.addr = _get_code_reg_addr(ch, desc->update_mode,
				      desc->precision_en);
	reg_len = _ad3552r_reg_len(msg.addr);
	spi_cfg.stream_length_keep_value = 1;
	spi_cfg.stream_mode_length = nb_ch * reg_len;
	msg.spi_cfg = &spi_cfg;
	msg.addr += reg_len - 1;
	msg.len = samples * nb_ch * bytes_per_sample;
	msg.data = data;

	return ad3552r_transfer(desc, &msg);
}

static void _wait_for_trigger(struct ad3552r_desc *desc)
{
	switch (desc->trigger_mode) {
	case AD3552R_TRIGGER_SW_LDAC:
	case AD3552R_TRIGGER_DELAY_DAC_UPDATE:
	case AD3552R_TRIGGER_HW_LDAC_INTERNAL:
		desc->is_rise_edge = 0;
		while (desc->is_rise_edge != 1)
			;
		break;
	case AD3552R_TRIGGER_HW_LDAC_EXTERNAL:
		while (!desc->is_hw_trigger_ready(desc->hw_trigger_ctx))
			;
		break;
	default:
		break;
	}
}

int32_t ad3552r_write_dev(struct ad3552r_desc *desc, uint8_t *data,
			  uint32_t samples)
{
	struct ad3552_transfer_data	msg = {0};
	struct ad3552_transfer_config	spi_cfg = {0};
	uint8_t		val[AD3552R_NUM_CH * AD3552R_MAX_REG_SIZE];
	uint8_t		addr;
	uint8_t		len;
	uint32_t	ch;
	int32_t		ret;
	uint32_t	bytes_per_sample;
	uint32_t	i;

	if (!desc || !desc->active_ch)
		return -EINVAL;

	if ((desc->update_mode == AD3552R_UPDATE_DAC ||
	     desc->update_mode == AD3552R_UPDATE_DAC_MASK) && 
	    desc->trigger_mode != AD3552R_TRIGGER_DELAY_DAC_UPDATE)
		return _ad3552r_write_dev_direct(desc, data, samples);

	bytes_per_sample = desc->precision_en ? 3 : 2;
	len = hweight8(desc->active_ch) * bytes_per_sample;
	ch = find_last_set_bit(desc->active_ch);
	addr = _get_code_reg_addr(ch, desc->update_mode,
				  desc->precision_en);
	addr +=  _ad3552r_reg_len(addr) - 1;
	msg.addr = addr;
	msg.data = val;
	msg.spi_cfg = &spi_cfg;
	msg.len = len;
	for (i = 0; i < samples; i++) {
		memcpy(val, data + i * len, len);
		ret = ad3552r_transfer(desc, &msg);
		if (IS_ERR_VALUE(ret))
			return ret;

		_wait_for_trigger(desc);

		if (desc->trigger_mode == AD3552R_TRIGGER_SW_LDAC) {
			if (desc->precision_en)
				addr = AD3552R_REG_ADDR_SW_LDAC_24B;
			else
				addr = AD3552R_REG_ADDR_SW_LDAC_16B;
			ret = ad3552r_write_reg(desc, addr, desc->active_ch);
			if (IS_ERR_VALUE(ret))
				return ret;
		}
	}

	return SUCCESS;
}

#ifdef DEBUG

void dump_buff(uint8_t *buff, uint32_t len)
{
	for (int i = 0; i < (int)len; i++)
		printf("0x%02x ", buff[i]);
	printf("\n");
}

#include <string.h>


#define MAX_REG 0x28

int32_t read_regs(struct ad3552r_desc *dac)
{
	int ret = 0;
	uint32_t addr = 0;
	uint8_t regs[MAX_REG];
	uint8_t addrs[MAX_REG];
	while (addr < MAX_REG) {
		uint32_t len;
		uint8_t buff[10];
		len = 2;
		buff[0] = addr | AD3552R_READ_BIT;
		buff[1] = 0xFF;
		buff[2] = 0xFF;
		buff[3] = 0xFF;
		buff[4] = 0xFF;

		ret = spi_write_and_read(dac->spi, buff, len);
		PRINT_AND_RET_ON_ERR(ret, "spi_write_and_read failed");
		addrs[addr] = addr;
		regs[addr] = buff[1];
		addr++;
	}

	dump_buff(addrs,MAX_REG);
	dump_buff(regs,MAX_REG);

	return ret;
}
#endif

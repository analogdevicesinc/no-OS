/***************************************************************************//**
 *   @file   iio_ad713x.c
 *   @brief  Implementation of iio_ad713x.c.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#ifdef IIO_SUPPORT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <errno.h>
#include <string.h>

#include "ad713x.h"
#include "iio.h"
#include "iio_ad713x.h"
#include "iio_types.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "spi_engine.h"

#define MEGA 1000000UL

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct ad713x_iio
 * @brief AD713x IIO private handler
 */
struct ad713x_iio {
	/** AD713x driver handler */
	struct ad713x_dev *drv_dev;
	/** Generic IIO device handler */
	struct iio_device *iio_dev;
	/** Integer part of the ODR */
	uint32_t odr_int;
	/** Decimal part of the ODR */
	uint32_t odr_flt;
	/** Integer part of the VREF */
	uint32_t vref_int;
	/** Decimal part of the VREF */
	uint32_t vref_micro;
	/** SPI Engine driver handler */
	struct no_os_spi_desc *spi_eng_desc;
	/** Active channels mask */
	uint32_t mask;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

#define AD713x_IIO_FUNC(_name)	static int _name(void *device, char *buf, \
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)

AD713x_IIO_FUNC(ad713x_iio_show_filter_3db);
AD713x_IIO_FUNC(ad713x_iio_show_offset);
AD713x_IIO_FUNC(ad713x_iio_store_offset);
AD713x_IIO_FUNC(ad713x_iio_show_raw);
AD713x_IIO_FUNC(ad713x_iio_show_odr);
AD713x_IIO_FUNC(ad713x_iio_store_odr);
AD713x_IIO_FUNC(ad713x_iio_show_scale);

/******************************************************************************/
/********************* Static Variables Definitions ***************************/
/******************************************************************************/

static struct iio_attribute channel_attributes[] = {
	{
		.name = "filter_low_pass_3db_frequency",
		.priv = 0,
		.shared = IIO_SEPARATE,
		.show = ad713x_iio_show_filter_3db,
		.store = NULL
	},
	{
		.name = "offset",
		.priv = 0,
		.shared = IIO_SEPARATE,
		.show = ad713x_iio_show_offset,
		.store = ad713x_iio_store_offset
	},
	{
		.name = "raw",
		.priv = 0,
		.shared = IIO_SEPARATE,
		.show = ad713x_iio_show_raw,
		.store = NULL
	},
	{
		.name = "sampling_frequency",
		.priv = 0,
		.shared = IIO_SHARED_BY_ALL,
		.show = ad713x_iio_show_odr,
		.store = ad713x_iio_store_odr
	},
	{
		.name = "scale",
		.priv = 0,
		.shared = IIO_SHARED_BY_ALL,
		.show = ad713x_iio_show_scale,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY
};

struct scan_type ad713x_iio_scan_type = {
	.sign = 's',
	.realbits = 24,
	.storagebits = 32,
	.shift = 8,
	.is_big_endian = true
};

#define AD713X_IIO_CHANN_DEF(nm, ch1) \
	{ \
		.name = nm, \
		.ch_type = IIO_VOLTAGE, \
		.channel = ch1, \
		.channel2 = 0, \
		.address = ch1, \
		.scan_index = ch1, \
		.scan_type = &ad713x_iio_scan_type, \
		.attributes = channel_attributes, \
		.ch_out = IIO_DIRECTION_INPUT, \
		.indexed = true, \
		.diferential = false, \
	}

static struct iio_channel ad713x_channels[] = {
	AD713X_IIO_CHANN_DEF("ch0", 0),
	AD713X_IIO_CHANN_DEF("ch1", 1),
	AD713X_IIO_CHANN_DEF("ch2", 2),
	AD713X_IIO_CHANN_DEF("ch3", 3)
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Transform decimal floating point into hexadecimal floating point for
 *        the ODR register.
 * @param [in] odr_flt - Decimal ODR in microhertz (no integer part)
 * @return hexadecimal floating point
 */
static uint32_t _ad713x_odr_itoh_float_conversion(uint32_t odr_flt)
{
	int i;
	uint32_t result = 0;

	for (i = 1; i <= 8; i++) {
		odr_flt *= 16;
		result |= (odr_flt / MEGA) << (32 - 4 * i);
		odr_flt %= MEGA;
		if (!odr_flt)
			break;
	}

	return result;
}

/**
 * @brief Transform hexadecimal floating point into decimal floating point for
 *        the ODR register.
 * @param [in] odr_flt - Decimal ODR in microhertz (no integer part)
 * @return decimal floating point
 */
static uint32_t _ad713x_odr_htoi_float_conversion(uint32_t odr_flt)
{
	return (uint32_t)(no_os_mul_u32_u32(odr_flt, 1000000) / 0x100000000ULL);
}

static int ad713x_iio_show_filter_3db(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad713x_iio	*iio_desc = (struct ad713x_iio *)device;
	struct ad713x_dev	*desc = iio_desc->drv_dev;
	uint8_t				value;
	int32_t				ret;
	uint32_t			val[2];
	uint32_t			temp;
	const uint32_t sinc_mult_micro[] = {0, 186100, 261700, 275300};
	const uint32_t fir_mult_micro[] = {433000, 108250};

	/** Slave ODR is controlled by external signal */
	if (!desc->mode_master_nslave)
		return -1;

	ret = ad713x_spi_reg_read(desc, AD713X_REG_CHAN_DIG_FILTER_SEL, &value);
	if (ret < 0)
		return ret;

	value = (value & AD713X_DIGFILTER_SEL_CH_MSK(channel->ch_num)) >>
		(channel->ch_num * 2);
	switch (value) {
	case SINC6:
	case SINC3:
	case SINC3_50_60_REJ:
		val[0] = no_os_mul_u32_u32(iio_desc->odr_int, sinc_mult_micro[value]) /
			 MEGA;
		temp = no_os_mul_u32_u32(iio_desc->odr_int, sinc_mult_micro[value]) %
		       MEGA;
		val[1] = no_os_mul_u32_u32(iio_desc->odr_flt, sinc_mult_micro[value]) +
			 temp;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
					(int32_t *)val);
	case FIR:
		ret = ad713x_spi_reg_read(desc, AD713X_REG_FIR_BW_SEL, &value);
		if (ret < 0)
			return ret;
		value = !!(value & AD713X_FIR_BW_SEL_CH_MSK(channel->ch_num));
		val[0] =  no_os_mul_u32_u32(iio_desc->odr_int, fir_mult_micro[value]) /
			  MEGA;
		temp = no_os_mul_u32_u32(iio_desc->odr_int, fir_mult_micro[value]) %
		       MEGA;
		val[1] = no_os_mul_u32_u32(iio_desc->odr_flt, fir_mult_micro[value]) +
			 temp;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
					(int32_t *)val);
	default:
		return -1;
	}
}

static int ad713x_iio_show_offset(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad713x_iio	*iio_desc = (struct ad713x_iio *)device;
	struct ad713x_dev	*desc = iio_desc->drv_dev;
	uint8_t				reg_value;
	const uint8_t		reg_addr[] = {
		AD713X_REG_CH0_OFFSET_LSB,
		AD713X_REG_CH1_OFFSET_LSB,
		AD713X_REG_CH2_OFFSET_LSB,
		AD713X_REG_CH3_OFFSET_LSB
	};
	uint32_t val = 0;
	int i, ret;

	for (i = 0; i < 3; i++) {
		ret = ad713x_spi_reg_read(desc, reg_addr[channel->ch_num] + i,
					  &reg_value);
		if (ret < 0)
			return ret;
		if (i == 2)
			reg_value &= 0x7F;
		val |= (uint32_t)reg_value << (8 * i);
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

static int ad713x_iio_store_offset(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ad713x_iio	*iio_desc = (struct ad713x_iio *)device;
	struct ad713x_dev	*desc = iio_desc->drv_dev;
	uint32_t			val;
	const uint8_t		reg_addr[] = {
		AD713X_REG_CH0_OFFSET_LSB,
		AD713X_REG_CH1_OFFSET_LSB,
		AD713X_REG_CH2_OFFSET_LSB,
		AD713X_REG_CH3_OFFSET_LSB
	};
	uint8_t reg_value;
	int i, ret;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	for (i = 0; i < 3; i++) {
		reg_value = (val >> (8 * i)) & 0xFF;
		if (i == 2)
			reg_value |= AD713X_CH_OFFSET_CAL_EN_MSK;
		ret = ad713x_spi_reg_write(desc, reg_addr[channel->ch_num] + i,
					   reg_value);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static int ad713x_iio_show_raw(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad713x_iio	*iio_desc = (struct ad713x_iio *)device;
	struct spi_engine_offload_message spi_engine_offload_message;
	uint32_t spi_eng_msg_cmds[1];
	uint32_t adc_buffer[3 * AD713X_CH_MAX] __attribute__((aligned));
	int ret;
	int32_t data;

	spi_eng_msg_cmds[0] = READ(4);

	spi_engine_offload_message.commands = spi_eng_msg_cmds;
	spi_engine_offload_message.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	spi_engine_offload_message.commands_data = NULL;
	spi_engine_offload_message.rx_addr = (uint32_t)adc_buffer;
	spi_engine_offload_message.tx_addr = (uint32_t)NULL;

	if (iio_desc->dcache_invalidate_range)
		iio_desc->dcache_invalidate_range((uint32_t)adc_buffer,
						  3 * iio_desc->iio_dev->num_ch * sizeof(uint32_t));

	/** Take read 3 samples and use the one in the middle;
	 *  workaround for a bug where the first and last samples are 0
	 *  todo: investigate and fix bug */
	ret = spi_engine_offload_transfer(iio_desc->spi_eng_desc,
					  spi_engine_offload_message,
					  3 * iio_desc->iio_dev->num_ch);
	if (ret != 0)
		return ret;

	if (iio_desc->dcache_invalidate_range)
		iio_desc->dcache_invalidate_range((uint32_t)adc_buffer,
						  3 * iio_desc->iio_dev->num_ch * sizeof(uint32_t));

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &data);
}

static int ad713x_iio_show_odr(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad713x_iio	*iio_desc = (struct ad713x_iio *)device;
	struct ad713x_dev	*desc = iio_desc->drv_dev;
	uint32_t odr_tab[2];

	/** Slave ODR is controlled by external signal */
	if (!desc->mode_master_nslave)
		return -1;

	odr_tab[0] = iio_desc->odr_int;
	odr_tab[1] = iio_desc->odr_flt;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
				(int32_t *)odr_tab);
}

static int ad713x_iio_store_odr(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad713x_iio	*iio_desc = (struct ad713x_iio *)device;
	struct ad713x_dev	*desc = iio_desc->drv_dev;
	uint8_t reg_data;
	uint32_t reg_int, reg_flt;
	uint64_t odr_uhz, odr_ref = 24000000000000ULL, rest;
	int i, ret;

	/** Slave ODR is controlled by external signal */
	if (!desc->mode_master_nslave)
		return -1;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO,
			(int32_t *)&iio_desc->odr_int, (int32_t *)&iio_desc->odr_flt);
	odr_uhz = no_os_mul_u32_u32(iio_desc->odr_int, MEGA) + iio_desc->odr_flt;

	rest = no_os_do_div(&odr_ref, odr_uhz);
	reg_int = odr_ref;
	rest *= MEGA;
	no_os_do_div(&rest, odr_uhz);
	reg_flt = _ad713x_odr_itoh_float_conversion(rest);

	for (i = 0; i < 7; i++) {
		if (i < 3)
			reg_data = (reg_int >> (8 * i)) & 0xFF;
		else
			reg_data = (reg_flt >> (8 * (i - 3))) & 0xFF;
		ret = ad713x_spi_reg_write(desc, AD713X_REG_ODR_VAL_INT_LSB + i,
					   reg_data);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static int ad713x_iio_show_scale(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad713x_iio	*iio_desc = (struct ad713x_iio *)device;
	uint32_t vref[2];
	uint64_t reference_nv;

	reference_nv = iio_desc->vref_int * 1000000000 +
		       iio_desc->vref_micro * 1000;

	no_os_do_div(&reference_nv, 0x7FFFFF);

	vref[0] = reference_nv / 1000000000;
	vref[1] = reference_nv % 1000000000;

	/** Even though the voltage is calculated in nanovolts we need to
	 *  say they are microvolts because we need the scale to be
	 *  shown in milivolts instead of volts
	 *  todo: fix if necessary */
	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
				(int32_t *)vref);
}

static int32_t ad713x_iio_prepare_transfer(struct ad713x_iio *desc,
		uint32_t mask)
{
	if (!desc)
		return -EINVAL;

	desc->mask = mask;

	return 0;
}

static int32_t ad713x_iio_read_dev(struct ad713x_iio *desc, uint32_t *buff,
				   uint32_t nb_samples)
{
	struct spi_engine_offload_message msg;
	uint32_t bytes;
	int32_t  ret;
	uint32_t spi_eng_msg_cmds[1];

	if (!desc)
		return -1;

	bytes = nb_samples * desc->iio_dev->num_ch * sizeof(buff[0]);

	spi_eng_msg_cmds[0] = READ(4);

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	msg.commands_data = NULL;
	msg.rx_addr = (uint32_t)buff;
	msg.tx_addr = (uint32_t)NULL;

	if (desc->dcache_invalidate_range)
		desc->dcache_invalidate_range(msg.rx_addr, bytes);

	ret = spi_engine_offload_transfer(desc->spi_eng_desc, msg, nb_samples);
	if (ret < 0)
		return ret;

	if (desc->dcache_invalidate_range)
		desc->dcache_invalidate_range(msg.rx_addr, bytes);

	return nb_samples;
}

/**
 * @brief Allocate memory for AD713x IIO handler.
 * @param desc - Pointer to the IIO device structure.
 * @param param - Pointer to the initialization structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int iio_ad713x_init(struct ad713x_iio **desc,
		    struct ad713x_iio_init_param *param)
{
	struct ad713x_iio *device;
	int32_t ret;
	uint8_t reg_data;
	uint64_t odr_mult, odr_ref = 24000000000000ULL, rest;
	int i;

	if (!param)
		return -EINVAL;

	device = (struct ad713x_iio *)no_os_calloc(1, sizeof(*device));
	if (!device)
		return -1;

	device->drv_dev = param->drv_dev;
	device->iio_dev = param->iio_dev;
	device->spi_eng_desc = param->spi_eng_desc;
	device->vref_int = param->vref_int;
	device->vref_micro = param->vref_micro;
	device->dcache_invalidate_range = param->dcache_invalidate_range;

	for (i = 0; i < 7; i++) {
		ret = ad713x_spi_reg_read(device->drv_dev,
					  AD713X_REG_ODR_VAL_INT_LSB + i, &reg_data);
		if (ret < 0)
			goto dev_err;
		if (i < 3)
			device->odr_int |= reg_data << (8 * i);
		else
			device->odr_flt |= reg_data << (8 * (i - 3));
	}
	device->odr_flt = _ad713x_odr_htoi_float_conversion(device->odr_flt);
	odr_mult = no_os_mul_u32_u32(device->odr_int, MEGA) + device->odr_flt;
	rest = no_os_do_div(&odr_ref, odr_mult);
	rest *= MEGA;
	no_os_do_div(&rest, odr_mult);
	device->odr_int = odr_ref;
	device->odr_flt = rest;

	*desc = device;

	return 0;
dev_err:
	no_os_free(device);

	return ret;
}

/**
 * @brief Free memory allocated by iio_ad713x_init().
 * @param desc - Pointer to the IIO device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int iio_ad713x_remove(struct ad713x_iio *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc);

	return 0;
}

struct iio_device ad713x_iio_desc = {
	.num_ch = NO_OS_ARRAY_SIZE(ad713x_channels),
	.channels = ad713x_channels,
	.attributes = channel_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.read_dev = (int32_t (*)())ad713x_iio_read_dev,
	.write_dev = NULL,
	.pre_enable = (int32_t (*)())ad713x_iio_prepare_transfer,
	.post_disable = NULL,
	.submit = NULL,
	.debug_reg_read = (int32_t (*)()) ad713x_spi_reg_read,
	.debug_reg_write = (int32_t (*)()) ad713x_spi_reg_write
};

#endif /* IIO_SUPPORT */

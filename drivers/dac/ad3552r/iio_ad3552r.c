/**************************************************************************//**
*   @file   iio_ad3552r.c
*   @brief  IIO implementation for ad3552r Driver
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "iio_types.h"
#include "ad3552r.h"
#include "error.h"
#include "util.h"

/*****************************************************************************/
/******************** Macros and Constants Definitions ***********************/
/*****************************************************************************/

#define STR(x) #x
#define XSTR(x) STR(x)

#define AD3552R_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = iio_ad3552r_attr_get,\
	.store = iio_ad3552r_attr_set\
}

#define AD3552R_DAC_CH(_idx)  {\
	.name = "dac" # _idx, \
	.ch_type = IIO_VOLTAGE,\
	.ch_out = 1,\
	.indexed = true,\
	.channel = _idx,\
	.scan_index = _idx,\
	.scan_type = &ad3552r_dac_scan_type,\
	.attributes = iio_ad3552r_ch_attributes}

#define AD3552R_DAC_PREC_CH(_idx)  {\
	.name = "dac" XSTR(_idx) "_prec", \
	.ch_type = IIO_VOLTAGE,\
	.ch_out = 1,\
	.indexed = true,\
	.channel = _idx + 2,\
	.scan_index = _idx + 2,\
	.scan_type = &ad3552r_dac_scan_type_prec}

#define AD3552R_CH_VOLTAGE		(AD3552R_CH_ENABLE + 1)

/*****************************************************************************/
/************************* Functions Definitions *****************************/
/*****************************************************************************/

static ssize_t iio_ad3552r_attr_get(void *device, char *buf, size_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t	val;
	uint16_t	val16;
	int32_t		ret;
	float		val2;

	val = 0;
	switch (priv) {
	case AD3552R_CH_VOLTAGE:
		ret = ad3552r_get_ch_voltage(device, channel->ch_num, &val2);
		if (IS_ERR_VALUE(ret))
			return ret;

		return snprintf(buf, len, "%.3f", val2);
	default:
		if (channel) {
			ret = ad3552r_get_ch_value(device, priv,
						   channel->ch_num,
						   &val16);
			val = val16;
		} else {
			ret = ad3552r_get_dev_value(device, priv, &val);
		}
		break;
	}

	if (IS_ERR_VALUE(ret))
		return ret;

	return snprintf(buf, len, "%"PRIu32"", val);
}
static ssize_t iio_ad3552r_attr_set(void *device, char *buf, size_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t val;
	int32_t	 ret;
	float	 val2;

	val = srt_to_uint32(buf);
	switch (priv) {
	case AD3552R_CH_VOLTAGE:
		ret = sscanf(buf, "%f", &val2);
		if (ret != 1)
			return -EINVAL;

		ret = ad3552r_set_ch_voltage(device, channel->ch_num, val2);
		if (IS_ERR_VALUE(ret))
			return ret;

		return len;
	default:
		if (channel)
			ret = ad3552r_set_ch_value(device, priv,
						   channel->ch_num,
						   val);
		else
			ret = ad3552r_set_dev_value(device, priv, val);
		break;
	}

	if (IS_ERR_VALUE(ret))
		return ret;

	return len;
}

void ad3552r_trigger_hander(struct ad3552r_desc *desc)
{
	printf("Hello from trigger handler\n");
}

/******************************************************************************/
/************************** IIO Types Declarations *****************************/
/******************************************************************************/

static struct scan_type ad3552r_dac_scan_type = {
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.sign = 'u',
	.is_big_endian = false
};

static struct scan_type ad3552r_dac_scan_type_prec = {
	.realbits = 16,
	.storagebits = 24,
	.shift = 0,
	.sign = 'u',
	.is_big_endian = false
};

static struct iio_attribute iio_ad3552r_ch_attributes[] = {
	AD3552R_ATTR("powerdown", AD3552R_CH_DAC_POWERDOWN),
	AD3552R_ATTR("amp_powerdown", AD3552R_CH_AMPLIFIER_POWERDOWN),
	AD3552R_ATTR("output_range", AD3552R_CH_OUTPUT_RANGE_SEL),
	AD3552R_ATTR("range_override", AD3552R_CH_RANGE_OVERRIDE),
	AD3552R_ATTR("gain_offset", AD3552R_CH_GAIN_OFFSET),
	AD3552R_ATTR("gain_offset_polarity", AD3552R_CH_GAIN_OFFSET_POLARITY),
	AD3552R_ATTR("gain_scaling_p", AD3552R_CH_GAIN_SCALING_P),
	AD3552R_ATTR("gain_scaling_n", AD3552R_CH_GAIN_SCALING_N),
	AD3552R_ATTR("trigger_sw_ldac", AD3552R_CH_TRIGGER_SOFTWARE_LDAC),
	AD3552R_ATTR("hw_ldac_mask", AD3552R_CH_HW_LDAC_MASK),
	AD3552R_ATTR("rfb", AD3552R_CH_RFB),
	AD3552R_ATTR("code_minV", AD3552R_CH_CODE_AT_MIN),
	AD3552R_ATTR("code_maxV", AD3552R_CH_CODE_AT_MAX),
	AD3552R_ATTR("select", AD3552R_CH_SELECT),
	AD3552R_ATTR("code", AD3552R_CH_CODE),
	AD3552R_ATTR("en", AD3552R_CH_ENABLE),
	AD3552R_ATTR("voltage", AD3552R_CH_VOLTAGE),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute iio_ad3552r_dev_attributes[] = {
	AD3552R_ATTR("sdo_drive_strength", AD3552R_SDO_DRIVE_STRENGTH),
	AD3552R_ATTR("vref_select", AD3552R_VREF_SELECT),
	AD3552R_ATTR("crc_en", AD3552R_CRC_ENABLE),
	AD3552R_ATTR("multi_IO_mode", AD3552R_SPI_MULTI_IO_MODE),
	AD3552R_ATTR("data_rate_mode", AD3552R_SPI_DATA_RATE),
	AD3552R_ATTR("sync_dual_spi_en", AD3552R_SPI_SYNCHRONOUS_ENABLE),
	AD3552R_ATTR("update_mode", AD3552R_UPDATE_MODE),
	AD3552R_ATTR("input_trigger_mode", AD3552R_INPUT_TRIGGER_MODE),
	AD3552R_ATTR("ldac_update_period_ns", AD3552R_DAC_UPDATE_PERIOD_NS),
	AD3552R_ATTR("precision_mode_en", AD3552R_PRECISION_MODE_ENABLED),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_channel iio_ad3552r_channels[] = {
	AD3552R_DAC_CH(0),
	AD3552R_DAC_CH(1),
	AD3552R_DAC_PREC_CH(0),
	AD3552R_DAC_PREC_CH(1),
};

/** IIO Descriptor */
struct iio_device ad3552r_iio_descriptor = {
	.num_ch = ARRAY_SIZE(iio_ad3552r_channels),
	.channels = iio_ad3552r_channels,
	.prepare_transfer = (int32_t (*)())ad3552r_prepare_write,
	.attributes = iio_ad3552r_dev_attributes,
	.write_dev = (int32_t (*)())ad3552r_write_dev,
	.debug_reg_read = (int32_t (*)())ad3552r_read_reg,
	.debug_reg_write = (int32_t (*)())ad3552r_write_reg,
	.trigger_callback = (void (*)())ad3552r_trigger_hander
};

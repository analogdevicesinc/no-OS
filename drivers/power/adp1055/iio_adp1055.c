/***************************************************************************//**
 *   @file   iio_adp1055.c
 *   @brief  Source file for the ADP1055 IIO Driver
 *   @author Ivan Gil Mercano (IvanGil.Mercano@analog.com)
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "adp1055.h"
#include "iio_adp1055.h"

#define ADP1055_IIO_OUTPUT_CHANNEL(x)		((x) - 6)
#define ADP1055_IIO_OUTPUT_OUTCHAN_MASK		NO_OS_GENMASK(3, 0)
#define ADP1055_IIO_OUTPUT_SRCHAN_MASK		NO_OS_GENMASK(5, 4)
#define ADP1055_IIO_OUT_MASK 				NO_OS_GENMASK(5, 0)
#define ADP1055_IIO_ENABLE_MASK(x)			NO_OS_BIT(x)
#define ADP1055_IIO_FREQ_MANT(x)		((x) > 48 && (x) < 64  ? -4 : \
                       				(x) > 64 && (x) < 128 ? -3 : \
                        			(x) > 128 && (x) < 256 ? -2 : \
                        			(x) > 256 && (x) < 510 ? -1 : 0)

static const char *const adp1055_enable_avail[2] = {
	"Disabled", "Enabled"
};

static const char *const adp1055_freq_avail[] = {
	"49KHz",
	"59KHz",
	"60KHz",
	"65KHz",
	"71KHz",
	"78KHz",
	"87KHz",
	"104KHz",
	"120KHz",
	"130KHz",
	"136KHz",
	"142KHz",
	"149KHz",
	"184KHz",
	"223KHz",
	"250KHz",
	"284KHz",
	"329KHz",
	"338KHz",
	"347KHz",
	"357KHz",
	"379KHz",
	"397KHz",
	"403KHz",
	"410KHz",
	"97.5KHz",
	"111.5KHz",
	"156.5KHz",
	"164.5KHz",
	"173.5KHz",
	"195.5KHz",
	"201.5KHz",
	"208.5KHz",
	"215.5KHz",
	"231.5KHz",
	"240.5KHz",
	"260.5KHz",
	"271.5KHz",
	"297.5KHz",
	"312.5KHz",
	"320.5KHz",
	"367.5KHz",
	"390.5KHz"
};

static const uint32_t adp1055_freq2_avail[] = {
	49,
	59,
	60,
	65,
	71,
	78,
	87,
	104,
	120,
	130,
	136,
	142,
	149,
	184,
	223,
	250,
	284,
	329,
	338,
	347,
	357,
	379,
	397,
	403,
	410,
	97,
	111,
	156,
	164,
	173,
	195,
	201,
	208,
	215,
	231,
	240,
	260,
	271,
	297,
	312,
	320,
	367,
	390,
};

enum adp1055_iio_enable_type {
	ADP1055_IIO_OUT_ENABLE,
	ADP1055_IIO_FEEDFORWARD_ENABLE,
	ADP1055_IIO_PULSE_ENABLE,
	ADP1055_IIO_FREQ_SYNC_ENABLE
};

enum ADP1055_iio_input_chan_type {
	ADP1055_IIO_VIN_CHAN,
	ADP1055_IIO_IIN_CHAN,
	ADP1055_IIO_VOUT_CHAN,
	ADP1055_IIO_IOUT_CHAN,
	ADP1055_IIO_TEMP2_CHAN,
	ADP1055_IIO_TEMP3_CHAN
};

enum ADP1055_iio_output_chan_type {
	ADP1055_IIO_OUTA_CHAN = 0x6,
	ADP1055_IIO_OUTB_CHAN = 0x7,
	ADP1055_IIO_OUTC_CHAN = 0x8,
	ADP1055_IIO_OUTD_CHAN = 0x9,
	ADP1055_IIO_SR1_CHAN = 0xA,
	ADP1055_IIO_SR2_CHAN = 0xB
};

enum ADP1055_iio_vout_value_type {
	ADP1055_IIO_VOUT_COMMAND_VALUE = ADP1055_VOUT_COMMAND,
	ADP1055_IIO_VOUT_SCALE_MONITOR_VALUE = ADP1055_VOUT_SCALE_MONITOR,
	ADP1055_IIO_VOUT_OFFSET_VALUE = ADP1055_VOUT_CAL_OFFSET,
};

static int adp1055_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);

static int adp1055_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp1055_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int adp1055_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp1055_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp1055_iio_read_freq(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp1055_iio_read_freq_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int adp1055_iio_write_freq(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp1055_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int adp1055_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static struct iio_attribute adp1055_input_attrs[] = {
	{
		.name = "raw",
		.show = adp1055_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adp1055_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp1055_output_attrs[] = {
	{
		.name = "enable",
		.show = adp1055_iio_read_enable,
		.store = adp1055_iio_write_enable,
		.priv = ADP1055_IIO_OUT_ENABLE
	},
	{
		.name = "frequency",
		.show = adp1055_iio_read_freq,
		.store = adp1055_iio_write_freq,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "freqyency_available",
		.show = adp1055_iio_read_freq_available,
		.shared = IIO_SHARED_BY_ALL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp1055_global_attrs[] = {
	{
		.name = "vout_command",
		.show = adp1055_iio_read_vout,
		.store = adp1055_iio_write_vout,
		.priv = ADP1055_IIO_VOUT_COMMAND_VALUE
	},
	{
		.name = "vout_scale_monitor",
		.show = adp1055_iio_read_vout,
		.store = adp1055_iio_write_vout,
		.priv = ADP1055_IIO_VOUT_SCALE_MONITOR_VALUE
	},
	{
		.name = "vout_offset",
		.show = adp1055_iio_read_vout,
		.store = adp1055_iio_write_vout,
		.priv = ADP1055_IIO_VOUT_OFFSET_VALUE
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp1055_debug_attrs[] = {
	{
		.name = "status_vout",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_VOUT_TYPE
	},
	{
		.name = "status_iout",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_IOUT_TYPE
	},
	{
		.name = "status_input",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_INPUT_TYPE
	},
	{
		.name = "status_temp",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_TEMP_TYPE
	},
	{
		.name = "status_cml",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_CML_TYPE
	},
	{
		.name = "status_other",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_OTHER_TYPE
	},
	{
		.name = "status_word",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_WORD_TYPE
	},
	{
		.name = "status_mfr",
		.show = adp1055_iio_read_status,
		.priv = ADP1055_STATUS_MFR_TYPE
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adp1055_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_VIN_CHAN,
		.address = ADP1055_IIO_VIN_CHAN,
		.attributes = adp1055_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = ADP1055_IIO_IIN_CHAN,
		.address = ADP1055_IIO_IIN_CHAN,
		.attributes = adp1055_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_VOUT_CHAN,
		.address = ADP1055_IIO_VOUT_CHAN,
		.attributes = adp1055_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = ADP1055_IIO_IOUT_CHAN,
		.address = ADP1055_IIO_IOUT_CHAN,
		.attributes = adp1055_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp2",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = ADP1055_IIO_TEMP2_CHAN,
		.address = ADP1055_IIO_TEMP2_CHAN,
		.attributes = adp1055_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp3",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = ADP1055_IIO_TEMP3_CHAN,
		.address = ADP1055_IIO_TEMP3_CHAN,
		.attributes = adp1055_input_attrs,
		.ch_out = false
	},
	{
		.name = "outa",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_OUTA_CHAN,
		.address = ADP1055_IIO_OUTA_CHAN,
		.attributes = adp1055_output_attrs,
		.ch_out = true,
	},
	{
		.name = "outb",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_OUTB_CHAN,
		.address = ADP1055_IIO_OUTB_CHAN,
		.attributes = adp1055_output_attrs,
		.ch_out = true,
	},
	{
		.name = "outc",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_OUTA_CHAN,
		.address = ADP1055_IIO_OUTA_CHAN,
		.attributes = adp1055_output_attrs,
		.ch_out = true,
	},
	{
		.name = "outd",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_OUTB_CHAN,
		.address = ADP1055_IIO_OUTB_CHAN,
		.attributes = adp1055_output_attrs,
		.ch_out = true,
	},
	{
		.name = "sr1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_SR1_CHAN,
		.address = ADP1055_IIO_SR1_CHAN,
		.attributes = adp1055_output_attrs,
		.ch_out = true,
	},
	{
		.name = "sr2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1055_IIO_SR2_CHAN,
		.address = ADP1055_IIO_SR2_CHAN,
		.attributes = adp1055_output_attrs,
		.ch_out = true,
	}
};

static struct iio_device adp1055_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adp1055_channels),
	.channels = adp1055_channels,
	.attributes = adp1055_global_attrs,
	.debug_attributes = adp1055_debug_attrs
};

/**
 * @brief Handles the read request for the raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 *
 */
static int adp1055_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int ret;
	uint16_t mant, val;
	uint8_t exp;
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;

	switch (channel->address) {
	case ADP1055_IIO_VIN_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_VIN, &mant, &exp);
		break;
	case ADP1055_IIO_IIN_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_IIN, &mant, &exp);
		break;
	case ADP1055_IIO_TEMP2_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_TEMP2, &mant, &exp);
		break;
	case ADP1055_IIO_TEMP3_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_TEMP3, &mant, &exp);
		break;
	case ADP1055_IIO_IOUT_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_IOUT, &mant, &exp);
		break;
	case ADP1055_IIO_VOUT_CHAN:
		ret = adp1055_read_vsense(adp1055, &mant);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&mant);
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	val = no_os_field_get(ADP1055_EXP_MASK, exp) |
	      no_os_field_get(ADP1055_MANT_MASK, mant);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the read request for scale attribute
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int vals[2], ret;
	uint16_t mant, exp;
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;

	switch (channel->address) {
	case ADP1055_IIO_VIN_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_VIN, &mant, (uint8_t *)&exp);
		break;
	case ADP1055_IIO_IIN_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_IIN, &mant, (uint8_t *)&exp);
		break;
	case ADP1055_IIO_TEMP2_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_TEMP2, &mant, (uint8_t *)&exp);
		break;
	case ADP1055_IIO_TEMP3_CHAN:
		ret = adp1055_read_value(adp1055, ADP1055_VALUE_TEMP3, &mant, (uint8_t *)&exp);
		break;
	case ADP1055_IIO_VOUT_CHAN:
		ret = adp1055_read_vsense(adp1055, &mant);

		vals[0] = mant;
		vals[1] = 10;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2,
					(int32_t *)vals);
	}
	if (ret)
		return ret;

	vals[0] = no_os_sign_extend16(mant, 10);
	vals[1] = no_os_sign_extend16(exp, 4) * (-1);

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t *)vals);
}

/**
 * @brief Handles the read request for status debug attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 */
static int adp1055_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int ret;
	uint16_t status;
	struct  adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;

	ret = adp1055_read_status(adp1055, priv, &status);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&status);

}

/**
 * @brief handles the read request for vout global attributes
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int ret;
	uint16_t val;
	uint8_t data[2];
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;

	if (!dev)
		return -EINVAL;

	iio_adp1055 = dev;

	if (!iio_adp1055->adp1055_desc)
		return -EINVAL;

	ret = adp1055_read(adp1055, priv, data, 2);
	if (ret)
		return ret;

	val = no_os_get_unaligned_le16(data);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the write request for vout global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	uint16_t val;
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	switch (priv) {
	case ADP1055_IIO_VOUT_COMMAND_VALUE:
		return adp1055_vout_value(adp1055, val, val + 1);
	case ADP1055_IIO_VOUT_OFFSET_VALUE:
		return adp1055_vout_offset(adp1055, val);
	case ADP1055_IIO_VOUT_SCALE_MONITOR_VALUE:
		return adp1055_vout_scale(adp1055, no_os_field_get(ADP1055_EXP_MASK, val),
					  no_os_field_get(ADP1055_MANT_MASK, val));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for freq attribute
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_read_freq(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int ret;
	uint16_t val;
	uint8_t data[2];
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;

	if (!dev)
		return -EINVAL;

	iio_adp1055 = dev;

	if (!iio_adp1055->adp1055_desc)
		return -EINVAL;

	ret = adp1055_read(adp1055, priv, data, 2);
	if (ret)
		return ret;

	val = no_os_get_unaligned_le16(data);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the read request for freq_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_read_freq_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1055_freq_avail); i++)
		length += sprintf(buf + length, "%s ", adp1055_freq_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for freq attribute
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_write_freq(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;
	int16_t mant, exp;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1055_freq_avail); i++)
		if (!strcmp(buf, adp1055_freq_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp1055_freq_avail))
		return -EINVAL;

	exp = ADP1055_IIO_FREQ_MANT(adp1055_freq2_avail[i]);

	mant = (int16_t)(adp1055_freq2_avail[i] / (1 << exp));

	return adp1055_set_pwm(adp1055, ADP1055_IIO_OUTPUT_CHANNEL(channel->address),
			       exp, mant);
}

/**
 * @brief Handles the read request for enable attribute
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;
	int ret;
	uint8_t val;

	ret = adp1055_read(adp1055, ADP1055_PWM_DISABLE_SETTING, &val, 1);
	if (ret)
		return ret;

	val = no_os_field_prep(ADP1055_IIO_OUT_MASK,
			       no_os_field_get(ADP1055_IIO_OUTPUT_OUTCHAN_MASK, val) |
			       no_os_field_get(ADP1055_IIO_OUTPUT_SRCHAN_MASK, val));

	if (no_os_field_get(ADP1055_IIO_ENABLE_MASK(ADP1055_IIO_OUTPUT_CHANNEL(
				    channel->address)), val))
		return sprintf(buf, "%s ", adp1055_enable_avail[1]);

	return sprintf(buf, "%s ", adp1055_enable_avail[0]);
}

/**
 * @brief Handles the write request for enable attribute
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1055_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adp1055_iio_desc *iio_adp1055 = dev;
	struct adp1055_desc *adp1055 = iio_adp1055->adp1055_desc;
	uint32_t i;


	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1055_enable_avail); i++)
		if (!strcmp(buf, adp1055_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp1055_enable_avail))
		return -EINVAL;

	return adp1055_set_pwm(adp1055,
			       i ? ADP1055_IIO_OUTPUT_CHANNEL(channel->address) : ADP1055_DISABLE_ALL,
			       adp1055->freq_exp, adp1055->freq_mant);
}

/**
 * @brief Initializes the ADP1050 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int adp1055_iio_init(struct adp1055_iio_desc **iio_desc,
		     struct adp1055_iio_desc_init_param *init_param)
{
	struct adp1055_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->adp1055_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = adp1055_init(&descriptor->adp1055_desc,
			   init_param->adp1055_init_param);
	if (ret)
		goto free_desc;

	ret = adp1055_vout_scale(descriptor->adp1055_desc,
				 no_os_field_get(ADP1055_EXP_MASK, init_param->vout_scale_monitor),
				 no_os_field_get(ADP1055_MANT_MASK, init_param->vout_scale_monitor));
	if (ret)
		goto free_desc;

	ret = adp1055_write(descriptor->adp1055_desc, ADP1055_VIN_SCALE_MONITOR,
			    init_param->vin_scale_monitor, 2);
	if (ret)
		goto free_desc;

	ret = adp1055_write(descriptor->adp1055_desc, ADP1055_IIN_SCALE_MONITOR,
			    init_param->iin_scale_monitor, 2);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &adp1055_iio_dev;

	*iio_desc = descriptor;

	return 0;

free_desc:
	adp1055_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int adp1055_iio_remove(struct adp1055_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	adp1055_remove(iio_desc->adp1055_desc);
	no_os_free(iio_desc);

	return 0;
}

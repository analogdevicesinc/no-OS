/***************************************************************************//**
 *   @file   iio_adp1050.c
 *   @brief  Source file for the ADP1050 IIO Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "adp1050.h"
#include "iio_adp1050.h"

#define ADP1050_IIO_FLOAT_FREQ_INDEX		24
#define ADP1050_IIO_FLOAT_FREQ(x)		((((x) - 0xF800) / 2))
#define ADP1050_IIO_FLOAT_FREQ_ENUM(x)		((((x) * 2) + 0xF800) + 1)
#define ADP1050_IIO_MODULATION_AVAIL_SIZE	8
#define ADP1050_IIO_MODULATION_AVAIL_MASK(x)	NO_OS_GENMASK((((x) * 2) + 1), ((x) * 2))
#define ADP1050_IIO_MODULATION_AVAIL_SELECT(x)	NO_OS_BIT(((x) * 2) + 1)
#define ADP1050_IIO_OUTPUT_CHANNELS		4
#define ADP1050_IIO_OUTPUT_CHANNEL(x)		((x) - 4)
#define ADP1050_IIO_OUTA_DUTY_CYCLE_REPORTING	NO_OS_BIT(2)
#define ADP1050_IIO_OUTB_DUTY_CYCLE_REPORTING	NO_OS_BIT(3)
#define ADP1050_IIO_OUTA_OUTB_MASK		NO_OS_GENMASK(1, 0)
#define ADP1050_IIO_SR1_SR2_MASK		NO_OS_GENMASK(5, 4)
#define ADP1050_IIO_OUT_MASK			NO_OS_GENMASK(3, 0)
#define ADP1050_IIO_ENABLE_MASK(x)		NO_OS_BIT(x)
#define ADP1050_IIO_PULSE_DEFAULT_VAL		0x05

static const char *const adp1050_enable_avail[2] = {
	"Disabled", "Enabled"
};

static const char *const adp1050_freq_avail[] = {
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

static const uint32_t adp1050_freq2_avail[] = {
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

static const char *const adp1050_loop_avail[2] = {
	[ADP1050_CLOSE_LOOP] = "Close_Loop",
	[ADP1050_OPEN_LOOP] = "Open_Loop",
};

static const char *const adp1050_modulation_avail[] = {
	[2] = "ADP1050_OUTA_SR1_FALLING_MOD_POSITIVE",
	[3] = "ADP1050_OUTA_SR1_FALLING_MOD_NEGATIVE",
	[8] = "ADP1050_OUTA_SR1_RISING_MOD_POSITIVE",
	[12] = "ADP1050_OUTA_SR1_RISING_MOD_NEGATIVE",
	[32] = "ADP1050_OUTB_SR2_FALLING_MOD_POSITIVE",
	[48] = "ADP1050_OUTB_SR2_FALLING_MOD_NEGATIVE",
	[128] = "ADP1050_OUTB_SR2_RISING_MOD_POSITIVE",
	[192] = "ADP1050_OUTB_SR2_RISING_MOD_NEGATIVE",
};

enum adp1050_iio_enable_type {
	ADP1050_IIO_OUT_ENABLE,
	ADP1050_IIO_FEEDFORWARD_ENABLE,
	ADP1050_IIO_PULSE_ENABLE,
	ADP1050_IIO_FREQ_SYNC_ENABLE
};

enum adp1050_iio_vout_value_type {
	ADP1050_IIO_VOUT_COMMAND_VALUE = ADP1050_VOUT_COMMAND,
	ADP1050_IIO_VOUT_MARGIN_LOW_VALUE = ADP1050_VOUT_MARGIN_LOW,
	ADP1050_IIO_VOUT_MARGIN_HIGH_VALUE = ADP1050_VOUT_MARGIN_HIGH,
	ADP1050_IIO_VOUT_SCALE_MONITOR_VALUE = ADP1050_VOUT_SCALE_MONITOR,
	ADP1050_IIO_VOUT_OFFSET_VALUE = ADP1050_VOUT_CAL_OFFSET,
};

enum adp1050_iio_input_chan_type {
	ADP1050_IIO_VIN_CHAN,
	ADP1050_IIO_IIN_CHAN,
	ADP1050_IIO_VOUT_CHAN,
	ADP1050_IIO_TEMP_CHAN
};

enum adp1050_iio_output_chan_type {
	ADP1050_IIO_OUTA_CHAN = 4,
	ADP1050_IIO_OUTB_CHAN = 5,
	ADP1050_IIO_SR1_CHAN = 6,
	ADP1050_IIO_SR2_CHAN = 7
};

static int adp1050_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);

static int adp1050_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp1050_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int adp1050_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp1050_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp1050_iio_read_freq(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp1050_iio_read_freq_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int adp1050_iio_write_freq(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp1050_iio_read_loop(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp1050_iio_read_loop_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int adp1050_iio_write_loop(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp1050_iio_read_modulation(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int adp1050_iio_read_modulation_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int adp1050_iio_write_modulation(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int adp1050_iio_read_duty_cycle(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int adp1050_iio_write_duty_cycle(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int adp1050_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int adp1050_iio_read_enable_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int adp1050_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static struct iio_attribute adp1050_input_attrs[] = {
	{
		.name = "raw",
		.show = adp1050_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adp1050_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp1050_output_attrs[] = {
	{
		.name = "enable",
		.show = adp1050_iio_read_enable,
		.store = adp1050_iio_write_enable,
		.priv = ADP1050_IIO_OUT_ENABLE
	},
	{
		.name = "enable_available",
		.show = adp1050_iio_read_enable_available,
		.priv = ADP1050_IIO_OUT_ENABLE
	},
	{
		.name = "frequency",
		.show = adp1050_iio_read_freq,
		.store = adp1050_iio_write_freq,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "frequency_available",
		.show = adp1050_iio_read_freq_available,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "duty_cycle",
		.show = adp1050_iio_read_duty_cycle,
		.store = adp1050_iio_write_duty_cycle
	},
	{
		.name = "modulation",
		.show = adp1050_iio_read_modulation,
		.store = adp1050_iio_write_modulation
	},
	{
		.name = "modulation_available",
		.show = adp1050_iio_read_modulation_available,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp1050_global_attrs[] = {
	{
		.name = "vout_command",
		.show = adp1050_iio_read_vout,
		.store = adp1050_iio_write_vout,
		.priv = ADP1050_IIO_VOUT_COMMAND_VALUE
	},
	{
		.name = "vout_margin_low",
		.show = adp1050_iio_read_vout,
		.store = adp1050_iio_write_vout,
		.priv = ADP1050_IIO_VOUT_MARGIN_LOW_VALUE
	},
	{
		.name = "vout_margin_high",
		.show = adp1050_iio_read_vout,
		.store = adp1050_iio_write_vout,
		.priv = ADP1050_IIO_VOUT_MARGIN_HIGH_VALUE
	},
	{
		.name = "vout_scale_monitor",
		.show = adp1050_iio_read_vout,
		.store = adp1050_iio_write_vout,
		.priv = ADP1050_IIO_VOUT_SCALE_MONITOR_VALUE
	},
	{
		.name = "vout_offset",
		.show = adp1050_iio_read_vout,
		.store = adp1050_iio_write_vout,
		.priv = ADP1050_IIO_VOUT_OFFSET_VALUE
	},
	{
		.name = "freq_sync",
		.show = adp1050_iio_read_enable,
		.store = adp1050_iio_write_enable,
		.priv = ADP1050_IIO_FREQ_SYNC_ENABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "freq_sync_available",
		.show = adp1050_iio_read_enable_available,
		.priv = ADP1050_IIO_FREQ_SYNC_ENABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "feedforward",
		.show = adp1050_iio_read_enable,
		.store = adp1050_iio_write_enable,
		.priv = ADP1050_IIO_FEEDFORWARD_ENABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "feedforward_available",
		.show = adp1050_iio_read_enable_available,
		.priv = ADP1050_IIO_FEEDFORWARD_ENABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "loop",
		.show = adp1050_iio_read_loop,
		.store = adp1050_iio_write_loop,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "loop_available",
		.show = adp1050_iio_read_loop_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "pulse_skipping",
		.show = adp1050_iio_read_enable,
		.store = adp1050_iio_write_enable,
		.priv = ADP1050_IIO_PULSE_ENABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "pulse_available",
		.show = adp1050_iio_read_enable_available,
		.priv = ADP1050_IIO_PULSE_ENABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp1050_debug_attrs[] = {
	{
		.name = "status_vout",
		.show = adp1050_iio_read_status,
		.priv = ADP1050_STATUS_VOUT_TYPE
	},
	{
		.name = "status_input",
		.show = adp1050_iio_read_status,
		.priv = ADP1050_STATUS_INPUT_TYPE,
	},
	{
		.name = "status_temperature",
		.show = adp1050_iio_read_status,
		.priv = ADP1050_STATUS_TEMPERATURE_TYPE,
	},
	{
		.name = "status_cml",
		.show = adp1050_iio_read_status,
		.priv = ADP1050_STATUS_CML_TYPE
	},
	{
		.name = "status_word",
		.show = adp1050_iio_read_status,
		.priv = ADP1050_STATUS_WORD_TYPE
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adp1050_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1050_IIO_VIN_CHAN,
		.address = ADP1050_IIO_VIN_CHAN,
		.attributes = adp1050_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = ADP1050_IIO_IIN_CHAN,
		.address = ADP1050_IIO_IIN_CHAN,
		.attributes = adp1050_input_attrs,
		.ch_out = false,
	},
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1050_IIO_VOUT_CHAN,
		.address = ADP1050_IIO_VOUT_CHAN,
		.attributes = adp1050_input_attrs,
		.ch_out = false,
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = ADP1050_IIO_TEMP_CHAN,
		.address = ADP1050_IIO_TEMP_CHAN,
		.attributes = adp1050_input_attrs,
		.ch_out = false,
	},
	{
		.name = "outa",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1050_IIO_OUTA_CHAN,
		.address = ADP1050_IIO_OUTA_CHAN,
		.attributes = adp1050_output_attrs,
		.ch_out = true,
	},
	{
		.name = "outb",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1050_IIO_OUTB_CHAN,
		.address = ADP1050_IIO_OUTB_CHAN,
		.attributes = adp1050_output_attrs,
		.ch_out = true,
	},
	{
		.name = "sr1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1050_IIO_SR1_CHAN,
		.address = ADP1050_IIO_SR1_CHAN,
		.attributes = adp1050_output_attrs,
		.ch_out = true,
	},
	{
		.name = "sr2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP1050_IIO_SR2_CHAN,
		.address = ADP1050_IIO_SR2_CHAN,
		.attributes = adp1050_output_attrs,
		.ch_out = true,
	}
};

static struct iio_device adp1050_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adp1050_channels),
	.channels = adp1050_channels,
	.attributes = adp1050_global_attrs,
	.debug_attributes = adp1050_debug_attrs
};

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int ret;
	uint16_t mant, val;
	uint8_t exp;
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;

	switch (channel->address) {
	case ADP1050_IIO_VIN_CHAN:
		ret = adp1050_read_value(adp1050, &mant, &exp, ADP1050_VIN);
		break;
	case ADP1050_IIO_IIN_CHAN:
		ret = adp1050_read_value(adp1050, &mant, &exp, ADP1050_IIN);
		break;
	case ADP1050_IIO_VOUT_CHAN:
		ret = adp1050_read_vsense(adp1050, &mant);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1,  (int32_t *)&mant);
	case ADP1050_IIO_TEMP_CHAN:
		ret = adp1050_read_value(adp1050, &mant, &exp, ADP1050_TEMP);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	val = no_os_field_get(ADP1050_EXP_MASK, exp) |
	      no_os_field_get(ADP1050_MANT_MASK, mant);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int vals[2], ret;
	uint16_t mant, exp;
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;

	switch (channel->address) {
	case ADP1050_IIO_VIN_CHAN:
		ret = adp1050_read_value(adp1050, &mant, (uint8_t *)&exp, ADP1050_VIN);
		break;
	case ADP1050_IIO_IIN_CHAN:
		ret = adp1050_read_value(adp1050, &mant, (uint8_t *)&exp, ADP1050_IIN);
		break;
	case ADP1050_IIO_VOUT_CHAN:
		ret = adp1050_read_vsense(adp1050, &mant);

		vals[0] = mant;
		vals[1] = 10;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2,
					(int32_t *)vals);
	case ADP1050_IIO_TEMP_CHAN:
		ret = adp1050_read_value(adp1050, &mant, (uint8_t *)&exp, ADP1050_TEMP);
		break;
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
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int ret;
	uint16_t status;
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;

	ret = adp1050_read_status(adp1050, priv, &status);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&status);
}

/**
 * @brief Handles the read request for vout global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int ret;
	uint16_t val;
	uint8_t data[2];
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;

	if (!dev)
		return -EINVAL;

	iio_adp1050 = dev;

	if (!iio_adp1050->adp1050_desc)
		return -EINVAL;

	ret = adp1050_read(adp1050, priv, data, 2);
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
static int adp1050_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	uint16_t val;
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	switch (priv) {
	case ADP1050_IIO_VOUT_COMMAND_VALUE:
		return adp1050_vout_value(adp1050, val, val + 1);
	case ADP1050_IIO_VOUT_MARGIN_LOW_VALUE:
		return adp1050_write(adp1050, ADP1050_MARGIN_LOW, val, 2);
	case ADP1050_IIO_VOUT_MARGIN_HIGH_VALUE:
		return adp1050_write(adp1050, ADP1050_MARGIN_HIGH, val,	2);
	case ADP1050_IIO_VOUT_OFFSET_VALUE:
		return adp1050_vout_offset(adp1050, val);
	case ADP1050_IIO_VOUT_SCALE_MONITOR_VALUE:
		return adp1050_vout_scale(adp1050, no_os_field_get(ADP1050_EXP_MASK, val),
					  no_os_field_get(ADP1050_MANT_MASK, val));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for freq attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_freq(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_freq2_avail); i++) {
		if (i < ADP1050_IIO_FLOAT_FREQ_INDEX) {
			if (adp1050_freq2_avail[i] == adp1050->freq)
				return sprintf(buf, "%s ", adp1050_freq_avail[i]);
		} else {
			if (adp1050_freq2_avail[i] == (uint32_t)ADP1050_IIO_FLOAT_FREQ(adp1050->freq))
				return sprintf(buf, "%s ", adp1050_freq_avail[i]);
		}
	}

	return -EINVAL;
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
static int adp1050_iio_read_freq_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_freq_avail); i++)
		length += sprintf(buf + length, "%s ", adp1050_freq_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for freq attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_write_freq(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_freq_avail); i++)
		if (!strcmp(buf, adp1050_freq_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp1050_freq_avail))
		return -EINVAL;

	if (i < ADP1050_IIO_FLOAT_FREQ_INDEX)
		return adp1050_set_pwm(adp1050, ADP1050_IIO_OUTPUT_CHANNEL(channel->address),
				       (enum adp1050_freq)adp1050_freq2_avail[i]);

	return adp1050_set_pwm(adp1050, ADP1050_IIO_OUTPUT_CHANNEL(channel->address),
			       (enum adp1050_freq)ADP1050_IIO_FLOAT_FREQ_ENUM(adp1050_freq2_avail[i]));
}

/**
 * @brief Handles the read request for loop attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_loop(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;

	return sprintf(buf, "%s", adp1050_loop_avail[adp1050->loop]);
}

/**
 * @brief Handles the read request for loop_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_loop_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_loop_avail); i++)
		length += sprintf(buf + length, "%s ", adp1050_loop_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for loop attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_write_loop(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_loop_avail); i++)
		if (!strcmp(buf, adp1050_loop_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp1050_loop_avail))
		return -EINVAL;

	if (i)
		return adp1050_set_open_loop(adp1050, 0, 0,
					     ADP1050_IIO_OUTPUT_CHANNEL(channel->address));

	return adp1050_set_close_loop(adp1050);
}

/**
 * @brief Handles the read request for modulation attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_modulation(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	int ret;
	uint8_t val;
	uint32_t i;

	switch (channel->address) {
	case ADP1050_IIO_OUTA_CHAN:
	case ADP1050_IIO_OUTB_CHAN:
		ret = adp1050_read(adp1050, ADP1050_OUTA_OUTB_MODULATION_SETTINGS, &val, 1);
		break;
	case ADP1050_IIO_SR1_CHAN:
	case ADP1050_IIO_SR2_CHAN:
		ret = adp1050_read(adp1050, ADP1050_SR1_SR2_MODULATION_SETTINGS, &val, 1);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	for (i = 0; i < ADP1050_IIO_MODULATION_AVAIL_SIZE; i++)
		if (no_os_field_get(ADP1050_IIO_MODULATION_AVAIL_MASK(i), val))
			return sprintf(buf, "%s ",
				       adp1050_modulation_avail[ADP1050_IIO_MODULATION_AVAIL_MASK(i)]);

	return -EINVAL;
}

/**
 * @brief Handles the read request for modulation_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_modulation_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_modulation_avail); i++)
		length += sprintf(buf + length, "%s ", adp1050_modulation_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for modulation attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_write_modulation(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	uint32_t i;

	for (i = 0; i < ADP1050_IIO_OUTPUT_CHANNELS; i++)
		if (!strcmp(buf, adp1050_modulation_avail[ADP1050_IIO_MODULATION_AVAIL_MASK(
					i)]))
			break;

	if (i == ADP1050_IIO_OUTPUT_CHANNELS)
		return -EINVAL;

	return adp1050_pwm_modulation(adp1050,
				      (enum adp1050_mod)ADP1050_IIO_MODULATION_AVAIL_SELECT(i),
				      ADP1050_IIO_OUTPUT_CHANNEL(channel->address),
				      ADP1050_IIO_MODULATION_AVAIL_MASK(i) % 2);
}

/**
 * @brief Handles the read request for duty_cycle attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_duty_cycle(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	int ret, vals[2];
	uint16_t mant, exp;

	switch (channel->address) {
	case ADP1050_IIO_OUTA_CHAN:
	case ADP1050_IIO_SR1_CHAN:
		ret = adp1050_write(adp1050, ADP1050_DUTY_CYCLE_READING_SETTINGS,
				    ADP1050_IIO_OUTA_DUTY_CYCLE_REPORTING, 1);
		break;
	case ADP1050_IIO_OUTB_CHAN:
	case ADP1050_IIO_SR2_CHAN:
		ret = adp1050_write(adp1050, ADP1050_DUTY_CYCLE_READING_SETTINGS,
				    ADP1050_IIO_OUTB_DUTY_CYCLE_REPORTING, 1);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	ret = adp1050_read_value(adp1050, &mant, (uint8_t *)&exp, ADP1050_DUTY_CYCLE);
	if (ret)
		return ret;

	vals[0] = no_os_sign_extend16(mant, 10);
	vals[1] = no_os_sign_extend16(exp, 4);

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t *)vals);
}

/**
 * @brief Handles the write request for duty_cycle attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_write_duty_cycle(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	uint16_t val;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return adp1050_pwm_duty_cycle(adp1050, no_os_field_get(ADP1050_MSB_MASK, val),
				      no_os_field_get(ADP1050_LSB_MASK, val),
				      ADP1050_IIO_OUTPUT_CHANNEL(channel->address));
}

/**
 * @brief Handles the read request for enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	int ret;
	uint8_t val;
	uint32_t mask;

	switch (priv) {
	case ADP1050_IIO_OUT_ENABLE:
		ret = adp1050_read(adp1050, ADP1050_PWM_OUTPUT_DISABLE, &val, 1);
		if (ret)
			return ret;

		val = no_os_field_prep(ADP1050_IIO_OUT_MASK,
				       no_os_field_get(ADP1050_IIO_OUTA_OUTB_MASK,
						       val) | no_os_field_get(ADP1050_IIO_SR1_SR2_MASK, val));

		if (no_os_field_get(ADP1050_IIO_ENABLE_MASK(ADP1050_IIO_OUTPUT_CHANNEL(
					    channel->address)), val))
			return sprintf(buf, "%s ", adp1050_enable_avail[1]);

		return sprintf(buf, "%s ", adp1050_enable_avail[0]);
	case ADP1050_IIO_FEEDFORWARD_ENABLE:
		ret = adp1050_read(adp1050, ADP1050_FEEDFORWARD_SS_FILTER_GAIN, &val, 1);
		if (ret)
			return ret;

		switch (adp1050->loop) {
		case ADP1050_OPEN_LOOP:
			mask = ADP1050_FEEDFORWARD_OL_ENABLE;
			break;
		case ADP1050_CLOSE_LOOP:
			mask = ADP1050_FEEDFORWARD_CL_ENABLE;
			break;
		default:
			return -EINVAL;
		}

		if (no_os_field_get(mask, val))
			return sprintf(buf, "%s ", adp1050_enable_avail[1]);

		return sprintf(buf, "%s ", adp1050_enable_avail[0]);
	case ADP1050_IIO_PULSE_ENABLE:
		if (adp1050->loop == ADP1050_CLOSE_LOOP)
			return sprintf(buf, "%s ", adp1050_enable_avail[0]);

		ret = adp1050_read(adp1050, ADP1050_OL_OPERATION_SETTINGS, &val, 1);
		if (ret)
			return ret;

		if (no_os_field_get(ADP1050_PULSE_SKIPPING_ENABLE,
				    val) == ADP1050_PULSE_SKIPPING_ENABLE)
			return sprintf(buf, "%s ", adp1050_enable_avail[1]);

		return sprintf(buf, "%s ", adp1050_enable_avail[0]);
	case ADP1050_IIO_FREQ_SYNC_ENABLE:
		ret = adp1050_read(adp1050, ADP1050_SYNCH_GENERAL_SETTINGS, &val, 1);
		if (ret)
			return ret;

		if (no_os_field_get(ADP1050_FREQ_SYNC_ON, val))
			return sprintf(buf, "%s ", adp1050_enable_avail[1]);

		return sprintf(buf, "%s ", adp1050_enable_avail[0]);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for enable_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_read_enable_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_enable_avail); i++)
		length += sprintf(buf + length, "%s ", adp1050_enable_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp1050_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adp1050_iio_desc *iio_adp1050 = dev;
	struct adp1050_desc *adp1050 = iio_adp1050->adp1050_desc;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp1050_enable_avail); i++)
		if (!strcmp(buf, adp1050_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp1050_enable_avail))
		return -EINVAL;

	switch (priv) {
	case ADP1050_IIO_OUT_ENABLE:
		return adp1050_set_pwm(adp1050,
				       i ? ADP1050_IIO_OUTPUT_CHANNEL(channel->address) : ADP1050_DISABLE_ALL,
				       adp1050->freq);
	case ADP1050_IIO_FEEDFORWARD_ENABLE:
		return adp1050_set_feedforward(adp1050, i ? true : false);
	case ADP1050_IIO_PULSE_ENABLE:
		return adp1050_pulse_skipping(adp1050, ADP1050_IIO_PULSE_DEFAULT_VAL,
					      i ? true : false);
	case ADP1050_IIO_FREQ_SYNC_ENABLE:
		return adp1050_freq_sync(adp1050, i ? true : false);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Initializes the ADP1050 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int adp1050_iio_init(struct adp1050_iio_desc **iio_desc,
		     struct adp1050_iio_desc_init_param *init_param)
{
	struct adp1050_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->adp1050_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = adp1050_init(&descriptor->adp1050_desc,
			   init_param->adp1050_init_param);
	if (ret)
		goto free_desc;

	ret = adp1050_unlock_pass(descriptor->adp1050_desc, ADP1050_CHIP_DEFAULT_PASS,
				  ADP1050_CHIP_PASS);
	if (ret)
		goto free_desc;

	ret = adp1050_unlock_pass(descriptor->adp1050_desc, ADP1050_EEPROM_DEFAULT_PASS,
				  ADP1050_EEPROM_PASS);
	if (ret)
		goto free_desc;

	ret = adp1050_unlock_pass(descriptor->adp1050_desc, ADP1050_TRIM_DEFAULT_PASS,
				  ADP1050_TRIM_PASS);
	if (ret)
		goto free_desc;

	ret = adp1050_vout_scale(descriptor->adp1050_desc,
				 no_os_field_get(ADP1050_EXP_MASK, init_param->vout_scale_monitor),
				 no_os_field_get(ADP1050_MANT_MASK, init_param->vout_scale_monitor));
	if (ret)
		goto free_desc;

	ret = adp1050_write(descriptor->adp1050_desc, ADP1050_VIN_SCALE_MONITOR,
			    init_param->vin_scale_monitor, 2);
	if (ret)
		goto free_desc;

	ret = adp1050_write(descriptor->adp1050_desc, ADP1050_IIN_SCALE_MONITOR,
			    init_param->iin_scale_monitor, 2);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &adp1050_iio_dev;

	*iio_desc = descriptor;

	return 0;

free_desc:
	adp1050_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int adp1050_iio_remove(struct adp1050_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	adp1050_remove(iio_desc->adp1050_desc);
	no_os_free(iio_desc);

	return 0;
}

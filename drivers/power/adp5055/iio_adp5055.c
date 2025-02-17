/***************************************************************************//**
 *   @file   iio_adp5055.c
 *   @brief  Source file for the ADP5055 IIO Driver
 *   @author Angelo Catapang (angelo.catapang@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "adp5055.h"
#include "iio_adp5055.h"

static const char *const adp5055_enable_avail[2] = {
	"DISABLED",
	"ENABLED"
};

static const char *const adp5055_enable_delay_avail[8] = {
	"NO_DELAY",
	"1_TSET",
	"2_TSET",
	"3_TSET",
	"4_TSET",
	"5_TSET",
	"6_TSET",
	"7_TSET"
};

static const char *const adp5055_disable_delay_avail[8] = {
	"NO_DELAY",
	"2_TSET",
	"4_TSET",
	"6_TSET",
	"8_TSET",
	"10_TSET",
	"12_TSET",
	"14_TSET"
};

static const char *const adp5055_dvs_interval_avail[4] = {
	"125US",
	"62US",
	"31US",
	"15US"
};

static const char *const adp5055_fast_transient_avail[4] = {
	"NONE",
	"1.5WINDOW_3G",
	"1.5WINDOW_5G",
	"2.5WINDOW_5G"
};

static const char *const adp5055_enable_mode_avail[4] = {
	"HW_ONLY",
	"SW_ONLY",
	"HW_AND_SW",
	"HW_OR_SW"
};

enum adp5055_iio_enable_type {
	ADP5055_IIO_VID_ENABLE,
	ADP5055_IIO_AUTO_PWM_PSM_ENABLE,
	ADP5055_IIO_OUTPUT_DISCHARGE_ENABLE,
	ADP5055_IIO_PG_HW_ENABLE,
	ADP5055_IIO_DVS_AUTO_ENABLE,
	ADP5055_IIO_OCP_BLANKING_ENABLE,
	ADP5055_IIO_PG_DELAY_ENABLE,
	ADP5055_IIO_VID_GO_ENABLE
};

enum adp5055_iio_output_chan_type {
	ADP5055_IIO_VOUT1_CHAN,
	ADP5055_IIO_VOUT2_CHAN,
	ADP5055_IIO_VOUT3_CHAN
};

static int adp5055_iio_read_status_lch(void *dev, char *buf, uint32_t len,
	const struct iio_ch_info *channel,
	intptr_t priv);

static int adp5055_iio_read_status_cml(void *dev, char *buf, uint32_t len,
	const struct iio_ch_info *channel,
	intptr_t priv);

static int adp5055_iio_read_enable_mode_available(void *dev, char *buf,
	uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int adp5055_iio_read_enable_mode(void *dev, char *buf, uint32_t len,
	const struct iio_ch_info *channel,
	intptr_t priv);

static int adp5055_iio_write_enable_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int adp5055_iio_read_pg(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int adp5055_iio_read_fast_transient_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int adp5055_iio_read_fast_transient(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int adp5055_iio_write_fast_transient(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int adp5055_iio_read_vid_low_limit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp5055_iio_write_vid_low_limit(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp5055_iio_read_vid_high_limit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp5055_iio_write_vid_high_limit(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp5055_iio_read_dvs_interval_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int adp5055_iio_read_dvs_interval(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int adp5055_iio_write_dvs_interval(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int adp5055_iio_read_vid_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int adp5055_iio_write_vid_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int adp5055_iio_read_disable_delay_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int adp5055_iio_read_disable_delay(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int adp5055_iio_write_disable_delay(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int adp5055_iio_read_enable_delay_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int adp5055_iio_read_enable_delay(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int adp5055_iio_write_enable_delay(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int adp5055_iio_read_enable_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int adp5055_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int adp5055_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static struct iio_attribute adp5055_output_attrs[] = {
	{
		.name = "enable",
		.show = adp5055_iio_read_enable,
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_VID_ENABLE
	},
	{
		.name = "enable_available",
		.show = adp5055_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "auto_pwm_psm",
		.show = adp5055_iio_read_enable,
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_AUTO_PWM_PSM_ENABLE
	},
	{
		.name = "auto_pwm_psm_available",
		.show = adp5055_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "output_discharge",
		.show = adp5055_iio_read_enable,
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_OUTPUT_DISCHARGE_ENABLE
	},
	{
		.name = "output_discharge_available",
		.show = adp5055_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "enable_delay",
		.show = adp5055_iio_read_enable_delay,
		.store = adp5055_iio_write_enable_delay
	},
	{
		.name = "enable_delay_available",
		.show = adp5055_iio_read_enable_delay_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "disable_delay",
		.show = adp5055_iio_read_disable_delay,
		.store = adp5055_iio_write_disable_delay
	},
	{
		.name = "disable_delay_available",
		.show = adp5055_iio_read_disable_delay_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "vid_raw",
		.show = adp5055_iio_read_vid_raw,
		.store = adp5055_iio_write_vid_raw
	},
	{
		.name = "vid_go",
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_VID_GO_ENABLE
	},
	{
		.name = "vid_go_available",
		.show = adp5055_iio_read_enable_available
	},
	{
		.name = "dvs_interval",
		.show = adp5055_iio_read_dvs_interval,
		.store = adp5055_iio_write_dvs_interval
	},
	{
		.name = "dvs_interval_available",
		.show = adp5055_iio_read_dvs_interval_available
	},
	{
		.name = "vid_high_limit",
		.show = adp5055_iio_read_vid_high_limit,
		.store = adp5055_iio_write_vid_high_limit
	},
	{
		.name = "vid_low_limit",
		.show = adp5055_iio_read_vid_low_limit,
		.store = adp5055_iio_write_vid_low_limit
	},
	{
		.name = "fast_transient",
		.show = adp5055_iio_read_fast_transient,
		.store = adp5055_iio_write_fast_transient
	},
	{
		.name = "fast_transient_available",
		.show = adp5055_iio_read_fast_transient_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "pg_hw_enable",
		.show = adp5055_iio_read_enable,
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_PG_HW_ENABLE
	},
	{
		.name = "pg_hw_enable_available",
		.show = adp5055_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "pg",
		.show = adp5055_iio_read_pg
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp5055_global_attrs[] = {
	{
		.name = "enable_mode",
		.show = adp5055_iio_read_enable_mode,
		.store = adp5055_iio_write_enable_mode
	},
	{
		.name = "enable_mode_available",
		.show = adp5055_iio_read_enable_mode_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "dvs_auto_enable",
		.show = adp5055_iio_read_enable,
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_DVS_AUTO_ENABLE
	},
	{
		.name = "dvs_auto_enable_available",
		.show = adp5055_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "ocp_blanking_enable",
		.show = adp5055_iio_read_enable,
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_OCP_BLANKING_ENABLE
	},
	{
		.name = "ocp_blanking_enable_available",
		.show = adp5055_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "pg_delay_enable",
		.show = adp5055_iio_read_enable,
		.store = adp5055_iio_write_enable,
		.priv = ADP5055_IIO_PG_DELAY_ENABLE
	},
	{
		.name = "pg_delay_enable_available",
		.show = adp5055_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp5055_debug_attrs[] = {
	{
		.name = "status_cml",
		.show = adp5055_iio_read_status_cml,
	},
	{
		.name = "status_lch",
		.show = adp5055_iio_read_status_lch,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adp5055_channels[] = {
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP5055_IIO_VOUT1_CHAN,
		.address = ADP5055_IIO_VOUT1_CHAN,
		.attributes = adp5055_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP5055_IIO_VOUT2_CHAN,
		.address = ADP5055_IIO_VOUT2_CHAN,
		.attributes = adp5055_output_attrs,
		.ch_out = true,
	},
	{
		.name = "vout3",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = ADP5055_IIO_VOUT3_CHAN,
		.address = ADP5055_IIO_VOUT3_CHAN,
		.attributes = adp5055_output_attrs,
		.ch_out = true,
	}
};

static struct iio_device adp5055_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adp5055_channels),
	.channels = adp5055_channels,
	.attributes = adp5055_global_attrs,
	.debug_attributes = adp5055_debug_attrs
};

/**
 * @brief Handles the read request for status_lch debug attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_status_lch(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint8_t val;

	ret = adp5055_read(adp5055, ADP5055_STATUS_LCH, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the read request for status_cml debug attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_status_cml(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint8_t val;

	ret = adp5055_read_status(adp5055, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the read request for enable_mode_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_enable_mode_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_enable_mode_avail); i++)
		length += sprintf(buf + length, "%s ", adp5055_enable_mode_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for enable_mode attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_enable_mode(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint8_t val;

	ret = adp5055_read(adp5055, ADP5055_CTRL_MODE1, &val);
	if (ret)
		return ret;

	return sprintf(buf, "%s ", adp5055_enable_mode_avail[no_os_field_get(ADP5055_EN_MODE_MASK, val)]);
}

/**
 * @brief Handles the write request for the enable_mode attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_enable_mode(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	int ret;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_enable_mode_avail); i++)
		if (!strcmp(buf, adp5055_enable_mode_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp5055_enable_mode_avail))
		return -EINVAL;

	return adp5055_set_enable_mode(adp5055, (enum adp5055_en_mode) i);
}

/**
 * @brief Handles the read request for pg attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_pg(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t mask;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		mask = ADP5055_DVS_INTVAL1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		mask = ADP5055_DVS_INTVAL2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		mask = ADP5055_DVS_INTVAL3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, ADP5055_PG_READ, &val);
	if (ret)
		return ret;

	val = no_os_field_get(mask, val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the read request for fast_transient_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_fast_transient_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_fast_transient_avail); i++)
		length += sprintf(buf + length, "%s ", adp5055_fast_transient_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for fast_transient attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_fast_transient(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t mask;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		mask = ADP5055_FT1_TH;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		mask = ADP5055_FT2_TH;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		mask = ADP5055_FT3_TH;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, ADP5055_FT_CFG, &val);
	if (ret)
		return ret;

	return sprintf(buf, "%s ", adp5055_fast_transient_avail[no_os_field_get(mask, val)]);
}

/**
 * @brief Handles the write request for the fast_transient attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_fast_transient(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	int ret;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_fast_transient_avail); i++)
		if (!strcmp(buf, adp5055_fast_transient_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp5055_fast_transient_avail))
		return -EINVAL;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		ch = ADP5055_VOUT1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		ch = ADP5055_VOUT2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		ch = ADP5055_VOUT3;
		break;
	default:
		return -EINVAL;
	}

	return adp5055_set_fast_transient(adp5055, ch, (enum adp5055_ft_th) i);
}

/**
 * @brief Handles the read request for vid_low_limit attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_vid_low_limit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t reg;
	uint8_t val;

	if (!dev)
		return -EINVAL;

	iio_adp5055 = dev;

	if (!iio_adp5055->adp5055_desc)
		return -EINVAL;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		reg = ADP5055_DVS_LIM1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		reg = ADP5055_DVS_LIM2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		reg = ADP5055_DVS_LIM3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, reg, &val);
	if (ret)
		return ret;

	val = no_os_field_get(ADP5055_VIDX_LOW_MASK, val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the write request for vid_low_limit attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_vid_low_limit(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		ch = ADP5055_VOUT1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		ch = ADP5055_VOUT2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		ch = ADP5055_VOUT3;
		break;
	default:
		return -EINVAL;
	}

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return adp5055_set_vidx_lim(adp5055, ch, ADP5055_VIDX_LOW, val);
}

/**
 * @brief Handles the read request for vid_high_limit attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_vid_high_limit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t reg;
	uint8_t val;

	if (!dev)
		return -EINVAL;

	iio_adp5055 = dev;

	if (!iio_adp5055->adp5055_desc)
		return -EINVAL;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		reg = ADP5055_DVS_LIM1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		reg = ADP5055_DVS_LIM2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		reg = ADP5055_DVS_LIM3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, reg, &val);
	if (ret)
		return ret;

	val = no_os_field_get(ADP5055_VIDX_HIGH_MASK, val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the write request for vid_high_limit attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_vid_high_limit(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		ch = ADP5055_VOUT1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		ch = ADP5055_VOUT2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		ch = ADP5055_VOUT3;
		break;
	default:
		return -EINVAL;
	}

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return adp5055_set_vidx_lim(adp5055, ch, ADP5055_VIDX_HIGH, val);
}

/**
 * @brief Handles the read request for dvs_interval_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_dvs_interval_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_dvs_interval_avail); i++)
		length += sprintf(buf + length, "%s ", adp5055_dvs_interval_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for dvs_interval attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_dvs_interval(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t mask;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		mask = ADP5055_DVS_INTVAL1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		mask = ADP5055_DVS_INTVAL2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		mask = ADP5055_DVS_INTVAL3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, ADP5055_DVS_CFG, &val);
	if (ret)
		return ret;

	return sprintf(buf, "%s ", adp5055_dvs_interval_avail[no_os_field_get(mask, val)]);
}

/**
 * @brief Handles the write request for the dvs_interval attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_dvs_interval(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	int ret;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_dvs_interval_avail); i++)
		if (!strcmp(buf, adp5055_dvs_interval_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp5055_dvs_interval_avail))
		return -EINVAL;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		ch = ADP5055_VOUT1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		ch = ADP5055_VOUT2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		ch = ADP5055_VOUT3;
		break;
	default:
		return -EINVAL;
	}

	return adp5055_set_dvs_interval(adp5055, ch, (enum adp5055_dvs_intval) i);
}

/**
 * @brief Handles the read request for vid_raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_vid_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t reg;
	uint8_t val;

	if (!dev)
		return -EINVAL;

	iio_adp5055 = dev;

	if (!iio_adp5055->adp5055_desc)
		return -EINVAL;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		reg = ADP5055_VID1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		reg = ADP5055_VID2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		reg = ADP5055_VID3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, reg, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Handles the write request for vid_raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_vid_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		ch = ADP5055_VOUT1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		ch = ADP5055_VOUT2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		ch = ADP5055_VOUT3;
		break;
	default:
		return -EINVAL;
	}

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return adp5055_set_vid_code(adp5055, ch, val);
}

/**
 * @brief Handles the read request for disable_delay_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_disable_delay_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_disable_delay_avail); i++)
		length += sprintf(buf + length, "%s ", adp5055_disable_delay_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for disable_delay attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_disable_delay(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t reg;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		reg = ADP5055_DLY1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		reg = ADP5055_DLY2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		reg = ADP5055_DLY3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, reg, &val);
	if (ret)
		return ret;

	return sprintf(buf, "%s ", adp5055_disable_delay_avail[no_os_field_get(ADP5055_DIS_DLY_MASK, val)]);
}

/**
 * @brief Handles the write request for the disable_delay attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_disable_delay(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	int ret;
	uint32_t i;
	uint32_t reg;
	uint8_t val;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_disable_delay_avail); i++)
		if (!strcmp(buf, adp5055_disable_delay_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp5055_disable_delay_avail))
		return -EINVAL;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		ch = ADP5055_VOUT1;
		reg = ADP5055_DLY1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		ch = ADP5055_VOUT2;
		reg = ADP5055_DLY2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		ch = ADP5055_VOUT3;
		reg = ADP5055_DLY3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, reg, &val);
	if (ret)
		return ret;

	return adp5055_set_enable_disable_delay(adp5055, ch, (enum adp5055_en_dly) no_os_field_get(ADP5055_EN_DLY_MASK, val), (enum adp5055_dis_dly) i);
}

/**
 * @brief Handles the read request for enable_delay_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_enable_delay_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_enable_delay_avail); i++)
		length += sprintf(buf + length, "%s ", adp5055_enable_delay_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for enable_delay attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_read_enable_delay(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint32_t reg;
	uint8_t val;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		reg = ADP5055_DLY1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		reg = ADP5055_DLY2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		reg = ADP5055_DLY3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, reg, &val);
	if (ret)
		return ret;

	return sprintf(buf, "%s ", adp5055_enable_delay_avail[no_os_field_get(ADP5055_EN_DLY_MASK, val)]);
}

/**
 * @brief Handles the write request for the enable_delay attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int adp5055_iio_write_enable_delay(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	int ret;
	uint32_t i;
	uint32_t reg;
	uint8_t val;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_enable_delay_avail); i++)
		if (!strcmp(buf, adp5055_enable_delay_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp5055_enable_delay_avail))
		return -EINVAL;

	switch(channel->address) {
	case ADP5055_IIO_VOUT1_CHAN:
		ch = ADP5055_VOUT1;
		reg = ADP5055_DLY1;
		break;
	case ADP5055_IIO_VOUT2_CHAN:
		ch = ADP5055_VOUT2;
		reg = ADP5055_DLY2;
		break;
	case ADP5055_IIO_VOUT3_CHAN:
		ch = ADP5055_VOUT3;
		reg = ADP5055_DLY3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp5055_read(adp5055, reg, &val);
	if (ret)
		return ret;

	return adp5055_set_enable_disable_delay(adp5055, ch, (enum adp5055_en_dly) i, (enum adp5055_dis_dly) no_os_field_get(ADP5055_DIS_DLY_MASK, val));
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
static int adp5055_iio_read_enable_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_enable_avail); i++)
		length += sprintf(buf + length, "%s ", adp5055_enable_avail[i]);

	return length;
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
static int adp5055_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	int ret;
	uint8_t val;
	uint32_t mask;

	switch (priv) {
	case ADP5055_IIO_VID_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			mask = ADP5055_CH1_ON;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			mask = ADP5055_CH2_ON;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			mask = ADP5055_CH3_ON;
			break;
		default:
			return -EINVAL;
		}

		ret = adp5055_read(adp5055, ADP5055_CTRL123, &val);
		if (ret)
			return ret;

		if (no_os_field_get(mask, val))
			return sprintf(buf, "%s ", adp5055_enable_avail[1]);

		return sprintf(buf, "%s ", adp5055_enable_avail[0]);
	case ADP5055_IIO_AUTO_PWM_PSM_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			mask = ADP5055_PSM1_ON;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			mask = ADP5055_PSM2_ON;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			mask = ADP5055_PSM3_ON;
			break;
		default:
			return -EINVAL;
		}

		ret = adp5055_read(adp5055, ADP5055_CTRL_MODE2, &val);
		if (ret)
			return ret;

		if (no_os_field_get(mask, val))
			return sprintf(buf, "%s ", adp5055_enable_avail[1]);

		return sprintf(buf, "%s ", adp5055_enable_avail[0]);
	case ADP5055_IIO_OUTPUT_DISCHARGE_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			mask = ADP5055_DSCG1_ON;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			mask = ADP5055_DSCG2_ON;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			mask = ADP5055_DSCG3_ON;
			break;
		default:
			return -EINVAL;
		}

		ret = adp5055_read(adp5055, ADP5055_CTRL_MODE2, &val);
		if (ret)
			return ret;

		if (no_os_field_get(mask, val))
			return sprintf(buf, "%s ", adp5055_enable_avail[1]);

		return sprintf(buf, "%s ", adp5055_enable_avail[0]);
	case ADP5055_IIO_PG_HW_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			mask = ADP5055_PG1_MASK;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			mask = ADP5055_PG2_MASK;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			mask = ADP5055_PG3_MASK;
			break;
		default:
			return -EINVAL;
		}

		ret = adp5055_read(adp5055, ADP5055_PG_CFG, &val);
		if (ret)
			return ret;

		if (no_os_field_get(mask, val))
			return sprintf(buf, "%s ", adp5055_enable_avail[0]);

		return sprintf(buf, "%s ", adp5055_enable_avail[1]);
	case ADP5055_IIO_DVS_AUTO_ENABLE:
		ret = adp5055_read(adp5055, ADP5055_CTRL_MODE1, &val);
		if (ret)
			return ret;

		if (no_os_field_get(ADP5055_DVS_AUTO, val))
			return sprintf(buf, "%s ", adp5055_enable_avail[1]);

		return sprintf(buf, "%s ", adp5055_enable_avail[0]);
	case ADP5055_IIO_OCP_BLANKING_ENABLE:
		ret = adp5055_read(adp5055, ADP5055_CTRL_MODE2, &val);
		if (ret)
			return ret;

		if (no_os_field_get(ADP5055_OCP_BLANKING, val))
			return sprintf(buf, "%s ", adp5055_enable_avail[1]);

		return sprintf(buf, "%s ", adp5055_enable_avail[0]);
	case ADP5055_IIO_PG_DELAY_ENABLE:
		ret = adp5055_read(adp5055, ADP5055_PG_CFG, &val);
		if (ret)
			return ret;

		if (no_os_field_get(ADP5055_PWRGD_DLY, val))
			return sprintf(buf, "%s ", adp5055_enable_avail[1]);

		return sprintf(buf, "%s ", adp5055_enable_avail[0]);
	default:
		return -EINVAL;
	}
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
static int adp5055_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adp5055_iio_desc *iio_adp5055 = dev;
	struct adp5055_desc *adp5055 = iio_adp5055->adp5055_desc;
	enum adp5055_channel ch;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp5055_enable_avail); i++)
		if (!strcmp(buf, adp5055_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(adp5055_enable_avail))
		return -EINVAL;

	switch (priv) {
	case ADP5055_IIO_VID_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			ch = ADP5055_VOUT1;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			ch = ADP5055_VOUT2;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			ch = ADP5055_VOUT3;
			break;
		default:
			return -EINVAL;
		}
		return adp5055_enable_channel(adp5055, ch, i ? true : false);
	case ADP5055_IIO_AUTO_PWM_PSM_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			ch = ADP5055_VOUT1;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			ch = ADP5055_VOUT2;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			ch = ADP5055_VOUT3;
			break;
		default:
			return -EINVAL;
		}
		return adp5055_set_auto_pwm_psm(adp5055, ch, i ? true : false);
	case ADP5055_IIO_OUTPUT_DISCHARGE_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			ch = ADP5055_VOUT1;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			ch = ADP5055_VOUT2;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			ch = ADP5055_VOUT3;
			break;
		default:
			return -EINVAL;
		}
		return adp5055_set_output_discharge(adp5055, ch, i ? true : false);
	case ADP5055_IIO_PG_HW_ENABLE:
		switch(channel->address) {
		case ADP5055_IIO_VOUT1_CHAN:
			ch = ADP5055_VOUT1;
			break;
		case ADP5055_IIO_VOUT2_CHAN:
			ch = ADP5055_VOUT2;
			break;
		case ADP5055_IIO_VOUT3_CHAN:
			ch = ADP5055_VOUT3;
			break;
		default:
			return -EINVAL;
		}
		return adp5055_set_pg_channel_output(adp5055, ch, i ? true : false);
	case ADP5055_IIO_DVS_AUTO_ENABLE:
		return adp5055_set_dvs_auto(adp5055, i ? true : false);
	case ADP5055_IIO_OCP_BLANKING_ENABLE:
		return adp5055_set_ocp_blanking(adp5055, i ? true : false);
	case ADP5055_IIO_PG_DELAY_ENABLE:
		return adp5055_set_pg_hw_delay(adp5055, i ? true : false);
	case ADP5055_IIO_VID_GO_ENABLE:
		if (i)
			return adp5055_start_vout(adp5055, ch);
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Initializes the ADP5055 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int adp5055_iio_init(struct adp5055_iio_desc **iio_desc,
		     struct adp5055_iio_desc_init_param *init_param)
{
	struct adp5055_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->adp5055_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = adp5055_init(&descriptor->adp5055_desc,
			   init_param->adp5055_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &adp5055_iio_dev;

	*iio_desc = descriptor;

	return 0;

free_desc:
	adp5055_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int adp5055_iio_remove(struct adp5055_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	adp5055_remove(iio_desc->adp5055_desc);
	no_os_free(iio_desc);

	return 0;
}

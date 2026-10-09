/***************************************************************************//**
 *   @file   iio_adp20086.c
 *   @brief  Source file for the ADP20086 IIO driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_units.h"
#include "adp20086.h"
#include "iio_adp20086.h"

/*
 * Which global attributes exist depends on how the part is wired, so the
 * template below is filtered per instance into adp20086_iio_desc.global_attrs.
 * Each entry is tagged with the pin it needs, or ADP20086_ATTR_ALWAYS.
 */
#define ADP20086_ATTR_ALWAYS	0
#define ADP20086_ATTR_NEEDS_EN	1
#define ADP20086_ATTR_NEEDS_INTB 2

struct adp20086_global_attr_template {
	struct iio_attribute attr;
	uint8_t requires;
};

static const char *const adp20086_bool_avail = "0 1";

static const char *const adp20086_soft_start_avail[4] = {
	"0.5", "1", "2", "4"
};

static const char *const adp20086_soft_shutdown_avail[4] = {
	"0.25", "0.5", "1", "2"
};

/* Voltage channel indices carried in iio_channel.address. */
enum adp20086_iio_voltage_chan {
	ADP20086_IIO_VOUT1,
	ADP20086_IIO_VOUT2,
	ADP20086_IIO_VOUT3,
	ADP20086_IIO_VOUT4,
	ADP20086_IIO_VIN,
	ADP20086_IIO_VDD,
};

/* Current channel indices carried in iio_channel.address (map 1:1 to outputs). */
enum adp20086_iio_current_chan {
	ADP20086_IIO_IOUT1,
	ADP20086_IIO_IOUT2,
	ADP20086_IIO_IOUT3,
	ADP20086_IIO_IOUT4,
};

/* priv ids for the per-channel configuration show/store handlers. */
enum adp20086_iio_cfg_attr {
	ADP20086_IIO_ENABLE,
	ADP20086_IIO_CURRENT_LIMIT,
	ADP20086_IIO_IOPEN,
	ADP20086_IIO_LOAD_DETECT,
	ADP20086_IIO_UVOUT,
	ADP20086_IIO_UVIN,
	ADP20086_IIO_OVIN,
	ADP20086_IIO_SOFT_START,
	ADP20086_IIO_SOFT_SHUTDOWN,
	ADP20086_IIO_OPEN_PROT,
	ADP20086_IIO_LOAD_DETECT_EN,
};

/* priv ids for the device-global show/store handlers. */
enum adp20086_iio_global_attr {
	ADP20086_IIO_GPIO_EN,
	ADP20086_IIO_INTB,
	ADP20086_IIO_PECE,
	ADP20086_IIO_PART_ID,
	ADP20086_IIO_REVISION,
	ADP20086_IIO_INT_MASK,
	ADP20086_IIO_INT_MASK2,
	ADP20086_IIO_INT_MASK3,
};

static int adp20086_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int adp20086_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);
static int adp20086_iio_read_cfg(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int adp20086_iio_write_cfg(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int adp20086_iio_read_bool_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_soft_start_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_soft_shutdown_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_global(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int adp20086_iio_write_global(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);
static int adp20086_iio_read_status(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int adp20086_iio_read_bist(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int adp20086_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval);
static int adp20086_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval);

static struct iio_attribute adp20086_vdd_attrs[] = {
	{
		.name = "raw",
		.show = adp20086_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adp20086_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp20086_vin_attrs[] = {
	{
		.name = "raw",
		.show = adp20086_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adp20086_iio_read_scale,
	},
	{
		.name = "uvin",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_UVIN,
	},
	{
		.name = "ovin",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_OVIN,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp20086_vout_attrs[] = {
	{
		.name = "raw",
		.show = adp20086_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adp20086_iio_read_scale,
	},
	{
		.name = "uvout",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_UVOUT,
	},
	END_ATTRIBUTES_ARRAY
};

/*
 * Current channels carry both the IOUT measurement (raw/scale) and the
 * per-output configuration, since the four current channels map 1:1 to the
 * four device outputs.
 */
static struct iio_attribute adp20086_current_attrs[] = {
	{
		.name = "raw",
		.show = adp20086_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adp20086_iio_read_scale,
	},
	{
		.name = "enable",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_ENABLE,
	},
	{
		.name = "enable_available",
		.show = adp20086_iio_read_bool_available,
		.shared = IIO_SHARED_BY_TYPE,
	},
	{
		.name = "current_limit",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_CURRENT_LIMIT,
	},
	{
		.name = "iopen",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_IOPEN,
	},
	{
		.name = "load_detect",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_LOAD_DETECT,
	},
	{
		.name = "soft_start",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_SOFT_START,
	},
	{
		.name = "soft_start_available",
		.show = adp20086_iio_read_soft_start_available,
		.shared = IIO_SHARED_BY_TYPE,
	},
	{
		.name = "soft_shutdown",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_SOFT_SHUTDOWN,
	},
	{
		.name = "soft_shutdown_available",
		.show = adp20086_iio_read_soft_shutdown_available,
		.shared = IIO_SHARED_BY_TYPE,
	},
	{
		.name = "open_prot",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_OPEN_PROT,
	},
	{
		.name = "open_prot_available",
		.show = adp20086_iio_read_bool_available,
		.shared = IIO_SHARED_BY_TYPE,
	},
	{
		.name = "load_detect_en",
		.show = adp20086_iio_read_cfg,
		.store = adp20086_iio_write_cfg,
		.priv = ADP20086_IIO_LOAD_DETECT_EN,
	},
	{
		.name = "load_detect_en_available",
		.show = adp20086_iio_read_bool_available,
		.shared = IIO_SHARED_BY_TYPE,
	},
	END_ATTRIBUTES_ARRAY
};


static const struct adp20086_global_attr_template adp20086_global_attrs[] = {
	{
		.attr = {
			.name = "gpio_en",
			.show = adp20086_iio_read_global,
			.store = adp20086_iio_write_global,
			.priv = ADP20086_IIO_GPIO_EN,
		},
		.requires = ADP20086_ATTR_NEEDS_EN,
	},
	{
		.attr = {
			.name = "gpio_en_available",
			.show = adp20086_iio_read_bool_available,
		},
		.requires = ADP20086_ATTR_NEEDS_EN,
	},
	{
		.attr = {
			.name = "intb",
			.show = adp20086_iio_read_global,
			.priv = ADP20086_IIO_INTB,
		},
		.requires = ADP20086_ATTR_NEEDS_INTB,
	},
	{
		.attr = {
			.name = "pece",
			.show = adp20086_iio_read_global,
			.priv = ADP20086_IIO_PECE,
		},
	},
	{
		.attr = {
			.name = "part_id",
			.show = adp20086_iio_read_global,
			.priv = ADP20086_IIO_PART_ID,
		},
	},
	{
		.attr = {
			.name = "revision",
			.show = adp20086_iio_read_global,
			.priv = ADP20086_IIO_REVISION,
		},
	},
	{
		.attr = {
			.name = "int_mask",
			.show = adp20086_iio_read_global,
			.store = adp20086_iio_write_global,
			.priv = ADP20086_IIO_INT_MASK,
		},
	},
	{
		.attr = {
			.name = "int_mask2",
			.show = adp20086_iio_read_global,
			.store = adp20086_iio_write_global,
			.priv = ADP20086_IIO_INT_MASK2,
		},
	},
	{
		.attr = {
			.name = "int_mask3",
			.show = adp20086_iio_read_global,
			.store = adp20086_iio_write_global,
			.priv = ADP20086_IIO_INT_MASK3,
		},
	},
};

static struct iio_attribute adp20086_debug_attrs[] = {
	{
		.name = "status",
		.show = adp20086_iio_read_status,
	},
	{
		.name = "bist",
		.show = adp20086_iio_read_bist,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adp20086_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = ADP20086_IIO_VIN,
		.address = ADP20086_IIO_VIN,
		.attributes = adp20086_vin_attrs,
		.ch_out = false,
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = ADP20086_IIO_VOUT1,
		.address = ADP20086_IIO_VOUT1,
		.attributes = adp20086_vout_attrs,
		.ch_out = false,
	},
	{
		.name = "vout2",
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = ADP20086_IIO_VOUT2,
		.address = ADP20086_IIO_VOUT2,
		.attributes = adp20086_vout_attrs,
		.ch_out = false,
	},
	{
		.name = "vout3",
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = ADP20086_IIO_VOUT3,
		.address = ADP20086_IIO_VOUT3,
		.attributes = adp20086_vout_attrs,
		.ch_out = false,
	},
	{
		.name = "vout4",
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = ADP20086_IIO_VOUT4,
		.address = ADP20086_IIO_VOUT4,
		.attributes = adp20086_vout_attrs,
		.ch_out = false,
	},
	{
		.name = "vdd",
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = ADP20086_IIO_VDD,
		.address = ADP20086_IIO_VDD,
		.attributes = adp20086_vdd_attrs,
		.ch_out = false,
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = true,
		.channel = ADP20086_IIO_IOUT1,
		.address = ADP20086_IIO_IOUT1,
		.attributes = adp20086_current_attrs,
		.ch_out = false,
	},
	{
		.name = "iout2",
		.ch_type = IIO_CURRENT,
		.indexed = true,
		.channel = ADP20086_IIO_IOUT2,
		.address = ADP20086_IIO_IOUT2,
		.attributes = adp20086_current_attrs,
		.ch_out = false,
	},
	{
		.name = "iout3",
		.ch_type = IIO_CURRENT,
		.indexed = true,
		.channel = ADP20086_IIO_IOUT3,
		.address = ADP20086_IIO_IOUT3,
		.attributes = adp20086_current_attrs,
		.ch_out = false,
	},
	{
		.name = "iout4",
		.ch_type = IIO_CURRENT,
		.indexed = true,
		.channel = ADP20086_IIO_IOUT4,
		.address = ADP20086_IIO_IOUT4,
		.attributes = adp20086_current_attrs,
		.ch_out = false,
	},
};

/* .attributes is filled in per instance by adp20086_iio_init(). */
static struct iio_device adp20086_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adp20086_channels),
	.channels = adp20086_channels,
	.debug_attributes = adp20086_debug_attrs,
	.debug_reg_read = adp20086_iio_reg_read,
	.debug_reg_write = adp20086_iio_reg_write,
};

/**
 * @brief Read the raw ADC code of a measurement channel.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written on success, negative error code otherwise.
 */
static int adp20086_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	uint8_t code;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	switch (channel->type) {
	case IIO_VOLTAGE:
		switch (channel->address) {
		case ADP20086_IIO_VIN:
			ret = adp20086_read_vin_code(adp20086, &code);
			break;
		case ADP20086_IIO_VDD:
			ret = adp20086_read_vdd_code(adp20086, &code);
			break;
		case ADP20086_IIO_VOUT1:
		case ADP20086_IIO_VOUT2:
		case ADP20086_IIO_VOUT3:
		case ADP20086_IIO_VOUT4:
			ret = adp20086_read_vout_code(adp20086,
						      channel->address - ADP20086_IIO_VOUT1, &code);
			break;
		default:
			return -EINVAL;
		}
		break;
	case IIO_CURRENT:
		ret = adp20086_read_iout_code(adp20086, channel->address, &code);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	val = code;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the scale of a measurement channel.
 *
 * raw x scale yields millivolts for voltage channels and milliamps for current
 * channels.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written on success, negative error code otherwise.
 */
static int adp20086_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t vals[2];

	switch (channel->type) {
	case IIO_VOLTAGE:
		if (channel->address == ADP20086_IIO_VDD)
			vals[0] = ADP20086_VDD_LSB_UV / MILLI;
		else
			vals[0] = ADP20086_VIN_LSB_UV / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
	case IIO_CURRENT:
		vals[0] = ADP20086_IOUT_LSB_UA / MILLI;
		vals[1] = (ADP20086_IOUT_LSB_UA % MILLI) * MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read a per-channel configuration attribute.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written on success, negative error code otherwise.
 */
static int adp20086_iio_read_cfg(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	enum adp20086_channel ch;
	enum adp20086_soft_start ssu;
	enum adp20086_soft_shutdown ssd;
	uint32_t micro;
	uint8_t reg;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;
	ch = channel->address;

	switch (priv) {
	case ADP20086_IIO_ENABLE:
		ret = adp20086_get_enabled_channels(adp20086, ch, &reg);
		if (ret)
			return ret;

		val = reg;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_CURRENT_LIMIT:
		ret = adp20086_get_ilim_ua(adp20086, ch, &micro);
		if (ret)
			return ret;

		val = micro / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_IOPEN:
		ret = adp20086_get_iopen_ua(adp20086, ch, &micro);
		if (ret)
			return ret;

		val = micro / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_LOAD_DETECT:
		ret = adp20086_get_ldet_threshold_ua(adp20086, ch, &micro);
		if (ret)
			return ret;

		val = micro / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_UVOUT:
		ret = adp20086_get_uvout_uv(adp20086, ch, &micro);
		if (ret)
			return ret;

		val = micro / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_UVIN:
		ret = adp20086_get_uvin_uv(adp20086, &micro);
		if (ret)
			return ret;

		val = micro / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_OVIN:
		ret = adp20086_get_ovin_uv(adp20086, &micro);
		if (ret)
			return ret;

		val = micro / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_SOFT_START:
		ret = adp20086_get_soft_start(adp20086, ch, &ssu);
		if (ret)
			return ret;

		return sprintf(buf, "%s ", adp20086_soft_start_avail[ssu]);
	case ADP20086_IIO_SOFT_SHUTDOWN:
		ret = adp20086_get_soft_shutdown(adp20086, ch, &ssd);
		if (ret)
			return ret;

		return sprintf(buf, "%s ", adp20086_soft_shutdown_avail[ssd]);
	case ADP20086_IIO_OPEN_PROT:
		ret = adp20086_get_enabled_open_protection(adp20086, ch, &reg);
		if (ret)
			return ret;

		val = reg;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_LOAD_DETECT_EN:
		ret = adp20086_get_enabled_load_detection(adp20086, ch, &reg);
		if (ret)
			return ret;

		val = reg;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write a per-channel configuration attribute.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer holding the requested value.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 0 on success, negative error code otherwise.
 */
static int adp20086_iio_write_cfg(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	enum adp20086_channel ch;
	uint32_t i;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;
	ch = channel->address;

	switch (priv) {
	case ADP20086_IIO_ENABLE:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		return val ? adp20086_enable_channel(adp20086, ch)
		       : adp20086_disable_channel(adp20086, ch);
	case ADP20086_IIO_CURRENT_LIMIT:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		val *= MICROAMPER_PER_MILLIAMPER;

		if (val < ADP20086_ILIM_LSB_UA || val > ADP20086_MAX_ILIM_UA)
			return -EINVAL;

		/* ILIM is 0-indexed, 52ma @ code 0b0000 */
		i = no_os_clamp(( val / ADP20086_ILIM_LSB_UA - 1), 0,
				ADP20086_ILIM_832MA);

		return adp20086_set_ilim(adp20086, ch,
					 (enum adp20086_ilim_available)i);
	case ADP20086_IIO_IOPEN:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret || val < 0)
			return ret;

		return adp20086_set_iopen(adp20086, ch, (uint32_t)val * MICROAMPER_PER_MILLIAMPER);
	case ADP20086_IIO_LOAD_DETECT:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret || val < 0)
			return ret;

		return adp20086_set_ldet_threshold(adp20086, ch,
						   (uint32_t)val * MICROAMPER_PER_MILLIAMPER);
	case ADP20086_IIO_UVOUT:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret || val < 0)
			return ret;

		return adp20086_set_uvout(adp20086, ch, (uint32_t)val * MICROAMPER_PER_MILLIAMPER);
	case ADP20086_IIO_UVIN:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret || val < 0)
			return ret;

		return adp20086_set_uvin(adp20086, (uint32_t)val * MICROAMPER_PER_MILLIAMPER);
	case ADP20086_IIO_OVIN:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret || val < 0)
			return ret;

		return adp20086_set_ovin(adp20086, (uint32_t)val * MICROAMPER_PER_MILLIAMPER);
	case ADP20086_IIO_SOFT_START:
		for (i = 0; i < NO_OS_ARRAY_SIZE(adp20086_soft_start_avail); i++)
			if (!strcmp(buf, adp20086_soft_start_avail[i]))
				break;

		if (i == NO_OS_ARRAY_SIZE(adp20086_soft_start_avail))
			return -EINVAL;

		return adp20086_set_soft_start(adp20086, ch,
					       (enum adp20086_soft_start)i);
	case ADP20086_IIO_SOFT_SHUTDOWN:
		for (i = 0; i < NO_OS_ARRAY_SIZE(adp20086_soft_shutdown_avail); i++)
			if (!strcmp(buf, adp20086_soft_shutdown_avail[i]))
				break;

		if (i == NO_OS_ARRAY_SIZE(adp20086_soft_shutdown_avail))
			return -EINVAL;

		return adp20086_set_soft_shutdown(adp20086, ch,
						  (enum adp20086_soft_shutdown)i);
	case ADP20086_IIO_OPEN_PROT:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		return val ? adp20086_enable_open_protection(adp20086, ch)
		       : adp20086_disable_open_protection(adp20086, ch);
	case ADP20086_IIO_LOAD_DETECT_EN:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		return val ? adp20086_enable_load_detection(adp20086, ch)
		       : adp20086_disable_load_detection(adp20086, ch);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read the "0 1" enumeration shared by the boolean attributes.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written.
 */
static int adp20086_iio_read_bool_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	return sprintf(buf, "%s ", adp20086_bool_avail);
}

/**
 * @brief Read the soft-start ramp enumeration.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written.
 */
static int adp20086_iio_read_soft_start_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp20086_soft_start_avail); i++)
		length += sprintf(buf + length, "%s ", adp20086_soft_start_avail[i]);

	return length;
}

/**
 * @brief Read the soft-shutdown ramp enumeration.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written.
 */
static int adp20086_iio_read_soft_shutdown_available(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp20086_soft_shutdown_avail); i++)
		length += sprintf(buf + length, "%s ",
				  adp20086_soft_shutdown_avail[i]);

	return length;
}

/**
 * @brief Read a device-global attribute.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written on success, negative error code otherwise.
 */
static int adp20086_iio_read_global(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	uint8_t reg, byte;
	int32_t val;
	bool flag;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	switch (priv) {
	case ADP20086_IIO_GPIO_EN:
		ret = adp20086_get_gpio_en(adp20086, &flag);
		if (ret)
			return ret;

		val = flag;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_INTB:
		ret = adp20086_get_intb(adp20086, &flag);
		if (ret)
			return ret;

		val = flag;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_PECE:
		ret = adp20086_get_pece(adp20086, &flag);
		if (ret)
			return ret;

		val = flag;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_PART_ID:
		ret = adp20086_get_part_id(adp20086, &byte);
		if (ret)
			return ret;

		val = byte;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_REVISION:
		ret = adp20086_get_revision(adp20086, &byte);
		if (ret)
			return ret;

		val = byte;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ADP20086_IIO_INT_MASK:
		reg = ADP20086_REG_MASK;
		break;
	case ADP20086_IIO_INT_MASK2:
		reg = ADP20086_REG_MASK2;
		break;
	case ADP20086_IIO_INT_MASK3:
		reg = ADP20086_REG_MASK3;
		break;
	default:
		return -EINVAL;
	}

	ret = adp20086_read(adp20086, reg, &byte);
	if (ret)
		return ret;

	val = byte;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write a device-global attribute.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer holding the requested value.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 0 on success, negative error code otherwise.
 */
static int adp20086_iio_write_global(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	uint8_t reg;
	int32_t val;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (val < 0)
		return -EINVAL;

	switch (priv) {
	case ADP20086_IIO_GPIO_EN:
		return adp20086_set_gpio_en(adp20086, !!val);
	case ADP20086_IIO_INT_MASK:
		reg = ADP20086_REG_MASK;
		break;
	case ADP20086_IIO_INT_MASK2:
		reg = ADP20086_REG_MASK2;
		break;
	case ADP20086_IIO_INT_MASK3:
		reg = ADP20086_REG_MASK3;
		break;
	default:
		return -EINVAL;
	}

	return adp20086_write(adp20086, reg, (uint8_t)val);
}

/**
 * @brief Read a decoded, human-readable status string (debug attribute).
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written on success, negative error code otherwise.
 */
static int adp20086_iio_read_status(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	struct adp20086_channel_status cs;
	struct adp20086_device_status ds;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	ret = adp20086_get_device_status(adp20086, &ds);
	if (ret)
		return ret;

	ret = adp20086_get_channel_status(adp20086, &cs);
	if (ret)
		return ret;

	return sprintf(buf,
		       "input: OVIN=%d UVIN=%d OVDD=%d UVDD=%d TLIM=%d ACC=%d\n"
		       "integrity: BIST=%d PARERR=%d ECC_COR=%d ECC_DET=%d FCAL=%d\n"
		       "per-ch (bit0=ch1): OC=0x%x OV=0x%x UV=0x%x TSD=0x%x "
		       "OPEN=0x%x LDET=0x%x\n",
		       ds.ovin, ds.uvin, ds.ovdd, ds.uvdd, ds.tlim_serv, ds.acc,
		       ds.bist, ds.parerr, ds.ecc_cor, ds.ecc_det, ds.adc_fcal,
		       cs.overcurrent, cs.overvoltage, cs.undervoltage,
		       cs.thermal_shutdown, cs.open_load, cs.load_detected);
}

/**
 * @brief Read the latched built-in self-test result (debug attribute).
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written on success, negative error code otherwise.
 */
static int adp20086_iio_read_bist(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	struct adp20086_device_status ds;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	ret = adp20086_get_device_status(adp20086, &ds);
	if (ret)
		return ret;

	val = ds.bist;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Register read callback for the IIO direct_reg_access debug interface.
 * @param dev     - The iio device structure.
 * @param reg     - Register address to read.
 * @param readval - Pointer to store the read value.
 * @return 0 on success, negative error code otherwise.
 */
static int adp20086_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	uint8_t val;
	int ret;

	if (!iio_adp20086 || reg > ADP20086_REG_LOAD_INT_EN|| !readval)
		return -EINVAL;

	ret = adp20086_read(iio_adp20086->adp20086_dev, (uint8_t)reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

/**
 * @brief Register write callback for the IIO direct_reg_access debug interface.
 * @param dev      - The iio device structure.
 * @param reg      - Register address to write.
 * @param writeval - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
static int adp20086_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;

	if (!iio_adp20086 || reg > ADP20086_REG_LOAD_INT_EN || writeval > 0xFF)
		return -EINVAL;

	return adp20086_write(iio_adp20086->adp20086_dev, (uint8_t)reg,
			      (uint8_t)writeval);
}

/**
 * @brief Initialize the ADP20086 IIO descriptor.
 * @param iio_desc   - The IIO device descriptor to allocate.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 on success, negative error code otherwise.
 */
int adp20086_iio_init(struct adp20086_iio_desc **iio_desc,
		      struct adp20086_iio_desc_init_param *init_param)
{
	struct adp20086_iio_desc *descriptor;
	uint32_t i, n = 0;
	int ret;

	if (!iio_desc || !init_param || !init_param->adp20086_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = adp20086_init(&descriptor->adp20086_dev,
			    init_param->adp20086_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev_inst = adp20086_iio_dev;

	/*
	 * Build the global attribute list for this instance: the EN and INTB
	 * attributes only make sense when the corresponding pin is actually
	 * wired, and the two conditions are independent.
	 */
	for (i = 0; i < NO_OS_ARRAY_SIZE(adp20086_global_attrs); i++) {
		switch (adp20086_global_attrs[i].requires) {
		case ADP20086_ATTR_NEEDS_EN:
			if (!descriptor->adp20086_dev->en_gpio_desc)
				continue;

			break;
		case ADP20086_ATTR_NEEDS_INTB:
			if (!descriptor->adp20086_dev->intb_gpio_desc)
				continue;

			break;
		default:
			break;
		}

		descriptor->global_attrs[n++] = adp20086_global_attrs[i].attr;
	}

	descriptor->global_attrs[n] = (struct iio_attribute)END_ATTRIBUTES_ARRAY;
	descriptor->iio_dev_inst.attributes = descriptor->global_attrs;

	descriptor->iio_dev = &descriptor->iio_dev_inst;

	*iio_desc = descriptor;

	return 0;

free_desc:
	adp20086_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by adp20086_iio_init().
 *
 * The channel and attribute tables are statically allocated and must not be
 * freed here (unlike the adp5055 IIO driver, which frees a static array).
 * @param iio_desc - The IIO device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int adp20086_iio_remove(struct adp20086_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	if (iio_desc->adp20086_dev)
		adp20086_remove(iio_desc->adp20086_dev);

	no_os_free(iio_desc);

	return 0;
}

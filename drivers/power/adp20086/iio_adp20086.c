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
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "adp20086.h"
#include "iio_adp20086.h"

/** Telemetry source behind a channel, held in struct iio_channel.address. */
enum adp20086_iio_telemetry {
	ADP20086_IIO_VOUT1,
	ADP20086_IIO_VOUT2,
	ADP20086_IIO_VOUT3,
	ADP20086_IIO_VOUT4,
	ADP20086_IIO_IOUT1,
	ADP20086_IIO_IOUT2,
	ADP20086_IIO_IOUT3,
	ADP20086_IIO_IOUT4,
	ADP20086_IIO_VIN,
	ADP20086_IIO_VDD,
};

/** Device-level attribute, held in struct iio_attribute.priv. */
enum adp20086_iio_device_attr {
	ADP20086_IIO_DEVICE_ID,
	ADP20086_IIO_REVISION,
	ADP20086_IIO_PECE,
	ADP20086_IIO_GPIO_EN,
	ADP20086_IIO_INTERRUPT_ARM,
	ADP20086_IIO_CLEAR_STATUS,
};

static int adp20086_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int adp20086_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);
static int adp20086_iio_read_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int adp20086_iio_write_enable(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);
static int adp20086_iio_read_bool_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_channel_status(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_status(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int adp20086_iio_read_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_write_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval);
static int adp20086_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval);

/*
 * Status attributes carry an enum adp20086_status_field in .priv and read the
 * one register that holds the bit. The per-channel form takes the field of
 * channel 1; the handler adds the channel index, which works because the
 * enum groups all four channels of a fault consecutively.
 *
 * Both forms read the device live. That is only safe because the IIO layer
 * clears CONFIG.CLR at init, making the STAT registers non-destructive to
 * read; see adp20086_iio_init().
 */
#define ADP20086_CHANNEL_STATUS_ATTR(_name, _field) {	\
	.name = (_name),				\
	.show = adp20086_iio_read_channel_status,	\
	.priv = (_field),				\
}

#define ADP20086_DEVICE_STATUS_ATTR(_name, _field) {	\
	.name = (_name),				\
	.show = adp20086_iio_read_status,		\
	.priv = (_field),				\
}

/* Every telemetry channel exposes the same pair of read-only attributes. */
static struct iio_attribute adp20086_telemetry_attrs[] = {
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

/*
 * The four IOUT channels map 1:1 to the device outputs, so they also carry the
 * output switch enable. The matching VOUT channels are left read-only: both
 * views refer to the same switch, and exposing "enable" twice would only invite
 * writing to one and reading back the other.
 */
static struct iio_attribute adp20086_iout_attrs[] = {
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
		.show = adp20086_iio_read_enable,
		.store = adp20086_iio_write_enable,
	},
	{
		.name = "enable_available",
		.show = adp20086_iio_read_bool_available,
		.shared = IIO_SHARED_BY_TYPE
	},
	ADP20086_CHANNEL_STATUS_ATTR("status_thermal_shutdown",
				     ADP20086_STATUS_TSOUT1),
	ADP20086_CHANNEL_STATUS_ATTR("status_overcurrent",
				     ADP20086_STATUS_OCOUT1),
	ADP20086_CHANNEL_STATUS_ATTR("status_ovout", ADP20086_STATUS_OVOUT1),
	ADP20086_CHANNEL_STATUS_ATTR("status_uvout", ADP20086_STATUS_UVOUT1),
	ADP20086_CHANNEL_STATUS_ATTR("status_load_detected",
				     ADP20086_STATUS_LDET1),
	ADP20086_CHANNEL_STATUS_ATTR("status_open_load",
				     ADP20086_STATUS_OPEN1),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adp20086_device_attrs[] = {
	{
		.name = "device_id",
		.show = adp20086_iio_read_device_attr,
		.priv = ADP20086_IIO_DEVICE_ID,
	},
	{
		.name = "revision",
		.show = adp20086_iio_read_device_attr,
		.priv = ADP20086_IIO_REVISION,
	},
	{
		.name = "pece",
		.show = adp20086_iio_read_device_attr,
		.priv = ADP20086_IIO_PECE,
	},
	{
		.name = "gpio_en",
		.show = adp20086_iio_read_device_attr,
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_GPIO_EN,
	},
	{
		.name = "gpio_en_available",
		.show = adp20086_iio_read_bool_available,
	},
	{
		.name = "interrupt_arm",
		.show = adp20086_iio_read_device_attr,
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_INTERRUPT_ARM,
	},
	{
		.name = "interrupt_arm_available",
		.show = adp20086_iio_read_bool_available,
	},
	{
		.name = "clear_status",
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_CLEAR_STATUS,
	},
	ADP20086_DEVICE_STATUS_ATTR("status_tlim_serv",
				    ADP20086_STATUS_TLIM_SERV),
	ADP20086_DEVICE_STATUS_ATTR("status_acc", ADP20086_STATUS_ACC),
	ADP20086_DEVICE_STATUS_ATTR("status_ovin", ADP20086_STATUS_OVIN),
	ADP20086_DEVICE_STATUS_ATTR("status_uvin", ADP20086_STATUS_UVIN),
	ADP20086_DEVICE_STATUS_ATTR("status_ovdd", ADP20086_STATUS_OVDD),
	ADP20086_DEVICE_STATUS_ATTR("status_uvdd", ADP20086_STATUS_UVDD),
	ADP20086_DEVICE_STATUS_ATTR("status_adc_fcal", ADP20086_STATUS_ADCFCAL),
	ADP20086_DEVICE_STATUS_ATTR("status_intbsts", ADP20086_STATUS_INTBSTS),
	ADP20086_DEVICE_STATUS_ATTR("status_bist", ADP20086_STATUS_BIST),
	ADP20086_DEVICE_STATUS_ATTR("status_parerr", ADP20086_STATUS_PARERR),
	ADP20086_DEVICE_STATUS_ATTR("status_ecc_cor", ADP20086_STATUS_ECC_COR),
	ADP20086_DEVICE_STATUS_ATTR("status_ecc_det", ADP20086_STATUS_ECC_DET),
	END_ATTRIBUTES_ARRAY
};

#define ADP20086_CHANNEL(_name, _type, _index, _address, _attrs) {	\
	.name = (_name),						\
	.ch_type = (_type),						\
	.channel = (_index),						\
	.address = (_address),						\
	.attributes = (_attrs),						\
	.indexed = true,						\
}

#define ADP20086_TELEMETRY_CHANNEL(_name, _type, _index, _address)	\
	ADP20086_CHANNEL(_name, _type, _index, _address,		\
			 adp20086_telemetry_attrs)

#define ADP20086_IOUT_CHANNEL(_name, _index, _address)			\
	ADP20086_CHANNEL(_name, IIO_CURRENT, _index, _address,		\
			 adp20086_iout_attrs)

static struct iio_channel adp20086_channels[] = {
	ADP20086_TELEMETRY_CHANNEL("vout1", IIO_VOLTAGE, 0, ADP20086_IIO_VOUT1),
	ADP20086_TELEMETRY_CHANNEL("vout2", IIO_VOLTAGE, 1, ADP20086_IIO_VOUT2),
	ADP20086_TELEMETRY_CHANNEL("vout3", IIO_VOLTAGE, 2, ADP20086_IIO_VOUT3),
	ADP20086_TELEMETRY_CHANNEL("vout4", IIO_VOLTAGE, 3, ADP20086_IIO_VOUT4),
	ADP20086_TELEMETRY_CHANNEL("vin", IIO_VOLTAGE, 4, ADP20086_IIO_VIN),
	ADP20086_TELEMETRY_CHANNEL("vdd", IIO_VOLTAGE, 5, ADP20086_IIO_VDD),
	ADP20086_IOUT_CHANNEL("iout1", 0, ADP20086_IIO_IOUT1),
	ADP20086_IOUT_CHANNEL("iout2", 1, ADP20086_IIO_IOUT2),
	ADP20086_IOUT_CHANNEL("iout3", 2, ADP20086_IIO_IOUT3),
	ADP20086_IOUT_CHANNEL("iout4", 3, ADP20086_IIO_IOUT4),
};

/* Setting both reg hooks makes iio emit the direct_reg_access debug attribute. */
static struct iio_device adp20086_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adp20086_channels),
	.channels = adp20086_channels,
	.attributes = adp20086_device_attrs,
	.debug_reg_read = adp20086_iio_reg_read,
	.debug_reg_write = adp20086_iio_reg_write,
};

/**
 * @brief Read the raw ADC code of a telemetry channel.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - Unused.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	uint8_t code;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	switch (channel->address) {
	case ADP20086_IIO_VOUT1:
	case ADP20086_IIO_VOUT2:
	case ADP20086_IIO_VOUT3:
	case ADP20086_IIO_VOUT4:
		ret = adp20086_read_vout_code(adp20086,
					      channel->address - ADP20086_IIO_VOUT1,
					      &code);
		break;
	case ADP20086_IIO_IOUT1:
	case ADP20086_IIO_IOUT2:
	case ADP20086_IIO_IOUT3:
	case ADP20086_IIO_IOUT4:
		ret = adp20086_read_iout_code(adp20086,
					      channel->address - ADP20086_IIO_IOUT1,
					      &code);
		break;
	case ADP20086_IIO_VIN:
		ret = adp20086_read_vin_code(adp20086, &code);
		break;
	case ADP20086_IIO_VDD:
		ret = adp20086_read_vdd_code(adp20086, &code);
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
 * @brief Read the ADC LSB weight of a telemetry channel, in mV or mA.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - Unused.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t vals[2];

	switch (channel->type) {
	case IIO_VOLTAGE:
		/* VIN and VOUT share the same LSB; only VDD is finer. */
		vals[0] = (channel->address == ADP20086_IIO_VDD)
			  ? ADP20086_VDD_LSB_UV / MILLI
			  : ADP20086_VOUT_LSB_UV / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
	case IIO_CURRENT:
		/* 3.5 mA does not divide evenly, so report mA plus micro-mA. */
		vals[0] = ADP20086_IOUT_LSB_UA / MICROAMPER_PER_MILLIAMPER;
		vals[1] = (ADP20086_IOUT_LSB_UA % MICROAMPER_PER_MILLIAMPER) * MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Parse a boolean attribute value written by the host.
 * @param buf - Input buffer holding the written value.
 * @param val - Parsed value.
 * @return 0 in case of success, error code otherwise.
 */
static int adp20086_iio_parse_bool(char *buf, bool *val)
{
	int32_t parsed;
	int ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &parsed, NULL);
	if (ret)
		return ret;

	if (parsed >> 1)
		return -EINVAL;

	*val = parsed;

	return 0;
}

/**
 * @brief Read whether the output switch of an IOUT channel is enabled.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - Unused.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_enabled_channels enabled;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_get_enabled_channels(iio_adp20086->adp20086_dev, &enabled);
	if (ret)
		return ret;

	switch (channel->address) {
	case ADP20086_IIO_IOUT1:
		val = enabled.out1;
		break;
	case ADP20086_IIO_IOUT2:
		val = enabled.out2;
		break;
	case ADP20086_IIO_IOUT3:
		val = enabled.out3;
		break;
	case ADP20086_IIO_IOUT4:
		val = enabled.out4;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Enable or disable the output switch of an IOUT channel.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - Unused.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_enable(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	bool enable;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_iio_parse_bool(buf, &enable);
	if (ret)
		return ret;

	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	ret = adp20086_set_channel(iio_adp20086->adp20086_dev,
				   channel->address - ADP20086_IIO_IOUT1,
				   enable);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief List the accepted values of a boolean attribute.
 * @param dev - Unused.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - Unused.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_bool_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	return snprintf(buf, len, "0 1");
}

/**
 * @brief Read a latched fault bit of the device.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - The enum adp20086_status_field to report.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_status(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	bool asserted;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_get_status_field(iio_adp20086->adp20086_dev, priv,
					&asserted);
	if (ret)
		return ret;

	val = asserted;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read a latched fault bit of an output channel.
 *
 * priv names the field of channel 1; the channel index is added to it, which
 * relies on enum adp20086_status_field grouping all four channels of a fault
 * consecutively.
 *
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_status_field of channel 1.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_channel_status(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	return adp20086_iio_read_status(dev, buf, len, channel,
					priv + channel->address -
					ADP20086_IIO_IOUT1);
}

/**
 * @brief Read a device identification attribute cached at init time.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - Device attribute selector.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	uint8_t byte = 0;
	bool flag = false;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	switch (priv) {
	case ADP20086_IIO_DEVICE_ID:
		ret = adp20086_get_part_id(adp20086, &byte);
		val = byte;
		break;
	case ADP20086_IIO_REVISION:
		ret = adp20086_get_revision(adp20086, &byte);
		val = byte;
		break;
	case ADP20086_IIO_PECE:
		ret = adp20086_get_pece(adp20086, &flag);
		val = flag;
		break;
	case ADP20086_IIO_GPIO_EN:
		ret = adp20086_get_gpio_en(adp20086, &flag);
		val = flag;
		break;
	case ADP20086_IIO_INTERRUPT_ARM:
		ret = adp20086_get_interrupt_armed(adp20086, &flag);
		val = flag;
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write a device-level attribute.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Unused.
 * @param priv - Device attribute selector.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	bool enable;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	ret = adp20086_iio_parse_bool(buf, &enable);
	if (ret)
		return ret;

	switch (priv) {
	case ADP20086_IIO_GPIO_EN:
		ret = adp20086_set_gpio_en(adp20086, enable);
		break;
	case ADP20086_IIO_INTERRUPT_ARM:
		ret = enable ? adp20086_arm_interrupt(adp20086)
		      : adp20086_disarm_interrupt(adp20086);
		break;
	case ADP20086_IIO_CLEAR_STATUS:
		/* Triggers; writing 0 is a no-op rather than an error. */
		if (!enable)
			return len;

		ret = adp20086_clear_status(adp20086);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Read a device register, backing the direct_reg_access debug attribute.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param reg - Register address.
 * @param readval - Register value.
 * @return 0 in case of success, error code otherwise.
 */
static int adp20086_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	uint8_t val;
	int ret;

	if (!iio_adp20086 || !readval || reg > ADP20086_REG_LOAD_INT_EN)
		return -EINVAL;

	ret = adp20086_read(iio_adp20086->adp20086_dev, (uint8_t)reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

/**
 * @brief Write a device register, backing the direct_reg_access debug attribute.
 *
 * Refuses to set CONFIG.CLR. Every status attribute reads the STAT registers
 * live and one at a time, which is only non-destructive while CLR is clear, so
 * setting it here would silently turn each status read into a clear of the
 * faults packed into the same byte. Writes to CONFIG that leave CLR clear are
 * allowed; use the clear_status attribute to flush the latches.
 *
 * @param dev - The ADP20086 IIO device descriptor.
 * @param reg - Register address.
 * @param writeval - Register value.
 * @return 0 in case of success, error code otherwise.
 */
static int adp20086_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;

	if (!iio_adp20086 || reg > ADP20086_REG_LOAD_INT_EN || writeval > 0xFF)
		return -EINVAL;

	if (reg == ADP20086_REG_CONFIG && (writeval & ADP20086_CONFIG_CLR))
		return -EPERM;

	return adp20086_write(iio_adp20086->adp20086_dev, (uint8_t)reg,
			      (uint8_t)writeval);
}

/**
 * @brief Initialize the ADP20086 IIO descriptor.
 * @param iio_desc - The ADP20086 IIO device descriptor.
 * @param init_param - The ADP20086 IIO initialization parameter.
 * @return 0 in case of success, error code otherwise.
 */
int adp20086_iio_init(struct adp20086_iio_desc **iio_desc,
		      struct adp20086_iio_desc_init_param *init_param)
{
	struct adp20086_iio_desc *descriptor;
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

	ret = adp20086_set_gpio_en(descriptor->adp20086_dev,
				   init_param->gpio_en_initial_state);
	if (ret)
		goto free_desc;

	ret = adp20086_set_all_channels(descriptor->adp20086_dev,
					init_param->channels_en_initial_state);
	if (ret)
		goto free_desc;

	ret = adp20086_set_gpio_en(descriptor->adp20086_dev,
				   init_param->gpio_en_initial_state);
	if (ret)
		goto free_desc;

	/*
	 * Every status attribute reads the STAT registers live and one at a
	 * time, so they must not be clear-on-read: a read of one channel's
	 * fault would otherwise clear the faults packed into the same byte for
	 * its neighbours. Clearing CLR also makes the faults latch until an
	 * explicit clear_status, which is what lets a transient fault still be
	 * seen after INTB has released. Nothing else in this driver sets CLR,
	 * and no attribute exposes it.
	 */
	ret = adp20086_set_clr_config(descriptor->adp20086_dev, false);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &adp20086_iio_dev;

	*iio_desc = descriptor;

	return 0;

free_desc:
	adp20086_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by adp20086_iio_init().
 * @param iio_desc - The ADP20086 IIO device descriptor.
 * @return 0 in case of success, error code otherwise.
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

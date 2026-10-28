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

/**
 * Per-output enable exposed by a channel, held in struct iio_attribute.priv.
 */
enum adp20086_iio_channel_enable {
	ADP20086_IIO_EN,
	ADP20086_IIO_LOAD_DETECT_EN,
	ADP20086_IIO_OPEN_PROT_EN,
};

/**
 * Per-output threshold exposed by a channel, held in struct iio_attribute.priv.
 */
enum adp20086_iio_channel_threshold {
	ADP20086_IIO_ILIM,
	ADP20086_IIO_LDET_THRESHOLD,
	ADP20086_IIO_IOPEN,
	ADP20086_IIO_UVOUT,
};

/** Per-output ramp exposed by a channel, held in struct iio_attribute.priv. */
enum adp20086_iio_channel_ramp {
	ADP20086_IIO_SOFT_STARTUP,
	ADP20086_IIO_SOFT_SHUTDOWN,
};

/** Device-level threshold, held in struct iio_attribute.priv. */
enum adp20086_iio_device_threshold {
	ADP20086_IIO_OVIN,
	ADP20086_IIO_UVIN,
};

/** Device-level attribute, held in struct iio_attribute.priv. */
enum adp20086_iio_device_attr {
	ADP20086_IIO_DEVICE_ID,
	ADP20086_IIO_REVISION,
	ADP20086_IIO_PECE,
	ADP20086_IIO_GPIO_EN,
	ADP20086_IIO_INTERRUPT_ARM,
	ADP20086_IIO_ENC,
	ADP20086_IIO_OVTST_EN,
	ADP20086_IIO_IIR_FILTER,
	ADP20086_IIO_DISCHARGE_RESISTORS,
	ADP20086_IIO_CLEAR_STATUS,
};

static const uint32_t adp20086_soft_startup_us[] = { 500, 1000, 2000, 4000 };
static const uint32_t adp20086_soft_shutdown_us[] = { 250, 500, 1000, 2000 };

static int adp20086_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int adp20086_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);
static int adp20086_iio_read_threshold(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);
static int adp20086_iio_write_threshold(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);
static int adp20086_iio_read_ilim_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_ramp(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int adp20086_iio_write_ramp(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);
static int adp20086_iio_read_ramp_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_mux(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int adp20086_iio_write_mux(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int adp20086_iio_read_mux_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_device_threshold(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_write_device_threshold(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_en(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);
static int adp20086_iio_write_en(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int adp20086_iio_read_bool_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_status(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int adp20086_iio_read_channel_status(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_mask(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int adp20086_iio_write_mask(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);
static int adp20086_iio_read_channel_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_write_channel_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_write_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adp20086_iio_read_legacy_adc(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);
static int adp20086_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval);
static int adp20086_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval);

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

#define ADP20086_CHANNEL_MASK_ATTR(_name, _ch1_src) {	\
	.name = (_name),				\
	.show = adp20086_iio_read_channel_mask,		\
	.store = adp20086_iio_write_channel_mask,	\
	.priv = (_ch1_src),				\
}

#define ADP20086_DEVICE_MASK_ATTR(_name, _src) {	\
	.name = (_name),				\
	.show = adp20086_iio_read_mask,			\
	.store = adp20086_iio_write_mask,		\
	.priv = (_src),					\
}

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
		.name = "en",
		.show = adp20086_iio_read_en,
		.store = adp20086_iio_write_en,
		.priv = ADP20086_IIO_EN,
	},
	{
		.name = "load_detect_en",
		.show = adp20086_iio_read_en,
		.store = adp20086_iio_write_en,
		.priv = ADP20086_IIO_LOAD_DETECT_EN,
	},
	{
		.name = "open_prot_en",
		.show = adp20086_iio_read_en,
		.store = adp20086_iio_write_en,
		.priv = ADP20086_IIO_OPEN_PROT_EN,
	},
	{
		.name = "en_available",
		.show = adp20086_iio_read_bool_available,
		.shared = IIO_SHARED_BY_ALL
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
	ADP20086_CHANNEL_MASK_ATTR("mask_uvout", ADP20086_INT_UV1M),
	ADP20086_CHANNEL_MASK_ATTR("mask_open_load", ADP20086_INT_OPNM1),
	{
		.name = "ilim",
		.show = adp20086_iio_read_threshold,
		.store = adp20086_iio_write_threshold,
		.priv = ADP20086_IIO_ILIM,
	},
	{
		.name = "ilim_available",
		.show = adp20086_iio_read_ilim_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "ldet_threshold",
		.show = adp20086_iio_read_threshold,
		.store = adp20086_iio_write_threshold,
		.priv = ADP20086_IIO_LDET_THRESHOLD,
	},
	{
		.name = "iopen",
		.show = adp20086_iio_read_threshold,
		.store = adp20086_iio_write_threshold,
		.priv = ADP20086_IIO_IOPEN,
	},
	{
		.name = "uvout",
		.show = adp20086_iio_read_threshold,
		.store = adp20086_iio_write_threshold,
		.priv = ADP20086_IIO_UVOUT,
	},
	{
		.name = "soft_startup",
		.show = adp20086_iio_read_ramp,
		.store = adp20086_iio_write_ramp,
		.priv = ADP20086_IIO_SOFT_STARTUP,
	},
	{
		.name = "soft_startup_available",
		.show = adp20086_iio_read_ramp_available,
		.priv = ADP20086_IIO_SOFT_STARTUP,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "soft_shutdown",
		.show = adp20086_iio_read_ramp,
		.store = adp20086_iio_write_ramp,
		.priv = ADP20086_IIO_SOFT_SHUTDOWN,
	},
	{
		.name = "soft_shutdown_available",
		.show = adp20086_iio_read_ramp_available,
		.priv = ADP20086_IIO_SOFT_SHUTDOWN,
		.shared = IIO_SHARED_BY_ALL
	},
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
		.name = "interrupt_arm",
		.show = adp20086_iio_read_device_attr,
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_INTERRUPT_ARM,
	},
	{
		.name = "mux",
		.show = adp20086_iio_read_mux,
		.store = adp20086_iio_write_mux,
	},
	{
		.name = "mux_available",
		.show = adp20086_iio_read_mux_available,
	},
	{
		.name = "enc",
		.show = adp20086_iio_read_device_attr,
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_ENC,
	},
	{
		.name = "ovtst_en",
		.show = adp20086_iio_read_device_attr,
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_OVTST_EN,
	},
	{
		.name = "iir_filter",
		.show = adp20086_iio_read_device_attr,
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_IIR_FILTER,
	},
	{
		.name = "discharge_resistors",
		.show = adp20086_iio_read_device_attr,
		.store = adp20086_iio_write_device_attr,
		.priv = ADP20086_IIO_DISCHARGE_RESISTORS,
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
	ADP20086_DEVICE_MASK_ATTR("mask_acc", ADP20086_INT_ACCM),
	ADP20086_DEVICE_MASK_ATTR("mask_ts", ADP20086_INT_TSM),
	ADP20086_DEVICE_MASK_ATTR("mask_vdd", ADP20086_INT_VDDM),
	ADP20086_DEVICE_MASK_ATTR("mask_vin", ADP20086_INT_VINM),
	ADP20086_DEVICE_MASK_ATTR("mask_oc", ADP20086_INT_OCM),
	ADP20086_DEVICE_MASK_ATTR("mask_ov", ADP20086_INT_OVM),
	ADP20086_DEVICE_MASK_ATTR("mask_uv", ADP20086_INT_UVM),
	ADP20086_DEVICE_MASK_ATTR("mask_fcal", ADP20086_INT_FCALM),
	ADP20086_DEVICE_MASK_ATTR("mask_bist", ADP20086_INT_BISTM),
	ADP20086_DEVICE_MASK_ATTR("mask_parerr", ADP20086_INT_PARERRM),
	ADP20086_DEVICE_MASK_ATTR("mask_ecc_cor", ADP20086_INT_ECC_CORM),
	ADP20086_DEVICE_MASK_ATTR("mask_ecc_det", ADP20086_INT_ECC_DETM),
	{
		.name = "ovin",
		.show = adp20086_iio_read_device_threshold,
		.store = adp20086_iio_write_device_threshold,
		.priv = ADP20086_IIO_OVIN,
	},
	{
		.name = "uvin",
		.show = adp20086_iio_read_device_threshold,
		.store = adp20086_iio_write_device_threshold,
		.priv = ADP20086_IIO_UVIN,
	},
	END_ATTRIBUTES_ARRAY
};

#define ADP20086_LEGACY_ADC_ATTR(_name, _adc) {		\
	.name = (_name),				\
	.show = adp20086_iio_read_legacy_adc,		\
	.priv = (_adc),					\
}

static struct iio_attribute adp20086_debug_attrs[] = {
	ADP20086_LEGACY_ADC_ATTR("legacy_adc1_raw", ADP20086_ADC1_LEGACY),
	ADP20086_LEGACY_ADC_ATTR("legacy_adc2_raw", ADP20086_ADC2_LEGACY),
	ADP20086_LEGACY_ADC_ATTR("legacy_adc3_raw", ADP20086_ADC3_LEGACY),
	ADP20086_LEGACY_ADC_ATTR("legacy_adc4_raw", ADP20086_ADC4_LEGACY),
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

static struct iio_device adp20086_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adp20086_channels),
	.channels = adp20086_channels,
	.attributes = adp20086_device_attrs,
	.debug_attributes = adp20086_debug_attrs,
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
		/* VIN and VOUT share the same LSB. */
		vals[0] = (channel->address == ADP20086_IIO_VDD)
			  ? ADP20086_VDD_LSB_UV / MILLI
			  : ADP20086_VOUT_LSB_UV / MILLI;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
	case IIO_CURRENT:
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
 * @brief Report a value held in micro-units as milli-units.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param micro - The value, in micro-units.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_format_milli(char *buf, uint32_t len, uint32_t micro)
{
	int32_t vals[2];

	vals[0] = micro / MILLI;
	vals[1] = (micro % MILLI) * MILLI;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
}

/**
 * @brief Parse a value written in milli-units into micro-units.
 * @param buf - Input buffer holding the written value.
 * @param micro - The parsed value, in micro-units.
 * @return 0 in case of success, error code otherwise.
 */
static int adp20086_iio_parse_milli(char *buf, uint32_t *micro)
{
	int32_t val1, val2;
	int ret;

	ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);
	if (ret)
		return ret;

	if (val1 < 0 || val2 < 0)
		return -EINVAL;

	*micro = (uint32_t)val1 * MILLI + (uint32_t)val2 / MILLI;

	return 0;
}

/**
 * @brief Read one of the per-output thresholds of an IOUT channel.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_iio_channel_threshold to report.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_threshold(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	enum adp20086_channel ch;
	uint32_t micro;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;
	ch = channel->address - ADP20086_IIO_IOUT1;

	switch (priv) {
	case ADP20086_IIO_ILIM:
		ret = adp20086_get_ilim(adp20086, ch, &micro);
		break;
	case ADP20086_IIO_LDET_THRESHOLD:
		ret = adp20086_get_ldet_threshold(adp20086, ch, &micro);
		break;
	case ADP20086_IIO_IOPEN:
		ret = adp20086_get_iopen(adp20086, ch, &micro);
		break;
	case ADP20086_IIO_UVOUT:
		ret = adp20086_get_uvout(adp20086, ch, &micro);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return adp20086_iio_format_milli(buf, len, micro);
}

/**
 * @brief Set one of the per-output thresholds of an IOUT channel.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_iio_channel_threshold to update.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_threshold(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	enum adp20086_channel ch;
	uint32_t micro;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	ret = adp20086_iio_parse_milli(buf, &micro);
	if (ret)
		return ret;

	adp20086 = iio_adp20086->adp20086_dev;
	ch = channel->address - ADP20086_IIO_IOUT1;

	switch (priv) {
	case ADP20086_IIO_ILIM:
		if (micro < ADP20086_ILIM_LSB_UA ||
		    micro > ADP20086_MAX_ILIM_UA ||
		    micro % ADP20086_ILIM_LSB_UA)
			return -EINVAL;

		ret = adp20086_set_ilim(adp20086, ch,
					micro / ADP20086_ILIM_LSB_UA - 1);
		break;
	case ADP20086_IIO_LDET_THRESHOLD:
		ret = adp20086_set_ldet_threshold(adp20086, ch, micro);
		break;
	case ADP20086_IIO_IOPEN:
		ret = adp20086_set_iopen(adp20086, ch, micro);
		break;
	case ADP20086_IIO_UVOUT:
		ret = adp20086_set_uvout(adp20086, ch, micro);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return len;
}

/**
 * @brief List the current limit steps the device offers, in mA.
 * @param dev - Unused.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - Unused.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_ilim_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t i, micro;
	int length = 0;
	int ret;

	for (i = ADP20086_ILIM_52MA; i <= ADP20086_ILIM_832MA; i++) {
		micro = (i + 1) * ADP20086_ILIM_LSB_UA;

		ret = snprintf(buf + length, len - length, "%s%lu.%06lu",
			       i ? " " : "",
			       (unsigned long)(micro / MILLI),
			       (unsigned long)((micro % MILLI) * MILLI));
		if (ret < 0)
			return ret;
		if ((uint32_t)ret >= len - length)
			return -ENOMEM;

		length += ret;
	}

	return length;
}

/**
 * @brief Pick the ramp ladder a per-channel ramp attribute refers to.
 * @param priv - The enum adp20086_iio_channel_ramp to look up.
 * @param table - The ramp durations, in microseconds, indexed by code.
 * @return Number of entries in table, error code otherwise.
 */
static int adp20086_iio_ramp_table(intptr_t priv, const uint32_t **table)
{
	switch (priv) {
	case ADP20086_IIO_SOFT_STARTUP:
		*table = adp20086_soft_startup_us;

		return NO_OS_ARRAY_SIZE(adp20086_soft_startup_us);
	case ADP20086_IIO_SOFT_SHUTDOWN:
		*table = adp20086_soft_shutdown_us;

		return NO_OS_ARRAY_SIZE(adp20086_soft_shutdown_us);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read one of the per-output ramps of an IOUT channel, in ms.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_iio_channel_ramp to report.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_ramp(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	enum adp20086_soft_shutdown ssd;
	enum adp20086_soft_startup ssu;
	struct adp20086_dev *adp20086;
	const uint32_t *table;
	enum adp20086_channel ch;
	int entries;
	uint8_t code;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	entries = adp20086_iio_ramp_table(priv, &table);
	if (entries < 0)
		return entries;

	adp20086 = iio_adp20086->adp20086_dev;
	ch = channel->address - ADP20086_IIO_IOUT1;

	if (priv == ADP20086_IIO_SOFT_STARTUP) {
		ret = adp20086_get_soft_startup(adp20086, ch, &ssu);
		code = ssu;
	} else {
		ret = adp20086_get_soft_shutdown(adp20086, ch, &ssd);
		code = ssd;
	}
	if (ret)
		return ret;

	if (code >= entries)
		return -EINVAL;

	return adp20086_iio_format_milli(buf, len, table[code]);
}

/**
 * @brief Set one of the per-output ramps of an IOUT channel.
 *
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_iio_channel_ramp to update.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_ramp(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	const uint32_t *table;
	enum adp20086_channel ch;
	uint32_t micro;
	int entries, code;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	entries = adp20086_iio_ramp_table(priv, &table);
	if (entries < 0)
		return entries;

	ret = adp20086_iio_parse_milli(buf, &micro);
	if (ret)
		return ret;

	for (code = 0; code < entries; code++)
		if (table[code] == micro)
			break;

	if (code == entries)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;
	ch = channel->address - ADP20086_IIO_IOUT1;

	if (priv == ADP20086_IIO_SOFT_STARTUP)
		ret = adp20086_set_soft_startup(adp20086, ch, code);
	else
		ret = adp20086_set_soft_shutdown(adp20086, ch, code);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief List the ramp durations the device offers, in ms.
 *
 * Emitted in the same format the ramp attributes read back in, so that a
 * client can match a value against this list verbatim.
 *
 * @param dev - Unused.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - The enum adp20086_iio_channel_ramp to list.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_ramp_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	const uint32_t *table;
	int entries, code;
	int length = 0;
	int ret;

	entries = adp20086_iio_ramp_table(priv, &table);
	if (entries < 0)
		return entries;

	for (code = 0; code < entries; code++) {
		ret = snprintf(buf + length, len - length, "%s%lu.%06lu",
			       code ? " " : "",
			       (unsigned long)(table[code] / MILLI),
			       (unsigned long)((table[code] % MILLI) * MILLI));
		if (ret < 0)
			return ret;
		if ((uint32_t)ret >= len - length)
			return -ENOMEM;

		length += ret;
	}

	return length;
}

/* Legacy ADC mux settings, indexed by enum adp20086_legacy_adc_mux_config. */
static const char * const adp20086_mux_config[] = {
	[ADP20086_LEGACY_MUX_IOUT] = "iout",
	[ADP20086_LEGACY_MUX_VOUT] = "vout",
	[ADP20086_LEGACY_MUX_VIN_VDD] = "vin_vdd",
};

/**
 * @brief Read which quantity the legacy ADC mux is routing.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - Unused.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_mux(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	enum adp20086_legacy_adc_mux_config config;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_get_mux_config(iio_adp20086->adp20086_dev, &config);
	if (ret)
		return ret;

	/* The field is two bits wide but only three settings are defined. */
	if (config >= NO_OS_ARRAY_SIZE(adp20086_mux_config))
		return -EINVAL;

	return snprintf(buf, len, "%s", adp20086_mux_config[config]);
}

/**
 * @brief Select which quantity the legacy ADC mux routes.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written setting.
 * @param len - Length of the input buffer.
 * @param channel - Unused.
 * @param priv - Unused.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_mux(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	uint32_t i;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp20086_mux_config); i++) {
		if (strcmp(buf, adp20086_mux_config[i]))
			continue;

		ret = adp20086_set_mux_config(iio_adp20086->adp20086_dev, i);
		if (ret)
			return ret;

		return len;
	}

	return -EINVAL;
}

/**
 * @brief List the legacy ADC mux settings the device offers.
 * @param dev - Unused.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - Unused.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_mux_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t i;
	int length = 0;
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adp20086_mux_config); i++) {
		ret = snprintf(buf + length, len - length, "%s%s",
			       i ? " " : "", adp20086_mux_config[i]);
		if (ret < 0)
			return ret;
		if ((uint32_t)ret >= len - length)
			return -ENOMEM;

		length += ret;
	}

	return length;
}

/**
 * @brief Read one of the device-level input thresholds.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - The enum adp20086_iio_device_threshold to report.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_device_threshold(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	uint32_t micro;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	switch (priv) {
	case ADP20086_IIO_OVIN:
		ret = adp20086_get_ovin(iio_adp20086->adp20086_dev, &micro);
		break;
	case ADP20086_IIO_UVIN:
		ret = adp20086_get_uvin(iio_adp20086->adp20086_dev, &micro);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return adp20086_iio_format_milli(buf, len, micro);
}

/**
 * @brief Set one of the device-level input thresholds.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Unused.
 * @param priv - The enum adp20086_iio_device_threshold to update.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_device_threshold(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	uint32_t micro;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_iio_parse_milli(buf, &micro);
	if (ret)
		return ret;

	switch (priv) {
	case ADP20086_IIO_OVIN:
		ret = adp20086_set_ovin(iio_adp20086->adp20086_dev, micro);
		break;
	case ADP20086_IIO_UVIN:
		ret = adp20086_set_uvin(iio_adp20086->adp20086_dev, micro);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Pick one channel's flag out of a struct adp20086_enabled_channels.
 * @param channel - Channel the attribute belongs to.
 * @param channels - The per-channel flags reported by the core driver.
 * @param flag - The flag of the channel the attribute belongs to.
 * @return 0 in case of success, error code otherwise.
 */
static int adp20086_iio_channel_flag(const struct iio_ch_info *channel,
				     const struct adp20086_enabled_channels *channels,
				     bool *flag)
{
	switch (channel->address) {
	case ADP20086_IIO_IOUT1:
		*flag = channels->out1;
		break;
	case ADP20086_IIO_IOUT2:
		*flag = channels->out2;
		break;
	case ADP20086_IIO_IOUT3:
		*flag = channels->out3;
		break;
	case ADP20086_IIO_IOUT4:
		*flag = channels->out4;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Read one of the per-output enables of an IOUT channel.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_iio_channel_enable to report.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_en(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_enabled_channels enabled;
	struct adp20086_dev *adp20086;
	bool flag;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	adp20086 = iio_adp20086->adp20086_dev;

	switch (priv) {
	case ADP20086_IIO_EN:
		ret = adp20086_get_enabled_channels(adp20086, &enabled);
		break;
	case ADP20086_IIO_LOAD_DETECT_EN:
		ret = adp20086_get_enabled_load_detection(adp20086, &enabled);
		break;
	case ADP20086_IIO_OPEN_PROT_EN:
		ret = adp20086_get_enabled_open_protection(adp20086, &enabled);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	ret = adp20086_iio_channel_flag(channel, &enabled, &flag);
	if (ret)
		return ret;

	val = flag;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Set one of the per-output enables of an IOUT channel.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_iio_channel_enable to update.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_en(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	struct adp20086_dev *adp20086;
	enum adp20086_channel ch;
	bool enable;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	ret = adp20086_iio_parse_bool(buf, &enable);
	if (ret)
		return ret;

	adp20086 = iio_adp20086->adp20086_dev;
	ch = channel->address - ADP20086_IIO_IOUT1;

	switch (priv) {
	case ADP20086_IIO_EN:
		ret = adp20086_set_channel(adp20086, ch, enable);
		break;
	case ADP20086_IIO_LOAD_DETECT_EN:
		ret = adp20086_set_load_detection(adp20086, ch, enable);
		break;
	case ADP20086_IIO_OPEN_PROT_EN:
		ret = adp20086_set_open_protection(adp20086, ch, enable);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return len;
}

/**
 * @brief List the accepted values of the device's boolean attributes.
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
 * @brief Read the mask bit of an interrupt source.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - The enum adp20086_interrupt_source to report.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_mask(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	bool masked;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_get_interrupt_mask(iio_adp20086->adp20086_dev, priv,
					  &masked);
	if (ret)
		return ret;

	val = masked;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Mask or unmask an interrupt source.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Unused.
 * @param priv - The enum adp20086_interrupt_source to update.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_mask(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	bool masked;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_iio_parse_bool(buf, &masked);
	if (ret)
		return ret;

	ret = adp20086_set_interrupt_mask(iio_adp20086->adp20086_dev, priv,
					  masked);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Resolve the interrupt source behind a per-channel mask attribute.
 * @param channel - Channel the attribute belongs to.
 * @param ch1_src - The enum adp20086_interrupt_source of channel 1.
 * @param src - The resolved interrupt source.
 * @return 0 in case of success, error code otherwise.
 */
static int adp20086_iio_channel_int_src(const struct iio_ch_info *channel,
					intptr_t ch1_src,
					enum adp20086_interrupt_source *src)
{
	if (channel->address < ADP20086_IIO_IOUT1 ||
	    channel->address > ADP20086_IIO_IOUT4)
		return -EINVAL;

	*src = ch1_src - (channel->address - ADP20086_IIO_IOUT1);

	return 0;
}

/**
 * @brief Read the mask bit of a per-channel interrupt source.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_interrupt_source of channel 1.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_channel_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	enum adp20086_interrupt_source src;
	int ret;

	ret = adp20086_iio_channel_int_src(channel, priv, &src);
	if (ret)
		return ret;

	return adp20086_iio_read_mask(dev, buf, len, channel, src);
}

/**
 * @brief Mask or unmask a per-channel interrupt source.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Input buffer holding the written value.
 * @param len - Length of the input buffer.
 * @param channel - Channel the attribute belongs to.
 * @param priv - The enum adp20086_interrupt_source of channel 1.
 * @return Number of bytes read from buf, error code otherwise.
 */
static int adp20086_iio_write_channel_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	enum adp20086_interrupt_source src;
	int ret;

	ret = adp20086_iio_channel_int_src(channel, priv, &src);
	if (ret)
		return ret;

	return adp20086_iio_write_mask(dev, buf, len, channel, src);
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
	case ADP20086_IIO_ENC:
		ret = adp20086_get_enc_config(adp20086, &flag);
		val = flag;
		break;
	case ADP20086_IIO_OVTST_EN:
		ret = adp20086_get_ovtst(adp20086, &flag);
		val = flag;
		break;
	case ADP20086_IIO_IIR_FILTER:
		ret = adp20086_get_iir_filter(adp20086, &flag);
		val = flag;
		break;
	case ADP20086_IIO_DISCHARGE_RESISTORS:
		ret = adp20086_get_discharge_resistors(adp20086, &flag);
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
	case ADP20086_IIO_ENC:
		ret = adp20086_set_enc_config(adp20086, enable);
		break;
	case ADP20086_IIO_OVTST_EN:
		ret = adp20086_set_ovtst(adp20086, enable);
		break;
	case ADP20086_IIO_IIR_FILTER:
		ret = adp20086_set_iir_filter(adp20086, enable);
		break;
	case ADP20086_IIO_DISCHARGE_RESISTORS:
		ret = adp20086_set_discharge_resistors(adp20086, enable);
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
 * @brief Read the raw code held by one of the legacy ADC registers.
 * @param dev - The ADP20086 IIO device descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the output buffer.
 * @param channel - Unused.
 * @param priv - The legacy ADC to read, an enum adp20086_legacy_adc.
 * @return Number of bytes written to buf, error code otherwise.
 */
static int adp20086_iio_read_legacy_adc(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adp20086_iio_desc *iio_adp20086 = dev;
	uint8_t code;
	int32_t val;
	int ret;

	if (!iio_adp20086)
		return -EINVAL;

	ret = adp20086_read_legacy_adc(iio_adp20086->adp20086_dev, priv, &code);
	if (ret)
		return ret;

	val = code;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
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

	ret = adp20086_clear_status(descriptor->adp20086_dev);
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

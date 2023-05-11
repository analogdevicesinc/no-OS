#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "iio.h"
#include "iio_ad413x.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "ad413x.h"

struct scan_type ad413x_iio_scan_type = {
	.sign = 'u',
	.realbits = 24,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true
};

static int32_t _ad413x_read_register(void *device, uint32_t reg,
				     uint32_t *readval)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;

	return ad413x_reg_read(iiodev->ad413x_dev, reg, readval);
}

static int32_t _ad413x_write_register(void *device, uint32_t reg,
				      uint32_t writeval)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;

	return ad413x_reg_write(iiodev->ad413x_dev, reg, writeval);
}

static int32_t ad413x_iio_update_active_channels(void *device,
		uint32_t mask)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	uint32_t ch_idx, readval;
	int32_t ret;

	while (mask) {
		ch_idx = no_os_find_first_set_bit(mask);
		mask &= ~(1 << ch_idx);
		ret = ad413x_reg_read(iiodev->ad413x_dev, AD413X_REG_CHN(ch_idx), &readval);
		if (ret)
			return ret;
		readval |= AD413X_ENABLE_M;
		ret = ad413x_reg_write(iiodev->ad413x_dev, AD413X_REG_CHN(ch_idx), readval);
		if (ret)
			return ret;
	}

	return 0;
}

static int32_t ad413x_iio_get_active_channels(void *device,
		uint32_t *mask)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	int32_t ret, ch_idx;
	uint32_t readval;

	*mask = 0;
	for (ch_idx = 0; ch_idx < 16; ch_idx++) {
		ret = ad413x_reg_read(iiodev->ad413x_dev, AD413X_REG_CHN(ch_idx), &readval);
		if (ret)
			return ret;
		if (readval & AD413X_ENABLE_M)
			*mask |= 1 << ch_idx;
	}

	return 0;
}

static int32_t ad413x_iio_close_channels(void *device)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	int32_t ret, ch_idx;
	uint32_t readval;

	for (ch_idx = 0; ch_idx < 16; ch_idx++) {
		ret = ad413x_reg_read(iiodev->ad413x_dev, AD413X_REG_CHN(ch_idx), &readval);
		if (ret)
			return ret;
		readval &= ~AD413X_ENABLE_M;
		ad413x_reg_write(iiodev->ad413x_dev, AD413X_REG_CHN(ch_idx), readval);
		if (ret)
			return ret;
	}

	return 0;
}

static int32_t ad413x_select_channel(void *device,
				     const struct iio_ch_info *ch_info)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	int32_t ret, idx;

	for (idx = 0; idx < 16; idx++) {
		ret = ad413x_ch_en(iiodev->ad413x_dev, idx, idx == ch_info->ch_num ? 1U : 0U);
		if (ret)
			return ret;
	}

	return ret;
}


static int ad413x_iio_read_calibscale(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	struct ad413x_dev *dev = iiodev->ad413x_dev;
	int32_t vals;
	uint8_t pr_nb;
	enum ad413x_gain gain;

	pr_nb = dev->ch[channel->ch_num].preset;
	gain = dev->preset[pr_nb].gain;

	switch (gain) {
	case AD413X_GAIN_1:
		vals = 1;
		break;
	case AD413X_GAIN_2:
		vals = 2;
		break;
	case AD413X_GAIN_4:
		vals = 4;
		break;
	case AD413X_GAIN_8:
		vals = 8;
		break;
	case AD413X_GAIN_16:
		vals = 16;
		break;
	case AD413X_GAIN_32:
		vals = 32;
		break;
	case AD413X_GAIN_64:
		vals = 64;
		break;
	case AD413X_GAIN_128:
		vals = 128;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals);
}

static int ad413x_iio_read_offset(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	struct ad413x_dev *dev = iiodev->ad413x_dev;
	int32_t value, ret;
	uint32_t readval;

	ret = ad413x_reg_read(dev, AD413X_REG_ADC_CTRL, &readval);
	if (ret)
		return ret;

	if (readval & AD413X_ADC_BIPOLAR)
		value = -0x800000;
	else
		value = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

static int ad413x_iio_read_raw(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	int32_t ret;
	uint32_t value, reg_data;

	ret = ad413x_select_channel(iiodev, channel);
	if (ret)
		return ret;

	ret = ad413x_single_conv(iiodev->ad413x_dev, &value, 1);
	if (ret)
		return ret;

	/* Check if data_en is enabled */
	ret = ad413x_reg_read(iiodev->ad413x_dev, AD413X_REG_ADC_CTRL, &reg_data);
	if (ret)
		return ret;
	if (reg_data | AD413X_ADC_DATA_STATUS)
		value >>= 8;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

static int ad413x_iio_scale_available(void *device, char *buf,
				      uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	//        2.5V unipolar, 1.25V unipolar, 2.5V bipolar, 1.25V bipolar
	strcpy(buf, "0.000149011U 0.000074505U 0.000298023B 0.000149011B");
	return strlen(buf);
}

static int ad413x_iio_store_scale(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	struct ad413x_dev *dev = (struct ad413x_dev *)iiodev->ad413x_dev;
	int32_t ret;
	char polarity;

	polarity = buf[11];

	//delete last char
	strcpy(&buf[11], &buf[12]);

	switch (polarity) {
	case 'U':
		//unipolar
		ret = ad413x_adc_bipolar(dev, 0U);
		if (ret)
			return ret;
		if (!strcmp(buf, "0.000149011"))
			ret = ad413x_set_int_ref(dev, AD413X_INTREF_2_5V);
		else if (!strcmp(buf, "0.000074505"))
			ret = ad413x_set_int_ref(dev, AD413X_INTREF_1_25V);
		else
			ret = -EINVAL;
		break;
	case 'B':
		//bipolar
		ret = ad413x_adc_bipolar(dev, 1U);
		if (ret)
			return ret;
		if (!strcmp(buf, "0.000298023"))
			ret = ad413x_set_int_ref(dev, AD413X_INTREF_2_5V);
		else if (!strcmp(buf, "0.000149011"))
			ret = ad413x_set_int_ref(dev, AD413X_INTREF_1_25V);
		else
			ret = -EINVAL;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int ad413x_iio_read_scale(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	uint8_t bi = 0, ref_val = 0;
	int32_t valt, ret;
	int32_t vals[2];
	uint32_t readval;

	ret = ad413x_reg_read(iiodev->ad413x_dev, AD413X_REG_ADC_CTRL, &readval);
	if (ret)
		return ret;

	if (readval & AD413X_ADC_BIPOLAR)
		bi = 1;

	if (readval & AD413X_ADC_REF_VAL)
		ref_val = 1;

	if (bi) {
		ad413x_iio_scan_type.sign = 's';
		if (ref_val == 0) {
			/* bipolar, 2.5Vref */
			vals[0] = 2500;
			vals[1] = 0x800000;
			valt = IIO_VAL_FRACTIONAL;
		} else {
			/* bipolar, 1.25Vref */
			vals[0] = 1250;
			vals[1] = 0x800000;
			valt = IIO_VAL_FRACTIONAL;
		}
	} else {
		ad413x_iio_scan_type.sign = 'u';
		if (ref_val == 0) {
			/* unipolar, 2.5Vref */
			vals[0] = 2500;
			vals[1] = 0xFFFFFF;
			valt = IIO_VAL_FRACTIONAL;
		} else {
			/* unipolar, 1.25Vref */
			vals[0] = 1250;
			vals[1] = 0xFFFFFF;
			valt = IIO_VAL_FRACTIONAL;
		}
	}

	return iio_format_value(buf, len, valt, 2, vals);
}

static int32_t ad413x_iio_read_samples(void *device,
				       int32_t *buff,
				       uint32_t nb_samples)
{
	struct ad413x_iio_dev *iiodev = (struct ad413x_iio_dev *)device;
	int32_t ret, ch_nb = 0;
	uint32_t mask;
	ret = ad413x_iio_get_active_channels(device, &mask);
	if (ret)
		return ret;

	while (mask) {
		ch_nb += mask & 1;
		mask >>= 1;
	}

	ret = ad413x_continuous_conv(iiodev->ad413x_dev, buff, ch_nb, nb_samples);
	if (ret)
		return ret;

	return nb_samples;
}

static struct iio_attribute ad413x_iio_vin_attrs[] = {
	{
		.name = "raw",
		.shared = IIO_SEPARATE,
		.show = ad413x_iio_read_raw,
		.store = NULL
	},
	{
		.name = "scale",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad413x_iio_read_scale,
		.store = ad413x_iio_store_scale
	},
	{
		.name = "offset",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad413x_iio_read_offset,
		.store = NULL
	},
	{
		.name = "calibscale",
		.shared = IIO_SEPARATE,
		.show = ad413x_iio_read_calibscale,
		.store = NULL
	},
	{
		.name = "scale_options",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad413x_iio_scale_available,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY
};

enum ad413x_chan {
	CH0,
	CH1,
	CH2,
	CH3,
	CH4,
	CH5,
	CH6,
	CH7,
	CH8,
	CH9,
	CH10,
	CH11,
	CH12,
	CH13,
	CH14,
	CH15
};

#define AD413X_IIO_CHANN_DEF(nm, ch_nb) \
	{ \
		.name = nm, \
		.ch_type = IIO_VOLTAGE, \
		.indexed = true, \
		.scan_type = &ad413x_iio_scan_type, \
		.scan_index = ch_nb, \
		.channel = ch_nb, \
		.attributes = ad413x_iio_vin_attrs, \
		.address = NULL, \
		.ch_out = 0, \
	}

static struct iio_channel ad413x_channels[] = {
	AD413X_IIO_CHANN_DEF("ch0",   0),
	AD413X_IIO_CHANN_DEF("ch1",   1),
	AD413X_IIO_CHANN_DEF("ch2",   2),
	AD413X_IIO_CHANN_DEF("ch3",   3),
	AD413X_IIO_CHANN_DEF("ch4",   4),
	AD413X_IIO_CHANN_DEF("ch5",   5),
	AD413X_IIO_CHANN_DEF("ch6",   6),
	AD413X_IIO_CHANN_DEF("ch7",   7),
	AD413X_IIO_CHANN_DEF("ch8",   8),
	AD413X_IIO_CHANN_DEF("ch9",   9),
	AD413X_IIO_CHANN_DEF("ch10", 10),
	AD413X_IIO_CHANN_DEF("ch11", 11),
	AD413X_IIO_CHANN_DEF("ch12", 12),
	AD413X_IIO_CHANN_DEF("ch13", 13),
	AD413X_IIO_CHANN_DEF("ch14", 14),
	AD413X_IIO_CHANN_DEF("ch15", 15),
};

static struct iio_device ad413x_iio_device = {
	.num_ch = NO_OS_ARRAY_SIZE(ad413x_channels),
	.channels = ad413x_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = ad413x_iio_update_active_channels,
	.post_disable = ad413x_iio_close_channels,
	.read_dev = (int32_t (*)())ad413x_iio_read_samples,
	.debug_reg_read = (int32_t (*)())_ad413x_read_register,
	.debug_reg_write = (int32_t (*)())_ad413x_write_register
};

int32_t ad413x_iio_init(struct ad413x_iio_dev **iio_dev,
			struct ad413x_iio_init_param init_param)
{
	int32_t ret;
	struct ad413x_iio_dev *desc;

	desc = (struct ad413x_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -1;

	desc->iio_dev = &ad413x_iio_device;

	ret = ad413x_init(&desc->ad413x_dev, init_param.ad413x_ip);
	if (ret != 0)
		goto error_desc;

	*iio_dev = desc;

	return 0;
error_desc:
	no_os_free(desc);

	return ret;
}

int32_t ad413x_iio_remove(struct ad413x_iio_dev *desc)
{
	int32_t ret;

	ret = ad413x_remove(desc->ad413x_dev);
	if (ret != 0)
		return ret;

	no_os_free(desc);

	return 0;
}

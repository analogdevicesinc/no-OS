#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "iio.h"
#include "iio_ad7746.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "ad7746.h"
#include <string.h>

static int32_t _ad7746_read_register2(struct ad7746_iio_dev *dev, uint32_t reg,
				      uint32_t *readval)
{
	return ad7746_reg_read(dev->ad7746_dev, reg, (uint8_t *)readval, 1);
}

static int32_t _ad7746_write_register2(struct ad7746_iio_dev *dev, uint32_t reg,
				       uint32_t writeval)
{
	uint8_t val = writeval;
	return ad7746_reg_write(dev->ad7746_dev, reg, &val, 1);
}

static inline bool _capdiff(struct ad7746_cap *cap1, struct ad7746_cap *cap2)
{
	return cap1->capchop != cap2->capchop ||
	       cap1->capdiff != cap2->capdiff ||
	       cap1->capen != cap2->capen ||
	       cap1->cin2 != cap2->cin2;
}

static inline bool _vtdiff(struct ad7746_vt *vt1, struct ad7746_vt *vt2)
{
	return vt1->extref != vt2->extref ||
	       vt1->vtchop != vt2->vtchop ||
	       vt1->vten != vt2->vten ||
	       vt1->vtmd != vt2->vtmd ||
	       vt1->vtshort != vt2->vtshort;
}

static inline bool _configdiff(struct ad7746_config *c1,
			       struct ad7746_config *c2)
{
	return c1->md != c2->md ||
	       c1->capf != c2->capf ||
	       c1->vtf != c2->vtf;
}

/* Values are Update Rate (Hz), Conversion Time (ms) + 1*/
static const uint8_t ad7746_vt_filter_rate_table[][2] = {
	{50, 20 + 1}, {31, 32 + 1}, {16, 62 + 1}, {8, 122 + 1},
};

static const uint8_t ad7746_cap_filter_rate_table[][2] = {
	{91, 11 + 1}, {84, 12 + 1}, {50, 20 + 1}, {26, 38 + 1},
	{16, 62 + 1}, {13, 77 + 1}, {11, 92 + 1}, {9, 110 + 1},
};

static int32_t ad7746_store_cap_filter_rate_setup(struct ad7746_dev *chip,
		int32_t val)
{
	uint32_t i;
	struct ad7746_config c = chip->setup.config;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad7746_cap_filter_rate_table); i++)
		if (val >= ad7746_cap_filter_rate_table[i][0])
			break;

	if (i >= NO_OS_ARRAY_SIZE(ad7746_cap_filter_rate_table))
		i = NO_OS_ARRAY_SIZE(ad7746_cap_filter_rate_table) - 1;

	c.capf = i;
	return ad7746_set_config(chip, c);
}

static int32_t ad7746_store_vt_filter_rate_setup(struct ad7746_dev *chip,
		int32_t val)
{
	uint32_t i;
	struct ad7746_config c = chip->setup.config;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad7746_vt_filter_rate_table); i++)
		if (val >= ad7746_vt_filter_rate_table[i][0])
			break;

	if (i >= NO_OS_ARRAY_SIZE(ad7746_vt_filter_rate_table))
		i = NO_OS_ARRAY_SIZE(ad7746_vt_filter_rate_table) - 1;

	c.vtf = i;
	return ad7746_set_config(chip, c);
}

// perform channel selection
static int32_t ad7746_select_channel(void *device,
				     const struct iio_ch_info *ch_info)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	struct ad7746_cap cap = desc->setup.cap;
	struct ad7746_vt vt = desc->setup.vt;
	int32_t ret, delay, idx;

	switch (ch_info->type) {
	case IIO_CAPACITANCE:
		cap.capen = true;
		cap.capdiff = ch_info->address & AD7746_CAPSETUP_CAPDIFF_MSK ? true : false;
		cap.cin2 = ch_info->address & AD7746_CAPSETUP_CIN2_MSK ? true : false;
		vt.vten = false;
		idx = desc->setup.config.capf;
		delay = ad7746_cap_filter_rate_table[idx][1];

		if (iiodev->capdac_set != ch_info->ch_num) {
			bool en;
			uint8_t code;

			en = (bool)(iiodev->capdac[ch_info->ch_num][0] & AD7746_CAPDAC_DACEN_MSK);
			code = iiodev->capdac[ch_info->ch_num][0] & AD7746_CAPDAC_DACP_MSK;
			ret = ad7746_set_cap_dac_a(desc, en, code);
			if (ret < 0)
				return ret;

			en = (bool)(iiodev->capdac[ch_info->ch_num][1] & AD7746_CAPDAC_DACEN_MSK);
			code = iiodev->capdac[ch_info->ch_num][1] & AD7746_CAPDAC_DACP_MSK;
			ret = ad7746_set_cap_dac_b(desc, en, code);
			if (ret < 0)
				return ret;

			iiodev->capdac_set = ch_info->ch_num;
		}
		break;
	case IIO_VOLTAGE:
		if (ch_info->ch_num == 1)
			vt.vtmd = AD7746_VTMD_VDD_MON;
		else
			vt.vtmd = AD7746_VIN_EXT_VIN;
		vt.vten = true;
		cap.capen = false;
		idx = desc->setup.config.vtf;
		delay = ad7746_cap_filter_rate_table[idx][1];
		break;
	case IIO_TEMP:
		if (ch_info->ch_num == 1)
			vt.vtmd = AD7746_VTMD_EXT_TEMP;
		else
			vt.vtmd = AD7746_VTMD_INT_TEMP;
		vt.vten = true;
		cap.capen = false;
		idx = desc->setup.config.vtf;
		delay = ad7746_cap_filter_rate_table[idx][1];
		break;
	default:
		return -EINVAL;
	}

	if (_capdiff(&desc->setup.cap, &cap)) {
		ret = ad7746_set_cap(desc, cap);
		if (ret < 0)
			return ret;
	}

	if (_vtdiff(&desc->setup.vt, &vt)) {
		ret = ad7746_set_vt(desc, vt);
		if (ret < 0)
			return ret;
	}

	return delay;
}

static int ad7746_iio_read_raw(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t ret, delay, value;
	uint32_t reg;
	struct ad7746_config c;

	ret = ad7746_select_channel(iiodev, channel);
	if (ret < 0)
		return ret;
	delay = ret;

	c = desc->setup.config;
	c.md = AD7746_MODE_SINGLE;

	if(_configdiff(&desc->setup.config, &c)) {
		ret = ad7746_set_config(desc, c);
		if (ret < 0)
			return ret;
	}

	no_os_mdelay(delay);

	switch (channel->type) {
	case IIO_TEMP:
		ret = ad7746_get_vt_data(desc, &reg);
		if (ret < 0)
			return ret;

		value = (reg & 0xffffff) - 0x800000;
		/*
		* temperature in milli degrees Celsius
		* T = ((*val / 2048) - 4096) * 1000
		*/
		value = (value * 125) / 256;
		break;
	case IIO_VOLTAGE:
		ret = ad7746_get_vt_data(desc, &reg);
		if (ret < 0)
			return ret;

		value = (reg & 0xffffff) - 0x800000;

		if (channel->ch_num == 1) /* supply_raw */
			value = value * 6;
		break;
	case IIO_CAPACITANCE:
		ret = ad7746_get_cap_data(desc, &reg);
		if (ret < 0)
			return ret;

		value = (reg & 0xffffff) - 0x800000;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

static int ad7746_iio_read_scale(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int32_t valt;
	int32_t vals[2];

	switch (channel->type) {
	case IIO_CAPACITANCE:
		/* 8.192pf / 2^24 */
		vals[0] =  0;
		vals[1] = 488;
		valt = IIO_VAL_INT_PLUS_NANO;
		break;
	case IIO_VOLTAGE:
		/* 1170mV / 2^23 */
		vals[0] = 1170;
		vals[1] = 23;
		valt = IIO_VAL_FRACTIONAL_LOG2;
		break;
	default:
		return -EINVAL;
		break;
	}

	return iio_format_value(buf, len, valt, 2, vals);
}

static int ad7746_iio_read_offset(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	int32_t value;

	value = (iiodev->capdac[channel->ch_num][channel->differential] &
		 AD7746_CAPDAC_DACP_MSK) * 338646;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

static int ad7746_iio_write_offset(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t val, ret;
	bool en;
	uint8_t code;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	if (val < 0 || val > 43008000) { /* 21pF */
		return -EINVAL;
	}

	/*
	* CAPDAC Scale = 21pF_typ / 127
	* CIN Scale = 8.192pF / 2^24
	* Offset Scale = CAPDAC Scale / CIN Scale = 338646
	*/

	val /= 338646;

	en = val > 0;
	code = en ? val : 0;
	iiodev->capdac[channel->ch_num][channel->differential] =
		en ? (code & AD7746_CAPDAC_DACP_MSK) | AD7746_CAPDAC_DACEN_MSK : 0;

	en = (bool)(iiodev->capdac[channel->ch_num][0] & AD7746_CAPDAC_DACEN_MSK);
	code = iiodev->capdac[channel->ch_num][0] & AD7746_CAPDAC_DACP_MSK;
	ret = ad7746_set_cap_dac_a(desc, en, code);
	if (ret < 0)
		return ret;

	en = (bool)(iiodev->capdac[channel->ch_num][1] & AD7746_CAPDAC_DACEN_MSK);
	code = iiodev->capdac[channel->ch_num][1] &AD7746_CAPDAC_DACP_MSK;
	ret = ad7746_set_cap_dac_b(desc, en, code);
	if (ret < 0)
		return ret;

	iiodev->capdac_set = channel->ch_num;

	ret = 0;

	return len;
}

static int ad7746_iio_read_samp_freq(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t value;

	switch (channel->type) {
	case IIO_CAPACITANCE:
		value = ad7746_cap_filter_rate_table[desc->setup.config.capf][0];
		break;
	case IIO_VOLTAGE:
		value = ad7746_vt_filter_rate_table[desc->setup.config.vtf][0];
		break;
	default:
		return -EINVAL;
		break;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

static int ad7746_iio_write_samp_freq(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t val, val2, ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, &val2);
	if (ret < 0)
		return ret;

	if (val2) {
		return -EINVAL;
	}

	switch (channel->type) {
	case IIO_CAPACITANCE:
		ret = ad7746_store_cap_filter_rate_setup(desc, val);
		break;
	case IIO_VOLTAGE:
		ret = ad7746_store_vt_filter_rate_setup(desc, val);
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int ad7746_iio_read_samp_freq_avail(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret = 0;

	switch (channel->type) {
	case IIO_CAPACITANCE:
		strcpy(buf, "91 84 50 26 16 13 11 9");
		ret = strlen(buf);
		break;
	case IIO_VOLTAGE:
		strcpy(buf, "50 31 16 8");
		ret = strlen(buf);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int ad7746_iio_read_calibscale(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t vals[2];
	uint8_t regval[2];
	uint8_t reg;
	int32_t ret;

	switch (channel->type) {
	case IIO_CAPACITANCE:
		reg = AD7746_REG_CAP_GAINH;
		break;
	case IIO_VOLTAGE:
		reg = AD7746_REG_VOLT_GAINH;
		break;
	default:
		return -EINVAL;
	}

	ret = ad7746_reg_read(desc, reg, regval, 2);
	if (ret < 0)
		return ret;
	/* 1 + gain_val / 2^16 */
	vals[0] = 1;
	vals[1] = (15625 * (((uint32_t)regval[0] << 8) | regval[1])) / 1024;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
}

static int ad7746_iio_write_calibscale(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t val, val2, ret;

	ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val, &val2);
	if (ret < 0)
		return ret;

	if (val != 1)
		return -EINVAL;

	val = (val2 * 1024) / 15625;

	switch (channel->type) {
	case IIO_CAPACITANCE:
		return ad7746_set_cap_gain(desc, (uint16_t)val);
		break;
	case IIO_VOLTAGE:
		return ad7746_set_volt_gain(desc, (uint16_t)val);
		break;
	default:
		return -EINVAL;
		break;
	}
}

static int ad7746_iio_read_calibbias(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t value;
	uint8_t regval[2];
	int32_t ret;

	ret = ad7746_reg_read(desc, AD7746_REG_CAP_OFFH, regval, 2);
	if (ret < 0)
		return ret;

	value = ((uint32_t)regval[0] << 8) | regval[1];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

static int ad7746_iio_write_calibbias(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	int32_t val, ret;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	if (val < 0 || val > 0xFFFF) {
		return -EINVAL;
	}

	return ad7746_set_cap_offset(desc, (uint16_t)val);
}

static int ad7746_start_offset_calib(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t ret;
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	bool doit = strcmp(buf, "");

	if (!doit)
		return 0;

	ret = ad7746_select_channel(iiodev, channel);
	if (ret < 0)
		return ret;

	ret = ad7746_calibrate(desc, AD7746_MODE_OFFSET_CALIB);
	if (ret < 0)
		return ret;

	return len;
}

static int ad7746_start_gain_calib(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t ret;
	struct ad7746_iio_dev *iiodev = (struct ad7746_iio_dev *)device;
	struct ad7746_dev *desc = (struct ad7746_dev *)iiodev->ad7746_dev;
	bool doit = strcmp(buf, "");

	if (!doit)
		return 0;

	ret = ad7746_select_channel(iiodev, channel);
	if (ret < 0)
		return ret;

	ret = ad7746_calibrate(desc, AD7746_MODE_GAIN_CALIB);
	if (ret < 0)
		return ret;

	return len;
}

static struct iio_attribute ad7746_iio_vin_attrs[] = {
	{
		.name = "raw",
		.show = ad7746_iio_read_raw,
		.store = NULL
	},
	{
		.name = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad7746_iio_read_scale,
		.store = NULL
	},
	{
		.name = "sampling_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad7746_iio_read_samp_freq,
		.store = ad7746_iio_write_samp_freq
	},
	{
		.name = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad7746_iio_read_samp_freq_avail,
		.store = NULL
	},
	{
		.name = "calibbias_calibration",
		.show = NULL,
		.store = ad7746_start_offset_calib
	},
	{
		.name = "calibscale_calibration",
		.show = NULL,
		.store = ad7746_start_gain_calib
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad7746_iio_cin_attrs[] = {
	{
		.name = "raw",
		.show = ad7746_iio_read_raw,
		.store = NULL
	},
	{
		.name = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad7746_iio_read_scale,
		.store = NULL
	},
	{
		.name = "offset",
		.show = ad7746_iio_read_offset,
		.store = ad7746_iio_write_offset
	},
	{
		.name = "sampling_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad7746_iio_read_samp_freq,
		.store = ad7746_iio_write_samp_freq
	},
	{
		.name = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad7746_iio_read_samp_freq_avail,
		.store = NULL
	},
	{
		.name = "calibscale",
		.show = ad7746_iio_read_calibscale,
		.store = ad7746_iio_write_calibscale
	},
	{
		.name = "calibbias",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad7746_iio_read_calibbias,
		.store = ad7746_iio_write_calibbias
	},
	{
		.name = "calibbias_calibration",
		.show = NULL,
		.store = ad7746_start_offset_calib
	},
	{
		.name = "calibscale_calibration",
		.show = NULL,
		.store = ad7746_start_gain_calib
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad7746_iio_temp_attrs[] = {
	{
		.name = "input",
		.show = ad7746_iio_read_raw,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY
};

enum ad7746_chan {
	VIN,
	VIN_VDD,
	TEMP_INT,
	TEMP_EXT,
	CIN1,
	CIN1_DIFF,
	CIN2,
	CIN2_DIFF,
};

static struct iio_channel ad7746_channels[] = {
	[VIN] = {
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = 0,
		.attributes = ad7746_iio_vin_attrs,
		.address = AD7746_VIN_EXT_VIN,
		.ch_out = false,
	},
	[VIN_VDD] = {
		.ch_type = IIO_VOLTAGE,
		.indexed = true,
		.channel = 1,
		.attributes = ad7746_iio_vin_attrs,
		.address = AD7746_VTMD_VDD_MON,
		.ch_out = false,
	},
	[TEMP_INT] = {
		.ch_type = IIO_TEMP,
		.indexed = true,
		.channel = 0,
		.attributes = ad7746_iio_temp_attrs,
		.address = AD7746_VTMD_INT_TEMP,
		.ch_out = false,
	},
	[TEMP_EXT] = {
		.ch_type = IIO_TEMP,
		.indexed = true,
		.channel = 1,
		.attributes = ad7746_iio_temp_attrs,
		.address = AD7746_VTMD_EXT_TEMP,
		.ch_out = false,
	},
	[CIN1] = {
		.ch_type = IIO_CAPACITANCE,
		.indexed = true,
		.channel = 0,
		.attributes = ad7746_iio_cin_attrs,
		.ch_out = false,
	},
	[CIN1_DIFF] = {
		.ch_type = IIO_CAPACITANCE,
		.diferential = true,
		.indexed = true,
		.channel = 0,
		.channel2 = 2,
		.attributes = ad7746_iio_cin_attrs,
		.address = AD7746_CAPSETUP_CAPDIFF_MSK,
		.ch_out = false,
	},
	[CIN2] = {
		.ch_type = IIO_CAPACITANCE,
		.indexed = true,
		.channel = 1,
		.attributes = ad7746_iio_cin_attrs,
		.address = AD7746_CAPSETUP_CIN2_MSK,
		.ch_out = false,
	},
	[CIN2_DIFF] = {
		.ch_type = IIO_CAPACITANCE,
		.diferential = true,
		.indexed = true,
		.channel = 1,
		.channel2 = 3,
		.attributes = ad7746_iio_cin_attrs,
		.address = AD7746_CAPSETUP_CAPDIFF_MSK | AD7746_CAPSETUP_CIN2_MSK,
		.ch_out = false,
	}
};

static struct iio_device ad7746_iio_device = {
	.num_ch = NO_OS_ARRAY_SIZE(ad7746_channels),
	.channels = ad7746_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.read_dev = NULL,
	.debug_reg_read = (int32_t (*)())_ad7746_read_register2,
	.debug_reg_write = (int32_t (*)())_ad7746_write_register2
};

int32_t ad7746_iio_init(struct ad7746_iio_dev **iio_dev,
			struct ad7746_iio_init_param *init_param)
{
	int32_t ret;
	struct ad7746_iio_dev *desc;

	desc = (struct ad7746_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -1;

	desc->iio_dev = &ad7746_iio_device;
	desc->capdac_set = -1;

	ret = ad7746_init(&desc->ad7746_dev, init_param->ad7746_initial);
	if (ret != 0)
		goto error_desc;

	if (desc->ad7746_dev->id != ID_AD7746)
		desc->iio_dev->num_ch -= 2;

	*iio_dev = desc;

	return 0;
error_desc:
	no_os_free(desc);

	return ret;
}

int32_t ad7746_iio_remove(struct ad7746_iio_dev *desc)
{
	int32_t ret;

	ret = ad7746_remove(desc->ad7746_dev);
	if (ret != 0)
		return ret;

	no_os_free(desc);

	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "ad7799.h"
#include "iio_ad7793.h"

static int ad7793_iio_read_offset(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int ad7793_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int ad7793_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int ad7793_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int ad7793_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);

#define AD7793_ADC_CHANNEL(x)					\
        {							\
                .ch_type = IIO_VOLTAGE,				\
                .indexed = 1,					\
                .channel = x,					\
                .channel2 = x,					\
		.diferential = 1,				\
                .ch_out = 0,					\
                .attributes = ad7793_attrs			\
        }

static struct iio_attribute ad7793_attrs[] = {
	{
		.name = "raw",
		.show = ad7793_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ad7793_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ad7793_iio_read_offset,
	},
	// {
	// 	.name = "calibbias",
	// 	.show = ad7793_iio_read_calibbias,
	// 	.store = ad7793_iio_write_calibbias,
	// },
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad7793_iio_channels[] = {
	AD7793_ADC_CHANNEL(0),
	AD7793_ADC_CHANNEL(1),
	AD7793_ADC_CHANNEL(2),
};

static struct iio_device iio_ad7793 = {
	.num_ch = 3,
	.channels = ad7793_iio_channels,
};

static int ad7793_iio_read_offset(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7793_iio_desc *desc = dev;
	int32_t val;

	if (desc->ad7793_desc->polarity)
		val = 0;
	else
		val = -NO_OS_BIT(23);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int ad7793_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7793_iio_desc *desc = dev;
	struct ad7799_dev *ad7793 = desc->ad7793_desc;
	int32_t val[2];

	if (ad7793->precision == AD7799_PRECISION_MV)
		val[0] = ad7793->vref_mv;
	else
		val[0] = ad7793->vref_mv * 1000;

	val[1] = 23;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 1, val);
}

static int ad7793_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7793_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = ad7799_get_channel(desc->ad7793_desc, channel->ch_num, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

int ad7793_iio_init(struct ad7793_iio_desc **desc, struct ad7793_iio_param *param)
{
	struct ad7793_iio_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ad7799_init(&descriptor->ad7793_desc, &param->ad7793_ip);
	if (ret)
		goto error;

	descriptor->iio_dev = &iio_ad7793;
	*desc = descriptor;

	return 0;
error:
	no_os_free(descriptor);

	return ret;
}

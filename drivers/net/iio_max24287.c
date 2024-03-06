#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_max24287.h"
#include "max24287.h"
#include "iio.h"

static int32_t _max24287_read2(struct max24287_iio_desc *iiodev,
			       uint32_t reg,
			       uint32_t *readval)
{
	int ret;
	uint16_t val;

	ret = max24287_read(iiodev->dev, reg, &val);
	if (ret)
		return ret;

	*readval = val;
	return 0;
}

static int32_t _max24287_write2(struct max24287_iio_desc *iiodev,
				uint32_t reg,
				uint32_t writeval)
{
	return max24287_write(iiodev->dev, reg, (uint16_t)writeval);
}

static int max24287_iio_read_attr(void *device, char *buf,
				  uint32_t len, const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret;
	struct max24287_iio_desc *iiodev = (struct max24287_iio_desc *)device;
	struct max24287_desc *d = iiodev->dev;
	int32_t val;
	enum max24287_speed speed;
	bool link;

	switch (priv) {
	case MAX24287_IIO_ATTR_SER_LINK:
		link = max24287_link_is_up(d);
		val = link;
		break;
	case MAX24287_IIO_ATTR_SER_SPEED:
		ret = max24287_get_config_serial(d, NULL, &speed);
		if (ret)
			return ret;

		val = speed;
		break;
	case MAX24287_IIO_ATTR_PAR_SPEED:
		ret = max24287_get_config_parallel(d, NULL, &speed);
		if (ret)
			return ret;
		val = speed;
		break;
	default:
		return -EINVAL;
	};

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max24287_iio_write_attr(void *device, char *buf,
				   uint32_t len, const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int ret = 0;
	int32_t val;
	struct max24287_iio_desc *iiodev = (struct max24287_iio_desc *)device;
	struct max24287_desc *d = iiodev->dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	switch (priv) {
	case MAX24287_IIO_ATTR_PAR_SPEED:
		ret = max24287_config_parallel(d, MAX24287_RGMII, val);
		break;
	case MAX24287_IIO_ATTR_SER_LINK:
		ret = max24287_config_link(d, val);
		break;
	case MAX24287_IIO_ATTR_SER_SPEED:
		ret = max24287_config_serial(d, MAX24287_SGMII, val);
		break;
	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return len;
}

static struct iio_attribute max24287_iio_attrs[] = {
	{
		.name = "par_speed",
		.priv = MAX24287_IIO_ATTR_PAR_SPEED,
		.show = max24287_iio_read_attr,
		.store = max24287_iio_write_attr,
	},
	{
		.name = "ser_link",
		.priv = MAX24287_IIO_ATTR_SER_LINK,
		.show = max24287_iio_read_attr,
		.store = max24287_iio_write_attr,
	},
	{
		.name = "ser_speed",
		.priv = MAX24287_IIO_ATTR_SER_SPEED,
		.show = max24287_iio_read_attr,
		.store = max24287_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device max24287_iio_template = {
	.attributes = max24287_iio_attrs,
	.debug_reg_read = (int32_t (*)())_max24287_read2,
	.debug_reg_write = (int32_t (*)())_max24287_write2
};

int32_t max24287_iio_init(struct max24287_iio_desc **iiodev,
			  struct max24287_iio_init_param *init_param)
{
	int ret = -ENOMEM;
	struct max24287_iio_desc *d;
	struct iio_device *d2;

	d = (struct max24287_iio_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d2 = (struct iio_device *)no_os_calloc(1, sizeof(*d2));
	if (!d2) {
		ret = -ENOMEM;
		goto end_0;
	}

	*d2 = max24287_iio_template;

	d->iio_dev = d2;
	d->dev = init_param->dev;

	*iiodev = d;

	return 0;
end_0:
	no_os_free(d);

	return ret;
}

int32_t max24287_iio_remove(struct max24287_iio_desc *iiodev)
{
	if (iiodev)
		no_os_free(iiodev->iio_dev);

	no_os_free(iiodev);

	return 0;
}

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_dp83tg.h"
#include "dp83tg.h"
#include "iio.h"

static int32_t _dp83tg_read2(struct dp83tg_iio_desc *iiodev,
			       uint32_t reg,
			       uint32_t *readval)
{
	int ret;
	uint16_t val;

	ret = dp83tg_read(iiodev->dev, reg, &val);
	if (ret)
		return ret;

	*readval = val;
	return 0;
}

static int32_t _dp83tg_write2(struct dp83tg_iio_desc *iiodev,
				uint32_t reg,
				uint32_t writeval)
{
	return dp83tg_write(iiodev->dev, reg, (uint16_t)writeval);
}

static int dp83tg_iio_read_attr(void *device, char *buf,
				  uint32_t len, const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret;
	struct dp83tg_iio_desc *iiodev = (struct dp83tg_iio_desc *)device;
	struct dp83tg_desc *d = iiodev->dev;
	int32_t val;
	uint16_t reg;
	bool link;

	switch (priv) {
	case DP83TG_IIO_ATTR_LINK:
		link = dp83tg_link_is_up(d);
		val = link;
		break;
	case DP83TG_IIO_ATTR_MII_LINK:
		link = dp83tg_mii_link_is_up(d);
		val = link;
		break;
	case DP83TG_IIO_ATTR_MDI_LINK:
		link = dp83tg_mdi_link_is_up(d);
		val = link;
		break;
	case DP83TG_IIO_ATTR_MASTER:
		dp83tg_read(d, DP83TG_PMA_PMD_CONTROL, &reg);
		val = reg & DP83TG_CFG_MASTER_SLAVE_MASK;
		break;
	default:
		return -EINVAL;
	};

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int dp83tg_iio_write_attr(void *device, char *buf,
				   uint32_t len, const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int ret = 0;
	int32_t val;
	struct dp83tg_iio_desc *iiodev = (struct dp83tg_iio_desc *)device;
	struct dp83tg_desc *d = iiodev->dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	switch (priv) {
	case DP83TG_IIO_ATTR_LINK:
		break;
	case DP83TG_IIO_ATTR_MII_LINK:
		break;
	case DP83TG_IIO_ATTR_MDI_LINK:
		break;
	case DP83TG_IIO_ATTR_MASTER:
		break;
	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return len;
}

static struct iio_attribute dp83tg_iio_attrs[] = {
	{
		.name = "link",
		.priv = DP83TG_IIO_ATTR_LINK,
		.show = dp83tg_iio_read_attr,
		.store = dp83tg_iio_write_attr,
	},
	{
		.name = "mii_link",
		.priv = DP83TG_IIO_ATTR_MII_LINK,
		.show = dp83tg_iio_read_attr,
		.store = dp83tg_iio_write_attr,
	},
	{
		.name = "mdi_link",
		.priv = DP83TG_IIO_ATTR_MDI_LINK,
		.show = dp83tg_iio_read_attr,
		.store = dp83tg_iio_write_attr,
	},
	{
		.name = "master",
		.priv = DP83TG_IIO_ATTR_MASTER,
		.show = dp83tg_iio_read_attr,
		.store = dp83tg_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device dp83tg_iio_template = {
	.attributes = dp83tg_iio_attrs,
	.debug_reg_read = (int32_t (*)())_dp83tg_read2,
	.debug_reg_write = (int32_t (*)())_dp83tg_write2
};

int32_t dp83tg_iio_init(struct dp83tg_iio_desc **iiodev,
			  struct dp83tg_iio_init_param *init_param)
{
	int ret = -ENOMEM;
	struct dp83tg_iio_desc *d;
	struct iio_device *d2;

	d = (struct dp83tg_iio_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d2 = (struct iio_device *)no_os_calloc(1, sizeof(*d2));
	if (!d2) {
		ret = -ENOMEM;
		goto end_0;
	}

	*d2 = dp83tg_iio_template;

	d->iio_dev = d2;
	d->dev = init_param->dev;

	*iiodev = d;

	return 0;
end_0:
	no_os_free(d);

	return ret;
}

int32_t dp83tg_iio_remove(struct dp83tg_iio_desc *iiodev)
{
	if (iiodev)
		no_os_free(iiodev->iio_dev);

	no_os_free(iiodev);

	return 0;
}

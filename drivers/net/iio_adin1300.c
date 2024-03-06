#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_adin1300.h"
#include "adin1300.h"
#include "iio.h"

static int32_t _adin1300_read2(struct adin1300_iio_desc *iiodev,
			       uint32_t reg,
			       uint32_t *readval)
{
	int ret;
	uint16_t val;

	if (reg > 0x1f)
		return -EINVAL;

	ret = adin1300_read(iiodev->dev, reg, &val);
	if (ret)
		return ret;

	*readval = val;
	return 0;
}

static int32_t _adin1300_write2(struct adin1300_iio_desc *iiodev,
				uint32_t reg,
				uint32_t writeval)
{
	if (reg > 0x1f)
		return -EINVAL;

	return adin1300_write(iiodev->dev, reg, (uint16_t)writeval);
}

static int adin1300_iio_read_attr(void *device, char *buf,
				  uint32_t len, const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret;
	struct adin1300_iio_desc *iiodev = (struct adin1300_iio_desc *)device;
	struct adin1300_desc *d = iiodev->dev;
	int32_t val;
	enum adin1300_speed speed;
	bool link;
	uint16_t mii_control;

	switch (priv) {
	case ADIN1300_IIO_ATTR_LINK:
		link = adin1300_link_is_up(d);
		val = link;
		break;
	case ADIN1300_IIO_ATTR_SPEED:
		speed = adin1300_resolved_speed(d);
		val = speed;
		break;
	case ADIN1300_IIO_ATTR_AUTONEGOTIATE:
		ret = adin1300_read(d, ADIN1300_MII_CONTROL, &mii_control);
		if (ret)
			return ret;

		val = no_os_field_get(ADIN1300_AUTONEG_EN_MASK, mii_control);
		break;
	default:
		return -EINVAL;
	};

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int adin1300_iio_write_attr(void *device, char *buf,
				   uint32_t len, const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int ret = 0;
	int32_t val;
	struct adin1300_iio_desc *iiodev = (struct adin1300_iio_desc *)device;
	struct adin1300_desc *d = iiodev->dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	switch (priv) {
	case ADIN1300_IIO_ATTR_LINK:
		ret = adin1300_write_bits(d, ADIN1300_PHY_CTRL_3, 0, ADIN1300_LINK_EN_MASK);
		if (ret)
			return ret;

		if (val)
			ret = adin1300_write_bits(d, ADIN1300_PHY_CTRL_3, ADIN1300_LINK_EN_MASK,
						  ADIN1300_LINK_EN_MASK);
		break;
	case ADIN1300_IIO_ATTR_SPEED:
		if (val > ADIN1300_1000_BASE_T_FULLDUPLEX)
			return -EINVAL;

		ret = adin1300_config_speed(d, val);
		break;
	case ADIN1300_IIO_ATTR_AUTONEGOTIATE:
		ret = adin1300_autoneg(d, (bool)val);
		break;
	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return len;
}

static struct iio_attribute adin1300_iio_attrs[] = {
	{
		.name = "link",
		.priv = ADIN1300_IIO_ATTR_LINK,
		.show = adin1300_iio_read_attr,
		.store = adin1300_iio_write_attr,
	},
	{
		.name = "speed",
		.priv = ADIN1300_IIO_ATTR_SPEED,
		.show = adin1300_iio_read_attr,
		.store = adin1300_iio_write_attr,
	},
	{
		.name = "autonegotiate",
		.priv = ADIN1300_IIO_ATTR_AUTONEGOTIATE,
		.show = adin1300_iio_read_attr,
		.store = adin1300_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device adin1300_iio_descice_template = {
	.attributes = adin1300_iio_attrs,
	.debug_reg_read = (int32_t (*)())_adin1300_read2,
	.debug_reg_write = (int32_t (*)())_adin1300_write2
};

int32_t adin1300_iio_init(struct adin1300_iio_desc **iiodev,
			  struct adin1300_iio_init_param *init_param)
{
	int ret;
	struct adin1300_iio_desc *d;
	struct iio_device *d2;

	d = (struct adin1300_iio_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d2 = (struct iio_device *)no_os_calloc(1, sizeof(*d2));
	if (!d2) {
		ret = -ENOMEM;
		goto end_0;
	}

	*d2 = adin1300_iio_descice_template;

	d->iio_dev = d2;
	d->dev = init_param->dev;

	*iiodev = d;

	return 0;
end_0:
	no_os_free(d);

	return ret;
}

int32_t adin1300_iio_remove(struct adin1300_iio_desc *iiodev)
{
	if (iiodev)
		no_os_free(iiodev->iio_dev);

	no_os_free(iiodev);

	return 0;
}

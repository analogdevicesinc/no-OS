#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_adin1110.h"
#include "adin1110.h"
#include "iio.h"

enum adin1110_phy_access_type {
        C22_REG_ACCESS,
        C22_VAL_ACCESS,
        C45_REG_ACCESS,
        C45_VAL_ACCESS,
};

static int32_t _adin1110_read2(struct adin1110_iio_desc *iiodev,
			       uint32_t reg,
			       uint32_t *readval)
{
	int ret;

	ret = adin1110_reg_read(iiodev->dev, reg, readval);
	if (ret)
		return ret;

	return 0;
}

static int32_t _adin1110_write2(struct adin1110_iio_desc *iiodev,
				uint32_t reg,
				uint32_t writeval)
{
	return adin1110_reg_write(iiodev->dev, reg, (uint16_t)writeval);
}

static int adin1110_iio_read_attr(void *device, char *buf,
				  uint32_t len, const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret;
	struct adin1110_iio_desc *iiodev = (struct adin1110_iio_desc *)device;
	struct adin1110_desc *d = iiodev->dev;
	uint16_t mdio_val;
	int32_t val;
        uint32_t c45_dev_id;

        switch (priv){
        case C22_REG_ACCESS:
		mdio_val = iiodev->c22_addr;
		break;
        case C22_VAL_ACCESS:
                ret = adin1110_mdio_read(d, 0x1, iiodev->c22_addr, &mdio_val);
                if (ret)
                        return ret;
                break;
	case C45_REG_ACCESS:
		mdio_val = iiodev->c45_addr;
		break;
        case C45_VAL_ACCESS:
                c45_dev_id = no_os_field_get(NO_OS_GENMASK(31, 16), iiodev->c45_addr);
                ret = adin1110_mdio_read_c45(d, 0x1, c45_dev_id, iiodev->c45_addr & 0xFFFF, &mdio_val);
                if (ret)
                        return ret;
 
                break;
        default:
                return -EINVAL;
        }

	val = mdio_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int adin1110_iio_write_attr(void *device, char *buf,
				   uint32_t len, const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int ret = 0;
	int32_t val;
	struct adin1110_iio_desc *iiodev = (struct adin1110_iio_desc *)device;
	struct adin1110_desc *d = iiodev->dev;
        uint32_t c45_dev_id;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

        switch (priv){
        case C22_REG_ACCESS:
                iiodev->c22_addr = val;
                break;
        case C22_VAL_ACCESS:
                ret = adin1110_mdio_write(d, 0x1, iiodev->c22_addr, val);
                if (ret)
                        return ret;
                break;
        case C45_REG_ACCESS:
                iiodev->c45_addr = val;
                break;
        case C45_VAL_ACCESS:
                c45_dev_id = no_os_field_get(NO_OS_GENMASK(31, 16), iiodev->c45_addr);
                ret = adin1110_mdio_write_c45(d, 0x1, c45_dev_id, iiodev->c45_addr & 0xFFFF, val);
                if (ret)
                        return ret;
 
                break;
        default:
                return -EINVAL;
        }

	return len;
}

static struct iio_attribute adin1110_iio_attrs[] = {
	{
		.name = "c22_mdio_addr",
		.priv = C22_REG_ACCESS,
		.show = adin1110_iio_read_attr,
		.store = adin1110_iio_write_attr,
	},
        {
		.name = "c22_mdio_val",
		.priv = C22_VAL_ACCESS,
		.show = adin1110_iio_read_attr,
		.store = adin1110_iio_write_attr,
	},
        {
		.name = "c45_mdio_addr",
		.priv = C45_REG_ACCESS,
		.show = adin1110_iio_read_attr,
		.store = adin1110_iio_write_attr,
	},
        {
		.name = "c45_mdio_val",
		.priv = C45_VAL_ACCESS,
		.show = adin1110_iio_read_attr,
		.store = adin1110_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device adin1110_iio_descice_template = {
	.attributes = adin1110_iio_attrs,
	.debug_reg_read = (int32_t (*)())_adin1110_read2,
	.debug_reg_write = (int32_t (*)())_adin1110_write2
};

int32_t adin1110_iio_init(struct adin1110_iio_desc **iiodev,
			  struct adin1110_iio_init_param *init_param)
{
	int ret;
	struct adin1110_iio_desc *d;
	struct iio_device *d2;

	d = (struct adin1110_iio_desc *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d2 = (struct iio_device *)no_os_calloc(1, sizeof(*d2));
	if (!d2) {
		ret = -ENOMEM;
		goto end_0;
	}

	*d2 = adin1110_iio_descice_template;

	d->iio_dev = d2;
	d->dev = init_param->dev;

	*iiodev = d;

	return 0;
end_0:
	no_os_free(d);

	return ret;
}

int32_t adin1110_iio_remove(struct adin1110_iio_desc *iiodev)
{
	if (iiodev)
		no_os_free(iiodev->iio_dev);

	no_os_free(iiodev);

	return 0;
}

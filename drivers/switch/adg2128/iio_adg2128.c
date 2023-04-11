#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "iio.h"
#include "iio_adg2128.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_adg2128.h"

static int32_t _adg2128_read_register2(struct adg2128_iio_dev *dev,
				       uint32_t reg,
				       uint32_t *readval)
{
	int ret;
	uint8_t vals[2];
	dev->i2c_desc->slave_address = reg;
	ret = no_os_i2c_read(dev->i2c_desc, vals, 2, 1);
	if (ret < 0)
		return ret;

	*readval = (vals[0] << 8) | vals[1];
	return 0;
}

static int32_t _adg2128_write_register2(struct adg2128_iio_dev *dev,
					uint32_t reg,
					uint32_t writeval)
{
	uint8_t vals[2];
	vals[0] = writeval >> 8;
	vals[1] = writeval;
	dev->i2c_desc->slave_address = reg;
	return no_os_i2c_write(dev->i2c_desc, vals, 2, 1);
}

static struct iio_device adg2128_iio_device = {
	.num_ch = 0,
	.channels = NULL,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.read_dev = NULL,
	.debug_reg_read = (int32_t (*)())_adg2128_read_register2,
	.debug_reg_write = (int32_t (*)())_adg2128_write_register2
};

int32_t adg2128_iio_init(struct adg2128_iio_dev **iio_dev,
			 struct no_os_i2c_desc *i2c_desc)
{
	struct adg2128_iio_dev *desc = (struct adg2128_iio_dev *)no_os_calloc(1,
				       sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adg2128_iio_device;
	desc->i2c_desc = i2c_desc;

	*iio_dev = desc;

	return 0;
}

int32_t adg2128_iio_remove(struct adg2128_iio_dev *desc)
{
	no_os_free(desc);

	return 0;
}

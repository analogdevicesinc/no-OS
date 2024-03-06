#ifndef IIO_MAX24287_H
#define IIO_MAX24287_H

#include "iio.h"
#include "max24287.h"

struct max24287_iio_desc {
	struct max24287_desc *dev;
	struct iio_device *iio_dev;
};

struct max24287_iio_init_param {
	struct max24287_desc *dev;
};

enum max24287_iio_attr_id {
	MAX24287_IIO_ATTR_PAR_SPEED,
	MAX24287_IIO_ATTR_SER_LINK,
	MAX24287_IIO_ATTR_SER_SPEED,
};

int32_t max24287_iio_init(struct max24287_iio_desc **iio_dev,
			  struct max24287_iio_init_param *init_param);
int32_t max24287_iio_remove(struct max24287_iio_desc *desc);

#endif /** IIO_MAX24287_H */
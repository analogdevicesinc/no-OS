#ifndef IIO_ADIN1300_H
#define IIO_ADIN1300_H

#include "iio.h"
#include "adin1300.h"

struct adin1300_iio_desc {
	struct adin1300_desc *dev;
	struct iio_device *iio_dev;
};

struct adin1300_iio_init_param {
	struct adin1300_desc *dev;
};

enum adin1300_iio_attr_id {
	ADIN1300_IIO_ATTR_LINK,
	ADIN1300_IIO_ATTR_SPEED,
	ADIN1300_IIO_ATTR_AUTONEGOTIATE,
};

int32_t adin1300_iio_init(struct adin1300_iio_desc **iio_dev,
			  struct adin1300_iio_init_param *init_param);
int32_t adin1300_iio_remove(struct adin1300_iio_desc *desc);

#endif /** IIO_ADIN1300_H */
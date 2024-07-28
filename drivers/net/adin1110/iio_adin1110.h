#ifndef IIO_ADIN1110_H
#define IIO_ADIN1110_H

#include "iio.h"
#include "adin1110.h"

struct adin1110_iio_desc {
	struct adin1110_desc *dev;
	struct iio_device *iio_dev;

        uint32_t c22_addr;
        uint32_t c45_addr;
};

struct adin1110_iio_init_param {
	struct adin1110_desc *dev;
};

int32_t adin1110_iio_init(struct adin1110_iio_desc **iio_dev,
			  struct adin1110_iio_init_param *init_param);
int32_t adin1110_iio_remove(struct adin1110_iio_desc *desc);

#endif /** IIO_ADIN1300_H */
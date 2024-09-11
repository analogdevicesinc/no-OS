#ifndef IIO_DP83TG_H
#define IIO_DP83TG_H

#include "iio.h"
#include "dp83tg.h"

struct dp83tg_iio_desc {
	struct dp83tg_desc *dev;
	struct iio_device *iio_dev;
};

struct dp83tg_iio_init_param {
	struct dp83tg_desc *dev;
};

enum dp83tg_iio_attr_id {
	DP83TG_IIO_ATTR_LINK,
    DP83TG_IIO_ATTR_MII_LINK,
    DP83TG_IIO_ATTR_MDI_LINK,
};

int32_t dp83tg_iio_init(struct dp83tg_iio_desc **iio_dev,
			  struct dp83tg_iio_init_param *init_param);
int32_t dp83tg_iio_remove(struct dp83tg_iio_desc *desc);

#endif /** IIO_DP83TG_H */
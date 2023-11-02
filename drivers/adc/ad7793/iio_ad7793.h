#include "iio.h"
#include "iio_trigger.h"

#include "ad7799.h"

struct ad7793_iio_param {
	struct ad7799_init_param ad7793_ip;
};

struct ad7793_iio_desc { 
	struct ad7799_dev *ad7793_desc;
	struct iio_device *iio_dev;
};

int ad7793_iio_init(struct ad7793_iio_desc **, struct ad7793_iio_param *);

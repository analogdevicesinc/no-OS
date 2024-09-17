#include <stdbool.h>
#include "iio_dp83tg.h"
#include "mwc.h"

extern volatile bool link;
int net_init(struct dp83tg_iio_desc **dp83tg_iio, bool master);
#include "iio.h"

ssize_t iio_read__configuration(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = device;
	uint32_t		    value;

	value = ad7124_read_reg(desc, CONFIGURATION, &value);

	return snprintf(buf, len, "%"PRIu32"", value);
}

ssize_t iio_change_configuration(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel) 
{
	struct ad7124_dev *desc = device;

    ad7124_write_reg(desc, CONFIGURATION, &srt_to_uint32(buf));

	return len;
}
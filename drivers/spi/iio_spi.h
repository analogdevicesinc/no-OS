#ifdef IIO_SUPPORT
#ifndef IIO_SPI_H
#define IIO_SPI_H

#include "spi.h"
#include "iio_types.h"

ssize_t iio_spi_write(struct spi_desc *dev, char *buf, size_t len,
			const struct iio_ch_info *channel, intptr_t priv);

ssize_t iio_spi_read_last(struct spi_desc *dev, char *buf, size_t len,
			const struct iio_ch_info *channel, intptr_t priv);

static struct iio_attribute spi_attrs[] =
{
	{
		.name = "spi_write_and_read",
		.priv = 0,
		.show = (ssize_t (*)())iio_spi_read_last,
		.store = (ssize_t (*)())iio_spi_write
	},
	END_ATTRIBUTES_ARRAY
};

extern struct iio_device spi_iio_descriptor;

#endif
#endif

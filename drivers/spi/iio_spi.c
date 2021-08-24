#ifdef IIO_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iio_spi.h"

static uint8_t g_buff[1000];
static uint32_t g_len;

ssize_t iio_spi_write(struct spi_desc *dev, char *buf, size_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	char *nb;
	int val;

	/* get the first token */
	nb = strtok(buf, " ");

	g_len = 0;
	/* walk through other tokens */
	while( nb != NULL ) {

		if (1 == sscanf(nb, "0x%x", &val))
			g_buff[g_len++] = val;
		else if (1 == sscanf(nb, "%d", &val))
			g_buff[g_len++] = val;
		else
			break;
		nb = strtok(NULL, " ");
	}

	spi_write_and_read(dev, g_buff, g_len);

	return len;
}

ssize_t iio_spi_read_last(struct spi_desc *dev, char *buf, size_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t i;
	uint32_t k;

	i = 0;
	k = 0;
	while (k < g_len) {
		i += sprintf(buf + i, "0x%X ", g_buff[k++]);
	}

	return i;
}

/** IIO Descriptor */
struct iio_device spi_iio_descriptor = {
	.attributes = spi_attrs,
};


#endif //IIO_SUPPORT

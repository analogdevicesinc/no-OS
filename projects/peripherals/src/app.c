
#include <stdio.h>
#include <stdint.h>
#include "spi.h"
#include "linux_spi.h"

int main(void)
{
	struct	spi_init_param	spi_param;
	struct	spi_desc 	*spi;
	uint8_t	data[2];

	printf("Hello\n");

	spi_param.device_id = 0,
	spi_param.mode = SPI_MODE_0;
	spi_param.chip_select = 0;
	spi_param.platform_ops = &linux_spi_ops;

	spi_init(&spi, &spi_param);

	spi_write_and_read(spi, data, 2);

	printf("Bye\n");

	return 0;
}


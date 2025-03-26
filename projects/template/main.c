#include <stdio.h>

#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_i2c.h"

int main()
{
	struct no_os_spi_init_param spi_ip = {
		.device_id = 0,
		.platform_ops = NULL,
	};
	struct no_os_spi_desc *spi;
	int ret;

	printf("Template\n");
	
	ret = no_os_spi_init(&spi, &spi_ip);
	if (ret)
		return ret;

	return 0;
}
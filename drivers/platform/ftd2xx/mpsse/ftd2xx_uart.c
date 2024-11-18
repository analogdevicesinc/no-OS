
#include <stdint.h>
#include <stdio.h>
#include "no_os_alloc.h"
#include "ftd2xx_uart.h"

static int ftd2xx_uart_init(struct no_os_uart_desc **desc,
			    struct no_os_uart_init_param *param)
{
        *desc = no_os_calloc(1, sizeof(struct no_os_uart_desc));
	return 0;
};

static int ftd2xx_uart_remove(struct no_os_uart_desc *desc)
{
	return 0;
};

static int ftd2xx_uart_write(struct no_os_uart_desc *desc,
			     const uint8_t *data,
			     uint32_t bytes_number)
{
	uint32_t count = 0;

	while (count < bytes_number) {
		printf("%d", data[count]);
	}

	return 0;
};

const struct no_os_uart_platform_ops ftd2xx_uart_ops = {
	.init = &ftd2xx_uart_init,
	.write = &ftd2xx_uart_write,
	.remove = &ftd2xx_uart_remove
};

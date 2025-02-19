
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_uart.h"

#include <stdio.h>

int main()
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret < 0)
		return ret;
	
	no_os_uart_stdio(uart_desc);

	while(1) {
		ret = printf("Hello World!\n\r");
		if (ret < 0)
			goto error;
		no_os_mdelay(1000);
	}

	return 0;

error:
	no_os_uart_remove(uart_desc);
}

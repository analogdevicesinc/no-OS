
#include <stdio.h>
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

int main(void)
{
	struct max_uart_init_param max_uart_extra_ip = {
		.flow = MAX_UART_FLOW_DIS
	};
	struct no_os_uart_init_param uart_ip = {
		.device_id = 0,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &max_uart_extra_ip,
	};

	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret < 0)
		return ret;

	ret = no_os_uart_write(uart_desc, "no_os_uart_write: Hello\n\r", 26);
	if (ret < 0)
		goto error;

	no_os_uart_stdio(uart_desc);

	printf("printf: Hello\n\r");

	printf("printf: Bye\n\r");

	while(1);

error:
	no_os_uart_remove(uart_desc);

	return ret;
}

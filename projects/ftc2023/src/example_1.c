
#include <stdio.h>
#include "no_os_init.h"
#include "no_os_uart.h"
#include "maxim_uart.h"

int main(void)
{
	struct max_uart_init_param uart_init_max = {
		.flow = UART_FLOW_DIS
	};
	struct no_os_uart_init_param uart_init = {
		.device_id = 0,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &uart_init_max,
	};
	struct no_os_uart_desc *uart;
	int ret;

	ret = no_os_init();
	if (ret < 0)
		return ret;

	ret = no_os_uart_init(&uart, &uart_init);
	if (ret < 0)
		return ret;

	no_os_uart_stdio(uart);

	printf("Hello\n");

	return 0;
}

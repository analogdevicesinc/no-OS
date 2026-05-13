#include "common_data.h"
#include "no_os_error.h"
#include "no_os_uart.h"
#include "maxim_uart_stdio.h"

extern int example_main();

int main()
{
	int ret;
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &adxl372_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	return example_main();
}

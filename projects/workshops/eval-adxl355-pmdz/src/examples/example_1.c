
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "common_data.h"

int main()
{
	
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret < 0)
		return ret;

	no_os_uart_stdio(uart_desc);
	while(1) {
		pr_info("Hello World\n\r");
		no_os_mdelay(1000);
	}

	return 0;

error:
	no_os_uart_remove(uart_desc);
}

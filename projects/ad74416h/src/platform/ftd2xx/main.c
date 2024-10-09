
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{

	int ret = -EINVAL;
	ad74416h_ip.spi_ip = ad74416h_spi_ip;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ad74416h_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = basic_example_main();
	if (ret) {
		no_os_uart_remove(uart_desc);
		return ret;
	}

#endif

#if (BASIC_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#endif

	return 0;
}

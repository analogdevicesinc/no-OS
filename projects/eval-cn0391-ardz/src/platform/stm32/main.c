#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#elif defined(IIO_EXAMPLE)
#include "iio_example.h"
#elif defined(IIO_LWIP_EXAMPLE)
#include "iio_lwip_example.h"
#endif

int main(void)
{
	int ret = -EINVAL;

	stm32_init();

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &cn0391_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	ret = basic_example_main();
#elif defined(IIO_EXAMPLE)
	ret = iio_example_main();
#elif defined(IIO_LWIP_EXAMPLE)
	ret = iio_lwip_example_main();
#else
#error At least one example has to be selected using EXAMPLE= in make command.
#endif

	return ret;
}

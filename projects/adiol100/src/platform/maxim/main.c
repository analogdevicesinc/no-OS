#include "parameters.h"
#include "common_data.h"
#include "maxim_uart_stdio.h"
#include "no_os_uart.h"

#ifdef CONFIG_ADIOL100_BASIC_EXAMPLE
extern int basic_example_main(void);
#endif

#ifdef CONFIG_ADIOL100_ILINK_EXAMPLE
extern int ilink_example_main(void);
#endif

int main(void)
{
    struct no_os_uart_desc *uart;
    no_os_uart_init(&uart, &adiol100_uart_ip);
    no_os_uart_stdio(uart);

#ifdef CONFIG_ADIOL100_BASIC_EXAMPLE
    return basic_example_main();
#endif

#ifdef CONFIG_ADIOL100_ILINK_EXAMPLE
    return ilink_example_main();
#endif
}

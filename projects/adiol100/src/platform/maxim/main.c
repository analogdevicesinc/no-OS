#include "parameters.h"
#include "common_data.h"
#include "maxim_uart_stdio.h"
#include "no_os_uart.h"

extern int basic_example_main(void);

int main(void)
{
    struct no_os_uart_desc *uart;
    no_os_uart_init(&uart, &adiol100_uart_ip);
    no_os_uart_stdio(uart);
    return basic_example_main();
}

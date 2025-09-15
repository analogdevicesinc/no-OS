#include <stdio.h>
#include <stdlib.h>

#include "no_os_uart.h"
#include "no_os_delay.h"
#include "maxim_usb_uart.h"
#include "maxim_uart_stdio.h"

static struct no_os_uart_init_param uart_ip = {
	.device_id = 0,
	.irq_id = 0x2,
	.asynchronous_rx = true,
	.baud_rate = 115200,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_usb_uart_ops,
	.extra = &(struct max_usb_uart_init_param){
                .vid = 0x0B6B,
                .pid = 0x003C
        },
};

int main()
{
        struct no_os_uart_desc *uart;
        uint32_t ret;

        ret = no_os_uart_init(&uart, &uart_ip);
        if (ret)
                return ret;

        no_os_uart_stdio(uart);

        while(1){
                printf("H!\n");
                // no_os_uart_write(uart, "H!\n", 3);
                no_os_mdelay(1000);
        }

        return 0;
}
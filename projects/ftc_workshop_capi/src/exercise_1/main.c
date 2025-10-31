#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

#include "platform.h"

int main()
{
	struct no_os_uart_desc *uart;
	struct no_os_uart_init_param uart_param = {
		.device_id = 0,
		.baud_rate = 115200,
		.platform_ops = &max_uart_ops,
		.size = NO_OS_UART_CS_8,
		.extra = &(struct max_uart_init_param){
			.flow = MAX_UART_FLOW_DIS,
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		}
	};
	struct no_os_gpio_init_param button1_param = {
		.port = 0,
		.number = 2,
		.pull = NO_OS_PULL_UP,
		.platform_ops = &max_gpio_ops,
		.extra = &(struct max_gpio_init_param){
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		}
	};
	struct no_os_gpio_init_param button2_param = {
		.port = 0,
		.number = 3,
		.pull = NO_OS_PULL_UP,
		.platform_ops = &max_gpio_ops,
		.extra = &(struct max_gpio_init_param){
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		}
	};
	struct no_os_gpio_desc *button1;
	struct no_os_gpio_desc *button2;
	uint8_t button1_val;
	uint8_t button2_val;
	int ret;
	
	ret = no_os_uart_init(&uart, &uart_param);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	ret = no_os_gpio_get(&button1, &button1_param);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&button2, &button2_param);
	if (ret)
		return ret;

	while (1){
		ret = no_os_gpio_get_value(button1, &button1_val);
		if (ret)
			printf("Error reading the state of button 1 (%d)\n", ret);
	
		ret = no_os_gpio_get_value(button2, &button2_val);
		if (ret)
			printf("Error reading the state of button 2 (%d)\n", ret);

		printf("Button state: %d %d\n", button1_val, button2_val);

		no_os_mdelay(1000);
	}

	return 0;
}
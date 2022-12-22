#include "parameters.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_gpio_init_param xgpio = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};


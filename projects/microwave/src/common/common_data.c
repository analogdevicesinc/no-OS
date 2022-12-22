#include "common_data.h"

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_gpio_init_param reset_gpio_ip = {
	.port = 1,
	.number = 27,
	.platform_ops = &max_gpio_ops,
};

struct no_os_gpio_init_param en_gpio_ip = {
	.port = 1,
	.number = 23,
	.platform_ops = &max_gpio_ops,
};

struct no_os_gpio_init_param clk_gpio_ip = {
	.port = 1,
	.number = 26,
	.platform_ops = &max_gpio_ops,
};

struct no_os_gpio_init_param data_gpio_ip = {
	.port = 1,
	.number = 29,
	.platform_ops = &max_gpio_ops,
};

struct no_os_gpio_init_param scanout_tx_gpio_ip = {
	.port = 1,
	.number = 28,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
};

struct no_os_gpio_init_param scanout_rx_gpio_ip = {
	.port = 1,
	.number = 24,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
};
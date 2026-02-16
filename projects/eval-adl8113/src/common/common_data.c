#include "common_data.h"

struct max_gpio_init_param adl8113_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct no_os_gpio_init_param led_red_ip = {
	.port = 0,
	.number = 18,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &adl8113_gpio_extra_ip
};

struct no_os_gpio_init_param led_green_ip = {
	.port = 0,
	.number = 19,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &adl8113_gpio_extra_ip
};

struct no_os_gpio_init_param led_blue_ip = {
	.port = 0,
	.number = 26,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &adl8113_gpio_extra_ip
};

struct no_os_gpio_init_param button1_ip = {
	.port = 0,
	.number = 2,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &adl8113_gpio_extra_ip
};

struct adl8113_init_param adl8113_param = {
	.gpio_va = {
		.port = 1,
		.number = 6,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &adl8113_gpio_extra_ip
	},
	.gpio_vb = {
		.port = 1,
		.number = 7,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &adl8113_gpio_extra_ip
	},
	.initial_mode = ADL8113_INTERNAL_AMPLIFIER
};

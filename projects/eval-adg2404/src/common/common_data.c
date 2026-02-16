#include "common_data.h"

struct max_gpio_init_param adg2404_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct adg2404_init_param adg2404_ip = {
	.gpio_a0 = {
		.port = 1,
		.number = 6,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &adg2404_gpio_extra_ip
	},
	.gpio_a1 = {
		.port = 1,
		.number = 7,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &adg2404_gpio_extra_ip
	},
	.gpio_en = {
		.port = 1,
		.number = 8,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &adg2404_gpio_extra_ip
	}
};

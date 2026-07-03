/*****************************************************************************
 * common_data.c
 * Defines common data to be used by aducm_blinky_example examples.
 *****************************************************************************/

#include "common_data.h"

struct no_os_gpio_init_param blinky_gpio_blue_ip = {
	.number = GPIO_BLUE_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param blinky_gpio_green_ip = {
	.number = GPIO_GREEN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

/*****************************************************************************
 * parameters.h
 * Definitions specific to ADuCM3029 platform used by aducm_blinky_example.
 *****************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <stdint.h>
#include <stddef.h>
#include "platform_init.h"
#include "aducm3029_gpio.h"

#define GPIO_BLUE_NUM	31
#define GPIO_GREEN_NUM	32
#define GPIO_OPS	&aducm_gpio_ops
#define GPIO_EXTRA	NULL

#endif /* __PARAMETERS_H__ */

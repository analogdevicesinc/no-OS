/*
 * GPIO user space helpers
 *
 * Copyright 2009-2013 Analog Devices Inc.
 * Michael Hennerich (michael.hennerich@analog.com)
 *
 * Licensed under the GPL-2 or later
 */

#ifndef __LINUX_GPIO_H__
#define __LINUX_GPIO_H__

#define GPIO_DIR_IN	0
#define GPIO_DIR_OUT	1

int gpio_export(unsigned gpio);
int gpio_unexport(unsigned gpio);
int gpio_dir(unsigned gpio, unsigned dir);
int gpio_dir_out(unsigned gpio);
int gpio_dir_in(unsigned gpio);
int gpio_value(unsigned gpio, unsigned value);

#endif /* __LINUX_GPIO_H__ */

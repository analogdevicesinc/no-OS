#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "adl8113.h"
#include "maxim_gpio.h"
#include "no_os_gpio.h"

extern struct max_gpio_init_param adl8113_gpio_extra_ip;
extern struct no_os_gpio_init_param led_red_ip;
extern struct no_os_gpio_init_param led_green_ip;
extern struct no_os_gpio_init_param led_blue_ip;
extern struct no_os_gpio_init_param button1_ip;
extern struct adl8113_init_param adl8113_param;

#endif /* __COMMON_DATA_H__ */

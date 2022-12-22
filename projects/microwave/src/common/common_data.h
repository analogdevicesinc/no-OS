#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "no_os_gpio.h"
#include "no_os_uart.h"

extern struct no_os_uart_init_param uart_ip;
extern struct no_os_gpio_init_param reset_gpio_ip;
extern struct no_os_gpio_init_param en_gpio_ip;
extern struct no_os_gpio_init_param clk_gpio_ip;
extern struct no_os_gpio_init_param data_gpio_ip;
extern struct no_os_gpio_init_param scanout_tx_gpio_ip;
extern struct no_os_gpio_init_param scanout_rx_gpio_ip;

#endif /* __COMMON_DATA_H__ */

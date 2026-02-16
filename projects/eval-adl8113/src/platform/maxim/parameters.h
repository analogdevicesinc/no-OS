#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_uart.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &adl8113_uart_extra_ip

#define GPIO_OPS        &max_gpio_ops
#define GPIO_EXTRA      &adl8113_gpio_extra_ip

extern struct max_uart_init_param adl8113_uart_extra_ip;
extern struct max_gpio_init_param adl8113_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */

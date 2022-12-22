#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_stdio.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &uart_extra_ip

extern struct max_uart_init_param uart_extra_ip;
extern struct max_gpio_init_param xgpio;

#endif /* __PARAMETERS_H__ */

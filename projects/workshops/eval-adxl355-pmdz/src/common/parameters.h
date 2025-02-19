#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"
#include "maxim_spi.h"

#define UART_IRQ_ID     UART0_IRQn
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600

#define INTC_DEVICE_ID  0

#define UART_OPS        &max_uart_ops

#endif /* __PARAMETERS_H__ */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &adiol100_uart_extra

#define SPI_DEVICE_ID   1
#define SPI_CS          1
#define SPI_BAUDRATE    1000000
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adiol100_spi_extra

#define GPIO_OPS        &max_gpio_ops
#define GPIO_EXTRA      NULL
#define GPIO_IRQ_OPS    &max_gpio_irq_ops

extern struct max_uart_init_param adiol100_uart_extra;
extern struct max_spi_init_param  adiol100_spi_extra;

#endif

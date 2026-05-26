#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_gpio.h"

#if (TARGET_NUM == 78000)
#define SPI_DEVICE_ID   1
#define SPI_CS          1
#elif (TARGET_NUM == 32650)
#define SPI_DEVICE_ID   1
#define SPI_CS          0
#elif (TARGET_NUM == 32690)
#define SPI_DEVICE_ID   0
#define SPI_CS          1
#else
#define SPI_DEVICE_ID   0
#define SPI_CS          0
#endif

#if (TARGET_NUM == 32690)
#define UART_IRQ_ID     UART2_IRQn
#define UART_DEVICE_ID  2
#else
#define UART_IRQ_ID     UART0_IRQn
#define UART_DEVICE_ID  0
#endif
#define INTC_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adxl37x_uart_extra
#define UART_OPS        &max_uart_ops

extern struct max_uart_init_param adxl37x_uart_extra;

#define SPI_BAUDRATE    100000
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adxl37x_spi_extra

extern struct max_spi_init_param adxl37x_spi_extra;

#define GPIO_OPS        &max_gpio_ops
#define GPIO_EXTRA      &adxl37x_gpio_extra

#define GPIO_INT1_PORT  1
#define GPIO_INT1_PIN   6
#define GPIO_INT2_PORT  1
#define GPIO_INT2_PIN   7

extern struct max_gpio_init_param adxl37x_gpio_extra;

#endif /* __PARAMETERS_H__ */

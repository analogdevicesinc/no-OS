
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "ftd2xx_spi.h"
#include "ftd2xx_uart.h"

#define UART_IRQ_ID     0
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   0
#define UART_EXTRA      NULL
#define UART_OPS        &ftd2xx_uart_ops

extern struct ftd2xx_spi_init ad74416h_spi_extra_ip;

#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    10000000
#define SPI_CS          0
#define SPI_OPS         &ftd2xx_spi_ops
#define SPI_EXTRA       &ad74416h_spi_extra_ip

#if 0
extern UART_HandleTypeDef huart5;

#define UART_IRQ_ID     UART5_IRQn

#define UART_DEVICE_ID  5
#define UART_BAUDRATE   115200
#define UART_EXTRA      &ad74416h_uart_extra_ip
#define UART_OPS        &stm32_uart_ops

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    10000000
#define SPI_CS          15
#define SPI_CS_PORT     0
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &ad74416h_spi_extra_ip

extern struct stm32_uart_init_param ad74416h_uart_extra_ip;
extern struct stm32_spi_init_param ad74416h_spi_extra_ip;
#endif

#endif /* __PARAMETERS_H__ */

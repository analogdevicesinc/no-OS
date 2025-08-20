#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <stdbool.h>

#include <no_os_spi.h>
#include <maxim_spi.h>

#include <no_os_gpio.h>
#include <maxim_gpio.h>

#include <no_os_uart.h>
#include <maxim_uart.h>
#include <maxim_usb_uart.h>

#include <maxim_gpio_irq.h>


extern struct max_gpio_init_param max_gpio_extra;

/* AFE_CTRL */
#define AFE_CTRL_PORT	0
#define AFE_CTRL_NUMBER 10
#define AFE_CTRL_PULL 	NO_OS_PULL_NONE
#define AFE_CTRL_OPS 	&max_gpio_ops
#define AFE_CTRL_EXTRA 	&max_gpio_extra

/* GATE_CTRL_Q4 */
#define GATE_CTRL_Q4_PORT	0
#define GATE_CTRL_Q4_NUMBER 	19
#define GATE_CTRL_Q4_PULL 	NO_OS_PULL_NONE
#define GATE_CTRL_Q4_OPS 	&max_gpio_ops
#define GATE_CTRL_Q4_EXTRA 	&max_gpio_extra

/* GATE_CTRL_Q3 */
#define GATE_CTRL_Q3_PORT	0
#define GATE_CTRL_Q3_NUMBER 	18
#define GATE_CTRL_Q3_PULL 	NO_OS_PULL_NONE
#define GATE_CTRL_Q3_OPS 	&max_gpio_ops
#define GATE_CTRL_Q3_EXTRA 	&max_gpio_extra

/* GATE_CTRL_Q2 */
#define GATE_CTRL_Q2_PORT	0
#define GATE_CTRL_Q2_NUMBER 	17
#define GATE_CTRL_Q2_PULL 	NO_OS_PULL_NONE
#define GATE_CTRL_Q2_OPS 	&max_gpio_ops
#define GATE_CTRL_Q2_EXTRA 	&max_gpio_extra

/* GATE_CTRL_Q1 */
#define GATE_CTRL_Q1_PORT	0
#define GATE_CTRL_Q1_NUMBER 	16
#define GATE_CTRL_Q1_PULL 	NO_OS_PULL_NONE
#define GATE_CTRL_Q1_OPS 	&max_gpio_ops
#define GATE_CTRL_Q1_EXTRA 	&max_gpio_extra

/* GP3 */
#define GP3_PORT	0
#define GP3_NUMBER 	4
#define GP3_PULL 	NO_OS_PULL_NONE
#define GP3_OPS 	&max_gpio_ops
#define GP3_EXTRA 	&max_gpio_extra

/* software chip select for data SPI */
#define DATA_SWSS_PORT		0
#define DATA_SWSS_NUMBER 	21
#define DATA_SWSS_PULL 		NO_OS_PULL_NONE
#define DATA_SWSS_OPS 		&max_gpio_ops
#define DATA_SWSS_EXTRA 	&max_gpio_extra

/* Data SPI */
#define DATA_SPI_DEVICE_ID 	2
#define DATA_SPI_SPEED 		1000000 /* 1 Mhz */
#define DATA_SPI_SS 		0
#define DATA_SPI_MODE 		NO_OS_SPI_MODE_3
#define DATA_SPI_BIT_ORDER 	NO_OS_SPI_BIT_ORDER_MSB_FIRST
#define DATA_SPI_LANES 		NO_OS_SPI_SINGLE_LANE
#define DATA_SPI_OPS 		&max_spi_ops
#define DATA_SPI_EXTRA 		&cfg_spi_extra
extern struct max_spi_init_param data_spi_extra;

/* software chip select for config SPI */
#define CFG_SWSS_PORT		0
#define CFG_SWSS_NUMBER 	20
#define CFG_SWSS_PULL 	 	NO_OS_PULL_NONE
#define CFG_SWSS_OPS 		&max_gpio_ops
#define CFG_SWSS_EXTRA 		&max_gpio_extra

/* Config SPI */
#define CFG_SPI_DEVICE_ID 	2
#define CFG_SPI_SPEED 		1000000 /* 1 Mhz */
#define CFG_SPI_SS 		0
#define CFG_SPI_MODE 		NO_OS_SPI_MODE_3
#define CFG_SPI_BIT_ORDER 	NO_OS_SPI_BIT_ORDER_MSB_FIRST
#define CFG_SPI_LANES 		NO_OS_SPI_SINGLE_LANE
#define CFG_SPI_OPS 		&max_spi_ops
#define CFG_SPI_EXTRA 		&cfg_spi_extra
extern struct max_spi_init_param cfg_spi_extra;

#define INTC_DEVICE_ID 		0

#define IIO_UART_DEVICE_ID 	0
#define IIO_UART_IRQ_ID 	USB_IRQn
#define IIO_UART_ASYNC_RX 	true
#define IIO_UART_BAUD_RATE 	115200
#define IIO_UART_SIZE 		NO_OS_UART_CS_8
#define IIO_UART_PARITY 	NO_OS_UART_PAR_NO
#define IIO_UART_STOP_BIT 	NO_OS_UART_STOP_1_BIT
#define IIO_UART_OPS 		&max_usb_uart_ops
#define IIO_UART_EXTRA 		&iio_uart_extra
extern struct max_usb_uart_init_param iio_uart_extra;

/* our serial log port is the pico rx */
#define SERIAL_LOG_DEVICE_ID 	1
#define SERIAL_LOG_IRQ_ID 	0
#define SERIAL_LOG_ASYNC_RX 	false
#define SERIAL_LOG_BAUD_RATE 	115200
#define SERIAL_LOG_SIZE 	NO_OS_UART_CS_8
#define SERIAL_LOG_PARITY 	NO_OS_UART_PAR_NO
#define SERIAL_LOG_STOP 	NO_OS_UART_STOP_1_BIT

#define GPIO_IRQ_OPS 		&max_gpio_irq_ops

#endif /* __PARAMETERS_H__ */

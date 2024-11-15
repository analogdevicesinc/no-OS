/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-adxl355-pmdz
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef IIO_LWIP_EXAMPLE
extern UART_HandleTypeDef huart1;
#define UART_INSTANCE	(&huart1)
#define SPI_DEVICE_ID   3
#define SPI_CS          12
#define SPI_CS_PORT     6
#else
extern UART_HandleTypeDef huart5;
#define UART_INSTANCE	(&huart5)
#define SPI_DEVICE_ID   1
#define SPI_CS          15
#define SPI_CS_PORT     0
#endif

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart5)
#endif
#define UART_IRQ_ID     UART5_IRQn

#define UART_DEVICE_ID  1
#define UART_BAUDRATE   115200
#define UART_EXTRA      &adxl355_uart_extra_ip
#define UART_OPS        &stm32_uart_ops

#define SPI_BAUDRATE    1000000
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adxl355_spi_extra_ip

extern struct stm32_uart_init_param adxl355_uart_extra_ip;
extern struct stm32_spi_init_param adxl355_spi_extra_ip;

#ifdef IIO_TRIGGER_EXAMPLE
extern struct stm32_gpio_irq_init_param adxl355_gpio_irq_extra_ip;
/* Setting for PortA Pin2 used for DATA_READY.
   Has to be adapted accordingly if another pin is used.
 */
#define ADXL355_GPIO_TRIG_IRQ_ID     0    /* Not used in stm32 platform */
#define ADXL355_GPIO_CB_HANDLE       NULL /* Not used in stm32 platform */

#define GPIO_IRQ_ID             2 /* Pin 2 */
#define GPIO_IRQ_OPS            &stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA          &adxl355_gpio_irq_extra_ip
#endif

#ifdef IIO_LWIP_EXAMPLE
#define ADIN1110_GPIO_RESET_PORT	2
#define ADIN1110_GPIO_RESET_PIN		7
#define ADIN1110_SPI_DEVICE_ID		2
#define ADIN1110_SPI_CS			12
#define ADIN1110_SPI_CS_PORT		1
#define ADIN1110_SPI_CLK_RATE		25000000
#define GPIO_OPS			&stm32_gpio_ops

extern struct stm32_gpio_init_param adin1110_reset_gpio_extra_ip;
extern struct stm32_spi_init_param adin1110_spi_extra_ip;
#endif

#endif /* __PARAMETERS_H__ */

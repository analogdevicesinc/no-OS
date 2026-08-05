/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-ad74416h
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"

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

/* ADC_RDY GPIO/IRQ mapping for the SDP-CK1Z board.
   PA2 is routed to the ADC_RDY signal and configured as GPIO_EXTI2. */
#define GPIO_ADC_RDY_PORT   0 /* Port A */
#define GPIO_ADC_RDY_PIN    2 /* Pin 2 */
#define GPIO_OPS            &stm32_gpio_ops
#define GPIO_EXTRA          NULL

#define GPIO_IRQ_ID1        GPIO_ADC_RDY_PIN
#define GPIO_IRQ_OPS        &stm32_gpio_irq_ops
#define GPIO_IRQ_ADC_EXTRA  &ad74416h_gpio_irq_extra_ip

extern struct stm32_uart_init_param ad74416h_uart_extra_ip;
extern struct stm32_spi_init_param ad74416h_spi_extra_ip;
extern struct stm32_gpio_irq_init_param ad74416h_gpio_irq_extra_ip;
extern struct no_os_gpio_init_param adc_rdy_gpio_ip;
extern struct no_os_irq_init_param adc_rdy_gpio_irq_ip;

#endif /* __PARAMETERS_H__ */

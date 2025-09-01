/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by admt400 project.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 *           Louijie Compo  (louijie.compo@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

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
#include "no_os_uart.h"

extern UART_HandleTypeDef huart5;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#endif

#define UART_DEVICE_ID          5
#define UART_BAUDRATE           230400
#define UART_EXTRA              &uart_extra_ip
#define UART_OPS                &stm32_uart_ops
#define UART_IRQ_ID             UART5_IRQn

/* SPI Pin mapping for Arduino interface */
#define SPI_DEVICE_ID		1
#define SPI_BAUDRATE		2000000
#define SPI_OPS			&stm32_spi_ops
#define SPI_CS_PORT             0
#define SPI_CS                  15
#define GPIO_SPI_SEL_PORT	6
#define GPIO_SPI_SEL_PIN	11
#define SPI_EXTRA		&spi_extra_ip
#define SPI_SEL_EXTRA		&spi_sel_b_extra_ip


#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_EXTRA		&gpio_extra_ip
#define GPIO_EXTRA		&gpio_extra_ip

#define GPIO_SHDN_N_PORT        6
#define GPIO_SHDN_N		7
#define GPIO_GPIO0_BUSY_PORT    3
#define GPIO_GPIO0_BUSY		12
#define GPIO_COIL_RS_PORT       6
#define GPIO_COIL_RS		9
#define GPIO_CNV_PORT           0
#define GPIO_CNV		11

#define GPIO_ACALC_PORT		0
#define GPIO_ACALC		10

#define GPIO_IRQ_ID		10
#define GPIO_IRQ_OPS		&stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA		&trigger_gpio_irq_ip
#define GPIO_IRQ_INTR_PRIORITY	EXTI15_10_IRQn

#define ADMT4000_GPIO_CB_HANDLE		NULL /* Not used in stm32 platform */
#define ADMT4000_GPIO_TRIG_IRQ_ID	0 /* Not used in stm32 platform */

#if (TMC)
#define TMC_CS_PORT             6
#define TMC_CS_PIN              10
#define SPI_TMC_EXTRA		&spi_tmc_extra_ip
#endif

extern struct stm32_uart_init_param uart_extra_ip;
extern struct stm32_spi_init_param spi_extra_ip;
extern struct stm32_spi_init_param spi_sel_b_extra_ip;
extern struct stm32_spi_init_param spi_tmc_extra_ip;
extern struct stm32_gpio_init_param gpio_extra_ip;
extern struct stm32_gpio_irq_init_param trigger_gpio_irq_ip;

#endif /* __PARAMETERS_H__ */
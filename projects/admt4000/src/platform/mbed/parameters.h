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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <PinNames.h>
#include "mbed_uart.h"
#include "mbed_spi.h"
#include "mbed_gpio.h"
#include "mbed_irq.h"
#include "mbed_gpio_irq.h"
#include "no_os_gpio.h"
#include "no_os_uart.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define UART_TX_PIN		CONSOLE_TX
#define	UART_RX_PIN		CONSOLE_RX
#define UART_IRQ_ID		0
#define UART_DEVICE_ID		0
#define UART_BAUDRATE		230400
#define UART_OPS		&mbed_uart_ops
#define UART_EXTRA		&uart_extra_ip

/* SPI Pin mapping for Arduino interface */
#define SPI_CS			ARDUINO_UNO_D10
#define SPI_MOSI		ARDUINO_UNO_D11
#define SPI_MISO		ARDUINO_UNO_D12
#define SPI_SCK			ARDUINO_UNO_D13
#define SPI_DEVICE_ID		0
#define SPI_BAUDRATE		2000000
#define SPI_OPS			&mbed_spi_ops
#define SPI_EXTRA		&spi_extra_ip
#define SPI_SEL_EXTRA		&spi_sel_b_extra_ip

#define GPIO_SPI_SEL_PORT	0
#define GPIO_SPI_SEL_PIN	ARDUINO_UNO_D8
#define GPIO_OPS		&mbed_gpio_ops
#define GPIO_EXTRA		&gpio_extra_ip
#define GPIO_SHDN_N		ARDUINO_UNO_D2
#define GPIO_GPIO0_BUSY		ARDUINO_UNO_D3
#define GPIO_COIL_RS		ARDUINO_UNO_D4
#define GPIO_CNV		ARDUINO_UNO_D5

#define GPIO_ACALC_PORT		0
#define GPIO_ACALC		ARDUINO_UNO_D6

#define IRQ_INT_ID		GPIO_IRQ_ID1
#define GPIO_IRQ_OPS		&mbed_gpio_irq_ops
#define GPIO_IRQ_EXTRA		&mbed_trigger_gpio_irq_ip
#define GPIO_IRQ_INTR_PRIORITY	EXTI15_10_IRQn

#define ADMT4000_GPIO_CB_HANDLE		NULL /* Not used in Mbed platform */
#define ADMT4000_GPIO_TRIG_IRQ_ID	0 /* Not used in Mbed platform */

#if (TMC)
#define TMC_CS_PORT             0
#define TMC_CS_PIN              ARDUINO_UNO_D7
#endif

extern struct mbed_uart_init_param uart_extra_ip;
extern struct mbed_spi_init_param spi_extra_ip;
extern struct mbed_spi_init_param spi_sel_b_extra_ip;
extern struct mbed_gpio_init_param gpio_extra_ip;
extern struct mbed_gpio_irq_init_param mbed_trigger_gpio_irq_ip;

#endif /* __PARAMETERS_H__ */
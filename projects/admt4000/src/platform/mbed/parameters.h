/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by admt400 project.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 *           Louijie Compo  (louijie.compo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <PinNames.h>
#include "mbed_uart.h"
#include "mbed_spi.h"
#include "mbed_gpio.h"
#include "no_os_gpio.h"
#include "no_os_uart.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define UART_TX_PIN	        CONSOLE_TX
#define	UART_RX_PIN	        CONSOLE_RX
#define UART_IRQ_ID             0
#define UART_DEVICE_ID	        0
#define UART_BAUDRATE	        115200
#define UART_OPS	        &mbed_uart_ops
#define UART_EXTRA	        &uart_extra_ip

/* SPI Pin mapping for Arduino interface */
#define SPI_CS		        ARDUINO_UNO_D10
#define SPI_MOSI	        ARDUINO_UNO_D11
#define SPI_MISO	        ARDUINO_UNO_D12
#define SPI_SCK 	        ARDUINO_UNO_D13
#define SPI_DEVICE_ID	        0
#define SPI_BAUDRATE	        2000000
#define SPI_OPS		        &mbed_spi_ops
#define SPI_EXTRA	        &spi_extra_ip
#define SPI_SEL_EXTRA       &spi_sel_b_extra_ip

#define GPIO_SPI_SEL_PORT	0
#define GPIO_SPI_SEL_PIN	ARDUINO_UNO_D8
#define GPIO_OPS            &mbed_gpio_ops
#define GPIO_EXTRA          &gpio_extra_ip
#define GPIO_SHDN_N         ARDUINO_UNO_D2
#define GPIO_GPIO0_BUSY     ARDUINO_UNO_D3
#define GPIO_COIL_RS        ARDUINO_UNO_D4

extern struct mbed_uart_init_param uart_extra_ip;
extern struct mbed_spi_init_param spi_extra_ip;
extern struct mbed_spi_init_param spi_sel_b_extra_ip;
extern struct mbed_gpio_init_param gpio_extra_ip;
extern struct no_os_gpio_init_param spi_sel_b_ip;
extern struct no_os_spi_init_param spi_sel_b_spi_ip;
extern struct admt4000_init_param admt_ip;
extern struct no_os_uart_init_param uart_ip;

#endif /* __PARAMETERS_H__ */
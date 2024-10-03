/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to pico platform used by eval-adxl355-pmdz
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "no_os_util.h"
#include "pico_uart.h"
#include "pico_spi.h"
#include "pico_gpio.h"
#include "pico_gpio_irq.h"
#include "pico_irq.h"
#include "pico_timer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_IRQ_ID     20
#define UART_EXTRA      &adxl355_uart_extra_ip
#define UART_OPS        &pico_uart_ops

#define UART_TX_PIN     UART0_TX_GP0
#define UART_RX_PIN     UART0_RX_GP1

#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    1000000
#define SPI_CS          SPI0_CS_GP17
#define SPI_OPS         &pico_spi_ops
#define SPI_EXTRA       &adxl355_spi_extra_ip

extern struct pico_spi_init_param adxl355_spi_extra_ip;
extern struct pico_uart_init_param adxl355_uart_extra_ip;

#define GPIO_DRDY_PIN_NUM   20
#define GPIO_DRDY_PORT_NUM  0 /* Not used for pico platform */
#define GPIO_OPS            &pico_gpio_ops
#define GPIO_EXTRA          NULL /* Not used for pico platform */

#ifdef IIO_TRIGGER_EXAMPLE
#define ADXL355_GPIO_TRIG_IRQ_ID     GPIO_DRDY_PIN_NUM
#define ADXL355_GPIO_CB_HANDLE       NULL /* Not used in pico platform */

#define GPIO_IRQ_ID             GPIO_DRDY_PIN_NUM
#define GPIO_IRQ_OPS            &pico_gpio_irq_ops
#define GPIO_IRQ_EXTRA          NULL /* Not used for pico platform */
#endif

#endif /* __PARAMETERS_H__ */

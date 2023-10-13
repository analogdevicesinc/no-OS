/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to pico platform used by eval-adis1647x
 *           project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#define UART_EXTRA      &adis1647x_uart_extra_ip
#define UART_OPS        &pico_uart_ops

#define UART_TX_PIN     UART0_TX_GP0
#define UART_RX_PIN     UART0_RX_GP1

#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    1000000
#define SPI_CS          SPI0_CS_GP17
#define SPI_OPS         &pico_spi_ops
#define SPI_EXTRA       &adis1647x_spi_extra_ip

extern struct pico_spi_init_param adis1647x_spi_extra_ip;
extern struct pico_uart_init_param adis1647x_uart_extra_ip;

#define GPIO_RESET_PIN_NUM   20
#define GPIO_RESET_PORT_NUM  0
#define GPIO_OPS            &pico_gpio_ops
#define GPIO_EXTRA          NULL /* Not used for pico platform */

#ifdef IIO_TRIGGER_EXAMPLE
#define GPIO_DRDY_PIN_NUM   21
#define GPIO_DRDY_PORT_NUM  0 /* Not used for pico platform */

#define ADIS1647X_GPIO_TRIG_IRQ_ID     GPIO_DRDY_PIN_NUM
#define ADIS1647X_GPIO_CB_HANDLE       NULL /* Not used in pico platform */

#define GPIO_IRQ_ID             GPIO_DRDY_PIN_NUM
#define GPIO_IRQ_OPS            &pico_gpio_irq_ops
#define GPIO_IRQ_EXTRA          NULL /* Not used for pico platform */
#endif

#endif /* __PARAMETERS_H__ */

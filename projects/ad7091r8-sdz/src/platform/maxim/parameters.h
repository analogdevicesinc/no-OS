/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-ad7091r8-sdz
 *           project.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
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
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_timer.h"
#include "no_os_timer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID		0
#endif

#define UART_DEVICE_ID		0
#define UART_IRQ_ID		UART0_IRQn
#define UART_BAUDRATE		115200
#define UART_EXTRA		&ad7091r8_uart_extra_ip
#define UART_OPS		&max_uart_ops
#define INTC_DEVICE_ID		0
#define SPI_DEVICE_ID		1
#define SPI_CS			1
#define SPI_BAUDRATE		200000
#define SPI_OPS			&max_spi_ops
#define SPI_EXTRA		&ad7091r8_spi_extra_ip

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&ad7091r8_gpio_extra_ip
#define GPIO_RESET_PIN_NUM	19
#define GPIO_RESET_PORT_NUM	0
#define GPIO_RESET_INIT		&ad7091r8_gpio_reset_ip
#define GPIO_CONVST_PIN_NUM	6
#define GPIO_CONVST_PORT_NUM	1
#define GPIO_CONVST_INIT	&ad7091r8_gpio_convst_ip
#define GPIO_IRQ_ID		0

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
/* AD7091R-8 Timer settings */
#define AD7091R8_TIMER_DEVICE_ID    0
#define AD7091R8_TIMER_FREQ_HZ      1000000
#define AD7091R8_TIMER_TICKS_COUNT  2000
#define AD7091R8_TIMER_EXTRA        NULL
#define TIMER_OPS                   &max_timer_ops

/* AD7091R-8 Timer trigger settings */
#define AD7091R8_TIMER_IRQ_ID       TMR0_IRQn
#define TIMER_IRQ_OPS               &max_irq_ops
#define AD7091R8_TIMER_IRQ_EXTRA    NULL

/* AD7091R-8 timer trigger settings */
#define AD7091R8_TIMER_CB_HANDLE    MXC_TMR0
#define AD7091R8_TIMER_TRIG_IRQ_ID  TMR0_IRQn

#endif /* IIO_TIMER_TRIGGER_EXAMPLE */

extern struct max_uart_init_param ad7091r8_uart_extra_ip;
extern struct max_spi_init_param ad7091r8_spi_extra_ip;
extern struct max_gpio_init_param ad7091r8_gpio_extra_ip;
extern struct no_os_gpio_init_param ad7091r8_gpio_reset_ip;
extern struct no_os_gpio_init_param ad7091r8_gpio_convst_ip;

#endif /* __PARAMETERS_H__ */

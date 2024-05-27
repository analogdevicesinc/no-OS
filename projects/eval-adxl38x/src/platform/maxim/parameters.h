/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adxl38x-pmdz
 *           project.
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#if (TARGET_NUM == 32690)
#define UART_IRQ_ID     UART2_IRQn
#define UART_DEVICE_ID  2
#define UART_BAUDRATE   57600
#else
#define UART_IRQ_ID     UART0_IRQn
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#endif
#define UART_EXTRA      &adxl38x_uart_extra_ip
#define UART_OPS        &max_uart_ops

#if (TARGET_NUM == 78000)
#define SPI_DEVICE_ID   1
#define SPI_CS          1
#elif (TARGET_NUM == 32650)
#define SPI_DEVICE_ID   1
#define SPI_CS          0
#elif (TARGET_NUM == 32690)
#define SPI_DEVICE_ID   0
#define SPI_CS          1
#else
#define SPI_DEVICE_ID   0
#define SPI_CS          0
#endif

#define SPI_BAUDRATE    1000000
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adxl38x_spi_extra_ip

extern struct max_uart_init_param adxl38x_uart_extra_ip;
extern struct max_spi_init_param adxl38x_spi_extra_ip;

#ifdef IIO_TRIGGER_EXAMPLE
#define GPIO_OPS            &max_gpio_ops

#if (TARGET_NUM == 78000)
#define GPIO_DRDY_PIN_NUM   19
#define GPIO_DRDY_PORT_NUM  0
/* Setting for Port1 Pin9 used for DATA_READY.
Has to be adapted accordingly if another pin is used.
*/
#define NVIC_GPIO_IRQ   GPIO0_IRQn
#define GPIO_IRQ_ID     0
#elif (TARGET_NUM == 32655)
#define GPIO_DRDY_PIN_NUM   9
#define GPIO_DRDY_PORT_NUM  1
/* Setting for Port1 Pin9 used for DATA_READY.
Has to be adapted accordingly if another pin is used.
 */
#define NVIC_GPIO_IRQ   GPIO1_IRQn
#define GPIO_IRQ_ID     1
#elif (TARGET_NUM == 32690)
#define GPIO_DRDY_PIN_NUM   14
#define GPIO_DRDY_PORT_NUM  0
#define NVIC_GPIO_IRQ   GPIO0_IRQn
#define GPIO_IRQ_ID     0
#else
#error IIO_TRIGGER_EXAMPLE is currently supported only on max32655 and max 78000 targets.
#endif

#define GPIO_EXTRA          &adxl38x_gpio_extra_ip

extern struct no_os_gpio_init_param adxl38x_gpio_drdy_ip;

#define ADXL38x_GPIO_TRIG_IRQ_ID GPIO_DRDY_PIN_NUM
#define ADXL38x_GPIO_CB_HANDLE   MXC_GPIO_GET_GPIO(GPIO_DRDY_PORT_NUM)

#define GPIO_IRQ_OPS    &max_gpio_irq_ops
#define GPIO_IRQ_EXTRA  &adxl38x_gpio_extra_ip

extern struct max_gpio_init_param adxl38x_gpio_extra_ip;
#endif

#endif /* __PARAMETERS_H__ */

/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-adxl38x-pmdz
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
extern UART_HandleTypeDef huart3;
#define UART_INSTANCE	(&huart3)
#define SPI_DEVICE_ID   1
#define SPI_CS          14
#define SPI_CS_PORT     3
#endif

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart5)
#endif

#define UART_IRQ_ID     USART3_IRQn
#define UART_DEVICE_ID  1
#define UART_BAUDRATE   115200
#define UART_EXTRA      &adxl38x_uart_extra_ip
#define UART_OPS        &stm32_uart_ops

#define SPI_BAUDRATE    1000000
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adxl38x_spi_extra_ip

extern struct stm32_uart_init_param adxl38x_uart_extra_ip;
extern struct stm32_spi_init_param adxl38x_spi_extra_ip;

#ifdef IIO_TRIGGER_EXAMPLE
extern struct stm32_gpio_irq_init_param adxl38x_gpio_irq_extra_ip;
/* Setting for PortA Pin2 used for DATA_READY.
   Has to be adapted accordingly if another pin is used.
 */
#define ADXL38x_GPIO_TRIG_IRQ_ID     0    /* Not used in stm32 platform */
#define ADXL38x_GPIO_CB_HANDLE       NULL /* Not used in stm32 platform */

#define GPIO_IRQ_ID             2 /* Pin 2 */
#define GPIO_IRQ_OPS            &stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA          &adxl38x_gpio_irq_extra_ip
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

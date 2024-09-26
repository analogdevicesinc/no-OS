/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to aducm3029 platform used by eval-adxl355-pmdz
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "stdio.h"
#include "platform_init.h"
#include "aducm3029_gpio.h"
#include "aducm3029_spi.h"
#include "aducm3029_irq.h"
#include "aducm3029_gpio_irq.h"
#include "aducm3029_uart.h"
#include "aducm3029_uart_stdio.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef IIO_SUPPORT
#define DATA_BUFFER_SIZE        300
#define INTC_DEVICE_ID          0
#endif
#define UART_IRQ_ID             ADUCM_UART_INT_ID

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_EXTRA      NULL
#define UART_OPS        &aducm_uart_ops

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    4000000
#define SPI_CS          0
#define SPI_OPS         &aducm_spi_ops
#define SPI_EXTRA       &adxl355_spi_extra_ip

#ifdef IIO_TRIGGER_EXAMPLE
#define GPIO_DRDY_PIN_NUM       12
#define GPIO_DRDY_PORT_NUM      0
#define GPIO_OPS                &aducm_gpio_ops
#define GPIO_EXTRA              NULL /* Not used for aducm3029 platform */

#define GPIO_IRQ_ID             ADUCM_GPIO_A_GROUP_SOFT_CTRL
#define GPIO_IRQ_OPS            &aducm_gpio_irq_ops
#define GPIO_IRQ_EXTRA          NULL /* Not used for aducm3029 platform */

#define ADXL355_GPIO_TRIG_IRQ_ID    GPIO_DRDY_PIN_NUM
#define ADXL355_GPIO_CB_HANDLE      NULL /* Not used for aducm3029 platform */

extern struct no_os_gpio_init_param adxl355_gpio_drdy_ip;
#endif

extern struct aducm_spi_init_param adxl355_spi_extra_ip;
#endif /* __PARAMETERS_H__ */

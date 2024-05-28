/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-ad400x
 *           project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
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
extern UART_HandleTypeDef huart5;
#define SAMPLES_PER_CHANNEL_PLATFORM 2000
#define MAX_SIZE_BASE_ADDR     (SAMPLES_PER_CHANNEL_PLATFORM * sizeof(uint32_t))
extern uint8_t in_buff[];

#define ADC_DDR_BASEADDR	in_buff

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID 0
#endif

#define UART_IRQ_ID		UART5_IRQn

#define UART_DEVICE_ID		5
#define UART_BAUDRATE		230400
#define UART_EXTRA		&ad400x_uart_extra_ip
#define UART_OPS		&stm32_uart_ops

/*
 * spi cs is not used, but current no-os mandates
 * a cs to be specifed. So we need to add some
 * dummy gpio value
 */
#define SPI_DEVICE_ID		1
#define SPI_BAUDRATE		20000000
#define SPI_CS			14
#define SPI_CS_PORT		GPIO_PORT_A
#define SPI_OPS			&stm32_spi_ops
#define SPI_EXTRA		&ad400x_spi_extra_ip

#define GPIO_CNV		15
#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_CNV_PORT		GPIO_PORT_A

#define GPIO_PORT_A		0

#define AD400X_ADC_REF_VOLTAGE 5000
extern struct stm32_uart_init_param ad400x_uart_extra_ip;
extern struct stm32_spi_init_param ad400x_spi_extra_ip;

#endif /* __PARAMETERS_H__ */

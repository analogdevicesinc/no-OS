/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-ad738x
 *           project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
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
#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "no_os_units.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern UART_HandleTypeDef huart5;
#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID 0
#endif

#define XSTR(x) STR(x)
#define STR(x) #x

#define UART_IRQ_ID		UART5_IRQn

#define UART_DEVICE_ID		5
#define UART_BAUDRATE		115200
#define UART_EXTRA		&uart_extra_ip
#define UART_OPS		&stm32_uart_ops


#define SPI_DEVICE_ID		1
#define SPI_BAUDRATE		45000000 /* multiples of sysclk */
#define SPI_CS			15
#define SPI_CS_PORT		GPIO_PORT_A
#define SPI_OPS			&stm32_spi_ops
#define SPI_EXTRA		&spi_extra_ip

#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_EXTRA		NULL
#define GPIO_CNV		10
#define GPIO_CNV_PORT		GPIO_PORT_A
#define GPIO_PORT_A		0

#define SAMPLES_PER_CHANNEL		1000
#define BYTES_PER_SAMPLE		4 /* one sample contains all channels */
#define MAX_SIZE_BASE_ADDR		(SAMPLES_PER_CHANNEL * BYTES_PER_SAMPLE)

/* time to xfer 1 byte */
#define TX_PWM_PERIOD_NS	(( 8 * KILO) / ((SPI_BAUDRATE / MEGA ) - 1))
#define TX_PWM_DUTY_NS		1 /* dont care */
#define TX_PWM_REPS		(BYTES_PER_SAMPLE - 1)

/* time to transfer a sample */
#define CS_PWM_PERIOD_NS	((TX_PWM_PERIOD_NS * BYTES_PER_SAMPLE) + CS_PWM_DUTY_NS + 200)
#define CS_PWM_DUTY_NS		30  /* quiet time, from datasheet */

/* sample rate */
#define TRIGGER_PERIOD_NS 	(CS_PWM_PERIOD_NS + TRIGGER_DUTY_NS)
#define TRIGGER_DUTY_NS		20
#define TRIGGER_PWM_ID		1

#define PWM_OPS			&stm32_pwm_ops
#define TRIGGER_PWM_EXTRA	&trigger_pwm_extra_init_params

extern struct stm32_uart_init_param uart_extra_ip;
extern struct stm32_spi_init_param spi_extra_ip;
extern struct stm32_pwm_init_param trigger_pwm_extra_init_params;

#define CLKGEN_INIT			NULL
#define OFFLOAD_INIT			NULL
#define SPI_ENGINE_INIT			NULL
#define DCACHE_INVALIDATE		NULL


#endif /* __PARAMETERS_H__ */

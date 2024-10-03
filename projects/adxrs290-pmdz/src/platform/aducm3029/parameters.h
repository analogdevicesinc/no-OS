/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-adxrs290-pmdz
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
#include "stdio.h"
#include "platform_init.h"
#include "aducm3029_gpio.h"
#include "aducm3029_spi.h"
#include "aducm3029_irq.h"
#include "aducm3029_gpio_irq.h"
#include "aducm3029_timer.h"
#include "aducm3029_uart.h"
#include "no_os_timer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define UART_DEVICE_ID	0
#define INTC_DEVICE_ID	0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200
#define UART_EXTRA      NULL
#define UART_OPS        &aducm_uart_ops

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    4000000
#define SPI_CS          0
#define SPI_OPS         &aducm_spi_ops
#define SPI_EXTRA       &adxrs290_spi_extra_ip

#define GPIO_SYNC_PIN_NUM       0x10
#define GPIO_SYNC_PORT_NUM      0
#define GPIO_OPS                &aducm_gpio_ops
#define GPIO_EXTRA              NULL

#ifdef IIO_TRIGGER_EXAMPLE
#define GPIO_IRQ_ID     ADUCM_XINT_SOFT_CTRL /* External interrupt used (to be able to use high level trigger) */
#define GPIO_IRQ_OPS    &aducm_gpio_irq_ops
#define GPIO_IRQ_EXTRA  NULL /* Not used for aducm3029 platform */

#define ADXRS290_GPIO_TRIG_IRQ_ID    ADI_XINT_EVENT_INT1 /* for EVAL-ADICUP3029 pin number 16 is connected to XINT1 */
#define ADXRS290_GPIO_CB_HANDLE      NULL /* Not used for aducm3029 platform */
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
/* ADXRS290 Timer settings */
extern struct aducm_timer_init_param adxrs290_timer_extra_ip;
#define ADXRS290_TIMER_DEVICE_ID    1
#define ADXRS290_TIMER_FREQ_HZ      200 /* Not used - Used clock source frequency is the one specified in adxrs290_timer_extra_ip */
#define ADXRS290_TIMER_TICKS_COUNT  0xffff
#define ADXRS290_TIMER_EXTRA        &adxrs290_timer_extra_ip
#define TIMER_OPS                   &aducm_timer_ops

/* ADXRS290 Timer trigger settings */
#define ADXRS290_TIMER_IRQ_ID       TMR1_EVT_IRQn /* IRQ id for TMR1 */
#define TIMER_IRQ_OPS               &aducm_irq_ops
#define ADADXRS290_TIMER_IRQ_EXTRA  NULL /* Not used */

/* ADXRS290 timer trigger settings */
#define ADXRS290_TIMER_CB_HANDLE    0 /* Device descriptor is being used as a handle in this case */
#define ADXRS290_TIMER_TRIG_IRQ_ID  ADUCM_TIMER1_INT_ID
#endif

extern struct aducm_spi_init_param adxrs290_spi_extra_ip;

#endif /* __PARAMETERS_H__ */

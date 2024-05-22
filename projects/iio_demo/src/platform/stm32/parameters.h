/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to stm32 platform used by iio_demo
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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
#include "stm32_timer.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "common_data.h"
#include "no_os_util.h"

#if defined(STM32F413xx) || defined(STM32H743xx)
#include "stm32_usb_uart.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* This value can be modified based on the number
of samples needed to be stored in the device buffer
and based on the available RAM memory of the platform */
#define SAMPLES_PER_CHANNEL_PLATFORM 2000

#if defined(STM32F413xx) || defined(STM32H743xx)
#define UART_EXTRA      NULL
#define UART_OPS        &stm32_usb_uart_ops
#elif defined(STM32F469xx)
#define UART_EXTRA      &iio_demo_uart_extra_ip
#define UART_OPS        &stm32_uart_ops
#else
#endif

extern UART_HandleTypeDef huart5;
#define UART_IRQ_ID     UART5_IRQn
#define INTC_DEVICE_ID 0
#define IIO_APP_HUART	(&huart5)
#define UART_DEVICE_ID      5
#define UART_BAUDRATE  115200

extern struct stm32_uart_init_param iio_demo_uart_extra_ip;
extern struct stm32_uart_init_param iio_demo_usb_uart_extra_ip;

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
/* Adc Demo Timer settings */
extern struct stm32_timer_init_param adc_demo_xtip;
#define ADC_DEMO_TIMER_DEVICE_ID    13
#define ADC_DEMO_TIMER_FREQ_HZ      1000000
#define ADC_DEMO_TIMER_TICKS_COUNT  2000
#define ADC_DEMO_TIMER_EXTRA        &adc_demo_xtip
#define TIMER_OPS                   &stm32_timer_ops

/* Adc Demo Timer trigger settings */
extern TIM_HandleTypeDef htim13;
#define ADC_DEMO_TIMER_IRQ_ID       TIM8_UP_TIM13_IRQn
#define TIMER_IRQ_OPS               &stm32_irq_ops
#define ADC_DEMO_TIMER_IRQ_EXTRA    &htim13

/* Adc Demo timer trigger settings */
#define ADC_DEMO_TIMER_CB_HANDLE    &htim13
#define ADC_DEMO_TIMER_TRIG_IRQ_ID  TIM8_UP_TIM13_IRQn

/* Dac Demo Timer settings */
extern struct stm32_timer_init_param dac_demo_xtip;
#define DAC_DEMO_TIMER_DEVICE_ID    14
#define DAC_DEMO_TIMER_FREQ_HZ      1000000
#define DAC_DEMO_TIMER_TICKS_COUNT  2000
#define DAC_DEMO_TIMER_EXTRA        &dac_demo_xtip
#define TIMER_OPS                   &stm32_timer_ops

/* Dac Demo Timer trigger settings */
extern TIM_HandleTypeDef htim14;
#define DAC_DEMO_TIMER_IRQ_ID       TIM8_TRG_COM_TIM14_IRQn
#define TIMER_IRQ_OPS               &stm32_irq_ops
#define DAC_DEMO_TIMER_IRQ_EXTRA    &htim14

/* Dac Demo timer trigger settings */
#define DAC_DEMO_TIMER_CB_HANDLE    &htim14
#define DAC_DEMO_TIMER_TRIG_IRQ_ID  TIM8_TRG_COM_TIM14_IRQn

#endif

#endif /* __PARAMETERS_H__ */

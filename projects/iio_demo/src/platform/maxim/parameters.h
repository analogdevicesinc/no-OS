/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by iio_demo
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_timer.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_timer.h"
#ifdef NO_OS_USB_UART
#include "maxim_usb_uart.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX_SIZE_BASE_ADDR	(SAMPLES_PER_CHANNEL * DEMO_CHANNELS * \
					sizeof(uint16_t))

#define SAMPLES_PER_CHANNEL_PLATFORM 2000

extern struct max_uart_init_param iio_demo_uart_extra_ip;
#ifdef NO_OS_USB_UART
extern struct max_usb_uart_init_param iio_demo_usb_uart_extra_ip;
#endif

#define INTC_DEVICE_ID	0
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600
#ifdef NO_OS_USB_UART
#define UART_IRQ_ID    	USB_IRQn
#define UART_EXTRA      &iio_demo_usb_uart_extra_ip
#define UART_OPS        &max_usb_uart_ops
#else
#define UART_IRQ_ID    	UART0_IRQn
#define UART_EXTRA      &iio_demo_uart_extra_ip
#define UART_OPS        &max_uart_ops
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
/* Adc Demo Timer settings */
#define ADC_DEMO_TIMER_DEVICE_ID    0
#define ADC_DEMO_TIMER_FREQ_HZ      1000000
#define ADC_DEMO_TIMER_TICKS_COUNT  2000
#define ADC_DEMO_TIMER_EXTRA        NULL
#define TIMER_OPS                   &max_timer_ops

/* Adc Demo Timer trigger settings */
#define ADC_DEMO_TIMER_IRQ_ID       TMR0_IRQn
#define TIMER_IRQ_OPS               &max_irq_ops
#define ADC_DEMO_TIMER_IRQ_EXTRA    NULL

/* Adc Demo timer trigger settings */
#define ADC_DEMO_TIMER_CB_HANDLE    MXC_TMR0
#define ADC_DEMO_TIMER_TRIG_IRQ_ID  TMR0_IRQn

/* Dac Demo Timer settings */
#define DAC_DEMO_TIMER_DEVICE_ID    1
#define DAC_DEMO_TIMER_FREQ_HZ      1000000
#define DAC_DEMO_TIMER_TICKS_COUNT  2000
#define DAC_DEMO_TIMER_EXTRA        NULL
#define TIMER_OPS                   &max_timer_ops

/* Dac Demo Timer trigger settings */
#define DAC_DEMO_TIMER_IRQ_ID       TMR1_IRQn
#define TIMER_IRQ_OPS               &max_irq_ops
#define DAC_DEMO_TIMER_IRQ_EXTRA    NULL

/* Dac Demo timer trigger settings */
#define DAC_DEMO_TIMER_CB_HANDLE    MXC_TMR1
#define DAC_DEMO_TIMER_TRIG_IRQ_ID  TMR1_IRQn

#endif

#endif /* __PARAMETERS_H__ */

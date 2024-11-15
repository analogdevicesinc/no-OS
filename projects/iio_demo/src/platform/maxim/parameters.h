/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by iio_demo
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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

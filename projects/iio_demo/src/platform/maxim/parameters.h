/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by iio_demo
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_timer.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_timer.h"
#ifdef CONFIG_USB_UART_MAXIM
#include "maxim_usb_uart.h"
#endif

#define MAX_SIZE_BASE_ADDR	(SAMPLES_PER_CHANNEL * DEMO_CHANNELS * \
					sizeof(uint16_t))

#define SAMPLES_PER_CHANNEL_PLATFORM 2000

extern struct max_uart_init_param iio_demo_uart_extra_ip;
#ifdef CONFIG_USB_UART_MAXIM
extern struct max_usb_uart_init_param iio_demo_usb_uart_extra_ip;
#endif

#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600
#ifdef CONFIG_USB_UART_MAXIM
#define UART_IRQ_ID    	USB_IRQn
#define UART_EXTRA      &iio_demo_usb_uart_extra_ip
#define UART_OPS        &max_usb_uart_ops
#else
#define UART_IRQ_ID    	UART0_IRQn
#define UART_EXTRA      &iio_demo_uart_extra_ip
#define UART_OPS        &max_uart_ops
#endif

#ifdef CONFIG_IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE
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

#ifdef CONFIG_IIO_DEMO_LIBIIO_EXAMPLE
/*
 * libiio example: the probe's UART (UART0) carries the uart transport, or the
 * logs on the other transports; TMR1 paces the on-chip ADC (timer0 trigger),
 * TMR0 is the usb transport's tick. Same priority for both timers: no-OS
 * walks every timer callback with one shared iterator.
 */
extern struct no_os_uart_init_param libiio_uart_ip;

#define LIBIIO_TIMER_FREQ_HZ		1000000
#define LIBIIO_TIMER_OPS		&max_timer_ops
#define LIBIIO_IRQ_OPS			&max_irq_ops
#define LIBIIO_TIMER_IRQ_PRIORITY	3

#define LIBIIO_USB_TICK_TIMER_ID	0
#define LIBIIO_USB_TICK_TICKS		100	/* 10 kHz */
#define LIBIIO_USB_TICK_IRQ_ID		TMR0_IRQn
#define LIBIIO_USB_TICK_IRQ_HANDLE	MXC_TMR0

/* libiio's ids; the host finds the device by its "IIO" interface. */
#define LIBIIO_USB_VENDOR_ID		0x0456
#define LIBIIO_USB_PRODUCT_ID		0xB673

#ifdef CONFIG_LIBIIO_TRANSPORT_NETWORK
/*
 * lwIP on the ADIN1110 as wired on the AD-APARD32690-SL (parameters.c).
 * Declared without lwIP's headers: they define ENABLE_LOOPBACK, as
 * app_config.h does.
 */
struct lwip_network_param;
extern struct lwip_network_param libiio_lwip_ip;
#endif
#endif /* CONFIG_IIO_DEMO_LIBIIO_EXAMPLE */

#endif /* __PARAMETERS_H__ */

/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to pico platform used by adxrs290
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
#include "common_data.h"
#include "no_os_util.h"
#include "pico_uart.h"
#include "pico_spi.h"
#include "pico_gpio.h"
#include "pico_gpio_irq.h"
#include "pico_irq.h"
#include "pico_timer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* This value can be modified based on the number
of samples needed to be stored in the device buffer
and based on the available RAM memory of the platform */
#define SAMPLES_PER_CHANNEL_PLATFORM 2000

#define UART_DEVICE_ID      0
#define UART_BAUDRATE  115200
#define UART_IRQ_ID        20
#define UART_EXTRA      &adxrs290_uart_extra_ip
#define UART_OPS        &pico_uart_ops

#define UART_TX_PIN     UART0_TX_GP0
#define UART_RX_PIN     UART0_RX_GP1


#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    1000000
#define SPI_CS          SPI0_CS_GP17
#define SPI_OPS         &pico_spi_ops
#define SPI_EXTRA       &adxrs290_spi_extra_ip

extern struct pico_uart_init_param adxrs290_uart_extra_ip;
extern struct pico_spi_init_param adxrs290_spi_extra_ip;

#define GPIO_SYNC_PIN_NUM   20
#define GPIO_SYNC_PORT_NUM  0 /* Not used for pico platform */
#define GPIO_OPS            &pico_gpio_ops
#define GPIO_EXTRA          NULL /* Not used for pico platform */


#ifdef IIO_TRIGGER_EXAMPLE
#define GPIO_IRQ_ID     GPIO_SYNC_PIN_NUM
#define GPIO_IRQ_OPS    &pico_gpio_irq_ops
#define GPIO_IRQ_EXTRA  NULL /* Not used for pico platform */

#define ADXRS290_GPIO_TRIG_IRQ_ID    GPIO_SYNC_PIN_NUM
#define ADXRS290_GPIO_CB_HANDLE      NULL /* Not used for pico platform */
#endif


#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#define ADXRS290_TIMER_DEVICE_ID    0 /* For alarm0 */
#define ADXRS290_TIMER_FREQ_HZ      1000000
#define ADXRS290_TIMER_TICKS_COUNT  1000
#define ADXRS290_TIMER_EXTRA        NULL /* Not used for pico platform */
#define TIMER_OPS                   &pico_timer_ops

/* ADXRS290 Timer trigger settings */
#define ADXRS290_TIMER_IRQ_ID       0 /* for TIMER_IRQ_0 */
#define TIMER_IRQ_OPS               &pico_irq_ops
#define ADADXRS290_TIMER_IRQ_EXTRA  NULL /* Not used for pico platform */

/* ADXRS290 timer trigger settings */
extern uint8_t adxrs290_timer_extra_ip;
#define ADXRS290_TIMER_CB_HANDLE    &adxrs290_timer_extra_ip /* for alarm0 */
#define ADXRS290_TIMER_TRIG_IRQ_ID  0 /* for TIMER_IRQ_0 */
#endif

#endif /* __PARAMETERS_H__ */

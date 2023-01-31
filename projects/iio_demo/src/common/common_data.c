/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by iio_demo examples.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef ENABLE_LOOPBACK
static uint16_t loopback_buffs[DEMO_CHANNELS][SAMPLES_PER_CHANNEL];
#endif

struct no_os_uart_init_param iio_demo_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct adc_demo_init_param adc_init_par = {
	.ext_buff_len = SAMPLES_PER_CHANNEL,
	.ext_buff = (uint16_t **)loopback_buffs,
	.dev_global_attr = 3333,
	.dev_ch_attr = {
		1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118,
		1119, 1120,	1121, 1122, 1123, 1124, 1125, 1126
	}
};

struct dac_demo_init_param dac_init_par = {
	.loopback_buffer_len = SAMPLES_PER_CHANNEL,
	.loopback_buffers = (uint16_t **)loopback_buffs,
	.dev_global_attr = 4444,
	.dev_ch_attr = {
		1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118,
		1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126
	}
};

uint8_t in_buff[MAX_SIZE_BASE_ADDR] = {0};
uint8_t out_buff[MAX_SIZE_BASE_ADDR] = {0};

#ifdef IIO_SW_TRIGGER_EXAMPLE
struct iio_sw_trig_init_param adc_trig_ip = {
	.name = ADC_DEMO_TRIG_NAME
};

struct iio_sw_trig_init_param dac_trig_ip = {
	.name = DAC_DEMO_TRIG_NAME
};
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
/* Adc Demo timer init parameter */
struct no_os_timer_init_param adc_demo_tip = {
	.id = ADC_DEMO_TIMER_DEVICE_ID,
	.freq_hz = ADC_DEMO_TIMER_FREQ_HZ,
	.ticks_count = ADC_DEMO_TIMER_TICKS_COUNT,
	.platform_ops = TIMER_OPS,
	.extra = ADC_DEMO_TIMER_EXTRA,
};

/* Adc Demo timer irq init parameter */
struct no_os_irq_init_param adc_demo_timer_irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = TIMER_IRQ_OPS,
	.extra = ADC_DEMO_TIMER_IRQ_EXTRA,
};

/* Adc Demo timer trigger callback info */
const struct iio_hw_trig_cb_info adc_demo_timer_cb_info = {
	.event = NO_OS_EVT_TIM_ELAPSED,
	.peripheral = NO_OS_TIM_IRQ,
	.handle = ADC_DEMO_TIMER_CB_HANDLE,
};

/* Adc Demo timer trigger init parameter */
struct iio_hw_trig_init_param adc_demo_timer_trig_ip = {
	.irq_id = ADC_DEMO_TIMER_TRIG_IRQ_ID,
	.cb_info = adc_demo_timer_cb_info,
	.name = ADC_DEMO_TIMER_TRIG_NAME,
};
struct no_os_uart_init_param adxl355_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};
/* Dac Demo timer init parameter */
struct no_os_timer_init_param dac_demo_tip = {
	.id = DAC_DEMO_TIMER_DEVICE_ID,
	.freq_hz = DAC_DEMO_TIMER_FREQ_HZ,
	.ticks_count = DAC_DEMO_TIMER_TICKS_COUNT,
	.platform_ops = TIMER_OPS,
	.extra = DAC_DEMO_TIMER_EXTRA,
};

/* Dac Demo timer irq init parameter */
struct no_os_irq_init_param dac_demo_timer_irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = TIMER_IRQ_OPS,
	.extra = DAC_DEMO_TIMER_IRQ_EXTRA,
};

/* Dac Demo timer trigger callback info */
const struct iio_hw_trig_cb_info dac_demo_timer_cb_info = {
	.event = NO_OS_EVT_TIM_ELAPSED,
	.peripheral = NO_OS_TIM_IRQ,
	.handle = DAC_DEMO_TIMER_CB_HANDLE,
};

/* Adc Demo timer trigger init parameter */
struct iio_hw_trig_init_param dac_demo_timer_trig_ip = {
	.irq_id = DAC_DEMO_TIMER_TRIG_IRQ_ID,
	.cb_info = dac_demo_timer_cb_info,
	.name = DAC_DEMO_TIMER_TRIG_NAME,
};
#endif

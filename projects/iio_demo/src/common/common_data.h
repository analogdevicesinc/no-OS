/***************************************************************************//**
 *   @file   iio_demo/src/common/common_data.h
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "app_config.h"
#include "no_os_util.h"
#include "adc_demo.h"
#include "dac_demo.h"
#if defined(IIO_SW_TRIGGER_EXAMPLE) || defined(IIO_TIMER_TRIGGER_EXAMPLE)
#include "iio_trigger.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param iio_demo_uart_ip;

#define DEMO_CHANNELS no_os_max(TOTAL_ADC_CHANNELS, TOTAL_DAC_CHANNELS)

#ifdef ENABLE_LOOPBACK
#define SAMPLES_PER_CHANNEL	SAMPLES_PER_CHANNEL_PLATFORM
#else //ENABLE_LOOPBACK
#define SAMPLES_PER_CHANNEL	0
#define loopback_buffs		NULL
#endif //ENABLE_LOOPBACK

extern uint8_t in_buff[];
extern uint8_t out_buff[];

#define MAX_SIZE_BASE_ADDR	(SAMPLES_PER_CHANNEL * DEMO_CHANNELS * \
					sizeof(uint16_t))

#define DAC_DDR_BASEADDR	out_buff
#define ADC_DDR_BASEADDR	in_buff

extern struct adc_demo_init_param adc_init_par;
extern struct dac_demo_init_param dac_init_par;

#ifdef IIO_SW_TRIGGER_EXAMPLE
#define ADC_DEMO_TRIG_NAME "adc-demo-sw-trig"
extern struct iio_sw_trig_init_param adc_trig_ip;

#define DAC_DEMO_TRIG_NAME "dac-demo-sw-trig"
extern struct iio_sw_trig_init_param dac_trig_ip;
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#define ADC_DEMO_TIMER_TRIG_NAME "adc_demo_timer_trig"
extern struct no_os_timer_init_param adc_demo_tip;
extern struct no_os_irq_init_param adc_demo_timer_irq_ip;
extern struct iio_hw_trig_init_param adc_demo_timer_trig_ip;

#define DAC_DEMO_TIMER_TRIG_NAME "dac_demo_timer_trig"
extern struct no_os_timer_init_param dac_demo_tip;
extern struct no_os_irq_init_param dac_demo_timer_irq_ip;
extern struct iio_hw_trig_init_param dac_demo_timer_trig_ip;
#endif

#endif /* __COMMON_DATA_H__ */

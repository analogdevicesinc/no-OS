/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Common init params shared across the cora_z7 project.
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include <stdint.h>
#include "platform_includes.h"
#include "app_config.h"
#include "no_os_uart.h"
#include "no_os_util.h"

/* UART init params - not used for IIO under NO_OS_LWIP_NETWORKING but still
 * required as a field in iio_app_init_param. */
extern struct no_os_uart_init_param cora_z7_uart_ip;

#ifndef ECHO_EXAMPLE

#include "adc_demo.h"
#include "dac_demo.h"

/* Number of virtual channels in each demo device */
#undef TOTAL_ADC_CHANNELS
#define TOTAL_ADC_CHANNELS	4
#undef TOTAL_DAC_CHANNELS
#define TOTAL_DAC_CHANNELS	4
#define DEMO_CHANNELS		no_os_max(TOTAL_ADC_CHANNELS, TOTAL_DAC_CHANNELS)

#ifdef ENABLE_LOOPBACK
#define SAMPLES_PER_CHANNEL	SAMPLES_PER_CHANNEL_PLATFORM
#else
#define SAMPLES_PER_CHANNEL	0
#define loopback_buffs		NULL
#endif

#define MAX_SIZE_BASE_ADDR	(SAMPLES_PER_CHANNEL * DEMO_CHANNELS * sizeof(uint16_t))

extern uint8_t in_buff[];
extern uint8_t out_buff[];

#define ADC_DDR_BASEADDR	in_buff
#define DAC_DDR_BASEADDR	out_buff

extern struct adc_demo_init_param adc_init_par;
extern struct dac_demo_init_param dac_init_par;

#endif /* ECHO_EXAMPLE */

#endif /* __COMMON_DATA_H__ */

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

uint8_t in_buff[MAX_SIZE_BASE_ADDR];
uint8_t out_buff[MAX_SIZE_BASE_ADDR];

#ifdef IIO_TRIGGER_EXAMPLE
struct iio_sw_trig_init_param adc_trig_ip = {
	.name = ADC_DEMO_TRIG_NAME
};
#endif

/***************************************************************************//**
 *   @file   aducm3029_adc.h
 *   @brief  Interface of aducm3029_adc.c
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef ADUCM3029_ADC_H
#define ADUCM3029_ADC_H

#include <stdint.h>


#define ADUCM3029_ADC_NUM_CH	6
#define ADUCM3029_CH(x) (1 << x)

struct adc_desc;

/**
 * @struct adc_init_param
 * @brief This can be extended in the future, no utility for the moment.
 */
struct adc_init_param {
	/* To add options in the future */
	uint32_t reserved;
};

/* Activate channels for reading */
int32_t aducm3029_adc_update_active_channels(struct adc_desc *desc,
		uint32_t mask);

/* Read adc channels */
int32_t aducm3029_adc_read(struct adc_desc *desc, uint16_t *buff,
			   uint32_t nb_samples);

/* Initialize the ADC */
int32_t aducm3029_adc_init(struct adc_desc **desc,
			   struct adc_init_param *param);

/* Free the resources allocated by adc_init(). */
int32_t aducm3029_adc_remove(struct adc_desc *desc);

#endif

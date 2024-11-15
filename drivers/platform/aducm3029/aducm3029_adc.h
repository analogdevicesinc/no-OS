/***************************************************************************//**
 *   @file   aducm3029_adc.h
 *   @brief  Interface of aducm3029_adc.c
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

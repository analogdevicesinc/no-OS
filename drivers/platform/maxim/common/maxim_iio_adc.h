/***************************************************************************//**
 *   @file   maxim_iio_adc.h
 *   @brief  The Maxim on-chip ADC as a libiio table device.
 *   @author Radu Ciobanu (radu-rares.ciobanu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_IIO_ADC_H_
#define MAXIM_IIO_ADC_H_

struct noos_iio_device_info;

/*
 * The part's own ADC as a libiio table device, sampling as fast as it
 * converts: one row of the project's device table, NOOS_IIOD_ONCHIP_ADC().
 */
int noos_onchip_adc_init(const void *param);
int noos_onchip_adc_get_device_info(struct noos_iio_device_info *info);

#define NOOS_IIOD_ONCHIP_ADC() \
	{ \
		.init = noos_onchip_adc_init, \
		.get_info = noos_onchip_adc_get_device_info, \
	}

#endif /* MAXIM_IIO_ADC_H_ */

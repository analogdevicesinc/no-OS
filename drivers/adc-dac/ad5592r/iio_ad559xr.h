/***************************************************************************//**
 *   @file   iio_ad559xr.h
 *   @brief  Header file implementation of AD5592R/AD5593R IIO driver.
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/
#ifndef __IIO_AD559XR_H__
#define __IIO_AD559XR_H__
#include "ad5592r.h"
#include "ad5593r.h"
#include "iio_app.h"

#define AD559XR_MAX_AIO_PORT 	 	8

#define AD559XR_MAX_PORT_NAMELEN 	24

#define AD559XR_ADC_RESOLUTION 		12
#define AD559XR_STORAGE_BITS 		16
#define AD559XR_STORAGE_BYTES 		(AD559XR_STORAGE_BITS/8)

#define AD559XR_FULL_SCALE_VALUE 	((1 << AD559XR_ADC_RESOLUTION) - 1)

/* AD559xr vref range is from 1000 to VDD (1v up to 5v5)
 * but has an internal VRef of 2500 mV (2v5)
 */
#define AD559XR_MIN_VDD 	1.0f
#define AD559XR_MAX_VDD 	5.5f
#define AD559XR_INTERNAL_VREF 	2.5f

#define AD5599XR_MAX_SAMPLE_SIZE 	(1024)

#define AD559XR_SLOPE_CONSTANT 	 	(3767897513)
#define AD559XR_SLOPE_NANO_SCALE 	1000000000

enum {
	AD559XR_AIO_RAW,
	AD559XR_AIO_SCALE,
	AD559XR_AIO_OFFSET,
	AD559XR_DAC_POWER_DOWN,
	AD559XR_GPIO_LABEL,
	AD559XR_GPIO_RAW,

	AD559XR_TEMP_RAW,
	AD559XR_TEMP_SCALE,
	AD559XR_TEMP_OFFSET,

	/* global attributes */
	AD559XR_ADC_RANGE,
	AD559XR_ADC_BUFFER,
	AD559XR_DAC_RANGE,
	AD559XR_REF_SELECT,
	AD559XR_VREF_IN_V,
	AD559XR_LDAC_MODE,
	AD559XR_REPETITION,
};

struct ad559xr_aio_port {
	char name[AD559XR_MAX_PORT_NAMELEN];
	struct ad5592r_dev *ad5592r;
	uint8_t number;
	uint8_t mode;
	uint8_t gpio_map;
	struct iio_channel *chptr; /* so we can access the scan_index */
};

struct iio_ad559xr_desc {
	/* vref is set to AD559XR_INTERNAL_VREF when corresponding
	 * ad559xr device is using internal VREF. but when using external,
	 * it will be set to whatever the external_vref field is set. */
	float vref;
	/* external vref is for ad559xr devices that are not sourcing from
	 * the internal chip VREF. */
	float external_vref;
	bool repeat;
	struct no_os_list_desc *aio_ports;
	struct no_os_iterator *aio_ports_iter;
	size_t port_count;
	struct iio_device iiodev;
	struct iio_channel *channels;
	uint8_t *raw_buffer;
	size_t raw_buffer_size;
};

struct iio_ad559xr_gpio_map {
	struct ad5592r_dev *ad559xr;

	/* this is only relevant for pins which are GPIO. look into your
	 * init_param.channel_modes[x] and match the index value you want
	 * that particular GPIO to have */
	uint8_t map[AD559XR_MAX_AIO_PORT];
};

int iio_ad559xr_aio_init(struct iio_ad559xr_desc **desc,
			 struct ad5592r_dev *ad5592r);
int iio_ad559xr_gpio_init(struct iio_ad559xr_desc **desc,
			  struct iio_ad559xr_gpio_map *gpio_map,
			  const size_t count);
int iio_ad559xr_remove(struct iio_ad559xr_desc *desc);

#endif /* __IIO_AD559XR_H__ */

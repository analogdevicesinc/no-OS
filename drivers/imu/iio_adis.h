/***************************************************************************//**
 *   @file   iio_adis.h
 *   @brief  Implementation of iio_adis.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
 ******************************************************************************/

#ifndef IIO_ADIS_H
#define IIO_ADIS_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio.h"
#include <errno.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum adis16505_chan_type {
	ADIS_GYRO_X,
	ADIS_GYRO_Y,
	ADIS_GYRO_Z,
	ADIS_ACCEL_X,
	ADIS_ACCEL_Y,
	ADIS_ACCEL_Z,
	ADIS_TEMP,
	ADIS_DELTA_ANGL_X,
	ADIS_DELTA_ANGL_Y,
	ADIS_DELTA_ANGL_Z,
	ADIS_DELTA_VEL_X,
	ADIS_DELTA_VEL_Y,
	ADIS_DELTA_VEL_Z,
};

struct adis_iio_dev {
	struct adis_dev *adis_dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS_ACCEL_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_ACCEL, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_accel_scan_type,  \
        .attributes = adis_iio_accel_attrs,  \
}

#define ADIS_GYRO_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_ANGL_VEL, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_anglvel_scan_type,  \
        .attributes = adis_iio_anglvel_attrs,  \
}

#define ADIS_DELTA_ANGL_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_ROT, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_delta_angl_scan_type,  \
        .attributes = adis_iio_delta_angl_attrs,  \
}

#define ADIS_DELTA_VEL_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_VELOCITY, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_delta_vel_scan_type,  \
        .attributes = adis_iio_delta_vel_attrs,  \
}

#define ADIS_TEMP_CHAN(idx, adis_nb) { \
	.ch_type = IIO_TEMP, \
	.channel = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_temp_scan_type,  \
        .attributes = adis_iio_temp_attrs,  \
}

/******************************************************************************/
/************************ Variables Declarations ******************************/
/******************************************************************************/
extern struct iio_attribute adis_dev_attrs[];
extern struct iio_attribute adis_iio_anglvel_attrs[];
extern struct iio_attribute adis_iio_delta_angl_attrs[];
extern struct iio_attribute adis_iio_delta_vel_attrs[];
extern struct iio_attribute adis_debug_attrs[];
extern struct iio_attribute adis_iio_accel_attrs[];
extern struct iio_attribute adis_iio_temp_attrs[];
extern struct iio_trigger adis_iio_trig_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Update adis iio active channels. */
int adis_iio_update_channels(void* dev, uint32_t mask);
/*! Read adis iio samples for the active channels. */
int adis_iio_read_samples(void* dev, int* buff, uint32_t samples);
/*! Callback for adis iio data ready trigger. */
int adis_iio_trigger_handler(struct iio_device_data *dev_data);

#endif

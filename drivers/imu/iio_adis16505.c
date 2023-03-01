/***************************************************************************//**
 *   @file   iio_adis16505.c
 *   @brief  Implementation of iio_adis16505.c
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_adis16505.h"
#include "iio_adis.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include <stdio.h>

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static struct scan_type adis16505_iio_accel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= false
};

static struct scan_type adis16505_iio_anglvel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= false
};

static struct scan_type adis16505_iio_delta_vel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= false
};

static struct scan_type adis16505_iio_delta_angl_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= false
};

static struct scan_type adis16505_iio_temp_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= false
};

static struct iio_channel adis16505_channels[] = {
	ADIS_GYRO_CHAN		(X, 	ADIS_GYRO_X, 		16505),
	ADIS_GYRO_CHAN		(Y, 	ADIS_GYRO_Y, 		16505),
	ADIS_GYRO_CHAN		(Z, 	ADIS_GYRO_Z, 		16505),
	ADIS_ACCEL_CHAN		(X,	ADIS_ACCEL_X, 		16505),
	ADIS_ACCEL_CHAN		(Y,	ADIS_ACCEL_Y, 		16505),
	ADIS_ACCEL_CHAN		(Z,	ADIS_ACCEL_Z, 		16505),
	ADIS_TEMP_CHAN		(ADIS_TEMP, 			16505),
	ADIS_DELTA_ANGL_CHAN	(X, 	ADIS_DELTA_ANGL_X, 	16505),
	ADIS_DELTA_ANGL_CHAN	(Y, 	ADIS_DELTA_ANGL_Y, 	16505),
	ADIS_DELTA_ANGL_CHAN	(Z, 	ADIS_DELTA_ANGL_Z, 	16505),
	ADIS_DELTA_VEL_CHAN	(X, 	ADIS_DELTA_VEL_X, 	16505),
	ADIS_DELTA_VEL_CHAN	(Y, 	ADIS_DELTA_VEL_Y, 	16505),
	ADIS_DELTA_VEL_CHAN	(Z, 	ADIS_DELTA_VEL_Z, 	16505),
};

static struct iio_device adis16505_iio_dev = {
	.num_ch 		= NO_OS_ARRAY_SIZE(adis16505_channels),
	.channels 		= adis16505_channels,
	.debug_attributes 	= adis_debug_attrs,
	.attributes		= adis_dev_attrs,
	.pre_enable 		= (int32_t (*)())adis_iio_update_channels,
	.read_dev 		= (int32_t (*)())adis_iio_read_samples,
	.trigger_handler 	= (int32_t (*)())adis_iio_trigger_handler,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize adis16505 iio device.
 * @param iio_dev    - The adis16505 iio device.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, error code otherwise.
 */
int adis16505_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param)
{
	int ret;
	struct adis_iio_dev *desc;

	desc = (struct adis_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adis16505_iio_dev;

	adis16505_data.adis_ip = init_param;

	ret = adis_init(&desc->adis_dev, &adis16505_data);
	if (ret)
		goto error_adis16505_init;

	*iio_dev = desc;

	return 0;

error_adis16505_init:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Remove adis16505 iio device.
 * @param desc - The adis16505 iio device.
 */
void adis16505_iio_remove(struct adis_iio_dev *desc)
{
	adis_remove(desc->adis_dev);
	no_os_free(desc);
}

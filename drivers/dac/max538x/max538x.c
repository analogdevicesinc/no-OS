/***************************************************************************//**
 *   @file   max538x.c
 *   @brief  Implementation of max538x Family Driver.
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
********************************************************************************
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include "max538x.h"
#include "no_os_i2c.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

const struct max538x_chip_info chip_info[] = {
	[MAX5380L] = {
		.vfactor = 2,
		.addr = 0x30,
	},
	[MAX5380M] = {
		.vfactor = 2,
		.addr = 0x31,
	},
	[MAX5380N] = {
		.vfactor = 2,
		.addr = 0x32,
	},
	[MAX5380K] = {
		.vfactor = 2,
		.addr = 0x33,
	},
	[MAX5381L] = {
		.vfactor = 4,
		.addr = 0x30,
	},
	[MAX5381M] = {
		.vfactor = 4,
		.addr = 0x31,
	},
	[MAX5381N] = {
		.vfactor = 4,
		.addr = 0x32,
	},
	[MAX5381K] = {
		.vfactor = 4,
		.addr = 0x33,
	},
	[MAX5382L] = {
		.vfactor = 0.9,
		.addr = 0x30,
	},
	[MAX5382M] = {
		.vfactor = 0.9,
		.addr = 0x31,
	},
	[MAX5382N] = {
		.vfactor = 0.9,
		.addr = 0x32,
	},
	[MAX5382K] = {
		.vfactor = 0.9,
		.addr = 0x33,
	},
};

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the device is
 *        present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret - The result of the initialization procedure.
 *                  Example: -1 - I2C peripheral wasn't initialized or the
 *                                device is not present.
 *                            0 - I2C peripheral was initialized and the
 *                                device is present.
*******************************************************************************/
int max538x_init(struct max538x_dev **device,
		 struct max538x_init_param init_param)
{
	int ret;
	struct max538x_dev *dev;

	if ((init_param.max538x_vdd == NULL || init_param.max538x_vdd == 0)
	    && init_param.active_device > MAX5381K)
		return -EINVAL;

	dev = (struct max538x_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->active_device = init_param.active_device;
	init_param.i2c_init.slave_address = chip_info[init_param.active_device].addr;

	if (dev->active_device <= MAX5381K)
		dev->max538x_vref = chip_info[init_param.active_device].vfactor;
	else
		dev->max538x_vref = chip_info[init_param.active_device].vfactor *
				    init_param.max538x_vdd;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
	if (ret)
		goto error_dev;

	*device = dev;

	return 0;

error_dev:
	no_os_free(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by max538x_init().
 * @param dev - The device structure.
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int max538x_remove(struct max538x_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Sets the DAC output voltage using vout
 *
 * @param dev - The device structure.
 * @param vout - voltage output to set.
 * @return ret - 0 if no error, else return error code of I2C transaction
*******************************************************************************/
int max538x_set_voutput(struct max538x_dev *dev, float vout)
{
	uint8_t vout_code;

	if (vout < 0 || vout > dev->max538x_vref)
		return -EINVAL;

	vout_code = (uint8_t)(NO_OS_DIV_U64(vout,
					    dev->max538x_vref) * MAX538X_RESOLUTION);

	return no_os_i2c_write(dev->i2c_desc, &vout_code, 1, 1); //no repeat start
}

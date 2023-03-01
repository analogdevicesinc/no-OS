/***************************************************************************//**
 *   @file   max31855.c
 *   @brief  Implementation of MAX31855 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include "max31855.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Device and comm init function
 * @param device - MAX31855 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max31855_init(struct max31855_dev **device,
		  struct max31855_init_param *init_param)
{
	int ret;
	struct max31855_dev *descriptor;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, &init_param->spi_init);
	if (ret)
		goto spi_err;

	*device = descriptor;

	return 0;
spi_err:
	no_os_free(descriptor);

	return ret;
}
/**
 * @brief Remove resources allocated by the init function
 * @param device - MAX31855 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max31855_remove(struct max31855_dev *device)
{
	int ret;
	if (!device)
		return -ENODEV;

	ret = no_os_spi_remove(device->comm_desc);
	if (ret)
		return -EINVAL;

	no_os_free(device);

	return 0;
}

/**
 * @brief Read raw register value
 * @param device - MAX31855 descriptor
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int max31855_read_raw(struct max31855_dev *device, uint32_t *val)
{
	int ret;
	uint8_t raw_array[4];

	ret = no_os_spi_write_and_read(device->comm_desc, raw_array, 4);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be32(raw_array);
	device->fault.value = MAX31855_GET_FAULT_BIT(*val);
	if (MAX31855_GET_FAULT_BIT(*val)) {
		device->fault.value = MAX31855_GET_FAULTS(*val);
		return -1;
	}

	return 0;
}

/**
 * @brief Read thermocouple and internal temperatures (converted in deg. C)
 * @param device - MAX31855 descriptor
 * @param thermocouple_temp - thermocouple probe temperature (deg. C)
 * @param internal_temp - board's temperature (used as reference)
 * @return 0 in case of success, errno errors otherwise
 */
int max31855_read_temp(struct max31855_dev *device,
		       struct max31855_decimal *thermocouple_temp,
		       struct max31855_decimal *internal_temp)
{
	int ret;
	uint32_t buff;
	uint32_t i_temp;
	uint32_t t_temp;

	if (!device)
		return -ENODEV;

	ret = max31855_read_raw(device, &buff);
	if (ret)
		return ret;

	i_temp = MAX31855_GET_INTERNAL_TEMP(buff);
	t_temp = MAX31855_GET_THERMOCOUPLE_TEMP(buff);
	i_temp = no_os_sign_extend32(i_temp, MAX31855_INTERNAL_TEMP_SIGN_POS);
	t_temp = no_os_sign_extend32(t_temp, MAX31855_THERMOCOUPLE_TEMP_SIGN_POS);

	thermocouple_temp->integer = no_os_div_s64_rem(t_temp,
				     MAX31855_THERMOCOUPLE_TEMP_DEC_DIV,
				     &thermocouple_temp->decimal);
	internal_temp->integer = no_os_div_s64_rem(i_temp,
				 MAX31855_INTERNAL_TEMP_DEC_DIV,
				 &internal_temp->decimal);

	return 0;
}

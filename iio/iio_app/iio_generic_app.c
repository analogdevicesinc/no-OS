/***************************************************************************//**
 *   @file   iio_generic_app.c
 *   @brief  Implementation of iio_generic_app.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include "error.h"
#include "iio.h"
#include "iio_generic_app.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Application init for reading/writing and parameterization of a
 * generic device.
 * @param desc - Application descriptor.
 * @param init - Application configuration structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_generic_app_init(struct iio_generic_app_desc **desc,
			     struct iio_interface_init_par *init)
{
	int32_t status;

	status = iio_register(init);
	if(status < 0)
		return FAILURE;

	*desc = calloc(1, sizeof(struct iio_generic_app_desc));
	if (!(*desc))
		return FAILURE;

	(*desc)->dev_name = init->dev_name;

	return SUCCESS;
}

/**
 * @brief Release resources.
 * @param desc - Application descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_generic_app_remove(struct iio_generic_app_desc *desc)
{
	int32_t status;

	if (!desc)
		return FAILURE;

	status = iio_unregister(desc->dev_name);
	if(status < 0)
		return FAILURE;

	free(desc);

	return SUCCESS;
}

/***************************************************************************//**
 *   @file   mbed_ain.cpp
 *   @brief  Implementation of Mbed specific analog input functionality
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
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

#include <mbed.h>

// Platform drivers needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif //  _cplusplus

#include "no_os_ain.h"
#include "mbed_ain_aout.h"
#include "no_os_error.h"

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read voltage from analog input pin.
 * @param desc[in] - Analog input descriptor.
 * @param value[in,out] - Analog input value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_ain_get_voltage(struct no_os_ain_desc *desc, float *value)
{
	/* Pointer to analog input object */
	mbed::AnalogIn *analog_input;

	if (!desc || !desc->extra || !value)
		return -EINVAL;

	analog_input = (AnalogIn *)((struct mbed_ain_desc *)
				    desc->extra)->ain_obj;
	*value = analog_input->read_voltage();

	return 0;
}

/**
 * @brief Initialize the Analog Input Pin.
 * @param desc[in, out] - Analog input descriptor structure.
 * @param param[in] - Structure that contains analog input
 *                    initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_ain_init(struct no_os_ain_desc **desc,
		       const struct no_os_ain_init_param *param)
{
	mbed::AnalogIn *analog_input;
	struct no_os_ain_desc *noos_ain_desc;
	struct mbed_ain_desc *mbed_analog_in_desc;

	if (!desc || !param)
		return -EINVAL;

	/* Allocate memory for general analog input descriptor */
	noos_ain_desc = (struct no_os_ain_desc *)calloc(1, sizeof(*noos_ain_desc));
	if (!noos_ain_desc)
		return -ENOMEM;

	/* Configure pin number and reference voltage*/
	noos_ain_desc->vref = param->vref;

	/* Allocate memory for mbed specific analog input descriptor
	 * for future use */
	mbed_analog_in_desc = (struct mbed_ain_desc *)calloc(1, sizeof(
				      *mbed_analog_in_desc));
	if (!mbed_analog_in_desc)
		goto err_mbed_ain_desc;

	if (((struct mbed_ain_init_param *)param->extra)->number) {
		/* Create and initialize mbed analog input object */
		analog_input = new AnalogIn((PinName)((struct mbed_ain_init_param *)
						      param->extra)->number, param->vref);
		if (!analog_input)
			goto err_ain_obj;
	} else {
		goto err_ain_pin;
	}

	mbed_analog_in_desc->ain_obj = analog_input;
	noos_ain_desc->extra = mbed_analog_in_desc;
	*desc = noos_ain_desc;
	return 0;


err_ain_obj:
	free(analog_input);
err_ain_pin:
	free(mbed_analog_in_desc);
err_mbed_ain_desc:
	free(noos_ain_desc);

	return -ENOMEM;
}

/**
 * @brief Deallocate resources allocated by no_os_ain_init().
 * @param desc[in, out] - Analog input descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_ain_remove(struct no_os_ain_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	if (((struct mbed_ain_desc *)desc->extra)->ain_obj)
		delete((mbed::AnalogIn *)((struct mbed_ain_desc *)desc->
					  extra)->ain_obj);

	if ((struct mbed_ain_desc *)desc->extra)
		free((struct mbed_ain_desc *)desc->extra);

	free(desc);

	return 0;
}
#ifdef __cplusplus
}
#endif //  _cplusplus

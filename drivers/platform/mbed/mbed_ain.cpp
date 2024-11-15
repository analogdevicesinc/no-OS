/***************************************************************************//**
 *   @file   mbed_ain.cpp
 *   @brief  Implementation of Mbed specific analog input functionality
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

/***************************************************************************//**
 *   @file   mbed_aout.cpp
 *   @brief  Implementation of Mbed specific analog output functionality
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

#include "no_os_aout.h"
#include "no_os_error.h"
#include "mbed_ain_aout.h"

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
 * @brief Write voltage to analog output pin.
 * @param desc[in] - Analog output descriptor.
 * @param value[in] - Analog output value in volts.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_aout_set_voltage(struct no_os_aout_desc *desc, float value)
{
	mbed::AnalogOut *analog_output; // Pointer to analog output object
	float volt_mapped;	// Analog voltage

	if (!desc || !desc->extra)
		return -EINVAL;

	/* Mbed AnalogOut APIs accept voltage in range from 0.0f
	 * (representing DAC min output range / 0%) to 1.0f (representing
	 * DAC max output range / 100%). Hence we need to map the voltage levels
	 * before passing it to mbed APIs */
	if (value >= desc->aout_min_v && value <= desc->aout_max_v) {
		volt_mapped = ((value - desc->aout_min_v) /
			       (desc->aout_max_v - desc->aout_min_v));
	} else {
		return -EINVAL;
	}

	analog_output = (AnalogOut *)((struct mbed_aout_desc *)
				      desc->extra)->aout_obj;
	analog_output->write(volt_mapped);

	return 0;
}

/**
 * @brief Initialize the analog output pin.
 * @param desc[in, out] - Analog output descriptor structure.
 * @param param[in] - Structure that contains analog output
 *                    initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_mbed_aout_init(struct no_os_aout_desc **desc,
			     const struct no_os_aout_init_param *param)
{
	mbed::AnalogOut *analog_output;
	struct no_os_aout_desc *noos_aout_desc;
	struct mbed_aout_desc *mbed_analog_out_desc;

	if (!desc || !param)
		return -EINVAL;

	/* Allocate memory for general analog output descriptor */
	noos_aout_desc = (struct no_os_aout_desc *) calloc(1,
			 sizeof(*noos_aout_desc));
	if (!noos_aout_desc)
		return -ENOMEM;

	/* Configure pin number,DAC max and min range */
	noos_aout_desc->aout_min_v = param->aout_min_v;
	noos_aout_desc->aout_max_v = param->aout_max_v;

	/* Allocate memory for mbed specific analog output descriptor
	* for future use */
	mbed_analog_out_desc = (struct mbed_aout_desc *) calloc(1,
			       sizeof(*mbed_analog_out_desc));
	if (!mbed_analog_out_desc)
		goto err_mbed_analog_out_desc;

	if (((struct mbed_aout_init_param *)param->extra)->number) {
		/* Create and initialize mbed analog output object */
		analog_output = new AnalogOut((PinName)((struct mbed_aout_init_param *)
							param->extra)->number);
		if (!analog_output)
			goto err_aout_obj;
	} else {
		goto err_analog_pin;
	}

	mbed_analog_out_desc->aout_obj = analog_output;
	noos_aout_desc->extra = mbed_analog_out_desc;
	*desc = noos_aout_desc;
	return 0;

err_aout_obj:
	free(analog_output);
err_analog_pin:
	free(mbed_analog_out_desc);
err_mbed_analog_out_desc:
	free(noos_aout_desc);

	return -ENOMEM;
}

/**
 * @brief Deallocate resources allocated by mbed_aout_init().
 * @param desc[in, out] - Analog output descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_aout_remove(struct no_os_aout_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	if (((struct mbed_aout_desc *)desc->extra)->aout_obj)
		delete((mbed::AnalogOut *)((struct mbed_aout_desc *)
					   desc->extra)->aout_obj);

	if ((struct mbed_aout_desc *)desc->extra)
		free((struct mbed_aout_desc *)desc->extra);

	free(desc);

	return 0;
}

#ifdef __cplusplus
}
#endif //  _cplusplus

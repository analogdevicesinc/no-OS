/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADE7978 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "platform.h"

/**
 * @brief Saves the current and voltage values of device 1 in rms_adc structure
 * @param dev - device structure
 * @param value - structure holding the measurements values
 * @param phase - selects the phase for measurements read
 * @return 0 in case of success, negative error code otherwise
 */
int rms_adc_values_read(struct ade7978_dev *dev, struct measurements *value,
			enum ade7978_phase phase)
{
	int ret;
	/* variables used to calculate the values of the measurements */
	float v1_rms, v2_rms, i_rms, temperature;

	if (!dev)
		return -ENODEV;
	if (!value)
		return -EINVAL;

	/* Read the measurements for the selected phase */
	ret = ade7978_read_data_ph(dev, phase);
	if (ret)
		return ret;

	/* Compute v1 value in mV */
	v1_rms = (float) ADE7978_FS_VOLTAGE_RMS * (float)((int32_t) dev->vrms_val) *
		 (float) ADE7978_VOLTAGE_TR_FCN / ((float) ADE7978_WAVE_FS_CODES *
				 (float) 10);

	/* Compute i value in mA */
	i_rms = (float) ADE7978_FS_CURRENT_RMS * (float)((int32_t) dev->irms_val) *
		(float) ADE7978_SHUNT_RES / (float) ADE7978_WAVE_FS_CODES;

	/* Save the values in the measurements structure */
	value->v1_rms = v1_rms;
	value->v1_rms_adc = dev->vrms_val;
	value->i_rms = i_rms;
	value->i_rms_adc = dev->irms_val;

	/* The second voltage channel is multiplexed with the temperature sensor.
	The channel function is selected by the user through temp_en */
	if (dev->temp_en) {
		/* Compute temperature in °C */
		temperature = 8.72101 * 0.00001 * dev->temperature - 306.47;
		/* Save the temperature values in the measurements structure */
		value->temperature_c = temperature;
		value->temperature  = dev->temperature;
	} else {
		/* Compute v2 value */
		v2_rms = (float) ADE7978_FS_VOLTAGE_RMS * (float)((int32_t) dev->v2rms_val) *
			 (float) ADE7978_VOLTAGE_TR_FCN / ((float) ADE7978_WAVE_FS_CODES *
					 (float) 10);
		/* Save the v2 values in the measurements structure */
		value->v2_rms = v2_rms;
		value->v2_rms_adc = dev->v2rms_val;
	}

	return 0;
}

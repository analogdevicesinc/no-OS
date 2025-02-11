/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADE7753 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
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
*******************************************************************************/

#include "common_data.h"

/**
 * @brief Read measurements
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int read_measurements(struct ade7753_dev *dev)
{
	int ret;
	struct ade7753_energy_values energy_vals;
	struct ade7753_rms_values rms_vals;
	struct ade7753_period_value period_val;
	float active_energy, apparent_energy;
	float current_rms_val, voltage_rms_val;
	float frequency_val;
	float temperature_C;
	int32_t status, temperature;

	/* read energy values */
	ret = ade7753_energy_vals(dev, &energy_vals);
	if (ret)
		return ret;

	active_energy = (float)energy_vals.active_energy_reg_val;
	apparent_energy = (float)energy_vals.apparent_energy_reg_val;

	/* read rms values */
	ret = ade7753_rms_vals(dev, &rms_vals);
	if (ret)
		return ret;

	/* value in mA */
	current_rms_val = ((float)(rms_vals.current_rms_reg_val) /
			   CURRENT_RMS_FS_CODE) * ADC_FS_RMS_IN * I_GAIN;
	/* value in mV */
	voltage_rms_val = ((float)rms_vals.voltage_rms_reg_val /
			   VOLTAGE_RMS_FS_CODE) * ADC_FS_RMS_IN * V_GAIN;

	pr_info("I rms: %.2f mA \n", current_rms_val);

	pr_info("V rms: %.2f mV \n", voltage_rms_val);

	/* read temperature */
	ret = ade7753_get_int_status(dev, ADE7753_TEMP_MSK, status);
	if (ret)
		return ret;
	if (status) {
		ret = ade7753_read(dev, ADE7753_REG_TEMP, &temperature);
		if (ret)
			return ret;

		temperature_C = (float)temperature / (float)TEMP_G;

		pr_info("Temperature %.2f °C \n", temperature_C);
	}

	/* read period value */
	ret = ade7753_period_val(dev, &period_val);
	if (ret)
		return ret;

	frequency_val = (float)FREQ_CC / (float)(period_val.period_reg_val + 1);

	pr_info("Frequency %.2f Hz \n", frequency_val);

	pr_info("\n");
	pr_info("\n");
	pr_info("\n");

	return 0;
}

/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADE7953 example project
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
 * @param channel - Selects the current channel that is read
 * @return 0 in case of success, negative error code otherwise.
 */
int read_measurements(struct ade7953_dev *dev, enum ade7953_i_ch_e channel)
{
	int ret;
	struct ade7953_energy_values energy_vals;
	struct ade7953_power_values power_vals;
	struct ade7953_rms_values rms_vals;
	struct ade7953_pq_values pq_vals;
	float active_energy, reactive_energy, apparent_energy;
	float active_power, reactive_power, apparent_power;
	float currentA_rms_val, currentB_rms_val, voltage_rms_val;
	float power_factor_val, frequency_val;

	/* read energy values */
	ret = ade7953_energy_vals(dev, &energy_vals, channel);
	if (ret)
		return ret;

	active_energy = (float)energy_vals.active_energy_reg_val;
	reactive_energy = (float)energy_vals.fundamental_reactive_energy_reg_val;
	apparent_energy = (float)energy_vals.apparent_energy_reg_val;

	/* read power values */
	ret = ade7953_power_vals(dev, &power_vals, channel);
	if (ret)
		return ret;

	active_power = ((float)power_vals.active_power_reg_val / POWER_ADC_FS_CODE)
		       * POWER_GAIN;
	reactive_power = ((float)power_vals.reactive_power_reg_val / POWER_ADC_FS_CODE)
			 * POWER_GAIN;
	apparent_power = ((float)power_vals.apparent_power_reg_val / POWER_ADC_FS_CODE)
			 * POWER_GAIN;

	pr_info("Active power: %.2f W \n", active_power);
	pr_info("Fundamental reactive power: %.2f VAR \n", reactive_power);
	pr_info("Apparent power: %.2f VA \n", apparent_power);

	/* read rms values */
	ret = ade7953_rms_vals(dev, &rms_vals, channel);
	if (ret)
		return ret;

	/* value in mA */
	currentA_rms_val = (((float)(rms_vals.current_chA_rms_reg_val) /
			     CURRENT_CHA_ADC_FS_CODE)
			    * ADC_FS_RMS_IN) * I_GAIN;
	/* value in mA */
	currentB_rms_val = (((float)(rms_vals.current_chB_rms_reg_val) /
			     CURRENT_CHB_ADC_FS_CODE)
			    * ADC_FS_RMS_IN) * I_GAIN;
	/* value in mV */
	voltage_rms_val = (((float)rms_vals.voltage_rms_reg_val / VOLTAGE_ADC_FS_CODE)
			   * ADC_FS_RMS_IN) * V_GAIN;

	if (channel == ADE7953_I_CH1)
		pr_info("IA rms: %.2f mA \n", currentA_rms_val);
	else
		pr_info("IB rms: %.2f mA \n", currentB_rms_val);

	pr_info("V rms: %.2f mV \n", voltage_rms_val);

	/* read power quality values */
	ret = ade7953_power_quality_vals(dev, &pq_vals);
	if (ret)
		return ret;

	power_factor_val = (float)pq_vals.power_factor_reg_val * PF_CC;
	frequency_val = (float)(FREQ_CC) / (float)(pq_vals.period_reg_val + 1);

	pr_info("Power factor: %.2f \n", power_factor_val);
	pr_info("Frequency %.2f Hz \n", frequency_val);

	pr_info("\n");
	pr_info("\n");
	pr_info("\n");

	return 0;
}

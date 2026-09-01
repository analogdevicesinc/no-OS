/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADE7758 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

/**
 * @brief Read measurements
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int read_measurements(struct ade7758_dev *dev)
{
	int ret;
	struct ade7758_energy_values energy_vals;
	struct ade7758_rms_values rms_vals;
	struct ade7758_freq_per_value freq_val;
	int32_t status;

	/* read energy values */
	if (no_os_test_bit(no_os_find_first_set_bit(ADE7758_ZXA_MSK),
			   &dev->irq_status)) {
		ret = ade7758_energy_vals_phase_a(dev, &energy_vals);
		if (ret)
			return ret;
		ret = ade7758_rms_vals_phase_a(dev, &rms_vals);
		if (ret)
			return ret;
	} else if (no_os_test_bit(no_os_find_first_set_bit(ADE7758_ZXB_MSK),
				  &dev->irq_status)) {
		ret = ade7758_energy_vals_phase_b(dev, &energy_vals);
		if (ret)
			return ret;
		ret = ade7758_rms_vals_phase_b(dev, &rms_vals);
		if (ret)
			return ret;
	} else if (no_os_test_bit(no_os_find_first_set_bit(ADE7758_ZXC_MSK),
				  &dev->irq_status)) {
		ret = ade7758_energy_vals_phase_c(dev, &energy_vals);
		if (ret)
			return ret;
		ret = ade7758_rms_vals_phase_c(dev, &rms_vals);
		if (ret)
			return ret;
	} else {
		ret = -EINVAL;
		return ret;
	}

	return 0;
}

/**
 * @brief print measurements
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int print_measurements(struct ade7758_dev *dev)
{
	int ret;
	struct ade7758_energy_values energy_vals;
	struct ade7758_rms_values rms_vals;
	struct ade7758_freq_per_value freq_val;
	float current_rms_val, voltage_rms_val;
	float frequency_val;
	float temperature_C;
	int32_t status, temperature;


	/* Phase A */
	current_rms_val = ((float)rms_vals.current_rms_reg_val_phase_a /
			   CURRENT_RMS_FS_CODE) * ADC_FS_RMS_IN * I_GAIN;
	/* value in mV */
	voltage_rms_val = ((float)rms_vals.voltage_rms_reg_val_phase_a /
			   VOLTAGE_RMS_FS_CODE) * ADC_FS_RMS_IN * V_GAIN;

	pr_info("I rms phase A: %.2f mA \n", current_rms_val);

	pr_info("V rms phase A: %.2f mV \n", voltage_rms_val);
	/* Phase B */
	current_rms_val = ((float)rms_vals.current_rms_reg_val_phase_b /
			   CURRENT_RMS_FS_CODE) * ADC_FS_RMS_IN * I_GAIN;
	/* value in mV */
	voltage_rms_val = ((float)rms_vals.voltage_rms_reg_val_phase_b /
			   VOLTAGE_RMS_FS_CODE) * ADC_FS_RMS_IN * V_GAIN;

	pr_info("I rms phase B: %.2f mA \n", current_rms_val);

	pr_info("V rms phase B: %.2f mV \n", voltage_rms_val);
	/* Phase C */
	current_rms_val = ((float)rms_vals.current_rms_reg_val_phase_c /
			   CURRENT_RMS_FS_CODE) * ADC_FS_RMS_IN * I_GAIN;
	/* value in mV */
	voltage_rms_val = ((float)rms_vals.voltage_rms_reg_val_phase_c /
			   VOLTAGE_RMS_FS_CODE) * ADC_FS_RMS_IN * V_GAIN;

	pr_info("I rms phase C: %.2f mA \n", current_rms_val);

	pr_info("V rms phase C: %.2f mV \n", voltage_rms_val);


	/* read & print temperature */
	ret = ade7758_read(dev, ADE7758_REG_TEMP, &temperature);
	if (ret)
		return ret;

	temperature_C = (((float)temperature - ADE7758_AMB_T_CODE) *
			 (float)TEMP_G) + ADE7758_AMB_T_CELSIUS;

	pr_info("Temperature %.2f °C \n", temperature_C);

	/* read & print freq value */
	ret = ade7758_frequency_period_val(dev, &freq_val);
	if (ret)
		return ret;

	frequency_val = (float)FREQ_CC * (float)freq_val.freq_per_reg_val;

	pr_info("Frequency %.2f Hz \n", frequency_val);

	pr_info("\n");
	pr_info("\n");
	pr_info("\n");

	return 0;
}


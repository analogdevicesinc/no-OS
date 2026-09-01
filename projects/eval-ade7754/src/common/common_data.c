/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADE7754 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

/**
 * @brief Read rms measurements
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int read_rms_measurements(struct ade7754_dev *dev)
{
	int ret;
	struct ade7754_rms_values rms_vals;
	int32_t status;

	/* read rms values */
	if (no_os_test_bit(no_os_find_first_set_bit(ADE7754_ZXA_MSK),
			   &dev->irq_status)) {
		ret = ade7754_rms_vals_phase_a(dev, &rms_vals);
		if (ret)
			return ret;
	} else if (no_os_test_bit(no_os_find_first_set_bit(ADE7754_ZXB_MSK),
				  &dev->irq_status)) {
		ret = ade7754_rms_vals_phase_b(dev, &rms_vals);
		if (ret)
			return ret;
	} else if (no_os_test_bit(no_os_find_first_set_bit(ADE7754_ZXC_MSK),
				  &dev->irq_status)) {
		ret = ade7754_rms_vals_phase_c(dev, &rms_vals);
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
int print_measurements(struct ade7754_dev *dev)
{
	int ret;
	struct ade7754_energy_values energy_vals;
	struct ade7754_rms_values rms_vals;
	struct ade7754_period_value period_val;
	struct ade7754_temp_value temp_val;
	float current_rms_val, voltage_rms_val;
	float period_value;
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
	ret = ade7754_temperature_val(dev, &temp_val);
	if (ret)
		return ret;

	temperature_C = ((float)temp_val.temp_reg_val * (float)TEMP_G) +
			ADE7754_AMB_T_CELSIUS;

	pr_info("Temperature %.2f °C \n", temperature_C);

	/* read & print period value */
	ret = ade7754_period_val(dev, &period_val);
	if (ret)
		return ret;

	period_value = ((float)PERIOD_RES * (float)period_val.per_reg_val) / 1000;

	pr_info("Period %.2f ms \n", period_value);

	pr_info("\n");
	pr_info("\n");
	pr_info("\n");

	return 0;
}

/**
 * @brief Toggle Led
 * @param gpio_led_desc - led descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc)
{
	uint8_t val;
	int ret;

	ret = no_os_gpio_get_value(gpio_led_desc, &val);
	if (ret)
		return ret;

	return no_os_gpio_set_value(gpio_led_desc, !val);
}

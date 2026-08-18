/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE9039 example.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdio.h>
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "ade9039.h"
#include "platform.h"

#define POWER_MAX_VALUE 20694066
#define ENERGY_ACCUMULATION_FACTOR 3515.625f
#define NOMINAL_FREQ 50

float convert_power_type(int32_t power, float scale)
{
	float real_power;
	real_power = power * scale / ((float)POWER_MAX_VALUE);
	real_power = real_power / 2.0f;

	return real_power;
}

float convert_energy_type(int32_t energy_hi, float scale)
{
	return (convert_power_type(energy_hi,
				   scale) * 2.0f) / ENERGY_ACCUMULATION_FACTOR;
}

int main(void)
{
	int ret;
	/* power values */
	float watt_conv, va_conv, fwatt_conv, fvar_conv, fva_conv;
	/* energy values */
	float watthr_conv, varhr_conv, fwatthr_conv, fvarhr_conv, fvarahr_conv;
	/* power factor values */
	float pf_conv, fpf_conv;
	/* rms values */
	uint32_t v_rms_val;
	int32_t i_rms_val;
	/* status0 value */
	uint32_t status;
	/* update flags */
	uint8_t update_pf = 0, update_eng = 0, update_temp = 0;
	/* temperature */
	int32_t temp_deg;
	uint32_t cnt = 0;

	/* parameters initialization structure */
	struct ade9039_init_param ade9039_ip;
	/* device structure */
	struct ade9039_dev *ade9039_dev;

	/* uart descriptor */
	struct no_os_uart_desc *uart_desc;
	/* psm0 descriptor */
	struct no_os_gpio_desc *psm0_desc;
	/* psm1 descriptor */
	struct no_os_gpio_desc *psm1_desc;
	/* reset descriptor */
	struct no_os_gpio_desc *reset_desc;
	/* gpio descriptor */
	struct no_os_gpio_desc *gpio_desc;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get_optional(&gpio_desc,
				      &gpio_led1_ip);
	if (ret)
		pr_err("Failed to get GPIO descriptor for LED with err code: %d\n", ret);

	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		pr_err("Failed to set GPIO direction for LED with err code: %d\n", ret);

	ret = no_os_gpio_get_optional(&psm0_desc,
				      &gpio_psm0_ip);
	if (ret)
		goto remove_led;


	ret = no_os_gpio_get_optional(&psm1_desc,
				      &gpio_psm1_ip);
	if (ret)
		goto remove_psm0;

	if (psm1_desc)
		ret = no_os_gpio_direction_output(psm1_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_psm1;

	if (psm0_desc)
		ret = no_os_gpio_direction_output(psm0_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_psm1;

	ret = no_os_gpio_get_optional(&reset_desc,
				      &gpio_reset_ip);
	if (ret)
		goto remove_psm1;

	if (reset_desc)
		ret = no_os_gpio_direction_output(reset_desc,
						  NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_reset;

	/* Initialize SPI */
	ade9039_ip.spi_init = &ade9039_spi_ip;
	/* Initialize PSM0 */
	ade9039_ip.psm0_desc = psm0_desc;
	/* Initialize PSM1 */
	ade9039_ip.psm1_desc = psm1_desc;
	/* Initialize Reset */
	ade9039_ip.reset_desc = reset_desc;
	/* Initialize temperature */
	ade9039_ip.temp_en = ENABLE;

	/* Initialize power mode */
	ade9039_ip.power_mode = NORMAL_MODE;
	/* Initialize the nominal frequency */
	ade9039_ip.freq = NOMINAL_FREQ;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE9039 SPI example \n");

	/* Initialize the device with the values stored
	in the initialization structure */
	ret = ade9039_init(&ade9039_dev, ade9039_ip);
	if (ret)
		goto remove_reset;
	/* setup the ade9039 device */
	ret = ade9039_setup(ade9039_dev);
	if (ret)
		goto free_dev;
	if (ade9039_dev->temp_en == ENABLE) {
		/* start a new temperature conversion */
		ret = ade9039_update_bits(ade9039_dev, ADE9039_REG_TEMP_CFG, ADE9039_TEMP_START,
					  no_os_field_prep(ADE9039_TEMP_START, 1));
		if (ret)
			goto free_dev;
	}

	no_os_mdelay(RESET_TIME);

	while (1) {
		ret = ade9039_read(ade9039_dev, ADE9039_REG_STATUS0, &status);
		if (ret)
			goto free_dev;

		if (no_os_test_bit(no_os_find_first_set_bit(ADE9039_STATUS0_THD_PF_RDY),
				   &status)) {
			ret = ade9039_read_power_factor_ph(ade9039_dev, ADE9039_PHASE_A);
			if (ret)
				goto free_dev;

			ret = ade9039_update_bits(ade9039_dev, ADE9039_REG_STATUS0,
						  ADE9039_STATUS0_THD_PF_RDY,
						  no_os_field_prep(ADE9039_STATUS0_THD_PF_RDY, 1));
			if (ret)
				goto free_dev;
			update_pf = 1;
		}

		if (no_os_test_bit(no_os_find_first_set_bit(ADE9039_STATUS0_EGYRDY), &status)) {
			ret = ade9039_read_power_ph(ade9039_dev, ADE9039_PHASE_A);
			if (ret)
				goto free_dev;
			ret = ade9039_read_energy_ph(ade9039_dev, ADE9039_PHASE_A);
			if (ret)
				goto free_dev;

			ret = ade9039_update_bits(ade9039_dev, ADE9039_REG_STATUS0,
						  ADE9039_STATUS0_EGYRDY,
						  no_os_field_prep(ADE9039_STATUS0_EGYRDY, 1));
			if (ret)
				goto free_dev;
			update_eng = 1;
		}

		if (no_os_test_bit(no_os_find_first_set_bit(ADE9039_STATUS0_TEMP_RDY),
				   &status)) {
			ret = ade9039_read_temp(ade9039_dev);
			if (ret)
				goto free_dev;

			/* Temperature (°C) = TEMP_RSLT × (−TEMP_GAIN/65536) + (TEMP_OFFSET/32) */
			temp_deg = (ade9039_dev->temp_offset / (1 << 5)) - (ade9039_dev->temp_raw *
					(1000
					 * (int64_t)ade9039_dev->temp_gain / 65536) / 1000);

			ret = ade9039_update_bits(ade9039_dev, ADE9039_REG_STATUS0,
						  ADE9039_STATUS0_TEMP_RDY,
						  no_os_field_prep(ADE9039_STATUS0_TEMP_RDY, 1));
			if (ret)
				goto free_dev;
			/* start a new temperature conversion */
			ret = ade9039_update_bits(ade9039_dev, ADE9039_REG_TEMP_CFG, ADE9039_TEMP_START,
						  no_os_field_prep(ADE9039_TEMP_START, 1));
			if (ret)
				goto free_dev;
			update_temp = 1;

		}

		if (cnt == READ_INTERVAL) {
			ret = ade9039_read_rms_ph(ade9039_dev, ADE9039_PHASE_A);
			if (ret)
				goto free_dev;
			i_rms_val = (int32_t)((uint64_t)(((uint64_t) ade9039_dev->irms_val *
							  ADE9039_FS_VOLTAGE
							  * ADE9039_CURRENT_TR_FCN) / (uint64_t)ADE9039_RMS_FS_CODES));
			v_rms_val = (uint32_t)((uint64_t)(((uint64_t) ade9039_dev->vrms_val *
							   ADE9039_FS_VOLTAGE
							   * ADE9039_VOLTAGE_TR_FCN) / (uint64_t)ADE9039_RMS_FS_CODES));
			pr_info("Phase A RMS -> Vrms: %u mV, Irms: %d mA\n", v_rms_val, i_rms_val);

			if (update_pf) {
				pf_conv = (float)ade9039_dev->pf_val / (1 << 27);
				fpf_conv = (float)ade9039_dev->fpf_val / (1 << 27);
				pr_info("Phase A Power Factor -> PF: %.6f, FPF: %.6f\n", pf_conv, fpf_conv);
				update_pf = 0;
			}

			if (update_eng) {
				watt_conv = convert_power_type(ade9039_dev->watt_val,
							       ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				va_conv = convert_power_type((int32_t)ade9039_dev->va_val,
							     ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				fwatt_conv = convert_power_type(ade9039_dev->fwatt_val,
								ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				fvar_conv = convert_power_type((int32_t)ade9039_dev->fvar_val,
							       ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				fva_conv = convert_power_type((int32_t)ade9039_dev->fva_val,
							      ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				watthr_conv = convert_energy_type(ade9039_dev->watthr_val,
								  ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				varhr_conv = convert_energy_type((int32_t)ade9039_dev->varhr_val,
								 ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				fwatthr_conv = convert_energy_type(ade9039_dev->fwatthr_val,
								   ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				fvarhr_conv = convert_energy_type((int32_t)ade9039_dev->fvarhr_val,
								  ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				fvarahr_conv = convert_energy_type((int32_t)ade9039_dev->fvarahr_val,
								   ADE9039_CURRENT_TR_FCN * ADE9039_VOLTAGE_TR_FCN);
				pr_info("Phase A Power -> Watt: %.6f W, VA: %.6f VA, FWatt: %.6f W, FVar: %.6f VAR, FVA: %.6f VA\n",
					watt_conv, va_conv, fwatt_conv, fvar_conv, fva_conv);
				pr_info("Phase A Energy -> E_WATT: %.6f Whr, E_VAR: %.6f VAhr, E_FWATT: %.6f Whr, E_FVAR: %.6f VARhr, E_FVA: %.6f VAhr\n",
					watthr_conv, varhr_conv, fwatthr_conv, fvarhr_conv, fvarahr_conv);
				update_eng = 0;
			}

			if (update_temp) {
				pr_info("Temperature: %d deg C \n", temp_deg);
				update_temp = 0;
			}

			ret = interface_toggle_led(gpio_desc);
			if (ret)
				pr_err("Failed to toggle LED with err code: %d\n", ret);
			cnt = 0;
		}

		cnt ++;
		no_os_mdelay(20);
	}

free_dev:
	ade9039_remove(ade9039_dev);
remove_reset:
	no_os_gpio_remove(reset_desc);
remove_psm1:
	no_os_gpio_remove(psm1_desc);
remove_psm0:
	no_os_gpio_remove(psm0_desc);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);

error:
	pr_err("ERROR\n");
	return ret;

}

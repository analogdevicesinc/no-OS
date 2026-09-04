/***************************************************************************//**
 *   @file   maxm86161.c
 *   @brief  Implementation of MAXM86161 optical pulse oximeter driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "maxm86161.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"

/**
 * @brief Read a single register from the MAXM86161.
 * @param dev      - Device descriptor.
 * @param reg_addr - Address of the register to read.
 * @param reg_data - Pointer where the read register value is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
		       uint8_t *reg_data)
{
	int ret;

	if (!dev || !reg_data)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, reg_data, 1, 1);
}

/**
 * @brief Write a single register on the MAXM86161.
 * @param dev      - Device descriptor.
 * @param reg_addr - Address of the register to write.
 * @param reg_data - Value to write into the register.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			uint8_t reg_data)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = reg_addr;
	buf[1] = reg_data;

	return no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
}

/**
 * @brief Read-modify-write the bits selected by a mask in a register.
 *
 * Reads the register, clears the bits covered by @p mask and writes @p data
 * into that bit field (aligned to the mask) before writing the register back.
 * @param dev      - Device descriptor.
 * @param reg_addr - Address of the register to update.
 * @param mask     - Bit mask selecting the field to modify.
 * @param data     - New value for the masked field (right-aligned).
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reg_update_bits(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t mask, uint8_t data)
{
	int ret;
	uint8_t reg_val;

	ret = maxm86161_reg_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= no_os_field_prep(mask, data);

	return maxm86161_reg_write(dev, reg_addr, reg_val);
}

/**
 * @brief Burst-read multiple bytes starting at a register address.
 * @param dev      - Device descriptor.
 * @param reg_addr - Starting register address for the burst read.
 * @param reg_data - Buffer where the read bytes are stored.
 * @param count    - Number of bytes to read.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_burst_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
			     uint8_t *reg_data, uint16_t count)
{
	int ret = 0;

	if (!dev || !reg_data)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;

	for (int offset = 0; offset < count;) {
		uint32_t chunk_len = no_os_min(count - offset, MAXM86161_FIFO_BURST_RD_CAP);

		ret = no_os_i2c_read(dev->i2c_desc, reg_data, chunk_len, 1);
		if (ret)
			return ret;

		offset += chunk_len;
		reg_data += chunk_len;
	}

	return 0;
}

/**
 * @brief Burst-write multiple bytes starting at a register address.
 * @param dev      - Device descriptor.
 * @param reg_addr - Starting register address for the burst write.
 * @param reg_data - Buffer holding the bytes to write.
 * @param count    - Number of bytes to write.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_burst_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t *reg_data, uint16_t count)
{
	int ret = 0;
	uint8_t buf[MAXM86161_SHA_CHALLENGE_LEN +
						1]; /* max size of applicable burst write op */

	if (!dev || !reg_data)
		return -EINVAL;

	buf[0] = reg_addr;
	memcpy(&buf[1], reg_data, count);

	ret = no_os_i2c_write(dev->i2c_desc, buf, count + 1, 1);

	return ret;
}

/**
 * @brief Verify the MAXM86161 part identity and cache revision info.
 *
 * Reads the part ID register and checks it against the expected value; on
 * mismatch -ENODEV is returned. On success the part ID and revision ID are
 * stored in the device descriptor.
 * @param dev - Device descriptor.
 * @return 0 on success, -ENODEV if the part ID does not match, or another
 *         negative error code on communication failure.
 */
int maxm86161_verify_id(struct maxm86161_dev *dev)
{
	int ret;
	uint8_t part_id;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PART_ID, &part_id);
	if (ret)
		return ret;

	if (part_id != MAXM86161_PART_ID_VAL)
		return -ENODEV;

	return maxm86161_reg_read(dev, MAXM86161_REG_REV_ID, &dev->rev_id);
}

/**
 * @brief Perform a software reset of the MAXM86161.
 *
 * Sets the RESET bit in the system control register and waits for the reset
 * settling delay before returning.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_reset(struct maxm86161_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_SYS_CTRL,
				  MAXM86161_SYS_CTRL_RESET);
	if (ret)
		return ret;

	no_os_mdelay(MAXM86161_RESET_DELAY_MS);

	return 0;
}

/**
 * @brief Put the MAXM86161 into shutdown (power-down) mode.
 *
 * Convenience wrapper around maxm86161_set_shutdown() that always enables
 * shutdown.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_shutdown(struct maxm86161_dev *dev)
{
	return maxm86161_set_shutdown(dev, true);
}

/**
 * @brief Enable or disable shutdown mode of the MAXM86161.
 * @param dev    - Device descriptor.
 * @param enable - true to enter shutdown, false to resume normal operation.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_shutdown(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SYS_CTRL,
					 MAXM86161_SYS_CTRL_SHDN, enable);
}

/**
 * @brief Enable or disable low-power mode of the MAXM86161.
 * @param dev    - Device descriptor.
 * @param enable - true to enable low-power mode, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_low_power_mode(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SYS_CTRL,
					 MAXM86161_SYS_CTRL_LP_MODE, enable);
}

/**
 * @brief Set the PPG sample rate.
 *
 * Updates the PPG_SR field of PPG_CFG_2 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param ppg_sr - Sample rate selection code (valid range 0x00 to 0x13).
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_sample_rate(struct maxm86161_dev *dev, uint8_t ppg_sr)
{
	int ret;

	if (!dev || ppg_sr > MAXM86161_PPG_SR_MAX)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_2,
					MAXM86161_PPG_CFG2_PPG_SR_MSK, ppg_sr);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the currently configured PPG sample rate.
 * @param dev    - Device descriptor.
 * @param ppg_sr - Pointer where the sample rate selection code is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_sample_rate(struct maxm86161_dev *dev, uint8_t *ppg_sr)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !ppg_sr)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_2, &reg_val);
	if (ret)
		return ret;

	*ppg_sr = no_os_field_get(MAXM86161_PPG_CFG2_PPG_SR_MSK, reg_val);
	return 0;
}

/**
 * @brief Set the PPG ADC integration time.
 *
 * Updates the PPG_TINT field of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev  - Device descriptor.
 * @param tint - Integration time selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint tint)
{
	int ret;

	if (!dev || tint > MAXM86161_TINT_117_3_US)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_PPG_TINT_MSK,
					(uint8_t)tint);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the currently configured PPG ADC integration time.
 * @param dev  - Device descriptor.
 * @param tint - Pointer where the integration time selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint *tint)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !tint)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*tint = no_os_field_get(MAXM86161_PPG_CFG1_PPG_TINT_MSK, reg_val);
	return 0;
}

/**
 * @brief Set the PPG ADC full-scale range.
 *
 * Updates the ADC_RGE field of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev   - Device descriptor.
 * @param range - ADC range selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range range)
{
	int ret;

	if (!dev || range > MAXM86161_ADC_RGE_32UA)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_ADC_RGE_MSK,
					(uint8_t)range);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the currently configured PPG ADC full-scale range.
 * @param dev   - Device descriptor.
 * @param range - Pointer where the ADC range selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range *range)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !range)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*range = no_os_field_get(MAXM86161_PPG_CFG1_ADC_RGE_MSK, reg_val);
	return 0;
}

/**
 * @brief Set the number of samples averaged per FIFO data point.
 *
 * Updates the SMP_AVE field of PPG_CFG_2 and caches the value in the device
 * configuration.
 * @param dev - Device descriptor.
 * @param avg - Sample averaging selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave avg)
{
	int ret;

	if (!dev || avg > MAXM86161_SMP_AVE_128)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_2,
					MAXM86161_PPG_CFG2_SMP_AVE_MSK,
					(uint8_t)avg);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the currently configured sample averaging.
 * @param dev - Device descriptor.
 * @param avg - Pointer where the sample averaging selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave *avg)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !avg)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_2, &reg_val);
	if (ret)
		return ret;

	*avg = no_os_field_get(MAXM86161_PPG_CFG2_SMP_AVE_MSK, reg_val);
	return 0;
}

/**
 * @brief Enable or disable the Ambient Light Cancellation (ALC) circuit.
 *
 * Updates the ALC_DISABLE bit of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev     - Device descriptor.
 * @param disable - true to disable ALC, false to enable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_alc_disable(struct maxm86161_dev *dev, bool disable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_ALC_DISABLE, disable);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the current Ambient Light Cancellation (ALC) disable state.
 * @param dev     - Device descriptor.
 * @param disable - Pointer set to true if ALC is disabled, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_alc_disable(struct maxm86161_dev *dev, bool *disable)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !disable)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*disable = !!no_os_field_get(MAXM86161_PPG_CFG1_ALC_DISABLE, reg_val);
	return 0;
}

/**
 * @brief Enable or disable the ADC add-offset feature.
 *
 * Updates the ADD_OFFSET bit of PPG_CFG_1 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param enable - true to enable the added offset, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_add_offset(struct maxm86161_dev *dev, bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_ADD_OFFSET, enable);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the current ADC add-offset enable state.
 * @param dev    - Device descriptor.
 * @param enable - Pointer set to true if the offset is enabled, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_add_offset(struct maxm86161_dev *dev, bool *enable)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !enable)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*enable = !!no_os_field_get(MAXM86161_PPG_CFG1_ADD_OFFSET, reg_val);
	return 0;
}

/**
 * @brief Set the LED settling time.
 *
 * Updates the LED_SETLNG field of PPG_CFG_3 and caches the value in the device
 * configuration.
 * @param dev      - Device descriptor.
 * @param settling - LED settling time selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling settling)
{
	int ret;

	if (!dev || settling > MAXM86161_LED_SETLNG_12_US)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					MAXM86161_PPG_CFG3_LED_SETLNG_MSK,
					(uint8_t)settling);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the currently configured LED settling time.
 * @param dev      - Device descriptor.
 * @param settling - Pointer where the LED settling time selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling *settling)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !settling)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_3, &reg_val);
	if (ret)
		return ret;

	*settling = no_os_field_get(MAXM86161_PPG_CFG3_LED_SETLNG_MSK, reg_val);
	return 0;
}

/**
 * @brief Select the PPG digital filter (CDM or FDM).
 *
 * Updates the DIG_FILT field of PPG_CFG_3 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param filter - Digital filter selection.
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel filter)
{
	int ret;

	if (!dev || filter > MAXM86161_DIG_FILT_FDM)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					MAXM86161_PPG_CFG3_DIG_FILT_MSK,
					(uint8_t)filter);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the currently selected PPG digital filter.
 * @param dev    - Device descriptor.
 * @param filter - Pointer where the digital filter selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel *filter)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !filter)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_3, &reg_val);
	if (ret)
		return ret;

	*filter = no_os_field_get(MAXM86161_PPG_CFG3_DIG_FILT_MSK, reg_val);
	return 0;
}

/**
 * @brief Set the photodiode bias (based on PD capacitance).
 * @param dev  - Device descriptor.
 * @param bias - Photodiode bias selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias bias)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PD_BIAS,
					 MAXM86161_PD_BIAS_MSK, (uint8_t)bias);
}

/**
 * @brief Get the currently configured photodiode bias.
 * @param dev  - Device descriptor.
 * @param bias - Pointer where the photodiode bias selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias *bias)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !bias)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PD_BIAS, &reg_val);
	if (ret)
		return ret;

	*bias = no_os_field_get(MAXM86161_PD_BIAS_MSK, reg_val);
	return 0;
}

/**
 * @brief Set the LED/measurement source for a single sequence slot.
 *
 * Selects the target LED_SEQ register and nibble (even/odd) based on the slot
 * index, caches the source in the device descriptor and writes the field.
 * @param dev  - Device descriptor.
 * @param slot - Sequence slot index (0 to MAXM86161_NUM_LED_SEQ - 1).
 * @param src  - LED/measurement source assigned to the slot.
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_set_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src src)
{
	uint8_t reg_addr;
	uint8_t mask;

	if (!dev || slot == 0 || slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	slot = slot - 1;

	reg_addr = MAXM86161_REG_LED_SEQ_1 + (slot / 2);

	if (slot % 2 == 0)
		mask = MAXM86161_LED_SEQ_ODD_MSK;
	else
		mask = MAXM86161_LED_SEQ_EVEN_MSK;

	return maxm86161_reg_update_bits(dev, reg_addr, mask, (uint8_t)src);
}

/**
 * @brief Get the LED/measurement source configured for a sequence slot.
 * @param dev  - Device descriptor.
 * @param slot - Sequence slot index (0 to MAXM86161_NUM_LED_SEQ - 1).
 * @param src  - Pointer where the slot's source is stored.
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_get_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src *src)
{
	int ret;
	uint8_t reg_addr;
	uint8_t reg_val;

	if (!dev || !src || slot == 0 || slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	slot = slot - 1;
	reg_addr = MAXM86161_REG_LED_SEQ_1 + (slot / 2);

	ret = maxm86161_reg_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	if (slot % 2 == 0)
		*src = no_os_field_get(MAXM86161_LED_SEQ_ODD_MSK, reg_val);
	else
		*src = no_os_field_get(MAXM86161_LED_SEQ_EVEN_MSK, reg_val);

	return 0;
}

/**
 * @brief Set the pulse amplitude (drive current) of an LED.
 *
 * Writes the LEDn_PA register for the selected LED and caches the value in the
 * device descriptor.
 * @param dev       - Device descriptor.
 * @param led_num   - LED index (1 to MAXM86161_NUM_LEDS).
 * @param amplitude - 8-bit pulse amplitude code.
 * @return 0 on success, -EINVAL for an invalid LED number, or another negative
 *         error code on failure.
 */
int maxm86161_set_led_pulse_amplitude(struct maxm86161_dev *dev,
				      enum maxm86161_led_num led_num, uint8_t amplitude)
{
	uint8_t reg_addr;

	if (!dev || led_num == 0 || led_num > MAXM86161_LED_NUM_PILOT_GREEN)
		return -EINVAL;

	if (led_num == MAXM86161_LED_NUM_PILOT_GREEN) {
		reg_addr = MAXM86161_REG_LED_PILOT_PA;
	} else {
		reg_addr = MAXM86161_REG_LED1_PA + (led_num - 1);
	}

	return maxm86161_reg_write(dev, reg_addr, amplitude);
}

/**
 * @brief Get the pulse amplitude (drive current) of an LED.
 * @param dev       - Device descriptor.
 * @param led_num   - LED index (1 to MAXM86161_NUM_LEDS).
 * @param amplitude - Pointer where the 8-bit pulse amplitude code is stored.
 * @return 0 on success, -EINVAL for an invalid LED number, or another negative
 *         error code on failure.
 */
int maxm86161_get_led_pulse_amplitude(struct maxm86161_dev *dev,
				      enum maxm86161_led_num led_num, uint8_t *amplitude)
{
	uint8_t reg_addr;

	if (!dev || !amplitude || led_num == 0
	    || led_num > MAXM86161_LED_NUM_PILOT_GREEN)
		return -EINVAL;

	reg_addr = (led_num == MAXM86161_LED_NUM_PILOT_GREEN) ?
		   MAXM86161_REG_LED_PILOT_PA :
		   MAXM86161_REG_LED1_PA + (led_num - 1);

	return maxm86161_reg_read(dev, reg_addr, amplitude);
}

/**
 * @brief Set the full-scale current range of an LED driver.
 *
 * Selects the appropriate mask in the LED_RANGE_1 register for the given LED,
 * caches the range in the device descriptor and updates the field.
 * @param dev     - Device descriptor.
 * @param led_num - LED index (1 to MAXM86161_NUM_LEDS).
 * @param range   - LED current range selection.
 * @return 0 on success, -EINVAL for an invalid LED number or range, or another
 *         negative error code on failure.
 */
int maxm86161_set_led_range(struct maxm86161_dev *dev,
			    enum maxm86161_led_num led_num,
			    enum maxm86161_led_range range)
{
	uint8_t mask;

	if (!dev || led_num == 0 || led_num > MAXM86161_LED_NUM_RED ||
	    range > MAXM86161_LED_RGE_124MA)
		return -EINVAL;

	switch (led_num) {
	case MAXM86161_LED_NUM_GREEN:
		mask = MAXM86161_LED1_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_IR:
		mask = MAXM86161_LED2_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_RED:
		mask = MAXM86161_LED3_RGE_MSK;
		break;
	default:
		return -EINVAL;
	}

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_LED_RANGE_1,
					 mask, (uint8_t)range);
}

/**
 * @brief Get the full-scale current range of an LED driver.
 * @param dev     - Device descriptor.
 * @param led_num - LED index (1 to MAXM86161_NUM_LEDS).
 * @param range   - Pointer where the LED current range selection is stored.
 * @return 0 on success, -EINVAL for an invalid LED number, or another negative
 *         error code on failure.
 */
int maxm86161_get_led_range(struct maxm86161_dev *dev,
			    enum maxm86161_led_num led_num,
			    enum maxm86161_led_range *range)
{
	int ret;
	uint8_t reg_val;
	uint8_t led_mask;

	if (!dev || !range || led_num == 0 || led_num > MAXM86161_LED_NUM_RED)
		return -EINVAL;

	switch (led_num) {
	case MAXM86161_LED_NUM_GREEN:
		led_mask = MAXM86161_LED1_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_IR:
		led_mask = MAXM86161_LED2_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_RED:
		led_mask = MAXM86161_LED3_RGE_MSK;
		break;
	default:
		return -EINVAL;
	}

	ret = maxm86161_reg_read(dev, MAXM86161_REG_LED_RANGE_1, &reg_val);
	if (ret)
		return ret;

	*range = no_os_field_get(led_mask, reg_val);
	return 0;
}

/**
 * @brief Get the full-scale current range of an LED driver.
 * @param dev     - Device descriptor.
 * @param led_num - LED index (1 to MAXM86161_NUM_LEDS).
 * @param range   - LED current range selection.
 * @param led_seq_num  - LED/measurement source assigned to the slot.
 * @param amplitude - Pointer where the 8-bit pilot pulse amplitude is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_led_cfg(struct maxm86161_dev *dev,
			  enum maxm86161_led_num led_num,
			  enum maxm86161_led_range range, uint8_t led_seq_num, uint8_t amplitude)
{
	int ret;

	const uint8_t led_seq_code[] = {
		[MAXM86161_LED_NUM_GREEN]	= MAXM86161_LED_SRC_GREEN,
		[MAXM86161_LED_NUM_IR]		= MAXM86161_LED_SRC_IR,
		[MAXM86161_LED_NUM_RED]		= MAXM86161_LED_SRC_RED,
		[MAXM86161_LED_NUM_PILOT_GREEN]	= MAXM86161_LED_SRC_PILOT_GREEN,
	};

	if (led_num != MAXM86161_LED_NUM_PILOT_GREEN) {
		ret = maxm86161_set_led_range(dev, led_num, range);
		if (ret)
			return ret;
	}

	ret = maxm86161_set_led_sequence(dev, led_seq_num, led_seq_code[led_num]);
	if (ret)
		return ret;

	return maxm86161_set_led_pulse_amplitude(dev, led_num, amplitude);
}

/**
 * @brief Configure the high-resolution DAC override for a sequence slot.
 *
 * Writes the per-slot HIRES_DAC register, optionally setting the override bit
 * together with the 6-bit DAC value.
 * @param dev      - Device descriptor.
 * @param slot     - Sequence slot index (1 to MAXM86161_NUM_LED_SEQ).
 * @param override - true to force the DAC value, false to use auto-calibration.
 * @param dac_val  - 6-bit DAC value (only the lower 6 bits are used).
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_set_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool override, uint8_t dac_val)
{
	uint8_t reg_addr;
	uint8_t reg_val;

	if (!dev || slot == 0 || slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	reg_addr = MAXM86161_REG_S1_HIRES_DAC + (slot - 1);
	reg_val = (override ? MAXM86161_HIRES_DAC_OVR : 0) |
		  no_os_field_get(MAXM86161_HIRES_DAC_VAL_MSK, dac_val);

	return maxm86161_reg_write(dev, reg_addr, reg_val);
}

/**
 * @brief Read back the high-resolution DAC configuration for a slot.
 * @param dev      - Device descriptor.
 * @param slot     - Sequence slot index (1 to MAXM86161_NUM_LED_SEQ).
 * @param override - Pointer set to true if the DAC override is active.
 * @param dac_val  - Pointer where the 6-bit DAC value is stored.
 * @return 0 on success, -EINVAL for an invalid slot, or another negative error
 *         code on failure.
 */
int maxm86161_get_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool *override, uint8_t *dac_val)
{
	int ret;
	uint8_t reg_addr;
	uint8_t reg_val;

	if (!dev || !override || !dac_val || slot == 0 ||
	    slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	reg_addr = MAXM86161_REG_S1_HIRES_DAC + (slot - 1);

	ret = maxm86161_reg_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	*override = !!no_os_field_get(MAXM86161_HIRES_DAC_OVR, reg_val);
	*dac_val = no_os_field_get(MAXM86161_HIRES_DAC_VAL_MSK, reg_val);
	return 0;
}

/**
 * @brief Set the FIFO almost-full watermark threshold.
 *
 * Updates the A_FULL field of FIFO_CFG_1 and caches the value in the device
 * configuration.
 * @param dev       - Device descriptor.
 * @param watermark - Almost-full threshold (valid range 0 to 127).
 * @return 0 on success, -EINVAL for an out-of-range value, or another negative
 *         error code on failure.
 */
int maxm86161_set_fifo_watermark(struct maxm86161_dev *dev, uint8_t watermark)
{
	int ret;

	if (!dev || watermark > MAXM86161_FIFO_A_FULL_MAX)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_1,
					MAXM86161_FIFO_A_FULL_MSK, watermark);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the FIFO almost-full watermark threshold.
 * @param dev       - Device descriptor.
 * @param watermark - Pointer where the almost-full threshold is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_watermark(struct maxm86161_dev *dev, uint8_t *watermark)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !watermark)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_FIFO_CFG_1, &reg_val);
	if (ret)
		return ret;

	*watermark = no_os_field_get(MAXM86161_FIFO_A_FULL_MSK, reg_val);
	return 0;
}

/**
 * @brief Flush (clear) the FIFO contents.
 *
 * Sets the FLUSH bit in FIFO_CFG_2 to discard all currently buffered samples.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_fifo_flush(struct maxm86161_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					 MAXM86161_FIFO_CFG2_FLUSH, 1);
}

/**
 * @brief Enable or disable FIFO roll-over on full.
 *
 * Updates the FIFO_RO bit of FIFO_CFG_2 and caches the value in the device
 * configuration. When enabled, new samples overwrite the oldest once the FIFO
 * is full.
 * @param dev    - Device descriptor.
 * @param enable - true to enable roll-over, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_fifo_rollover(struct maxm86161_dev *dev, bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					MAXM86161_FIFO_CFG2_FIFO_RO, enable);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the FIFO roll-over enable state.
 * @param dev    - Device descriptor.
 * @param enable - Pointer set to true if roll-over is enabled, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_rollover(struct maxm86161_dev *dev, bool *enable)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !enable)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_FIFO_CFG_2, &reg_val);
	if (ret)
		return ret;

	*enable = !!no_os_field_get(MAXM86161_FIFO_CFG2_FIFO_RO, reg_val);
	return 0;
}

/**
 * @brief Set the FIFO almost-full interrupt assertion behavior.
 *
 * Updates the A_FULL_TYPE bit of FIFO_CFG_2 and caches the value in the device
 * configuration, selecting whether the almost-full interrupt is repeated for
 * each new sample or asserted only once.
 * @param dev  - Device descriptor.
 * @param type - Almost-full interrupt type selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_fifo_a_full_type(struct maxm86161_dev *dev, bool type)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					MAXM86161_FIFO_CFG2_A_FULL_TYPE, type);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the FIFO almost-full interrupt assertion behavior.
 * @param dev  - Device descriptor.
 * @param type - Pointer where the almost-full interrupt type is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_a_full_type(struct maxm86161_dev *dev, bool *type)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !type)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_FIFO_CFG_2, &reg_val);
	if (ret)
		return ret;

	*type = !!no_os_field_get(MAXM86161_FIFO_CFG2_A_FULL_TYPE, reg_val);
	return 0;
}

/**
 * @brief Configure whether reading FIFO data clears the status bit.
 *
 * Updates the STAT_CLR bit of FIFO_CFG_2 and caches the value in the device
 * configuration.
 * @param dev    - Device descriptor.
 * @param enable - true to clear the FIFO status on data read, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_fifo_stat_clr(struct maxm86161_dev *dev, bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					MAXM86161_FIFO_CFG2_STAT_CLR, enable);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the number of samples currently stored in the FIFO.
 * @param dev   - Device descriptor.
 * @param count - Pointer where the FIFO sample count is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_fifo_count(struct maxm86161_dev *dev, uint8_t *count)
{
	if (!dev || !count)
		return -EINVAL;

	return maxm86161_reg_read(dev, MAXM86161_REG_FIFO_DATA_COUNT, count);
}

/**
 * @brief Read the FIFO overflow counter.
 *
 * Reads the OVF_COUNTER register and returns the number of samples lost due to
 * FIFO overflow (lower 7 bits).
 * @param dev            - Device descriptor.
 * @param overflow_count - Pointer where the overflow sample count is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_fifo_overflow_check(struct maxm86161_dev *dev,
				  uint8_t *overflow_count)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !overflow_count)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_OVF_COUNTER, &reg_val);
	if (ret)
		return ret;

	*overflow_count = no_os_field_get(MAXM86161_FIFO_OVF_COUNTER_MSK, reg_val);
	return 0;
}

/**
 * @brief Burst-read raw sample data from the FIFO.
 *
 * Reads num_samples samples from the FIFO_DATA register, each sample being
 * MAXM86161_FIFO_DATA_SIZE bytes wide.
 * @param dev         - Device descriptor.
 * @param data        - Buffer receiving the raw sample bytes; must hold at least
 *                      num_samples * MAXM86161_FIFO_DATA_SIZE bytes.
 * @param num_samples - Number of samples to read (must be non-zero).
 * @return 0 on success, -EINVAL for invalid arguments, or another negative
 *         error code on failure.
 */
int maxm86161_read_fifo_data(struct maxm86161_dev *dev, uint8_t *data,
			     uint16_t num_samples)
{
	if (!dev || !data || num_samples == 0)
		return -EINVAL;

	return maxm86161_burst_reg_read(dev, MAXM86161_REG_FIFO_DATA, data,
					num_samples * MAXM86161_FIFO_DATA_SIZE);
}

/**
 * @brief Decode a raw 3-byte FIFO sample into its tag and data fields.
 *
 * Combines the three raw bytes (big-endian) into a 24-bit value and extracts
 * the sample tag and the measurement data.
 * @param raw  - Pointer to the 3 raw sample bytes.
 * @param tag  - Pointer where the decoded sample tag is stored.
 * @param data - Pointer where the decoded measurement data is stored.
 * @return 0 on success, -EINVAL if any pointer is NULL.
 */
int maxm86161_decode_fifo_sample(uint8_t *raw, uint8_t *tag, uint32_t *data)
{
	uint32_t raw_val;

	if (!raw || !tag || !data)
		return -EINVAL;

	/* data is received in big endian format. convert to single raw value */
	raw_val = ((uint32_t)raw[0] << 16) | ((uint32_t)raw[1] << 8) | raw[2];

	*tag = no_os_field_get(MAXM86161_FIFO_TAG_MSK, raw_val);
	*data = no_os_field_get(MAXM86161_FIFO_DATA_MSK, raw_val);

	return 0;
}

/**
 * @brief Read the combined interrupt status of the MAXM86161.
 *
 * Reads both INT_STATUS_1 and INT_STATUS_2 and packs them into a single 16-bit
 * value (STATUS_2 in the high byte, STATUS_1 in the low byte).
 * @param dev    - Device descriptor.
 * @param status - Pointer where the packed interrupt status is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_interrupt_status(struct maxm86161_dev *dev, uint16_t *status)
{
	int ret;
	uint8_t stat1, stat2;

	if (!dev || !status)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_1, &stat1);
	if (ret)
		return ret;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_2, &stat2);
	if (ret)
		return ret;

	*status = ((uint16_t)stat2 << 8) | stat1;
	return 0;
}

/**
 * @brief Clear pending interrupt status flags.
 *
 * Reads INT_STATUS_1 and INT_STATUS_2 (which are cleared on read) to
 * acknowledge and clear any pending interrupt flags.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_clear_interrupt_status(struct maxm86161_dev *dev)
{
	int ret;
	uint8_t dummy;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_1, &dummy);
	if (ret)
		return ret;

	return maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_2, &dummy);
}

/**
 * @brief Register a callback for an interrupt source and enable/disable it.
 *
 * Enables or disables the enable bit for the requested interrupt source in the
 * appropriate INT_EN register and stores the callback and its argument in the
 * device's handler table. The PWR_RDY source has no enable bit, so only the
 * callback is registered for it.
 * @param dev        - Device descriptor.
 * @param int_source - Interrupt source to configure.
 * @param enable     - true to enable the interrupt, false to disable it.
 * @param callback   - Callback invoked when the interrupt fires.
 * @param cb_args    - Opaque argument passed to the callback.
 * @return 0 on success, -EINVAL for an invalid interrupt source, or another
 *         negative error code on failure.
 */
int maxm86161_set_irq_handler(struct maxm86161_dev *dev,
			      enum maxm86161_irq_sources int_source,
			      bool enable, maxm86161_int_cb_t callback,
			      void *cb_args)
{
	int ret;
	uint8_t int_mask = 0;
	uint8_t int_reg;
	struct maxm86161_int_handler handler = {
		.callback = callback,
		.args = cb_args,
	};

	if (!dev || int_source < 0 || int_source >= MAXM86161_INT_COUNT)
		return -EINVAL;

	int_reg = MAXM86161_REG_INT_EN_1;

	switch (int_source) {
	case MAXM86161_INT_PWR_RDY:
		/* PWR_RDY has no enable bit; just register the callback */
		dev->int_handlers[int_source] = handler;
		return 0;
	case MAXM86161_INT_DIE_TEMP_RDY:
		int_mask = MAXM86161_INT1_DIE_TEMP_RDY_EN;
		break;
	case MAXM86161_INT_LED_COMPB:
		int_mask = MAXM86161_INT1_LED_COMPB_EN;
		break;
	case MAXM86161_INT_PROX_INT:
		int_mask = MAXM86161_INT1_PROX_INT_EN;
		break;
	case MAXM86161_INT_ALC_OVF:
		int_mask = MAXM86161_INT1_ALC_OVF_EN;
		break;
	case MAXM86161_INT_DATA_RDY:
		int_mask = MAXM86161_INT1_DATA_RDY_EN;
		break;
	case MAXM86161_INT_A_FULL:
		int_mask = MAXM86161_INT1_A_FULL_EN;
		break;
	case MAXM86161_INT_SHA_DONE:
		int_mask = MAXM86161_INT2_SHA_DONE_EN;
		int_reg = MAXM86161_REG_INT_EN_2;
		break;
	default:
		return -EINVAL;
	}

	ret = maxm86161_reg_update_bits(dev, int_reg, int_mask, enable);
	if (ret)
		return ret;

	dev->int_handlers[int_source] = handler;

	return 0;
}

/**
 * @brief Top-level interrupt dispatcher for the MAXM86161.
 *
 * Intended to be registered as the GPIO/INTB interrupt callback. Reads the
 * combined interrupt status and, for every asserted source, invokes the
 * registered per-source callback (if any). Does nothing if the context is NULL
 * or the status read fails.
 * @param ctx - Opaque context, expected to be a struct maxm86161_dev pointer.
 */
void maxm86161_irq_handler(void *ctx)
{
	struct maxm86161_dev *dev = (struct maxm86161_dev *)ctx;
	uint16_t status;
	int ret, i;

	if (!dev)
		return;

	/* Disable IRQ during interrupt */
	ret = no_os_irq_disable(dev->irq_ctrl, dev->gpio_intb->number);
	if (ret)
		return;

	ret = maxm86161_get_interrupt_status(dev, &status);
	if (ret) {
		no_os_irq_enable(dev->irq_ctrl, dev->gpio_intb->number);
		return;
	}

	for (i = 0; i < MAXM86161_INT_COUNT; i++) {
		if (!no_os_field_get(maxm86161_irq_status_bits[i], status))
			continue;
		if (dev->int_handlers[i].callback)
			dev->int_handlers[i].callback(dev->int_handlers[i].args);
	}

	ret = no_os_irq_enable(dev->irq_ctrl, dev->gpio_intb->number);
	if (ret)
		return;
}

/**
 * @brief Set the proximity mode entry threshold.
 *
 * Writes the PROX_INT_THRESH register that determines the ADC count at which
 * the device transitions from proximity to normal PPG mode.
 * @param dev       - Device descriptor.
 * @param threshold - Proximity interrupt threshold value.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_prox_threshold(struct maxm86161_dev *dev, uint8_t threshold)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_write(dev, MAXM86161_REG_PROX_INT_THRESH,
				   threshold);
}

/**
 * @brief Get the proximity mode entry threshold.
 * @param dev       - Device descriptor.
 * @param threshold - Pointer where the proximity threshold value is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_prox_threshold(struct maxm86161_dev *dev, uint8_t *threshold)
{
	if (!dev || !threshold)
		return -EINVAL;

	return maxm86161_reg_read(dev, MAXM86161_REG_PROX_INT_THRESH, threshold);
}

/**
 * @brief Enable or disable the picket-fence detect-and-replace feature.
 *
 * Updates the ENABLE bit of the PICKET_FENCE register.
 * @param dev    - Device descriptor.
 * @param enable - true to enable picket-fence, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_pf_enable(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PICKET_FENCE,
					 MAXM86161_PF_ENABLE, enable);
}

/**
 * @brief Set the picket-fence detect/replace ordering.
 *
 * Updates the ORDER bit of the PICKET_FENCE register.
 * @param dev   - Device descriptor.
 * @param order - Ordering selection (false or true).
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_pf_order(struct maxm86161_dev *dev, bool order)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PICKET_FENCE,
					 MAXM86161_PF_ORDER, order);
}

/**
 * @brief Set the picket-fence IIR time constant.
 *
 * Updates the IIR_TC field of the PICKET_FENCE register.
 * @param dev    - Device descriptor.
 * @param iir_tc - IIR time constant selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_pf_iir_tc(struct maxm86161_dev *dev, uint8_t iir_tc)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PICKET_FENCE,
					 MAXM86161_PF_IIR_TC_MSK, iir_tc);
}

/**
 * @brief Set the picket-fence IIR initialization value.
 *
 * Updates the IIR_INIT field of the PICKET_FENCE register.
 * @param dev            - Device descriptor.
 * @param iir_init_value - IIR initialization value selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_pf_iir_init(struct maxm86161_dev *dev, uint8_t iir_init_value)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PICKET_FENCE,
					 MAXM86161_PF_IIR_INIT_MSK,
					 iir_init_value);
}

/**
 * @brief Set the picket-fence threshold sigma multiplier.
 *
 * Updates the THRESH_SIGMA field of the PICKET_FENCE register.
 * @param dev                  - Device descriptor.
 * @param threshold_sigma_mult - Threshold sigma multiplier selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_pf_threshold_sigma(struct maxm86161_dev *dev,
				     uint8_t threshold_sigma_mult)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PICKET_FENCE,
					 MAXM86161_PF_THRESH_SIGMA_MSK,
					 threshold_sigma_mult);
}
/**
 * @brief Get the picket-fence enable state.
 * @param dev    - Device descriptor.
 * @param enable - Pointer set to true if picket-fence is enabled, false otherwise.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_pf_enable(struct maxm86161_dev *dev, bool *enable)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !enable)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PICKET_FENCE, &reg_val);
	if (ret)
		return ret;

	*enable = !!no_os_field_get(MAXM86161_PF_ENABLE, reg_val);
	return 0;
}

/**
 * @brief Get the picket-fence detect/replace ordering.
 * @param dev   - Device descriptor.
 * @param order - Pointer where the ordering selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_pf_order(struct maxm86161_dev *dev, bool *order)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !order)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PICKET_FENCE, &reg_val);
	if (ret)
		return ret;

	*order = !!no_os_field_get(MAXM86161_PF_ORDER, reg_val);
	return 0;
}

/**
 * @brief Get the picket-fence IIR time constant.
 * @param dev    - Device descriptor.
 * @param iir_tc - Pointer where the IIR time constant selection is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_pf_iir_tc(struct maxm86161_dev *dev, uint8_t *iir_tc)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !iir_tc)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PICKET_FENCE, &reg_val);
	if (ret)
		return ret;

	*iir_tc = no_os_field_get(MAXM86161_PF_IIR_TC_MSK, reg_val);
	return 0;
}

/**
 * @brief Get the picket-fence IIR initialization value.
 * @param dev            - Device descriptor.
 * @param iir_init_value - Pointer where the IIR initialization value is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_pf_iir_init(struct maxm86161_dev *dev,
			      uint8_t *iir_init_value)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !iir_init_value)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PICKET_FENCE, &reg_val);
	if (ret)
		return ret;

	*iir_init_value = no_os_field_get(MAXM86161_PF_IIR_INIT_MSK, reg_val);
	return 0;
}

/**
 * @brief Get the picket-fence threshold sigma multiplier.
 * @param dev                  - Device descriptor.
 * @param threshold_sigma_mult - Pointer where the threshold sigma multiplier is
 *                               stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_get_pf_threshold_sigma(struct maxm86161_dev *dev,
				     uint8_t *threshold_sigma_mult)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !threshold_sigma_mult)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PICKET_FENCE, &reg_val);
	if (ret)
		return ret;

	*threshold_sigma_mult = no_os_field_get(MAXM86161_PF_THRESH_SIGMA_MSK,
						reg_val);
	return 0;
}

static int maxm86161_trigger_temp_read(struct maxm86161_dev *dev)
{
	int ret;
	uint8_t reg_val;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_DIE_TEMP_CFG,
				  MAXM86161_TEMP_EN);
	if (ret)
		return ret;

	/* wait for bit clear to indicate temp has been sampled. */
	for (int i = 0; i < MAXM86161_TEMP_MEAS_WAIT_TRIES; i++) {
		no_os_mdelay(MAXM86161_TEMP_MEAS_DELAY_MS);

		ret = maxm86161_reg_read(dev, MAXM86161_REG_DIE_TEMP_CFG,
					 &reg_val);
		if (ret)
			return ret;

		if (!no_os_field_get(MAXM86161_TEMP_EN, reg_val))
			return 0;
	}

	return -ETIMEDOUT;
}

/**
 * @brief Read and convert the die temperature.
 *
 * Reads the integer and fractional die-temperature registers and combines them
 * into a signed temperature in degrees Celsius (fractional LSB = 0.0625 degC).
 * @param dev         - Device descriptor.
 * @param temperature - Pointer where the temperature in micro-degrees Celsius
 *                      (1 degC = 1000000) is stored.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_read_die_temperature(struct maxm86161_dev *dev,
				   int32_t *temperature)
{
	int ret;
	uint16_t temp_raw;
	uint8_t *temp_raw_ptr = (uint8_t *)&temp_raw;
	uint8_t temp_frac;
	int8_t signed_int;

	if (!dev || !temperature)
		return -EINVAL;

	ret = maxm86161_trigger_temp_read(dev);
	if (ret)
		return ret;

	ret = maxm86161_burst_reg_read(dev, MAXM86161_REG_DIE_TEMP_INT,
				       temp_raw_ptr, sizeof(temp_raw));
	if (ret)
		return ret;

	signed_int = (int8_t)no_os_field_get(MAXM86161_REG_DIE_TEMP_INT_MASK, temp_raw);
	temp_frac = no_os_field_get(
			    MAXM86161_REG_DIE_TEMP_FRAC_MASK << MAXM86161_DIE_TEMP_FRAC_OFFSET,
			    temp_raw);

	*temperature = (int32_t)signed_int * MAXM86161_DIE_TEMP_SCALE_UC +
		       (int32_t)temp_frac * MAXM86161_DIE_TEMP_FRAC_SCALE_UC;

	return 0;
}

/**
 * @brief Start the DAC (offset) calibration procedure.
 *
 * Sets the START_CAL bit in the DAC calibration enable register to begin
 * calibration. Completion should be polled via maxm86161_get_calibration_status().
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_start_calibration(struct maxm86161_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_DAC_CAL_EN,
					 MAXM86161_START_CAL, 1);
}

/**
 * @brief Query the DAC calibration status.
 *
 * Reads the DAC calibration register and reports whether calibration has
 * completed and whether the result is out of range.
 * @param dev      - Device descriptor.
 * @param complete - Pointer set to true when calibration has completed.
 * @param oor      - Pointer set to true when the calibration result is out of range.
 * @return 0 on success, -EINVAL if any pointer is NULL, or another negative
 *         error code on failure.
 */
int maxm86161_get_calibration_status(struct maxm86161_dev *dev,
				     bool *complete, bool *oor)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !complete || !oor)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_DAC_CAL_EN, &reg_val);
	if (ret)
		return ret;

	*complete = !!no_os_field_get(MAXM86161_CAL_DAC_COMPLETE, reg_val);
	*oor = !!no_os_field_get(MAXM86161_CAL_DAC1_OOR, reg_val);

	return 0;
}

/**
 * @brief Enable or disable burst sampling mode and set its rate.
 *
 * Updates the BURST_EN bit and, on success, the BURST_RATE field of PPG_CFG_3.
 * @param dev    - Device descriptor.
 * @param enable - true to enable burst mode, false to disable it.
 * @return 0 on success, -EINVAL for a negative error code on failure.
 */
int maxm86161_set_burst_enable(struct maxm86161_dev *dev, bool enable)
{
	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					 MAXM86161_PPG_CFG3_BURST_EN, enable);
}

/**
 * @brief Enable or disable burst sampling mode and set its rate.
 *
 * Updates the BURST_EN bit and, on success, the BURST_RATE field of PPG_CFG_3.
 * @param dev    - Device descriptor.
 * @param rate   - Burst sampling rate selection.
 * @return 0 on success, -EINVAL for an out-of-range rate, or another negative
 *         error code on failure.
 */
int maxm86161_set_burst_rate(struct maxm86161_dev *dev,
			     enum maxm86161_burst_rate rate)
{
	if (!dev || rate > MAXM86161_BURST_RATE_256HZ)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					 MAXM86161_PPG_CFG3_BURST_RATE_MSK,
					 (uint8_t)rate);
}

/**
 * @brief Enable or disable burst sampling mode and set its rate.
 *
 * Updates the BURST_EN bit and, on success, the BURST_RATE field of PPG_CFG_3.
 * @param dev    - Device descriptor.
 * @param enable - true to enable burst mode, false to disable it.
 * @param rate   - Burst sampling rate selection.
 * @return 0 on success, -EINVAL for an out-of-range rate, or another negative
 *         error code on failure.
 */
int maxm86161_set_burst_mode(struct maxm86161_dev *dev, bool enable,
			     enum maxm86161_burst_rate rate)
{
	int ret;

	if (!dev || rate > MAXM86161_BURST_RATE_256HZ)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					MAXM86161_PPG_CFG3_BURST_EN, enable);
	if (ret)
		return ret;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					 MAXM86161_PPG_CFG3_BURST_RATE_MSK,
					 (uint8_t)rate);
}

/**
 * @brief Enable or disable timestamp tagging of PPG samples.
 *
 * Updates the TIMESTAMP_EN bit of the PPG sync control register.
 * @param dev    - Device descriptor.
 * @param enable - true to enable timestamps, false to disable them.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_timestamp_enable(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_TIMESTAMP_EN, enable);
}

/**
 * @brief Enable or disable DAC-code tagging of PPG samples.
 *
 * Updates the DAC_CODE_TAG bit of the PPG sync control register.
 * @param dev    - Device descriptor.
 * @param enable - true to enable DAC-code tagging, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_dac_code_tag(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_DAC_CODE_TAG, enable);
}

/**
 * @brief Configure the behavior of the GPIO/sync pin.
 *
 * Updates the GPIO_CTRL field of the PPG sync control register.
 * @param dev  - Device descriptor.
 * @param ctrl - GPIO control mode selection.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_set_gpio_ctrl(struct maxm86161_dev *dev,
			    enum maxm86161_gpio_ctrl ctrl)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_GPIO_CTRL_MSK,
					 (uint8_t)ctrl);
}

/**
 * @brief Issue a software-forced PPG synchronization event.
 *
 * Sets the SW_FORCE bit in the PPG sync control register to trigger a sync.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_force_sync(struct maxm86161_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_SW_FORCE, 1);
}

/**
 * @brief Enable or disable the SHA-256 authentication engine.
 *
 * Updates the SHA_EN bit of the SHA configuration register.
 * @param dev    - Device descriptor.
 * @param enable - true to enable the SHA engine, false to disable it.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_sha_enable(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SHA_CFG,
					 MAXM86161_SHA_EN, enable);
}

/**
 * @brief Issue a SHA command and start the SHA-256 operation.
 *
 * Writes the command to the SHA command register and then sets the SHA_START
 * bit in the SHA configuration register to begin the operation.
 * @param dev - Device descriptor.
 * @param cmd - SHA command code to execute.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_sha_start(struct maxm86161_dev *dev, uint8_t cmd)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_SHA_CMD, cmd);
	if (ret)
		return ret;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SHA_CFG,
					 MAXM86161_SHA_START, true);
}

/**
 * @brief Write a SHA-256 challenge into the device memory.
 *
 * Enables memory writes, sets the memory index to the SHA memory start and
 * burst-writes the challenge into the memory data register. The challenge must
 * be exactly 20 bytes (a 160-bit value).
 * @param dev       - Device descriptor.
 * @param challenge - Buffer holding the challenge bytes.
 * @param len       - Length of the challenge in bytes (must be 20).
 * @return 0 on success, -EINVAL for invalid arguments or an incorrect length,
 *         or another negative error code on failure.
 */
int maxm86161_sha_write_challenge(struct maxm86161_dev *dev,
				  uint8_t *challenge, uint8_t len)
{
	int ret;

	/* challenge must be a 160-bit value */
	if (!dev || !challenge || len != MAXM86161_SHA_CHALLENGE_LEN)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					MAXM86161_MEM_WR_EN, true);
	if (ret)
		return ret;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX,
				  MAXM86161_SHA_MEM_START);
	if (ret)
		return ret;

	return maxm86161_burst_reg_write(dev, MAXM86161_REG_MEM_DATA,
					 challenge, MAXM86161_SHA_CHALLENGE_LEN);
}

/**
 * @brief Read the SHA-256 response from the device memory.
 *
 * Sets the memory index to the SHA memory start and burst-reads the response
 * from the memory data register.
 * @param dev      - Device descriptor.
 * @param response - Buffer receiving the response bytes.
 * @param len      - Number of bytes to read (must be non-zero).
 * @return 0 on success, -EINVAL for invalid arguments, or another negative
 *         error code on failure.
 */
int maxm86161_sha_read_response(struct maxm86161_dev *dev,
				uint8_t *response, uint8_t len)
{
	int ret;

	if (!dev || !response || len == 0)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX, MAXM86161_SHA_MEM_START);
	if (ret)
		return ret;

	return maxm86161_burst_reg_read(dev, MAXM86161_REG_MEM_DATA, response, len);
}

/**
 * @brief Read a single byte from the device memory.
 *
 * Selects the memory bank based on the address (bank 1 for addresses above
 * 0xFF), sets the memory index to the low byte of the address and reads the
 * memory data register.
 * @param dev  - Device descriptor.
 * @param addr - Memory address to read (valid range 0x000 to 0x17F).
 * @param data - Pointer where the read byte is stored.
 * @return 0 on success, -EINVAL for invalid arguments or an out-of-range
 *         address, or another negative error code on failure.
 */
int maxm86161_memory_read(struct maxm86161_dev *dev, uint16_t addr,
			  uint8_t *data)
{
	int ret;
	uint8_t bank;

	if (!dev || !data || addr > MAXM86161_MEM_IDX_MAX)
		return -EINVAL;

	bank = (addr > MAXM86161_MEM_IDX_BANK0_MSK) ? 1 : 0;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					MAXM86161_MEM_BANK_SEL, bank);
	if (ret)
		return ret;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX,
				  (uint8_t)no_os_field_get(MAXM86161_MEM_IDX_BANK0_MSK, addr));
	if (ret)
		return ret;

	return maxm86161_reg_read(dev, MAXM86161_REG_MEM_DATA, data);
}

/**
 * @brief Write a single byte to the device memory.
 *
 * Enables memory writes and selects the memory bank based on the address (bank
 * 1 for addresses above 0xFF), sets the memory index to the low byte of the
 * address and writes the memory data register.
 * @param dev  - Device descriptor.
 * @param addr - Memory address to write (valid range 0x000 to 0x17F).
 * @param data - Byte value to write.
 * @return 0 on success, -EINVAL for invalid arguments or an out-of-range
 *         address, or another negative error code on failure.
 */
int maxm86161_memory_write(struct maxm86161_dev *dev, uint16_t addr,
			   uint8_t data)
{
	int ret;
	uint8_t bank;

	if (!dev || addr > MAXM86161_MEM_IDX_MAX)
		return -EINVAL;

	bank = (addr > MAXM86161_MEM_IDX_BANK0_MSK) ? 1 : 0;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					MAXM86161_MEM_WR_EN |
					MAXM86161_MEM_BANK_SEL,
					MAXM86161_MEM_WR_EN |
					(bank ? MAXM86161_MEM_BANK_SEL : 0));
	if (ret)
		return ret;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX,
				  (uint8_t)no_os_field_get(MAXM86161_MEM_IDX_BANK0_MSK, addr));
	if (ret)
		return ret;

	return maxm86161_reg_write(dev, MAXM86161_REG_MEM_DATA, data);
}

int maxm86161_memory_bank_set(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					 MAXM86161_MEM_BANK_SEL, enable);
}

int maxm86161_memory_bank_get(struct maxm86161_dev *dev, bool *enable)
{

	int ret;
	uint8_t reg_val;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_MEM_CTRL, &reg_val);
	if (ret)
		return ret;

	*enable = no_os_field_get(MAXM86161_MEM_BANK_SEL, reg_val);

	return 0;
}

/**
 * @brief Initialize the MAXM86161 device driver.
 *
 * Allocates the device descriptor, initializes the I2C Bus,
 * verifies the part ID, resets the device, places it into shutdown, clears the
 * interrupt status and selects single-PPG mode. Optionally configures the INTB
 * GPIO and registers a falling-edge interrupt callback when the corresponding
 * init parameters are supplied. On any failure all acquired resources are
 * released.
 * @param device     - Pointer that receives the allocated device descriptor.
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_init(struct maxm86161_dev **device,
		   struct maxm86161_init_param *init_param)
{
	struct maxm86161_dev *dev;
	int ret = 0, r = 0;

	if (!device || !init_param)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto err_free_dev;

	ret = maxm86161_verify_id(dev);
	if (ret)
		goto err_bus;

	ret = maxm86161_reset(dev);
	if (ret)
		goto err_bus;

	ret = maxm86161_set_shutdown(dev, true);
	if (ret)
		goto err_bus;

	ret = maxm86161_clear_interrupt_status(dev);
	if (ret)
		goto err_bus;

	/*
	 * MAXM86161 is single-channel only. set to 1 defined by datasheet spec,
	 * but is generally ignored for MAXM86161.
	 */
	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_SYS_CTRL,
					MAXM86161_SYS_CTRL_SINGLE_PPG, 1);
	if (ret)
		goto err_bus;

	if (init_param->gpio_intb_init) {
		ret = no_os_gpio_get(&dev->gpio_intb,
				     init_param->gpio_intb_init);
		if (ret)
			goto err_bus;

		ret = no_os_gpio_direction_input(dev->gpio_intb);
		if (ret)
			goto err_gpio;

		if (init_param->irq_init) {
			struct no_os_callback_desc irq_cb = {
				.callback = maxm86161_irq_handler,
				.ctx = dev,
				.event = NO_OS_EVT_GPIO,
				.peripheral = NO_OS_GPIO_IRQ,
				.handle = init_param->irq_handle,
			};

			ret = no_os_irq_ctrl_init(&dev->irq_ctrl, init_param->irq_init);
			if (ret)
				goto err_gpio;

			ret = no_os_irq_register_callback(dev->irq_ctrl, dev->gpio_intb->number,
							  &irq_cb);
			if (ret)
				goto err_irq;

			ret = no_os_irq_trigger_level_set(dev->irq_ctrl, dev->gpio_intb->number,
							  NO_OS_IRQ_EDGE_FALLING);
			if (ret)
				goto err_irq;

			ret = no_os_irq_set_priority(dev->irq_ctrl, dev->gpio_intb->number, 7);
			if (ret)
				goto err_irq;

			ret = no_os_irq_enable(dev->irq_ctrl, dev->gpio_intb->number);
			if (ret)
				goto err_irq;
		}
	}

	*device = dev;
	return 0;

err_irq:
	if (dev->irq_ctrl)
		no_os_irq_ctrl_remove(dev->irq_ctrl);
err_gpio:
	if (dev->gpio_intb)
		no_os_gpio_remove(dev->gpio_intb);
err_bus:
	r = no_os_i2c_remove(dev->i2c_desc);
	if (r)
		return r;
err_free_dev:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free resources allocated by maxm86161_init().
 *
 * Puts the device into shutdown, disables and unregisters the interrupt
 * callback and removes the IRQ controller (if used), removes the INTB GPIO
 * (if used), tears down the bus backend and frees the device descriptor. The
 * first error encountered while releasing GPIO/bus resources is returned.
 * @param dev - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int maxm86161_remove(struct maxm86161_dev *dev)
{
	struct no_os_callback_desc irq_cb = { 0 };
	int ret = 0;

	if (!dev)
		return -EINVAL;

	maxm86161_shutdown(dev);

	if (dev->irq_ctrl) {
		no_os_irq_disable(dev->irq_ctrl, dev->gpio_intb->number);
		irq_cb.callback = maxm86161_irq_handler;
		irq_cb.ctx = dev;
		irq_cb.event = NO_OS_EVT_GPIO;
		irq_cb.peripheral = NO_OS_GPIO_IRQ;
		no_os_irq_unregister_callback(dev->irq_ctrl, dev->gpio_intb->number,
					      &irq_cb);
		no_os_irq_ctrl_remove(dev->irq_ctrl);
	}

	if (dev->gpio_intb) {
		ret = no_os_gpio_remove(dev->gpio_intb);
		if (ret)
			return ret;
	}

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}

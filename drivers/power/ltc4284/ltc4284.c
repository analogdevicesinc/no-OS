/***************************************************************************//**
 *   @file   ltc4284.c
 *   @brief  Implementation of LTC4284 Driver
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ltc4284.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/**
 * @brief Read a single byte from LTC4284 register
 * @param dev - Device descriptor
 * @param reg - Register address
 * @param val - Pointer to store read value
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_byte(struct ltc4284_dev *dev, uint8_t reg, uint8_t *val)
{
	int ret;

	if (!dev || !val)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, val, 1, 1);
}

/**
 * @brief Write a single byte to LTC4284 register
 * @param dev - Device descriptor
 * @param cmd - Command byte which contains register address
 * @param val - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_write_byte(struct ltc4284_dev *dev, uint8_t cmd, uint8_t val)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = cmd;
	buf[1] = val;

	return no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
}

/**
 * @brief Read a 16-bit word from LTC4284 (big-endian)
 * @param dev - Device descriptor
 * @param reg - Register address
 * @param val - Pointer to store read value
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_word(struct ltc4284_dev *dev, uint8_t reg, uint16_t *val)
{
	int ret;
	uint8_t buf[2];

	if (!dev || !val)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, 2, 1);
	if (ret)
		return ret;

	/* LTC4284 uses big-endian (MSB first) */
	*val = ((uint16_t)buf[0] << 8) | buf[1];

	return 0;
}

/**
 * @brief Write a 16-bit word to LTC4284 (big-endian)
 * @param dev - Device descriptor
 * @param reg - Register address
 * @param val - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_write_word(struct ltc4284_dev *dev, uint8_t reg, uint16_t val)
{
	uint8_t buf[3];

	if (!dev)
		return -EINVAL;

	buf[0] = reg;
	buf[1] = (uint8_t)(val >> 8);   /* MSB */
	buf[2] = (uint8_t)(val & 0xFF); /* LSB */

	return no_os_i2c_write(dev->i2c_desc, buf, 3, 1);
}

/**
 * @brief Update register bits with mask
 * @param dev - Device descriptor
 * @param reg - Register address
 * @param mask - Bit mask
 * @param val - New value for masked bits
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_update_bits(struct ltc4284_dev *dev, uint8_t reg,
			uint8_t mask, uint8_t val)
{
	int ret;
	uint8_t reg_val;

	if (!dev)
		return -EINVAL;

	ret = ltc4284_read_byte(dev, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= (val & mask);

	return ltc4284_write_byte(dev, reg, reg_val);
}

/**
 * @brief Initialize the LTC4284 device
 * @param device - Pointer to device descriptor pointer
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_init(struct ltc4284_dev **device,
		 const struct ltc4284_init_param *init_param)
{
	struct ltc4284_dev *dev;
	uint8_t status;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	if (init_param->i2c_init->slave_address < LTC4284_I2C_ADDR_0 ||
	    init_param->i2c_init->slave_address > LTC4284_I2C_ADDR_7)
		return -EINVAL;

	/* Datasheet Table 11: V_ILIM is a 4-bit DAC in 1 mV steps from 15 to 30 mV. */
	if (init_param->vsense_mv < LTC4284_VILIM_MIN_MV ||
	    init_param->vsense_mv > LTC4284_VILIM_MAX_MV)
		return -EINVAL;

	/* Dividers are consumed by every VIN/VDS/power read; a zero divider
	 * would produce zeroed telemetry silently.
	 */
	if (init_param->vpwr_divider == 0 || init_param->drain_divider == 0)
		return -EINVAL;

	if (init_param->rsense_uohm == 0)
		return -EINVAL;

	dev = (struct ltc4284_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto error_dev;

	dev->i2c_addr = init_param->i2c_init->slave_address;
	dev->rsense_uohm = init_param->rsense_uohm;
	dev->vpwr_divider = init_param->vpwr_divider;
	dev->drain_divider = init_param->drain_divider;
	dev->vsense_mv = init_param->vsense_mv;

	if (init_param->alert_gpio) {
		ret = no_os_gpio_get_optional(&dev->alert_gpio, init_param->alert_gpio);
		if (ret)
			goto error_i2c;

		if (dev->alert_gpio) {
			ret = no_os_gpio_direction_input(dev->alert_gpio);
			if (ret)
				goto error_gpio;
		}
	}

	ret = ltc4284_read_byte(dev, LTC4284_REG_SYSTEM_STATUS, &status);
	if (ret)
		goto error_gpio;

	*device = dev;

	return 0;

error_gpio:
	if (dev->alert_gpio)
		no_os_gpio_remove(dev->alert_gpio);
error_i2c:
	no_os_i2c_remove(dev->i2c_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Remove the LTC4284 device and free resources
 * @param dev - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_remove(struct ltc4284_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltc4284_enable_fet(dev, false);
	if (ret)
		pr_warning("ltc4284_remove: FET disable failed (%d), continuing cleanup\n",
			   ret);

	if (dev->alert_gpio)
		no_os_gpio_remove(dev->alert_gpio);

	no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);

	return 0;
}

/**
 * @brief Read input voltage (VPWR)
 * @param dev - Device descriptor
 * @param vin_mv - Pointer to store voltage in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_vin(struct ltc4284_dev *dev, uint32_t *vin_mv)
{
	int ret;
	uint16_t adc_code;
	uint32_t v_pin_uv;

	if (!dev || !vin_mv)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_VPWR, &adc_code);
	if (ret)
		return ret;

	/* Datasheet p.51: V_single_ended = code × 2.048 V / 65536.
	 * Then scale by the external board divider from bus → RTNS pin.
	 *
	 * NB: the intermediate product (code × 2_048_000) overflows uint32_t
	 * for code > ~2098 — a 48V bus over 40:1 already reads code ≈ 39000.
	 * Use uint64_t for the intermediate, then narrow at the end.
	 */
	v_pin_uv = (uint32_t)((uint64_t)adc_code * LTC4284_ADC_SINGLE_ENDED_FS_UV /
			      LTC4284_ADC_LEVELS);
	/* Promote to uint64_t so v_pin_uv * vpwr_divider does not overflow
	 * uint32_t at high divider ratios (v_pin_uv can reach ~2_048_000).
	 */
	*vin_mv = (uint32_t)((uint64_t)v_pin_uv * dev->vpwr_divider / MILLI);

	return 0;
}

/**
 * @brief Read input current (SENSE)
 * @param dev - Device descriptor
 * @param iin_ma - Pointer to store current in milliamps
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_iin(struct ltc4284_dev *dev, uint32_t *iin_ma)
{
	int ret;
	uint16_t adc_code;
	uint32_t sense_uv;

	if (!dev || !iin_ma)
		return -EINVAL;
	if (dev->rsense_uohm == 0)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_SENSE, &adc_code);
	if (ret)
		return ret;

	/* Datasheet p.51: I = code × 32.768 mV / (65536 × R_sense).
	 * Compute sense voltage in µV first, then divide by R_sense (µΩ) to get
	 * mA (µV / µΩ = A × 1e-6/1e-6 = A; but we want mA, so × 1000):
	 *   V_sense_uV = code × 32768 / 65536 = code / 2   (exact integer)
	 *   I_mA       = V_sense_uV × 1000 / R_sense_uohm
	 * Example: 50A on 312 µΩ → V_sense = 15600 µV → code = 31200 →
	 *          I_mA = 15600 × 1000 / 312 = 50000. ✓
	 */
	sense_uv = (uint32_t)adc_code * LTC4284_ADC_DIFFERENTIAL_FS_UV /
		   LTC4284_ADC_LEVELS;
	*iin_ma = sense_uv * MILLI / dev->rsense_uohm;

	return 0;
}

/**
 * @brief Read drain-to-source voltage across the external MOSFET
 *
 * Reads the DRAIN ADC register (0x65). The value is V_DRAIN referenced to
 * VEE (the chip's ground = supply-side -48V), which equals the V_DS across
 * the external hot-swap MOSFET. Under normal conduction V_DS ~= I * Rds_on
 * (a few mV to tens of mV). When the FET is off, V_DS rises to approximately
 * VIN, and inductive transients can push it further and peg the ADC at
 * full-scale (drain_divider * 2.048 V, e.g. 81.92 V for a 40:1 divider).
 *
 * @param dev - Device descriptor
 * @param vds_mv - Pointer to store V_DS in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_vds(struct ltc4284_dev *dev, uint32_t *vds_mv)
{
	int ret;
	uint16_t adc_code;
	uint32_t v_pin_uv;

	if (!dev || !vds_mv)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_DRAIN, &adc_code);
	if (ret)
		return ret;

	/* Same as read_vin, but through the DRAIN pin divider.
	 * uint64_t intermediate to avoid overflow (see read_vin note).
	 */
	v_pin_uv = (uint32_t)((uint64_t)adc_code * LTC4284_ADC_SINGLE_ENDED_FS_UV /
			      LTC4284_ADC_LEVELS);
	/* uint64_t promotion mirrors read_vin: v_pin_uv * drain_divider can
	 * overflow uint32_t at high divider ratios.
	 */
	*vds_mv = (uint32_t)((uint64_t)v_pin_uv * dev->drain_divider / MILLI);

	return 0;
}

/**
 * @brief Read output bus voltage delivered to the load
 *
 * Computed as VIN - V_DS. This is the actual bus voltage the load sees,
 * not a raw ADC register. When the FET is conducting, VOUT ~= VIN. When
 * the FET is off, VOUT collapses toward 0 (the load is disconnected).
 *
 * @param dev - Device descriptor
 * @param vout_mv - Pointer to store output voltage in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_vout(struct ltc4284_dev *dev, uint32_t *vout_mv)
{
	int ret;
	uint32_t vin_mv, vds_mv;

	if (!dev || !vout_mv)
		return -EINVAL;

	ret = ltc4284_read_vin(dev, &vin_mv);
	if (ret)
		return ret;

	ret = ltc4284_read_vds(dev, &vds_mv);
	if (ret)
		return ret;

	*vout_mv = (vin_mv > vds_mv) ? (vin_mv - vds_mv) : 0;

	return 0;
}

/**
 * @brief Read power consumption
 * @param dev - Device descriptor
 * @param power_mw - Pointer to store power in milliwatts
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_power(struct ltc4284_dev *dev, uint32_t *power_mw)
{
	int ret;
	uint16_t adc_code;
	uint64_t p_at_pin_uw;

	if (!dev || !power_mw)
		return -EINVAL;
	if (dev->rsense_uohm == 0)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_POWER, &adc_code);
	if (ret)
		return ret;

	/* Datasheet p.51:  P = code × 32.768 mV × 2.048 V / (65536 × R_sense)
	 * The chip internally multiplies ADC1 (current sense) and ADC2 (voltage)
	 * and left-justifies the product into a single 16-bit register — so the
	 * formula has ONE 65536 in the denominator, not two.
	 *
	 * Unit accounting (working in µV and µΩ so the micro cancels):
	 *   P_uW_at_pin = code × 32768 (uV) × 2048000 (uV) / (65536 × R_uohm)
	 *   P_mW_at_bus = P_uW_at_pin × vpwr_divider / 1000
	 *
	 * Assumes CONFIG_3.VPWR_SELECT = 0 (RTNS → input power). If flipped to
	 * DRNS (MOSFET power), replace vpwr_divider with drain_divider.
	 *
	 * 64-bit intermediate: 65535 × 32768 × 2_048_000 ≈ 4.4e15, fits comfortably.
	 */
	p_at_pin_uw = (uint64_t)adc_code * LTC4284_ADC_DIFFERENTIAL_FS_UV *
		      LTC4284_ADC_SINGLE_ENDED_FS_UV /
		      ((uint64_t)LTC4284_ADC_LEVELS * dev->rsense_uohm);
	*power_mw = (uint32_t)(p_at_pin_uw * dev->vpwr_divider / MILLI);

	return 0;
}

/**
 * @brief Read cumulative energy accumulator
 * @param dev - Device descriptor
 * @param energy_code - Pointer to store the raw 48-bit accumulator value
 * @return 0 in case of success, negative error code otherwise
 *
 * Returns the raw 48-bit ENERGY accumulator code from registers
 * 0x7A..0x7F (datasheet p.51), not a physical energy in millijoules.
 * The physical conversion requires the ADC conversion time (t_conv,
 * datasheet Table 12) and the sense-resistor value; callers can
 * observe accumulation over time but must apply the conversion
 * themselves.
 */
int ltc4284_read_energy(struct ltc4284_dev *dev, uint64_t *energy_code)
{
	int ret;
	uint8_t reg = LTC4284_REG_ENERGY_1;
	uint8_t buf[6];

	if (!dev || !energy_code)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, sizeof(buf), 1);
	if (ret)
		return ret;

	*energy_code = ((uint64_t)buf[0] << 40) | ((uint64_t)buf[1] << 32) |
		       ((uint64_t)buf[2] << 24) | ((uint64_t)buf[3] << 16) |
		       ((uint64_t)buf[4] << 8)  | (uint64_t)buf[5];

	return 0;
}

/**
 * @brief Enable or disable FET drivers
 * @param dev - Device descriptor
 * @param enable - True to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_enable_fet(struct ltc4284_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return ltc4284_update_bits(dev, LTC4284_REG_CONTROL_1,
				   LTC4284_CONTROL_1_ON,
				   enable ? LTC4284_CONTROL_1_ON : 0);
}

/**
 * @brief Clear all fault conditions
 * @param dev - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_clear_faults(struct ltc4284_dev *dev)
{
	if (!dev)
		return -EINVAL;

	/* Writing 0x00 to FAULT register clears all faults */
	return ltc4284_write_byte(dev, LTC4284_REG_FAULT, 0x00);
}

/**
 * @brief Read system status register
 * @param dev - Device descriptor
 * @param status - Pointer to store status byte
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_status(struct ltc4284_dev *dev, uint8_t *status)
{
	if (!dev || !status)
		return -EINVAL;

	return ltc4284_read_byte(dev, LTC4284_REG_SYSTEM_STATUS, status);
}

/**
 * @brief Get current fault conditions
 * @param dev - Device descriptor
 * @param faults - Pointer to store fault byte
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_get_fault(struct ltc4284_dev *dev, uint8_t *faults)
{
	if (!dev || !faults)
		return -EINVAL;

	return ltc4284_read_byte(dev, LTC4284_REG_FAULT, faults);
}

/**
 * @brief Arm the LTC4284 fault-log capture into EEPROM
 * @param dev - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 *
 * Sets FAULT_LOG_ENABLE (bit 7) in FAULT_LOG_CONTROL (0x90). The chip
 * captures the fault + ADC snapshot into EEPROM on the next fault
 * event; poll FAULT_LOG_DONE (bit 4) to detect completion. The EEPROM
 * write itself is signalled by EEPROM_BUSY in SYSTEM_STATUS.
 */
int ltc4284_arm_fault_log(struct ltc4284_dev *dev)
{
	int ret;
	uint8_t status;
	int timeout = 100; /* 100ms timeout */

	if (!dev)
		return -EINVAL;

	ret = ltc4284_write_byte(dev, LTC4284_REG_FAULT_LOG_CONTROL,
				 LTC4284_FAULT_LOG_ENABLE);
	if (ret)
		return ret;

	/* Wait for any in-flight EEPROM write to complete */
	while (timeout > 0) {
		ret = ltc4284_read_byte(dev, LTC4284_REG_SYSTEM_STATUS, &status);
		if (ret)
			return ret;

		if (!(status & LTC4284_SYSTEM_STATUS_EEPROM_BUSY))
			return 0;

		no_os_mdelay(1);
		timeout--;
	}

	return -ETIMEDOUT;
}

/**
 * @brief Restore configuration from EEPROM
 * @param dev - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_restore_from_eeprom(struct ltc4284_dev *dev)
{
	if (!dev)
		return -EINVAL;

	/* Restore from EEPROM by triggering a reboot */
	return ltc4284_write_byte(dev, LTC4284_REG_REBOOT, 0x01);
}

/**
 * @brief Set steady-state V_ILIM in millivolts
 * @param dev - Device descriptor
 * @param mv - Desired V_ILIM in millivolts (15..30 mV)
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_set_ilim_mv(struct ltc4284_dev *dev, uint8_t mv)
{
	uint8_t code;

	if (!dev)
		return -EINVAL;
	if (mv < LTC4284_VILIM_MIN_MV || mv > LTC4284_VILIM_MAX_MV)
		return -EINVAL;

	code = mv - LTC4284_VILIM_MIN_MV;

	return ltc4284_update_bits(dev, LTC4284_REG_CONFIG_1,
				   LTC4284_CONFIG_1_ILIM,
				   no_os_field_prep(LTC4284_CONFIG_1_ILIM,
						   code));
}

/**
 * @brief Set current-limit foldback factor
 * @param dev - Device descriptor
 * @param code - Foldback code (0: 100%, 1: 50%, 2: 20%, 3: 10%)
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_set_foldback(struct ltc4284_dev *dev, uint8_t code)
{
	if (!dev)
		return -EINVAL;
	if (code > 0x3)
		return -EINVAL;

	return ltc4284_update_bits(dev, LTC4284_REG_CONFIG_1,
				   LTC4284_CONFIG_1_FB,
				   no_os_field_prep(LTC4284_CONFIG_1_FB, code));
}

/**
 * @brief Set overcurrent retry policy
 * @param dev - Device descriptor
 * @param code - Retry code (0: latch-off, 1: 1 retry, 2: 7 retries, 3: unlimited)
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_set_oc_retry(struct ltc4284_dev *dev, uint8_t code)
{
	if (!dev)
		return -EINVAL;
	if (code > 0x3)
		return -EINVAL;

	return ltc4284_update_bits(dev, LTC4284_REG_CONTROL_2,
				   LTC4284_CONTROL_2_OC_RETRY,
				   no_os_field_prep(LTC4284_CONTROL_2_OC_RETRY,
						   code));
}

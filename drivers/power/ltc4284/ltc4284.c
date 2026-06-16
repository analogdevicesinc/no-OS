/***************************************************************************//**
 *   @file   ltc4284.c
 *   @brief  Implementation of LTC4284 Driver
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
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
 * @param reg - Register address
 * @param val - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_write_byte(struct ltc4284_dev *dev, uint8_t reg, uint8_t val)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = reg;
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

	if (init_param->i2c_addr < LTC4284_I2C_ADDR_0 ||
	    init_param->i2c_addr > LTC4284_I2C_ADDR_3)
		return -EINVAL;

	if (init_param->vsense_mv != 15 && init_param->vsense_mv != 20 &&
	    init_param->vsense_mv != 25 && init_param->vsense_mv != 30)
		return -EINVAL;

	dev = (struct ltc4284_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto error_dev;

	dev->i2c_addr = init_param->i2c_addr;
	dev->rsense_mohm = init_param->rsense_mohm;
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
		return ret;

	if (dev->alert_gpio)
		no_os_gpio_remove(dev->alert_gpio);

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Read input voltage (VPWR)
 * @param dev - Device descriptor
 * @param vin_mv - Pointer to store voltage in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_vin(struct ltc4284_dev *dev, uint16_t *vin_mv)
{
	int ret;
	uint16_t adc_code;

	if (!dev || !vin_mv)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_VPWR, &adc_code);
	if (ret)
		return ret;

	/* Convert ADC code to millivolts
	 * VIN = ADC_code * LSB (application-specific scaling)
	 * Default scaling: VIN_LSB = 32mV (32000uV)
	 */
	*vin_mv = (uint16_t)((uint32_t)adc_code * LTC4284_VPWR_LSB_UV / MILLI);

	return 0;
}

/**
 * @brief Read input current (SENSE)
 * @param dev - Device descriptor
 * @param iin_ma - Pointer to store current in milliamps
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_iin(struct ltc4284_dev *dev, uint16_t *iin_ma)
{
	int ret;
	uint16_t adc_code;
	uint32_t sense_uv;

	if (!dev || !iin_ma)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_SENSE, &adc_code);
	if (ret)
		return ret;

	/* Convert ADC code to sense voltage in microvolts
	 * SENSE_UV = (ADC_code * VSENSE_mV * 1000) / 256 (8-bit mode)
	 * or / 65536 (16-bit mode)
	 */
	sense_uv = ((uint32_t)adc_code * dev->vsense_mv * MILLI) /
		   LTC4284_ADC_8BIT_LEVELS;

	/* Convert sense voltage to current: I = V_SENSE / R_SENSE
	 * I_mA = (sense_uV / rsense_mohm)
	 */
	if (dev->rsense_mohm == 0)
		return -EINVAL;

	*iin_ma = (uint16_t)(sense_uv / dev->rsense_mohm);

	return 0;
}

/**
 * @brief Read output voltage (DRAIN)
 * @param dev - Device descriptor
 * @param vout_mv - Pointer to store voltage in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_vout(struct ltc4284_dev *dev, uint16_t *vout_mv)
{
	int ret;
	uint16_t adc_code;

	if (!dev || !vout_mv)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_DRAIN, &adc_code);
	if (ret)
		return ret;

	/* Convert ADC code to millivolts
	 * VOUT = ADC_code * LSB (application-specific scaling)
	 * Default scaling: DRAIN_LSB = 32mV (32000uV)
	 */
	*vout_mv = (uint16_t)((uint32_t)adc_code * LTC4284_DRAIN_LSB_UV / MILLI);

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

	if (!dev || !power_mw)
		return -EINVAL;

	ret = ltc4284_read_word(dev, LTC4284_REG_POWER, &adc_code);
	if (ret)
		return ret;

	/* Power register contains computed power value
	 * Power scaling depends on VPWR and SENSE scaling
	 * For now, return raw ADC code (application-specific scaling needed)
	 */
	*power_mw = (uint32_t)adc_code;

	return 0;
}

/**
 * @brief Read cumulative energy
 * @param dev - Device descriptor
 * @param energy_mj - Pointer to store energy in millijoules
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_energy(struct ltc4284_dev *dev, uint64_t *energy_mj)
{
	int ret;
	uint8_t buf[3];
	uint32_t energy_code;

	if (!dev || !energy_mj)
		return -EINVAL;

	/* Energy is a 24-bit value (3 bytes) */
	ret = no_os_i2c_write(dev->i2c_desc,
	(uint8_t[]) {
		LTC4284_REG_ENERGY
	}, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, 3, 1);
	if (ret)
		return ret;

	/* Assemble 24-bit value (big-endian) */
	energy_code = ((uint32_t)buf[0] << 16) |
		      ((uint32_t)buf[1] << 8) |
		      buf[2];

	/* Convert to millijoules (application-specific scaling needed) */
	*energy_mj = (uint64_t)energy_code;

	return 0;
}

/**
 * @brief Read temperature
 * @param dev - Device descriptor
 * @param temp_mc - Temperature in milli-degrees Celsius
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_read_temperature(struct ltc4284_dev *dev, int16_t *temp_mc)
{
	int ret;
	uint16_t adc_code;

	if (!dev || !temp_mc)
		return -EINVAL;

	/* Read temperature from ADIO3 register (2-byte ADC value)
	 * TODO: Implement proper NTC thermistor conversion formula
	 * based on datasheet specifications */
	ret = ltc4284_read_word(dev, LTC4284_REG_ADIO3, &adc_code);
	if (ret)
		return ret;

	/* Placeholder conversion - needs proper thermistor formula
	 * For now, return a nominal room temperature value
	 * Actual implementation requires NTC beta value and voltage divider specs */
	*temp_mc = 25000;  /* 25.000 degrees Celsius */

	return 0;
}

/**
 * @brief Set current limit
 * @param dev - Device descriptor
 * @param ilim_code - Current limit code (0-15)
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_set_current_limit(struct ltc4284_dev *dev, uint8_t ilim_code)
{
	if (!dev)
		return -EINVAL;

	/* Validate range (4-bit value) */
	if (ilim_code > 15)
		return -EINVAL;

	return ltc4284_update_bits(dev, LTC4284_REG_CONFIG_1,
				   LTC4284_CONFIG_1_ILIM,
				   ilim_code << 4);
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
 * @brief Store current configuration to EEPROM
 * @param dev - Device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_store_config(struct ltc4284_dev *dev)
{
	int ret;
	uint8_t status;
	int timeout = 100; /* 100ms timeout */

	if (!dev)
		return -EINVAL;

	/* Trigger EEPROM store by writing to SNAPSHOT register */
	ret = ltc4284_write_byte(dev, LTC4284_REG_SNAPSHOT, 0x01);
	if (ret)
		return ret;

	/* Wait for EEPROM write to complete */
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
int ltc4284_restore_config(struct ltc4284_dev *dev)
{
	if (!dev)
		return -EINVAL;

	/* Restore from EEPROM by triggering a reboot */
	return ltc4284_write_byte(dev, LTC4284_REG_REBOOT, 0x01);
}

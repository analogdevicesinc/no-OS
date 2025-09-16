/*******************************************************************************
 * @file    max20362.c
 * @brief   Implementation of MAX20362 Micro-Battery PMIC driver
 * @author  Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include "max20362.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/**
 * @brief Write register
 * @param dev - Device structure
 * @param reg - Register address
 * @param val - Value to write
 * @return 0 on success, negative error code otherwise
 */
int max20362_reg_write(struct max20362_dev *dev, uint8_t reg, uint8_t val)
{
	uint8_t write_buf[2];
	int ret;

	if (!dev || !dev->i2c_desc)
		return -ENODEV;

	write_buf[0] = reg;
	write_buf[1] = val;

	return no_os_i2c_write(dev->i2c_desc, write_buf, 2, 1);
}

/**
 * @brief Read register
 * @param dev - Device structure
 * @param reg - Register address
 * @param val - Pointer to store read value
 * @return 0 on success, negative error code otherwise
 */
int max20362_reg_read(struct max20362_dev *dev, uint8_t reg, uint8_t *val)
{
	int ret;

	if (!dev || !dev->i2c_desc)
		return -ENODEV;

	if (!val)
		return -EINVAL;

	/* MAX20362 requires register address write followed by data read */
	ret = no_os_i2c_write(dev->i2c_desc, &reg, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, val, 1, 1);
}

/**
 * @brief Modify register bits
 * @param dev - Device structure
 * @param reg - Register address
 * @param mask - Bit mask
 * @param val - New value for masked bits
 * @return 0 on success, negative error code otherwise
 */
int max20362_reg_update_bits(struct max20362_dev *dev, uint8_t reg,
			     uint8_t mask, uint8_t val)
{
	uint8_t reg_val;
	int ret;

	if (!dev)
		return -ENODEV;

	ret = max20362_reg_read(dev, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= (val & mask);

	return max20362_reg_write(dev, reg, reg_val);
}

/**
 * @brief Initialize MAX20362 device
 * @param device - Pointer to device structure pointer
 * @param init_param - Initialization parameters
 * @return 0 on success, negative error code otherwise
 */
int max20362_init(struct max20362_dev **device,
		  const struct max20362_init_param *init_param)
{
	struct max20362_dev *dev;
	int ret;
	uint8_t chip_id;

	if (!device)
		return -ENODEV;

	if (!init_param)
		return -EINVAL;

	dev = (struct max20362_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret) {
		no_os_free(dev);
		return ret;
	}

	/* Read and verify chip ID */
	ret = max20362_reg_read(dev, MAX20362_REG_CHIP_ID, &chip_id);
	if (ret)
		goto error;

	if (chip_id != MAX20362_CHIP_ID_VAL) {
		ret = -ENODEV;
		goto error;
	}

	/* Clear all interrupts */
	ret = max20362_clear_all_interrupts(dev);
	if (ret)
		goto error;

	/* Configure input current limit */
	if (init_param->input_current_limit_ma) {
		ret = max20362_set_input_current_limit(dev,
						       init_param->input_current_limit_ma);
		if (ret)
			goto error;
	}

	/* Configure battery voltage droop */
	ret = max20362_set_bbat_vdrop(dev, init_param->bbat_vdrop);
	if (ret)
		goto error;

	/* Configure buck-boost voltage */
	if (init_param->buck_boost_voltage_uv) {
		ret = max20362_set_buck_boost_voltage(dev,
						      init_param->buck_boost_voltage_uv);
		if (ret)
			goto error;
	}

	/* Configure capacitor voltage */
	if (init_param->cap_voltage_uv) {
		/* Default to 250mV steps */
		ret = max20362_set_cap_voltage(dev, init_param->cap_voltage_uv,
					       MAX20362_VCAP_STEP_250MV);
		if (ret)
			goto error;
	}

	/* Configure LDO voltage */
	if (init_param->ldo_voltage_uv) {
		ret = max20362_set_ldo_voltage(dev, init_param->ldo_voltage_uv);
		if (ret)
			goto error;
	}

	/* Enable buck-boost if requested */
	if (init_param->buck_boost_enable) {
		ret = max20362_enable_buck_boost(dev, true);
		if (ret)
			goto error;
	}

	/* Enable LDO if requested */
	if (init_param->ldo_enable) {
		ret = max20362_enable_ldo(dev, true);
		if (ret)
			goto error;
	}

	*device = dev;
	return 0;

error:
	no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free resources and remove device
 * @param dev - Device structure pointer
 * @return 0 on success, negative error code otherwise
 */
int max20362_remove(struct max20362_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	/* Disable buck-boost and LDO */
	max20362_enable_buck_boost(dev, false);
	max20362_enable_ldo(dev, false);

	/* Clear interrupts */
	max20362_clear_all_interrupts(dev);

	ret = no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);

	return ret;
}

/**
 * @brief Enable/disable buck-boost converter
 * @param dev - Device structure
 * @param enable - Enable flag
 * @return 0 on success, negative error code otherwise
 */
int max20362_enable_buck_boost(struct max20362_dev *dev, bool enable)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_update_bits(dev, MAX20362_REG_BBST_CFG0,
					MAX20362_BBSTCFG_BBSTENA_MSK,
					enable ? MAX20362_BBSTCFG_BBSTENA_MSK : 0);
}

/**
 * @brief Set buck-boost output voltage
 * @param dev - Device structure
 * @param microvolts - Voltage in microvolts (1.5V-5.5V, 50mV steps)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_buck_boost_voltage(struct max20362_dev *dev,
				    uint32_t microvolts)
{
	int ret;
	uint8_t reg_val;

	if (!dev)
		return -ENODEV;

	/* Check voltage range */
	if (microvolts < MAX20362_BBOUT_MIN_UV ||
	    microvolts > MAX20362_BBOUT_MAX_UV)
		return -EINVAL;

	/* Calculate register value: (V - 1.5V) / 50mV */
	reg_val = (microvolts - MAX20362_BBOUT_MIN_UV) / MAX20362_BBOUT_STEP_UV;

	/* Unlock register for writing */
	ret = max20362_unlock_registers(dev);
	if (ret)
		return ret;

	/* Write voltage setting */
	ret = max20362_reg_write(dev, MAX20362_REG_BBST_VSET,
				 reg_val & MAX20362_BBSTVSET_MSK);

	/* Lock register again */
	max20362_lock_registers(dev);

	return ret;
}

/**
 * @brief Configure buck-boost operating parameters
 * @param dev - Device structure
 * @param half_bw - Half bandwidth mode enable
 * @param low_emi - Low EMI mode enable
 * @param fast_fpwm - Fast FPWM mode enable
 * @return 0 on success, negative error code otherwise
 */
int max20362_config_buck_boost_mode(struct max20362_dev *dev,
				    bool half_bw, bool low_emi, bool fast_fpwm)
{
	uint8_t mask = MAX20362_BBSTCFG_BBHALFBW_MSK |
		       MAX20362_BBSTCFG_BBSTLOWEMI_MSK |
		       MAX20362_BBSTCFG_BBSTFAST_MSK;
	uint8_t val = (half_bw ? MAX20362_BBSTCFG_BBHALFBW_MSK : 0) |
		      (low_emi ? MAX20362_BBSTCFG_BBSTLOWEMI_MSK : 0) |
		      (fast_fpwm ? MAX20362_BBSTCFG_BBSTFAST_MSK : 0);

	if (!dev)
		return -ENODEV;

	return max20362_reg_update_bits(dev, MAX20362_REG_BBST_CFG0, mask, val);
}

/**
 * @brief Configure buck-boost discharge modes
 * @param dev - Device structure
 * @param active_discharge - Active discharge enable
 * @param passive_discharge - Passive discharge enable
 * @return 0 on success, negative error code otherwise
 */
int max20362_config_buck_boost_discharge(struct max20362_dev *dev,
		bool active_discharge, bool passive_discharge)
{
	uint8_t mask = MAX20362_BBSTCFG_BBSTACTDSC_MSK |
		       MAX20362_BBSTCFG_BBSTPSVDSC_MSK;
	uint8_t val = (active_discharge ? MAX20362_BBSTCFG_BBSTACTDSC_MSK : 0) |
		      (passive_discharge ? MAX20362_BBSTCFG_BBSTPSVDSC_MSK : 0);

	if (!dev)
		return -ENODEV;

	return max20362_reg_update_bits(dev, MAX20362_REG_BBST_CFG0, mask, val);
}

/**
 * @brief Enable/disable LDO regulator
 * @param dev - Device structure
 * @param enable - Enable flag
 * @return 0 on success, negative error code otherwise
 */
int max20362_enable_ldo(struct max20362_dev *dev, bool enable)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_update_bits(dev, MAX20362_REG_LDO_CFG,
					MAX20362_LDOCFG_ENA_MSK,
					enable ? MAX20362_LDOCFG_ENA_MSK : 0);
}

/**
 * @brief Set LDO output voltage
 * @param dev - Device structure
 * @param microvolts - Voltage in microvolts (0.9V-4.0V, 100mV steps)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_ldo_voltage(struct max20362_dev *dev, uint32_t microvolts)
{
	int ret;
	uint8_t ldo_cfg_val;
	uint8_t reg_val;

	if (!dev)
		return -ENODEV;

	/* Check voltage range */
	if (microvolts < MAX20362_LDO_MIN_UV ||
	    microvolts > MAX20362_LDO_MAX_UV)
		return -EINVAL;

	/* Calculate register value: (V - 0.9V) / 100mV */
	reg_val = (microvolts - MAX20362_LDO_MIN_UV) / MAX20362_LDO_STEP_UV;

	/* Save configuration */
	ret = max20362_reg_read(dev, MAX20362_REG_LDO_CFG, &ldo_cfg_val);
	if (ret)
		return ret;

	ret = max20362_enable_ldo(dev, false);
	if (ret)
		return ret;

	ret = max20362_reg_write(dev, MAX20362_REG_LDO_VSET,
				 reg_val & MAX20362_LDOVSET_MSK);
	if (ret)
		return ret;

	/* LDO voltage change is only effective after toggling LDO enable */
	ret = max20362_enable_ldo(dev, true);
	if (ret)
		return ret;

	/* Restore LDO enable configuration */
	return max20362_enable_ldo(dev, ldo_cfg_val & MAX20362_LDOCFG_ENA_MSK);
}

/**
 * @brief Configure LDO operating modes
 * @param dev - Device structure
 * @param low_iq_mode - Low quiescent current mode
 * @param passive_discharge - Passive discharge enable
 * @param active_discharge - Active discharge enable
 * @return 0 on success, negative error code otherwise
 */
int max20362_config_ldo_mode(struct max20362_dev *dev, bool low_iq_mode,
			     bool passive_discharge, bool active_discharge)
{
	uint8_t mask = MAX20362_LDOCFG_LOWIQ_MSK |
		       MAX20362_LDOCFG_PSV_MSK |
		       MAX20362_LDOCFG_ACT_MSK;
	uint8_t val = (low_iq_mode ? MAX20362_LDOCFG_LOWIQ_MSK : 0) |
		      (passive_discharge ? MAX20362_LDOCFG_PSV_MSK : 0) |
		      (active_discharge ? MAX20362_LDOCFG_ACT_MSK : 0);

	if (!dev)
		return -ENODEV;

	return max20362_reg_update_bits(dev, MAX20362_REG_LDO_CFG, mask, val);
}

/**
 * @brief Set LDO input source
 * @param dev - Device structure
 * @param source - LDO input source enum
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_ldo_input_source(struct max20362_dev *dev,
				  enum max20362_ldo_source source)
{
	int ret;
	uint8_t val = (source == MAX20362_LDO_SOURCE_BBOUT) ?
		      MAX20362_LDOCFG_BBSTSUP_MSK : 0;

	if (!dev)
		return -ENODEV;

	if (source < MAX20362_LDO_SOURCE_BBOUT || source > MAX20362_LDO_SOURCE_BATT)
		return -EINVAL;

	ret = max20362_reg_update_bits(dev, MAX20362_REG_LDO_CFG,
				       MAX20362_LDOCFG_BBSTSUP_MSK, val);
	if (!ret)
		dev->ldo_source = source;

	return ret;
}

/**
 * @brief Set DVS operating mode
 * @param dev - Device structure
 * @param source - DVS source enum
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_dvs_mode(struct max20362_dev *dev,
			  enum max20362_dvs_source source)
{
	static const uint8_t mode_vals[] = {
		[MAX20362_DVS_SOURCE_I2C] = MAX20362_DVS_I2C_MODE,
		[MAX20362_DVS_SOURCE_PSPI] = MAX20362_DVS_PSPI_MODE,
		[MAX20362_DVS_SOURCE_ROUND_ROBIN] = MAX20362_DVS_AUTO_MODE
	};
	int ret;
	uint8_t mask = MAX20362_DVSCFG_DVSSOURCE_MSK | MAX20362_DVSCFG_RRENA_MSK;
	uint8_t val, rr_enable;

	if (!dev)
		return -ENODEV;

	if (source >= NO_OS_ARRAY_SIZE(mode_vals))
		return -EINVAL;

	rr_enable = (source == MAX20362_DVS_SOURCE_ROUND_ROBIN) ? 1 : 0;
	val = no_os_field_prep(MAX20362_DVSCFG_DVSSOURCE_MSK, mode_vals[source]) |
	      no_os_field_prep(MAX20362_DVSCFG_RRENA_MSK, rr_enable);

	ret = max20362_reg_update_bits(dev, MAX20362_REG_DVS_CFG, mask, val);
	if (ret)
		return ret;

	/* Wait 300us for DVS mode change per datasheet recommendation. */
	no_os_udelay(MAX20362_DVS_SETTLE_DELAY_US);

	return 0;
}

/**
 * @brief Configure DVS round-robin voltage table
 * @param dev - Device structure
 * @param voltages_uv - Array of voltages in microvolts
 * @param count - Number of voltages (1-20)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_dvs_rr_table(struct max20362_dev *dev,
			      const uint32_t *voltages_uv, uint8_t count)
{
	int ret;
	int i;
	uint8_t reg_addr;
	uint8_t reg_val;

	if (!dev)
		return -ENODEV;

	if (!voltages_uv)
		return -EINVAL;

	/* MAX20362 supports up to MAX20362_RR_MAX_VOLTAGES RR voltages */
	if (count < MAX20362_RR_MIN_COUNT || count > MAX20362_RR_MAX_VOLTAGES)
		return -EINVAL;

	ret = max20362_unlock_registers(dev);
	if (ret)
		return ret;

	/* Program each voltage in the RR table */
	for (i = 0; i < count; i++) {
		/* Check voltage range */
		if (voltages_uv[i] < MAX20362_BBOUT_MIN_UV ||
		    voltages_uv[i] > MAX20362_BBOUT_MAX_UV) {
			ret = -EINVAL;
			goto error;
		}

		/* Calculate register value */
		reg_val = (voltages_uv[i] - MAX20362_BBOUT_MIN_UV) /
			  MAX20362_BBOUT_STEP_UV;

		/* RR voltage registers start at 0x20 */
		reg_addr = MAX20362_RR_VSET_ADDR(i);

		ret = max20362_reg_write(dev, reg_addr, reg_val & MAX20362_VOLTAGE_MASK_7BIT);
		if (ret)
			goto error;
	}

	/* Configure RR size in RRCfg1 register */
	ret = max20362_reg_update_bits(dev, MAX20362_REG_RR_CFG1,
				       MAX20362_RRCFG1_RRSIZE_MSK,
				       (count - MAX20362_REGISTER_COUNT_OFFSET) & MAX20362_RRCFG1_RRSIZE_MSK);
	if (ret)
		/* Jump over any code that might be added after this */
		goto error;

error:
	/* Lock register again (ignore return value to preserve original error) */
	max20362_lock_registers(dev);
	if (ret)
		return ret;

	/* Enable round-robin mode */
	return max20362_reg_update_bits(dev, MAX20362_REG_DVS_CFG,
					MAX20362_DVSCFG_RRENA_MSK, MAX20362_DVSCFG_RRENA_MSK);
}

/**
 * @brief Set energy storage capacitor voltage
 * @param dev - Device structure
 * @param microvolts - Voltage in microvolts (1.6V-9.5V)
 * @param step_size - Step size (0=500mV, 1=250mV, 2=125mV)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_cap_voltage(struct max20362_dev *dev, uint32_t microvolts,
			     uint8_t step_size)
{
	uint8_t reg_val = 0;
	uint8_t vset_val = 0;
	uint8_t mask, val;

	if (!dev)
		return -ENODEV;

	/* Check step size */
	if (step_size > MAX20362_VCAP_STEP_125MV)
		return -EINVAL;

	/* Check voltage range */
	if (microvolts < MAX20362_VCAP_MIN_UV ||
	    microvolts > MAX20362_VCAP_MAX_UV)
		return -EINVAL;

	/* Calculate VSET value based on step size
	 * Per datasheet, the mapping depends on BBstStepCap value */
	switch (step_size) {
	case MAX20362_VCAP_STEP_500MV: /* 500mV steps */
		if (microvolts < MAX20362_VCAP_500MV_MIN_UV) {
			return -EINVAL; /* Below minimum range */
		}
		vset_val = (microvolts - MAX20362_VCAP_500MV_MIN_UV) /
			   MAX20362_VCAP_500MV_STEP_UV;
		if (vset_val > MAX20362_VCAP_500MV_VSET_MAX)
			vset_val = MAX20362_VCAP_500MV_VSET_MAX; /* Max 9.5V */
		break;
	case MAX20362_VCAP_STEP_250MV: /* 250mV steps */
		if (microvolts < MAX20362_VCAP_250MV_MIN_UV) {
			return -EINVAL; /* Below minimum range */
		}
		vset_val = (microvolts - MAX20362_VCAP_250MV_MIN_UV) /
			   MAX20362_VCAP_250MV_STEP_UV;
		if (vset_val > MAX20362_VCAP_250MV_VSET_MAX)
			vset_val = MAX20362_VCAP_250MV_VSET_MAX;
		break;
	case MAX20362_VCAP_STEP_125MV: /* 125mV steps */
		if (microvolts < MAX20362_VCAP_125MV_MIN_UV) {
			return -EINVAL; /* Below minimum range */
		} else if (microvolts <= MAX20362_VCAP_125MV_UPPER_LIMIT_UV) {
			vset_val = MAX20362_VCAP_125MV_BASE_OFFSET +
				   ((microvolts - MAX20362_VCAP_125MV_MIN_UV) /
				    MAX20362_VCAP_125MV_STEP_UV);
		} else {
			/* Values above 2.9V use max register value for this mode */
			vset_val = MAX20362_VCAP_125MV_VSET_MAX; /* Max value */
		}
		break;
	}

	/* Build register value using field_prep */
	mask = MAX20362_CAPVSET_STEP_MSK | MAX20362_CAPVSET_VSET_MSK;
	val = no_os_field_prep(MAX20362_CAPVSET_STEP_MSK, step_size) |
	      no_os_field_prep(MAX20362_CAPVSET_VSET_MSK, vset_val);

	return max20362_reg_update_bits(dev, MAX20362_REG_CAP_VSET, mask, val);
}

/**
 * @brief Set input current limit
 * @param dev - Device structure
 * @param ma - Current limit in milliamps (5-50mA)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_input_current_limit(struct max20362_dev *dev, uint8_t ma)
{
	uint8_t reg_val;

	if (!dev)
		return -ENODEV;

	/* Check current range */
	if (ma < MAX20362_ILIM_MIN_MA || ma > MAX20362_ILIM_MAX_MA)
		return -EINVAL;

	/* Calculate register value: direct mapping, 1mA steps */
	reg_val = ma - MAX20362_ILIM_MIN_MA;

	return max20362_reg_update_bits(dev, MAX20362_REG_IGN_CFG,
					MAX20362_IGNCFG_ILIM_MSK, reg_val);
}

/**
 * @brief Set battery voltage droop
 * @param dev - Device structure
 * @param vdrop - Voltage droop enum value
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_bbat_vdrop(struct max20362_dev *dev,
			    enum max20362_bbat_vdrop vdrop)
{
	if (!dev)
		return -ENODEV;

	if (vdrop > MAX20362_BBAT_VDROP_200MV)
		return -EINVAL;

	/* Enum values map directly to register bits */
	return max20362_reg_update_bits(dev, MAX20362_REG_IGN_CFG,
					MAX20362_IGNCFG_BBVDROP_MSK,
					no_os_field_prep(MAX20362_IGNCFG_BBVDROP_MSK, vdrop));
}

/**
 * @brief Read fault status flags
 * @param dev - Device structure
 * @param status - Pointer to store status register value
 * @return 0 on success, negative error code otherwise
 */
int max20362_get_status(struct max20362_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_read(dev, MAX20362_REG_STATUS, status);
}

/**
 * @brief Read main interrupt status
 * @param dev - Device structure
 * @param status - Status byte pointer
 * @return 0 on success, negative error code otherwise
 */
int max20362_get_int_status(struct max20362_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_read(dev, MAX20362_REG_INT, status);
}

/**
 * @brief Read LDO interrupt status
 * @param dev - Device structure
 * @param status - Status byte pointer
 * @return 0 on success, negative error code otherwise
 */
int max20362_get_ldo_int_status(struct max20362_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_read(dev, MAX20362_REG_LDO_INT, status);
}

/**
 * @brief Read Ingenuity interrupt status
 * @param dev - Device structure
 * @param status - Status byte pointer
 * @return 0 on success, negative error code otherwise
 */
int max20362_get_ingen_int_status(struct max20362_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_read(dev, MAX20362_REG_INGEN_INT, status);
}

/**
 * @brief Set main interrupt mask
 * @param dev - Device structure
 * @param mask - Interrupt mask (1=mask/disable, 0=unmask/enable)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_int_mask(struct max20362_dev *dev, uint8_t mask)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_write(dev, MAX20362_REG_INT_MASK, mask);
}

/**
 * @brief Set LDO interrupt mask
 * @param dev - Device structure
 * @param mask - Interrupt mask (1=mask/disable, 0=unmask/enable)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_ldo_int_mask(struct max20362_dev *dev, uint8_t mask)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_write(dev, MAX20362_REG_LDO_INT_MASK, mask);
}

/**
 * @brief Set Ingenuity interrupt mask
 * @param dev - Device structure
 * @param mask - Interrupt mask (1=mask/disable, 0=unmask/enable)
 * @return 0 on success, negative error code otherwise
 */
int max20362_set_ingen_int_mask(struct max20362_dev *dev, uint8_t mask)
{
	if (!dev)
		return -ENODEV;

	return max20362_reg_write(dev, MAX20362_REG_INGEN_INT_MASK, mask);
}

/**
 * @brief Clear all interrupt flags
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max20362_clear_all_interrupts(struct max20362_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	/* Clear main interrupts by writing 1 to all bits */
	ret = max20362_reg_write(dev, MAX20362_REG_INT, MAX20362_INTERRUPT_CLEAR_ALL);
	if (ret)
		return ret;

	/* Clear LDO interrupts */
	ret = max20362_reg_write(dev, MAX20362_REG_LDO_INT,
				 MAX20362_INTERRUPT_CLEAR_ALL);
	if (ret)
		return ret;

	/* Clear Ingenuity interrupts */
	return max20362_reg_write(dev, MAX20362_REG_INGEN_INT,
				  MAX20362_INTERRUPT_CLEAR_ALL);
}

/**
 * @brief Unlock write protection for protected registers
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max20362_unlock_registers(struct max20362_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	/* First ensure BBLck bit is 0 in LOCK_MSK register */
	ret = max20362_reg_write(dev, MAX20362_REG_LOCK_MSK,
				 MAX20362_LOCK_MASK_UNLOCKED);
	if (ret)
		return ret;

	/* Write unlock password */
	return max20362_reg_write(dev, MAX20362_REG_LOCK_UNLOCK,
				  MAX20362_UNLOCK_PASSWORD);
}

/**
 * @brief Lock write protection for protected registers
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max20362_lock_registers(struct max20362_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	/* Write lock password */
	ret = max20362_reg_write(dev, MAX20362_REG_LOCK_UNLOCK,
				 MAX20362_LOCK_PASSWORD);
	if (ret)
		return ret;

	/* Set BBLck bit to 1 in LOCK_MSK register */
	return max20362_reg_write(dev, MAX20362_REG_LOCK_MSK,
				  MAX20362_LOCK_MASK_LOCKED);
}

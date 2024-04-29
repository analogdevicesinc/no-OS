/*******************************************************************************
*   @file   ltc2992.c
*   @brief  Implementation of LTC2992 Driver
*   @authors Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"
#include "no_os_units.h"

#include "ltc2992.h"

static const uint32_t sense_mv_lsb[2] = {25, 400};
static const uint32_t dsense_nv_lsb[2] = {12500, 200000};
static const uint32_t gpio_uv_lsb[2] = {500, 8000};
static const uint8_t sshift[2] = {4, 8};
static const uint8_t pshift[2] = {0, 8};

static const struct ltc2992_regmap gpio_regmap[6] = {
	{
		.data = LTC2992_REG_G1,
		.max = LTC2992_REG_G1_MAX,
		.min = LTC2992_REG_G1_MIN,
		.max_thresh = LTC2992_REG_G1_MAX_THRESH,
		.min_thresh = LTC2992_REG_G1_MIN_THRESH,
		.fault = LTC2992_REG_FAULT1,
		.alert = LTC2992_REG_ALERT1,
		.fault_alert_mask = LTC2992_G1_FAULT_ALERT_MSK,
		.ctrl = LTC2992_REG_GPIO_IO_CTRL,
		.ctrl_mask = LTC2992_G1_CTRL_BIT,
	},
	{
		.data = LTC2992_REG_G2,
		.max = LTC2992_REG_G2_MAX,
		.min = LTC2992_REG_G2_MIN,
		.max_thresh = LTC2992_REG_G2_MAX_THRESH,
		.min_thresh = LTC2992_REG_G2_MIN_THRESH,
		.fault = LTC2992_REG_FAULT2,
		.alert = LTC2992_REG_ALERT2,
		.fault_alert_mask = LTC2992_G2_FAULT_ALERT_MSK,
		.ctrl = LTC2992_REG_GPIO_IO_CTRL,
		.ctrl_mask = LTC2992_G2_CTRL_BIT,
	},
	{
		.data = LTC2992_REG_G3,
		.max = LTC2992_REG_G3_MAX,
		.min = LTC2992_REG_G3_MIN,
		.max_thresh = LTC2992_REG_G3_MAX_THRESH,
		.min_thresh = LTC2992_REG_G3_MIN_THRESH,
		.fault = LTC2992_REG_FAULT3,
		.alert = LTC2992_REG_ALERT3,
		.fault_alert_mask = LTC2992_G3_FAULT_ALERT_MSK,
		.ctrl = LTC2992_REG_GPIO_IO_CTRL,
		.ctrl_mask = LTC2992_G3_CTRL_BIT,
	},
	{
		.data = LTC2992_REG_G4,
		.max = LTC2992_REG_G4_MAX,
		.min = LTC2992_REG_G4_MIN,
		.max_thresh = LTC2992_REG_G4_MAX_THRESH,
		.min_thresh = LTC2992_REG_G4_MIN_THRESH,
		.fault = LTC2992_REG_FAULT3,
		.alert = LTC2992_REG_ALERT3,
		.fault_alert_mask = LTC2992_G4_FAULT_ALERT_MSK,
		.ctrl = LTC2992_REG_GPIO4_CTRL,
		.ctrl_mask = LTC2992_G4_CTRL_BIT,
	},
};

/**
 * @brief Generate device I2C address based on ADR pin states.
 * @param init_param - Initialization parameter.
 * @param a0 - ADR0 state.
 * 	       Example: LTC2992_ADR_HIGH - ADR at high logic level.
 * 			LTC2992_ADR_LOW - ADR at ground..
 * 			LTC2992_ADR_NC - ADR not connected.
 * @param a1 - ADR1 state.
 * 	       Example: LTC2992_ADR_HIGH - ADR at high logic level.
 * 			LTC2992_ADR_LOW - ADR at ground.
 * 			LTC2992_ADR_NC - ADR not connected.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_addr_gen(struct ltc2992_init_param *init_param,
		     enum ltc2992_adr_state a0,
		     enum ltc2992_adr_state a1)
{
	a0 = no_os_clamp_t(uint8_t, a0, LTC2992_ADR_HIGH, LTC2992_ADR_NC);
	a1 = no_os_clamp_t(uint8_t, a1, LTC2992_ADR_HIGH, LTC2992_ADR_NC);

	switch (a0) {
	case LTC2992_ADR_HIGH:
		switch (a1) {
		case LTC2992_ADR_HIGH:
			init_param->i2c_init.slave_address = 0x69;
			return 0x69;
		case LTC2992_ADR_LOW:
			init_param->i2c_init.slave_address = 0x67;
			return 0x67;
		case LTC2992_ADR_NC:
			init_param->i2c_init.slave_address = 0x6D;
			return 0x6D;
		default:
			return -EINVAL;
		}
	case LTC2992_ADR_LOW:
		switch (a1) {
		case LTC2992_ADR_HIGH:
			init_param->i2c_init.slave_address = 0x6C;
			return 0x6C;
		case LTC2992_ADR_LOW:
			init_param->i2c_init.slave_address = 0x6F;
			return 0x6F;
		case LTC2992_ADR_NC:
			init_param->i2c_init.slave_address = 0x6E;
			return 0x6E;
		default:
			return -EINVAL;
		}
	case LTC2992_ADR_NC:
		switch (a1) {
		case LTC2992_ADR_HIGH:
			init_param->i2c_init.slave_address = 0x68;
			return 0x68;
		case LTC2992_ADR_LOW:
			init_param->i2c_init.slave_address = 0x6B;
			return 0x6B;
		case LTC2992_ADR_NC:
			init_param->i2c_init.slave_address = 0x6A;
			return 0x6A;
		default:
			return -EINVAL;
		}
	default:
		return -EINVAL;
	}
}

/**
 * @brief Initialize communication peripheral for the device.
 * @param device - The device structure.
 * @param init_param - Initial parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_init(struct ltc2992_dev **device,
		 struct ltc2992_init_param *init_param)
{
	int ret;
	struct ltc2992_dev *dev;
	uint32_t data;

	if (!device)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(struct ltc2992_dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto i2c_err;

	ret = no_os_gpio_get_optional(&dev->alert_gpio_desc,
				      &init_param->alert_gpio_init);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->alert_gpio_desc);
	if (ret)
		goto dev_err;

	ret = ltc2992_read_reg(dev, LTC2992_REG_MFR_SPECIAL_ID, &data, 2);
	if (ret)
		goto dev_err;

	if (data != LTC2992_MFR_ID_VALUE) {
		ret = -EIO;
		goto dev_err;
	}

	ret = ltc2992_write_reg(dev, LTC2992_REG_CTRLB,
				LTC2992_PEAK_RST_BIT | LTC2992_RESET_BIT, 1);
	if (ret)
		goto dev_err;

	data = no_os_field_prep(LTC2992_OFFSET_CAL_MSK,
				init_param->calibration) |
	       no_os_field_prep(LTC2992_MEASURE_MODE_MSK,
				init_param->measure_mode) |
	       no_os_field_prep(LTC2992_VSEL_CON_MSK,
				init_param->continuous_vsel) |
	       no_os_field_prep(LTC2992_VSEL_SNAP_MSK,
				init_param->snapshot_vsel);

	ret = ltc2992_write_reg(dev, LTC2992_REG_CTRLA, data, 1);
	if (ret)
		goto dev_err;

	data = no_os_field_prep(LTC2992_ALERT_CLR_BIT,
				init_param->alert_clear) |
	       no_os_field_prep(LTC2992_READ_CTRL_BIT,
				init_param->fault_clear_on_read) |
	       no_os_field_prep(LTC2992_STUCK_BUS_TIMEOUT_BIT,
				init_param->stuck_bus_timer_wakeup_en);

	ret = ltc2992_write_reg(dev, LTC2992_REG_CTRLB, data, 1);
	if (ret)
		goto dev_err;

	data = init_param->resolution << LTC2992_RESOLUTION_POS;
	ret = ltc2992_write_reg(dev, LTC2992_REG_NADC, data, 1);
	if (ret)
		goto dev_err;
	dev->resolution = init_param->resolution;

	if (init_param->shunt_resistor <= 0) {
		ret = -EINVAL;
		goto dev_err;
	}
	dev->shunt_resistor = init_param->shunt_resistor;

	*device = dev;
	return 0;

dev_err:
	no_os_gpio_remove(dev->alert_gpio_desc);
	no_os_i2c_remove(dev->i2c_desc);
i2c_err:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free resources allocated to the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_remove(struct ltc2992_dev *dev)
{
	if (!dev)
		return -ENODEV;

	no_os_gpio_remove(dev->alert_gpio_desc);
	no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);

	return 0;
}

/**
 * @brief Read raw data from device register.
 * @param dev - The device structure.
 * @param addr - The register address.
 * @param data - Register data pointer.
 * @param num_bytes - Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_read_reg(struct ltc2992_dev *dev, uint8_t addr,
		     uint32_t *data, uint8_t num_bytes)
{
	int ret;
	uint8_t rx_buf[3] = {0};

	num_bytes = no_os_clamp_t(uint8_t, num_bytes, 0, 3);
	ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, rx_buf, num_bytes, 0);
	if (ret)
		return ret;

	switch (num_bytes) {
	case 1:
		*data = rx_buf[0];
		break;
	case 2:
		*data = no_os_get_unaligned_be16(rx_buf);
		break;
	case 3:
		*data = no_os_get_unaligned_be24(rx_buf);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Write raw data to device register.
 * @param dev - The device structure.
 * @param addr - The register address.
 * @param value - Value to write.
 * @param num_bytes - Number of bytes to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_write_reg(struct ltc2992_dev *dev, uint8_t addr,
		      uint32_t value, uint8_t num_bytes)
{
	uint8_t tx_buf[4] = {0};

	num_bytes = no_os_clamp_t(uint8_t, num_bytes, 0, 3);
	tx_buf[0] = addr;

	switch (num_bytes) {
	case 1:
		tx_buf[1] = (uint8_t)value;
		break;
	case 2:
		no_os_put_unaligned_be16((uint16_t)value, &tx_buf[1]);
		break;
	case 3:
		no_os_put_unaligned_be24(value, &tx_buf[1]);
		break;
	default:
		return -EINVAL;
	}

	return no_os_i2c_write(dev->i2c_desc, tx_buf, num_bytes + 1, 1);
}

/**
 * @brief Update register value in the device.
 * @param dev - The device structure.
 * @param addr - The register address.
 * @param mask - Mask of the bitfield to be updated.
 * @param val - Value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_update_reg(struct ltc2992_dev *dev, uint8_t addr,
		       uint8_t mask, uint8_t val)
{
	int ret;
	uint32_t rx_data;

	ret = ltc2992_read_reg(dev, addr, &rx_data, 1);
	if (ret)
		return ret;

	rx_data &= ~mask;
	rx_data |= no_os_field_prep(mask, val);

	return ltc2992_write_reg(dev, addr, rx_data, 1);
}

/**
 * @brief Read SENSE-type registers and convert to millivolts value.
 * @param dev - The device structure.
 * @param reg - SENSE register.
 * @param data - Data read.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_read_sense_reg(struct ltc2992_dev *dev,
				  uint8_t reg,
				  uint32_t *data)
{
	int ret;

	ret = ltc2992_read_reg(dev, reg, data, 2);
	if (ret)
		return ret;

	*data >>= sshift[dev->resolution];
	*data *= sense_mv_lsb[dev->resolution];

	return 0;
}

/**
 * @brief Write to SENSE-type registers from millivolts values.
 * @param dev - The device structure.
 * @param reg - SENSE register.
 * @param data - Data to write in millivolts scale.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_write_sense_reg(struct ltc2992_dev *dev,
				   uint8_t reg,
				   uint32_t data)
{
	data /= sense_mv_lsb[dev->resolution];
	data <<= sshift[dev->resolution];

	if (dev->resolution == LTC2992_RESOLUTION_12_BIT) {
		data = no_os_clamp(data, 0x0, LTC2992_12B_SENSE_VAL_MSK);
		data &= LTC2992_12B_SENSE_VAL_MSK;
	} else {
		data = no_os_clamp(data, 0x0, LTC2992_8B_SENSE_VAL_MSK);
		data &= LTC2992_8B_SENSE_VAL_MSK;
	}

	return ltc2992_write_reg(dev, reg, data, 2);
}

/**
 * @brief Read DSENSE-type registers and convert to microvolts value.
 * @param dev - The device structure.
 * @param reg - DSENSE register.
 * @param data - Data read.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_read_dsense_reg(struct ltc2992_dev *dev,
				   uint8_t reg,
				   uint32_t *data)
{
	int ret;

	ret = ltc2992_read_reg(dev, reg, data, 2);
	if (ret)
		return ret;

	if (reg == LTC2992_REG_ISUM)
		*data <<= 1;

	*data >>= sshift[dev->resolution];
	*data *= dsense_nv_lsb[dev->resolution];
	*data = NO_OS_DIV_ROUND_CLOSEST(*data, MILLIAMPER_PER_AMPER);

	return 0;
}

/**
 * @brief Write to DSENSE-type registers from microvolts values.
 * @param dev - The device structure.
 * @param reg - DSENSE register.
 * @param data - Data to write in microvolts scale.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_write_dsense_reg(struct ltc2992_dev *dev,
				    uint8_t reg,
				    uint32_t data)
{
	data *= MILLIAMPER_PER_AMPER;
	data /= dsense_nv_lsb[dev->resolution];
	data <<= sshift[dev->resolution];

	if (reg == LTC2992_REG_ISUM)
		data >>= 1;

	if (dev->resolution == LTC2992_RESOLUTION_12_BIT) {
		data = no_os_clamp(data, 0x0, LTC2992_12B_SENSE_VAL_MSK);
		data &= LTC2992_12B_SENSE_VAL_MSK;
	} else {
		data = no_os_clamp(data, 0x0, LTC2992_8B_SENSE_VAL_MSK);
		data &= LTC2992_8B_SENSE_VAL_MSK;
	}

	return ltc2992_write_reg(dev, reg, data, 2);
}

/**
 * @brief Read POWER-type registers and convert to milliwatts value.
 * @param dev - The device structure.
 * @param reg - POWER register.
 * @param data - Data read.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_read_power_reg(struct ltc2992_dev *dev,
				  uint8_t reg,
				  uint32_t *data)
{
	int ret;
	uint64_t power;

	ret = ltc2992_read_reg(dev, reg, data, 3);
	if (ret)
		return ret;

	*data >>= pshift[dev->resolution];

	if (reg == LTC2992_REG_PSUM)
		*data <<= 1;

	power = (uint64_t)(*data);
	power *= no_os_mul_u32_u32(sense_mv_lsb[dev->resolution],
				   dsense_nv_lsb[dev->resolution]);
	power = no_os_div_u64(power, dev->shunt_resistor);
	power = no_os_div_u64(power, MICRO);

	*data = (uint32_t)power;

	return 0;
}

/**
 * @brief Write to POWER-type registers from milliwatts values.
 * @param dev - The device structure.
 * @param reg - POWER register.
 * @param data - Data to write in milliwatts scale.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_write_power_reg(struct ltc2992_dev *dev,
				   uint8_t reg,
				   uint32_t data)
{
	uint64_t power = (uint64_t)data;

	power *= no_os_mul_u32_u32(dev->shunt_resistor, MICRO);
	power /= no_os_mul_u32_u32(sense_mv_lsb[dev->resolution],
				   dsense_nv_lsb[dev->resolution]);

	data = (uint32_t)power;
	data <<= pshift[dev->resolution];

	if (reg == LTC2992_REG_PSUM)
		data >>= 1;

	if (dev->resolution == LTC2992_RESOLUTION_12_BIT) {
		data = no_os_clamp(data, 0x0, LTC2992_12B_POWER_VAL_MSK);
		data &= LTC2992_12B_POWER_VAL_MSK;
	} else {
		data = no_os_clamp(data, 0x0, LTC2992_8B_POWER_VAL_MSK);
		data &= LTC2992_8B_POWER_VAL_MSK;
	}

	return ltc2992_write_reg(dev, reg, data, 3);
}

/**
 * @brief Read bus voltage in a sense channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1 - Read in SENSE1 channel.
 * 			   LTC2992_SENSE2 - Read in SENSE2 channel.
 * @param voltage_data - Voltage read in millivolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_voltage(struct ltc2992_dev *dev,
			enum ltc2992_sense sense,
			uint32_t *voltage_data)
{
	return ltc2992_read_sense_reg(dev, LTC2992_REG_SENSE(sense),
				      voltage_data);
}

/**
 * @brief Read shunt voltage in a sense channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1 - Read in SENSE1 channel.
 * 			   LTC2992_SENSE2 - Read in SENSE2 channel.
 * @param vshunt_data - Voltage read in microvolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_vshunt(struct ltc2992_dev *dev,
		       enum ltc2992_sense sense,
		       uint32_t *vshunt_data)
{
	return ltc2992_read_dsense_reg(dev, LTC2992_REG_DSENSE(sense),
				       vshunt_data);
}

/**
 * @brief Read bus current along a sense channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1 - Read in SENSE1 channel.
 * 			   LTC2992_SENSE2 - Read in SENSE2 channel.
 * @param current_data - Current read in milliamperes.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_current(struct ltc2992_dev *dev,
			enum ltc2992_sense sense,
			uint32_t *current_data)
{
	int ret;

	ret = ltc2992_get_vshunt(dev, sense, current_data);
	if (ret)
		return ret;

	*current_data = NO_OS_DIV_ROUND_CLOSEST(*current_data,
						dev->shunt_resistor);

	return 0;
}

/**
 * @brief Read bus power output in a sense channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1 - Read in SENSE1 channel.
 * 			   LTC2992_SENSE2 - Read in SENSE2 channel.
 * @param power_data - Power read in milliwatts.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_power(struct ltc2992_dev *dev,
		      enum ltc2992_sense sense,
		      uint32_t *power_data)
{
	return ltc2992_read_power_reg(dev, LTC2992_REG_POWER(sense),
				      power_data);
}

/**
 * @brief Read the shunt voltage sum or power sum of both sense channels.
 * @param dev - The device structure.
 * @param chan - Measured channel.
 * 		 Example: LTC2992_VSHUNT - Read ISUM.
 * 			  LTC2992_POWER - Read PSUM.
 * @param sum_data - Sum read in microvolts or milliwatts.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_sum(struct ltc2992_dev *dev,
		    enum ltc2992_channel chan,
		    uint32_t *sum_data)
{
	int ret;

	switch (chan) {
	case LTC2992_VSHUNT:
	case LTC2992_ISUM:
	case LTC2992_CURRENT:
		ret = ltc2992_read_dsense_reg(dev, LTC2992_REG_ISUM, sum_data);
		if (ret)
			return ret;

		if (chan == LTC2992_CURRENT)
			*sum_data = NO_OS_DIV_ROUND_CLOSEST(*sum_data,
							    dev->shunt_resistor);
		break;
	case LTC2992_PSUM:
	case LTC2992_POWER:
		return ltc2992_read_power_reg(dev, LTC2992_REG_PSUM, sum_data);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Read sense data at a SENSE channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1 - Read in SENSE1 channel.
 * 			   LTC2992_SENSE2 - Read in SENSE2 channel.
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE - Read bus voltage in millivolts.
 * 			  LTC2992_VSHUNT - Read shunt voltage in microvolts.
 * 			  LTC2992_CURRENT - Read current in milliamperes.
 * 			  LTC2992_POWER - Read power in milliwatts.
 * 			  LTC2992_ISUM - Read ISUM in microvolts.
 * 			  LTC2992_PSUM - Read PSUM in milliwatts.
 * @param data - Read data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_sense_data(struct ltc2992_dev *dev,
			   enum ltc2992_sense sense,
			   enum ltc2992_channel chan,
			   uint32_t *data)
{
	switch (chan) {
	case LTC2992_VOLTAGE:
		return ltc2992_get_voltage(dev, sense, data);
	case LTC2992_VSHUNT:
		return ltc2992_get_vshunt(dev, sense, data);
	case LTC2992_CURRENT:
		return ltc2992_get_current(dev, sense, data);
	case LTC2992_POWER:
		return ltc2992_get_power(dev, sense, data);
	case LTC2992_ISUM:
	case LTC2992_PSUM:
		return ltc2992_get_sum(dev, chan, data);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read maximum sense data at a SENSE channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1 - Read in SENSE1 channel.
 * 			   LTC2992_SENSE2 - Read in SENSE2 channel.
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE - Read bus voltage in millivolts.
 * 			  LTC2992_VSHUNT - Read shunt voltage in microvolts.
 * 			  LTC2992_CURRENT - Read current in milliamperes.
 * 			  LTC2992_POWER - Read power in milliwatts.
 * 			  LTC2992_ISUM - Read ISUM in microvolts.
 * 			  LTC2992_PSUM - Read PSUM in milliwatts.
 * @param data - Read max data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_max_data(struct ltc2992_dev *dev,
			 enum ltc2992_sense sense,
			 enum ltc2992_channel chan,
			 uint32_t *data)
{
	int ret;

	switch (chan) {
	case LTC2992_VOLTAGE:
		return ltc2992_read_sense_reg(dev,
					      LTC2992_REG_SENSE_MAX(sense),
					      data);
	case LTC2992_VSHUNT:
		return ltc2992_read_dsense_reg(dev,
					       LTC2992_REG_DSENSE_MAX(sense),
					       data);
	case LTC2992_CURRENT:
		ret = ltc2992_read_dsense_reg(dev,
					      LTC2992_REG_DSENSE_MAX(sense),
					      data);
		if (ret)
			return ret;

		*data = NO_OS_DIV_ROUND_CLOSEST(*data, dev->shunt_resistor);
		return 0;
	case LTC2992_POWER:
		return ltc2992_read_power_reg(dev,
					      LTC2992_REG_POWER_MAX(sense),
					      data);
	case LTC2992_ISUM:
		return ltc2992_read_dsense_reg(dev,
					       LTC2992_REG_ISUM_MAX,
					       data);
	case LTC2992_PSUM:
		return ltc2992_read_power_reg(dev,
					      LTC2992_REG_PSUM_MAX,
					      data);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Read minimum sense data at a SENSE channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1 - Read in SENSE1 channel.
 * 			   LTC2992_SENSE2 - Read in SENSE2 channel.
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE - Read bus voltage in millivolts.
 * 			  LTC2992_VSHUNT - Read shunt voltage in microvolts.
 * 			  LTC2992_CURRENT - Read current in milliamperes.
 * 			  LTC2992_POWER - Read power in milliwatts.
 * 			  LTC2992_ISUM - Read ISUM in microvolts.
 * 			  LTC2992_PSUM - Read PSUM in milliwatts.
 * @param data - Read min data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_min_data(struct ltc2992_dev *dev,
			 enum ltc2992_sense sense,
			 enum ltc2992_channel chan,
			 uint32_t *data)
{
	int ret;

	switch (chan) {
	case LTC2992_VOLTAGE:
		return ltc2992_read_sense_reg(dev,
					      LTC2992_REG_SENSE_MIN(sense),
					      data);
	case LTC2992_VSHUNT:
		return ltc2992_read_dsense_reg(dev,
					       LTC2992_REG_DSENSE_MIN(sense),
					       data);
	case LTC2992_CURRENT:
		ret = ltc2992_read_dsense_reg(dev,
					      LTC2992_REG_DSENSE_MIN(sense),
					      data);
		if (ret)
			return ret;

		*data = NO_OS_DIV_ROUND_CLOSEST(*data, dev->shunt_resistor);
		break;
	case LTC2992_POWER:
		return ltc2992_read_power_reg(dev,
					      LTC2992_REG_POWER_MIN(sense),
					      data);
	case LTC2992_ISUM:
		return ltc2992_read_dsense_reg(dev,
					       LTC2992_REG_ISUM_MIN,
					       data);
	case LTC2992_PSUM:
		return ltc2992_read_power_reg(dev,
					      LTC2992_REG_PSUM_MIN,
					      data);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set upper thresh level for alert at a SENSE channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1
 * 			   LTC2992_SENSE2
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE - Bus voltage thresh in millivolts.
 * 			  LTC2992_VSHUNT - Shunt voltage thresh in microvolts.
 * 			  LTC2992_CURRENT - Current thresh in milliamperes.
 * 			  LTC2992_POWER - Power thresh in milliwatts.
 * 			  LTC2992_ISUM - ISUM thresh in microvolts.
 * 			  LTC2992_PSUM - PSUM thresh in milliwatts.
 * @param thresh_value - Thresh level to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_set_max_thresh(struct ltc2992_dev *dev,
			   enum ltc2992_sense sense,
			   enum ltc2992_channel chan,
			   uint32_t thresh_value)
{
	switch (chan) {
	case LTC2992_VOLTAGE:
		return ltc2992_write_sense_reg(dev,
					       LTC2992_REG_SENSE_MAX_THRESH(sense),
					       thresh_value);
	case LTC2992_VSHUNT:
		return ltc2992_write_dsense_reg(dev,
						LTC2992_REG_DSENSE_MAX_THRESH(sense),
						thresh_value);
	case LTC2992_CURRENT:
		thresh_value *= dev->shunt_resistor;
		return ltc2992_write_dsense_reg(dev,
						LTC2992_REG_DSENSE_MAX_THRESH(sense),
						thresh_value);
	case LTC2992_POWER:
		return ltc2992_write_power_reg(dev,
					       LTC2992_REG_POWER_MAX_THRESH(sense),
					       thresh_value);
	case LTC2992_ISUM:
		return ltc2992_write_dsense_reg(dev,
						LTC2992_REG_ISUM_MAX_THRESH,
						thresh_value);
	case LTC2992_PSUM:
		return ltc2992_write_power_reg(dev,
					       LTC2992_REG_PSUM_MAX_THRESH,
					       thresh_value);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Set lower thresh level for alert at a SENSE channel.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1
 * 			   LTC2992_SENSE2
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE - Bus voltage thresh in millivolts.
 * 			  LTC2992_VSHUNT - Shunt voltage thresh in microvolts.
 * 			  LTC2992_CURRENT - Current thresh in milliamperes.
 * 			  LTC2992_POWER - Power thresh in milliwatts.
 * 			  LTC2992_ISUM - ISUM thresh in microvolts.
 * 			  LTC2992_PSUM - PSUM thresh in milliwatts.
 * @param thresh_value - Thresh level to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_set_min_thresh(struct ltc2992_dev *dev,
			   enum ltc2992_sense sense,
			   enum ltc2992_channel chan,
			   uint32_t thresh_value)
{
	switch (chan) {
	case LTC2992_VOLTAGE:
		return ltc2992_write_sense_reg(dev,
					       LTC2992_REG_SENSE_MIN_THRESH(sense),
					       thresh_value);
	case LTC2992_VSHUNT:
		return ltc2992_write_dsense_reg(dev,
						LTC2992_REG_DSENSE_MIN_THRESH(sense),
						thresh_value);
	case LTC2992_CURRENT:
		thresh_value *= dev->shunt_resistor;
		return ltc2992_write_dsense_reg(dev,
						LTC2992_REG_DSENSE_MIN_THRESH(sense),
						thresh_value);
	case LTC2992_POWER:
		return ltc2992_write_power_reg(dev,
					       LTC2992_REG_POWER_MIN_THRESH(sense),
					       thresh_value);
	case LTC2992_ISUM:
		return ltc2992_write_dsense_reg(dev,
						LTC2992_REG_ISUM_MIN_THRESH,
						thresh_value);
	case LTC2992_PSUM:
		return ltc2992_write_power_reg(dev,
					       LTC2992_REG_PSUM_MIN_THRESH,
					       thresh_value);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Enable alert.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1
 * 			   LTC2992_SENSE2
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE
 * 			  LTC2992_VSHUNT
 * 			  LTC2992_CURRENT
 * 			  LTC2992_POWER
 * 			  LTC2992_ISUM
 * 			  LTC2992_PSUM
 * @param alert - Alert value to set.
 * 		  Example: LTC2992_ALERT_DISABLE_ALL - Disable alert
 * 		  	   LTC2992_ALERT_ENABLE_UV - Enable undervalue
 * 		  				     alert
 * 		  	   LTC2992_ALERT_ENABLE_OV - Enable overvalue
 * 		  				     alert
 * 		  	   LTC2992_ALERT_ENABLE_ALL - Enable undervalue
 * 		  				      and overvalue alert
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_set_alert(struct ltc2992_dev *dev,
		      enum ltc2992_sense sense,
		      enum ltc2992_channel chan,
		      enum ltc2992_alert_value alert)
{
	switch (chan) {
	case LTC2992_VOLTAGE:
		switch (sense) {
		case LTC2992_SENSE1:
			return ltc2992_update_reg(dev,
						  LTC2992_REG_ALERT1,
						  LTC2992_SENSE_ALERT_MSK,
						  (uint8_t)alert);
		case LTC2992_SENSE2:
			return ltc2992_update_reg(dev,
						  LTC2992_REG_ALERT2,
						  LTC2992_SENSE_ALERT_MSK,
						  (uint8_t)alert);
		default:
			return -EINVAL;
		}
	case LTC2992_VSHUNT:
	case LTC2992_CURRENT:
		switch (sense) {
		case LTC2992_SENSE1:
			return ltc2992_update_reg(dev,
						  LTC2992_REG_ALERT1,
						  LTC2992_DSENSE_ALERT_MSK,
						  (uint8_t)alert);
		case LTC2992_SENSE2:
			return ltc2992_update_reg(dev,
						  LTC2992_REG_ALERT2,
						  LTC2992_DSENSE_ALERT_MSK,
						  (uint8_t)alert);
		default:
			return -EINVAL;
		}
	case LTC2992_POWER:
		switch (sense) {
		case LTC2992_SENSE1:
			return ltc2992_update_reg(dev,
						  LTC2992_REG_ALERT1,
						  LTC2992_POWER_ALERT_MSK,
						  (uint8_t)alert);
		case LTC2992_SENSE2:
			return ltc2992_update_reg(dev,
						  LTC2992_REG_ALERT2,
						  LTC2992_POWER_ALERT_MSK,
						  (uint8_t)alert);
		default:
			return -EINVAL;
		}
	case LTC2992_ISUM:
		return ltc2992_update_reg(dev,
					  LTC2992_REG_ALERT3,
					  LTC2992_ISUM_ALERT_MSK,
					  (uint8_t)alert);
	case LTC2992_PSUM:
		return ltc2992_update_reg(dev,
					  LTC2992_REG_ALERT3,
					  LTC2992_PSUM_ALERT_MSK,
					  (uint8_t)alert);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Get fault status.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1
 * 			   LTC2992_SENSE2
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE
 * 			  LTC2992_VSHUNT
 * 			  LTC2992_CURRENT
 * 			  LTC2992_POWER
 * 			  LTC2992_ISUM
 * 			  LTC2992_PSUM
 * @param fault - Alert value to set.
 * 		  Example: LTC2992_FAULT_NONE - No fault event
 * 			   LTC2992_FAULT_UNDERVALUE - Undervalue fault occurred
 * 			   LTC2992_FAULT_OVERVALUE - Overvalue fault occurred
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_fault(struct ltc2992_dev *dev,
		      enum ltc2992_sense sense,
		      enum ltc2992_channel chan,
		      enum ltc2992_fault_status *fault)
{
	int ret;
	uint32_t fault_data;

	switch (chan) {
	case LTC2992_VOLTAGE:
		ret = ltc2992_read_reg(dev, LTC2992_REG_SENSE_FAULT(sense),
				       &fault_data, 1);
		if (ret)
			return ret;

		*fault = (enum ltc2992_fault_status)
			 no_os_field_get(LTC2992_SENSE_FAULT_MSK, fault_data);
		break;
	case LTC2992_VSHUNT:
	case LTC2992_CURRENT:
		ret = ltc2992_read_reg(dev, LTC2992_REG_DSENSE_FAULT(sense),
				       &fault_data, 1);
		if (ret)
			return ret;

		*fault = (enum ltc2992_fault_status)
			 no_os_field_get(LTC2992_DSENSE_FAULT_MSK, fault_data);
		break;
	case LTC2992_POWER:
		ret = ltc2992_read_reg(dev, LTC2992_REG_POWER_FAULT(sense),
				       &fault_data, 1);
		if (ret)
			return ret;

		*fault = (enum ltc2992_fault_status)
			 no_os_field_get(LTC2992_POWER_FAULT_MSK, fault_data);
		break;
	case LTC2992_ISUM:
		ret = ltc2992_read_reg(dev, LTC2992_REG_FAULT3, &fault_data, 1);
		if (ret)
			return ret;

		*fault = (enum ltc2992_fault_status)
			 no_os_field_get(LTC2992_ISUM_FAULT_MSK, fault_data);
		break;
	case LTC2992_PSUM:
		ret = ltc2992_read_reg(dev, LTC2992_REG_FAULT3, &fault_data, 1);
		if (ret)
			return ret;

		*fault = (enum ltc2992_fault_status)
			 no_os_field_get(LTC2992_PSUM_FAULT_MSK, fault_data);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Clear fault status.
 * @param dev - The device structure.
 * @param sense - Sense channel.
 * 		  Example: LTC2992_SENSE1
 * 			   LTC2992_SENSE2
 * @param chan - Measured data channel.
 * 		 Example: LTC2992_VOLTAGE
 * 			  LTC2992_VSHUNT
 * 			  LTC2992_CURRENT
 * 			  LTC2992_POWER
 * 			  LTC2992_ISUM
 * 			  LTC2992_PSUM
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_clear_fault(struct ltc2992_dev *dev,
			enum ltc2992_sense sense,
			enum ltc2992_channel chan)
{
	switch (chan) {
	case LTC2992_VOLTAGE:
		return ltc2992_update_reg(dev, LTC2992_REG_SENSE_FAULT(sense),
					  LTC2992_SENSE_FAULT_MSK, 0);
	case LTC2992_VSHUNT:
	case LTC2992_CURRENT:
		return ltc2992_update_reg(dev, LTC2992_REG_DSENSE_FAULT(sense),
					  LTC2992_DSENSE_FAULT_MSK, 0);
	case LTC2992_POWER:
		return ltc2992_update_reg(dev, LTC2992_REG_POWER_FAULT(sense),
					  LTC2992_POWER_FAULT_MSK, 0);
	case LTC2992_ISUM:
		return ltc2992_update_reg(dev, LTC2992_REG_FAULT3,
					  LTC2992_ISUM_FAULT_MSK, 0);
	case LTC2992_PSUM:
		return ltc2992_update_reg(dev, LTC2992_REG_FAULT3,
					  LTC2992_PSUM_FAULT_MSK, 0);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read GPIO register data in millivolts.
 * @param dev - The device structure.
 * @param reg - Register address to read.
 * @param data - Data read
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_read_gpio_reg(struct ltc2992_dev *dev, uint8_t reg,
				 uint32_t *data)
{
	int ret;

	ret = ltc2992_read_reg(dev, reg, data, 2);
	if (ret)
		return ret;

	*data >>= sshift[dev->resolution];
	*data *= gpio_uv_lsb[dev->resolution];
	*data = NO_OS_DIV_ROUND_CLOSEST(*data, MILLI);

	return 0;
}

/**
 * @brief Write data in millivolts to a GPIO register.
 * @param dev - The device structure.
 * @param reg - Register address to read.
 * @param data - Data read
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2992_write_gpio_reg(struct ltc2992_dev *dev, uint8_t reg,
				  uint32_t data)
{
	data *= MILLI;
	data /= gpio_uv_lsb[dev->resolution];
	data <<= sshift[dev->resolution];
	if (dev->resolution == LTC2992_RESOLUTION_12_BIT)
		data &= LTC2992_12B_SENSE_VAL_MSK;
	else
		data &= LTC2992_8B_SENSE_VAL_MSK;

	return ltc2992_write_reg(dev, reg, data, 2);
}

/**
 * @brief Get data from a GPIO data converter.
 * @param dev - The device structure.
 * @param gpio - GPIO to get.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param gpio_data - Data read
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_gpio_data(struct ltc2992_dev *dev,
			  enum ltc2992_gpio gpio,
			  uint32_t *gpio_data)
{
	return ltc2992_read_gpio_reg(dev, gpio_regmap[gpio].data, gpio_data);
}

/**
 * @brief Get maximum data from a GPIO data converter.
 * @param dev - The device structure.
 * @param gpio - GPIO to read.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param gpio_data - Data read
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_gpio_max_data(struct ltc2992_dev *dev,
			      enum ltc2992_gpio gpio,
			      uint32_t *gpio_data)
{
	return ltc2992_read_gpio_reg(dev, gpio_regmap[gpio].max, gpio_data);
}

/**
 * @brief Get minimum data from a GPIO data converter.
 * @param dev - The device structure.
 * @param gpio - GPIO to get.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param gpio_data - Data read
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_gpio_min_data(struct ltc2992_dev *dev,
			      enum ltc2992_gpio gpio,
			      uint32_t *gpio_data)
{
	return ltc2992_read_gpio_reg(dev, gpio_regmap[gpio].min, gpio_data);
}

/**
 * @brief Set upper threshold level in a GPIO.
 * @param dev - The device structure.
 * @param gpio - GPIO to set.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param thresh_value - Set threshold.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_set_gpio_max_thresh(struct ltc2992_dev *dev,
				enum ltc2992_gpio gpio,
				uint32_t thresh_value)
{
	return ltc2992_write_gpio_reg(dev, gpio_regmap[gpio].max_thresh,
				      thresh_value);
}

/**
 * @brief Set lower threshold level in a GPIO.
 * @param dev - The device structure.
 * @param gpio - GPIO to set.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param thresh_value - Set threshold.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_set_gpio_min_thresh(struct ltc2992_dev *dev,
				enum ltc2992_gpio gpio,
				uint32_t thresh_value)
{
	return ltc2992_write_gpio_reg(dev, gpio_regmap[gpio].min_thresh,
				      thresh_value);
}

/**
 * @brief Enable alert when GPIO level reaches beyond threshold limit.
 * @param dev - The device structure.
 * @param gpio - GPIO to enable alert.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param alert_value - Alert value to set.
 * 			Example: LTC2992_ALERT_DISABLE_ALL - Disable alert
 * 				 LTC2992_ALERT_ENABLE_UV - Enable undervalue
 * 							   alert
 * 				 LTC2992_ALERT_ENABLE_OV - Enable overvalue
 * 							   alert
 * 				 LTC2992_ALERT_ENABLE_ALL - Enable undervalue
 * 							    and overvalue alert
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_set_gpio_alert(struct ltc2992_dev *dev,
			   enum ltc2992_gpio gpio,
			   enum ltc2992_alert_value alert_value)
{
	return ltc2992_update_reg(dev, gpio_regmap[gpio].alert,
				  gpio_regmap[gpio].fault_alert_mask,
				  (uint8_t)alert_value);
}

/**
 * @brief Get GPIO fault status.
 * @param dev - The device structure.
 * @param gpio - GPIO to read fault status.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param fault - Alert value to set.
 * 		  Example: LTC2992_FAULT_NONE - No fault event
 * 			   LTC2992_FAULT_UNDERVALUE - Undervalue fault occurred
 * 			   LTC2992_FAULT_OVERVALUE - Overvalue fault occurred
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_gpio_fault(struct ltc2992_dev *dev,
			   enum ltc2992_gpio gpio,
			   enum ltc2992_fault_status *fault)
{
	int ret;
	uint32_t fault_data;

	ret = ltc2992_read_reg(dev, gpio_regmap[gpio].fault, &fault_data, 1);
	if (ret)
		return ret;

	*fault = (enum ltc2992_fault_status)
		 no_os_field_get(gpio_regmap[gpio].fault_alert_mask,
				 fault_data);

	return 0;
}

/**
 * @brief Clear GPIO fault status.
 * @param dev - The device structure.
 * @param gpio - GPIO to clear fault.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_clear_gpio_fault(struct ltc2992_dev *dev,
			     enum ltc2992_gpio gpio)
{
	return ltc2992_update_reg(dev, gpio_regmap[gpio].fault,
				  gpio_regmap[gpio].fault_alert_mask, 0);
}

/**
 * @brief Reset alert pin (GPI04) status.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_reset_alert_pin(struct ltc2992_dev *dev)
{

	return ltc2992_update_reg(dev, LTC2992_REG_GPIO4_CTRL,
				  LTC2992_G4_ALERT_PIN_BIT, 0);
}

/**
 * @brief Get GPIO state.
 * @param dev - The device structure.
 * @param gpio - GPIO to read state.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param state - GPIO state
 * 		  Example: 0 - GPIO is logic low
 * 			   1 - GPIO is logic high
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_get_gpio_state(struct ltc2992_dev *dev,
			   enum ltc2992_gpio gpio,
			   uint8_t *state)
{
	int ret;
	uint32_t gpio_status;

	ret = ltc2992_read_reg(dev, LTC2992_REG_GPIO_STATUS, &gpio_status, 1);
	if (ret)
		return ret;

	*state = (uint8_t)no_os_field_get(NO_OS_BIT(3 - gpio), gpio_status);

	return 0;
}

/**
 * @brief Set GPIO output.
 * @param dev - The device structure.
 * @param gpio - GPIO to set.
 * 		 Example: LTC2992_GPIO1
 * 			  LTC2992_GPIO2
 * 			  LTC2992_GPIO3
 * 			  LTC2992_GPIO4
 * @param output - GPIO output to set
 * 		  Example: LTC2992_GPIO_OUTPUT_HI_Z - Output high impedance.
 * 			   LTC2992_GPIO_OUTPUT_PULL_LOW - Pull output low.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2992_set_gpio_output(struct ltc2992_dev *dev,
			    enum ltc2992_gpio gpio,
			    enum ltc2992_gpio_output output)
{
	return ltc2992_update_reg(dev, gpio_regmap[gpio].ctrl,
				  gpio_regmap[gpio].ctrl_mask, (uint8_t)output);
}

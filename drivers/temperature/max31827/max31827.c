/***************************************************************************//**
 *   @file   max31827.c
 *   @brief  Implementation of MAX31827 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
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

#include "max31827.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_i2c.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/**
 * @brief MAX31827 conversion period in ms
 */
const uint16_t max31827_conversions[] = {
	[MAX31827_CNV_1_DIV_64_HZ] = 64000,
	[MAX31827_CNV_1_DIV_32_HZ] = 32000,
	[MAX31827_CNV_1_DIV_16_HZ] = 16000,
	[MAX31827_CNV_1_DIV_4_HZ] = 4000,
	[MAX31827_CNV_1_HZ] = 1000,
	[MAX31827_CNV_4_HZ] = 250,
	[MAX31827_CNV_8_HZ] = 125,
};

/**
 * @brief MAX31827 temperature resolution
 */
const uint16_t max31827_resolutions[] = {
	[MAX31827_RES_8_BIT] = 1000,
	[MAX31827_RES_9_BIT] = 500,
	[MAX31827_RES_10_BIT] = 250,
	[MAX31827_RES_12_BIT] = 62,
};

/**
 * @brief MAX31827 temperature conversion times in us
 */
const int max31827_conv_times[] = {
	[MAX31827_RES_8_BIT] = 8750,
	[MAX31827_RES_9_BIT] = 17500,
	[MAX31827_RES_10_BIT] = 35000,
	[MAX31827_RES_12_BIT] = 140000,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Read a register value
 * @param dev - MAX31827 descriptor
 * @param addr - register address
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int max31827_reg_read(struct max31827_device *dev, uint8_t addr, uint16_t *val)
{
	int ret;
	uint8_t uval[2];

	ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, uval, 2, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be16(uval);

	return 0;
}

/**
 * @brief Write a register value
 * @param dev - MAX31827 descriptor
 * @param addr - register address
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int max31827_reg_write(struct max31827_device *dev, uint8_t addr, uint16_t val)
{
	uint8_t buff[3] = {addr, val >> 8, val & 0xFF};

	return no_os_i2c_write(dev->i2c_desc, buff, NO_OS_ARRAY_SIZE(buff), 1);
}

/**
 * @brief Read-modify-write operation
 * @param dev - MAX31827 descriptor
 * @param addr - register address
 * @param mask - Mask for specific register bits to be updated
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int max31827_reg_update_bits(struct max31827_device *dev, uint8_t addr,
			     uint16_t mask, uint16_t val)
{
	int ret;
	uint16_t regval;

	ret = max31827_reg_read(dev, addr, &regval);
	if (ret)
		return ret;

	regval &= ~mask;
	regval |= no_os_field_prep(mask, val);

	return max31827_reg_write(dev, addr, regval);
}

/**
 * @brief Device and comm init function
 * @param dev - MAX31827 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max31827_init(struct max31827_device **dev,
		  struct max31827_init_param *init_param)
{
	struct max31827_device *temp_dev;
	int ret;

	temp_dev = no_os_calloc(1, sizeof(*temp_dev));
	if (!temp_dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&temp_dev->i2c_desc, &init_param->i2c_init_param);
	if (ret)
		goto free_dev;

	temp_dev->enable = true;
	temp_dev->resolution = MAX31827_RES_12_BIT;
	temp_dev->update_interval = max31827_conversions[MAX31827_CNV_1_HZ];

	ret = max31827_init_client(temp_dev, init_param);
	if (ret)
		goto remove_i2c;

	*dev = temp_dev;

	return 0;

remove_i2c:
	ret = no_os_i2c_remove(temp_dev->i2c_desc);
	if (ret)
		pr_err("Failed to remove I2C descriptor\r\n");

free_dev:
	no_os_free(temp_dev);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param dev - MAX31827 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max31827_remove(struct max31827_device *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Initialize MAX31827 device setup
 * @param dev - MAX31827 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max31827_init_client(struct max31827_device *dev,
			 struct max31827_init_param *init_param)
{
	unsigned int res = 0;
	uint32_t lsb_idx;

	res |= MAX31827_DEVICE_ENABLE(1);

	res |= MAX31827_CONF_RESO_MASK;

	res |= no_os_field_prep(MAX31827_CONF_COMP_INT_MASK,
				init_param->comp_int);

	res |= no_os_field_prep(MAX31827_CONF_TIMEOUT_MASK,
				!init_param->timeout_enable);

	res |= no_os_field_prep(MAX31827_CONF_ALRM_POL_MASK,
				init_param->alarm_pol);

	if (init_param->fault_q)
		lsb_idx = no_os_find_first_set_bit(init_param->fault_q);

	if (no_os_hweight32(init_param->fault_q) != 1 || lsb_idx > 3) {
		pr_err("Fault queue should be one of these: 1, 2, 4, 8\r\n");
		return -EINVAL;
	}

	res |= no_os_field_prep(MAX31827_CONF_FLT_Q_MASK, lsb_idx);

	return max31827_reg_write(dev, MAX31827_CONF_REG, res);
}

/**
 * @brief Read a temperature from a register
 * @param dev - MAX31827 descriptor
 * @param addr - register address
 * @param val - temperature value
 * @return 0 in case of success, negative error code otherwise
 */
int max31827_read_temp(struct max31827_device *dev, uint8_t addr, int32_t *val)
{
	int ret;
	uint16_t uval;
	int32_t data;

	ret = max31827_reg_read(dev, addr, &uval);
	if (ret)
		return ret;

	data = (no_os_sign_extend32(uval, 15) * MILLI);

	*val = data >> 4;

	return 0;
}

/**
 * @brief Read input temperature
 * @param dev - MAX31827 descriptor
 * @param val - temperature value
 * @return 0 in case of success, negative error code otherwise
 */
int max31827_read_temp_input(struct max31827_device *dev, int32_t *val)
{
	int ret;

	if (!dev->enable) {
		ret = max31827_reg_update_bits(dev, MAX31827_CONF_REG,
					       MAX31827_CONF_1SHOT_MASK, 1);
		if (ret)
			return ret;

		no_os_udelay(max31827_conv_times[dev->resolution]);
	}

	/**
	 * For 12-bit resolution the conversion time is 140 ms,
	 * thus an additional 15 ms is needed to complete the
	 * conversion: 125 ms + 15 ms = 140 ms
	 */
	if (dev->resolution == MAX31827_RES_12_BIT &&
	    dev->update_interval == max31827_conversions[MAX31827_CNV_8_HZ])
		no_os_udelay(15000);

	return max31827_read_temp(dev, MAX31827_T_REG, val);
}

/**
 * @brief Shutdown then write register value
 * @param dev - MAX31827 descriptor
 * @param reg - register address
 * @param mask - Mask for specific register bits to be updated
 * @param val - register value (requires prior bit shifting)
 * @return 0 in case of success, negative error code otherwise
 */
int max31827_shutdown_write(struct max31827_device *dev, uint8_t reg,
			    uint16_t mask, uint16_t val)
{
	uint16_t cfg;
	uint16_t cnv_rate;
	int ret;

	/**
	 * Before the Temperature Threshold Alarm, Alarm Hysteresis Threshold
	 * and Resolution bits from Configuration register are changed over I2C,
	 * the part must be in shutdown mode.
	 */
	if (!dev->enable) {
		if (!mask)
			return max31827_reg_write(dev, reg, val);
		else
			return max31827_reg_update_bits(dev, reg, mask, val);
	}

	ret = max31827_reg_read(dev, MAX31827_CONF_REG, &cfg);
	if (ret)
		return ret;

	cnv_rate = no_os_field_prep(MAX31827_CONF_CNV_RATE_MASK, cfg);
	cfg = cfg & ~(MAX31827_CONF_1SHOT_MASK | MAX31827_CONF_CNV_RATE_MASK);
	ret = max31827_reg_write(dev, MAX31827_CONF_REG, cfg);
	if (ret)
		return ret;

	if (!mask)
		ret = max31827_reg_write(dev, reg, val);
	else
		ret = max31827_reg_update_bits(dev, reg, mask, val);
	if (ret)
		return ret;

	return max31827_reg_update_bits(dev, MAX31827_CONF_REG,
					MAX31827_CONF_CNV_RATE_MASK, cnv_rate);
}

/**
 * @brief Write the alarm value to the specified register
 * @param dev - MAX31827 descriptor
 * @param reg - register address
 * @param val - temperature value
 * @return 0 in case of success, negative error code otherwise
 */
int max31827_write_alarm_val(struct max31827_device *dev, unsigned int reg,
			     int32_t val)
{
	int32_t data;
	bool sign = (val < 0);

	data = sign ? -val : val;

	data = ((data << 4) / MILLI) * (sign ? -1 : 1);

	return max31827_shutdown_write(dev, reg, 0, data);
}

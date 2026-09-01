/***************************************************************************//**
 *   @file   max31889.c
 *   @brief  Temperature sensor functionalities
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "max31889.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"

int32_t max31889_write_reg(struct max31889_desc *desc, uint8_t reg_addr,
			   uint8_t *data, uint8_t length)
{
	int32_t ret = 0;
	struct no_os_i2c_desc *i2c_desc;

	if (!desc || !desc->i2c_desc)
		return -EINVAL;

	i2c_desc = desc->i2c_desc;

	ret = no_os_i2c_write(i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;
	return no_os_i2c_write(i2c_desc, data, length, 1);
}

int32_t max31889_read_reg(struct max31889_desc *desc, uint8_t reg_addr,
			  uint8_t *data, uint8_t length)
{
	int32_t ret = 0;
	struct no_os_i2c_desc *i2c_desc;

	if (!desc || !desc->i2c_desc)
		return -EINVAL;

	i2c_desc = desc->i2c_desc;

	ret = no_os_i2c_write(i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(i2c_desc, data, length, 1);
}

int32_t max31889_trig_conversion(struct max31889_desc *desc)
{
	if (!desc || !desc->i2c_desc)
		return -EINVAL;

	return no_os_i2c_write(desc->i2c_desc, (uint8_t[]) {
		MAX31889_REG_TEMP_SETUP, MAX31889_MEASUREMENT_TRIG_VALUE
	}, 2, 1);

}

int32_t max31889_trig_and_read_temp(struct max31889_desc *desc,
				    double *temperature)
{
	int32_t ret = 0;
	uint8_t reg_tmp;
	int16_t raw_temp;
	uint8_t data[2]	= {0};
	uint16_t step_counter = 0;

	if (!desc || !temperature)
		return -EINVAL;

	ret = max31889_trig_conversion(desc);
	if (ret < 0)
		return ret;

	// Wait for the temperature measurement to be ready
	// Check the status register until the temperature is ready
	do {
		ret = max31889_read_reg(desc, MAX31889_REG_STATUS, &reg_tmp, 1);
		if (ret)
			return ret;
		++step_counter;
		no_os_mdelay(
			TEMP_MEAS_WAIT_UNTIL_ERROR_MS); // Delay to allow the conversion to complete
	} while (!(reg_tmp & NO_OS_BIT(1)) && step_counter < MAX31889_MAX_CONV_STEPS);

	if (!(reg_tmp & NO_OS_BIT(1)))
		return -ETIMEDOUT;

	ret = max31889_read_reg(desc, MAX31889_REG_FIFO_DATA, data, 2);
	if (ret < 0)
		return ret;

	raw_temp = no_os_get_unaligned_be16(data);

	*temperature = raw_temp * MAX31889_CONV_RATE; // Convert to Celsius
	return ret;
}

int32_t max31889_init(struct max31889_desc **desc,
		      struct max31889_init_param *param)
{
	int32_t ret = 0;
	struct max31889_desc *max31889_desc_tmp;

	if (!desc || !param)
		return -EINVAL;

	max31889_desc_tmp = (struct max31889_desc *)no_os_malloc(sizeof(
				    *max31889_desc_tmp));
	if (!max31889_desc_tmp)
		return -ENOMEM;

	ret = no_os_i2c_init(&max31889_desc_tmp->i2c_desc, param->i2c_ip);
	if (ret) {
		no_os_free(max31889_desc_tmp);
		return ret;
	}

	*desc = max31889_desc_tmp;

	return 0;
}

int32_t max31889_remove(struct max31889_desc *desc)
{
	if (!desc)
		return -EINVAL;
	if (desc->i2c_desc)
		no_os_i2c_remove(desc->i2c_desc);
	no_os_free(desc);

	return 0;
}

/***************************************************************************//**
 *   @file   max31889.c
 *   @brief  Temperature sensor functionalities
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#include "common_data.h"
#include "max31889.h"

int32_t max31889_write_reg(struct max31889_desc *desc, uint8_t reg_addr,
			   uint8_t *data, uint8_t length)
{
	int32_t ret = 0;
	struct no_os_i2c_desc *i2c_desc;

	if (!desc && !desc->i2c_desc)
		return -EINVAL;

	i2c_desc = desc->i2c_desc;

	// Write the register address
	ret = no_os_i2c_write(i2c_desc, &reg_addr, 1, 0);
	if (ret) {
		printf("Failed to write to MAX31889 register");
		return ret;
	}
	// Write the data to the register
	ret = no_os_i2c_write(i2c_desc, data, length, 1);
	if (ret) {
		printf("Failed to write data to MAX31889 register");
		return ret;
	}

	return ret;
}

int32_t max31889_read_reg(struct max31889_desc *desc, uint8_t reg_addr,
			  uint8_t *data, uint8_t length)
{
	int32_t ret = 0;
	struct no_os_i2c_desc *i2c_desc;

	if (!desc && !desc->i2c_desc)
		return -EINVAL;

	i2c_desc = desc->i2c_desc;

	// Set the register address to read
	ret = no_os_i2c_write(i2c_desc, &reg_addr, 1, 0);

	if (ret) {
		printf("Failed to write to MAX31889 register");
		return ret;
	}

	ret = no_os_i2c_read(i2c_desc, data, length, 1);
	if (ret) {
		printf("Failed to read register value from MAX31889");
		return ret;
	}

	return ret;
}

int32_t max31889_trig_conversion(struct max31889_desc *desc)
{
	int32_t ret = 0;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_write(desc->i2c_desc, (uint8_t[]) {
		MAX31889_REG_TEMP_SETUP, 0x01
	}, 2, 1);
	if (ret) {
		printf("Failed to write to MAX31889 system control register.");
		return ret;
	}

	return ret;
}

int32_t max31889_trig_and_read_temp(struct max31889_desc *desc,
				    float *temperature)
{
	int32_t ret = 0;
	uint8_t data[2]	= {0};

	if (!desc || !temperature)
		return -EINVAL;

	//Enable one conversion, sets back to 0 after one measurement
	ret = max31889_trig_conversion(desc);
	if (ret < 0) {
		printf("Failed to trigger conversion on MAX31889.\n\r");
		return ret;
	}

	ret = max31889_read_reg(desc, MAX31889_REG_FIFO_DATA, data, 2);
	if (ret < 0) {
		printf("Failed to read temperature data from MAX31889.\n\r");
		return ret;
	}

	int16_t raw_temp = (data[0] << 8) | data[1];
	*temperature = raw_temp * 0.005; // Convert to Celsius
	return ret;
}

int32_t setup_max31889(struct max31889_desc *desc)
{
	int32_t ret = 0;
	float temperature;
	struct no_os_i2c_desc *max31889_i2c_desc;

	if (!desc || !desc->i2c_desc) {
		printf("I2C descriptor is NULL.\n\r");
		return -1;
	}

	max31889_i2c_desc = desc->i2c_desc;

	while (true) {
		ret = max31889_trig_and_read_temp(desc, &temperature);
		if (ret) {
			printf("Failed to read temperature from MAX31889.\n\r");
		} else {
			printf("Temperature: %.2f °C\n\r", temperature);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
	return ret;
}

int32_t max31889_init(struct max31889_desc **desc,
		      struct max31889_init_param *param)
{
	int32_t ret = 0;
	struct max31889_desc *max31889_desc;

	if (!desc || !param)
		return -EINVAL;

	max31889_desc = (struct max31889_desc *)malloc(sizeof(*max31889_desc));
	if (!max31889_desc)
		return -ENOMEM;

	ret = no_os_i2c_init(&max31889_desc->i2c_desc, param->i2c_ip);
	if (ret) {
		free(max31889_desc);
		return ret;
	}

	*desc = max31889_desc;
	return ret;
}

int32_t max31889_remove(struct max31889_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (desc->i2c_desc)
		no_os_i2c_remove(desc->i2c_desc);

	free(desc);

	return 0;
}
/***************************************************************************//**
 *   @file   pico/pico_i2c.c
 *   @brief  Implementation of pico i2c driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "pico_i2c.h"
#include "pico/stdlib.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define PICO_I2C_MAX_INSTANCES	2U

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static uint8_t last_slave_address[PICO_I2C_MAX_INSTANCES];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static int pico_i2c_config_baudrate(struct no_os_i2c_desc *desc)
{
	struct pico_i2c_desc *pico_i2c;

	pico_i2c = desc->extra;

	desc->max_speed_hz = i2c_set_baudrate(pico_i2c->i2c_instance,
					      desc->max_speed_hz);

	return 0;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc  - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_i2c_init(struct no_os_i2c_desc **desc,
		      const struct no_os_i2c_init_param *param)
{
	int32_t ret;
	struct no_os_i2c_desc *descriptor;
	struct pico_i2c_desc *pico_i2c;
	struct pico_i2c_init_param *pico_i2c_ip;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	descriptor = (struct no_os_i2c_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	pico_i2c = (struct pico_i2c_desc *)no_os_calloc(1, sizeof(*pico_i2c));
	if (!pico_i2c) {
		ret = -ENOMEM;
		goto free_desc;
	}

	switch (param->device_id) {
	case 0:
		pico_i2c->i2c_instance = i2c0;
		break;
	case 1:
		pico_i2c->i2c_instance = i2c1;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};

	descriptor->max_speed_hz = i2c_init(pico_i2c->i2c_instance,
					    param->max_speed_hz);

	pico_i2c_ip = param->extra;
	gpio_set_function(pico_i2c_ip->i2c_sda_pin, GPIO_FUNC_I2C);
	gpio_set_function(pico_i2c_ip->i2c_scl_pin, GPIO_FUNC_I2C);
	gpio_pull_up(pico_i2c_ip->i2c_sda_pin);
	gpio_pull_up(pico_i2c_ip->i2c_scl_pin);

	descriptor->extra = pico_i2c;
	descriptor->device_id = param->device_id;
	descriptor->slave_address = param->slave_address;
	*desc = descriptor;

	return 0;
error:
	no_os_free(pico_i2c);
free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct pico_i2c_desc *pico_i2c;

	if (!desc || !desc->extra)
		return -EINVAL;

	pico_i2c = desc->extra;
	i2c_deinit(pico_i2c->i2c_instance);

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief I2C write transaction as master.
 * @param desc         - The I2C descriptor.
 * @param data         - The buffer with the data to transmit.
 * @param bytes_number - Number of bytes in the buffer.
 * @param stop_bit     - Specifies whether to end the transaction with a stop bit.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_i2c_write(struct no_os_i2c_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number,
		       uint8_t stop_bit)
{
	int ret;
	struct pico_i2c_desc *pico_i2c;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	pico_i2c = desc->extra;

	if (desc->slave_address != last_slave_address[desc->device_id]) {
		last_slave_address[desc->device_id] = desc->slave_address;
		ret = pico_i2c_config_baudrate(desc);
		if (ret)
			return ret;
	}

	ret = i2c_write_blocking(pico_i2c->i2c_instance, desc->slave_address, data,
				 bytes_number, !stop_bit);

	if (ret == PICO_ERROR_GENERIC)
		return -EINVAL;

	return 0;
}

/**
 * @brief I2C read transaction as master.
 * @param desc         - The I2C descriptor.
 * @param data         - The buffer where received data is to be stored.
 * @param bytes_number - Number of bytes to receive.
 * @param stop_bit     - Specifies whether to end the transaction with a stop bit.
 * @return 0 in case of success, error code otherwise.
 */

int32_t pico_i2c_read(struct no_os_i2c_desc *desc,
		      uint8_t *data,
		      uint8_t bytes_number,
		      uint8_t stop_bit)
{
	int ret;
	struct pico_i2c_desc *pico_i2c;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	pico_i2c = desc->extra;

	if (desc->slave_address != last_slave_address[desc->device_id]) {
		last_slave_address[desc->device_id] = desc->slave_address;
		ret = pico_i2c_config_baudrate(desc);
		if (ret)
			return ret;
	}

	ret = i2c_read_blocking(pico_i2c->i2c_instance, desc->slave_address, data,
				bytes_number, !stop_bit);

	if (ret == PICO_ERROR_GENERIC)
		return -EINVAL;

	return 0;
}

/**
 * @brief pico platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops pico_i2c_ops = {
	.i2c_ops_init = &pico_i2c_init,
	.i2c_ops_write = &pico_i2c_write,
	.i2c_ops_read = &pico_i2c_read,
	.i2c_ops_remove = &pico_i2c_remove
};

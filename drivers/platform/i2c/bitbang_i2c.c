/***************************************************************************//**
 *   @file   bitbang_i2c.c
 *   @brief  Implementation of bitbang I2C driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <stdlib.h>
#include <stdbool.h>
#include "bitbang_i2c.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

static void bitbang_i2c_delay(uint32_t frequency);
static int32_t bitbang_i2c_start_condition(struct bitbang_i2c_desc *desc);
static int32_t bitbang_i2c_stop_condition(struct bitbang_i2c_desc *desc);
static int32_t bitbang_i2c_write_bit(struct bitbang_i2c_desc *desc, uint8_t bit);
static int32_t bitbang_i2c_read_bit(struct bitbang_i2c_desc *desc, uint8_t *bit);
static int32_t bitbang_i2c_write_byte(struct bitbang_i2c_desc *desc, uint8_t byte);
static int32_t bitbang_i2c_read_byte(struct bitbang_i2c_desc *desc, uint8_t *byte, bool ack);

/**
 * @brief Generate delay based on I2C frequency
 * @param frequency - I2C frequency in Hz
 */
static void bitbang_i2c_delay(uint32_t frequency)
{
	uint32_t delay_us;

	if (!frequency)
		return;

	/* Calculate half period delay in microseconds */
	delay_us = (1000000 / frequency) / 2;

	/* Minimum delay of 1us for very high frequencies */
	if (!delay_us)
		delay_us = 1;

	no_os_udelay(delay_us);
}

/**
 * @brief Generate I2C start condition
 * @param desc - Bitbang I2C descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t bitbang_i2c_start_condition(struct bitbang_i2c_desc *desc)
{
	int32_t ret;

	/* SDA high, SCL high */
	ret = no_os_gpio_set_value(desc->sda_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* SDA transitions to low while SCL is high */
	ret = no_os_gpio_set_value(desc->sda_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* SCL transitions to low */
	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	return 0;
}

/**
 * @brief Generate I2C stop condition
 * @param desc - Bitbang I2C descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t bitbang_i2c_stop_condition(struct bitbang_i2c_desc *desc)
{
	int32_t ret;

	/* SDA low, SCL low */
	ret = no_os_gpio_set_value(desc->sda_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* SCL transitions to high */
	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* SDA transitions to high while SCL is high */
	ret = no_os_gpio_set_value(desc->sda_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	return 0;
}

/**
 * @brief Write a single bit to I2C bus
 * @param desc - Bitbang I2C descriptor
 * @param bit - Bit value to write (0 or 1)
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t bitbang_i2c_write_bit(struct bitbang_i2c_desc *desc, uint8_t bit)
{
	int32_t ret;

	/* Set SDA to bit value */
	ret = no_os_gpio_set_value(desc->sda_gpio, bit ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* Clock high */
	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* Clock low */
	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	return 0;
}

/**
 * @brief Read a single bit from I2C bus
 * @param desc - Bitbang I2C descriptor
 * @param bit - Pointer to store the read bit value
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t bitbang_i2c_read_bit(struct bitbang_i2c_desc *desc, uint8_t *bit)
{
	int32_t ret;
	uint8_t gpio_val;

	/* Set SDA as input (high impedance) */
	ret = no_os_gpio_direction_input(desc->sda_gpio);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* Clock high */
	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* Read SDA value */
	ret = no_os_gpio_get_value(desc->sda_gpio, &gpio_val);
	if (ret)
		return ret;

	*bit = gpio_val;

	/* Clock low */
	ret = no_os_gpio_set_value(desc->scl_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	bitbang_i2c_delay(desc->frequency);

	/* Set SDA back to output */
	ret = no_os_gpio_direction_output(desc->sda_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Write a byte to I2C bus
 * @param desc - Bitbang I2C descriptor
 * @param byte - Byte to write
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t bitbang_i2c_write_byte(struct bitbang_i2c_desc *desc, uint8_t byte)
{
	int32_t ret;
	uint8_t ack_bit;
	int i;

	/* Write 8 bits, MSB first */
	for (i = 7; i >= 0; i--) {
		ret = bitbang_i2c_write_bit(desc, (byte >> i) & 0x01);
		if (ret)
			return ret;
	}

	/* Read ACK bit */
	ret = bitbang_i2c_read_bit(desc, &ack_bit);
	if (ret)
		return ret;

	/* ACK should be 0 (low) */
	if (ack_bit)
		return -ENODEV;

	return 0;
}

/**
 * @brief Read a byte from I2C bus
 * @param desc - Bitbang I2C descriptor
 * @param byte - Pointer to store the read byte
 * @param ack - True to send ACK, false to send NACK
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t bitbang_i2c_read_byte(struct bitbang_i2c_desc *desc, uint8_t *byte, bool ack)
{
	int32_t ret;
	uint8_t bit;
	uint8_t data;
	int i;

	data = 0;

	/* Read 8 bits, MSB first */
	for (i = 7; i >= 0; i--) {
		ret = bitbang_i2c_read_bit(desc, &bit);
		if (ret)
			return ret;
		data |= (bit << i);
	}

	*byte = data;

	/* Send ACK/NACK */
	ret = bitbang_i2c_write_bit(desc, ack ? 0 : 1);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t bitbang_i2c_init(struct no_os_i2c_desc **desc,
				const struct no_os_i2c_init_param *param)
{
	struct bitbang_i2c_desc *bitbang_desc;
	struct bitbang_i2c_init_param *bitbang_param;
	int32_t ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	bitbang_param = (struct bitbang_i2c_init_param *)param->extra;

	if (!bitbang_param->sda_gpio || !bitbang_param->scl_gpio)
		return -EINVAL;

	bitbang_desc = no_os_calloc(1, sizeof(*bitbang_desc));
	if (!bitbang_desc)
		return -ENOMEM;

	bitbang_desc->sda_gpio = bitbang_param->sda_gpio;
	bitbang_desc->scl_gpio = bitbang_param->scl_gpio;
	bitbang_desc->frequency = param->max_speed_hz;

	/* Configure GPIOs as outputs and set to high (idle state) */
	ret = no_os_gpio_direction_output(bitbang_desc->sda_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(bitbang_desc->scl_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		goto error;

	*desc = no_os_calloc(1, sizeof(**desc));
	if (!*desc) {
		ret = -ENOMEM;
		goto error;
	}

	(*desc)->extra = bitbang_desc;
	(*desc)->slave_address = param->slave_address;
	(*desc)->max_speed_hz = param->max_speed_hz;

	return 0;

error:
	no_os_free(bitbang_desc);
	return ret;
}

/**
 * @brief Free the resources allocated by bitbang_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t bitbang_i2c_remove(struct no_os_i2c_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Specifies whether to end the transaction with a stop bit.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t bitbang_i2c_write(struct no_os_i2c_desc *desc,
				 uint8_t *data,
				 uint8_t bytes_number,
				 uint8_t stop_bit)
{
	struct bitbang_i2c_desc *bitbang_desc;
	int32_t ret;
	int32_t stop_ret;
	uint8_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	bitbang_desc = (struct bitbang_i2c_desc *)desc->extra;

	/* Start condition */
	ret = bitbang_i2c_start_condition(bitbang_desc);
	if (ret)
		return ret;

	/* Send slave address with write bit (0) */
	ret = bitbang_i2c_write_byte(bitbang_desc, (desc->slave_address << 1) | 0);
	if (ret)
		goto stop;

	/* Send data bytes */
	for (i = 0; i < bytes_number; i++) {
		ret = bitbang_i2c_write_byte(bitbang_desc, data[i]);
		if (ret)
			goto stop;
	}

stop:
	/* Send stop condition if requested or on error */
	if (stop_bit || ret) {
		stop_ret = bitbang_i2c_stop_condition(bitbang_desc);
		if (!ret)
			ret = stop_ret;
	}

	return ret;
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Specifies whether to end the transaction with a stop bit.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t bitbang_i2c_read(struct no_os_i2c_desc *desc,
				uint8_t *data,
				uint8_t bytes_number,
				uint8_t stop_bit)
{
	struct bitbang_i2c_desc *bitbang_desc;
	int32_t ret;
	int32_t stop_ret;
	uint8_t i;
	bool send_ack;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	bitbang_desc = (struct bitbang_i2c_desc *)desc->extra;

	/* Start condition */
	ret = bitbang_i2c_start_condition(bitbang_desc);
	if (ret)
		return ret;

	/* Send slave address with read bit (1) */
	ret = bitbang_i2c_write_byte(bitbang_desc, (desc->slave_address << 1) | 1);
	if (ret)
		goto stop;

	/* Read data bytes */
	for (i = 0; i < bytes_number; i++) {
		send_ack = (i < bytes_number - 1); /* ACK all but last byte */
		ret = bitbang_i2c_read_byte(bitbang_desc, &data[i], send_ack);
		if (ret)
			goto stop;
	}

stop:
	/* Send stop condition if requested or on error */
	if (stop_bit || ret) {
		stop_ret = bitbang_i2c_stop_condition(bitbang_desc);
		if (!ret)
			ret = stop_ret;
	}

	return ret;
}

/**
 * @brief Bitbang I2C platform operations structure
 */
const struct no_os_i2c_platform_ops bitbang_i2c_ops = {
	.i2c_ops_init = &bitbang_i2c_init,
	.i2c_ops_write = &bitbang_i2c_write,
	.i2c_ops_read = &bitbang_i2c_read,
	.i2c_ops_remove = &bitbang_i2c_remove
};
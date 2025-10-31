/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "capi_i2c_bitbang.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/* Helper function to set SDA line state */
static int capi_i2c_bitbang_set_sda(struct capi_i2c_bitbang_handle *desc, uint8_t state)
{
	if (desc->pull_type == CAPI_I2C_BITBANG_PULL_EXTERNAL) {
		/* Open-drain mode: high = input (pulled up), low = output low */
		if (state == CAPI_GPIO_HIGH) {
			return capi_gpio_pin_set_direction(&desc->sda_pin, CAPI_GPIO_INPUT);
		} else {
			int ret = capi_gpio_pin_set_direction(&desc->sda_pin, CAPI_GPIO_OUTPUT);
			if (ret != 0)
				return ret;
			return capi_gpio_pin_set_value(&desc->sda_pin, CAPI_GPIO_LOW);
		}
	} else {
		/* Push-pull mode */
		return capi_gpio_pin_set_value(&desc->sda_pin, state);
	}
}

/* Helper function to set SCL line state */
static int capi_i2c_bitbang_set_scl(struct capi_i2c_bitbang_handle *desc, uint8_t state)
{
	if (desc->pull_type == CAPI_I2C_BITBANG_PULL_EXTERNAL) {
		/* Open-drain mode: high = input (pulled up), low = output low */
		if (state == CAPI_GPIO_HIGH) {
			return capi_gpio_pin_set_direction(&desc->scl_pin, CAPI_GPIO_INPUT);
		} else {
			int ret = capi_gpio_pin_set_direction(&desc->scl_pin, CAPI_GPIO_OUTPUT);
			if (ret != 0)
				return ret;
			return capi_gpio_pin_set_value(&desc->scl_pin, CAPI_GPIO_LOW);
		}
	} else {
		/* Push-pull mode */
		return capi_gpio_pin_set_value(&desc->scl_pin, state);
	}
}

/* Helper function to read SDA line state */
static int capi_i2c_bitbang_get_sda(struct capi_i2c_bitbang_handle *desc, uint8_t *value)
{
	return capi_gpio_pin_get_value(&desc->sda_pin, value);
}

/* Helper function to read SCL line state */
static int capi_i2c_bitbang_get_scl(struct capi_i2c_bitbang_handle *desc, uint8_t *value)
{
	return capi_gpio_pin_get_value(&desc->scl_pin, value);
}

/* Wait for SCL to go high (clock stretching support) */
static int capi_i2c_bitbang_wait_scl_high(struct capi_i2c_bitbang_handle *desc)
{
	uint8_t scl_state;
	uint32_t timeout_count = desc->timeout_us;

	while (timeout_count--) {
		if (capi_i2c_bitbang_get_scl(desc, &scl_state) != 0)
			return -EIO;

		if (scl_state == CAPI_GPIO_HIGH)
			return 0;

		no_os_udelay(1);
	}

	return -ETIMEDOUT;
}

/* Generate I2C start condition */
static int capi_i2c_bitbang_start(struct capi_i2c_bitbang_handle *desc)
{
	int ret;

	/* Ensure both lines are high initially */
	ret = capi_i2c_bitbang_set_sda(desc, CAPI_GPIO_HIGH);
	if (ret != 0)
		return ret;

	ret = capi_i2c_bitbang_set_scl(desc, CAPI_GPIO_HIGH);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Wait for SCL to actually go high (clock stretching) */
	ret = capi_i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	/* Start condition: SDA goes low while SCL is high */
	ret = capi_i2c_bitbang_set_sda(desc, CAPI_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Pull SCL low to complete start condition */
	ret = capi_i2c_bitbang_set_scl(desc, CAPI_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	return 0;
}

/* Generate I2C stop condition */
static int capi_i2c_bitbang_stop(struct capi_i2c_bitbang_handle *desc)
{
	int ret;

	/* Ensure SDA is low */
	ret = capi_i2c_bitbang_set_sda(desc, CAPI_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Release SCL (let it go high) */
	ret = capi_i2c_bitbang_set_scl(desc, CAPI_GPIO_HIGH);
	if (ret != 0)
		return ret;

	/* Wait for SCL to actually go high */
	ret = capi_i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Stop condition: SDA goes high while SCL is high */
	ret = capi_i2c_bitbang_set_sda(desc, CAPI_GPIO_HIGH);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	return 0;
}

/* Write a single bit to the I2C bus */
static int capi_i2c_bitbang_write_bit(struct capi_i2c_bitbang_handle *desc, uint8_t bit)
{
	int ret;

	/* Set data line */
	ret = capi_i2c_bitbang_set_sda(desc, bit ? CAPI_GPIO_HIGH : CAPI_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Clock high */
	ret = capi_i2c_bitbang_set_scl(desc, CAPI_GPIO_HIGH);
	if (ret != 0)
		return ret;

	/* Wait for SCL to go high */
	ret = capi_i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Clock low */
	ret = capi_i2c_bitbang_set_scl(desc, CAPI_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	return 0;
}

/* Read a single bit from the I2C bus */
static int capi_i2c_bitbang_read_bit(struct capi_i2c_bitbang_handle *desc, uint8_t *bit)
{
	int ret;

	/* Release SDA for input */
	ret = capi_i2c_bitbang_set_sda(desc, CAPI_GPIO_HIGH);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Clock high */
	ret = capi_i2c_bitbang_set_scl(desc, CAPI_GPIO_HIGH);
	if (ret != 0)
		return ret;

	/* Wait for SCL to go high */
	ret = capi_i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Read data */
	ret = capi_i2c_bitbang_get_sda(desc, bit);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Clock low */
	ret = capi_i2c_bitbang_set_scl(desc, CAPI_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	return 0;
}

/* Write a byte to the I2C bus and read ACK */
static int capi_i2c_bitbang_write_byte(struct capi_i2c_bitbang_handle *desc, uint8_t byte)
{
	int ret;
	uint8_t i, ack;

	/* Send 8 bits, MSB first */
	for (i = 0; i < 8; i++) {
		ret = capi_i2c_bitbang_write_bit(desc, (byte >> (7 - i)) & 0x01);
		if (ret != 0)
			return ret;
	}

	/* Read ACK bit */
	ret = capi_i2c_bitbang_read_bit(desc, &ack);
	if (ret != 0)
		return ret;

	/* ACK should be 0, NACK is 1 */
	return (ack == 0) ? 0 : -EIO;
}

/* Read a byte from the I2C bus and send ACK/NACK */
static int capi_i2c_bitbang_read_byte(struct capi_i2c_bitbang_handle *desc, uint8_t *byte,
				      bool send_nack)
{
	int ret;
	uint8_t i, bit;

	*byte = 0;

	/* Read 8 bits, MSB first */
	for (i = 0; i < 8; i++) {
		ret = capi_i2c_bitbang_read_bit(desc, &bit);
		if (ret != 0)
			return ret;

		*byte = (*byte << 1) | (bit & 0x01);
	}

	/* Send ACK (0) or NACK (1) */
	ret = capi_i2c_bitbang_write_bit(desc, send_nack ? 1 : 0);
	if (ret != 0)
		return ret;

	return 0;
}

/* Initialize the I2C bitbang controller */
static int capi_i2c_bitbang_init(struct capi_i2c_controller_handle **handle,
				 const struct capi_i2c_config *config)
{
	struct capi_i2c_bitbang_handle *bitbang;
	struct capi_i2c_bitbang_extra *extra;
	int ret;
	uint32_t speed_hz;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	extra = (struct capi_i2c_bitbang_extra *)config->extra;

	/* Allocate bitbang handle */
	bitbang = (struct capi_i2c_bitbang_handle *)no_os_calloc(1, sizeof(*bitbang));
	if (!bitbang)
		return -ENOMEM;

	bitbang->base.init_allocated = true;
	bitbang->base.ops = config->ops;

	/* Copy GPIO pins */
	bitbang->sda_pin = extra->sda_pin;
	bitbang->scl_pin = extra->scl_pin;

	/* Configure pull-up settings */
	bitbang->pull_type = extra->pull_type;
	bitbang->timeout_us = extra->timeout_us ? extra->timeout_us : CAPI_I2C_BITBANG_TIMEOUT_US;

	/* Calculate timing parameters from clock frequency */
	speed_hz = config->clk_freq_hz ? config->clk_freq_hz : CAPI_I2C_BITBANG_SPEED_STANDARD;
	bitbang->half_period_us = 1000000 / (2 * speed_hz);
	bitbang->quarter_period_us = bitbang->half_period_us / 2;

	/* Ensure minimum timing */
	if (bitbang->half_period_us < 1)
		bitbang->half_period_us = 1;
	if (bitbang->quarter_period_us < 1)
		bitbang->quarter_period_us = 1;

	/* Configure GPIO initial state based on pull-up configuration */
	if (bitbang->pull_type == CAPI_I2C_BITBANG_PULL_EXTERNAL) {
		/* Open-drain: configure as input initially (pulled high) */
		ret = capi_gpio_pin_set_direction(&bitbang->sda_pin, CAPI_GPIO_INPUT);
		if (ret != 0)
			goto error;

		ret = capi_gpio_pin_set_direction(&bitbang->scl_pin, CAPI_GPIO_INPUT);
		if (ret != 0)
			goto error;
	} else {
		/* Push-pull: configure as output high */
		ret = capi_gpio_pin_set_direction(&bitbang->sda_pin, CAPI_GPIO_OUTPUT);
		if (ret != 0)
			goto error;

		ret = capi_gpio_pin_set_value(&bitbang->sda_pin, CAPI_GPIO_HIGH);
		if (ret != 0)
			goto error;

		ret = capi_gpio_pin_set_direction(&bitbang->scl_pin, CAPI_GPIO_OUTPUT);
		if (ret != 0)
			goto error;

		ret = capi_gpio_pin_set_value(&bitbang->scl_pin, CAPI_GPIO_HIGH);
		if (ret != 0)
			goto error;
	}

	*handle = (struct capi_i2c_controller_handle *)bitbang;

	return 0;

error:
	no_os_free(bitbang);
	return ret;
}

/* Deinitialize the I2C bitbang controller */
static int capi_i2c_bitbang_deinit(struct capi_i2c_controller_handle *handle)
{
	if (!handle)
		return -EINVAL;

	if (handle->init_allocated)
		no_os_free(handle);

	return 0;
}

/* Transmit data to I2C device */
static int capi_i2c_bitbang_transmit(struct capi_i2c_device *device,
				     struct capi_i2c_transfer *transfer)
{
	struct capi_i2c_bitbang_handle *bitbang;
	int ret;
	uint32_t i;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	bitbang = (struct capi_i2c_bitbang_handle *)device->controller;

	/* Generate start condition */
	ret = capi_i2c_bitbang_start(bitbang);
	if (ret != 0)
		return ret;

	/* Send slave address with write bit (0) */
	ret = capi_i2c_bitbang_write_byte(bitbang, (device->address << 1) | 0);
	if (ret != 0)
		goto error_stop;

	/* Send sub-address if present */
	if (transfer->sub_address && transfer->sub_address_len > 0) {
		for (i = 0; i < transfer->sub_address_len; i++) {
			ret = capi_i2c_bitbang_write_byte(bitbang, transfer->sub_address[i]);
			if (ret != 0)
				goto error_stop;
		}
	}

	/* Send data bytes */
	for (i = 0; i < transfer->len; i++) {
		ret = capi_i2c_bitbang_write_byte(bitbang, transfer->buf[i]);
		if (ret != 0)
			goto error_stop;
	}

	/* Generate stop condition if requested */
	if (!transfer->no_stop) {
		ret = capi_i2c_bitbang_stop(bitbang);
		if (ret != 0)
			return ret;
	}

	return 0;

error_stop:
	if (!transfer->no_stop)
		capi_i2c_bitbang_stop(bitbang);
	return ret;
}

/* Receive data from I2C device */
static int capi_i2c_bitbang_receive(struct capi_i2c_device *device,
				    struct capi_i2c_transfer *transfer)
{
	struct capi_i2c_bitbang_handle *bitbang;
	int ret;
	uint32_t i;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	bitbang = (struct capi_i2c_bitbang_handle *)device->controller;

	/* If sub-address is specified, write it first */
	if (transfer->sub_address && transfer->sub_address_len > 0) {
		/* Generate start condition */
		ret = capi_i2c_bitbang_start(bitbang);
		if (ret != 0)
			return ret;

		/* Send slave address with write bit (0) */
		ret = capi_i2c_bitbang_write_byte(bitbang, (device->address << 1) | 0);
		if (ret != 0)
			goto error_stop;

		/* Send sub-address */
		for (i = 0; i < transfer->sub_address_len; i++) {
			ret = capi_i2c_bitbang_write_byte(bitbang, transfer->sub_address[i]);
			if (ret != 0)
				goto error_stop;
		}

		/* Generate repeated start if requested */
		if (transfer->repeated_start) {
			ret = capi_i2c_bitbang_start(bitbang);
			if (ret != 0)
				goto error_stop;
		}
	} else {
		/* Generate start condition */
		ret = capi_i2c_bitbang_start(bitbang);
		if (ret != 0)
			return ret;
	}

	/* Send slave address with read bit (1) */
	ret = capi_i2c_bitbang_write_byte(bitbang, (device->address << 1) | 1);
	if (ret != 0)
		goto error_stop;

	/* Read data bytes */
	for (i = 0; i < transfer->len; i++) {
		bool send_nack = (i == transfer->len - 1); /* NACK on last byte */
		ret = capi_i2c_bitbang_read_byte(bitbang, &transfer->buf[i], send_nack);
		if (ret != 0)
			goto error_stop;
	}

	/* Generate stop condition if requested */
	if (!transfer->no_stop) {
		ret = capi_i2c_bitbang_stop(bitbang);
		if (ret != 0)
			return ret;
	}

	return 0;

error_stop:
	if (!transfer->no_stop)
		capi_i2c_bitbang_stop(bitbang);
	return ret;
}

/* Register callback (not supported for bitbang) */
static int capi_i2c_bitbang_register_callback(struct capi_i2c_controller_handle *handle,
		capi_i2c_callback *const callback, void *const callback_arg)
{
	/* Bitbang implementation doesn't support async operations */
	return -ENOTSUP;
}

/* Async transmit (not supported for bitbang) */
static int capi_i2c_bitbang_transmit_async(struct capi_i2c_device *device,
		struct capi_i2c_transfer *transfer)
{
	/* Bitbang implementation doesn't support async operations */
	return -ENOTSUP;
}

/* Async receive (not supported for bitbang) */
static int capi_i2c_bitbang_receive_async(struct capi_i2c_device *device,
		struct capi_i2c_transfer *transfer)
{
	/* Bitbang implementation doesn't support async operations */
	return -ENOTSUP;
}

/* ISR (not applicable for bitbang) */
static void capi_i2c_bitbang_isr(struct capi_i2c_controller_handle *handle)
{
	/* Bitbang implementation doesn't use interrupts */
}

/* I2C bitbang operations structure */
const struct capi_i2c_ops capi_i2c_bitbang_ops = {
	.init = capi_i2c_bitbang_init,
	.deinit = capi_i2c_bitbang_deinit,
	.transmit = capi_i2c_bitbang_transmit,
	.receive = capi_i2c_bitbang_receive,
	.register_callback = capi_i2c_bitbang_register_callback,
	.transmit_async = capi_i2c_bitbang_transmit_async,
	.receive_async = capi_i2c_bitbang_receive_async,
	.isr = capi_i2c_bitbang_isr
};

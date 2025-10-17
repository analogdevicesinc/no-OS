/***************************************************************************//**
 *   @file   i2c_bitbang.c
 *   @brief  Implementation of I2C bitbang driver
 *   @author Generated with Claude Code
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files *********************************/
/******************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "i2c_bitbang.h"

/******************************************************************************/
/************************ Functions Definitions ******************************/
/******************************************************************************/

/**
 * @brief Set SDA line state
 * @param desc - I2C bitbang descriptor
 * @param state - GPIO state to set
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_set_sda(struct i2c_bitbang_desc *desc, uint8_t state)
{
	if (desc->pull_type == I2C_BITBANG_PULL_EXTERNAL) {
		/* Open-drain mode: high = input (pulled up), low = output low */
		if (state == NO_OS_GPIO_HIGH) {
			return no_os_gpio_direction_input(desc->sda);
		} else {
			return no_os_gpio_direction_output(desc->sda, NO_OS_GPIO_LOW);
		}
	} else {
		/* Push-pull mode */
		return no_os_gpio_set_value(desc->sda, state);
	}
}

/**
 * @brief Set SCL line state
 * @param desc - I2C bitbang descriptor
 * @param state - GPIO state to set
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_set_scl(struct i2c_bitbang_desc *desc, uint8_t state)
{
	if (desc->pull_type == I2C_BITBANG_PULL_EXTERNAL) {
		/* Open-drain mode: high = input (pulled up), low = output low */
		if (state == NO_OS_GPIO_HIGH) {
			return no_os_gpio_direction_input(desc->scl);
		} else {
			return no_os_gpio_direction_output(desc->scl, NO_OS_GPIO_LOW);
		}
	} else {
		/* Push-pull mode */
		return no_os_gpio_set_value(desc->scl, state);
	}
}

/**
 * @brief Read SDA line state
 * @param desc - I2C bitbang descriptor
 * @param value - Pointer to store the read value
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_get_sda(struct i2c_bitbang_desc *desc, uint8_t *value)
{
	return no_os_gpio_get_value(desc->sda, value);
}

/**
 * @brief Read SCL line state with clock stretching support
 * @param desc - I2C bitbang descriptor
 * @param value - Pointer to store the read value
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_get_scl(struct i2c_bitbang_desc *desc, uint8_t *value)
{
	return no_os_gpio_get_value(desc->scl, value);
}

/**
 * @brief Wait for SCL to go high (clock stretching support)
 * @param desc - I2C bitbang descriptor
 * @return 0 in case of success, -ETIMEDOUT if timeout occurs
 */
static int32_t i2c_bitbang_wait_scl_high(struct i2c_bitbang_desc *desc)
{
	uint8_t scl_state;
	uint32_t timeout_count = desc->timeout_us;

	while (timeout_count--) {
		if (i2c_bitbang_get_scl(desc, &scl_state) != 0)
			return -EIO;

		if (scl_state == NO_OS_GPIO_HIGH)
			return 0;

		no_os_udelay(1);
	}

	return -ETIMEDOUT;
}

/**
 * @brief Generate I2C start condition
 * @param desc - I2C bitbang descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_start(struct i2c_bitbang_desc *desc)
{
	int32_t ret;

	/* Ensure both lines are high initially */
	ret = i2c_bitbang_set_sda(desc, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	ret = i2c_bitbang_set_scl(desc, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Wait for SCL to actually go high (clock stretching) */
	ret = i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	/* Start condition: SDA goes low while SCL is high */
	ret = i2c_bitbang_set_sda(desc, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Pull SCL low to complete start condition */
	ret = i2c_bitbang_set_scl(desc, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	return 0;
}

/**
 * @brief Generate I2C stop condition
 * @param desc - I2C bitbang descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_stop(struct i2c_bitbang_desc *desc)
{
	int32_t ret;

	/* Ensure SDA is low */
	ret = i2c_bitbang_set_sda(desc, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Release SCL (let it go high) */
	ret = i2c_bitbang_set_scl(desc, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	/* Wait for SCL to actually go high */
	ret = i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Stop condition: SDA goes high while SCL is high */
	ret = i2c_bitbang_set_sda(desc, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	return 0;
}

/**
 * @brief Write a single bit to the I2C bus
 * @param desc - I2C bitbang descriptor
 * @param bit - Bit value (0 or 1)
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_write_bit(struct i2c_bitbang_desc *desc, uint8_t bit)
{
	int32_t ret;

	/* Set data line */
	ret = i2c_bitbang_set_sda(desc, bit ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Clock high */
	ret = i2c_bitbang_set_scl(desc, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	/* Wait for SCL to go high */
	ret = i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->half_period_us);

	/* Clock low */
	ret = i2c_bitbang_set_scl(desc, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	return 0;
}

/**
 * @brief Read a single bit from the I2C bus
 * @param desc - I2C bitbang descriptor
 * @param bit - Pointer to store the bit value
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_read_bit(struct i2c_bitbang_desc *desc, uint8_t *bit)
{
	int32_t ret;

	/* Release SDA for input */
	ret = i2c_bitbang_set_sda(desc, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Clock high */
	ret = i2c_bitbang_set_scl(desc, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	/* Wait for SCL to go high */
	ret = i2c_bitbang_wait_scl_high(desc);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Read data */
	ret = i2c_bitbang_get_sda(desc, bit);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	/* Clock low */
	ret = i2c_bitbang_set_scl(desc, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_udelay(desc->quarter_period_us);

	return 0;
}

/**
 * @brief Write a byte to the I2C bus and read ACK
 * @param desc - I2C bitbang descriptor
 * @param byte - Byte to write
 * @return 0 in case of success (ACK received), negative error code otherwise
 */
static int32_t i2c_bitbang_write_byte(struct i2c_bitbang_desc *desc, uint8_t byte)
{
	int32_t ret;
	uint8_t i, ack;

	/* Send 8 bits, MSB first */
	for (i = 0; i < 8; i++) {
		ret = i2c_bitbang_write_bit(desc, (byte >> (7 - i)) & 0x01);
		if (ret != 0)
			return ret;
	}

	/* Read ACK bit */
	ret = i2c_bitbang_read_bit(desc, &ack);
	if (ret != 0)
		return ret;

	/* ACK should be 0, NACK is 1 */
	return (ack == 0) ? 0 : -EIO;
}

/**
 * @brief Read a byte from the I2C bus and send ACK/NACK
 * @param desc - I2C bitbang descriptor
 * @param byte - Pointer to store the read byte
 * @param send_nack - If true, send NACK; if false, send ACK
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_read_byte(struct i2c_bitbang_desc *desc, uint8_t *byte, bool send_nack)
{
	int32_t ret;
	uint8_t i, bit;

	*byte = 0;

	/* Read 8 bits, MSB first */
	for (i = 0; i < 8; i++) {
		ret = i2c_bitbang_read_bit(desc, &bit);
		if (ret != 0)
			return ret;

		*byte = (*byte << 1) | (bit & 0x01);
	}

	/* Send ACK (0) or NACK (1) */
	ret = i2c_bitbang_write_bit(desc, send_nack ? 1 : 0);
	if (ret != 0)
		return ret;

	return 0;
}

/**
 * @brief Initialize the I2C bitbang driver
 * @param desc - Pointer to I2C descriptor pointer
 * @param param - I2C initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_init(struct no_os_i2c_desc **desc,
				const struct no_os_i2c_init_param *param)
{
	struct no_os_i2c_desc *descriptor;
	struct i2c_bitbang_desc *bitbang;
	struct i2c_bitbang_init_param *init_param;
	int32_t ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	init_param = (struct i2c_bitbang_init_param *)param->extra;

	/* Allocate descriptor */
	descriptor = (struct no_os_i2c_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	/* Allocate bitbang specific structure */
	bitbang = (struct i2c_bitbang_desc *)no_os_calloc(1, sizeof(*bitbang));
	if (!bitbang) {
		ret = -ENOMEM;
		goto error_desc;
	}

	/* Initialize GPIO pins */
	ret = no_os_gpio_get(&bitbang->sda, &init_param->sda_init);
	if (ret != 0)
		goto error_bitbang;

	ret = no_os_gpio_get(&bitbang->scl, &init_param->scl_init);
	if (ret != 0)
		goto error_sda;

	/* Configure pull-up settings */
	bitbang->pull_type = init_param->pull_type;
	bitbang->timeout_us = init_param->timeout_us ? init_param->timeout_us : I2C_BITBANG_TIMEOUT_US;

	/* Calculate timing parameters */
	if (param->max_speed_hz == 0) {
		bitbang->half_period_us = 1000000 / (2 * I2C_BITBANG_SPEED_STANDARD);
	} else {
		bitbang->half_period_us = 1000000 / (2 * param->max_speed_hz);
	}
	bitbang->quarter_period_us = bitbang->half_period_us / 2;

	/* Ensure minimum timing */
	if (bitbang->half_period_us < 1)
		bitbang->half_period_us = 1;
	if (bitbang->quarter_period_us < 1)
		bitbang->quarter_period_us = 1;

	/* Configure GPIO initial state based on pull-up configuration */
	if (bitbang->pull_type == I2C_BITBANG_PULL_EXTERNAL) {
		/* Open-drain: configure as input initially (pulled high) */
		ret = no_os_gpio_direction_input(bitbang->sda);
		if (ret != 0)
			goto error_scl;

		ret = no_os_gpio_direction_input(bitbang->scl);
		if (ret != 0)
			goto error_scl;
	} else {
		/* Push-pull: configure as output high */
		ret = no_os_gpio_direction_output(bitbang->sda, NO_OS_GPIO_HIGH);
		if (ret != 0)
			goto error_scl;

		ret = no_os_gpio_direction_output(bitbang->scl, NO_OS_GPIO_HIGH);
		if (ret != 0)
			goto error_scl;
	}

	/* Initialize descriptor */
	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->slave_address = param->slave_address;
	descriptor->extra = bitbang;

	*desc = descriptor;

	return 0;

error_scl:
	no_os_gpio_remove(bitbang->scl);
error_sda:
	no_os_gpio_remove(bitbang->sda);
error_bitbang:
	no_os_free(bitbang);
error_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by i2c_bitbang_init()
 * @param desc - I2C descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_remove(struct no_os_i2c_desc *desc)
{
	struct i2c_bitbang_desc *bitbang;

	if (!desc || !desc->extra)
		return -EINVAL;

	bitbang = (struct i2c_bitbang_desc *)desc->extra;

	/* Free GPIO resources */
	no_os_gpio_remove(bitbang->scl);
	no_os_gpio_remove(bitbang->sda);

	/* Free allocated memory */
	no_os_free(bitbang);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write data to a slave device
 * @param desc - I2C descriptor
 * @param data - Buffer containing data to write
 * @param bytes_number - Number of bytes to write
 * @param stop_bit - Generate stop condition if non-zero
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_write(struct no_os_i2c_desc *desc,
				 uint8_t *data,
				 uint8_t bytes_number,
				 uint8_t stop_bit)
{
	struct i2c_bitbang_desc *bitbang;
	int32_t ret;
	uint8_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	bitbang = (struct i2c_bitbang_desc *)desc->extra;

	/* Generate start condition */
	ret = i2c_bitbang_start(bitbang);
	if (ret != 0)
		return ret;

	/* Send slave address with write bit (0) */
	ret = i2c_bitbang_write_byte(bitbang, (desc->slave_address << 1) | 0);
	if (ret != 0)
		goto error_stop;

	/* Send data bytes */
	for (i = 0; i < bytes_number; i++) {
		ret = i2c_bitbang_write_byte(bitbang, data[i]);
		if (ret != 0)
			goto error_stop;
	}

	/* Generate stop condition if requested */
	if (stop_bit) {
		ret = i2c_bitbang_stop(bitbang);
		if (ret != 0)
			return ret;
	}

	return 0;

error_stop:
	if (stop_bit)
		i2c_bitbang_stop(bitbang);
	return ret;
}

/**
 * @brief Read data from a slave device
 * @param desc - I2C descriptor
 * @param data - Buffer to store read data
 * @param bytes_number - Number of bytes to read
 * @param stop_bit - Generate stop condition if non-zero
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t i2c_bitbang_read(struct no_os_i2c_desc *desc,
				uint8_t *data,
				uint8_t bytes_number,
				uint8_t stop_bit)
{
	struct i2c_bitbang_desc *bitbang;
	int32_t ret;
	uint8_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	bitbang = (struct i2c_bitbang_desc *)desc->extra;

	/* Generate start condition */
	ret = i2c_bitbang_start(bitbang);
	if (ret != 0)
		return ret;

	/* Send slave address with read bit (1) */
	ret = i2c_bitbang_write_byte(bitbang, (desc->slave_address << 1) | 1);
	if (ret != 0)
		goto error_stop;

	/* Read data bytes */
	for (i = 0; i < bytes_number; i++) {
		bool send_nack = (i == bytes_number - 1); /* NACK on last byte */
		ret = i2c_bitbang_read_byte(bitbang, &data[i], send_nack);
		if (ret != 0)
			goto error_stop;
	}

	/* Generate stop condition if requested */
	if (stop_bit) {
		ret = i2c_bitbang_stop(bitbang);
		if (ret != 0)
			return ret;
	}

	return 0;

error_stop:
	if (stop_bit)
		i2c_bitbang_stop(bitbang);
	return ret;
}

/**
 * @brief I2C bitbang platform operations structure
 */
const struct no_os_i2c_platform_ops i2c_bitbang_ops = {
	.i2c_ops_init = i2c_bitbang_init,
	.i2c_ops_write = i2c_bitbang_write,
	.i2c_ops_read = i2c_bitbang_read,
	.i2c_ops_remove = i2c_bitbang_remove
};
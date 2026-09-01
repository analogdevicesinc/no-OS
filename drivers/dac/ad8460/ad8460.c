/***************************************************************************//**
 *   @file   ad8460.c
 *   @brief  Implementation of AD8460 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "ad8460.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/******************************************************************************/

/**
 * @brief Read a register value
 * @param dev - AD8460 descriptor
 * @param addr - register address
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_reg_read(struct ad8460_device *dev, uint8_t addr, uint8_t *val)
{
	int ret;
	uint8_t buf[2];

	buf[0] = 0x80 | addr;
	buf[1] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	*val = buf[1];

	return 0;
}

/**
 * @brief Write a register value
 * @param dev - AD8460 descriptor
 * @param addr - register address
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_reg_write(struct ad8460_device *dev, uint8_t addr, uint8_t val)
{
	uint8_t buf[2];

	buf[0] = addr;
	buf[1] = val;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2);
}

/**
 * @brief Read-modify-write operation
 * @param dev - AD8460 descriptor
 * @param addr - register address
 * @param mask - Mask for specific register bits to be updated
 * @param val - register value
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_reg_update_bits(struct ad8460_device *dev, uint8_t addr,
			   uint8_t mask, uint8_t val)
{
	int ret;
	uint8_t regval;

	ret = ad8460_reg_read(dev, addr, &regval);
	if (ret)
		return ret;

	regval &= ~mask;
	regval |= no_os_field_prep(mask, val);

	return ad8460_reg_write(dev, addr, regval);
}

/**
 * @brief Device and comm init function
 * @param dev - AD8460 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ad8460_init(struct ad8460_device **dev,
		struct ad8460_init_param *init_param)
{
	struct ad8460_device *temp_dev;
	int ret;

	temp_dev = no_os_calloc(1, sizeof(*temp_dev));
	if (!temp_dev)
		return -ENOMEM;

	ret = no_os_spi_init(&temp_dev->spi_desc, &init_param->spi_init_param);
	if (ret) {
		pr_err("Failed to initialize SPI descriptor\r\n");
		goto free_dev;
	}

	ret = no_os_gpio_get_optional(&temp_dev->gpio_rstn, &init_param->gpio_rstn);
	if (ret) {
		pr_err("Failed to get reset gpio\r\n");
		goto remove_spi;
	}

	temp_dev->refio_1p2v_mv = init_param->refio_1p2v_mv;
	temp_dev->ext_resistor_ohms = init_param->ext_resistor_ohms;

	*dev = temp_dev;

	return 0;

remove_spi:
	ret = no_os_spi_remove(temp_dev->spi_desc);
	if (ret)
		pr_err("Failed to remove SPI descriptor\r\n");

free_dev:
	no_os_free(temp_dev);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param dev - AD8460 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ad8460_remove(struct ad8460_device *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Reset the AD8460 device
 * @param dev - AD8460 descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_reset(struct ad8460_device *dev)
{
	int ret;

	ret = no_os_gpio_direction_output(dev->gpio_rstn, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* minimum duration of 10ns */
	no_os_udelay(1);

	ret = no_os_gpio_set_value(dev->gpio_rstn, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* bring all registers to their default state */
	return ad8460_reg_write(dev, 0x03, 1);
}

/**
 * @brief Get the value of a high voltage DAC word
 * @param dev - AD8460 descriptor
 * @param index - index of the word to be read
 * @param val - value of the word
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_get_hvdac_word(struct ad8460_device *dev, int8_t index,
			  uint16_t *val)
{
	int ret;
	uint8_t buf[3];

	buf[0] = 0x80 | AD8460_HVDAC_DATA_WORD(index);
	buf[1] = 0x00;
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_le16(buf + 1);

	return 0;
}

/**
 * @brief Set the value of a high voltage DAC word
 * @param dev - AD8460 descriptor
 * @param index - index of the word to be set
 * @param val - value of the word
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_set_hvdac_word(struct ad8460_device *dev, int8_t index, uint16_t val)
{
	uint8_t buf[3];

	buf[0] = AD8460_HVDAC_DATA_WORD(index);

	val = no_os_field_prep(AD8460_DATA_BYTE_FULL_MSK, val);

	no_os_put_unaligned_le16(val, buf + 1);

	return no_os_spi_write_and_read(dev->spi_desc, buf, 3);
}

/**
 * @brief Enable or disable the APG mode
 * @param dev - AD8460 descriptor
 * @param val - enable or disable APG mode
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_enable_apg_mode(struct ad8460_device *dev, int val)
{
	int ret;

	ret = ad8460_reg_update_bits(dev, 0x02, AD8460_APG_MODE_ENABLE_MSK, val);
	if (ret)
		return ret;

	return ad8460_reg_update_bits(dev, 0x00, AD8460_WAVE_GEN_MODE_MSK, val);
}

/**
 * @brief Read the shutdown flag
 * @param dev - AD8460 descriptor
 * @param flag - shutdown flag
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_read_shutdown_flag(struct ad8460_device *dev, uint8_t *flag)
{
	uint8_t val;
	int ret;

	ret = ad8460_reg_read(dev, 0x0E, &val);
	if (ret)
		return ret;

	*flag = !!no_os_field_get(AD8460_SHUTDOWN_FLAG_MSK, val);

	return 0;
}

/**
 * @brief Reset the high voltage driver
 * @param dev - AD8460 descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_hv_reset(struct ad8460_device *dev)
{
	int ret;

	ret = ad8460_reg_update_bits(dev, 0x00, AD8460_HV_RESET_MSK, 1);
	if (ret)
		return ret;

	no_os_udelay(20);

	return ad8460_reg_update_bits(dev, 0x00, AD8460_HV_RESET_MSK, 0);
}

/**
 * @brief Set the sample value
 * @param dev - AD8460 descriptor
 * @param val - sample value
 * @return 0 in case of success, negative error code otherwise
 */
int ad8460_set_sample(struct ad8460_device *dev, uint16_t val)
{
	int ret;

	ret = ad8460_enable_apg_mode(dev, 1);
	if (ret)
		return ret;

	ret = ad8460_set_hvdac_word(dev, 0, val);
	if (ret)
		return ret;

	return ad8460_reg_update_bits(dev, 0x02, AD8460_PATTERN_DEPTH_MSK, 0);
}

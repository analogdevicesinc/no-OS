/***************************************************************************//**
 *   @file   tcs34725.c
 *   @brief  Implementation of TCS34725 RGB color sensor driver.
 *   @author Monica Constandachi (monica.constandachi@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "tcs34725.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int tcs34725_init(struct tcs34725_desc **desc,
		  struct tcs34725_init_param *init_param)
{
	struct tcs34725_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->comm_desc, init_param->comm_param);
	if (ret)
		goto free_desc;

	*desc = descriptor;
	return 0;

free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Read the raw value of a register.
 * @param desc - The device structure.
 * @param addr - The register's address.
 * @param val - The value to be read.
 * @return 0 in case of success, negative error code otherwise
 */
int tcs34725_reg_read(struct tcs34725_desc *desc, uint8_t addr, uint8_t *val)
{
	uint8_t cmd;
	int ret;

	if (!desc || !val)
		return -EINVAL;

	cmd = TCS34725_COMMAND_BIT | addr;

	ret = no_os_i2c_write(desc->comm_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(desc->comm_desc, val, 1, 1);
}

/**
 * @brief Write a raw value to a register.
 * @param desc - The device structure.
 * @param addr - The register's address.
 * @param val - The value to be written.
 * @return 0 in case of success, error code otherwise
 */
int tcs34725_reg_write(struct tcs34725_desc *desc, uint8_t addr, uint8_t val)
{
	uint8_t buf[2];

	if (!desc)
		return -EINVAL;

	buf[0] = TCS34725_COMMAND_BIT | addr;
	buf[1] = val;

	return no_os_i2c_write(desc->comm_desc, buf, 2, 1);
}

/**
 * @brief Read the current RGB color sample.
 * @param desc - The device structure.
 * @param r - The red component.
 * @param g - The green component.
 * @param b - The blue component.
 * @param c - The clear component.
 * @return 0 in case of success, error code otherwise.
 */

int tcs34725_get_rgbc(struct tcs34725_desc *desc, uint16_t *r, uint16_t *g,
		      uint16_t *b, uint16_t *c)
{
	uint8_t buf[8];
	uint8_t cmd = TCS34725_COMMAND_BIT | TCS34725_COMMAND_INC |
		      TCS34725_CDATAL;
	int ret;

	if (!desc || !r || !g || !b || !c)
		return -EINVAL;

	ret = no_os_i2c_write(desc->comm_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc->comm_desc, buf, 8, 1);
	if (ret)
		return ret;

	*c = no_os_get_unaligned_le16(&buf[0]);
	*r = no_os_get_unaligned_le16(&buf[2]);
	*g = no_os_get_unaligned_le16(&buf[4]);
	*b = no_os_get_unaligned_le16(&buf[6]);

	return 0;
}

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise.
 */
int tcs34725_remove(struct tcs34725_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_remove(desc->comm_desc);

	no_os_free(desc);

	return ret;
}

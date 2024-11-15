/***************************************************************************//**
 *   @file   no_os_mdio.c
 *   @brief  Source file for MDIO interface driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <errno.h>
#include "no_os_mdio.h"

/**
 * @brief Initialize the MDIO interface.
 * @param desc - The MDIO descriptor.
 * @param param - MDIO parameters.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_init(struct no_os_mdio_desc **desc,
		    struct no_os_mdio_init_param *param)
{
	int ret;

	if (!param || !param->ops)
		return -EINVAL;

	if (!param->ops->init)
		return -ENOSYS;

	ret = param->ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->id = param->id;
	(*desc)->c45 = param->c45;
	(*desc)->addr = param->addr;
	(*desc)->ops = param->ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_mdio_init().
 * @param desc - The MDIO descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_remove(struct no_os_mdio_desc *desc)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->remove)
		return -ENOSYS;

	return desc->ops->remove(desc);
}

/**
 * @brief Write a register using MDIO.
 * @param desc - The MDIO descriptor.
 * @param reg - Register address.
 * 		For clause 45 reg is constructed using NO_OS_MDIO_C45_ADDR macro.
 * 		It is otherwise the plain clause 22 address (0...31).
 * @param val - Value to write into register.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_write(struct no_os_mdio_desc *desc, uint32_t reg, uint16_t val)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->write)
		return -ENOSYS;

	return desc->ops->write(desc, reg, val);
}

/**
 * @brief Read a register using MDIO.
 * @param desc - The MDIO descriptor.
 * @param reg - Register address.
 * 		For clause 45 reg is constructed using NO_OS_MDIO_C45_ADDR macro.
 * 		It is otherwise the plain clause 22 address (0...31).
 * @param val - Value read from register.
 * @return 0 in case of success, error code otherwise.
 */
int no_os_mdio_read(struct no_os_mdio_desc *desc, uint32_t reg, uint16_t *val)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->read)
		return -ENOSYS;

	return desc->ops->read(desc, reg, val);
}

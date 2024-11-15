/***************************************************************************//**
 *   @file   no_os_eeprom.c
 *   @brief  Implementation of the EEPROM APIs
 *   @author Mahesh Phalke (mahesh.phalke@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <inttypes.h>
#include <stdlib.h>
#include "no_os_eeprom.h"
#include "no_os_error.h"

/**
 * @brief 	Initialize the EEPROM
 * @param	desc - EEPROM descriptor
 * @param	param - EEPROM init parameters
 * @return	0 in case of success, negative error code otherwise
 */
int32_t no_os_eeprom_init(struct no_os_eeprom_desc **desc,
			  const struct no_os_eeprom_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	ret = param->platform_ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_eeprom_init()
 * @param desc - EEPROM descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int32_t no_os_eeprom_remove(struct no_os_eeprom_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief 	Write the EEPROM data
 * @param	desc - EEPROM descriptor
 * @param	address - EEPROM address/location to write
 * @param	data - EEPROM data (pointer)
 * @param	bytes - Number of data bytes to write
 * @return	0 in case of success, negative error code otherwise
 */
int32_t no_os_eeprom_write(struct no_os_eeprom_desc *desc, uint32_t address,
			   uint8_t *data, uint16_t bytes)
{
	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->write)
		return -ENOSYS;

	return desc->platform_ops->write(desc, address, data, bytes);
}

/**
 * @brief 	Read the EEPROM data
 * @param	desc - EEPROM descriptor
 * @param	address - EEPROM address/location to read
 * @param	data - EEPROM data (pointer)
 * @param	bytes - Number of data bytes to read
 * @return	0 in case of success, negative error code otherwise
 */
int32_t no_os_eeprom_read(struct no_os_eeprom_desc *desc, uint32_t address,
			  uint8_t *data, uint16_t bytes)
{
	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->read)
		return -ENOSYS;

	return desc->platform_ops->read(desc, address, data, bytes);
}

/***************************************************************************//**
 *   @file   no_os_eeprom.c
 *   @brief  Implementation of the EEPROM APIs
 *   @author Mahesh Phalke (mahesh.phalke@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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

/***************************************************************************//**
 *   @file   24xx32a.c
 *   @brief  24AA32A/24LC32A EEPROM Interfaces
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "24xx32a.h"
#include "no_os_eeprom.h"
#include "no_os_i2c.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Definitions *****************************/
/******************************************************************************/

/**
 * @brief 	Initialize the 24XX32A EEPROM
 * @param	desc - EEPROM descriptor
 * @param	param - EEPROM init parameters
 * @return	0 in case of success, negative error code otherwise
 */
int32_t eeprom_24xx32a_init(struct no_os_eeprom_desc **desc,
			    const struct no_os_eeprom_init_param *param)
{
	int32_t ret;
	struct no_os_eeprom_desc *eeprom_desc;
	struct eeprom_24xx32a_dev *eeprom_dev;
	struct eeprom_24xx32a_init_param *eeprom_init_param;

	if (!desc || !param)
		return -EINVAL;

	eeprom_desc = (struct no_os_eeprom_desc*)no_os_calloc(1, sizeof(*eeprom_desc));
	if (!eeprom_desc)
		return -ENOMEM;

	eeprom_desc->device_id = param->device_id;

	eeprom_dev = (struct eeprom_24xx32a_dev*)no_os_calloc(1, sizeof(*eeprom_dev));
	if (!eeprom_dev) {
		ret = -ENOMEM;
		goto error_eeprom_dev;
	}

	eeprom_init_param = param->extra;
	ret = no_os_i2c_init(&eeprom_dev->i2c_desc, eeprom_init_param->i2c_init);
	if (ret)
		goto error_eeprom_init;

	eeprom_desc->extra = eeprom_dev;
	*desc = eeprom_desc;

	return 0;

error_eeprom_init:
	no_os_free(eeprom_dev);
error_eeprom_dev:
	no_os_free(eeprom_desc);

	return ret;
}

/**
 * @brief 	Read the 24XX32A EEPROM data
 * @param	desc - EEPROM descriptor
 * @param	address - EEPROM address/location to read
 * @param	data - EEPROM data (pointer)
 * @param	bytes - Number of data bytes to read
 * @return	0 in case of success, negative error code otherwise
 */
int32_t eeprom_24xx32a_read(struct no_os_eeprom_desc *desc, uint32_t address,
			    uint8_t *data, uint16_t bytes)
{
	int32_t ret;
	uint16_t indx;
	uint8_t buff[2];
	uint32_t curr_address = address;
	struct eeprom_24xx32a_dev *eeprom_dev;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	eeprom_dev = desc->extra;

	/* Perform byte by byte read */
	for (indx = 0; indx < bytes; indx++) {
		no_os_put_unaligned_be16(curr_address, buff);

		ret = no_os_i2c_write(eeprom_dev->i2c_desc, buff, sizeof(buff), 1);
		if (ret)
			return ret;

		ret = no_os_i2c_read(eeprom_dev->i2c_desc, &data[indx], 1, 1);
		if (ret)
			return ret;

		curr_address++;
	}

	return 0;
}

/**
 * @brief 	Write the 24XX32A EEPROM data
 * @param	desc - EEPROM descriptor
 * @param	address - EEPROM address/location to write
 * @param	data - EEPROM data (pointer)
 * @param	bytes - Number of data bytes to write
 * @return	0 in case of success, negative error code otherwise
 */
int32_t eeprom_24xx32a_write(struct no_os_eeprom_desc *desc, uint32_t address,
			     uint8_t *data, uint16_t bytes)
{
	int32_t ret;
	uint16_t indx;
	uint8_t buff[34];
	uint8_t page;
	uint8_t pagecount = bytes / 32 + (bytes % 32 ? 1 : 0);
	uint32_t curr_address = address;
	struct eeprom_24xx32a_dev *eeprom_dev;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	eeprom_dev = desc->extra;

	if (address % 32 == 0) {
		/* Perform page write */
		for (page = 0; page < pagecount; page++) {
			no_os_put_unaligned_be16(address + page * 32, buff);
			memcpy(&buff[2], &data[page * 32], 32);

			ret = no_os_i2c_write(eeprom_dev->i2c_desc, buff, sizeof(buff), 1);
			if (ret)
				return ret;

			/* Write cycle time (typ 5msec as per datasheet) */
			no_os_mdelay(5);
		}
	} else {
		/* Perform byte by byte write */
		for (indx = 0; indx < bytes; indx++) {
			no_os_put_unaligned_be16(curr_address, buff);
			buff[2] = data[indx];

			ret = no_os_i2c_write(eeprom_dev->i2c_desc, buff, 3, 1);
			if (ret)
				return ret;

			/* Write cycle time (typ 5msec as per datasheet) */
			no_os_mdelay(5);

			curr_address++;
		}
	}

	return 0;
}

/**
 * @brief Free the resources allocated by eeprom_24xx32a_init()
 * @param desc - EEPROM descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int32_t eeprom_24xx32a_remove(struct no_os_eeprom_desc *desc)
{
	struct eeprom_24xx32a_dev *eeprom_dev;
	int32_t ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	eeprom_dev = desc->extra;

	/* Free the I2C descriptor */
	ret = no_os_i2c_remove(eeprom_dev->i2c_desc);
	if (ret)
		return ret;

	/* Free the EEPROM descriptor objects */
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
* @brief 24XX32A EEPROM specific ops structure
*/
const struct no_os_eeprom_platform_ops eeprom_24xx32a_ops = {
	.init = &eeprom_24xx32a_init,
	.read = &eeprom_24xx32a_read,
	.write = &eeprom_24xx32a_write,
	.remove = &eeprom_24xx32a_remove,
};

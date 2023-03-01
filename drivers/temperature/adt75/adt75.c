/***************************************************************************//**
 *   @file   adt75.c
 *   @brief  Source file of ADT75 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "adt75.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Read the raw value of a register.
 * @param desc - The device structure.
 * @param addr - The register's address.
 * @param val - The value to be read.
 * @return 0 in case of success, negative error code otherwise
 */
int adt75_reg_read(struct adt75_desc *desc, uint32_t addr, uint16_t *val)
{
	uint8_t buff[ADT75_FRAME_SIZE];
	int reg_size;
	int ret;

	reg_size = (addr == ADT75_CONF_REG)? 1 : 2;

	buff[0] = no_os_field_get(ADT75_DATA_REG_MASK, addr);
	ret = no_os_i2c_write(desc->comm_desc, buff, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc->comm_desc, buff, reg_size, 1);
	if (ret)
		return ret;

	if (reg_size == 2)
		*val = no_os_get_unaligned_be16(buff);
	else
		*val = buff[0];

	return 0;
}

/**
 * @brief Write a raw value to a register.
 * @param desc - The device structure.
 * @param addr - The register's address.
 * @param val - The value to be written.
 * @return 0 in case of success, -EINVAL otherwise
 */
int adt75_reg_write(struct adt75_desc *desc, uint32_t addr, uint16_t val)
{
	uint8_t buff[ADT75_FRAME_SIZE];
	int reg_size;

	reg_size = (addr == ADT75_CONF_REG)? 1 : 2;

	buff[0] = no_os_field_get(ADT75_DATA_REG_MASK, addr);
	no_os_put_unaligned_be16(val, &buff[1]);

	return no_os_i2c_write(desc->comm_desc, buff, reg_size + 1, 1);
}

/**
 * @brief Read the current temperature sample (in milliCelsius)
 * @param desc - The device structure.
 * @param val - The temperature value.
 * @return 0 in case of success, -EINVAL otherwise
 */
int adt75_get_single_temp(struct adt75_desc *desc, int32_t *val)
{
	uint16_t reg_val;
	int ret;

	ret = adt75_reg_read(desc, ADT75_TEMP_VALUE_REG, &reg_val);
	if (ret)
		return ret;

	/** Wait for a new conversion to finish */
	no_os_mdelay(ADT75_CONV_DELAY_MS);

	reg_val = no_os_field_get(ADT75_TEMP_MASK, reg_val);
	*val = no_os_sign_extend32(reg_val, ADT75_SIGN_BIT);
	*val *= MILLIDEGREE_PER_DEGREE / ADT75_TEMP_DIV;

	return 0;
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adt75_init(struct adt75_desc **desc, struct adt75_init_param *init_param)
{
	struct adt75_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_i2c_init(&descriptor->comm_desc, &init_param->comm_param);
	if (ret)
		goto free_desc;

	*desc = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int adt75_remove(struct adt75_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_remove(desc->comm_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

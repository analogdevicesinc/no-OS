/***************************************************************************//**
 *   @file   stm32/stm32_i2c.c
 *   @brief  Implementation of stm32 i2c driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"
#include "stm32_i2c.h"

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_i2c_init(struct no_os_i2c_desc **desc,
		       const struct no_os_i2c_init_param *param)
{
	int32_t ret;
	struct no_os_i2c_desc *descriptor;
	struct stm32_i2c_desc *xdesc;
	struct stm32_i2c_init_param *i2cinit;
	I2C_TypeDef *base = NULL;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct no_os_i2c_desc *)no_os_calloc(1,
			sizeof(struct no_os_i2c_desc));
	if (!descriptor)
		return -ENOMEM;

	xdesc = (struct stm32_i2c_desc *)no_os_calloc(1,sizeof(struct stm32_i2c_desc));
	if (!xdesc) {
		ret = -ENOMEM;
		goto error_1;
	}

	i2cinit = param->extra;
	descriptor->extra = xdesc;

	switch (param->device_id) {
#if defined(I2C1)
	case 1:
		base = I2C1;
		break;
#endif
#if defined(I2C2)
	case 2:
		base = I2C2;
		break;
#endif
#if defined(I2C3)
	case 3:
		base = I2C3;
		break;
#endif
	default:
		ret = -EINVAL;
		goto error_2;
	};

	xdesc->hi2c.Instance = base;
#if defined (STM32F4) || defined (STM32F1) || defined (STM32F2) || defined (STM32L1)
	xdesc->hi2c.Init.ClockSpeed = param->max_speed_hz;
	xdesc->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
#else
	xdesc->hi2c.Init.Timing = i2cinit->i2c_timing;
#endif
	xdesc->hi2c.Init.OwnAddress1 = 0;
	xdesc->hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	xdesc->hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	xdesc->hi2c.Init.OwnAddress2 = 0;
	xdesc->hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	xdesc->hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	ret = HAL_I2C_Init(&xdesc->hi2c);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error_2;
	}

	/* copy settings to device descriptor */
	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->slave_address = param->slave_address;
	*desc = descriptor;

	return 0;
error_2:
	no_os_free(xdesc);
error_1:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct stm32_i2c_desc *sdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	sdesc = desc->extra;
	HAL_I2C_DeInit(&sdesc->hi2c);
	no_os_free(desc->extra);
	no_os_free(desc);
	return 0;
}

/**
 * @brief I2C write transaction as master.
 * @param desc - The I2C descriptor.
 * @param data - The buffer with the data to transmit.
 * @param bytes_number - Number of bytes in the buffer.
 * @param stop_bit - Specifis whether to end the transaction with a stop bit.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_i2c_write(struct no_os_i2c_desc *desc,
			uint8_t *data,
			uint8_t bytes_number,
			uint8_t stop_bit)
{
	int ret;
	struct stm32_i2c_desc *xdesc;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	xdesc = desc->extra;

	if (!stop_bit) {
		ret = HAL_I2C_Master_Seq_Transmit_IT(&xdesc->hi2c, desc->slave_address << 1,
						     data,
						     bytes_number, I2C_FIRST_FRAME);
	} else {
		ret = HAL_I2C_Master_Transmit(&xdesc->hi2c, desc->slave_address << 1, data,
					      bytes_number, HAL_MAX_DELAY);
	}

	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief I2C read transaction as master.
 * @param desc - The I2C descriptor.
 * @param data - The buffer where received data is to be stored.
 * @param bytes_number - Number of bytes to receive.
 * @param stop_bit - Specifis whether to end the transaction with a stop bit.
 * @return 0 in case of success, -1 otherwise.
 */

int32_t stm32_i2c_read(struct no_os_i2c_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number,
		       uint8_t stop_bit)
{
	int ret;
	struct stm32_i2c_desc *xdesc;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	xdesc = desc->extra;

	if (!stop_bit) {
		ret = HAL_I2C_Master_Seq_Receive_IT(&xdesc->hi2c, desc->slave_address << 1,
						    data,
						    bytes_number, I2C_LAST_FRAME);
	} else {
		ret = HAL_I2C_Master_Receive(&xdesc->hi2c, desc->slave_address << 1, data,
					     bytes_number, HAL_MAX_DELAY);
	}

	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief stm32 platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops stm32_i2c_ops = {
	.i2c_ops_init = &stm32_i2c_init,
	.i2c_ops_write = &stm32_i2c_write,
	.i2c_ops_read = &stm32_i2c_read,
	.i2c_ops_remove = &stm32_i2c_remove
};

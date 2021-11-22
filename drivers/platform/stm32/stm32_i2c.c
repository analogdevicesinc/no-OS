/***************************************************************************//**
 *   @file   stm32/stm32_i2c.c
 *   @brief  Implementation of stm32 i2c driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "util.h"
#include "i2c.h"
#include "stm32_i2c.h"

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_i2c_init(struct i2c_desc **desc,
		       const struct i2c_init_param *param)
{
	int32_t ret;
	struct i2c_desc	*i2c_desc;

	if (!desc || !param)
		return -EINVAL;

	i2c_desc = (struct i2c_desc *)calloc(1, sizeof(*i2c_desc));
	if (!i2c_desc)
		return -ENOMEM;

	struct stm32_i2c_desc *xdesc;
	struct stm32_i2c_init_param *xinit;

	xdesc = (stm32_i2c_desc*)calloc(1,sizeof(stm32_i2c_desc));
	if (!xdesc) {
		ret = -ENOMEM;
		goto error;
	}

	i2c_desc->extra = xdesc;
	xinit = param->extra;

	xdesc->hi2c.Instance = xinit->base;
	xdesc->hi2c.Init.ClockSpeed = param->max_speed_hz;
	xdesc->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	xdesc->hi2c.Init.OwnAddress1 = 0;
	xdesc->hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	xdesc->hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	xdesc->hi2c.Init.OwnAddress2 = 0;
	xdesc->hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	xdesc->hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	
	ret = HAL_I2C_Init(&xdesc->hi2c);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	/* copy settings to device descriptor */
	i2c_desc->max_speed_hz = param->max_speed_hz;
	i2c_desc->slave_address = param->slave_address;
	*desc = i2c_desc;

	return SUCCESS;
error:
	free(i2c_desc);
	free(xdesc);
	return ret;
}

/**
 * @brief Free the resources allocated by i2c_init().
 * @param desc - The I2C descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_i2c_remove(struct i2c_desc *desc)
{
/*	struct stm32_i2c_desc *sdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	sdesc = desc->extra;
	HAL_I2C_DeInit(&sdesc->hi2c);
	free(desc->extra);
	free(desc);*/
	return 0;
}

/**
 * @brief Write and read data to/from I2C.
 * @param desc - The I2C descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_i2c_write(struct i2c_desc *desc,
			uint8_t *data,
			uint8_t bytes_number,
			uint8_t stop_bit)
{
/*	uint8_t *tx = data;
	uint8_t *rx = data;
	struct stm32_i2c_desc *sdesc;
	struct stm32_gpio_desc *gdesc;
	I2C_TypeDef * I2Cx;
	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	sdesc = desc->extra;
	I2Cx = sdesc->hi2c.Instance;

	return SUCCESS;*/
	return 0;
}

int32_t stm32_i2c_read(struct i2c_desc *desc,
			uint8_t *data,
			uint8_t bytes_number,
			uint8_t stop_bit)
{
	return 0;
}

/**
 * @brief stm32 platform specific I2C platform ops structure
 */
const struct i2c_platform_ops stm32_i2c_ops = {
	.i2c_ops_init = &stm32_i2c_init,
	.i2c_ops_write = &stm32_i2c_write,
	.i2c_ops_read = &stm32_i2c_read,
	.i2c_ops_remove = &stm32_i2c_remove
};

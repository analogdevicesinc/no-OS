/***************************************************************************//**
 *   @file   i2c.c
 *   @brief  Implementation of Xilinx I2C Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include "error.h"
#include "i2c.h"
#include "xilinx_platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param init_param - The structure that contains the I2C parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(struct i2c_desc **desc,
		 const struct i2c_init_param *param)
{
#ifdef XIIC_H
	i2c_desc *dev;
	xil_i2c_desc *xil_dev;
	xil_i2c_init_param *xil_param;
	int32_t ret;

	dev = calloc(1, sizeof *dev);
	if(!dev)
		return FAILURE;

	dev->extra = calloc(1, sizeof *xil_dev);
	if(!(dev->extra)) {
		free(dev);
		return FAILURE;
	}

	xil_dev = dev->extra;
	xil_param = param->extra;

	xil_dev->type = xil_param->type;
	xil_dev->id = xil_param->id;
	dev->max_speed_hz = param->max_speed_hz;
	dev->slave_address = param->slave_address;

	xil_dev->config = XIic_LookupConfig(xil_dev->id);
	if (xil_dev->config == NULL)
		goto error;

	ret = XIic_CfgInitialize(&xil_dev->instance, xil_dev->config,
				 xil_dev->config->BaseAddress);
	if(ret != 0)
		goto error;

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				     XIic_InterruptHandler, &xil_dev->instance);

	ret = XIic_Start(&xil_dev->instance);
	if(ret != 0)
		goto error;

	ret = XIic_SetAddress(&xil_dev->instance, XII_ADDR_TO_SEND_TYPE,
			      dev->slave_address);
	if(ret != 0)
		goto error;

	*desc = dev;

#endif
	return SUCCESS;
#ifdef XIIC_H
error:
	free(dev);

	return FAILURE;
#endif
}

/**
 * @brief Free the resources allocated by i2c_init().
 * @param desc - The I2C descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_remove(struct i2c_desc *desc)
{
#ifdef XIIC_H
	int32_t ret;
	xil_i2c_desc *xil_desc;
	xil_desc = desc->extra;

	ret = XIic_Stop(&xil_desc->instance);
	if(ret != 0)
		return FAILURE;

	free(desc);
#endif
	return SUCCESS;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_write(struct i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit)
{
#ifdef XIIC_H
	xil_i2c_desc *xil_desc;
	xil_desc = desc->extra;
	return XIic_Send(xil_desc->instance.BaseAddress, desc->slave_address, data,
			 bytes_number, stop_bit ? XIIC_STOP : XIIC_REPEATED_START);
#else
	return SUCCESS;
#endif
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_read(struct i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit)
{
#ifdef XIIC_H
	xil_i2c_desc *xil_desc;
	xil_desc = desc->extra;

	return XIic_Recv(xil_desc->instance.BaseAddress, desc->slave_address, data,
			 bytes_number, stop_bit ? XIIC_STOP : XIIC_REPEATED_START);
#else
	return SUCCESS;
#endif
}


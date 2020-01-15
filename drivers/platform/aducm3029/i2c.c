/***************************************************************************//**
 *   @file   aducm3029/i2c.c
 *   @brief  Implementation of I2C interface for the ADuCM302x I2C Driver
 *   @author MChindri (mihail.chindris@analog.com)
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
/************************* Include Files **************************************/
/******************************************************************************/

#include "i2c_extra.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** Used to know if driver already initialized */
static uint32_t	initialized;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Allocates the memory needed for the descriptor
 * @return A address to the allocated memory, NULL if the allocation fails.
 */
static struct i2c_desc *alloc_desc_mem(void)
{
	struct i2c_desc		*desc;
	struct aducm_i2c_desc	*aducm_desc;

	desc = calloc(1, sizeof(struct i2c_desc));
	if (!desc)
		return NULL;

	aducm_desc = calloc(1, sizeof(struct aducm_i2c_desc));
	if (!aducm_desc) {
		free(desc);
		return NULL;
	}
	desc->extra = aducm_desc;

	/* Allocate and align buffer to 32 bits */
	aducm_desc->adi_i2c_buffer = calloc(1, ADI_I2C_MEMORY_SIZE + 3);
	if (!aducm_desc->adi_i2c_buffer) {
		free(aducm_desc);
		free(desc);
		return NULL;
	}
	uint32_t mem = (uint32_t)aducm_desc->adi_i2c_buffer;
	aducm_desc->adi_i2c_buffer = (uint8_t *)((mem+3u) & (~(3u)));
	aducm_desc->adi_i2c_buffer_offset =
		(uint32_t)aducm_desc->adi_i2c_buffer - mem;

	return desc;
}

/**
 * @brief Deallocates the memory needed for the descriptor
 * @param desc - Descriptor to be deallocated
 */
static void free_desc_mem(struct i2c_desc *desc)
{
	struct aducm_i2c_desc *aducm_desc = desc->extra;

	free((void *)((uint32_t)aducm_desc->adi_i2c_buffer -
		      aducm_desc->adi_i2c_buffer_offset));
	free(desc->extra);
	free(desc);
}

/**
 * @brief Initialize the I2C communication peripheral.
 * Supported bitrate are between 100kHz and 400 kHz.
 * @param param - Descriptor of the I2C device used in the call of the driver
 * functions
 * @param param: Descriptor used to configure the I2C device. The extra field it
 * is not used and must be set to NULL
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(struct i2c_desc **desc,
		 const struct i2c_init_param *param)
{
	ADI_I2C_RESULT			i2c_ret;
	struct aducm_i2c_desc		*aducm_desc;

	/* Initial checks.*/
	if (!desc || !param || initialized != 0 ||
	    param->slave_address >= (1 << 7)) /* Only 7 bit address supported */
		return FAILURE;

	initialized = 1;
	*desc = alloc_desc_mem();
	if (!(*desc))
		return FAILURE;
	aducm_desc = (*desc)->extra;

	(*desc)->max_speed_hz = param->max_speed_hz;
	(*desc)->slave_address = param->slave_address;

	/* Driving strength must be enabled for I2C pins */
	if (ADI_GPIO_SUCCESS != adi_gpio_DriveStrengthEnable(
		    ADI_GPIO_PORT0, ADI_GPIO_PIN_4 | ADI_GPIO_PIN_5, true))
		goto failure;

	i2c_ret = adi_i2c_Open(0, aducm_desc->adi_i2c_buffer,
			       ADI_I2C_MEMORY_SIZE, &aducm_desc->i2c_handler);
	if (i2c_ret != ADI_I2C_SUCCESS)
		goto failure;

	i2c_ret = adi_i2c_Reset(aducm_desc->i2c_handler);
	if (i2c_ret != ADI_I2C_SUCCESS)
		goto failure;

	i2c_ret = adi_i2c_SetSlaveAddress(aducm_desc->i2c_handler,
					  param->slave_address);
	if (i2c_ret != ADI_I2C_SUCCESS)
		goto failure;

	i2c_ret = adi_i2c_SetBitRate(aducm_desc->i2c_handler,
				     param->max_speed_hz);
	if (i2c_ret != ADI_I2C_SUCCESS)
		goto failure; //SYS clock not initialized or bad max_speed_hz

	return SUCCESS;
failure:
	free_desc_mem(*desc);
	*desc = NULL;
	return FAILURE;
}

/**
 * @brief Free the resources allocated by \ref i2c_init
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_remove(struct i2c_desc *desc)
{
	struct aducm_i2c_desc *aducm_desc;

	if (desc == NULL || desc->extra == NULL)
		return FAILURE;

	initialized = 0;
	aducm_desc = desc->extra;
	adi_i2c_Close(aducm_desc->i2c_handler);
	free_desc_mem(desc);

	return SUCCESS;
}

/**
 * @brief Write data to a slave device taking in account the specified options
 * If general \ref i2c_general_call is set, message must be formatted by the
 * application
 * 10-bit addressing is not supported
 * @param desc:	Descriptor of the I2C device
 * @param data:	Buffer with the data to be written
 * @param bytes_number:	Number of bytes to be written.
 * @param option: Type of transfer
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_write(struct i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t option)
{
	if (!desc || (option & i2c_10_bit_transfer))
		return FAILURE; //10_bit_address scheme not supported

	struct aducm_i2c_desc *adicup_desc = desc->extra;
	ADI_I2C_TRANSACTION	trans[1];
	uint32_t		errors;

	if (option & i2c_general_call) {
		if (ADI_I2C_SUCCESS != adi_i2c_IssueGeneralCall(
			    adicup_desc->i2c_handler, data, bytes_number,
			    &errors))
			return FAILURE;
		return SUCCESS;
	}

	if (ADI_I2C_SUCCESS != adi_i2c_SetSlaveAddress(adicup_desc->i2c_handler,
			desc->slave_address))
		return FAILURE;

	trans->bRepeatStart = 0;
	trans->pPrologue = 0;
	trans->nPrologueSize = 0;
	trans->pData = data;
	trans->nDataSize = bytes_number;
	trans->bReadNotWrite = 0;

	if (ADI_I2C_SUCCESS != adi_i2c_ReadWrite(adicup_desc->i2c_handler,
			trans, &errors))
		return FAILURE;

	return SUCCESS;
}

/**
 * @brief Read data from a slave device
 * 10-bit addressing is not supported
 * @param desc:	Descriptor of the I2C device
 * @param data:	Buffer with the data to be written
 * @param bytes_number:	Number of bytes to be written.
 * @param option: Type of transfer
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_read(struct i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t option)
{
	/*
	 * 10_bit_address scheme not supported by adi driver. However there is
	 * hardware support for it and it could be implemented.
	 * i2c_general_call only on write mode
	 */
	if (!desc || (option & i2c_10_bit_transfer) ||
	    (option & i2c_general_call))
		return FAILURE;

	struct aducm_i2c_desc	*adicup_desc = desc->extra;
	ADI_I2C_TRANSACTION	trans[1];
	uint32_t		errors;

	if (option & i2c_general_call)
		return FAILURE;

	if (ADI_I2C_SUCCESS != adi_i2c_SetSlaveAddress(adicup_desc->i2c_handler,
			desc->slave_address))
		return FAILURE;

	trans->bRepeatStart = option & i2c_repeated_start;
	trans->pPrologue = 0;
	trans->nPrologueSize = 0;
	trans->pData = data;
	trans->nDataSize = bytes_number;
	trans->bReadNotWrite = 1;

	if (ADI_I2C_SUCCESS != adi_i2c_ReadWrite(adicup_desc->i2c_handler,
			trans, &errors))
		return FAILURE;

	return SUCCESS;
}

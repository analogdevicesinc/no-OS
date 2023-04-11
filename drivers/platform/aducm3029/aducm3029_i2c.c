/***************************************************************************//**
 *   @file   aducm3029/aducm3029_i2c.c
 *   @brief  Implementation of I2C driver for ADuCM302x
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#include "no_os_i2c.h"
#include "aducm3029_i2c.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include <stdlib.h>
#include <drivers/i2c/adi_i2c.h>
#include <drivers/gpio/adi_gpio.h>
#include <string.h>

#define UNINITIALIZED_BITRATE 0
#define UNINITIALIZED_ADDRESS 0xFF

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** Used to know how many instances are created */
static uint32_t nb_created_desc;

/** Handle needed by low level functions */
static ADI_I2C_HANDLE i2c_handler = NULL;

/**
 * Buffer needed by the ADI I2C driver to operate.
 * Buffer needed to be aligned to 32 bits.
 */
static uint8_t adi_i2c_buffer[ADI_I2C_MEMORY_SIZE]
__attribute__((aligned (4)));

/** Save the current state of the bitrate to not change it each time */
static uint32_t last_bitrate = UNINITIALIZED_BITRATE;

/** Save the current slave_address to not change it each time */
static uint8_t last_address = UNINITIALIZED_ADDRESS;

/**
 * @struct i2c_extra
 * @brief I2C platform specific parameters.
 */
struct aducm_i2c_extra {
	/** Pointer to the prologue data. */
	uint8_t *prologue_data;
	/** Prologue size. */
	uint8_t prologue_size;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Configure slave address and bitrate if needed
 * @param desc - Descriptor of the I2C device
 * @return 0 in case of success, -1 otherwise.
 */
static uint32_t set_transmission_configuration(struct no_os_i2c_desc *desc)
{
	ADI_I2C_RESULT i2c_ret;

	if (desc->max_speed_hz != last_bitrate) {
		i2c_ret = adi_i2c_SetBitRate(i2c_handler, desc->max_speed_hz);
		if (i2c_ret != ADI_I2C_SUCCESS)
			return -EINVAL;
		last_bitrate = desc->max_speed_hz;
	}
	if (desc->slave_address != last_address) {
		i2c_ret = adi_i2c_SetSlaveAddress(i2c_handler,
						  desc->slave_address);
		if (i2c_ret != ADI_I2C_SUCCESS)
			return -EINVAL;
		last_address = desc->slave_address;
	}

	return 0;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * Supported bitrates are between 100kHz and 400 kHz.
 * Is slave address is 0, then this instance will be used for general call.
 * 10 bits addressing is not supported.
 * @param desc - Descriptor of the I2C device used in the call of the driver
 * functions.
 * @param param - Parameter used to configure the I2C device. The extra field
 * it is not used and must be set to NULL.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t aducm3029_i2c_init(struct no_os_i2c_desc **desc,
				  const struct no_os_i2c_init_param *param)
{
	struct aducm_i2c_extra *aducm_i2c;

	if (!desc || !param)
		return -EINVAL;

	*desc = no_os_calloc(1, sizeof(**desc));
	if (!(*desc))
		return -ENOMEM;

	if (nb_created_desc == 0) {
		if (ADI_I2C_SUCCESS != adi_i2c_Open(0, adi_i2c_buffer,
						    ADI_I2C_MEMORY_SIZE,
						    &i2c_handler)) {
			no_os_free(*desc);
			*desc = NULL;
			return -EFAULT;
		}
		/* Driving strength must be enabled for I2C pins */
		if (ADI_GPIO_SUCCESS != adi_gpio_DriveStrengthEnable(
			    ADI_GPIO_PORT0, ADI_GPIO_PIN_4 | ADI_GPIO_PIN_5,
			    true)) {
			no_os_free(*desc);
			*desc = NULL;
			adi_i2c_Close(i2c_handler);
			i2c_handler = NULL;
			return -EFAULT;
		}
	}

	aducm_i2c = no_os_calloc(1, sizeof(*aducm_i2c));
	(*desc)->max_speed_hz = param->max_speed_hz;
	(*desc)->slave_address = param->slave_address;
	(*desc)->extra = aducm_i2c;
	nb_created_desc++;

	return 0;
}

/**
 * @brief Free the resources allocated by \ref i2c_init
 * @param desc - Descriptor of the I2C device
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t aducm3029_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct aducm_i2c_extra *aducm_i2c = desc->extra;

	if (!desc)
		return -EINVAL;
	nb_created_desc--;
	if (nb_created_desc == 0) {
		adi_i2c_Close(i2c_handler);
		i2c_handler = NULL;
		last_address = UNINITIALIZED_ADDRESS;
		last_bitrate = UNINITIALIZED_BITRATE;
	}
	if (aducm_i2c->prologue_data)
		no_os_free(aducm_i2c->prologue_data);
	no_os_free(aducm_i2c);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write data to a slave device
 * @param desc - Descriptor of the I2C device
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t aducm3029_i2c_write(struct no_os_i2c_desc *desc,
				   uint8_t *data,
				   uint8_t bytes_number,
				   uint8_t stop_bit)
{
	struct aducm_i2c_extra *aducm_i2c = desc->extra;

	if (!desc)
		return -EINVAL;

	ADI_I2C_TRANSACTION trans[1];
	uint32_t errors;
	int32_t ret;
	uint8_t *temp_ptr = NULL;

	ret = set_transmission_configuration(desc);
	if (ret < 0)
		return ret;

	if (desc->slave_address == 0) { //General call
		if (ADI_I2C_SUCCESS != adi_i2c_IssueGeneralCall(i2c_handler,
				data, bytes_number, &errors))
			return -EIO;
		return 0;
	}

	if (stop_bit == 0) {
		aducm_i2c->prologue_size = bytes_number;
		if (aducm_i2c->prologue_data) {
			temp_ptr = realloc(aducm_i2c->prologue_data, bytes_number);
			if (!temp_ptr) {
				no_os_free(aducm_i2c->prologue_data);
				return -1;
			}
			aducm_i2c->prologue_data = temp_ptr;
		} else {
			aducm_i2c->prologue_data = no_os_malloc(bytes_number);
			if (!aducm_i2c->prologue_data)
				return -1;
		}
		memcpy(aducm_i2c->prologue_data, data, bytes_number);

		return 0;
	}

	trans->bRepeatStart = false;
	trans->pPrologue = 0;
	trans->nPrologueSize = 0;
	trans->pData = data;
	trans->nDataSize = bytes_number;
	trans->bReadNotWrite = 0;
	if (ADI_I2C_SUCCESS != adi_i2c_ReadWrite(i2c_handler, trans, &errors))
		return -EIO;

	return 0;
}

/**
 * @brief Read data from a slave device
 * @param desc - Descriptor of the I2C device
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control. NOTE: not applicable in this case
 *                   Example: 0 - A stop condition will not be generated.
 *                            1 - A stop condition will be generated
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t aducm3029_i2c_read(struct no_os_i2c_desc *desc,
				  uint8_t *data,
				  uint8_t bytes_number,
				  uint8_t stop_bit)
{
	struct aducm_i2c_extra *aducm_i2c = desc->extra;

	if (!desc)
		return -EINVAL;

	ADI_I2C_TRANSACTION trans[1];
	uint32_t errors;
	int32_t ret;

	ret = set_transmission_configuration(desc);
	if (ret < 0)
		return ret;

	if (aducm_i2c->prologue_size != 0) {
		trans->bRepeatStart = true;
		trans->pPrologue = aducm_i2c->prologue_data;
		trans->nPrologueSize = aducm_i2c->prologue_size;
	} else {
		trans->bRepeatStart = false;
		trans->pPrologue = NULL;
		trans->nPrologueSize = 0;
	}

	trans->pData = data;
	trans->nDataSize = bytes_number;
	trans->bReadNotWrite = 1;
	ret = adi_i2c_ReadWrite(i2c_handler, trans, &errors);

	if (aducm_i2c->prologue_size != 0) {
		no_os_free(aducm_i2c->prologue_data);
		aducm_i2c->prologue_data = NULL;
		aducm_i2c->prologue_size = 0;
	}

	return ret;
}

/**
 * @brief ADuCM3029 platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops aducm_i2c_ops = {
	.i2c_ops_init = &aducm3029_i2c_init,
	.i2c_ops_write = &aducm3029_i2c_write,
	.i2c_ops_read = &aducm3029_i2c_read,
	.i2c_ops_remove = &aducm3029_i2c_remove
};


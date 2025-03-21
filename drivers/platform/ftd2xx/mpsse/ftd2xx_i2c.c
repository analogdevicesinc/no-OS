/*******************************************************************************
 *   @file   ftd2xx_i2c.c
 *   @brief  Implementation of I2C driver for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "no_os_error.h"
#include "no_os_i2c.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

#include "ftd2xx_i2c.h"
#include "ftd2xx_platform.h"
#include "libmpsse_i2c.h"

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int ftd2xx_i2c_init(struct no_os_i2c_desc **desc,
		    const struct no_os_i2c_init_param *param)
{
	struct ftd2xx_i2c_desc *extra_desc;
	struct ftd2xx_i2c_init *extra_init;
	struct no_os_i2c_desc *i2c_desc;
	FT_DEVICE_LIST_INFO_NODE node;
	ChannelConfig channelConf;
	DWORD num_channels = 0;
	UCHAR value;
	FT_STATUS status;
	int ret;

	if (!desc || !param || param->device_id > FTD2XX_MAX_DEV_PER_CHIP)
		return -EINVAL;

	i2c_desc = (struct no_os_i2c_desc *)no_os_calloc(1, sizeof(*i2c_desc));
	if (!i2c_desc)
		return -ENOMEM;

	extra_desc = (struct ftd2xx_i2c_desc*)no_os_calloc(1,
			sizeof(struct ftd2xx_i2c_desc));
	if (!extra_desc) {
		ret = -ENOMEM;
		goto error;
	}

	i2c_desc->extra = extra_desc;
	extra_init = param->extra;

	channelConf.ClockRate = (I2C_CLOCKRATE)param->max_speed_hz;
	channelConf.LatencyTimer = 10;
	if (extra_init->channel_config) {
		channelConf.Options = I2C_ENABLE_PIN_STATE_CONFIG;
		channelConf.Pin = extra_init->channel_config_pin;
	}

	Init_libMPSSE();

	status = I2C_GetNumChannels(&num_channels);
	if ((status != FT_OK) || (num_channels == 0)) {
		pr_err("I2C_GetNumChannels() failed (%d, %d)\n",
		       status, num_channels);
		ret = -EIO;
		goto error;
	}

	if (ftHandle[param->device_id] == NULL) {
		status = I2C_OpenChannel(param->device_id, &extra_desc->ftHandle);
		if (status != FT_OK) {
			pr_err("I2C_OpenChannel() failed (%d)\n", status);
			ret = status;
			goto error;
		}
		ftHandle[param->device_id] = extra_desc->ftHandle;
	} else {
		extra_desc->ftHandle = ftHandle[param->device_id];
	}

	status = I2C_InitChannel(extra_desc->ftHandle, &channelConf);
	if (status != FT_OK) {
		pr_err("I2C_InitChannel() failed (%d)\n", status);
		ret = status;
		goto error;
	}

	if (extra_init->extra_pins)
		FT_WriteGPIO(extra_desc->ftHandle, extra_init->extra_pins_dir,
			     extra_init->extra_pins_val);

	i2c_desc->slave_address = param->slave_address;
	i2c_desc->max_speed_hz = param->max_speed_hz;
	i2c_desc->device_id = param->device_id;
	*desc = i2c_desc;

	return 0;

error:
	no_os_free(extra_desc);
	no_os_free(i2c_desc);

	return ret;
}

/**
 * @brief I2C write transaction as master.
 * @param desc - The I2C descriptor.
 * @param data - The buffer with the data to transmit.
 * @param bytes_number - Number of bytes in the buffer.
 * @param stop_bit - Specifis whether to end the transaction with a stop bit.
 * @return 0 in case of success, -1 otherwise.
 */
int ftd2xx_i2c_write(struct no_os_i2c_desc *desc, uint8_t *data,
		     uint8_t bytes_number, uint8_t stop_bit)
{
	struct ftd2xx_i2c_desc *extra_desc = desc->extra;
	DWORD transferred;
	DWORD options = 0;
	FT_STATUS status;

	if (!desc || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	if (stop_bit)
		options = I2C_TRANSFER_OPTIONS_STOP_BIT;

	status = I2C_DeviceWrite(extra_desc->ftHandle,
				 (UCHAR)desc->slave_address,
				 (DWORD)bytes_number, (UCHAR *)data,
				 &transferred, options |
				 I2C_TRANSFER_OPTIONS_START_BIT);
	if (status != FT_OK)
		return (int)status;

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
int ftd2xx_i2c_read(struct no_os_i2c_desc *desc, uint8_t *data,
		    uint8_t bytes_number, uint8_t stop_bit)
{
	struct ftd2xx_i2c_desc *extra_desc = desc->extra;
	DWORD transferred;
	DWORD options = 0;
	FT_STATUS status;

	if (!desc || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	if (stop_bit)
		options = I2C_TRANSFER_OPTIONS_STOP_BIT;

	status = I2C_DeviceRead(extra_desc->ftHandle,
				(UCHAR)desc->slave_address,
				(DWORD)bytes_number, (UCHAR *)data,
				&transferred, options |
				I2C_TRANSFER_OPTIONS_START_BIT);
	if (status != FT_OK)
		return (int)status;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int ftd2xx_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct ftd2xx_i2c_desc *extra_desc = desc->extra;
	if (!desc)
		return -EINVAL;

	I2C_CloseChannel(extra_desc->ftHandle);
	Cleanup_libMPSSE();
	no_os_free(desc);

	return 0;
}

/**
 * @brief ftd2xx platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops ftd2xx_i2c_ops = {
	.i2c_ops_init = &ftd2xx_i2c_init,
	.i2c_ops_write = &ftd2xx_i2c_write,
	.i2c_ops_read = &ftd2xx_i2c_read,
	.i2c_ops_remove = &ftd2xx_i2c_remove
};

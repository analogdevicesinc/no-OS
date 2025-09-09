/***************************************************************************//**
 *   @file   adgs6414d.c
 *   @brief  Implementation of ADGS6414D Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdio.h>
#include <stdlib.h>
#include "adgs6414d.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * SPI write operation.
 * @param dev - The device structure.
 * @param reg_addr - Register address.
 * @param data - Data to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_spi_write(struct adgs6414d_dev *dev, uint8_t reg_addr,
			uint8_t data)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = ADGS6414D_CMD_WRITE | (reg_addr & 0x7F);
	buf[1] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2);
}

/**
 * SPI read operation.
 * @param dev - The device structure.
 * @param reg_addr - Register address.
 * @param data - Pointer to store read data.
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_spi_read(struct adgs6414d_dev *dev, uint8_t reg_addr,
		       uint8_t *data)
{
	uint8_t buf[2];
	int ret;

	if (!dev || !data)
		return -EINVAL;

	buf[0] = ADGS6414D_CMD_READ | (reg_addr & 0x7F);
	buf[1] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	*data = buf[1];

	return 0;
}

/**
 * Perform software reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_soft_reset(struct adgs6414d_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adgs6414d_spi_write(dev, ADGS6414D_REG_SOFT_RESETB,
				  ADGS6414D_SOFT_RESET_VAL1);
	if (ret)
		return ret;

	ret = adgs6414d_spi_write(dev, ADGS6414D_REG_SOFT_RESETB,
				  ADGS6414D_SOFT_RESET_VAL2);
	if (ret)
		return ret;

	dev->switch_state = 0x00;

	return 0;
}

/**
 * Set switch state for a specific channel.
 * @param dev - The device structure.
 * @param channel - Switch channel (0-7).
 * @param state - Switch state (true = closed, false = open).
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_set_switch(struct adgs6414d_dev *dev, uint8_t channel, bool state)
{
	int ret;
	uint8_t new_state;

	if (!dev)
		return -EINVAL;

	if (channel >= ADGS6414D_NUM_SWITCHES)
		return -EINVAL;

	if (state)
		new_state = dev->switch_state | (1 << channel);
	else
		new_state = dev->switch_state & ~(1 << channel);

	ret = adgs6414d_spi_write(dev, ADGS6414D_REG_SW_DATA, new_state);
	if (ret)
		return ret;

	dev->switch_state = new_state;

	return 0;
}

/**
 * Get switch state for a specific channel.
 * @param dev - The device structure.
 * @param channel - Switch channel (0-7).
 * @param state - Pointer to store the switch state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_get_switch(struct adgs6414d_dev *dev, uint8_t channel,
			 bool *state)
{
	if (!dev || !state)
		return -EINVAL;

	if (channel >= ADGS6414D_NUM_SWITCHES)
		return -EINVAL;

	*state = (dev->switch_state & (1 << channel)) ? true : false;

	return 0;
}

/**
 * Set multiple switches at once using bitmask.
 * @param dev - The device structure.
 * @param mask - Bitmask for switch states (bit 0 = SW1, bit 7 = SW8).
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_set_switches(struct adgs6414d_dev *dev, uint8_t mask)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adgs6414d_spi_write(dev, ADGS6414D_REG_SW_DATA, mask);
	if (ret)
		return ret;

	dev->switch_state = mask;

	return 0;
}

/**
 * Get all switch states as bitmask.
 * @param dev - The device structure.
 * @param mask - Pointer to store the switch states bitmask.
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_get_switches(struct adgs6414d_dev *dev, uint8_t *mask)
{
	if (!dev || !mask)
		return -EINVAL;

	*mask = dev->switch_state;

	return 0;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_init(struct adgs6414d_dev **device,
		   struct adgs6414d_init_param *init_param)
{
	struct adgs6414d_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct adgs6414d_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_spi;

	dev->switch_state = init_param->initial_state;

	ret = adgs6414d_set_switches(dev, init_param->initial_state);
	if (ret)
		goto error_config;

	*device = dev;

	return 0;

error_config:
	no_os_spi_remove(dev->spi_desc);
error_spi:
	no_os_free(dev);

	return ret;
}

/**
 * Free the resources allocated by adgs6414d_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adgs6414d_remove(struct adgs6414d_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_spi_remove(dev->spi_desc);
	no_os_free(dev);

	return 0;
}

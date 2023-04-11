/***************************************************************************//**
 *   @file   ltc2358.c
 *   @brief  Implementation of LTC2358 Driver.
 *   @author Kim Seer Paller (kimseer.paller@analog.com)
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

#include <stdlib.h>
#include "ltc2358.h"
#include "errno.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Create 24-bit configuration word for the 8 channels.
 * @param channel - Channel number.
 * @param config_number - Configuration number for the channel.
 * @param config_word - 24-bit config word created.
 */
void ltc2358_create_config_word(uint8_t channel, uint8_t config_number,
				uint32_t *config_word)
{
	*config_word = *config_word | ((uint32_t) \
				       (config_number & LTC2358_CHANNEL_MSK)
				       << (channel * LTC2358_BYTES_PER_CH));
}

/**
 * @brief Transmit 24-bit (3 bytes) of configuration information and reads back
 * 24 bytes of data (3 bytes / 24-bit for each channel).
 * @param dev - Device handler.
 * @param config_word - 3 bytes of configuration data for 8 channels.
 * @param data_array - Data array to read in 24 bytes of data from 8 channels.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ltc2358_write_and_read(struct ltc2358_dev *dev,
				      uint32_t config_word,
				      uint8_t data_array[24])
{
	uint8_t tx_buff[24] = {0};

	struct no_os_spi_msg msgs = {
		.tx_buff = tx_buff,
		.rx_buff = data_array,
		.bytes_number = 24,
		.cs_change = 1,
	};

	no_os_put_unaligned_be24(config_word, tx_buff);

	return no_os_spi_transfer(dev->spi_desc, &msgs, 1);
}

/**
 * @brief Read single channel data.
 * @param dev - Device handler.
 * @param config_word - 24-bit config word created.
 * @param data_array - 24 bytes of data from 8 channels.
 * @param channel - Selected channel (0-7).
 * @param readval - 3 bytes raw data for specific channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ltc2358_channel_data(struct ltc2358_dev *dev, uint32_t config_word,
			     uint8_t data_array[24], uint8_t channel,
			     uint32_t *readval)
{
	int32_t ret;

	ret = ltc2358_write_and_read(dev, config_word, data_array);
	if (ret)
		return ret;

	*readval = no_os_get_unaligned_be24(&data_array[channel *
						    LTC2358_BYTES_PER_CH]);

	return 0;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ltc2358_init(struct ltc2358_dev **device,
		     struct ltc2358_init_param *init_param)
{
	struct ltc2358_dev *dev;
	int32_t ret;

	dev = (struct ltc2358_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error;

	*device = dev;

	return ret;
error:
	no_os_free(dev);

	return 0;
}

/**
 * @brief Free memory allocated by ltc2358_init().
 * @param dev - Device handler.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ltc2358_remove(struct ltc2358_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/***************************************************************************//**
 *   @file   demo_dev.c
 *   @brief  Implementation of iio_demo.c.
 *   @author Cristian Pop (cristian.pop@analog.com)
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "demo_dev.h"
#include "error.h"
#include "util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

int32_t iio_demo_reg_write(struct iio_demo_desc *desc, uint32_t reg,
			   uint32_t writeval)
{
	if (!desc || reg > MAX_REG_ADDR)
		return -EINVAL;

	desc->dummy_regs[reg] = writeval;

	return SUCCESS;
}

int32_t iio_demo_reg_read(struct iio_demo_desc *desc, uint32_t reg,
			  uint32_t *readval)
{
	if (!desc || reg > MAX_REG_ADDR || !readval)
		return -EINVAL;

	*readval = desc->dummy_regs[reg];

	return SUCCESS;
}

/**
 * @brief get_demo_channel_attr().
 * @param device- Physical instance of a iio_demo_device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
ssize_t get_demo_channel_attr(void *device, char *buf, size_t len,
			      const struct iio_ch_info *channel)
{
	struct iio_demo_desc *desc = device;

	return snprintf(buf, len, "%"PRIu32"", desc->dev_ch_attr);
}

/**
 * @brief set_demo_channel_attr().
 * @param device - Physical instance of a iio_demo_device.
 * @param buf - Value to be written to attribute.
 * @param len -	Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
ssize_t set_demo_channel_attr(void *device, char *buf, size_t len,
			      const struct iio_ch_info *channel)
{
	struct iio_demo_desc *desc = device;
	desc->dev_ch_attr = srt_to_uint32(buf);

	return len;
}

/**
 * @brief get_demo_global_attr().
 * @param device- Physical instance of a iio_demo_device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
ssize_t get_demo_global_attr(void *device, char *buf, size_t len,
			     const struct iio_ch_info *channel)
{
	struct iio_demo_desc *desc = device;
	return snprintf(buf, len, "%"PRIu32"", desc->dev_global_attr);
}

/**
 * @brief set_demo_global_attr().
 * @param device - Physical instance of a iio_demo_device.
 * @param buf - Value to be written to attribute.
 * @param len -	Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
ssize_t set_demo_global_attr(void *device, char *buf, size_t len,
			     const struct iio_ch_info *channel)
{
	struct iio_demo_desc *desc = device;
	desc->dev_global_attr = srt_to_uint32(buf);

	return len;
}

int32_t iio_demo_update_active_channels(void *dev, uint32_t mask)
{
	struct iio_demo_desc *desc = dev;

	desc->ch_mask = mask;
	/* If a real device. Here needs to be selected the channels to be read*/

	return SUCCESS;
}

int32_t iio_demo_close_channels(void *dev)
{
	struct iio_demo_desc *desc = dev;

	desc->ch_mask = 0;
	/* If a real device.
	 * If needed, previously selected channels can be closed */

	return SUCCESS;
}

static bool get_next_ch_idx(uint32_t ch_mask, uint32_t last_idx,
			    uint32_t *new_idx)
{
	last_idx++;
	ch_mask >>= last_idx;
	if (!ch_mask) {
		*new_idx = -1;
		return 0;
	}
	while (!(ch_mask & 1)) {
		last_idx++;
		ch_mask >>= 1;
	}
	*new_idx = last_idx;

	return 1;
}

int32_t	iio_demo_read_local_samples(void *dev, uint16_t *buff,
				    uint32_t nb_samples)
{
	struct iio_demo_desc	*desc = dev;
	uint32_t		ch_idx;
	uint32_t		i;
	uint32_t		k;

	ch_idx = -1;
	k = 0;
	for (i = 0; i < nb_samples; i++)
		while (get_next_ch_idx(desc->ch_mask, ch_idx, &ch_idx))
			buff[k++] = local_ch[ch_idx][i % NB_LOCAL_SAMPLES];

	return nb_samples;
}

int32_t	iio_demo_write_local_samples(void *dev, uint16_t *buff,
				     uint32_t nb_samples)
{
	struct iio_demo_desc	*desc = dev;
	uint32_t		ch_idx;
	uint32_t		i;
	uint32_t		k;

	ch_idx = -1;
	k = 0;
	for (i = 0; i < nb_samples; i++)
		while (get_next_ch_idx(desc->ch_mask, ch_idx, &ch_idx))
			local_ch[ch_idx][i % NB_LOCAL_SAMPLES] = buff[k++];

	return nb_samples;
}

/**
 * @brief iio demo init function, registers a demo .
 * @param desc - Descriptor.
 * @param init - Configuration structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_demo_dev_init(struct iio_demo_desc **desc,
			  struct iio_demo_init_param *init)
{
	struct iio_demo_desc *ldesc;

	ldesc = (struct iio_demo_desc*)calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	ldesc->dev_global_attr = init->dev_global_attr;
	ldesc->dev_ch_attr = init->dev_ch_attr;
	ldesc->ch_mask = 0;

	*desc = ldesc;

	return SUCCESS;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_demo_dev_remove(struct iio_demo_desc *desc)
{
	if (!desc)
		return FAILURE;

	free(desc);

	return SUCCESS;
}

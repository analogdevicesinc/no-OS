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

/* Read a device register. The register address to read is set on
 * in desc->active_reg_addr in the function set_demo_reg_attr
 */
ssize_t get_demo_reg_attr(void *device, char *buf, size_t len,
			  const struct iio_ch_info *channel)
{
	struct iio_demo_desc	*desc = device;
	uint32_t		value;

	value = desc->dummy_regs[desc->active_reg_addr];

	return snprintf(buf, len, "%"PRIu32"", value);
}

/* Flow of reading and writing registers. This is how iio works for
 * direct_reg_access attribute:
 * Read register:
 * 	   //Reg_addr in decimal
 * 	   reg_addr = "10";
 * 	1. set_demo_reg_attr(dev, reg_addr, len, channel);
 * 	2. get_demo_reg_attr(dev, out_buf, out_len, channel);
 * Write register:
 * 	   sprintf(write_buf, "0x%x 0x%x", reg_addr, value);
 * 	1. set_demo_reg_attr(dev, write_buf, len, channel);
 */
ssize_t set_demo_reg_attr(void *device, char *buf, size_t len,
			  const struct iio_ch_info *channel)
{
	struct iio_demo_desc	*desc = device;
	uint32_t		nb_filled;
	uint32_t		addr;
	uint32_t		value;

	nb_filled = sscanf(buf, "0x"PRIx32" 0x"PRIx32"", &addr, &value);
	if (nb_filled == 2) {
		/* Write register */
		if (addr >= MAX_REG_ADDR)
			return -EINVAL;
		desc->dummy_regs[addr] = value;
	} else {
		nb_filled = sscanf(buf, "%"PRIu32, &addr);
		if (nb_filled == 1) {
			if (addr >= MAX_REG_ADDR)
				return -EINVAL;
			desc->active_reg_addr = addr;
			return len;
		} else {
			return -EINVAL;
		}
	}

	return len;
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

/**
 * @brief Transfer data from RAM to device.
 * @param iio_inst - Physical instance of a iio_demo_dac device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return Number of bytes transfered, or negative value in case of failure.
 */
ssize_t iio_demo_transfer_mem_to_dev(void *iio_inst,
				     size_t bytes_count,
				     uint32_t ch_mask)
{
	struct iio_demo_device *demo_device;
	demo_device = (struct iio_demo_device *)iio_inst;
	if (!demo_device)
		return FAILURE;

	return bytes_count;
}

/**
 * @brief Transfer data from device into RAM.
 * @param iio_inst - Physical instance of a device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
ssize_t iio_demo_transfer_dev_to_mem(void *iio_inst,
				     size_t bytes_count,
				     uint32_t ch_mask)
{
	struct iio_demo_device *demo_device;
	demo_device = (struct iio_demo_device *)iio_inst;
	if (!demo_device)
		return FAILURE;

	/* In some cases a transfer is necessary before a "iio_demo_read_dev"
	 * function is called. In this case an implementation should be provided
	 * in this location. For an example check "iio_axi_adc" module, where
	 * data is transfered from device to RAM memory by DMA.
	 */
	return bytes_count;
}

/**
 * @brief Write chunk of data into RAM.
 * This function is probably called multiple times by libtinyiiod before a
 * "iio_transfer_mem_to_dev" call, since we can only write "bytes_count" bytes
 * at a time.
 * @param iio_inst - Physical instance of a iio_demo_dac device.
 * @param buf - Values to write.
 * @param offset - Offset in memory after the nth chunk of data.
 * @param bytes_count - Number of bytes to write.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
ssize_t iio_demo_write_dev(void *iio_inst, char *buf,
			   size_t offset,  size_t bytes_count, uint32_t ch_mask)
{
	struct iio_demo_desc *demo_device;
	uint32_t index, addr;
	uint16_t *buf16;

	if (!iio_inst)
		return FAILURE;

	if (!buf)
		return FAILURE;

	buf16 = (uint16_t *)buf;
	demo_device = (struct iio_demo_desc *)iio_inst;
	addr = demo_device->ddr_base_addr + offset;
	for(index = 0; index < bytes_count; index += 2) {
		uint32_t *local_addr = (uint32_t *)(addr +
						    (index * 2) % demo_device->ddr_base_size);
		*local_addr = (buf16[index + 1] << 16) | buf16[index];
	}


	return bytes_count;
}

/**
 * @brief Read chunk of data from RAM to pbuf.
 * Call "iio_demo_transfer_dev_to_mem" first.
 * This function is probably called multiple times by libtinyiiod after a
 * "iio_demo_transfer_dev_to_mem" call, since we can only read "bytes_count"
 * bytes at a time.
 * @param iio_inst - Physical instance of a device.
 * @param pbuf - Buffer where value is stored.
 * @param offset - Offset to the remaining data after reading n chunks.
 * @param bytes_count - Number of bytes to read.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
ssize_t iio_demo_read_dev(void *iio_inst, char *pbuf, size_t offset,
			  size_t bytes_count, uint32_t ch_mask)
{
	struct iio_demo_desc *demo_device;
	uint32_t i, j = 0, current_ch = 0;
	uint16_t *pbuf16;
	size_t samples;

	if (!iio_inst)
		return FAILURE;

	if (!pbuf)
		return FAILURE;

	demo_device = (struct iio_demo_desc*)iio_inst;
	pbuf16 = (uint16_t*)pbuf;
	samples = (bytes_count * DEMO_NUM_CHANNELS) / hweight8(
			  ch_mask);
	samples /= 2; /* because of uint16_t *pbuf16 = (uint16_t*)pbuf; */
	offset = (offset * DEMO_NUM_CHANNELS) / hweight8(ch_mask);

	for (i = 0; i < samples; i++) {

		if (ch_mask & BIT(current_ch)) {
			pbuf16[j] = *(uint16_t*)(demo_device->ddr_base_addr +
						 (offset + i * 2) % demo_device->ddr_base_size);

			j++;
		}

		if (current_ch + 1 < DEMO_NUM_CHANNELS)
			current_ch++;
		else
			current_ch = 0;
	}

	return bytes_count;
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
	ldesc->ddr_base_addr = init->ddr_base_addr;
	ldesc->ddr_base_size = init->ddr_base_size;

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

/***************************************************************************//**
 *   @file   iio_types.h
 *   @brief  Header file for iio_types
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef IIO_TYPES_H_
#define IIO_TYPES_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct iio_ch_info {
	int16_t ch_num;
	bool ch_out;
};

struct iio_attribute {
	const char *name;
	ssize_t (*show)(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
	ssize_t (*store)(void *device, char *buf, size_t len,
			 const struct iio_ch_info *channel);
};

struct iio_channel {
	char *name;
	struct iio_attribute **attributes;
	bool ch_out;
};

struct iio_device {
	const char *name;
	uint16_t num_ch;
	struct iio_channel **channels;
	struct iio_attribute **attributes;
};

/**
 * struct iio_interface_init_par - iio interface init par.
 * @dev_name: device name.
 * @num_ch: number of channels.
 * @dev_instance: physical instance of a device.
 * @iio_device: device descriptor(describes channels and attributes)
 * @get_xml: Generate device xml.
 * @transfer_dev_to_mem: transfer data from ADC into RAM.
 * @read_data: Read data from RAM to pbuf. It should be called after "transfer_dev_to_mem".
 * @transfer_mem_to_dev: Transfer data from RAM to DAC.
 * @write_data: Write data to RAM. It should be called before "transfer_mem_to_dev".
 */
struct iio_interface_init_par {
	const char *dev_name;
	void *dev_instance;
	struct iio_device *iio_device;
	ssize_t (*get_xml)(char** xml, struct iio_device *iio_dev);
	ssize_t (*transfer_dev_to_mem)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	ssize_t (*read_data)(void *dev_instance, char *pbuf, size_t offset,
			     size_t bytes_count, uint32_t ch_mask);
	ssize_t (*transfer_mem_to_dev)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	ssize_t (*write_data)(void *dev_instance, char *pbuf, size_t offset,
			      size_t bytes_count, uint32_t ch_mask);
};

struct iio_server_ops {
	/* Read from from a peripheral device (UART, USB, NETWORK) */
	ssize_t (*read)(char *buf, size_t len);
	/* Write to a peripheral device (UART, USB, NETWORK) */
	ssize_t (*write)(const char *buf, size_t len);
};

#endif /* IIO_TYPES_H_ */

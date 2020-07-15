/***************************************************************************//**
 *   @file   iio.h
 *   @brief  Header file of iio
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

#ifndef IIO_H_
#define IIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_types.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct iio_interface
 * @brief Links a physical device instance "void *dev_instance"
 * with a "iio_device *iio" that describes capabilities of the device.
 */
struct iio_interface {
	/** Device name */
	const char *name;
	/** Opened channels */
	uint32_t ch_mask;
	/** Physical instance of a device */
	void *dev_instance;
	/** Device descriptor(describes channels and attributes) */
	struct iio_device *iio;
	/** Transfer data from device into RAM */
	ssize_t (*transfer_dev_to_mem)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	/** Read data from RAM to pbuf. It should be called after "transfer_dev_to_mem" */
	ssize_t (*read_data)(void *dev_instance, char *pbuf, size_t offset,
			     size_t bytes_count, uint32_t ch_mask);
	/** Transfer data from RAM to device */
	ssize_t (*transfer_mem_to_dev)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	/** Write data to RAM. It should be called before "transfer_mem_to_dev" */
	ssize_t (*write_data)(void *dev_instance, char *pbuf, size_t offset,
			      size_t bytes_count, uint32_t ch_mask);
};

enum pysical_link_type {
	USE_UART,
	USE_NETWORK
};

struct iio_desc;

struct iio_init_param {
	enum pysical_link_type	phy_type;
	/*
	 * struct uart_init_parma *init_param or
	 * when implemented: struct tcp_socket_init_param *tcp_socket_init_param
	 */
	void			*phy_init_param;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Set communication ops and read/write ops that will be called from
 * "libtinyiiod". */
ssize_t iio_init(struct iio_desc **desc, struct iio_init_param *init_param);
/* Free the resources allocated by iio_init(). */
ssize_t iio_remove(struct iio_desc *desc);
/* Execut an iio step. */
ssize_t iio_step(struct iio_desc *desc);
/* Register interface. */
ssize_t iio_register(struct iio_desc *desc,
		     struct iio_interface *iio_interface);
/* Unregister interface. */
ssize_t iio_unregister(struct iio_desc *desc,
		       struct iio_interface *iio_interface);

#endif /* IIO_H_ */

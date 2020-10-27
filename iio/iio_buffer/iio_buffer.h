/***************************************************************************//**
*   @file   iio_buffer.h
*   @brief  Header file of iio_buffer
*   @author Cristian Pop (cristian.pop@analog.com)
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
#ifndef IIO_BUFFER_H_
#define IIO_BUFFER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "iio.h"
#include "iio_types.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct iio_buffer_init_param
 * @brief Configuration structure.
 */
struct iio_buffer_init_param {
	/** number of channels */
	uint8_t	num_channels;
	/** scan type */
	struct scan_type *scan_type;
	/** memory base address, where acquisition is made */
	uint32_t ddr_base;
	/** converter device instance */
	void *dev_instance;
	/** converter read data function pointer */
	int32_t (*read_data)(void *dev,
			     uint32_t *buf,
			     uint16_t samples);
};

/**
 * @struct iio_buffer_desc
 * @brief Structure holding iio descriptor.
 */
struct iio_buffer_desc {
	/** iio device descriptor */
	struct iio_device dev_descriptor;
	/** memory base address, where acquisition is made */
	uint32_t ddr_base;
	/** converter device instance */
	void *dev_instance;
	/** converter read data function pointer */
	int32_t (*read_data)(void *dev,
			     uint32_t *buf,
			     uint16_t samples);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init buffer iio. */
int32_t iio_buffer_init(struct iio_buffer_desc **desc,
			struct iio_buffer_init_param *init);
/* Get desciptor. */
void iio_buffer_get_dev_descriptor(struct iio_buffer_desc *desc,
				   struct iio_device **dev_descriptor);
/* Free the resources allocated by iio_buffer_init(). */
int32_t iio_buffer_remove(struct iio_buffer_desc *desc);

#endif /* IIO_BUFFER_H_ */

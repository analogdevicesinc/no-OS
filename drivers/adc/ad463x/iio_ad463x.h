/***************************************************************************//**
*   @file   iio_ad463x.h
*   @brief  Header file of iio_ad463x
*   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
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
#ifndef IIO_AD463x
#define IIO_AD463x

#ifdef IIO_SUPPORT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "iio_types.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct iio_ad463x {
	/* Mask of active ch */
	uint32_t mask;
	/** iio device descriptor */
	struct iio_device iio_dev_desc;
	/** Device Descriptor */
	struct ad463x_dev *ad463x_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init function. */
int32_t iio_ad463x_init(struct iio_ad463x **desc,
			struct ad463x_dev *dev);

/* Free the resources allocated by iio_ad463x_init(). */
int32_t iio_ad463x_remove(struct iio_ad463x *desc);

#endif /* IIO_SUPPORT */

#endif /* IIO_AD463x */

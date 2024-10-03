/***************************************************************************//**
*   @file   iio_ad9361.h
*   @brief  Header file of iio_ad9361
*   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
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
#ifndef IIO_AD9361_H_
#define IIO_AD9361_H_

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
 * @struct iio_ad9361_init_param
 * @brief Configuration structure.
 */
struct iio_ad9361_init_param {
	/** ad9361 device instance pointer */
	struct ad9361_rf_phy *ad9361_phy;
};

/**
 * @struct iio_ad9361_desc
 * @brief Structure holding iio descriptor.
 */
struct iio_ad9361_desc {
	/** iio device descriptor */
	struct iio_device dev_descriptor;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init ad9361 iio. */
int32_t iio_ad9361_init(struct iio_ad9361_desc **desc,
			struct iio_ad9361_init_param *init);
/* Get desciptor. */
void iio_ad9361_get_dev_descriptor(struct iio_ad9361_desc *desc,
				   struct iio_device **dev_descriptor);
/* Free the resources allocated by iio_ad9361_init(). */
int32_t iio_ad9361_remove(struct iio_ad9361_desc *desc);

#endif /* IIO_AD9361_H_ */

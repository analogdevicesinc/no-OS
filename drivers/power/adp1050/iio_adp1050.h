/***************************************************************************//**
 *   @file   iio_adp1050.h
 *   @brief  Header file for the ADP1050 IIO Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef IIO_ADP1050_H
#define IIO_ADP1050_H

#include <stdbool.h>
#include "iio.h"
#include "adp1050.h"

/**
 * @brief Structure holding the ADP1050 IIO device descriptor
*/
struct adp1050_iio_desc {
	struct adp1050_desc *adp1050_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the ADP1050 IIO initalization parameter.
*/
struct adp1050_iio_desc_init_param {
	struct adp1050_init_param *adp1050_init_param;
	uint16_t vout_scale_monitor;
	uint16_t vin_scale_monitor;
	uint16_t iin_scale_monitor;
};

/** Initializes the ADP1050 IIO descriptor. */
int adp1050_iio_init(struct adp1050_iio_desc **,
		     struct adp1050_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int adp1050_iio_remove(struct adp1050_iio_desc *);

#endif /* IIO_ADP1050_H */

/***************************************************************************//**
 *   @file   iio_max2201x.h
 *   @brief  Header file of IIO MAX2201X Driver.
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
#ifndef IIO_MAX2201X_H
#define IIO_MAX2201X_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "max2201x.h"

enum max2201x_iio_slew_rate {
	MAX2201X_IIO_SLEW_RATE,
	MAX2201X_IIO_RANGE,
	MAX2201X_IIO_STEP_SIZE,
	MAX2201X_IIO_UPDATE_RATE,
};

enum max2201x_iio_ao_config {
	MAX2201X_IIO_POLARITY,
	MAX2201X_IIO_CM_SENSE,
	MAX2201X_IIO_LDAC,
};

enum max2201x_iio_config {
	MAX2201X_IIO_DAC_REF,
	MAX2201X_IIO_LD_CTRL,
	MAX2201X_IIO_CURR_LIM,
	MAX2201X_IIO_OVC_SHTDN = 4,
	MAX2201X_IIO_TH_SHTDN = 6,
	MAX2201X_IIO_OW_DETECT = 14,
};

enum max2201x_iio_timeout {
	MAX2201X_IIO_TIMEOUT,
	MAX2201X_IIO_TIMEOUT_EN = 8,
	MAX2201X_IIO_TIMEOUT_CFG = 9,
};

enum max2201x_iio_available {
	MAX2201X_IIO_ENABLE_AVAILABLE,
	MAX2201X_IIO_RANGE_AVAILABLE,
	MAX2201X_IIO_STEP_SIZE_AVAILABLE,
	MAX2201X_IIO_UPDATE_RATE_AVAILABLE,
	MAX2201X_IIO_OP_MODE_AVAILABLE,
	MAX2201X_IIO_POLARITY_AVAILABLE,
	MAX2201X_IIO_CM_SENSE_AVAILABLE,
	MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE,
	MAX2201X_IIO_TIMEOUT_AVAILABLE,
};

/**
 * @brief Structure holding the MAX2201X IIO descriptor
*/
struct max2201x_iio_desc {
	struct max2201x_desc *max2201x_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the MAX2201X IIO initialization parameter.
*/
struct max2201x_iio_desc_init_param {
	struct max2201x_init_param *max2201x_init_param;
};

/** IIO descriptor initialize function. */
int max2201x_iio_init(struct max2201x_iio_desc **,
		      struct max2201x_iio_desc_init_param *);

/** Free resources allocated by the iio_init() function. */
int max2201x_iio_remove(struct max2201x_iio_desc *);

#endif /* IIO_MAX2201X_H */

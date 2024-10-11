/***************************************************************************//**
 *   @file   iio_max22017.h
 *   @brief  Header file of IIO MAX22017 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef IIO_MAX22017_H
#define IIO_MAX22017_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "max22017.h"

enum max22017_iio_slew_rate {
	MAX22017_IIO_SLEW_RATE,
	MAX22017_IIO_RANGE,
	MAX22017_IIO_STEP_SIZE,
	MAX22017_IIO_UPDATE_RATE,
};

enum max22017_iio_ao_config {
	MAX22017_IIO_POLARITY,
	MAX22017_IIO_CM_SENSE,
	MAX22017_IIO_LDAC,
};

enum max22017_iio_config {
	MAX22017_IIO_DAC_REF,
	MAX22017_IIO_LD_CTRL,
	MAX22017_IIO_CURR_LIM,
	MAX22017_IIO_OVC_SHTDN = 4,
	MAX22017_IIO_TH_SHTDN = 6,
	MAX22017_IIO_OW_DETECT = 14,
};

enum max22017_iio_timeout {
	MAX22017_IIO_TIMEOUT,
	MAX22017_IIO_TIMEOUT_EN = 8,
	MAX22017_IIO_TIMEOUT_CFG = 9,
};

enum max22017_iio_available {
	MAX22017_IIO_ENABLE_AVAILABLE,
	MAX22017_IIO_RANGE_AVAILABLE,
	MAX22017_IIO_STEP_SIZE_AVAILABLE,
	MAX22017_IIO_UPDATE_RATE_AVAILABLE,
	MAX22017_IIO_OP_MODE_AVAILABLE,
	MAX22017_IIO_POLARITY_AVAILABLE,
	MAX22017_IIO_CM_SENSE_AVAILABLE,
	MAX22017_IIO_CONFIG_ENABLE_AVAILABLE,
	MAX22017_IIO_TIMEOUT_AVAILABLE,
};

/**
 * @brief Structure holding the MAX22017 IIO descriptor
*/
struct max22017_iio_desc {
	struct max22017_desc *max22017_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the MAX22017 IIO initialization parameter.
*/
struct max22017_iio_desc_init_param {
	struct max22017_init_param *max22017_init_param;
};

/** IIO descriptor initialize function. */
int max22017_iio_init(struct max22017_iio_desc **,
		      struct max22017_iio_desc_init_param *);

/** Free resources allocated by the iio_init() function. */
int max22017_iio_remove(struct max22017_iio_desc *);

#endif /* IIO_MAX22017_H */

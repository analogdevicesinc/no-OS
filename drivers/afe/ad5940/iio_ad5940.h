/***************************************************************************//**
 *   @file   iio_ad5940.h
 *   @brief  Header file of ad5940 iio driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef IIO_AD5940_H
#define IIO_AD5940_H

#include "iio.h"
#include "ad5940.h"

enum ad5940_iio_attr {
	AD5940_IIO_EXCITATION_FREQUENCY,
	AD5940_IIO_EXCITATION_AMPLITUDE,
	AD5940_IIO_IMPEDANCE_MODE,
	AD5940_IIO_MAGNITUDE_MODE,
	AD5940_IIO_GPIO1_TOGGLE,
};

struct ad5940_iio_dev {
	struct ad5940_dev *ad5940;
	struct iio_device *iio;
	bool magnitude_mode;
	bool gpio1;
	uint32_t AppBuff[512];
};

struct ad5940_iio_init_param {
	struct ad5940_init_param *ad5940_init;
};

int32_t ad5940_iio_init(struct ad5940_iio_dev **iio_dev,
			struct ad5940_iio_init_param *init_param);
int32_t ad5940_iio_remove(struct ad5940_iio_dev *desc);

#endif /** IIO_AD5940_H */


/***************************************************************************//**
 *   @file   iio_lm75.h
 *   @brief  IIO Header file of LM75 Driver.
 *   @author Niel Acuna (niel.acuna@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#ifndef __IIO_LM75_H__
#define __IIO_LM75_H__
#include "lm75.h"
#include "iio_types.h"

#define LM75_UPDATE_INTERVAL 	100

#define LM75_CH_ATTRIBUTE_INPUT 	0
#define LM75_CH_ATTRIBUTE_MAX 		1
#define LM75_CH_ATTRIBUTE_HYST 		2

struct iio_lm75 {
	struct lm75_dev *lm75;
	struct iio_device iio_dev;
};

int iio_lm75_init(struct iio_lm75 **iio_lm75pp,
		  struct lm75_dev *dev);
int iio_lm75_remove(struct iio_lm75 *iio_lm75);

#endif /* __IIO_LM75_H__ */

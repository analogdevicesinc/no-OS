/***************************************************************************//**
*   @file   iio_adpd188.h
*   @brief  Header of the ADPD188 IIO driver.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
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

#ifndef IIO_ADPD188_H
#define IIO_ADPD188_H

#include "adpd188.h"
#include "iio.h"

#define ADPD1080_CHANNEL_NO		8
#define ADPD1080_WORD_BIT_SIZE		16
#define ADPD1080_BITS_PER_SAMPLE	32
#define ADPD1080_WORDS_PER_SAMPLE	\
	(ADPD1080_BITS_PER_SAMPLE / ADPD1080_WORD_BIT_SIZE)

struct adpd188_iio_init_param {
	struct adpd188_init_param drv_init_param;
};

struct adpd188_iio_desc {
	struct adpd188_dev *drv_dev;
	uint8_t ch_mask;
};

extern struct iio_device iio_adpd188_device;

int32_t adpd188_iio_init(struct adpd188_iio_desc **device,
			 struct adpd188_iio_init_param *init_param);
int32_t adpd188_iio_remove(struct adpd188_iio_desc *dev);

#endif /* IIO_ADPD188_H */


/***************************************************************************//**
 *   @file   iio_ad5791.h
 *   @brief  Header of AD5791 IIO Driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
 *
*******************************************************************************/
#ifndef IIO_AD5791_H_
#define IIO_AD5791_H_

#include "iio.h"
#include "no_os_util.h"
#include "ad5791.h"

#define AD5791_CH_NO 1

/**
 * @enum ad5791_iio_powerdown_modes
 * @brief AD5791 channel power down modes.
 */
enum ad5791_iio_powerdown_modes {
	AD5791_6kOHMS_TO_GND,
	AD5791_THREE_STATE
};

/**
 * @struct ad5791_iio_desc
 * @brief D5791 IIO driver handler.
 */
struct ad5791_iio_desc {
	struct ad5791_dev *ad5791_handle;
	struct iio_device *ad5791_iio_dev;
	enum ad5791_iio_powerdown_modes curr_mode;
	uint32_t vref_mv;
	uint32_t vref_neg_mv;
};

/**
 * @struct ad5791_iio_init_param
 * @brief AD5791 IIO driver initialization structure.
 */
struct ad5791_iio_init_param {
	struct ad5791_init_param *ad5791_initial;
	uint32_t vref_mv;
	uint32_t vref_neg_mv;
};

/* Initialize the AD5791 IIO driver. */
int32_t ad5791_iio_init(struct ad5791_iio_desc **iio_dev,
			struct ad5791_iio_init_param *init_param);

/* Free memory allocated by ad5791_iio_init(). */
int32_t ad5791_iio_remove(struct ad5791_iio_desc *desc);

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
extern struct iio_device const iio_ad5791_device;

#endif /* IIO_AD5791_H_ */

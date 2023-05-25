/***************************************************************************//**
 *   @file   iio_ad7091r8.h
 *   @brief  Header file of IIO AD7091R8 driver header file.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
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
#ifndef IIO_AD7091R8_H
#define IIO_AD7091R8_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"

/******************************************************************************/
/***************************** Define Section *********************************/
/******************************************************************************/
#define ad7091r8_iio_device(chans) {					\
	.num_ch = NO_OS_ARRAY_SIZE(chans),				\
	.channels = chans,						\
	.pre_enable = (int32_t (*)())ad7091r8_buffer_preenable,		\
	.trigger_handler = (int32_t (*)())ad7091r8_trigger_handler,	\
	.debug_reg_read = (int32_t (*)())ad7091r8_iio_read_reg,		\
	.debug_reg_write = (int32_t (*)())ad7091r8_iio_write_reg	\
}

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
extern struct iio_trigger ad7091r8_iio_timer_trig_desc;

/** @struct ad7091r8_iio_dev
 *  @brief AD7091r8 IIO device descriptor structure
 */
struct ad7091r8_iio_dev {
	struct ad7091r8_dev *ad7091r8_dev;
	struct iio_device *iio_dev;
};

/** @struct ad7091r8_iio_dev_init_param
 *  @brief AD7091r8 IIO device initial parameters structure
 */
struct ad7091r8_iio_dev_init_param {
	struct ad7091r8_init_param *ad7091r8_dev_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Function to be called to initialize a AD7091R-2/-4/-8 device. */
int ad7091r8_iio_init(struct ad7091r8_iio_dev **iio_dev,
		      struct ad7091r8_iio_dev_init_param *init_param);

/*! Function to be called to remove a AD7091R-2/-4/-8 device. */
int ad7091r8_iio_remove(struct ad7091r8_iio_dev *desc);

#endif /** IIO_AD7091R8_H */

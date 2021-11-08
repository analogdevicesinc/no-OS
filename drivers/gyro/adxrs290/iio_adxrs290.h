/***************************************************************************//**
 *   @file   iio_adxrs290.h
 *   @brief  Implementation of ADXRS290 iio.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef IIO_ADXRS290_H
#define IIO_ADXRS290_H

#include "iio_types.h"
#include "gpio.h"
#include "irq.h"
#include "circular_buffer.h"

struct iio_adxrs290_desc {
	struct adxrs290_dev *dev;
	struct circular_buffer buf[1];
	struct gpio_desc *sync;
	struct irq_ctrl_desc *irq_ctrl;
	uint32_t irq_nb;
	uint32_t mask;
	char trigger_name[20];
};

struct iio_adxrs290_init_param {
	struct irq_ctrl_desc *irq_ctrl;
	uint32_t irq_nb;
	void *irq_config;
	struct gpio_init_param *gpio_sync;
	struct adxrs290_dev *dev;
	int8_t *buf;
	uint32_t buffer_size;
	char *trigger_name;
};

int32_t iio_adxrs290_cfg(struct iio_adxrs290_desc *desc,
			  struct iio_adxrs290_init_param *param);
int32_t iio_adxrs290_remove(struct iio_adxrs290_desc *desc);

extern struct iio_device adxrs290_iio_descriptor;
extern struct iio_trigger adxrs290_iio_trigger_descriptor;

#endif

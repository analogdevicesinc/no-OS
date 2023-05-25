/***************************************************************************//**
*   @file   iio_hmc630x.h
*   @brief  Header file of hmc6300 and hmc6301 driver extension for IIOD.
*   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
* Copyright 2023(c) Analog Devices, Inc.
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
#ifndef IIO_HMC630X_H
#define IIO_HMC630X_H

#include "iio.h"
#include "hmc630x.h"

struct hmc630x_iio_dev {
	struct hmc630x_dev *dev;
	struct iio_device *iio_dev;
};

struct hmc630x_iio_init_param {
	struct hmc630x_init_param *ip;
};

enum hmc630x_iio_attr_id {
	HMC630X_IIO_ATTR_ENABLED,
	HMC630X_IIO_ATTR_TEMP_EN,
	HMC630X_IIO_ATTR_VCO,
	HMC630X_IIO_ATTR_VCO_AVAILABLE,
	HMC630X_IIO_ATTR_VCO_BAND,
	HMC630X_IIO_ATTR_VCO_LOCK,
	HMC630X_IIO_ATTR_IF_ATTN,
	HMC6300_IIO_ATTR_RF_ATTN,
	HMC6301_IIO_ATTR_RF_LNA_GAIN,
	HMC6301_IIO_ATTR_BB_ATTN1,
	HMC6301_IIO_ATTR_BB_ATTN2,
	HMC6301_IIO_ATTR_BB_ATTNI_FINE,
	HMC6301_IIO_ATTR_BB_ATTNQ_FINE,
	HMC6301_IIO_ATTR_BB_LPC,
	HMC6301_IIO_ATTR_BB_HPC,
};

int32_t hmc630x_iio_init(struct hmc630x_iio_dev **iio_dev,
			 struct hmc630x_iio_init_param *init_param);
int32_t hmc630x_iio_remove(struct hmc630x_iio_dev *desc);

#endif /** IIO_HMC630X_H */

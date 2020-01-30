/***************************************************************************//**
 *   @file   iio_axi_adc_app.h
 *   @brief  Header file of iio_axi_adc_app.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef IIO_AXI_ADC_APP_H_
#define IIO_AXI_ADC_APP_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "axi_adc_core.h"
#include "axi_dmac.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct iio_axi_adc_desc
 * @brief Application desciptor.
 */
struct iio_axi_adc_app_desc {
	/** iio DAC device handle */
	struct iio_axi_adc *iio_axi_adc_inst;
};

/**
 * @struct iio_axi_adc_init_param
 * @brief Application configuration.
 */
struct iio_axi_adc_app_init_param {
	/** ADC device */
	struct axi_adc *rx_adc;
	/** Receive DMA device */
	struct axi_dmac *rx_dmac;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init application. */
int32_t iio_axi_adc_app_init(struct iio_axi_adc_app_desc **desc,
			     struct iio_axi_adc_app_init_param *param);
/* Free the resources allocated by iio_axi_adc_app_init(). */
int32_t iio_axi_adc_app_remove(struct iio_axi_adc_app_desc *desc);

#endif // IIO_AXI_ADC_APP_H_

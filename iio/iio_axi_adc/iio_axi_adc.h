/***************************************************************************//**
*   @file   iio_axi_adc.h
*   @brief  Header file of iio_axi_adc
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
#ifndef IIO_AXI_ADC_H_
#define IIO_AXI_ADC_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "iio_types.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct iio_axi_adc_init_par {
	struct axi_adc *adc;
	struct axi_dmac *dmac;
	uint32_t adc_ddr_base;
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

struct iio_axi_adc {
	struct axi_adc *adc;
	struct axi_dmac *dmac;
	uint32_t adc_ddr_base;
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Init axi_adc. */
ssize_t iio_axi_adc_init(struct iio_axi_adc **iio_axi_adc,
			 struct iio_axi_adc_init_par *init);
/* Free the resources allocated by iio_axi_adc_init(). */
ssize_t iio_axi_adc_remove(struct iio_axi_adc *iio_axi_adc);
/* Create iio_device. */
struct iio_device *iio_axi_adc_create_device(const char *device_name,
		uint16_t num_ch);
/* Delete iio_device. */
ssize_t iio_axi_adc_delete_device(struct iio_device *iio_adc_device);
/* Transfer data from ADC into RAM: "capture" */
ssize_t iio_axi_adc_transfer_dev_to_mem(void *iio_inst, size_t bytes_count,
					uint32_t ch_mask);
/* Read data from RAM to pbuf. It should be called after "iio_axi_adc_transfer_dev_to_mem()" */
ssize_t iio_axi_adc_read_dev(void *iio_inst, char *pbuf, size_t offset,
			     size_t bytes_count, uint32_t ch_mask);
/* Get an axi_adc xml */
ssize_t iio_axi_adc_get_xml(char** xml, struct iio_device *iio_dev);

#endif /* IIO_AXI_ADC_H_ */

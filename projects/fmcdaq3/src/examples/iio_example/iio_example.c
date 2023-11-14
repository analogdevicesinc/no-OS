/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for adf4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
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

#include "iio_example.h"

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 */
int iio_example_main(struct ad9152_dev *ad9152_device,
		     struct ad9680_dev *ad9680_device,
		     struct axi_adc *ad9680_core,
		     struct axi_dac *ad9152_core,
		     struct axi_dmac *ad9680_dmac)
{
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct axi_dmac *ad9152_dmac;
	int32_t status;

//	axi_dmac_init(&ad9152_dmac, &ad9152_dmac_param); todo-you have to add

	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_device *adc_dev_desc, *dac_dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9680_core,
		.rx_dmac = ad9680_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = ad9152_core,
		.tx_dmac = ad9152_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange
#endif
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (NO_OS_IS_ERR_VALUE(status))
		return -1;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (NO_OS_IS_ERR_VALUE(status))
		return -1;
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	static struct iio_data_buffer write_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
		IIO_APP_DEVICE("axi_dac", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff, NULL),
		IIO_APP_DEVICE("ad9680_dev", ad9680_device,
			       &ad9680_iio_descriptor, NULL, NULL, NULL),
		IIO_APP_DEVICE("ad9152_dev", ad9152_device,
			       &ad9152_iio_descriptor, NULL, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}

/***************************************************************************//**
 *   @file   iio_demo/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Cristian Pop (cristian.pop@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "demo_dev.h"
#include "iio_demo_dev.h"
#include "app_config.h"
#include "parameters.h"
#include "error.h"
#include "iio_app.h"

#ifdef XILINX_PLATFORM
#include <xparameters.h>
#include <xil_cache.h>
#include <xil_cache.h>
#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>

#define MAX_SIZE_BASE_ADDR	(NB_LOCAL_SAMPLES * DEMO_NUM_CHANNELS *\
					sizeof(uint16_t))

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
static uint8_t out_buff[MAX_SIZE_BASE_ADDR];

#define DAC_DDR_BASEADDR	((uint32_t)out_buff)
#define ADC_DDR_BASEADDR	((uint32_t)in_buff)

#endif

int32_t platform_init()
{
#ifdef ADUCM_PLATFORM
	if (ADI_PWR_SUCCESS != adi_pwr_Init())
		return FAILURE;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_HCLK, 1u))
		return FAILURE;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_PCLK, 1u))
		return FAILURE;
	adi_initComponents();
#endif
	return SUCCESS;
}

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;

	/* iio demo configurations. */
	struct iio_demo_init_param iio_demo_in_init_par;

	/* iio demo configurations. */
	struct iio_demo_init_param iio_demo_out_init_par;

	/* iio instance descriptor. */
	struct iio_demo_desc *iio_demo_in_desc;

	/* iio instance descriptor. */
	struct iio_demo_desc *iio_demo_out_desc;

	status = platform_init();
	if (IS_ERR_VALUE(status))
		return status;

	iio_demo_out_init_par = (struct iio_demo_init_param) {
		.dev_global_attr = 1100,
		.dev_ch_attr = 1111,
	};
	status = iio_demo_dev_init(&iio_demo_out_desc, &iio_demo_out_init_par);
	if (status < 0)
		return status;

	iio_demo_in_init_par = (struct iio_demo_init_param) {
		.dev_global_attr = 2200,
		.dev_ch_attr = 2211,
	};
	status = iio_demo_dev_init(&iio_demo_in_desc, &iio_demo_in_init_par);
	if (IS_ERR_VALUE(status))
		return status;

	struct iio_data_buffer rd_buf = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};
	struct iio_data_buffer wr_buf = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("demo_device_output", iio_demo_out_desc,
			       &iio_demo_dev_out_descriptor,NULL, &wr_buf),
		IIO_APP_DEVICE("demo_device_input", iio_demo_in_desc,
			       &iio_demo_dev_in_descriptor,&rd_buf, NULL)
	};

	return iio_app_run(devices, 2);
}

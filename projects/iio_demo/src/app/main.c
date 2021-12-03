/***************************************************************************//**
 *   @file   iio_demo/src/app/main.c
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

#include "app_config.h"
#include "parameters.h"
#include "iio_app.h"
#include "iio_adc_demo.h"
#include "iio_dac_demo.h"
#include "util.h"

#ifdef XILINX_PLATFORM
#include <xparameters.h>
#include <xil_cache.h>
#include <xil_cache.h>
#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM
#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>
#endif

#ifdef STM32_PLATFORM
#include "stm32_hal.h"
#endif

#include "error.h"
#define DEMO_CHANNELS		max(TOTAL_ADC_CHANNELS, TOTAL_DAC_CHANNELS)
#ifdef ENABLE_LOOPBACK
#define SAMPLES_PER_CHANNEL	200
static uint16_t loopback_buffs[DEMO_CHANNELS][SAMPLES_PER_CHANNEL];
#else //ENABLE_LOOPBACK
#define SAMPLES_PER_CHANNEL	0
#define loopback_buffs		NULL
#endif //ENABLE_LOOPBACK

#if defined(ADUCM_PLATFORM) || defined(STM32_PLATFORM) || defined(LINUX_PLATFORM)

#define MAX_SIZE_BASE_ADDR	(SAMPLES_PER_CHANNEL * DEMO_CHANNELS * \
					sizeof(uint16_t))

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
static uint8_t out_buff[MAX_SIZE_BASE_ADDR];

#define DAC_DDR_BASEADDR	((uint32_t)out_buff)
#define ADC_DDR_BASEADDR	((uint32_t)in_buff)

#endif

int32_t platform_init()
{
#if defined(ADUCM_PLATFORM)
	if (ADI_PWR_SUCCESS != adi_pwr_Init())
		return FAILURE;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_HCLK, 1u))
		return FAILURE;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_PCLK, 1u))
		return FAILURE;
	adi_initComponents();
#elif defined(STM32_PLATFORM)
	HAL_Init();
	SystemClock_Config();
#endif
	return 0;
}

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;

	/* adc demo configurations. */
	struct adc_demo_init_param adc_init_par;

	/* dac demo configurations. */
	struct dac_demo_init_param dac_init_par;

	/* adc instance descriptor. */
	struct adc_demo_desc *adc_desc;

	/* dac instance descriptor. */
	struct dac_demo_desc *dac_desc;

	status = platform_init();
	if (status != SUCCESS)
		return status;

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_data_buffer dac_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	adc_init_par = (struct adc_demo_init_param) {
		.loopback_buffer_len = SAMPLES_PER_CHANNEL,
		.loopback_buffers = loopback_buffs,
		.dev_global_attr = 3333,
		.dev_ch_attr = {1111,1112,1113,1114,1115,1116,1117,1118,1119,1120,1121,1122,1123,1124,1125,1126}
	};
	status = adc_demo_init(&adc_desc, &adc_init_par);
	if (status != SUCCESS)
		return status;

	dac_init_par = (struct dac_demo_init_param) {
		.loopback_buffer_len = SAMPLES_PER_CHANNEL,
		.loopback_buffers = loopback_buffs,
		.dev_global_attr = 4444,
		.dev_ch_attr = {1111,1112,1113,1114,1115,1116,1117,1118,1119,1120,1121,1122,1123,1124,1125,1126}
	};
	status = dac_demo_init(&dac_desc, &dac_init_par);
	if (status != SUCCESS)
		return status;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adc_demo", adc_desc,
			       &adc_demo_iio_descriptor,&adc_buff, NULL),
		IIO_APP_DEVICE("dac_demo", dac_desc,
			       &dac_demo_iio_descriptor,NULL, &dac_buff)
	};

	return iio_app_run(devices, ARRAY_SIZE(devices));
}

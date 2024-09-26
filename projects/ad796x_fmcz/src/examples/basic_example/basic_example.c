/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of BASIC example
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "basic_example.h"
#include "common_data.h"
#include "ad796x.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief BASIC example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function basic_app_run and will not return.
*******************************************************************************/
int basic_example_main()
{
	uint32_t *buf = (uintptr_t)ADC_DDR_BASEADDR;
	int i, ret;
	struct ad796x_dev *adc_dev;

	pr_info("init\n");

	ret = ad796x_init(&adc_dev, &ad796x_init_param);
	if (ret)
		return ret;

	pr_info("Capture start.\n");
	ret = ad796x_read_data(adc_dev, buf, SAMPLES_PER_CHANNEL);
	if (ret) {
		pr_err("read data error %d\n", ret);
		ad796x_remove(adc_dev);
		return ret;
	}

	for (i = 0; i < SAMPLES_PER_CHANNEL; i++, buf++)
		printf("CH1: %ld\n", *buf);

	pr_info("\n Capture done.\n");

	return ad796x_remove(adc_dev);
}

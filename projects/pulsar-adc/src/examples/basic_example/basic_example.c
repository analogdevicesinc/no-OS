/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of IIO example for basic_demo project.
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

#include "basic_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "pulsar_adc.h"

/***************************************************************************//**
 * @brief basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute print the sample data.
*******************************************************************************/
int basic_example_main()
{
	struct pulsar_adc_dev *dev;
	uint32_t *data = ADC_DDR_BASEADDR;
	int32_t ret, i;
	uint16_t resolution;
	char sign;

	ret = pulsar_adc_init(&dev, &pulsar_adc_init_param);
	if (ret)
		return ret;

	sign = dev->dev_info->sign;
	resolution = dev->dev_info->resolution;

	ret = pulsar_adc_read_data(dev, data, SAMPLES_PER_CHANNEL);
	if (ret) {
		pr_info("Error: pulsar_adc_read_data: %ld\n", ret);
		pulsar_adc_remove(dev);
		return ret;
	}

	for (i = 0, data = ADC_DDR_BASEADDR; i < SAMPLES_PER_CHANNEL; i++, data++) {
		if (sign == 's')
			printf("ADC: %ld\n\r", no_os_sign_extend32(*data, resolution - 1));
		else
			printf("ADC: %ld\n\r", *data);
	}

	return pulsar_adc_remove(dev);
}

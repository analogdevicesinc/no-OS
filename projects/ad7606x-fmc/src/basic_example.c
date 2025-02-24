/***************************************************************************//**
* @file basic_example.c
* @brief Implementation of a basic example
* @author Alexandru Ardelean (aardelean@baylibre.com)
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

#include <string.h>

#include <xil_cache.h>

#include "ad7606.h"
#include "parameters.h"
#include "common_data.h"

#include "no_os_print_log.h"

#define AD7606X_FMC_SAMPLE_NO		1000

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	uint32_t *buf = (uint32_t *)ADC_DDR_BASEADDR;
	struct ad7606_init_param *init_param;
	struct ad7606_dev *dev;
	double scales[8];
	uint32_t i, ch;
	int sign_bit, ret;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	if (AD7606X_MODE == AD7606X_MODE_PARALLEL) {
		pr_info("AD7606X Reference Design - Parallel interface.\n");
		init_param = &ad7606x_init_param_parallel;
	} else {
		pr_info("AD7606X Reference Design - Serial interface.\n");
		init_param = &ad7606x_init_param_serial;
	}

	ret = ad7606_init(&dev, init_param);
	if (ret) {
		pr_err("Error initializing.\n");
		return ret;
	}

	ret = ad7606_capture_pre_enable(dev);
	if (ret) {
		pr_err("Error pre-enabling buffers\n");
		goto error;
	}

	memset(buf, 0, AD7606X_FMC_SAMPLE_NO * sizeof(uint32_t));
	ret = ad7606_read_samples(dev, buf, AD7606X_FMC_SAMPLE_NO);
	if (ret < 0) {
		pr_err("Error getting samples.\n");
		goto error;
	}

	for (ch = 0; ch < 8; ch++) {
		ret = ad7606_get_ch_scale(dev, ch, &scales[ch]);
		if (ret)
			goto error;
	}

	sign_bit = ad7606_get_resolution_bits(dev) - 1;

	for (i = 0; i < AD7606X_FMC_SAMPLE_NO; i += ch) {
		for (ch = 0; ch < 8; ch++) {
			int32_t sample = no_os_sign_extend32(buf[i + ch], sign_bit);
			int mvolts = sample * scales[ch];
			pr_info("%08lx=%d ", sample, mvolts);
		}
		pr_info("\n");
	}

	pr_info("Capture done. \n");

error:
	ad7606_capture_post_disable(dev);
	ad7606_remove(dev);

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return ret;
}

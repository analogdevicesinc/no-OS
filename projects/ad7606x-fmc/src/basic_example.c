/***************************************************************************//**
* @file basic_example.c
* @brief Implementation of a basic example
* @author Alexandru Ardelean (aardelean@baylibre.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#include <string.h>

#include "ad7606.h"
#include "parameters.h"
#include "common_data.h"

#include "no_os_print_log.h"

/******************************************************************************/
/************************ Variables & Definitions *****************************/
/******************************************************************************/

#define AD7606X_FMC_SAMPLE_NO		(1000)

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	struct ad7606_init_param *init_param = &ad7606x_init_param_serial;
	uint32_t *buf = (uint32_t *)ADC_DDR_BASEADDR;
	struct ad7606_dev *dev;
	uint32_t i, ch;
	int ret;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	pr_info("AD7606X Reference Design.\n");

	ret = ad7606_init(&dev, init_param);
	if (ret < 0) {
		pr_err("Error initializing.\n");
		goto error;
	}

	memset(buf, 0, AD7606X_FMC_SAMPLE_NO * sizeof(uint32_t));
	ret = ad7606_read_samples(dev, buf, AD7606X_FMC_SAMPLE_NO);
	if (ret < 0) {
		pr_err("Error getting samples.\n");
		goto error;
	}

	for (i = 0; i < AD7606X_FMC_SAMPLE_NO; i+= ch) {
		for (ch = 0; ch < 8; ch++) {
			int32_t voltage_mV = ad7606_scale_sample(dev, buf[i + ch], ch);
			int32_t votage_Abs_mV = voltage_mV < 0 ? -voltage_mV : voltage_mV;
			int volts = votage_Abs_mV / 1000;
			int mvolts = votage_Abs_mV % 1000;
			pr_info("%08lx=%s%d.%03d ", buf[i + ch], (voltage_mV < 0 ? "-" : " "),
				volts, mvolts);
		}
		pr_info("\n");
	}

	pr_info("Capture done. \n");

error:
	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return ret;
}

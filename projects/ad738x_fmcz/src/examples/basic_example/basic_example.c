/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic example for  eval-ad738x project.
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
#include "no_os_print_log.h"


#define AD738X_STORAGE_BITS 16
#define AD738X_RESOLUTION 14
#define AD738X_SHIFT (AD738X_STORAGE_BITS - AD738X_RESOLUTION)
/***************************************************************************//**
 * @brief basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute print the sample data.
*******************************************************************************/
int basic_example_main()
{
	struct ad738x_dev *dev;
	uint32_t *buf = (uint32_t *)ADC_DDR_BASEADDR;
	int16_t ch0, ch1;
	int32_t ret, i;

	ret = ad738x_init(&dev, &ad738x_init_param);
	if (ret)
		return ret;
	while (true) {
		ret = ad738x_read_data(dev, buf, 400);
		if (ret != 0)
			return ret;

		for (i = 0; i < 400; i++) {
			ch0 = buf[i] >> AD738X_STORAGE_BITS;
			ch1 = buf[i] & NO_OS_GENMASK(AD738X_STORAGE_BITS, 0);
			ch0 >>= AD738X_SHIFT;
			ch1 >>= AD738X_SHIFT;

			pr_info("ADC sample %lu: %hd %hd\r\n", i, ch0, ch1);
		}

		no_os_mdelay(5000);
	}

	return ad738x_remove(dev);
}

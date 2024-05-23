/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of Basic example
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "basic_example.h"
#include "common_data.h"
#include "ad469x.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief basic example main execution.
 *
 * @return ret - 0 on success.
*/
int basic_example_main()
{
	struct ad469x_dev *dev;
	int ret;
	uint32_t i, j;
	uint8_t num_channels;
	uint32_t *buf = ADC_DDR_BASEADDR;

	ret = ad469x_init(&dev, &ad469x_init_param);
	if (ret)
		return ret;

	ret = ad469x_get_num_channels(dev, &num_channels);
	if (ret)
		return ret;

	if (ad469x_init_param.ch_sequence == AD469x_single_cycle) {
		for (i = 0; i < num_channels; i++) {
			ret = ad469x_read_data(dev, i, buf, NUM_SAMPLES);
			if (ret)
				goto out;
			for (j = 0; j <  NUM_SAMPLES; j++) {
				printf("ch: %ld %ld\r\n",
				       i,  buf[(i * num_channels) + j]);
			}
		}
	} else {
		ret = ad469x_seq_read_data(dev, buf, NUM_SAMPLES);
		if (ret)
			goto out;

		for (i = 0; i < NUM_SAMPLES; i++) {
			for (j = 0; j < num_channels; j++) {
				uint32_t index = (i * num_channels) + j;
				if (ad469x_is_temp_channel(dev, j)) {
					printf(" temp: %ld", buf[index]);
					continue;
				}
				printf(" ch%ld: %ld", j, buf[index]);
			}
			printf("\r\n");
		}
	}
out:
	ad469x_remove(dev);
	return ret;
}

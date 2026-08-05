/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of Basic example
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "ad469x.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

/**
 * @brief basic example main execution.
 *
 * @return ret - 0 on success.
*/
int example_main()
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
				       (long)i, (long)buf[(i * num_channels) + j]);
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
					printf(" temp: %ld", (long)buf[index]);
					continue;
				}
				printf(" ch%ld: %ld", (long)j, (long)buf[index]);
			}
			printf("\r\n");
		}
	}
out:
	ad469x_remove(dev);
	return ret;
}

/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic example for  eval-ad738x project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "basic_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#ifdef NUMBER_OF_CHANNELS
#define AD738X_NUM_CHANNELS NUMBER_OF_CHANNELS
#else
#define AD738X_NUM_CHANNELS 2
#endif

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
#ifdef LATTICE_PLATFORM
	uint16_t *buf = (uint16_t *)ADC_DDR_BASEADDR;
#else
	uint32_t *buf = (uint32_t *)ADC_DDR_BASEADDR;
	int16_t ch0, ch1;
#endif

	int32_t ret, i, j;

	ret = ad738x_init(&dev, &ad738x_init_param);
	if (ret)
		return ret;
	while (true) {
		ret = ad738x_read_data(dev, buf, 400);
		if (ret != 0)
			return ret;
#ifdef STM32_PLATFORM
		int16_t ch0, ch1;
		for (i = 0; i < 400; i++) {
			ch0 = buf[i] >> AD738X_STORAGE_BITS;
			ch1 = buf[i] & NO_OS_GENMASK(AD738X_STORAGE_BITS, 0);
			ch0 >>= AD738X_SHIFT;
			ch1 >>= AD738X_SHIFT;

			pr_info("ADC sample %lu: %hd %hd\r\n", i, ch0, ch1);
		}
#else
		for (i = 0; i < 400; i += AD738X_NUM_CHANNELS) {
			for (j = 0; j < AD738X_NUM_CHANNELS; j++) {
				pr_info("CH%d: %d\t", j, buf[i + j]);
			}
			pr_info("\r\n");
		}
#endif
		no_os_mdelay(5000);
	}

	return ad738x_remove(dev);
}

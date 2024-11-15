/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for ad7091r8-sdz project
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "basic_example.h"
#include "common_data.h"
#include "ad7091r8.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int basic_example_main()
{
	struct ad7091r8_dev *ad7091r8_dev;
	uint16_t val;
	int ret;

	printf("AD7091R-8 basic_example - Read each channel sequentially\n\r");
	/* Use ad7091r4_ip and ad7091r2_ip for AD7091R-4 and AD7091R-2 respectively */
	ret = ad7091r8_init(&ad7091r8_dev, &ad7091r8_ip);
	if (ret)
		return ret;

	/* Enable all channels */
	ret = ad7091r8_spi_reg_write(ad7091r8_dev, 0x01, 0xFF);
	if (ret)
		goto error;

	while(1) {
		ret = ad7091r8_sequenced_read(ad7091r8_dev, &val);
		if (ret)
			goto error;

		printf("Channel %d raw ADC output: %d\n\r",
		       no_os_field_get(AD7091R8_REG_RESULT_CH_ID_MASK, val),
		       no_os_field_get(AD7091R8_REG_RESULT_DATA_MASK, val));
		no_os_mdelay(1000);
	}

error:
	printf("Error on AD7091R-8 basic_example: %d\n\r", ret);
	ad7091r8_remove(ad7091r8_dev);
	return ret;
}

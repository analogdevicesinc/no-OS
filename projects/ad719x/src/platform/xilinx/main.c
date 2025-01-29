/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for XILINX platform of ad719x project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "ad719x.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/***************************************************************************//**
 * @brief Main function execution for XILINX platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/

int main()
{
	struct ad719x_dev *dev;
	float temp, v;
	int ret, x = 0;
	uint32_t avg;
	uint32_t samples = 0;
	uint32_t command;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();


	ad7190_dev_ip.chip_id = AD7195;
	ad7190_dev_ip.data_rate_code = 1;
        ad7190_dev_ip.current_gain = AD719X_ADC_GAIN_1;
    ret = ad719x_init(&dev, ad7190_dev_ip);
	if (ret)
		return ret;

    printf("init \n" );
	ret = ad719x_temperature_read(dev, &temp);
	if (ret)
		return ret;

    command = AD719X_MODE_SEL(AD719X_MODE_CONT) |
		  AD719X_MODE_CLKSRC(AD719X_INT_CLK_4_92_MHZ_TRIST) |
		  AD719X_MODE_RATE(dev->data_rate_code);

	ret = ad719x_set_register_value(dev, AD719X_REG_MODE, command, 3);
	if (ret != 0)
		return ret;

        /* Set ch0 */
        ret = ad719x_channels_select(dev, AD719X_CH_MASK(AD719X_CH_0));
       if (ret)
		return ret;

	while (x < 200) {
		ret = ad719x_get_register_value(dev, AD719X_REG_DATA, 3, &samples);
		no_os_mdelay(10);
		pr_info("%d \n", samples);
		x++;
	}

	ret = ad719x_continuous_read_avg(dev, 100, &avg);
	if (ret)
		return ret;

	v = ad719x_convert_to_volts(dev, avg, 3.3);

	pr_info("Temperature = %.6f C\n", temp);
	pr_info("Average read from 100 samples = %.6f Volts.", avg);

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

	return ret;
}

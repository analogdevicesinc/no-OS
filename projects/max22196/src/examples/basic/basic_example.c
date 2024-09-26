/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example sources file for max22196.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "max22196.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

static uint32_t delay_available[MAX22196_CHANNELS] = {
	50, 100, 400, 800, 1600, 3200, 12800, 20000
};

/**
 * @brief Basic example main execution
 * @return ret - Result for the main execution of the example. If working
 * 		 correctly the device's channel 0 will be set to SOURCE mode
 * 		 and also have delay values changed. Counter values for
 * 		 channel 0 will be changed as well and printed afterwards.
 * 		 0 will be returned at the end in case of succes.
*/
int basic_example_main()
{
	int ret;
	struct max22196_desc *max22196_desc;
	enum max22196_delay delay_write = MAX22196_DELAY_100US;
	enum max22196_delay delay_read;
	uint32_t flt_en, clr_filtr;
	uint8_t rand_reg;

	uint16_t chan_msb_lsb_byte = 0xFF00;
	uint16_t chan_msb_lsb_byte2;

	ret = max22196_init(&max22196_desc, &max22196_ip);
	if (ret)
		goto exit;

	/* Setting source mode for MAX22196's channel 0. */
	ret = max22196_set_mode(max22196_desc, 0, MAX22196_SOURCE_MODE);
	if (ret)
		goto remove_max22196;

	/* Setting a filter for channel 0 and then reading if the delay was set
	   right. */
	ret = max22196_filter_set(max22196_desc, 0, 0, 1, delay_write);
	if (ret)
		goto remove_max22196;

	ret = max22196_filter_get(max22196_desc, 0, &clr_filtr, &flt_en, &delay_read);
	if (ret)
		goto remove_max22196;

	pr_info("Delay for channel 0 is %d us\n", delay_available[delay_read]);

	ret = max22196_set_chan_cnt(max22196_desc, 0, chan_msb_lsb_byte);
	if (ret)
		goto remove_max22196;

	ret = max22196_get_chan_cnt(max22196_desc, 0, &chan_msb_lsb_byte2);
	if (ret)
		goto remove_max22196;

	pr_info("Channel 0 LSB Byte : 0x%x\n", no_os_field_get(MAX22196_LSB_MASK,
			chan_msb_lsb_byte2));
	pr_info("Channel 0 MSB Byte : 0x%x\n", no_os_field_get(MAX22196_MSB_MASK,
			chan_msb_lsb_byte2));

remove_max22196:
	max22196_remove(max22196_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}

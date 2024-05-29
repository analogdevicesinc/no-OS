/***************************************************************************//**
 *   @file   voltage_output_example.c
 *   @brief  Implementation of channel output configuration for eval-ad5460 project
 *   @author Akila Marimuthu (akila.marimuthu@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "voltage_output_example.h"
#include "common_data.h"
#include "ad5460.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Voltage output example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int voltage_output_example_main()
{
	struct ad5460_desc *ad5460_desc;
	int ret;
	uint16_t dac_code0, val;
	int32_t output_in_mvolts_ch0 = 6000;

	ret = ad5460_init(&ad5460_desc, &ad5460_ip);
	if (ret)
		goto error;

	pr_info("ad5460 successfully initialized!\r\n");

	/* Set channel function */
	ret = ad5460_set_channel_function(ad5460_desc, 0, AD5460_VOLTAGE_OUT);
	if (ret)
		goto error_ad5460;

	/* set output range */
	ret = ad5460_set_channel_vout_range(ad5460_desc, 0, AD5460_VOUT_RANGE_0_12V);
	if (ret)
		goto error_ad5460;

	/* Set channel 0 output */
	ret = ad5460_dac_voltage_to_code(ad5460_desc, output_in_mvolts_ch0, &dac_code0,
					 0);
	if (ret)
		goto error_ad5460;

	ret = ad5460_set_channel_dac_code(ad5460_desc, 0, dac_code0);
	if (ret)
		goto error_ad5460;

	pr_info("For channel 0, expected output = %d mV \n DAC code = %d \n",
		output_in_mvolts_ch0, dac_code0);

	ret = ad5460_reg_read(ad5460_desc, AD5460_DAC_ACTIVE(0), &val);
	if (ret)
		goto error;

	pr_info("DAC ACTIVE CODE of channel 0 = %d \n", val);

	return 0;

error_ad5460:
	ad5460_remove(ad5460_desc);
error:
	pr_info("AD5460 Error!\r\n");
	return ret;
}

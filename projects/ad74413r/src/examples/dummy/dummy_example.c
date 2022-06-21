/***************************************************************************//**
 *   @file   dummy_example.c
 *   @brief  DUMMY example header for ad74413r project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "dummy_example.h"
#include "common_data.h"
#include "ad74413r.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int dummy_example_main()
{
	int ret;
	uint8_t val;
	uint32_t dac_code;
	uint16_t temp;
	union ad74413r_live_status status;
	struct ad74413r_desc *ad74413r_desc;
	struct ad74413r_init_param ad74413r_ip = {
		.chip_id = AD74412R,
		.comm_param = ad74413r_spi_ip
	};

	struct ad74413r_decimal result[3];

	ret = ad74413r_init(&ad74413r_desc, &ad74413r_ip);
	if (ret)
		goto error;

	ret = ad74413r_clear_errors(ad74413r_desc);
	if (ret)
		goto error;

	ret = ad74413r_set_adc_channel_enable(ad74413r_desc, AD74413R_CH_A, true);
	if (ret)
		goto error;

	ret = ad74413r_set_channel_function(ad74413r_desc, AD74413R_CH_A,
					    AD74413R_DIGITAL_INPUT);
	if (ret)
		goto error;

	/** Set the threshold voltage to 5V */
	ret = ad74413r_set_threshold(ad74413r_desc, AD74413R_CH_A, 5000);
	if (ret)
		goto error;

	/** The comparator output will be available on the GPO_A pin */
	ret = ad74413r_set_gpo_config(ad74413r_desc, AD74413R_CH_A,
				      AD74413R_GPO_CONFIG_COMP);
	if (ret)
		goto error;

	ret = ad74413r_set_adc_channel_enable(ad74413r_desc, AD74413R_CH_B, true);
	if (ret)
		goto error;

	ret = ad74413r_set_channel_function(ad74413r_desc, AD74413R_CH_B,
					    AD74413R_VOLTAGE_IN);
	if (ret)
		goto error;

	ret = ad74413r_set_adc_channel_enable(ad74413r_desc, AD74413R_CH_C, true);
	if (ret)
		goto error;

	ret = ad74413r_set_channel_function(ad74413r_desc, AD74413R_CH_C,
					    AD74413R_VOLTAGE_OUT);
	if (ret)
		goto error;

	ad74413r_dac_voltage_to_code(5000, &dac_code);
	/** Set the DAC output on channel C to 5V */
	ret = ad74413r_set_channel_dac_code(ad74413r_desc, AD74413R_CH_C, dac_code);
	if (ret)
		goto error;

	ret = ad74413r_set_adc_channel_enable(ad74413r_desc, AD74413R_CH_D, true);
	if (ret)
		return ret;

	/** Measure input current on channel D */
	ret = ad74413r_set_channel_function(ad74413r_desc, AD74413R_CH_D,
					    AD74413R_CURRENT_IN_EXT);
	if (ret)
		return ret;

	ret = ad74413r_set_diag_channel_enable(ad74413r_desc, AD74413R_CH_A, true);
	if (ret)
		goto error;

	ret = ad74413r_set_diag(ad74413r_desc, AD74413R_CH_A, AD74413R_DIAG_TEMP);
	if (ret)
		goto error;

	while (1) {
		/* Clear the screen. */
		pr_info("%c",27);
		pr_info("%c",'[');
		pr_info("%c",'2');
		pr_info("%c",'J');

		ret = ad74413r_gpo_get(ad74413r_desc, AD74413R_CH_A, &val);
		if (ret)
			goto error;

		for (uint32_t i = 0; i < 3; i++) {
			ret = ad74413r_adc_get_value(ad74413r_desc, AD74413R_CH_B + i, &result[i]);
			if (ret)
				goto error;
		}

		ret = ad74413r_get_live(ad74413r_desc, &status);
		if (ret)
			goto error;

		ret = ad74413r_get_temp(ad74413r_desc, AD74413R_CH_A, &temp);
		if (ret)
			goto error;

		pr_info("Channel A: GPO_A status %d (5V threshold)\n", val);
		pr_info("Channel B: %ld"".%02lu mV (Voltage input)\n",
			(int32_t)result[0].integer,
			result[0].decimal);
		pr_info("Channel C: %ld"".%02lu mA (Voltage output)\n",
			(int32_t)result[1].integer,
			result[1].decimal);
		pr_info("Channel D: %ld"".%02lu mA (Current input)\n",
			(int32_t)result[2].integer,
			result[2].decimal);
		pr_info("\nDiagnostic channel A: %d deg. C (die's temperature)\n", temp);

		pr_info("\n================================================================================\n\n");

		pr_info("Channel A fault: %d\n", status.status_bits.VI_ERR_A);
		pr_info("Channel B fault: %d\n", status.status_bits.VI_ERR_B);
		pr_info("Channel C fault: %d\n", status.status_bits.VI_ERR_C);
		pr_info("Channel D fault: %d\n", status.status_bits.VI_ERR_D);

		no_os_mdelay(1000);
	}

error:
	pr_info("Error!\n");
	ad74413r_remove(ad74413r_desc);

	return 0;
}

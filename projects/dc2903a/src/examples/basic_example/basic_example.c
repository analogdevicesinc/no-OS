/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic example for DC2903A project.
 *   @author JSanBuenaventura (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "ltc2672.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int basic_example_main()
{
	int i, ret, num_currents, toggle_times = 4;
	bool toggle_flag = false;
	float currents_to_set[] = {.21, .41, 1.21, 1.41}; // current to set in mA
	float currents_to_toggle[] = {0.4, 0.1};
	struct ltc2672_dev *ltc2672_desc;

	num_currents = NO_OS_ARRAY_SIZE(currents_to_set);

	/* Initialize device */
	ret = ltc2672_init(&ltc2672_desc, &ltc2672_ip);
	if (ret)
		return ret;

	/* Continuously configure the part and measure pins with multimeter for verification */
	while (1) {
		/*max current of 3.125mA*/
		ret = ltc2672_set_span_all_channels(ltc2672_desc,
						    LTC2672_50VREF);
		if (ret) {
			goto error;
		}

		/* Print Span set to all channels */
		pr_info("All DAC channels have span config of 3.125mA\n");

		for (i = 0; i < num_currents; i++) {
			ret = ltc2672_set_current_all_channels(ltc2672_desc, currents_to_set[i]);
			if (ret) {
				goto error;
			}

			/* Print Current set to all channels */
			pr_info("All DAC channels have current of: %0.2f mA\n", currents_to_set[i]);

			/*Lengthy delay to better visualize the change in values*/
			no_os_mdelay(1500);
		}

		/* Configure MUX pin to output the VREF measurement */
		pr_info("MUX ouptut pin configured to measure VREF (~1.25V).\n");

		ret = ltc2672_monitor_mux(ltc2672_desc, LTC2672_MUX_VREF);
		if (ret) {
			goto error;
		}

		/*Lengthy delay to better visualize the change in values*/
		no_os_mdelay(2000);

		/* Chip Power Down */
		pr_info("Chip Power Down. All Channels and VREF should measure 0.\n");

		ret = ltc2672_chip_power_down(ltc2672_desc);
		if (ret) {
			goto error;
		}

		/*Lengthy delay to better visualize the change in values*/
		no_os_mdelay(2000);

		/* Setup and Demo toggle function on OUT3 */
		pr_info("Toggle function on OUT3. Current toggles between %0.2f and %0.2f mA.\n",
			currents_to_toggle[0], currents_to_toggle[1]);

		ret = ltc2672_setup_toggle_channel(ltc2672_desc, LTC2672_DAC3,
						   currents_to_toggle[0], currents_to_toggle[1]);
		if (ret) {
			goto error;
		}

		ret = ltc2672_enable_toggle_channel(ltc2672_desc, 8);
		if (ret) {
			goto error;
		}

		for (i = 0; i < toggle_times; i++) {
			toggle_flag ^= true;

			ret = ltc2672_global_toggle(ltc2672_desc, toggle_flag);
			if (ret) {
				goto error;
			}

			/*Lengthy delay to better visualize the change in values*/
			no_os_mdelay(2000);
		}

		ret = ltc2672_enable_toggle_channel(ltc2672_desc, 0);
		if (ret) {
			goto error;
		}

		/*Lengthy delay to better visualize the change in values*/
		no_os_mdelay(2000);

	}// end while

	return 0;

error:
	ltc2672_remove(ltc2672_desc);
	return ret;
}

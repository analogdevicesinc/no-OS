/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic example for DC2903A project.
 *   @author JSanBuenaventura (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example code for eval-ltc4306-pmdz project
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
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
#include "common_data.h"
#include "ltc4306.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
int LTC4306_USED_DOWNSTREAM_CHANNELS[] = {1, 2};

#define LTC4306_NUM_DOWNSTREAM	NO_OS_ARRAY_SIZE(LTC4306_USED_DOWNSTREAM_CHANNELS)
#define MAX_DUT					MAX5380M

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
	struct ltc4306_dev *ltc4306;
	struct max538x_dev *max538x;
	float v0[] = {0.16, 0.26, 0.36, 0.46, 0.56, 0.66, 0.76, 0.86, 0.96};
	float inc = 0.5;
	int ret, i, j;
	int voltage_inc = NO_OS_ARRAY_SIZE(v0);
	int flash_instances = 16;

	ret = ltc4306_addr_gen(&ltc4306_user_init, LTC4306_HIGH, LTC4306_HIGH,
			       LTC4306_HIGH);
	if (ret)
		goto error_ltc4306;

	ret = ltc4306_init(&ltc4306, ltc4306_user_init);
	if (ret)
		goto error_ltc4306;

	ret = max538x_init(&max538x, max538x_user_init);
	if (ret)
		goto error_max538x;

	for (i = 1; i < 3; i++) {
		ret = ltc4306_set_gpio_output_state(ltc4306, i, true);
		if (ret)
			goto error_ltc4306;
	}

	pr_info("LTC4306 GPIO LED's will blink %d times:\n", flash_instances);

	/* Blink LTC4306 Green LEDs*/
	for (i = 0; i < flash_instances; i++) {
		ret = ltc4306_set_gpio_output_state(ltc4306, 2, false);
		if (ret)
			goto error_ltc4306;

		no_os_mdelay(50);

		ret = ltc4306_set_gpio_output_state(ltc4306, 1, false);
		if (ret)
			goto error_ltc4306;

		no_os_mdelay(50);

		ret = ltc4306_set_gpio_output_state(ltc4306, 2, true);
		if (ret)
			goto error_ltc4306;

		no_os_mdelay(50);

		ret = ltc4306_set_gpio_output_state(ltc4306, 1, true);
		if (ret)
			goto error_ltc4306;

		no_os_mdelay(50);
	}

	/* Turn OFF LTC4306 Green GPIO LEDs */
	ret = ltc4306_set_gpio_output_state(ltc4306, 1, true);
	if (ret)
		goto error_ltc4306;

	ret = ltc4306_set_gpio_output_state(ltc4306, 2, true);
	if (ret)
		goto error_ltc4306;

	pr_info("LTC4306 configure the %d attached MAX5380 DAC's: \n",
		LTC4306_NUM_DOWNSTREAM);
	for (i = 0; i < LTC4306_NUM_DOWNSTREAM; i++) {
		pr_info("DAC %d: \t", i + 1);
		for (j = 0; j < voltage_inc; j++) {
			pr_info("%0.4f \t", (v0[j] + (inc * (float)i)));
		}
		pr_info("\n");
	}

	no_os_mdelay(50);

	/* Multiple DAC Configuration Loop */
	while (1) {
		for (j = 0; j < voltage_inc; j ++) {
			for (i = 0; i < LTC4306_NUM_DOWNSTREAM; i++) {
				ret = ltc4306_set_downstream_channel(ltc4306,
								     LTC4306_USED_DOWNSTREAM_CHANNELS[i], true);
				if (ret)
					goto error_ltc4306;

				ret = max538x_set_voutput(max538x, (v0[j] + (inc * (float)i)));
				if (ret)
					goto error_max538x;

				ret = ltc4306_set_downstream_channel(ltc4306,
								     LTC4306_USED_DOWNSTREAM_CHANNELS[i], false);
				if (ret)
					goto error_ltc4306;
			}
		}// end num downstream
	}// end while

error_ltc4306:
	ltc4306_remove(ltc4306);
error_max538x:
	max538x_remove(max538x);
error:
	pr_info("Error!\r\n");
	return 0;
}

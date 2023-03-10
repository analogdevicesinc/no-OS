/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  BASIC example header for eval-adf4002-pmdz project
 *   @author GMois (george.mois@analog.com)
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
#include "adf4106.h"
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
int basic_example_main()
{
	struct adf4106_dev *adf4106_desc;
	struct adf4106_init_param adf4106_ip = { 0 };
	unsigned int x = 0;
	uint64_t set_f;
	int ret;

	// PA10
	const struct no_os_gpio_init_param gpio_le_init = {
			.number = 10,
			.port = 0,
			.pull = NO_OS_PULL_UP,
			.platform_ops = &stm32_gpio_ops,
			.extra = NULL
	};
	// PB1
	const struct no_os_gpio_init_param gpio_le2_init = {
			.number = 1,
			.port = 1,
			.pull = NO_OS_PULL_NONE,
			.platform_ops = &stm32_gpio_ops,
			.extra = NULL
	};
	// PB2
	struct no_os_gpio_init_param gpio_ce_init = {
			.number = 2,
			.port = 1,
			.pull = NO_OS_PULL_NONE,
			.platform_ops = &stm32_gpio_ops,
			.extra = NULL
	};
	// PA5
	struct no_os_gpio_init_param gpio_ce2_init = {
			.number = 5,
			.port = 0,
			.pull = NO_OS_PULL_NONE,
			.platform_ops = &stm32_gpio_ops,
			.extra = NULL
	};
	struct adf4106_settings_t adf4106_init_settings =
	{
			/** Reference Input Frequency */
			.ref_in = 10000000, //10 MHz
			/** PFD max frequency */ //1000 kHz
			.pfd_max = 1000000,
			/** The initial value of the 14 bit Reference Counter register */
			.ref_counter = 0, //10
			/** The width of the anti-backlash pulse, this pulse
			 * ensures that no dead zone is in the PFD transfer function and minimizes
			 * phase noise and reference spurs.
			 */
			.anti_backlash_width = 0,
			/** Should be set to zero for Normal operation */
			.test_mode_bits = 0,
			/** determines the number of consecutive cycles of phase
			 * delay, that must occur before lock detect is set
			 */
			.lock_detect_precision = 0,

			/* N Latch */
			/** a 6 bits counter is supported at ADF4106 */
			.a_n_counter = 0, //8
			/** a 13 bits counter */
			.b_n_counter = 0, //181
			/** determines which charge pump current settings is used */
			.cp_gain = 0,

			/* Functional/Initialization latch */
			/** resets the R and N counters */
			.counter_reset = 0,
			/** activate power down mode */
			.power_down1 = 0,
			/** the type of the MUXOUT output */
			.muxout_control = 1,
			/** the polarity of the Phase Detector */
			.phase_detector_pol = 1,
			/** the type of the Charge Pump output */
			.cp_type = 0,
			/** set the de:sired Fast Lock Mode */
			.fast_lock_mode = 0,
			/** how long will be the secondary charge pump current
			 * active, before reverting to the primary current
			 */
			.timer_counter_control = 0,
			/** is used when the RF output is stable and the system is
			 * in static state
			 */
			.current_setting1 = 15,
			/** is meant to be used when the system is dynamic and in a
			 * state of change (i.ae., when a new output frequency is programmed)
			 */
			.current_setting2 = 15,
			/** define the type of the power down */
			.power_down2 = 0,
			/** the value of the prescaler */
			.prescaler_value = 2,
	};

	/* SPI */
	adf4106_ip.spi_init = adf4106_spi_ip;
	/* GPIO */
	adf4106_ip.gpio_le = gpio_le_init;
	adf4106_ip.gpio_ce = gpio_ce_init;
	adf4106_ip.gpio_le2 = gpio_le2_init;
	adf4106_ip.gpio_ce2 = gpio_ce2_init;
	/* Device Settings */
	adf4106_ip.this_device = ID_ADF4106,
	adf4106_ip.init_method = INIT_LATCH,
	adf4106_ip.adf4106_st = adf4106_init_settings;

	/* Clear the screen. */
	printf("%c",27);
	printf("%c",'[');
	printf("%c",'2');
	printf("%c",'J');
	pr_info("ADF4106 sample project.\n");

	ret = adf4106_init(&adf4106_desc, adf4106_ip);
	if (ret)
		goto error;

	pr_info("Device initialized.\n");

	while(1) {
		x++;
		pr_info("+\n");
		no_os_mdelay(3000);

		if (x == 3){
			pr_info("Setting frequency to 5800MHz.\n");
			set_f = adf4106_set_frequency(adf4106_desc,
							   5800000000);

			printf("Frequency set to %luMHz.\n", (int)(set_f/1000000));
		}
	}

error:
	pr_info("Error!\n");
	return 0;
}

/***************************************************************************//**
 *   @file   sweep_example.c
 *   @brief  On-device VCO-cal sweep example for the adf4368 project.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include "common_data.h"
#include "no_os_print_log.h"

/* Sweep configuration. */
#define ADF4368_SWEEP_START_HZ		4550000000ULL
#define ADF4368_SWEEP_STOP_HZ		4700000000ULL
/* Coarse step to build the VCO cal table. */
#define ADF4368_SWEEP_CAL_STEP_HZ	1000000ULL
/* Fine step used for the actual sweep. */
#define ADF4368_SWEEP_STEP_HZ		5000ULL
#define ADF4368_SWEEP_DWELL_US		100

/**
 * @brief Sweep example main execution.
 *
 * Runs the VCO-cal sweep algorithm natively on the device: build the
 * calibration table with an auto-cal sweep, then replay the range with the
 * manual-cal FRAC/N_INT sweep.
 *
 * @return ret - Result of the example execution.
 */
int example_main()
{
	struct adf4368_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &adf4368_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adf4368_init(&dev, &adf4368_ip);
	if (ret)
		goto error;

	/* Single-loop reference, doubler off. */
	ret = adf4368_set_en_ref_doubler(dev, false);
	if (ret)
		goto remove_adf4368;

	ret = adf4368_set_ref_div(dev, 1);
	if (ret)
		goto remove_adf4368;

	/* Per-step dwell applied by the sweep. */
	dev->manual_cal.sweep_delay_us = ADF4368_SWEEP_DWELL_US;

	pr_info("Building VCO cal table %u..%u MHz step %u kHz\n",
		(unsigned int)(ADF4368_SWEEP_START_HZ / 1000000ULL),
		(unsigned int)(ADF4368_SWEEP_STOP_HZ / 1000000ULL),
		(unsigned int)(ADF4368_SWEEP_CAL_STEP_HZ / 1000ULL));

	ret = adf4368_sweep_auto_cal(dev, ADF4368_SWEEP_START_HZ,
				     ADF4368_SWEEP_STOP_HZ,
				     ADF4368_SWEEP_CAL_STEP_HZ);
	if (ret)
		pr_warning("sweep_auto_cal failed %d\n", ret);
	else
		pr_info("vco_cal_count %u\n",
			(unsigned int)dev->manual_cal.vco_cal_count);

	/* Replay the range applying the stored VCO cal manually per step. */
	if (dev->manual_cal.vco_cal_count) {
		pr_info("Running manual sweep\n");
		ret = adf4368_sweep_manual_cal(dev, ADF4368_SWEEP_START_HZ,
					       ADF4368_SWEEP_STOP_HZ,
					       ADF4368_SWEEP_STEP_HZ);
		if (ret)
			pr_warning("sweep_manual_cal failed %d\n", ret);
		else
			pr_info("manual sweep done\n");
	}

remove_adf4368:
	adf4368_remove(dev);
error:
	if (ret)
		pr_info("Error!\n");

	return ret;
}

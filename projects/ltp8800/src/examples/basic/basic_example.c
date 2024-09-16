/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltp8800 project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "ltp8800.h"

int basic_example_main()
{
	struct ltp8800_dev *dev;
	struct ltp8800_status status;
	int ret, vals[4];

	pr_info("Running basic example.\n");

	ret = ltp8800_init(&dev, &ltp8800_ip);
	if (ret)
		goto exit;

	ret = ltp8800_vout_settings(dev, LTP8800_VOUT_SETTING_ADI_FACTORY);
	if (ret)
		goto exit;

	ret = ltp8800_vout_value(dev, 800);
	if (ret)
		goto exit;

	ret = ltp8800_set_vin(dev, 45000, 40000);
	if (ret)
		goto exit;

	ret = ltp8800_set_fault_limit(dev, LTP8800_VIN_OV_FAULT_LIMIT_TYPE,
				      65500);
	if (ret)
		goto exit;

	ret = ltp8800_loop_compensation(dev, 120, 220, 95, 40);
	if (ret)
		goto exit;

	while(1) {
		ret = ltp8800_read_value(dev, LTP8800_VIN, &vals[0]);
		if (ret)
			goto exit;

		ret = ltp8800_read_value(dev, LTP8800_VOUT, &vals[1]);
		if (ret)
			goto exit;

		ret = ltp8800_read_value(dev, LTP8800_IOUT, &vals[2]);
		if (ret)
			goto exit;

		ret = ltp8800_read_value(dev, LTP8800_FORWARD_DIODE_TEMP,
					 &vals[3]);
		if (ret)
			goto exit;

		ret = ltp8800_read_status(dev, LTP8800_STATUS_ALL_TYPE,
					  &status);
		if (status.vout) {
			pr_debug("Status vout asserted.\n");
		}
		if (status.iout) {
			pr_debug("Status iout asserted.\n");
		}
		if (status.input) {
			pr_debug("Status input asserted.\n");
		}
		if (status.temp) {
			pr_debug("Status temp asserted.\n");
		}
		if (status.cml) {
			pr_debug("Status cml asserted.\n");
		}
		if (status.mfr_specific) {
			pr_debug("Status mfr_specific asserted.\n");
		}

		pr_info("vin = %d mV | vout = %d mV | iout = %d mA | temp0 = %d C\n",
			vals[0], vals[1], vals[2], vals[3] / 1000);

		no_os_mdelay(500);
	}

exit:
	pr_err("Error code: %d.\n", ret);
	ltp8800_restore_user_settings(dev);
	ltp8800_set_device_state(dev, LTP8800_STATE_OFF);
	ltp8800_remove(dev);
	return ret;
}

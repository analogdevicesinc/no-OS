/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for adp5055 project.
 *   @author Angelo Catapang (angelo.catapang@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "adp5055.h"

int example_main()
{
	int ret;

	struct adp5055_desc *adp5055_desc;
	uint8_t model_id;

	float vout1_voltage = EXAMPLE_VOUT1;
	float vout1_ref;
	uint8_t vid1_code;

	uint8_t pg1_val;
	uint8_t status_lch;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adp5055_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adp5055_init(&adp5055_desc, &adp5055_ip);
	if (ret)
		goto exit;

	ret = adp5055_read(adp5055_desc, ADP5055_MODEL_ID, &model_id);
	if (ret)
		goto exit;

	if (model_id != 0x40)
		goto exit;

	ret = adp5055_set_dvs_auto(adp5055_desc, true);
	if (ret)
		goto exit;

	ret = adp5055_set_auto_pwm_psm(adp5055_desc, ADP5055_VOUT1, false);
	if (ret)
		goto exit;

	ret = adp5055_set_output_discharge(adp5055_desc, ADP5055_VOUT1, false);
	if (ret)
		goto exit;

	ret = adp5055_set_enable_disable_delay(adp5055_desc, ADP5055_VOUT1,
					       ADP5055_EN_NO_DELAY, ADP5055_DIS_NO_DELAY);
	if (ret)
		goto exit;

	ret = adp5055_set_dvs_interval(adp5055_desc, ADP5055_VOUT1,
				       ADP5055_DVS_INTVAL_125US);
	if (ret)
		goto exit;

	ret = adp5055_set_vidx_lim(adp5055_desc, ADP5055_VOUT1, ADP5055_VIDX_HIGH, 0);
	if (ret)
		goto exit;

	ret = adp5055_set_vidx_lim(adp5055_desc, ADP5055_VOUT1, ADP5055_VIDX_LOW, 0);
	if (ret)
		goto exit;


	ret = adp5055_set_fast_transient(adp5055_desc, ADP5055_VOUT1, ADP5055_FT_NONE);
	if (ret)
		goto exit;

	ret = adp5055_set_pg_hw_delay(adp5055_desc, false);
	if (ret)
		goto exit;

	ret = adp5055_set_pg_channel_output(adp5055_desc, ADP5055_VOUT1, true);
	if (ret)
		goto exit;

	vout1_ref = vout1_voltage / (1 + (EXAMPLE_RTOP1 / EXAMPLE_RBOT1));
	vid1_code = (uint8_t)((vout1_ref - 0.408) / 0.0015);

	ret = adp5055_set_vid_code(adp5055_desc, ADP5055_VOUT1, vid1_code);
	if (ret)
		goto exit;

	ret = adp5055_enable_channel(adp5055_desc, ADP5055_VOUT1, true);
	if (ret)
		goto exit;

	ret = adp5055_start_vout(adp5055_desc, ADP5055_VOUT1);
	if (ret)
		goto exit;


	/* Checking statuses. */
	ret = adp5055_read_pg(adp5055_desc, ADP5055_VOUT1, &pg1_val);
	if (ret)
		goto exit;

	ret = adp5055_read(adp5055_desc, ADP5055_STATUS_LCH, &status_lch);
	if (ret)
		goto exit;

exit:
	if (ret)
		pr_info("Error!\n");

	adp5055_remove(adp5055_desc);

	return ret;
}

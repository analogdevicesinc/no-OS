/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for adp5055 project.
 *   @author Angelo Catapang (angelo.catapang@analog.com)
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

	ret = adp5055_set_enable_disable_delay(adp5055_desc, ADP5055_VOUT1, ADP5055_EN_NO_DELAY, ADP5055_DIS_NO_DELAY);
	if (ret)
		goto exit;

	ret = adp5055_set_dvs_interval(adp5055_desc, ADP5055_VOUT1, ADP5055_DVS_INTVAL_125US);
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
	vid1_code = (uint8_t) ((vout1_ref - 0.408) / 0.0015);

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

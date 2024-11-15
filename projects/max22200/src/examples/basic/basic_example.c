/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "basic_example.h"
#include "common_data.h"
#include "max22200.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

static const char *const ch_drive[2] = {
	"CURRENT-DRIVE",
	"VOLTAGE-DRIVE"
};

static const char *const ch_side[2] = {
	"LOW-SIDE",
	"HIGH-SIDE"
};

static const char *const ch_op_mode[3] = {
	"INDEPENDENT",
	"HALF-BRIDGE",
	"FULL-BRIDGE"
};

static const uint32_t chop_frequency[2] = {
	100, 80
};

int basic_example_main()
{
	struct max22200_desc *max22200_desc;
	int ret;
	uint32_t i;
	enum max22200_ch_drive chan_drive;
	enum max22200_ch_side chan_side;
	enum max22200_ch_op_mode chan_op_mode;
	enum max22200_ch_freq chan_freq;
	enum max22200_chopping_freq chop_freq;

	ret = max22200_init(&max22200_desc, &max22200_ip);
	if (ret)
		goto exit;

	ret = max22200_set_chop_freq(max22200_desc, MAX22200_FREQ_80KHZ);
	if (ret)
		goto remove_max22200;

	ret = max22200_get_chop_freq(max22200_desc, &chop_freq);
	if (ret)
		goto remove_max22200;

	ret = max22200_set_ch_mode(max22200_desc, 0, MAX22200_CURRENT_DRIVE,
				   MAX22200_LOW_SIDE,
				   MAX22200_HALF_BRIDGE_MODE);
	if (ret)
		goto remove_max22200;

	ret = max22200_set_ch_mode(max22200_desc, 1, MAX22200_CURRENT_DRIVE,
				   MAX22200_LOW_SIDE,
				   MAX22200_HALF_BRIDGE_MODE);
	if (ret)
		goto remove_max22200;

	pr_info("Both channel 0 and channel 1 are now configured as half-bridge\n");
	pr_info("Channel drive for both is Current Drive.\n");
	pr_info("Channel side for both is Low-Side.\n");

	ret = max22200_set_ch_freq(max22200_desc, 0, MAX22200_FREQMAIN_DIV_2);
	if (ret)
		goto remove_max22200;

	ret = max22200_set_ch_freq(max22200_desc, 1, MAX22200_FREQMAIN_DIV_2);
	if (ret)
		goto remove_max22200;

	for (i = 0; i < MAX22200_CHANNELS; i++) {
		ret = max22200_get_ch_mode(max22200_desc, i, &chan_drive,
					   &chan_side, &chan_op_mode);
		if (ret)
			goto remove_max22200;

		ret = max22200_get_ch_freq(max22200_desc, i, &chan_freq);
		if (ret)
			goto remove_max22200;

		pr_info("For channel %d : \n", i);
		pr_info("Channel drive : %s\n", ch_drive[i]);
		pr_info("Channel side : %s\n", ch_side[i]);
		pr_info("Channel operation mode: %s\n", ch_op_mode[i]);
		pr_info("Frequency : %dKHz\n", chop_frequency[chop_freq] / (chan_freq + 1));
	}

	ret = max22200_set_ch_enable(max22200_desc, 0, MAX22200_HHF_ENABLE, 0);
	if (ret)
		goto remove_max22200;

	ret = max22200_set_cfg_dpm(max22200_desc, 0, 0);
	if (ret)
		goto remove_max22200;

	ret = max22200_fault_mask_set(max22200_desc, MAX22200_M_UVM, true);

remove_max22200:
	max22200_remove(max22200_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}

/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
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

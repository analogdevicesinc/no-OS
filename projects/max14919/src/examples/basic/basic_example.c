/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "max14919.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

int basic_example_main()
{
	struct max14919_desc *desc;
	enum max14919_out_state state[4] = {
		MAX14919_OUT_OFF,
		MAX14919_OUT_ON,
		MAX14919_OUT_OFF,
		MAX14919_OUT_OFF
	};
	int ret;
	uint8_t gpio_state;

	ret = max14919_init(&desc, &max14919_ip);
	if (ret)
		goto exit;

	ret = max14919_set_out(desc, state);
	if (ret)
		goto free_max14919;

	pr_info("MAX14919 output channel 2 is ON.\n");

	ret = max14919_get_fault(desc, &gpio_state);
	if (ret)
		goto free_max14919;

	switch (gpio_state) {
	case 0:
		pr_info("MAX14919 detected faults.\n");
		break;
	case 1:
		pr_info("MAX14919 detected no faults.\n");
		break;
	default:
		goto free_max14919;
	}

	ret = max14919_detect_rev(desc, &gpio_state);
	if (ret)
		goto free_max14919;

	switch (gpio_state) {
	case 0:
		pr_info("Current polarity is normal.\n");
		break;
	case 1:
		pr_info("Current polarity is reversed.\n");
		break;
	default:
		goto free_max14919;
	}

	ret = max14919_set_climit(desc, true);
	if (ret)
		goto free_max14919;

	pr_info("2x Current Limiting has been activated.");

free_max14919:
	max14919_remove(desc);
exit:
	if (ret)
		pr_info("Error!\n");

	return ret;
}

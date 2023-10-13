/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example sources file for max22190.
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
#include "max22190.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

/**
 * @brief Basic example main execution
 * @return ret - Result of the example execution. If working correctly, will
 * 		 show the user for each channels if there is digital input
 * 		 present or not. It also changes delay values for channel 3 and
 * 		 sets the state for channel 0 to OFF. If all these operations
 * 		 were performed correctly 0 will be returned.
*/
int basic_example_main()
{
	int ret, i;
	struct max22190_desc *max22190_desc;
	uint32_t clrf, fbp;
	bool enabled;
	enum max22190_delay delay;
	uint32_t rand_reg;

	ret = max22190_init(&max22190_desc, &max22190_ip);
	if (ret)
		goto exit;

	for(i = 0; i < MAX22190_CHANNELS; i++) {
		ret = max22190_reg_read(max22190_desc, MAX22190_DIGITAL_INPUT_REG, &rand_reg);
		if (ret)
			goto remove_max22190;

		if (no_os_field_get(NO_OS_BIT(i), rand_reg))
			pr_info("Channel %d has digital input.\n", i);
		else
			pr_info("Channel %d doesn't have digital input.\n", i);
	}

	ret = max22190_filter_set(max22190_desc, 3, 0, 1, MAX22190_DELAY_50US);
	if (ret)
		goto remove_max22190;
	no_os_udelay(50);

	ret = max22190_filter_get(max22190_desc, 3, &clrf, &fbp, &delay);
	if (ret)
		goto remove_max22190;

	ret = max22190_chan_state(max22190_desc, 0, MAX22190_CH_OFF);
	if (ret)
		goto remove_max22190;

	ret = max22190_fault_enable_set(max22190_desc, MAX22190_FAULT2_FAULT8CKE, true);
	if (ret)
		goto remove_max22190;

	ret = max22190_fault_enable_get(max22190_desc, MAX22190_FAULT1_FAULT2E,
					&enabled);
	if (ret)
		goto remove_max22190;

	if (enabled)
		pr_info("Fault 2 bit in FAULT1_EN register was set correctly because of enabling a bit in FAULT2_EN register.\n");
	else
		pr_info("Fault 2 bit in FAULT1_EN register was not set correctly because of enabling a bit in FAULT2_EN register.\n");

	ret = max22190_wbe_set(max22190_desc, 3, true);
	if (ret)
		goto remove_max22190;

	for(i = 0; i < MAX22190_CHANNELS; i++) {
		ret = max22190_wbe_get(max22190_desc, i, &enabled);
		if (ret)
			goto remove_max22190;

		if (enabled)
			pr_info("Channel %d has wire-break detection enabled.\n", i);
		else
			pr_info("Channel %d has wire-break detection disabled.\n", i);
	}

remove_max22190:
	max22190_remove(max22190_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}

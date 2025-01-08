/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example sources file for ades1754.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "no_os_print_log.h"

#define ADES1754_CELL_RESOLUTION		16383.0f

static const char* const ades1754_alert_msg[] = {
	[ADES1754_CELL_OV] = "Cell Overvoltage Alert!\n",
	[ADES1754_CELL_UV] = "Cell Undervoltage Alert!\n",
	[ADES1754_BIPOLAR_OV] = "Bipolar Overvoltage Alert!\n",
	[ADES1754_BIPOLAR_UV] = "Bipolar Undervoltage Alert!\n",
	[ADES1754_BLOCK_OV] = "Block Overvoltage Alert!\n",
	[ADES1754_BLOCK_UV] = "Block Undervoltage Alert!\n",
	[ADES1754_CELL_MISMATCH] = "Cell Mismatch Alert!\n",
	[ADES1754_AUXIN_OV] = "Aux IN Overvoltage Alert!\n",
	[ADES1754_AUXIN_UV] = "Aux IN Undervoltage Alert!\n"
};

/**
 * @brief Basic example main execution
 * @return ret - Result of the example execution.
*/
int example_main()
{
	struct ades1754_desc *ades1754_desc;
	bool alert = false, enable;
	int32_t cell_voltage;
	float real_voltage;
	int ret, i;

	ret = ades1754_init(&ades1754_desc, &ades1754_ip);
	if (ret)
		goto exit;

	/* Continous CELL Voltage reading, and checkings for ALERT. */
	while (!alert) {
		for (i = 0; i < 15; i++) {
			ret = ades1754_get_cell_data(ades1754_desc, i,
						     &cell_voltage);
			if (ret)
				goto remove_ades1754;

			real_voltage = cell_voltage;
			real_voltage = real_voltage * 5.0f / ADES1754_CELL_RESOLUTION;
			pr_info("Cell %d Voltage : %0.5fV\n", i, real_voltage);
		}

		for (i = 0; i < 9; i++) {
			ret = ades1754_get_alert(ades1754_desc, i, &enable);
			if (ret)
				goto remove_ades1754;

			if (enable) {
				pr_info("%s", ades1754_alert_msg[i]);
				alert = true;
			}

		}

		if (alert)
			pr_info("Alert detected, solve alert related failures and start again!\n");
		else
			no_os_mdelay(500);
	}

remove_ades1754:
	ades1754_remove(ades1754_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}

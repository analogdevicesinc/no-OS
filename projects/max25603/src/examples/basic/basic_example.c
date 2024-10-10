/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "basic_example.h"
#include "common_data.h"
#include "max25603.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

int basic_example_main()
{
	struct max25603_desc *max25603_desc;
	int ret;

	ret = max25603_init(&max25603_desc, &max25603_ip);
	if (ret)
		goto exit;

	ret = max25603_sel_comp(max25603_desc, MAX25603_COMP1);
	if (ret)
		goto remove_max25603;

	ret = max25603_set_enable(max25603_desc, MAX25603_EN2, 100000, 100);
	if (ret)
		goto remove_max25603;

	ret = max25603_set_beam(max25603_desc, MAX25603_LOW_BEAM, 50, 50);
	if (ret)
		goto remove_max25603;

	no_os_mdelay(5000);

	ret = max25603_set_beam(max25603_desc, MAX25603_DISABLE_BEAM, 0, 0);
	if (ret)
		goto remove_max25603;

	ret = max25603_set_enable(max25603_desc, MAX25603_DISABLE_EN, 0, 0);
	if (ret)
		goto remove_max25603;

remove_max25603:
	max25603_remove(max25603_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}

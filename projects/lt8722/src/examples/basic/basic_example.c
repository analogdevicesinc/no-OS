/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for lt8722 project.
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
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
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "basic_example.h"
#include "common_data.h"
#include "lt8722.h"

int basic_example_main()
{
	int ret;
	double voltage;
	struct lt8722_dev *lt8722_dev;

	pr_info("\nBasic example");

	ret = lt8722_init(&lt8722_dev, &lt8722_ip);
	if (ret)
		goto exit;

	// /* Set output voltage */
	ret = lt8722_set_output_voltage(lt8722_dev, 2.5);
	if (ret)
		goto exit;

	ret = lt8722_get_output_voltage(lt8722_dev, &voltage);
	if (ret)
		goto exit;

	return 0;

exit:
	if (ret)
		pr_err("Error!\n");

	lt8722_remove(lt8722_dev);

	return ret;
}

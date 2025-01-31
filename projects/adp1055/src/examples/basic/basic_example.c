/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for adp1055 project.
 *   @author Ivan Gil Mercano (Ivangil.mercano@analog.com)
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
#include "no_os_i2c.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_pwm.h"
#include "adp1055.h"

int example_main()
{
	int ret;

	struct adp1055_desc *adp1055_desc;
	uint8_t data = 120;
	uint8_t data2[2];
	uint16_t vout;
	int16_t val;

	pr_info("INIT: ");
	ret = adp1055_init(&adp1055_desc, &adp1055_ip);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("VOUT SCALE: ");
	ret = adp1055_vout_scale(adp1055_desc, -12, 341);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("VOUT VALUE: ");
	ret = adp1055_vout_value(adp1055_desc, 0x3000, 0x5000);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("PWM CONFIG: ");
	ret = adp1055_pwm_config(adp1055_desc, 0x00DE, 0x000E, 0, 0, ADP1055_OUTA);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("SET PWM: ");
	ret = adp1055_set_pwm(adp1055_desc, ADP1055_OUTA, -4, 782);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("READ VALUE: ");
	ret = adp1055_read_value(adp1055_desc, ADP1055_VALUE_DUTY_CYCLE, &vout, &data);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	/* Checking statuses. */
	pr_info("READ STAT VOUT: ");
	ret = adp1055_read(adp1055_desc, ADP1055_STATUS_VOUT, &data, 1);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("READ STAT INPUT: ");
	ret = adp1055_read(adp1055_desc, ADP1055_STATUS_INPUT, &data, 1);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("READ STAT CML: ");
	ret = adp1055_read(adp1055_desc, ADP1055_STATUS_CML, &data, 1);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

	pr_info("READ STAT WORD: ");
	ret = adp1055_read(adp1055_desc, ADP1055_STATUS_WORD, data2, 2);
	if (ret)
		pr_info("Fail \n");
	else
		pr_info("Pass \n");

exit:
	if (ret)
		pr_info("Error!\n");
	adp1055_remove(adp1055_desc);
	return ret;
}

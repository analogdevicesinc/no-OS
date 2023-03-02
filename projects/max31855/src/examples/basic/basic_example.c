/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for max31855 project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "max31855.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct max31855_decimal thermocouple_temp;
	struct max31855_decimal ambient_temp;
	struct max31855_dev *dev;
	int ret;

	ret = max31855_init(&dev, &max31855_ip);
	if (ret)
		goto error;

	while (1) {
		pr_info("%c", 27);
		pr_info("%c", '[');
		pr_info("%c", '2');
		pr_info("%c", 'J');

		ret = max31855_read_temp(dev, &thermocouple_temp, &ambient_temp);
		if (ret)
			goto free_dev;

		pr_info("Thermocouple temperature %.02d.%.02d\n", thermocouple_temp.integer,
			thermocouple_temp.decimal);
		pr_info("Ambient temperature %.02d.%.02d\n", ambient_temp.integer,
			ambient_temp.decimal);

		no_os_mdelay(2000);
	}

free_dev:
	max31855_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}

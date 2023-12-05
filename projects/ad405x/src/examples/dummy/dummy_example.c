/***************************************************************************//**
 *   @file   dummy_example.c
 *   @brief  DUMMY example header for ad450x project
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "dummy_example.h"
#include "common_data.h"
#include "ad405x.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int dummy_example_main()
{
	struct no_os_i3c_master_desc *i3c_master;
	struct ad405x_dev *ad405x_desc;
	//struct other_device_dev *other_device_desc;
	int ret;

	// Init bus in memory, referencing the peripherals and doing the DAA
	ret = no_os_i3c_init(&i3c_master, i3c_ops, 1);
	// Do dynamic address assignment
	ret = no_os_i3c_do_daa(i3c_master);
	// Bring-up the peripherals drivers
	ret = ad405x_init(&ad405x_desc, ad405x_ip);
	//ret = other_device_init(&other_device_desc, other_device_ip);


	if (ret)
		goto error;
	ret = ad405x_soft_reset(ad405x_desc);

	uint32_t data[32] = {0};
	union ad405x_sts_reg_flags status_flags = {0};

	while(1) {

		pr_info("Single read \n");
		ret = ad405x_get_raw(ad405x_desc,&data[0]);
		if (ret)
			goto error;
		pr_info(" x=%d", (int)data[0]);

		pr_info("==========================================================\n");
		ret = ad405x_get_sts_reg(ad405x_desc, &status_flags);
		if (ret)
			goto error;
		pr_info("Activity flag = %d \n", (uint8_t)(status_flags.fields.Activity));
		pr_info("DATA_RDY flag = %d \n", (uint8_t)(status_flags.fields.DATA_RDY));
		pr_info("===========================================================\n");

		if (ret)
			goto error;

		no_os_mdelay(1000);
	}

error:
	pr_info("Error!\n");
	return 0;
}

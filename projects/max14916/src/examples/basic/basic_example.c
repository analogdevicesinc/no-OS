/***************************************************************************//**
 *   @file   max14916/src/examples/basic/basic_example.c
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
#include "max149x6-base.h"
#include "max14916.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

/**
 * @brief Basic example main execution
 * @return ret - Result of the example execution. If working correctly, will
 * 		 turn the status led's on and off in the while loop, set some
 * 		 values in the config 2 register, and then return 0.
*/
int basic_example_main()
{
	int ret, i;
	int j = 0;
	struct max149x6_desc *max14916_desc;
	uint32_t reg_val;
	enum max14916_wd wd = MAX14916_WD_600MS;
	enum max14916_ow_off_cs ow_off_cs = MAX14916_OW_OFF_CS_300UA;
	enum max14916_sht_vdd_thr sht_vdd_thr = MAX14916_SHT_VDD_THR_14V;

	/* Intializing the device MAX14916. */
	ret = max14916_init(&max14916_desc, &max14916_ip);
	if(ret)
		goto exit;

	/* Create a loop that will turn SLEDs on and off like in a "loading screen"
	   sequence. */
	while (j < 5) {
		for(i = 0; i < MAX14916_CHANNELS; i++) {
			ret = max14916_sled_set(max14916_desc, i, MAX14916_SLED_ON);
			if (ret)
				goto remove_max14916;
			no_os_mdelay(200);
		}

		for(i = 0; i < MAX14916_CHANNELS; i++) {
			ret = max14916_sled_set(max14916_desc, i, MAX14916_SLED_OFF);
			if (ret)
				goto remove_max14916;
			no_os_mdelay(200);
		}
		j++;
	}

	/* Set FLED time. */
	ret = max14916_fled_time_set(max14916_desc, MAX14916_FLED_TIME_2S);
	if (ret)
		goto remove_max14916;

	/* Verify the FLED time set. */
	ret = max149x6_reg_read(max14916_desc, MAX14916_CONFIG1_REG, &reg_val);
	if (ret)
		goto remove_max14916;

	reg_val = no_os_field_get(MAX14916_FLED_STRETCH_MASK, reg_val);

	pr_info("FLED TIME = %d\n", reg_val);

	/* Set Watch-Dog Timer. */
	ret = max14916_set_wd(max14916_desc, wd);
	if (ret)
		goto remove_max14916;

	/* Set Open Wire OFF current source. */
	ret = max14916_set_ow_off(max14916_desc, ow_off_cs);
	if (ret)
		goto remove_max14916;

	/* Set Short to VDD Threshold. */
	ret = max14916_set_sht_vdd_thr(max14916_desc, sht_vdd_thr);
	if (ret)
		goto remove_max14916;

	/* Reading Config 2 to check if the mentioned above values have been set
	   correctly. */
	ret = max149x6_reg_read(max14916_desc, MAX14916_CONFIG2_REG, &reg_val);
	if (ret)
		goto remove_max14916;

	pr_info("CONFIG2 = 0x%x\n", reg_val);

remove_max14916:
	max14916_remove(max14916_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}

/***************************************************************************//**
 *   @file   digital_input_logic.c
 *   @brief  Digital Input Logic example code for ad74416h-pmdz project
 *   @author Raquel Grau (raquel.grau@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "digital_input_logic.h"
#include "common_data.h"
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Digital Input Logic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int digital_input_logic_example_main()
{
	struct ad74416h_desc *ad74416h_desc;
	int ret;

	uint16_t reg_value = 0xAAAA;
	uint32_t dac_code = 0;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure Channel A as Digital Input Logic mode
	ret = ad74416h_set_channel_function(ad74416h_desc, 0, AD74416H_DIGITAL_INPUT);
	if (ret) {
		pr_info("Error setting Channel 0 as digital input logic mode\r\n");
		goto error_ad74416h;
	}

	//Uncomment the desired function depending on which type of DI is desired
	ret = configure_di_type1_3(ad74416h_desc, 0);
	//ret = configure_di_type2(ad74416h_desc, 0);
	if(ret){
		pr_info("Error configuring the DI type\r\n");
		goto error_ad74416h;
	}


error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error!\r\n");
	return 0;
}

/**********************************************************************************
 * @brief This function configures the digital input as a Type1/Type3 Digital input
 * @param desc - The device structure
 * @return ret - Result of the execution.
**********************************************************************************/
int configure_di_type1_3(struct ad74416h_desc *desc, uint32_t ch)
{
	int ret;
	//Set a fixed threshold mode
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch), AD74416H_DIN_THRESH_MODE_MSK, 1);
	if(ret)
		return ret;
	//Set the threshold to 8.5V
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch), AD74416H_COMP_THRESH_MSK, 0x37);
	if(ret)
		return ret;
	//Configure the current sink to 2.4mA
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch), AD74416H_DIN_SINK_RANGE_MSK, 0);
	if(ret)
		return ret;
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch), AD74416H_DIN_SINK_MSK, 0x14);
	if(ret)
		return ret;
	return 0;
}

/**********************************************************************************
 * @brief This function configures the digital input as a Type2 Digital input
 * @param desc - The device structure
 * @return ret - Result of the execution.
**********************************************************************************/
int configure_di_type2(struct ad74416h_desc *desc, uint32_t ch)
{
        int ret;
        //Set a fixed threshold mode
        ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch), AD74416H_DIN_THRESH_MODE_MSK, 1);
        if(ret)
                return ret;
        //Set the threshold to 8V
        ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch), AD74416H_COMP_THRESH_MSK, 0x37);
        if(ret)
                return ret;
        //Configure the current sink to 6.96mA
        ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch), AD74416H_DIN_SINK_RANGE_MSK, 1);
        if(ret)
                return ret;
        ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch), AD74416H_DIN_SINK_MSK, 0x1D);
        if(ret)
                return ret;
        return 0;
}


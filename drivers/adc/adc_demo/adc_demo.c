/***************************************************************************//**
 * @file adc_demo.c
 * @brief Implementation of ADC Demo Driver.
 * @author RNechita (ramona.nechita@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "adc_demo.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/* default sine lookup table to be used if ext_buff is not available */
const uint16_t sine_lut[128] = {
	0x0000, 0x0064, 0x00C8, 0x012C, 0x018F, 0x01F1, 0x0252, 0x02B1,
	0x030F, 0x036B, 0x03C5, 0x041C, 0x0471, 0x04C3, 0x0512, 0x055F,
	0x05A7, 0x05ED, 0x062E, 0x066C, 0x06A6, 0x06DC, 0x070D, 0x073A,
	0x0763, 0x0787, 0x07A7, 0x07C2, 0x07D8, 0x07E9, 0x07F5, 0x07FD,
	0x07FF, 0x07FD, 0x07F5, 0x07E9, 0x07D8, 0x07C2, 0x07A7, 0x0787,
	0x0763, 0x073A, 0x070D, 0x06DC, 0x06A6, 0x066C, 0x062E, 0x05ED,
	0x05A7, 0x055F, 0x0512, 0x04C3, 0x0471, 0x041C, 0x03C5, 0x036B,
	0x030F, 0x02B1, 0x0252, 0x01F1, 0x018F, 0x012C, 0x00C8, 0x0064,
	0x0000, 0xFF9B, 0xFF37, 0xFED3, 0xFE70, 0xFE0E, 0xFDAD, 0xFD4E,
	0xFCF0, 0xFC94, 0xFC3A, 0xFBE3, 0xFB8E, 0xFB3C, 0xFAED, 0xFAA0,
	0xFA58, 0xFA12, 0xF9D1, 0xF993, 0xF959, 0xF923, 0xF8F2, 0xF8C5,
	0xF89C, 0xF878, 0xF858, 0xF83D, 0xF827, 0xF816, 0xF80A, 0xF802,
	0xF800, 0xF802, 0xF80A, 0xF816, 0xF827, 0xF83D, 0xF858, 0xF878,
	0xF89C, 0xF8C5, 0xF8F2, 0xF923, 0xF959, 0xF993, 0xF9D1, 0xFA12,
	0xFA58, 0xFAA0, 0xFAED, 0xFB3C, 0xFB8E, 0xFBE3, 0xFC3A, 0xFC94,
	0xFCF0, 0xFD4E, 0xFDAD, 0xFE0E, 0xFE70, 0xFED3, 0xFF37, 0xFF9B
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief init function for the adc demo driver
 * @param desc - descriptor for the adc
 * @param param - initialization param for adc
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adc_demo_init(struct adc_demo_desc **desc,
		      struct adc_demo_init_param *param)
{
	struct adc_demo_desc *adesc;
	adesc = (struct adc_demo_desc*)no_os_calloc(1, sizeof(*adesc));

	if(!adesc)
		return -ENOMEM;

	adesc->ext_buff = param->ext_buff;
	adesc->ext_buff_len = param->ext_buff_len;
	for(int i = 0; i < TOTAL_ADC_CHANNELS; i++)
		adesc->adc_ch_attr[i] = param->dev_ch_attr[i];
	adesc->adc_global_attr = param->dev_global_attr;
	*desc = adesc;

	return 0;
}

/**
 * @brief free allocated resources
 * @param desc - descriptor for the adc
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adc_demo_remove(struct adc_demo_desc *desc)
{
	if(!desc)
		return -EINVAL;

	no_os_free(desc);

	return 0;
}

/**
 * @brief active adc channels
 * @param dev - descriptor for the adc
 * @param mask - active channels mask
 * @return 0 in case of success, -1 otherwise.
 */
int32_t update_adc_channels(void *dev, uint32_t mask)
{
	struct adc_demo_desc *desc;

	if(!dev)
		return -ENODEV;

	desc = dev;

	desc->active_ch = mask;
	/* If a real device. Here needs to be selected the channels to be read*/

	return 0;
}

/**
 * @brief close all channels
 * @param dev - physical instance of an adc device
 * @return 0 in case of success.
 */
int32_t close_adc_channels(void* dev)
{
	struct adc_demo_desc *desc;

	if(!dev)
		return -ENODEV;

	desc = dev;

	desc->active_ch = 0;

	return 0;
}

/**
 * @brief read function for the adc demo driver
 * @param desc - descriptor for the adc
 * @param reg_index - the address at which we want to read
 * @param readval- the value read from register
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adc_demo_reg_read(struct adc_demo_desc *desc, uint8_t reg_index,
			  uint8_t *readval)
{
	if(!desc || reg_index >= NO_OS_ARRAY_SIZE(desc->reg))
		return -EINVAL;

	*readval = desc->reg[reg_index];

	return 0;
}

/**
 * @brief write function for the adc demo driver
 * @param desc - descriptor for the adc
 * @param reg_index - the address at which we want to write
 * @param writeval - the value to be written
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adc_demo_reg_write(struct adc_demo_desc *desc, uint8_t reg_index,
			   uint8_t writeval)
{

	if(!desc || reg_index >= NO_OS_ARRAY_SIZE(desc->reg))
		return -EINVAL;

	desc->reg[reg_index] = writeval;

	return 0;
}

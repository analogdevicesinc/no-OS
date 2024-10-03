/***************************************************************************//**
*   @file   adf4156_cfg.h
*   @brief  Header file of ADF4156 Driver Configuration.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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

#ifndef __AD4156_CFG_H__
#define __AD4156_CFG_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adf4156.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
static struct adf4156_platform_data adf4156_pdata_lpc = {
	10000000,                   //clkin in Hz
	20000,                      //channel_spacing in Hz
	1,                          //ref_doubler_en
	0,                          //ref_div2_en
	ADF4156_INT_VAL(23),        //r0_user_settings
	ADF4156_MOD_WORD(2),        //r2_user_settings
	ADF4156_PD_POL(1),          //r3_user_settings
	0,                          //r4_user_settings
};

#endif // __AD4156_CFG_H__

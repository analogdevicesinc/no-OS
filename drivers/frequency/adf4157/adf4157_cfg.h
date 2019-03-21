/***************************************************************************//**
*   @file   ADF4157_cfg.h
*   @brief  Header file of ADF4157 Driver Configuration.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

#ifndef __AD4157_CFG_H__
#define __AD4157_CFG_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adf4157.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
static struct adf4157_platform_data adf4157_pdata_lpc = {
	10000000,                   //clkin
	0,                          //ref_doubler_en
	0,                          //ref_div2_en
	ADF4157_INT_VAL(23),        //r0_user_settings
	0,                          //r2_user_settings
	ADF4157_PD_POL(1),          //r3_user_settings
	0,                          //r4_user_settings
};

#endif // __AD4157_CFG_H__

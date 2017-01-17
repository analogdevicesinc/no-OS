/***************************************************************************//**
* @file xcvr.c
* @brief Implementation of xcvr Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2016(c) Analog Devices, Inc.
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
#include "xcvr_core.h"

/***************************************************************************//**
* @brief xcvr_read
*******************************************************************************/
int32_t xcvr_read(xcvr_core core,
					uint32_t reg_addr,
					uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core.base_address + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief xcvr_write
*******************************************************************************/
int32_t xcvr_write(xcvr_core core,
					 uint32_t reg_addr,
					 uint32_t reg_data)
{
	ad_reg_write((core.base_address + reg_addr), reg_data);

	return 0;
}

/*******************************************************************************
* @brief xcvr_init
*******************************************************************************/

int32_t xcvr_init(xcvr_core core)
{
	uint32_t status;
	uint32_t timeout;

	xcvr_write(core, XCVR_REG_RESETN, 0);

	xcvr_write(core, XCVR_REG_CONTROL,
				 ((core.lpm_enable ? XCVR_LPM_DFE_N : 0) |
				  XCVR_SYSCLK_SEL(core.sys_clk_sel) |
				  XCVR_OUTCLK_SEL(core.out_clk_sel)));

	xcvr_write(core, XCVR_REG_RESETN, XCVR_RESETN);

	timeout = 100;
	do {
		mdelay(1);
		xcvr_read(core,XCVR_REG_STATUS, &status);
	} while ((timeout--) && (status == 0));

	if (status) {
		ad_printf("XCVR successfully initialized.\n");

		return 0;
	} else {
		ad_printf("XCVR initialization error.\n");

		return -1;
	}
}

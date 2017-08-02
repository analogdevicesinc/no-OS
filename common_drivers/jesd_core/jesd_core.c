/***************************************************************************//**
* @file jesd_core.c
* @brief Implementation of Jesd Core.
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
#include "jesd_core.h"

/***************************************************************************//**
* @brief jesd_read
*******************************************************************************/
int32_t jesd_read(jesd_core core,
					uint32_t reg_addr,
					uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core.base_address + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief jesd_write
*******************************************************************************/
int32_t jesd_write(jesd_core core,
					uint32_t reg_addr,
					uint32_t reg_data)
{
	ad_reg_write((core.base_address + reg_addr), reg_data);

	return 0;
}


/***************************************************************************//**
* @brief jesd_init
*******************************************************************************/
int32_t jesd_setup(jesd_core core)
{
	jesd_write(core, 0x210, (((core.octets_per_frame-1) << 16) |
		((core.frames_per_multiframe*core.octets_per_frame)-1)));
	jesd_write(core, 0x0c0, 0);
	return(0);
}

/***************************************************************************//**
* @brief jesd generate SYSREF if necessar
*******************************************************************************/
int32_t jesd_sysref_control(jesd_core core, uint32_t enable)
{
	if ((core.sysref_type == INTERN) && (core.subclass_mode >= 1)) {

		// generate SYS_REF

		ad_gpio_set(core.sysref_gpio_pin, enable);
		mdelay(10);
	}
	return 0;
}
/***************************************************************************//**
* @brief jesd_read_status
*******************************************************************************/
int32_t jesd_status(jesd_core core)
{
	uint32_t status;
	int32_t timeout;
	int32_t ret;

	ret = 0;
	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		jesd_read(core, 0x280, &status);
		if ((status & 0x13) == 0x13) break;
		timeout = timeout - 1;
	}
	if ((status & 0x10) != 0x10) {
		ad_printf("%s jesd_status: out of sync (%x)!\n", __func__, status);
		ret = -1;
	}
	if ((status & 0x03) != 0x03) {
		ad_printf("%s jesd_status: not in data phase (%x)!\n", __func__, status);
		ret = -1;
	}
	return(ret);
}

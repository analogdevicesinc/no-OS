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

#ifdef XILINX

/***************************************************************************//**
 * @brief xcvr_drp_read Dynamic reconfiguration port read access for Xilinx devices
 *******************************************************************************/
int32_t xcvr_drp_read(xcvr_core core, uint8_t drp_sel,
				uint32_t drp_addr,
				uint32_t *drp_data)
{
	uint32_t timeout = 20;
	uint32_t val = 0;

	xcvr_write(core, drp_sel ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL, XCVR_BROADCAST);

	xcvr_write(core, drp_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL,
				 XCVR_CM_ADDR(drp_addr));

	do {
		xcvr_read(core, drp_sel ? XCVR_REG_CH_STATUS : XCVR_REG_CM_STATUS, &val);
		if (val & (drp_sel ? XCVR_CH_BUSY : XCVR_CM_BUSY)) {
			mdelay(1);
			continue;
		}

		*drp_data = drp_sel ? XCVR_CH_RDATA(val) : XCVR_CM_RDATA(val);
		return 0;
	} while (timeout--);

	xil_printf("%s: Timeout!\n", __func__);
	return -1;
}

/***************************************************************************//**
 * @brief xcvr_drp_write Dynamic reconfiguration port write access for Xilinx devices
 *******************************************************************************/
int32_t xcvr_drp_write(xcvr_core core, uint8_t drp_sel,
				uint32_t drp_addr,
				uint32_t drp_data)
{
	uint32_t timeout = 20;
	uint32_t val = 0;

	xcvr_write(core, drp_sel ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL, XCVR_BROADCAST);

	xcvr_write(core, drp_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL,
				 XCVR_CM_ADDR(drp_addr) | XCVR_CM_WDATA(drp_data));

	do {
		xcvr_read(core, drp_sel ? XCVR_REG_CH_STATUS : XCVR_REG_CM_STATUS, &val);
		if (val & (drp_sel ? XCVR_CH_BUSY : XCVR_CM_BUSY)) {
			mdelay(1);
			continue;
		}

		return 0;
	} while (timeout--);

	xil_printf("%s: Timeout!\n", __func__);
	return -1;
}

#endif

/*******************************************************************************
 * @brief xcvr_init
 *******************************************************************************/

int32_t xcvr_setup(xcvr_core core)
{
	uint32_t status;
	int32_t timeout;

#ifdef ALTERA
	if (core.rx_tx_n == 0)
	{
		ad_reg_write((core.tx_lane_pll_base_address + 0x000), 0x02);
		timeout = 100;
		while (timeout > 0)
		{
			mdelay(1);
			timeout = timeout - 1;
			status = ad_reg_read(core.tx_lane_pll_base_address + 0xa00);
			if ((status & 0x04) == 0x00) break;
		}
		if ((status & 0x04) != 0x00)
		{
			ad_printf("%s ERROR: xcvr calibration failed!\n", __func__);
			return(-1);
		}
		status = ad_reg_read(core.tx_lane_pll_base_address + 0x400);
		ad_reg_write((core.tx_lane_pll_base_address + 0x400), (status | 0x01));
		ad_reg_write((core.tx_lane_pll_base_address + 0x000), 0x01);
		timeout = 100;
		while (timeout > 0)
		{
			mdelay(1);
			timeout = timeout - 1;
			status = ad_reg_read(core.tx_lane_pll_base_address + 0xa00);
			if ((status & 0x03) == 0x01) break;
		}
		if ((status & 0x03) != 0x01)
		{
			ad_printf("%s ERROR: xcvr calibration failed!\n", __func__);
			return(-1);
		}
	}
#endif

	xcvr_write(core, XCVR_REG_RESETN, 0);
	if (core.reconfig_bypass == 0)
	{
		ad_printf("ERROR: XCVR reconfiguration is NOT supported yet!\n");
		return(-1);
	}

	xcvr_write(core, XCVR_REG_RESETN, XCVR_RESETN);

	timeout = 100;
	while (timeout > 0)
	{
		mdelay(1);
		timeout = timeout - 1;
		xcvr_read(core, XCVR_REG_STATUS, &status);
		if (status == 1)
			break;
	}

	if (status == 0)
	{
		ad_printf("ERROR: XCVR initialization failed!\n");
		return(-1);
	}

	return(0);
}

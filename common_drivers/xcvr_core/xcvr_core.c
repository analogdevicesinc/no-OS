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

/***************************************************************************//**
 * @brief xcvr_drp_read Dynamic reconfiguration port read access for Xilinx devices
 *******************************************************************************/
int32_t xcvr_drp_read(xcvr_core core, uint8_t cm_ch_n, uint8_t drp_sel,
	uint32_t drp_addr, uint32_t *drp_data) {

#ifdef XILINX
	int32_t timeout = 20;
	uint32_t val = 0;

	xcvr_write(core, ((cm_ch_n == XCVR_DRP_CHANNEL) ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL), drp_sel);
	xcvr_write(core, ((cm_ch_n == XCVR_DRP_CHANNEL) ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL), XCVR_DRP_ADDR(drp_addr));
	while (timeout > 0) {
		xcvr_read(core, ((cm_ch_n == XCVR_DRP_CHANNEL) ? XCVR_REG_CH_STATUS : XCVR_REG_CM_STATUS), &val);
		*drp_data = XCVR_DRP_RDATA(val);
		if (XCVR_DRP_BUSY(val) == 0) return(0);
		timeout = timeout - 1;
	}
	ad_printf("%s: Timeout!\n", __func__);
	return (-1);
#endif

#ifdef ALTERA
	return(-1);
#endif
}

/***************************************************************************//**
 * @brief xcvr_drp_write Dynamic reconfiguration port write access for Xilinx devices
 *******************************************************************************/
int32_t xcvr_drp_write(xcvr_core core, uint8_t cm_ch_n, uint8_t drp_sel,
	uint32_t drp_addr, uint32_t drp_data) {

#ifdef XILINX
	int32_t timeout = 20;
	uint32_t val = 0;

	xcvr_write(core, ((cm_ch_n == XCVR_DRP_CHANNEL) ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL), drp_sel);
	xcvr_write(core, ((cm_ch_n == XCVR_DRP_CHANNEL) ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL), (XCVR_DRP_WR |
		XCVR_DRP_ADDR(drp_addr) | XCVR_DRP_WDATA(drp_data)));
	while (timeout > 0) {
		xcvr_read(core, ((cm_ch_n == XCVR_DRP_CHANNEL) ? XCVR_REG_CH_STATUS : XCVR_REG_CM_STATUS), &val);
		if (XCVR_DRP_BUSY(val) == 0) return(0);
		timeout = timeout - 1;
	}
	ad_printf("%s: Timeout!\n", __func__);
	return (-1);
#endif

#ifdef ALTERA
	return(-1);
#endif
}

/*******************************************************************************
 * @brief xcvr_setup
 *******************************************************************************/
int32_t xcvr_setup(xcvr_core core)
{

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

  	xcvr_write(core, 0x420, 0x1);
  	xcvr_write(core, 0x424, 0x1);
	mdelay(1);
	xcvr_write(core, 0x420, 0x0);
	xcvr_write(core, 0x424, 0x0);
	mdelay(10);
	mdelay(10);
	return(0);
}

/*******************************************************************************
 * @brief xcvr_getconfig
 *******************************************************************************/
int32_t xcvr_getconfig(xcvr_core *core)
{

return 0;
}

/*******************************************************************************
 * @brief xcvr_reset
 *******************************************************************************/
int32_t xcvr_reset(xcvr_core core)
{
	uint32_t status;
	int32_t timeout;

	xcvr_write(core, XCVR_REG_RESETN, 0);

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
		ad_printf("%s ERROR: XCVR initialization failed!\n", __func__);
		return(-1);
	}

	return(0);
}

/* xcvr_eyescan */

uint32_t xcvr_read_es(xcvr_core core, uint32_t addr) {

	uint32_t data;

	xcvr_read(core, (addr*4), &data);
	ad_printf("%x- %x\n", addr, data);
	return(0);
}

uint32_t xcvr_read_es_drp(xcvr_core core, uint32_t addr) {

	uint32_t data;

	xcvr_drp_read(core, XCVR_DRP_CHANNEL, core.es_lane, addr, &data);
	ad_printf("drp-%x- %x\n", addr, data);
	return(0);
}

int32_t xcvr_eyescan(xcvr_core core) {

	uint32_t data;
	uint32_t rate;
	uint32_t limit;
	uint32_t lpm_dfe_n;
	int32_t timeout;
	int32_t xmin;
	int32_t xmax;
	int32_t xdmin;
	int32_t xdmax;
	int32_t xdlast;
	int32_t xdiff;
	int32_t ymin;
	int32_t ymax;
	int32_t ydmin;
	int32_t ydmax;
	int32_t ydlast;
	int32_t hmin;
	int32_t hmax;
	int32_t vmin;
	int32_t vmax;
	int32_t x;
	int32_t y;
	int32_t a;
	int32_t d;

	/* get some basics on the messy part, you may assume this is done in rtl- */
	/* but to be safe, make sure you set them up properly yourself. */

	xcvr_read(core, XCVR_REG_CONTROL, &data);
	rate = (data >> 8) & 0xf;
	lpm_dfe_n = (data >> 12) & 0x1;
	if (rate == 0) {
		xcvr_drp_read(core, XCVR_DRP_CHANNEL, core.es_lane, 0x088, &data);
		rate = (data & 0x7) + 1;
	}
	if (lpm_dfe_n == 0) {
		ad_printf("%s ERROR: DFE mode NOT supported(%d)!\n", __func__, lpm_dfe_n);
		return(-1);
	}

	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x031, 0xffff);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x032, 0xffff);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x033, 0xffff);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x034, 0xffff);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x035, 0xffff);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x036, 0x0000);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x037, 0x0000);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x038, 0xff00);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x039, 0xffff);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x03a, 0xffff);
	xcvr_drp_read(core, XCVR_DRP_CHANNEL, core.es_lane, 0x082, &data);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x082, (data | 0x20));
	xcvr_drp_read(core, XCVR_DRP_CHANNEL, core.es_lane, 0x03d, &data);
	xcvr_drp_write(core, XCVR_DRP_CHANNEL, core.es_lane, 0x03d, (data | 0x300));

	/* set usual stuff now- keep them honest */

	if ((rate == 0) || (rate >= 6)) {
		ad_printf("%s ERROR: invalid rate (%d)!\n", __func__, rate);
		return(-1);
	}

	if (rate == 1) limit = 32;
	if (rate == 2) limit = 64;
	if (rate == 3) limit = 128;
	if (rate == 4) limit = 256;
	if (rate == 5) limit = 512;

	if (core.es_hoffset_min < (-1*limit)) core.es_hoffset_min = -1*limit;
	if (core.es_hoffset_max > limit) core.es_hoffset_max = limit;
	if (core.es_hoffset_step > limit) core.es_hoffset_step = limit;

	xcvr_write(core, XCVR_REG_ES_START_ADDRESS, core.es_start_address);
	xcvr_write(core, XCVR_REG_ES_LANE_SELECT, core.es_lane);
	xcvr_write(core, XCVR_REG_ES_PRESCALE, core.es_prescale);
	data = ((core.es_voffset_step & 0xff)<<16) | ((core.es_voffset_max & 0xff)<<8) | (core.es_voffset_min & 0xff);
	xcvr_write(core, XCVR_REG_ES_VOFFSET, data);
	data = ((core.es_hoffset_max & 0xfff)<<16) | (core.es_hoffset_min & 0xfff);
	xcvr_write(core, XCVR_REG_ES_HOFFSET, data);
	xcvr_write(core, XCVR_REG_ES_HOFFSET_STEP, (core.es_hoffset_step & 0xffff));

	/* request and poll (be patient it will take about 10 seconds */

	timeout = 10000*rate;
	xcvr_write(core, XCVR_REG_ES_REQUEST, 0x1);
	ad_printf("running eyescan, this will take a few minutes\n");
	while (timeout > 0) {
		timeout = timeout - 1;
		mdelay(1);
		xcvr_read(core, XCVR_REG_ES_REQUEST, &data);
		if (XCVR_ES_STATUS(data) == 0x0) break;
	}

	if (XCVR_ES_STATUS(data) == 0x1) {
		ad_printf("%s ERROR: timed out!\n", __func__);
		return(-1);
	}
	xcvr_read(core, XCVR_REG_ES_STATUS, &data);
	if (XCVR_ES_STATUS(data) == 0x1) {
		ad_printf("%s ERROR: data may not be valid!\n", __func__);
		return(-1);
	}

	ad_printf("eye diagram for lane %d:\n", core.es_lane);

	xdiff = 0;
	hmin = 1024;
	hmax = -1024;
	ymin = 0;
	ymax = 0;
	ydmin = 0;
	ydmax = 0;
	ydlast = -1;
	a = core.es_start_address;
	for (y = core.es_voffset_min; y <= core.es_voffset_max; y = y + core.es_voffset_step) {
		xmin = 1;
		xmax = 0;
		xdmin = 1;
		xdmax = 0;
		xdlast = 1;
		ad_printf("%5d: ", y);
		for (x = core.es_hoffset_min; x <= core.es_hoffset_max; x = x + core.es_hoffset_step) {
			if (lpm_dfe_n == 1) {
				d = (ad_reg_read(a) == 0xffff0000) ? 0 : 1;
				a = a + 0x4;
			}
			if ((d == 0) && (xdlast == 1)) xdmin = x;
			if (d == 0) xdmax = x;
			if (((d == 1) || (x == core.es_hoffset_max)) && ((xdmax-xdmin) > (xmax-xmin))) {
				xmax = xdmax;
				xmin = xdmin;
			}
			xdlast = d;
			ad_printf("%d", d);
		}
		d = (xmax - xmin) + 1;
		if (xdiff < d) xdiff = d;
		if ((d > 0) && (hmin > xmin)) hmin = xmin;
		if ((d > 0) && (hmax < xmax)) hmax = xmax;
		if ((d > 0) && (ydlast == 0)) ydmin = y;
		if (d > 0) ydmax = y;
		if (((d <= 0) || (y == core.es_voffset_max)) && ((ydmax-ydmin) > (ymax-ymin))) {
			ymax = ydmax;
			ymin = ydmin;
		}
		ydlast = d;
		ad_printf("\n");
	}
	vmin = ymin;
	vmax = ymax;
	ad_printf("eyescan levels: v(%d, %d, %d), h(%d, %d, %d)\n", vmin, vmax, (vmax-vmin+1),
		hmin, hmax, (hmax-hmin+1));
	return(0);
}


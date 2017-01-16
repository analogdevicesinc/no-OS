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
* @brief jesd204b_read
*******************************************************************************/
int32_t jesd204b_read(jesd204_core core,
					  uint32_t reg_addr,
					  uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core.base_addr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_write
*******************************************************************************/
int32_t jesd204b_write(jesd204_core core,
					   uint32_t reg_addr,
					   uint32_t reg_data)
{
	ad_reg_write((core.base_addr + reg_addr), reg_data);

	return 0;
}


/***************************************************************************//**
* @brief jesd204_init
*******************************************************************************/
int32_t jesd204_init(jesd204_core core)
{

	jesd204b_write(core, JESD204_REG_TRX_RESET,
			JESD204_TRX_GT_WDT_DIS | JESD204_TRX_RESET);
	jesd204b_write(core, JESD204_REG_TRX_ILA_SUPPORT,
			JESD204_TRX_ILA_EN);
	jesd204b_write(core, JESD204_REG_TRX_SCRAMBLING,
			JESD204_TRX_SCR_EN);
	jesd204b_write(core, JESD204_REG_TRX_SYSREF_HANDLING,
			0);
	jesd204b_write(core, JESD204_REG_TRX_OCTETS_PER_FRAME,
			JESD204_TRX_OCTETS_PER_FRAME(core.octets_per_frame));
	jesd204b_write(core, JESD204_REG_TRX_FRAMES_PER_MULTIFRAME,
			JESD204_TRX_FRAMES_PER_MULTIFRAME(core.frames_per_multiframe));
	jesd204b_write(core, JESD204_REG_TRX_SUBCLASS_MODE,
			JESD204_TRX_SUBCLASS_MODE(core.subclass_mode));

	ad_printf("JESD204 initialization done.\n");
    return 0;
}

/***************************************************************************//**
* @brief jesd204 generate SYSREF if necessar
*******************************************************************************/
int32_t jesd204_gen_sysref(jesd204_core core)
{
    if ((core.sysref_type == INTERN) && (core.subclass_mode >= 1)) {

        // generate SYS_REF

        ad_gpio_set(core.gpio_sysref, 1);
        mdelay(10);
    }
    return 0;
}
/***************************************************************************//**
* @brief jesd204_read_status
*******************************************************************************/
int32_t jesd204_read_status(jesd204_core core)
{
	uint32_t status;
	uint32_t timeout;
	uint8_t link;
	int32_t ret;

	timeout = 100;
	do {
		mdelay(1);
		jesd204b_read(core, JESD204_REG_TRX_RESET, &status);
		status &= JESD204_TRX_RESET;
	} while ((timeout--) && (status == JESD204_TRX_RESET));

	if (status == JESD204_TRX_RESET) {
		ad_printf("jesd_status: jesd reset not completed!\n");
		return -1;
	}

	if (core.subclass_mode >= 1) {
	  timeout = 100;
	  do {
		mdelay(1);
		jesd204b_read(core, JESD204_REG_TRX_SYNC_STATUS, &status);
		status &= JESD204_TRX_SYSREF_CAPTURED;
	  } while ((timeout--) && (status != JESD204_TRX_SYSREF_CAPTURED));

	  if (status != JESD204_TRX_SYSREF_CAPTURED) {
		ad_printf("jesd_status: missing SYS_REF!\n");
		return -1;
	  } else
		ad_printf("SYNC STATUS: 0x%x\n", status);
	}

	timeout = 100;
	do {
		mdelay(1);
		jesd204b_read(core, JESD204_REG_TRX_SYNC_STATUS, &status);
		status &= JESD204_TRX_SYNC_ACHIEVED;
	} while ((timeout--) && (status != JESD204_TRX_SYNC_ACHIEVED));

	if (status != JESD204_TRX_SYNC_ACHIEVED) {
		ad_printf("jesd_status: Link SYNC not achieved!\n");
		return -1;
	}

	if (core.rx_tx_n == 0)
		return 0;

	jesd204b_read(core, JESD204_REG_RX_LINK_ERROR_STATUS, &status);
	for (link = 0; link < 8; link++) {
		if (status & JESD204_RX_LINK_K_CH_ERR(link)) {
			ad_printf("Link %d: K_CH_ERR\n", link);
			ret = -1;
		}
		if (status & JESD204_RX_LINK_DISP_ERR(link)) {
			ad_printf("Link %d: DISP_ERR\n", link);
			ret = -1;
		}
		if (status & JESD204_RX_LINK_NOT_IN_TBL_ERR(link)) {
			ad_printf("Link %d: NOT_IN_TBL_ERR\n", link);
			ret = -1;
		}
	}

	if (status & JESD204_RX_LINK_LANE_ALIGN_ERR_ALARM) {
		ad_printf("jesd_status: frame alignment error!\n");
		ret = -1;
	}

	if (status & JESD204_RX_LINK_SYSREF_LMFC_ALARM) {
		ad_printf("jesd_status: sysref alignment error!\n");
		ret = -1;
	}

	if (status & JESD204_RX_LINK_BUFF_OVF_ALARM) {
		ad_printf("jesd_status: receive buffer overflow error!\n");
		ret = -1;
	}

	return ret;
}

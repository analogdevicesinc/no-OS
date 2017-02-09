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
#ifdef ALTERA
	return(0);
#endif

	jesd_write(core, JESD204_REG_TRX_RESET,
			JESD204_TRX_GT_WDT_DIS | JESD204_TRX_RESET);
	jesd_write(core, JESD204_REG_TRX_ILA_SUPPORT,
			JESD204_TRX_ILA_EN);
	jesd_write(core, JESD204_REG_TRX_SCRAMBLING,
			core.scramble_enable);
	jesd_write(core, JESD204_REG_TRX_SYSREF_HANDLING,
			0);
	jesd_write(core, JESD204_REG_TRX_OCTETS_PER_FRAME,
			JESD204_TRX_OCTETS_PER_FRAME(core.octets_per_frame));
	jesd_write(core, JESD204_REG_TRX_FRAMES_PER_MULTIFRAME,
			JESD204_TRX_FRAMES_PER_MULTIFRAME(core.frames_per_multiframe));
	jesd_write(core, JESD204_REG_TRX_SUBCLASS_MODE,
			JESD204_TRX_SUBCLASS_MODE(core.subclass_mode));

    return 0;
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
	uint32_t timeout;
	uint8_t link;
	int32_t ret;

#ifdef ALTERA
	return(0);
#endif
	timeout = 100;
	do {
		mdelay(1);
		jesd_read(core, JESD204_REG_TRX_RESET, &status);
		status &= JESD204_TRX_RESET;
	} while ((timeout--) && (status == JESD204_TRX_RESET));

	if (status == JESD204_TRX_RESET) {
		ad_printf("%s jesd_status: jesd reset not completed!\n", __func__);
		return -1;
	}

	if (core.subclass_mode >= 1) {
	  timeout = 100;
	  do {
		mdelay(1);
		jesd_read(core, JESD204_REG_TRX_SYNC_STATUS, &status);
		status &= JESD204_TRX_SYSREF_CAPTURED;
	  } while ((timeout--) && (status != JESD204_TRX_SYSREF_CAPTURED));

	  if (status != JESD204_TRX_SYSREF_CAPTURED) {
		ad_printf("%s jesd_status: missing SYS_REF!\n", __func__);
		return -1;
	  }
	}

	timeout = 100;
	do {
		mdelay(1);
		jesd_read(core, JESD204_REG_TRX_SYNC_STATUS, &status);
		status &= JESD204_TRX_SYNC_ACHIEVED;
	} while ((timeout--) && (status != JESD204_TRX_SYNC_ACHIEVED));

	if (status != JESD204_TRX_SYNC_ACHIEVED) {
		ad_printf("%s jesd_status: Link SYNC not achieved!\n", __func__);
		return -1;
	}

	if (core.rx_tx_n == 0)
		return 0;

	jesd_read(core, JESD204_REG_RX_LINK_ERROR_STATUS, &status);
	for (link = 0; link < 8; link++) {
		if (status & JESD204_RX_LINK_K_CH_ERR(link)) {
			ad_printf("%s Link %d: K_CH_ERR\n", __func__, link);
			ret = -1;
		}
		if (status & JESD204_RX_LINK_DISP_ERR(link)) {
			ad_printf("%s Link %d: DISP_ERR\n", __func__, link);
			ret = -1;
		}
		if (status & JESD204_RX_LINK_NOT_IN_TBL_ERR(link)) {
			ad_printf("%s Link %d: NOT_IN_TBL_ERR\n", __func__, link);
			ret = -1;
		}
	}

	if (status & JESD204_RX_LINK_LANE_ALIGN_ERR_ALARM) {
		ad_printf("%s jesd_status: frame alignment error!\n", __func__);
		ret = -1;
	}

	if (status & JESD204_RX_LINK_SYSREF_LMFC_ALARM) {
		ad_printf("%s jesd_status: sysref alignment error!\n", __func__);
		ret = -1;
	}

	if (status & JESD204_RX_LINK_BUFF_OVF_ALARM) {
		ad_printf("%s jesd_status: receive buffer overflow error!\n", __func__);
		ret = -1;
	}

	return ret;
}

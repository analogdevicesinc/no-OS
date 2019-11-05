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

/*******************************************************************************/
static const char *axi_jesd204_rx_link_status_label[4] = {
	"RESET",	// 0
	"WAIT FOR PHY",	// 1
	"CGS",		// 2
	"DATA"		// 3
};

static const char *axi_jesd204_tx_link_status_label[] = {
	"WAIT",
	"CGS",
	"ILAS",
	"DATA"
};

static const char *axi_jesd204_rx_lane_status_label[4] = {
	"INIT",		// 0
	"CHECK",	// 1
	"DATA",		// 2
	"UNKNOWN"	// 3
};

/***************************************************************************//**
* @brief jesd_read
*******************************************************************************/
int32_t jesd_read(jesd_core *jesd,
		  uint32_t reg_addr,
		  uint32_t *reg_data)
{
	*reg_data = ad_reg_read((jesd->base_address + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief jesd_write
*******************************************************************************/
int32_t jesd_write(jesd_core *jesd,
		   uint32_t reg_addr,
		   uint32_t reg_data)
{
	ad_reg_write((jesd->base_address + reg_addr), reg_data);

	return 0;
}


/***************************************************************************//**
* @brief jesd_init
*******************************************************************************/
int32_t jesd_setup(jesd_core *jesd)
{
	jesd_write(jesd, JESD204_REG_LINK_DISABLE, 1);
	jesd_write(jesd, JESD204_REG_LINK_CONF0, (((jesd->octets_per_frame-1) << 16) |
			((jesd->frames_per_multiframe*jesd->octets_per_frame)-1)));
	jesd_write(jesd, JESD204_REG_LINK_DISABLE, 0);
	mdelay(100);
	return(0);
}

/***************************************************************************//**
* @brief jesd generate SYSREF if necessar
*******************************************************************************/
int32_t jesd_sysref_control(jesd_core *jesd, uint32_t enable)
{
	gpio_desc *sysref_pin;
	if ((jesd->sysref_type == INTERN) && (jesd->subclass_mode >= 1)) {

		// generate SYS_REF

		gpio_get(&sysref_pin, jesd->sysref_gpio_pin);

		gpio_set_value(sysref_pin, enable);

		gpio_remove(sysref_pin);

		mdelay(10);
	}
	return 0;
}

/***************************************************************************//**
* @brief jesd_read_status generic
*******************************************************************************/
int32_t jesd_status(jesd_core *jesd)
{
	uint32_t status;
	int32_t timeout;
	int32_t ret;

	ret = 0;
	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		jesd_read(jesd, 0x280, &status);
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
	ad_printf("%s jesd_status: %x\n", __func__, status);

	return(ret);
}

/***************************************************************************//**
* @brief axi_jesd204_rx_status_read
*******************************************************************************/
int32_t axi_jesd204_rx_status_read(jesd_core *jesd)
{
	uint32_t sysref_status;
	uint32_t link_disabled;
	uint32_t link_status;
	uint32_t clock_ratio;
	int32_t ret = 0;

	ret |= jesd_read(jesd, JESD204_REG_LINK_STATE, &link_disabled);
	ret |= jesd_read(jesd, JESD204_REG_LINK_STATUS, &link_status);
	ret |= jesd_read(jesd, JESD204_REG_SYSREF_STATUS, &sysref_status);
	ret |= jesd_read(jesd, JESD204_REG_LINK_CLK_RATIO, &clock_ratio);

	ad_printf("Rx link is %s\n",
		  (link_disabled & 0x1) ? "disabled" : "enabled");

	if (clock_ratio == 0) {
		ad_printf("Measured Link Clock: off\n");
	} else {
		ad_printf("Measured Link Clock: %d MHz\n",
			  (clock_ratio * 100 + 0x7fff) >> 16);
	}

	if (!link_disabled) {
		ad_printf("Link status: %s\n",
			  axi_jesd204_rx_link_status_label[link_status & 0x3]);
		ad_printf("SYSREF captured: %s\n",
			  (sysref_status & 1) ? "Yes" : "No");
		if (sysref_status & 2) {
			ad_printf("SYSREF alignment ERROR\n");
		}

	} else {
		ad_printf("External reset is %s\n",
			  (link_disabled & 0x2) ? "asserted" : "deasserted");
	}

	return ret;
}

/***************************************************************************//**
* @brief axi_jesd204_tx_status_read
*******************************************************************************/
int32_t axi_jesd204_tx_status_read(jesd_core *jesd)
{
	uint32_t sysref_status;
	uint32_t link_disabled;
	uint32_t link_status;
	uint32_t clock_ratio;
	int32_t ret = 0;

	ret |= jesd_read(jesd, JESD204_REG_LINK_STATE, &link_disabled);
	ret |= jesd_read(jesd, JESD204_REG_LINK_STATUS, &link_status);
	ret |= jesd_read(jesd, JESD204_REG_SYSREF_STATUS, &sysref_status);
	ret |= jesd_read(jesd, JESD204_REG_LINK_CLK_RATIO, &clock_ratio);

	ad_printf("Tx link is %s\n",
		  (link_disabled & 0x1) ? "disabled" : "enabled");

	if (clock_ratio == 0) {
		ad_printf("Measured Link Clock: off\n");
	} else {
		ad_printf("Measured Link Clock: %d MHz\n",
			  (clock_ratio * 100 + 0x7fff) >> 16);;
	}

	if (!link_disabled) {
		ad_printf("Link status: %s\n",
			  axi_jesd204_tx_link_status_label[link_status & 0x3]);
		ad_printf("SYSREF captured: %s\n",
			  (sysref_status & 1) ? "Yes" : "No");
		if (sysref_status & 2) {
			ad_printf("SYSREF alignment ERROR\n");
		}
	} else {
		ad_printf("External reset is %s\n",
			  (link_disabled & 0x2) ? "asserted" : "deasserted");
	}

	return ret;
}

/***************************************************************************//**
* @brief axi_jesd204_rx_laneinfo_read
*******************************************************************************/
/* FIXME: This violates every single sysfs ABI recommendation */
int32_t axi_jesd204_rx_laneinfo_read(jesd_core *jesd, uint32_t lane)
{
	uint32_t lane_status;
	uint32_t lane_latency;
	uint32_t octets_per_multiframe;
	uint32_t val[4];
	int32_t ret = 0;

	ret |= jesd_read(jesd, JESD204_RX_REG_LANE_STATUS(lane), &lane_status);

	ad_printf("CGS state: %s\n",
		  axi_jesd204_rx_lane_status_label[lane_status & 0x3]);

	ad_printf("Initial Frame Synchronization: %s\n",
		  (lane_status & BIT(4)) ? "Yes" : "No");

	if (!(lane_status & BIT(4)))
		return ret;

	ret |= jesd_read(jesd, JESD204_REG_LINK_CONF0, &octets_per_multiframe);
	octets_per_multiframe &= 0xffff;
	octets_per_multiframe += 1;

	ret |= jesd_read(jesd, JESD204_RX_REG_LANE_LATENCY(lane), &lane_latency);
	ad_printf("Lane Latency: %d Multi-frames and %d Octets\n",
		  lane_latency / octets_per_multiframe,
		  lane_latency % octets_per_multiframe);

	ad_printf("Initial Lane Alignment Sequence: %s\n",
		  (lane_status & BIT(5)) ? "Yes" : "No");

	if (!(lane_status & BIT(5)))
		return ret;

	ret |= jesd_read(jesd, JESD204_RX_REG_ILAS(lane, 0), &val[0]);
	ret |= jesd_read(jesd, JESD204_RX_REG_ILAS(lane, 1), &val[1]);
	ret |= jesd_read(jesd, JESD204_RX_REG_ILAS(lane, 2), &val[2]);
	ret |= jesd_read(jesd, JESD204_RX_REG_ILAS(lane, 3), &val[3]);

	ad_printf("DID: %d, BID: %d, LID: %d, L: %d, SCR: %d, F: %d\n",
		  (val[0] >> 16) & 0xff,
		  (val[0] >> 24) & 0xf,
		  (val[1] >> 0) & 0x1f,
		  (val[1] >> 8) & 0x1f,
		  (val[1] >> 15) & 0x1,
		  (val[1] >> 16) & 0xff
		 );

	ad_printf("K: %d, M: %d, N: %d, CS: %d, N': %d, S: %d, HD: %d\n",
		  (val[1] >> 24) & 0x1f,
		  (val[2] >> 0) & 0xff,
		  (val[2] >> 8) & 0x1f,
		  (val[2] >> 14) & 0x3,
		  (val[2] >> 16) & 0x1f,
		  (val[2] >> 24) & 0x1f,
		  (val[3] >> 7) & 0x1
		 );

	ad_printf("FCHK: 0x%X, CF: %d\n",
		  (val[3] >> 24) & 0xff,
		  (val[3] >> 0) & 0x1f
		 );

	ad_printf("ADJCNT: %d, PHADJ: %d, ADJDIR: %d, JESDV: %d, SUBCLASS: %d\n",
		  (val[0] >> 28) & 0xff,
		  (val[1] >> 5) & 0x1,
		  (val[1] >> 6) & 0x1,
		  (val[2] >> 29) & 0x7,
		  (val[2] >> 21) & 0x7
		 );

	return ret;
}

/*******************************************************************************/
/*******************************************************************************/

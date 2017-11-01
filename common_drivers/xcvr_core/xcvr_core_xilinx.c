// ***************************************************************************
// ***************************************************************************
// Copyright 2014 - 2017 (c) Analog Devices, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in
// the documentation and/or other materials provided with the
// distribution.
// - Neither the name of Analog Devices, Inc. nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
// - The use of this software may or may not infringe the patent rights
// of one or more patent holders. This license does not release you
// from the requirement that you obtain separate licenses from these
// patent holders to use this software.
// - Use of the software either in source or binary form, must be run
// on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************

#ifdef XILINX
#include "xcvr_core_xilinx.h"

/* all are broad-cast reads and writes */

int32_t xcvr_read(struct xcvr_core *core, uint32_t reg_addr, uint32_t *reg_data) {

	int32_t i;
	uint32_t data;
	struct xcvr_core_phy *phy;

	*reg_data = -1;
	data = -1;

	for (i = 0; i < core->no_of_phys; i++) {
		phy = &core->phys[i];
		if (i == 0) {
			data = ad_reg_read(phy->base_address + reg_addr);
		}
		if (ad_reg_read(phy->base_address + reg_addr) != data) {
			return(-1);
		}
	}

	*reg_data = data;
	return(0);
}

int32_t xcvr_write(struct xcvr_core *core, uint32_t reg_addr, uint32_t reg_data) {

	int32_t i;
	struct xcvr_core_phy *phy;

	for (i = 0; i < core->no_of_phys; i++) {
		phy = &core->phys[i];
		ad_reg_write((phy->base_address + reg_addr), reg_data);
	}
	return(0);
}

int32_t xcvr_drp_read(struct xcvr_core *core, uint8_t cm_ch_n, uint8_t drp_sel,
	uint32_t drp_addr, uint32_t *drp_data) {

	return(0);
}

int32_t xcvr_drp_write(struct xcvr_core *core, uint8_t cm_ch_n, uint8_t drp_sel,
	uint32_t drp_addr, uint32_t drp_data) {

	return(0);
}

int32_t xcvr_reconfig(struct xcvr_core *core, uint32_t lane_rate, uint32_t ref_clk) {

	return(0);
}

// ***************************************************************************
// ***************************************************************************

int32_t xcvr_setup(struct xcvr_core *core) {

	ad_gpio_set(core->gpio_reset, 0x1);
	ad_gpio_set(core->gpio_reset, 0x0);

	if (core->tx_or_rx_n == 1) {
	  	xcvr_write(core, XCVR_TX_RESET_ADDR, 0x1);
	  	xcvr_write(core, XCVR_TX_RESET_ADDR, 0x0);
		return(0);
	}

	if (core->tx_or_rx_n == 0) {
	  	xcvr_write(core, XCVR_RX_RESET_ADDR, 0x1);
	  	xcvr_write(core, XCVR_RX_RESET_ADDR, 0x0);
		return(0);
	}

	return(0);
}

int32_t xcvr_status(struct xcvr_core *core) {

	int32_t timeout = 20;
	uint32_t data;
	uint32_t mask;

	data = -1;
	mask = -1;

	if (core->tx_or_rx_n == 1) {
		xcvr_read(core, XCVR_TXPLL_TYPE_ADDR, &data);
		mask = XCVR_TXRESET_STATUS_MASK;
	}
	if (core->tx_or_rx_n == 0) {
		xcvr_read(core, XCVR_RXPLL_TYPE_ADDR, &data);
		mask = XCVR_RXRESET_STATUS_MASK;
	}

	if (data == XCVR_CPLL) {
		mask = mask | XCVR_CPLL_STATUS_MASK;
	}
	if (data == XCVR_QPLL) {
		mask = mask | XCVR_QPLL_STATUS_MASK;
	}
	if (data == XCVR_QPLL1) {
		mask = mask | XCVR_QPLL1_STATUS_MASK;
	}

	while (timeout > 0) {
		xcvr_read(core, XCVR_STATUS_ADDR, &data);
		if ((data & mask) == 0) return(0);
		timeout = timeout - 1;
		mdelay(1);
	}

	if (core->tx_or_rx_n == 0) {
		ad_printf("rx_%s: invalid status, data(%x), mask(%x)!\n", __func__, data, mask);
	} else {
		ad_printf("tx_%s: invalid status, data(%x), mask(%x)!\n", __func__, data, mask);
	}
	return(0);
}

// ***************************************************************************
// ***************************************************************************

int32_t xcvr_eyescan_init(struct xcvr_core *core) {

	core->es_lane = 2;
	core->es_start_address = 0x0;
	core->es_prescale = 0;
	core->es_voffset_min = -127;
	core->es_voffset_max = 127;
	core->es_voffset_step = 1;
	core->es_hoffset_min = -512;
	core->es_hoffset_max = 512;
	core->es_hoffset_step = 1;
	return(0);
}

int32_t xcvr_eyescan(struct xcvr_core *core) {

	return(0);
}

#endif

// ***************************************************************************
// ***************************************************************************

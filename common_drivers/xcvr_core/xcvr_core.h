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

#ifndef XCVR_CORE_H
#define XCVR_CORE_H

#include "platform_drivers.h"
#include "xcvr_core_xilinx.h"
#include "xcvr_core_altera.h"

/* physical link is allowed to be part of multiple instances */
/* phy is always fully consumed (split occurs when full) */
/* worst case -> no_of_lanes == no_of_phys */

struct xcvr_core_phy {

	uint32_t base_address;
	uint32_t no_of_lanes;
};

struct xcvr_core {

	struct xcvr_core_phy *phys;
	uint32_t no_of_phys;
	uint32_t tx_or_rx_n;
	uint32_t gpio_reset;
	uint32_t link_pll_present;
	uint32_t link_pll_base_address;
	uint32_t lane_pll_base_address;
	uint32_t es_base_address;
	uint32_t es_lane;
	uint32_t es_start_address;
	uint32_t es_prescale;
	uint32_t es_voffset_min;
	uint32_t es_voffset_max;
	uint32_t es_voffset_step;
	uint32_t es_hoffset_min;
	uint32_t es_hoffset_max;
	uint32_t es_hoffset_step;
};

int32_t xcvr_read(struct xcvr_core *core, uint32_t reg_addr, uint32_t *reg_data);
int32_t xcvr_write(struct xcvr_core *core, uint32_t reg_addr, uint32_t reg_data);

int32_t xcvr_drp_read(struct xcvr_core *core, uint8_t cm_ch_n, uint8_t drp_sel,
	uint32_t drp_addr, uint32_t *drp_data);
int32_t xcvr_drp_write(struct xcvr_core *core, uint8_t cm_ch_n, uint8_t drp_sel,
	uint32_t drp_addr, uint32_t drp_data);

int32_t xcvr_reconfig(struct xcvr_core *core, uint32_t lane_rate, uint32_t ref_clk);

int32_t xcvr_setup(struct xcvr_core *core);
int32_t xcvr_status(struct xcvr_core *core);

int32_t xcvr_eyescan_init(struct xcvr_core *core);
int32_t xcvr_eyescan(struct xcvr_core *core);

#endif

// ***************************************************************************
// ***************************************************************************

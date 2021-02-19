/***************************************************************************//**
 *   @file   axi_jesd204_rx.h
 *   @brief  Driver for the Analog Devices AXI-JESD204-RX peripheral.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
#ifndef AXI_JESD204_RX_H_
#define AXI_JESD204_RX_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct jesd204_rx_config {
	uint8_t octets_per_frame;
	uint8_t frames_per_multiframe;
	uint8_t subclass_version;
};

/* JESD204 Supported encoding scheme */
enum jesd204_rx_encoder {
	JESD204_RX_ENCODER_UNKNOWN,
	JESD204_RX_ENCODER_8B10B,
	JESD204_RX_ENCODER_64B66B,
	JESD204_RX_ENCODER_MAX,
};

struct axi_jesd204_rx {
	const char *name;
	uint32_t base;
	uint32_t version;
	uint32_t num_lanes;
	uint32_t data_path_width;
	uint32_t tpl_data_path_width;
	struct jesd204_rx_config config;
	uint32_t device_clk_khz;
	uint32_t lane_clk_khz;
	enum jesd204_rx_encoder encoder;
};

struct jesd204_rx_init {
	const char *name;
	uint32_t base;
	uint8_t octets_per_frame;
	uint8_t frames_per_multiframe;
	uint8_t subclass;
	uint32_t device_clk_khz;
	uint32_t lane_clk_khz;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t axi_jesd204_rx_lane_clk_enable(struct axi_jesd204_rx *jesd);
int32_t axi_jesd204_rx_lane_clk_disable(struct axi_jesd204_rx *jesd);
uint32_t axi_jesd204_rx_status_read(struct axi_jesd204_rx *jesd);
int32_t axi_jesd204_rx_laneinfo_read(struct axi_jesd204_rx *jesd,
				     uint32_t lane);
int32_t axi_jesd204_rx_watchdog(struct axi_jesd204_rx *jesd);
int32_t axi_jesd204_rx_init(struct axi_jesd204_rx **jesd204,
			    const struct jesd204_rx_init *init);
int32_t axi_jesd204_rx_remove(struct axi_jesd204_rx *jesd);
#endif

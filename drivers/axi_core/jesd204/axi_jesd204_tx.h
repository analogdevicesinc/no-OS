/***************************************************************************//**
 *   @file   axi_jesd204_tx.h
 *   @brief  Driver for the Analog Devices AXI-JESD204-TX peripheral.
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
#ifndef AXI_JESD204_TX_H_
#define AXI_JESD204_TX_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "jesd204.h"
#include "no_os_clk.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct jesd204_tx_config {
	uint8_t device_id;
	uint8_t bank_id;
	uint8_t lane_id;
	uint8_t lanes_per_device;
	uint8_t octets_per_frame;
	uint16_t frames_per_multiframe;
	uint8_t converters_per_device;
	uint8_t resolution;
	uint8_t bits_per_sample;
	uint8_t samples_per_frame;
	uint8_t jesd_version;
	uint8_t subclass_version;
	uint8_t control_bits_per_sample;
	uint32_t version;
	bool enable_scrambling;
	bool high_density;
};

/**
 * @struct jesd204_tx
 * @brief JESD204B/C Transmit Peripheral Device Structure.
 */
struct axi_jesd204_tx {
	/** Device Name */
	const char *name;
	/** Base address */
	uint32_t base;
	/** Number of lanes */
	uint32_t num_lanes;
	/** Data path width */
	uint32_t data_path_width;
	/** TPL data path width */
	uint32_t tpl_data_path_width;
	/** Tx coniguration */
	struct jesd204_tx_config config;
	/** Device Clock in KHz */
	uint32_t device_clk_khz;
	/** Lane Clock in KHz */
	uint32_t lane_clk_khz;
	/** Selected Encoder */
	enum jesd204_encoder encoder;
	/** Lane Clock */
	struct no_os_clk_desc *lane_clk;
	/** JESD204 FSM device */
	struct jesd204_dev *jdev;
};

/**
 * @struct jesd204_tx_init
 * @brief JESD204B/C Transmit Peripheral Initialization Structure.
 */
struct jesd204_tx_init {
	/** Device Name */
	const char *name;
	/** Base address */
	uint32_t base;
	/** Number of octets per frame (F) */
	uint8_t octets_per_frame;
	/** Number of frames per multi-frame (K) */
	uint16_t frames_per_multiframe;
	/**  Number of converter per device (M) */
	uint8_t converters_per_device;
	/** Converter resolution (N) */
	uint8_t converter_resolution;
	/** Number of bits per sample (N') */
	uint8_t bits_per_sample;
	/** If specified the JESD204B link is configured for high density (HD) operation. */
	bool high_density;
	/**  Number of control bits per conversion sample (CS) */
	uint8_t control_bits_per_sample;
	/** The JESD204B subclass */
	uint8_t subclass;
	/** Device Clock in KHz for the JESD204 interface */
	uint32_t device_clk_khz;
	/** Lane Clock in KHz */
	uint32_t lane_clk_khz;
	/** Lane Clock */
	struct no_os_clk_desc *lane_clk;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** JESD204 TX Lane Clock Enable */
int32_t axi_jesd204_tx_lane_clk_enable(struct axi_jesd204_tx *jesd);
/** JESD204 TX Lane Clock Disable */
int32_t axi_jesd204_tx_lane_clk_disable(struct axi_jesd204_tx *jesd);
/** JESD204 TX Status Read */
uint32_t axi_jesd204_tx_status_read(struct axi_jesd204_tx *jesd);
/** Device initialization */
int32_t axi_jesd204_tx_init_legacy(struct axi_jesd204_tx **jesd204,
				   const struct jesd204_tx_init *init);
/** Device initialization, JED FSM ON */
int32_t axi_jesd204_tx_init(struct axi_jesd204_tx **jesd204,
			    const struct jesd204_tx_init *init);
/** Resources Deallocation */
int32_t axi_jesd204_tx_remove(struct axi_jesd204_tx *jesd);
#endif

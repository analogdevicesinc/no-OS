/***************************************************************************//**
 *   @file   axi_jesd204_tx.h
 *   @brief  Driver for the Analog Devices AXI-JESD204-TX peripheral.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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

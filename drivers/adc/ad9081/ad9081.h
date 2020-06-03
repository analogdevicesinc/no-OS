/***************************************************************************//**
 *   @file   ad9081.h
 *   @brief  Header file of AD9081 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef AD9081_H_
#define AD9081_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "clk.h"
#include "spi.h"
#include "gpio.h"
#include "adi_ad9081.h"

/******************************************************************************/
/********************** Macros and Types Declarations *************************/
/******************************************************************************/
#define MAX_NUM_MAIN_DATAPATHS	4
#define MAX_NUM_CHANNELIZER	8

struct ad9081_jesd_link {
	bool is_jrx;
	adi_cms_jesd_param_t jesd_param;
	uint32_t jrx_tpl_phase_adjust;
	uint8_t logiclane_mapping[8];
	uint8_t link_converter_select[16];
	uint64_t lane_rate;
};

struct dac_settings_cache {
	uint16_t chan_gain[MAX_NUM_CHANNELIZER];
};

struct ad9081_phy {
	spi_desc		*spi_desc;
	gpio_desc		*gpio_reset;
	struct clk		*jesd_rx_clk;
	struct clk		*jesd_tx_clk;
	struct clk		*dev_clk;
	adi_ad9081_device_t	ad9081;
	struct ad9081_jesd_link	jesd_tx_link;
	struct ad9081_jesd_link	jesd_rx_link[2];
	uint32_t	multidevice_instance_count;
	bool		config_sync_01_swapped;
	uint32_t	lmfc_delay;
	uint32_t	nco_sync_ms_extra_lmfc_num;
	/* TX */
	uint64_t	dac_frequency_hz;
	/* The 4 DAC Main Datapaths */
	uint32_t	tx_main_interp;
	uint8_t		tx_dac_chan_xbar[MAX_NUM_MAIN_DATAPATHS];
	int64_t		tx_main_shift[MAX_NUM_MAIN_DATAPATHS];
	/* The 8 DAC Channelizers */
	uint32_t	tx_chan_interp;
	int64_t		tx_chan_shift[MAX_NUM_CHANNELIZER];
	struct dac_settings_cache	dac_cache;
	/* RX */
	uint64_t 	adc_frequency_hz;
	uint32_t	rx_nyquist_zone;
	/* The 4 ADC Main Datapaths */
	int64_t		rx_cddc_shift[MAX_NUM_MAIN_DATAPATHS];
	uint32_t	adc_main_decimation[MAX_NUM_MAIN_DATAPATHS];
	uint8_t 	rx_cddc_dcm[MAX_NUM_MAIN_DATAPATHS];
	uint8_t 	rx_cddc_c2r[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_cddc_select;
	/* The 8 ADC Channelizers */
	int64_t		rx_fddc_shift[MAX_NUM_CHANNELIZER];
	uint32_t	adc_chan_decimation[MAX_NUM_CHANNELIZER];
	uint8_t		rx_fddc_dcm[MAX_NUM_CHANNELIZER];
	uint8_t 	rx_fddc_c2r[MAX_NUM_CHANNELIZER];
	uint8_t 	rx_fddc_select;
};

struct link_init_param {
	uint32_t	device_id;
	uint32_t	octets_per_frame;
	uint32_t	frames_per_multiframe;
	uint32_t	samples_per_converter_per_frame;
	uint32_t	high_density;
	uint32_t	converter_resolution;
	uint32_t	bits_per_sample;
	uint32_t	converters_per_device;
	uint32_t	control_bits_per_sample;
	uint32_t	lanes_per_device;
	uint32_t	subclass;
	uint32_t	link_mode;
	uint32_t	dual_link;
	uint32_t	version;
	uint8_t		logical_lane_mapping[8];
	/* JTX */
	uint8_t		link_converter_select[16];
	/* JRX */
	uint32_t	tpl_phase_adjust;
};

struct ad9081_init_param {
	spi_init_param	*spi_init;
	gpio_init_param	*gpio_reset;
	struct clk	*dev_clk;
	struct clk	*jesd_rx_clk;
	struct clk	*jesd_tx_clk;
	uint32_t	multidevice_instance_count;
	bool		jesd_sync_pins_01_swap_enable;
	uint32_t	lmfc_delay_dac_clk_cycles;
	uint32_t	nco_sync_ms_extra_lmfc_num;
	/* TX */
	uint64_t	dac_frequency_hz;
	/* The 4 DAC Main Datapaths */
	uint32_t	tx_main_interpolation;
	int64_t		tx_main_nco_frequency_shift_hz[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		tx_dac_channel_crossbar_select[MAX_NUM_MAIN_DATAPATHS];
	/* The 8 DAC Channelizers */
	uint32_t	tx_channel_interpolation;
	int64_t		tx_channel_nco_frequency_shift_hz[MAX_NUM_CHANNELIZER];
	uint16_t	tx_channel_gain[MAX_NUM_CHANNELIZER];
	struct link_init_param	*jesd_tx_link;
	/* RX */
	uint64_t 	adc_frequency_hz;
	uint32_t	nyquist_zone;
	/* The 4 ADC Main Datapaths */
	int64_t		rx_main_nco_frequency_shift_hz[MAX_NUM_MAIN_DATAPATHS];
	uint32_t	rx_main_decimation[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_main_complex_to_real_enable[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_main_enable[MAX_NUM_MAIN_DATAPATHS];
	/* The 8 ADC Channelizers */
	int64_t		rx_channel_nco_frequency_shift_hz[MAX_NUM_CHANNELIZER];
	uint32_t	rx_channel_decimation[MAX_NUM_CHANNELIZER];
	uint8_t		rx_channel_complex_to_real_enable[MAX_NUM_CHANNELIZER];
	uint8_t		rx_channel_enable[MAX_NUM_CHANNELIZER];
	struct link_init_param	*jesd_rx_link[2];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int32_t ad9081_init(struct ad9081_phy **device,
		    const struct ad9081_init_param *init_param);
/* Remove the device. */
int32_t ad9081_remove(struct ad9081_phy *device);
/* Work function. */
void ad9081_work_func(struct ad9081_phy *phy);
#endif

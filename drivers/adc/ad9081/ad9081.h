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
#include "no_os_clk.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "adi_ad9081.h"
#include "jesd204.h"

/******************************************************************************/
/********************** Macros and Types Declarations *************************/
/******************************************************************************/
#define MAX_NUM_MAIN_DATAPATHS	4
#define MAX_NUM_CHANNELIZER	8

struct ad9081_jesd_link {
	bool is_jrx;
	adi_cms_jesd_param_t jesd_param;
	struct jesd204_link jesd204_link;
	uint32_t jrx_tpl_phase_adjust;
	uint8_t logiclane_mapping[8];
	uint8_t link_converter_select[16];
	uint64_t lane_rate;
	unsigned long lane_rate_kbps;
	unsigned long lane_cal_rate_kbps;
};

struct dac_settings_cache {
	uint16_t chan_gain[MAX_NUM_CHANNELIZER];
};

struct ad9081_phy {
	struct no_os_spi_desc		*spi_desc;
	struct no_os_gpio_desc		*gpio_reset;
	struct no_os_gpio_desc		*ms_sync_en_gpio;
	struct no_os_clk		*jesd_rx_clk;
	struct no_os_clk		*jesd_tx_clk;
	struct no_os_clk		*dev_clk;
	struct jesd204_dev		*jdev;
	uint8_t		sync_ms_gpio_num;
	bool		sysref_coupling_ac_en;
	bool 		sysref_cmos_input_en;
	uint8_t 	sysref_cmos_single_end_term_pos;
	uint8_t 	sysref_cmos_single_end_term_neg;
	adi_ad9081_device_t	ad9081;
	struct ad9081_jesd_link	jrx_link_tx[2];
	struct ad9081_jesd_link	jtx_link_rx[2];
	uint32_t	multidevice_instance_count;
	bool		config_sync_01_swapped;
	bool		config_sync_0a_cmos_en;
	uint32_t	lmfc_delay;
	uint32_t	nco_sync_ms_extra_lmfc_num;
	bool		nco_sync_direct_sysref_mode_en;
	uint32_t	sysref_average_cnt_exp;
	bool		sysref_continuous_dis;
	bool		is_initialized;
	bool		tx_disable;
	bool		rx_disable;
	/* TX */
	uint64_t	dac_frequency_hz;
	/* The 4 DAC Main Datapaths */
	uint32_t	tx_main_interp;
	uint8_t		tx_dac_chan_xbar[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		tx_dac_chan_xbar_1x_non1x[MAX_NUM_MAIN_DATAPATHS];
	int64_t		tx_main_shift[MAX_NUM_MAIN_DATAPATHS];
	/* The 8 DAC Channelizers */
	uint32_t	tx_chan_interp;
	int64_t		tx_chan_shift[MAX_NUM_CHANNELIZER];
	uint32_t	tx_dac_fsc[MAX_NUM_MAIN_DATAPATHS];
	bool		tx_ffh_hopf_via_gpio_en;
	struct dac_settings_cache	dac_cache;
	/* RX */
	uint32_t	adc_dcm[2];
	uint64_t 	adc_frequency_hz;
	uint32_t	rx_nyquist_zone[MAX_NUM_MAIN_DATAPATHS];
	/* The 4 ADC Main Datapaths */
	int64_t		rx_cddc_shift[MAX_NUM_MAIN_DATAPATHS];
	uint32_t	adc_main_decimation[MAX_NUM_MAIN_DATAPATHS];
	uint8_t 	rx_cddc_dcm[MAX_NUM_MAIN_DATAPATHS];
	uint8_t 	rx_fddc_mxr_if[MAX_NUM_CHANNELIZER];
	uint8_t 	rx_cddc_c2r[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_cddc_select;
	/* The 8 ADC Channelizers */
	int64_t		rx_fddc_shift[MAX_NUM_CHANNELIZER];
	uint32_t	adc_chan_decimation[MAX_NUM_CHANNELIZER];
	uint8_t		rx_fddc_dcm[MAX_NUM_CHANNELIZER];
	uint8_t 	rx_fddc_c2r[MAX_NUM_CHANNELIZER];
	uint8_t		rx_cddc_gain_6db_en[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_fddc_gain_6db_en[MAX_NUM_CHANNELIZER];
	uint8_t 	rx_fddc_select;
	uint8_t		rx_cddc_nco_channel_select_mode[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_ffh_gpio_mux_sel[6];
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
	struct no_os_spi_init_param	*spi_init;
	struct no_os_gpio_init_param	*gpio_reset;
	struct no_os_gpio_init_param	*ms_sync_enable;
	struct no_os_clk	*dev_clk;
	struct no_os_clk	*jesd_rx_clk;
	struct no_os_clk	*jesd_tx_clk;
	uint8_t		master_slave_sync_gpio_num;
	bool		sysref_coupling_ac_en;
	bool		sysref_cmos_input_enable;
	uint8_t 	sysref_cmos_single_end_term_pos;
	uint8_t 	sysref_cmos_single_end_term_neg;
	uint32_t	multidevice_instance_count;
	bool		jesd_sync_pins_01_swap_enable;
	bool 		config_sync_0a_cmos_enable;
	uint32_t	lmfc_delay_dac_clk_cycles;
	uint32_t	nco_sync_ms_extra_lmfc_num;
	bool		nco_sync_direct_sysref_mode_enable;
	uint32_t	sysref_average_cnt_exp;
	bool		continuous_sysref_mode_disable;
	bool		tx_disable;
	bool		rx_disable;
	/* TX */
	uint64_t	dac_frequency_hz;
	/* The 4 DAC Main Datapaths */
	uint32_t	tx_main_interpolation;
	int64_t		tx_main_nco_frequency_shift_hz[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		tx_dac_channel_crossbar_select[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		tx_maindp_dac_1x_non1x_crossbar_select[MAX_NUM_MAIN_DATAPATHS];
	uint32_t	tx_full_scale_current_ua[MAX_NUM_MAIN_DATAPATHS];
	/* The 8 DAC Channelizers */
	uint32_t	tx_channel_interpolation;
	int64_t		tx_channel_nco_frequency_shift_hz[MAX_NUM_CHANNELIZER];
	uint16_t	tx_channel_gain[MAX_NUM_CHANNELIZER];
	struct link_init_param	*jrx_link_tx[2];
	/* RX */
	uint64_t 	adc_frequency_hz;
	uint32_t	nyquist_zone[MAX_NUM_MAIN_DATAPATHS];
	/* The 4 ADC Main Datapaths */
	int64_t		rx_main_nco_frequency_shift_hz[MAX_NUM_MAIN_DATAPATHS];
	uint32_t	rx_main_decimation[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_main_complex_to_real_enable[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_main_digital_gain_6db_enable[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_main_enable[MAX_NUM_MAIN_DATAPATHS];
	/* The 8 ADC Channelizers */
	int64_t		rx_channel_nco_frequency_shift_hz[MAX_NUM_CHANNELIZER];
	uint32_t	rx_channel_decimation[MAX_NUM_CHANNELIZER];
	uint8_t		rx_channel_complex_to_real_enable[MAX_NUM_CHANNELIZER];
	uint8_t		rx_channel_nco_mixer_mode[MAX_NUM_CHANNELIZER];
	uint8_t		rx_channel_digital_gain_6db_enable[MAX_NUM_CHANNELIZER];
	uint8_t		rx_channel_enable[MAX_NUM_CHANNELIZER];
	uint8_t		rx_cddc_nco_channel_select_mode[MAX_NUM_MAIN_DATAPATHS];
	uint8_t		rx_ffh_gpio_mux_selection[6];
	struct link_init_param	*jtx_link_rx[2];
};

/* ffh: 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n */

#define AD9081_PERI_SEL_GPIO6		2
#define AD9081_PERI_SEL_GPIO7		3
#define AD9081_PERI_SEL_GPIO8		4
#define AD9081_PERI_SEL_GPIO9		5
#define AD9081_PERI_SEL_GPIO10		6
#define AD9081_PERI_SEL_SYNCINB1_P	7
#define AD9081_PERI_SEL_SYNCINB1_N	8

#define AD9081_FFH_CHAN_SEL_REG_MODE		0 /* 0:  Register Map control (Use ddc_nco_regmap_chan_sel) */
#define AD9081_FFH_CHAN_SEL_1GPIO_MODE		1 /* 1:  profile_pins[0]     is used. Pin level control {3'b0, profile_pins[0]} */
#define AD9081_FFH_CHAN_SEL_2GPIO_MODE		2 /* 2:  profile_pins[1 :0] are used. Pin level control {2'b0, profile_pins[1:0]} */
#define AD9081_FFH_CHAN_SEL_3GPIO_MODE		3 /* 3:  profile_pins[2 :0] are used. Pin level control {1'b0, profile_pins[2:0]} */
#define AD9081_FFH_CHAN_SEL_4GPIO_MODE		4 /* 4:  profile_pins[3 :0] are used. Pin level control { profile_pins[3:0]} */
#define AD9081_FFH_CHAN_SEL_GPIO0_EDGE_MODE	8 /* 8:  profile_pins[0] Pin edge control- increment internal counter when rising edge of profile_pins[0] Pin. */
#define AD9081_FFH_CHAN_SEL_GPIO1_EDGE_MODE	9 /* 9:  profile_pins[1] Pin edge control- increment internal counter when rising edge of profile_pins[1] Pin. */
#define AD9081_FFH_CHAN_SEL_GPIO2_EDGE_MODE	10 /* 10: profile_pins[2] Pin edge control- increment internal counter when rising edge of profile_pins[2] Pin. */
#define AD9081_FFH_CHAN_SEL_GPIO3_EDGE_MODE	11 /* 11: profile_pins[3] Pin edge control- increment internal counter when rising edge of profile_pins[3] Pin. */
#define AD9081_FFH_CHAN_SEL_FHT_EXP_MODE	12 /* 12: FHT expire based control - increment internal counter when FHT is expired. */

/*
 * JESD204-FSM defines
 */
#define DEFRAMER_LINK0_TX 0
#define DEFRAMER_LINK1_TX 1
#define FRAMER_LINK0_RX 2
#define FRAMER_LINK1_RX 3

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

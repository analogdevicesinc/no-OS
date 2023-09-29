/***************************************************************************//**
 *   @file   adrv9009.h
 *   @brief  Header file of ADRV9009 Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef ADRV9009_H_
#define ADRV9009_H_

#include "talise.h"
#include "talise_agc_types.h"
#include "talise_gpio_types.h"

enum adrv900x_device_id {
	ID_ADRV9009,
	ID_ADRV90081,
	ID_ADRV90082,
	ID_ADRV9009_X2,
	ID_ADRV9009_X4,
};

#define DEFRAMER_LINK_TX	0
#define FRAMER_LINK_RX		1
#define FRAMER_LINK_ORX		2

enum adrv9009_clocks {
	RX_SAMPL_CLK,
	OBS_SAMPL_CLK,
	TX_SAMPL_CLK,
	NUM_ADRV9009_CLKS,
};

struct adrv9009_rf_phy {
	int		spi_device_id;

	struct adi_hal	no_os_hal;

	uint8_t		*stream;
	uint8_t		*fw;
	uint32_t	fw_size;

	taliseInit_t 	*talInit;
	taliseDevice_t 	talise_device;
	taliseDevice_t 	*talDevice;

	taliseAgcCfg_t 	*rxAgcCtrl;

	taliseArmGpioConfig_t	arm_gpio_config;
	taliseTxAttenCtrlPin_t	tx1_atten_ctrl_pin;
	taliseTxAttenCtrlPin_t	tx2_atten_ctrl_pin;
	taliseTxPaProtectCfg_t	tx_pa_protection;
	taliseAuxDac_t	auxdac;

	uint16_t	gpio3v3SrcCtrl;
	uint16_t	gpio3v3PinLevel;
	uint16_t	gpio3v3OutEn;

	struct no_os_clk_desc	*dev_clk;
	struct no_os_clk_desc 	*jesd_rx_clk;
	struct no_os_clk_desc 	*jesd_tx_clk;
	struct no_os_clk_desc 	*clks[NUM_ADRV9009_CLKS];

	uint32_t 	initCalMask;
	uint32_t	tracking_cal_mask;

	int16_t		current_loopBandwidth_kHz[2];
	uint8_t		loopFilter_stability;
	uint64_t	trx_lo_frequency;

	uint32_t 	framer_b_m;
	uint32_t 	framer_b_f;
	uint32_t 	orx_channel_enabled;

	struct jesd204_dev	*jdev;

	bool			is_initialized;
};

struct adrv9009_init_param {
	int		spi_device_id;

	struct no_os_spi_init_param	*spi_init;
	struct no_os_gpio_init_param	*gpio_adrv_resetb_init;
	struct no_os_gpio_init_param	*gpio_adrv_sysref_req_init;

	uint8_t		*stream;
	uint8_t		*fw;
	uint32_t	fw_size;

	taliseInit_t	*talInit;

	taliseAgcCfg_t 	*rxAgcCtrl;

	uint8_t	arm_gpio_config_orx1_tx_sel0_pin_gpio_pin_sel;
	uint8_t	arm_gpio_config_orx1_tx_sel0_pin_polarity;
	uint8_t	arm_gpio_config_orx1_tx_sel0_pin_enable;

	uint8_t	arm_gpio_config_orx1_tx_sel1_pin_gpio_pin_sel;
	uint8_t	arm_gpio_config_orx1_tx_sel1_pin_polarity;
	uint8_t	arm_gpio_config_orx1_tx_sel1_pin_enable;
	uint8_t	arm_gpio_config_orx2_tx_sel0_pin_gpio_pin_sel;
	uint8_t	arm_gpio_config_orx2_tx_sel0_pin_polarity;
	uint8_t	arm_gpio_config_orx2_tx_sel0_pin_enable;

	uint8_t	arm_gpio_config_orx2_tx_sel1_pin_gpio_pin_sel;
	uint8_t	arm_gpio_config_orx2_tx_sel1_pin_polarity;
	uint8_t	arm_gpio_config_orx2_tx_sel1_pin_enable;
	uint8_t	arm_gpio_config_en_tx_tracking_cals_gpio_pin_sel;
	uint8_t	arm_gpio_config_en_tx_tracking_cals_polarity;
	uint8_t	arm_gpio_config_en_tx_tracking_cals_enable;

	uint8_t	tx1_atten_ctrl_pin_step_size;
	uint8_t	tx1_atten_ctrl_pin_tx_atten_inc_pin;
	uint8_t	tx1_atten_ctrl_pin_tx_atten_dec_pin;
	uint8_t	tx1_atten_ctrl_pin_enable;

	uint8_t	tx2_atten_ctrl_pin_step_size;
	uint8_t	tx2_atten_ctrl_pin_tx_atten_inc_pin;
	uint8_t	tx2_atten_ctrl_pin_tx_atten_dec_pin;
	uint8_t	tx2_atten_ctrl_pin_enable;

	uint8_t		tx_pa_protection_avg_duration;
	uint8_t		tx_pa_protection_tx_atten_step;
	uint16_t	tx_pa_protection_tx1_power_threshold;
	uint16_t	tx_pa_protection_tx2_power_threshold;
	uint8_t		tx_pa_protection_peak_count;
	uint8_t		tx_pa_protection_tx1_peak_threshold;
	uint8_t		tx_pa_protection_tx2_peak_threshold;

	uint16_t	aux_dac_enables;
	uint8_t		aux_dac_vref0;
	uint8_t		aux_dac_resolution0;
	uint16_t	aux_dac_values0;
	uint8_t		aux_dac_vref1;
	uint8_t		aux_dac_resolution1;
	uint16_t	aux_dac_values1;
	uint8_t		aux_dac_vref2;
	uint8_t		aux_dac_resolution2;
	uint16_t	aux_dac_values2;
	uint8_t		aux_dac_vref3;
	uint8_t		aux_dac_resolution3;
	uint16_t	aux_dac_values3;
	uint8_t		aux_dac_vref4;
	uint8_t		aux_dac_resolution4;
	uint16_t	aux_dac_values4;
	uint8_t		aux_dac_vref5;
	uint8_t		aux_dac_resolution5;
	uint16_t	aux_dac_values5;
	uint8_t		aux_dac_vref6;
	uint8_t		aux_dac_resolution6;
	uint16_t	aux_dac_values6;
	uint8_t		aux_dac_vref7;
	uint8_t		aux_dac_resolution7;
	uint8_t		aux_dac_values7;
	uint8_t		aux_dac_vref8;
	uint8_t		aux_dac_resolution8;
	uint16_t	aux_dac_values8;
	uint8_t		aux_dac_vref9;
	uint8_t		aux_dac_resolution9;
	uint16_t	aux_dac_values9;
	uint16_t	aux_dac_values10;
	uint16_t	aux_dac_values11;

	uint16_t	gpio3v3_source_control;
	uint16_t 	gpio3v3_output_level_mask;
	uint16_t 	gpio3v3_output_enable_mask;

	struct no_os_clk_desc	*dev_clk;

	uint64_t	trx_pll_lo_frequency_hz;
};

int adrv9009_init(struct adrv9009_rf_phy **dev,
	const struct adrv9009_init_param *init_param);

int adrv9009_remove(struct adrv9009_rf_phy *dev);

#endif

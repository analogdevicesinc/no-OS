/***************************************************************************//**
 *   @file   ad9361_api.h
 *   @brief  Header file of AD9361 API Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef AD9361_API_H_
#define AD9361_API_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "util.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	/* Device selection */
	enum dev_id	dev_sel;
	/* Identification number */
	uint8_t		id_no;
	/* Reference Clock */
	uint32_t	reference_clk_rate;
	/* Base Configuration */
	uint8_t		two_rx_two_tx_mode_enable;	/* adi,2rx-2tx-mode-enable */
	uint8_t		one_rx_one_tx_mode_use_rx_num;	/* adi,1rx-1tx-mode-use-rx-num */
	uint8_t		one_rx_one_tx_mode_use_tx_num;	/* adi,1rx-1tx-mode-use-tx-num */
	uint8_t		frequency_division_duplex_mode_enable;	/* adi,frequency-division-duplex-mode-enable */
	uint8_t		frequency_division_duplex_independent_mode_enable;	/* adi,frequency-division-duplex-independent-mode-enable */
	uint8_t		tdd_use_dual_synth_mode_enable;	/* adi,tdd-use-dual-synth-mode-enable */
	uint8_t		tdd_skip_vco_cal_enable;		/* adi,tdd-skip-vco-cal-enable */
	uint32_t	tx_fastlock_delay_ns;	/* adi,tx-fastlock-delay-ns */
	uint32_t	rx_fastlock_delay_ns;	/* adi,rx-fastlock-delay-ns */
	uint8_t		rx_fastlock_pincontrol_enable;	/* adi,rx-fastlock-pincontrol-enable */
	uint8_t		tx_fastlock_pincontrol_enable;	/* adi,tx-fastlock-pincontrol-enable */
	uint8_t		external_rx_lo_enable;	/* adi,external-rx-lo-enable */
	uint8_t		external_tx_lo_enable;	/* adi,external-tx-lo-enable */
	uint8_t		dc_offset_tracking_update_event_mask;	/* adi,dc-offset-tracking-update-event-mask */
	uint8_t		dc_offset_attenuation_high_range;	/* adi,dc-offset-attenuation-high-range */
	uint8_t		dc_offset_attenuation_low_range;	/* adi,dc-offset-attenuation-low-range */
	uint8_t		dc_offset_count_high_range;			/* adi,dc-offset-count-high-range */
	uint8_t		dc_offset_count_low_range;			/* adi,dc-offset-count-low-range */
	uint8_t		split_gain_table_mode_enable;	/* adi,split-gain-table-mode-enable */
	uint32_t	trx_synthesizer_target_fref_overwrite_hz;	/* adi,trx-synthesizer-target-fref-overwrite-hz */
	uint8_t		qec_tracking_slow_mode_enable;	/* adi,qec-tracking-slow-mode-enable */
	/* ENSM Control */
	uint8_t		ensm_enable_pin_pulse_mode_enable;	/* adi,ensm-enable-pin-pulse-mode-enable */
	uint8_t		ensm_enable_txnrx_control_enable;	/* adi,ensm-enable-txnrx-control-enable */
	/* LO Control */
	uint64_t	rx_synthesizer_frequency_hz;	/* adi,rx-synthesizer-frequency-hz */
	uint64_t	tx_synthesizer_frequency_hz;	/* adi,tx-synthesizer-frequency-hz */
	uint8_t		tx_lo_powerdown_managed_enable;	/* adi,tx-lo-powerdown-managed-enable */
	/* Rate & BW Control */
	uint32_t	rx_path_clock_frequencies[6];	/* adi,rx-path-clock-frequencies */
	uint32_t	tx_path_clock_frequencies[6];	/* adi,tx-path-clock-frequencies */
	uint32_t	rf_rx_bandwidth_hz;	/* adi,rf-rx-bandwidth-hz */
	uint32_t	rf_tx_bandwidth_hz;	/* adi,rf-tx-bandwidth-hz */
	/* RF Port Control */
	uint32_t	rx_rf_port_input_select;	/* adi,rx-rf-port-input-select */
	uint32_t	tx_rf_port_input_select;	/* adi,tx-rf-port-input-select */
	/* TX Attenuation Control */
	int32_t		tx_attenuation_mdB;	/* adi,tx-attenuation-mdB */
	uint8_t		update_tx_gain_in_alert_enable;	/* adi,update-tx-gain-in-alert-enable */
	/* Reference Clock Control */
	uint8_t		xo_disable_use_ext_refclk_enable;	/* adi,xo-disable-use-ext-refclk-enable */
	uint32_t	dcxo_coarse_and_fine_tune[2];	/* adi,dcxo-coarse-and-fine-tune */
	uint32_t	clk_output_mode_select;		/* adi,clk-output-mode-select */
	/* Gain Control */
	uint8_t		gc_rx1_mode;	/* adi,gc-rx1-mode */
	uint8_t		gc_rx2_mode;	/* adi,gc-rx2-mode */
	uint8_t		gc_adc_large_overload_thresh;	/* adi,gc-adc-large-overload-thresh */
	uint8_t		gc_adc_ovr_sample_size;	/* adi,gc-adc-ovr-sample-size */
	uint8_t		gc_adc_small_overload_thresh;	/* adi,gc-adc-small-overload-thresh */
	uint16_t	gc_dec_pow_measurement_duration;	/* adi,gc-dec-pow-measurement-duration */
	uint8_t		gc_dig_gain_enable;	/* adi,gc-dig-gain-enable */
	uint16_t	gc_lmt_overload_high_thresh;	/* adi,gc-lmt-overload-high-thresh */
	uint16_t	gc_lmt_overload_low_thresh;	/* adi,gc-lmt-overload-low-thresh */
	uint8_t		gc_low_power_thresh;	/* adi,gc-low-power-thresh */
	uint8_t		gc_max_dig_gain;	/* adi,gc-max-dig-gain */
	/* Gain MGC Control */
	uint8_t		mgc_dec_gain_step;	/* adi,mgc-dec-gain-step */
	uint8_t		mgc_inc_gain_step;	/* adi,mgc-inc-gain-step */
	uint8_t		mgc_rx1_ctrl_inp_enable;	/* adi,mgc-rx1-ctrl-inp-enable */
	uint8_t		mgc_rx2_ctrl_inp_enable;	/* adi,mgc-rx2-ctrl-inp-enable */
	uint8_t		mgc_split_table_ctrl_inp_gain_mode;	/* adi,mgc-split-table-ctrl-inp-gain-mode */
	/* Gain AGC Control */
	uint8_t		agc_adc_large_overload_exceed_counter;	/* adi,agc-adc-large-overload-exceed-counter */
	uint8_t		agc_adc_large_overload_inc_steps;	/* adi,agc-adc-large-overload-inc-steps */
	uint8_t		agc_adc_lmt_small_overload_prevent_gain_inc_enable;	/* adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable */
	uint8_t		agc_adc_small_overload_exceed_counter;	/* adi,agc-adc-small-overload-exceed-counter */
	uint8_t		agc_dig_gain_step_size;	/* adi,agc-dig-gain-step-size */
	uint8_t		agc_dig_saturation_exceed_counter;	/* adi,agc-dig-saturation-exceed-counter */
	uint32_t	agc_gain_update_interval_us; /* adi,agc-gain-update-interval-us */
	uint8_t		agc_immed_gain_change_if_large_adc_overload_enable;	/* adi,agc-immed-gain-change-if-large-adc-overload-enable */
	uint8_t		agc_immed_gain_change_if_large_lmt_overload_enable;	/* adi,agc-immed-gain-change-if-large-lmt-overload-enable */
	uint8_t		agc_inner_thresh_high;	/* adi,agc-inner-thresh-high */
	uint8_t		agc_inner_thresh_high_dec_steps;	/* adi,agc-inner-thresh-high-dec-steps */
	uint8_t		agc_inner_thresh_low;	/* adi,agc-inner-thresh-low */
	uint8_t		agc_inner_thresh_low_inc_steps;	/* adi,agc-inner-thresh-low-inc-steps */
	uint8_t		agc_lmt_overload_large_exceed_counter;	/* adi,agc-lmt-overload-large-exceed-counter */
	uint8_t		agc_lmt_overload_large_inc_steps;	/* adi,agc-lmt-overload-large-inc-steps */
	uint8_t		agc_lmt_overload_small_exceed_counter;	/* adi,agc-lmt-overload-small-exceed-counter */
	uint8_t		agc_outer_thresh_high;	/* adi,agc-outer-thresh-high */
	uint8_t		agc_outer_thresh_high_dec_steps;	/* adi,agc-outer-thresh-high-dec-steps */
	uint8_t		agc_outer_thresh_low;	/* adi,agc-outer-thresh-low */
	uint8_t		agc_outer_thresh_low_inc_steps;	/* adi,agc-outer-thresh-low-inc-steps */
	uint32_t	agc_attack_delay_extra_margin_us;	/* adi,agc-attack-delay-extra-margin-us */
	uint8_t		agc_sync_for_gain_counter_enable;	/* adi,agc-sync-for-gain-counter-enable */
	/* Fast AGC */
	uint32_t	fagc_dec_pow_measuremnt_duration;	/* adi,fagc-dec-pow-measurement-duration */
	uint32_t	fagc_state_wait_time_ns;	/* adi,fagc-state-wait-time-ns */
	/* Fast AGC - Low Power */
	uint8_t		fagc_allow_agc_gain_increase;	/* adi,fagc-allow-agc-gain-increase-enable */
	uint32_t	fagc_lp_thresh_increment_time;	/* adi,fagc-lp-thresh-increment-time */
	uint32_t	fagc_lp_thresh_increment_steps;	/* adi,fagc-lp-thresh-increment-steps */
	/* Fast AGC - Lock Level (Lock Level is set via slow AGC inner high threshold) */
	uint8_t		fagc_lock_level_lmt_gain_increase_en;	/* adi,fagc-lock-level-lmt-gain-increase-enable */
	uint32_t	fagc_lock_level_gain_increase_upper_limit;	/* adi,fagc-lock-level-gain-increase-upper-limit */
	/* Fast AGC - Peak Detectors and Final Settling */
	uint32_t	fagc_lpf_final_settling_steps;	/* adi,fagc-lpf-final-settling-steps */
	uint32_t	fagc_lmt_final_settling_steps;	/* adi,fagc-lmt-final-settling-steps */
	uint32_t	fagc_final_overrange_count;	/* adi,fagc-final-overrange-count */
	/* Fast AGC - Final Power Test */
	uint8_t		fagc_gain_increase_after_gain_lock_en;	/* adi,fagc-gain-increase-after-gain-lock-enable */
	/* Fast AGC - Unlocking the Gain */
	uint32_t	fagc_gain_index_type_after_exit_rx_mode;	/* adi,fagc-gain-index-type-after-exit-rx-mode */
	uint8_t		fagc_use_last_lock_level_for_set_gain_en;	/* adi,fagc-use-last-lock-level-for-set-gain-enable */
	uint8_t		fagc_rst_gla_stronger_sig_thresh_exceeded_en;	/* adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable */
	uint32_t	fagc_optimized_gain_offset;	/* adi,fagc-optimized-gain-offset */
	uint32_t	fagc_rst_gla_stronger_sig_thresh_above_ll;	/* adi,fagc-rst-gla-stronger-sig-thresh-above-ll */
	uint8_t		fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en;	/* adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable */
	uint8_t		fagc_rst_gla_engergy_lost_goto_optim_gain_en;	/* adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable */
	uint32_t	fagc_rst_gla_engergy_lost_sig_thresh_below_ll;	/* adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll */
	uint32_t	fagc_energy_lost_stronger_sig_gain_lock_exit_cnt;	/* adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt */
	uint8_t		fagc_rst_gla_large_adc_overload_en;	/* adi,fagc-rst-gla-large-adc-overload-enable */
	uint8_t		fagc_rst_gla_large_lmt_overload_en;	/* adi,fagc-rst-gla-large-lmt-overload-enable */
	uint8_t		fagc_rst_gla_en_agc_pulled_high_en;	/* adi,fagc-rst-gla-en-agc-pulled-high-enable */
	uint32_t	fagc_rst_gla_if_en_agc_pulled_high_mode;	/* adi,fagc-rst-gla-if-en-agc-pulled-high-mode */
	uint32_t	fagc_power_measurement_duration_in_state5;	/* adi,fagc-power-measurement-duration-in-state5 */
	/* RSSI Control */
	uint32_t	rssi_delay;	/* adi,rssi-delay */
	uint32_t	rssi_duration;	/* adi,rssi-duration */
	uint8_t		rssi_restart_mode;	/* adi,rssi-restart-mode */
	uint8_t		rssi_unit_is_rx_samples_enable;	/* adi,rssi-unit-is-rx-samples-enable */
	uint32_t	rssi_wait;	/* adi,rssi-wait */
	/* Aux ADC Control */
	uint32_t	aux_adc_decimation;	/* adi,aux-adc-decimation */
	uint32_t	aux_adc_rate;	/* adi,aux-adc-rate */
	/* AuxDAC Control */
	uint8_t		aux_dac_manual_mode_enable;	/* adi,aux-dac-manual-mode-enable */
	uint32_t	aux_dac1_default_value_mV;	/* adi,aux-dac1-default-value-mV */
	uint8_t		aux_dac1_active_in_rx_enable;	/* adi,aux-dac1-active-in-rx-enable */
	uint8_t		aux_dac1_active_in_tx_enable;	/* adi,aux-dac1-active-in-tx-enable */
	uint8_t		aux_dac1_active_in_alert_enable;	/* adi,aux-dac1-active-in-alert-enable */
	uint32_t	aux_dac1_rx_delay_us;	/* adi,aux-dac1-rx-delay-us */
	uint32_t	aux_dac1_tx_delay_us;	/* adi,aux-dac1-tx-delay-us */
	uint32_t	aux_dac2_default_value_mV;	/* adi,aux-dac2-default-value-mV */
	uint8_t		aux_dac2_active_in_rx_enable;	/* adi,aux-dac2-active-in-rx-enable */
	uint8_t		aux_dac2_active_in_tx_enable;	/* adi,aux-dac2-active-in-tx-enable */
	uint8_t		aux_dac2_active_in_alert_enable;	/* adi,aux-dac2-active-in-alert-enable */
	uint32_t	aux_dac2_rx_delay_us;	/* adi,aux-dac2-rx-delay-us */
	uint32_t	aux_dac2_tx_delay_us;	/* adi,aux-dac2-tx-delay-us */
	/* Temperature Sensor Control */
	uint32_t	temp_sense_decimation;	/* adi,temp-sense-decimation */
	uint16_t	temp_sense_measurement_interval_ms;	/* adi,temp-sense-measurement-interval-ms */
	int8_t		temp_sense_offset_signed;	/* adi,temp-sense-offset-signed */
	uint8_t		temp_sense_periodic_measurement_enable;	/* adi,temp-sense-periodic-measurement-enable */
	/* Control Out Setup */
	uint8_t		ctrl_outs_enable_mask;	/* adi,ctrl-outs-enable-mask */
	uint8_t		ctrl_outs_index;	/* adi,ctrl-outs-index */
	/* External LNA Control */
	uint32_t	elna_settling_delay_ns;	/* adi,elna-settling-delay-ns */
	uint32_t	elna_gain_mdB;	/* adi,elna-gain-mdB */
	uint32_t	elna_bypass_loss_mdB;	/* adi,elna-bypass-loss-mdB */
	uint8_t		elna_rx1_gpo0_control_enable;	/* adi,elna-rx1-gpo0-control-enable */
	uint8_t		elna_rx2_gpo1_control_enable;	/* adi,elna-rx2-gpo1-control-enable */
	uint8_t		elna_gaintable_all_index_enable;	/* adi,elna-gaintable-all-index-enable */
	/* Digital Interface Control */
	uint8_t		digital_interface_tune_skip_mode;	/* adi,digital-interface-tune-skip-mode */
	uint8_t		digital_interface_tune_fir_disable;	/* adi,digital-interface-tune-fir-disable */
	uint8_t		pp_tx_swap_enable;	/* adi,pp-tx-swap-enable */
	uint8_t		pp_rx_swap_enable;	/* adi,pp-rx-swap-enable */
	uint8_t		tx_channel_swap_enable;	/* adi,tx-channel-swap-enable */
	uint8_t		rx_channel_swap_enable;	/* adi,rx-channel-swap-enable */
	uint8_t		rx_frame_pulse_mode_enable;	/* adi,rx-frame-pulse-mode-enable */
	uint8_t		two_t_two_r_timing_enable;	/* adi,2t2r-timing-enable */
	uint8_t		invert_data_bus_enable;	/* adi,invert-data-bus-enable */
	uint8_t		invert_data_clk_enable;	/* adi,invert-data-clk-enable */
	uint8_t		fdd_alt_word_order_enable;	/* adi,fdd-alt-word-order-enable */
	uint8_t		invert_rx_frame_enable;	/* adi,invert-rx-frame-enable */
	uint8_t		fdd_rx_rate_2tx_enable;	/* adi,fdd-rx-rate-2tx-enable */
	uint8_t		swap_ports_enable;	/* adi,swap-ports-enable */
	uint8_t		single_data_rate_enable;	/* adi,single-data-rate-enable */
	uint8_t		lvds_mode_enable;	/* adi,lvds-mode-enable */
	uint8_t		half_duplex_mode_enable;	/* adi,half-duplex-mode-enable */
	uint8_t		single_port_mode_enable;	/* adi,single-port-mode-enable */
	uint8_t		full_port_enable;	/* adi,full-port-enable */
	uint8_t		full_duplex_swap_bits_enable;	/* adi,full-duplex-swap-bits-enable */
	uint32_t	delay_rx_data;	/* adi,delay-rx-data */
	uint32_t	rx_data_clock_delay;	/* adi,rx-data-clock-delay */
	uint32_t	rx_data_delay;	/* adi,rx-data-delay */
	uint32_t	tx_fb_clock_delay;	/* adi,tx-fb-clock-delay */
	uint32_t	tx_data_delay;	/* adi,tx-data-delay */
	uint32_t	lvds_bias_mV;	/* adi,lvds-bias-mV */
	uint8_t		lvds_rx_onchip_termination_enable;	/* adi,lvds-rx-onchip-termination-enable */
	uint8_t		rx1rx2_phase_inversion_en;	/* adi,rx1-rx2-phase-inversion-enable */
	uint8_t		lvds_invert1_control;	/* adi,lvds-invert1-control */
	uint8_t		lvds_invert2_control;	/* adi,lvds-invert2-control */
	/* GPO Control */
	uint8_t		gpo0_inactive_state_high_enable;	/* adi,gpo0-inactive-state-high-enable */
	uint8_t		gpo1_inactive_state_high_enable;	/* adi,gpo1-inactive-state-high-enable */
	uint8_t		gpo2_inactive_state_high_enable;	/* adi,gpo2-inactive-state-high-enable */
	uint8_t		gpo3_inactive_state_high_enable;	/* adi,gpo3-inactive-state-high-enable */
	uint8_t		gpo0_slave_rx_enable;	/* adi,gpo0-slave-rx-enable */
	uint8_t		gpo0_slave_tx_enable;	/* adi,gpo0-slave-tx-enable */
	uint8_t		gpo1_slave_rx_enable;	/* adi,gpo1-slave-rx-enable */
	uint8_t		gpo1_slave_tx_enable;	/* adi,gpo1-slave-tx-enable */
	uint8_t		gpo2_slave_rx_enable;	/* adi,gpo2-slave-rx-enable */
	uint8_t		gpo2_slave_tx_enable;	/* adi,gpo2-slave-tx-enable */
	uint8_t		gpo3_slave_rx_enable;	/* adi,gpo3-slave-rx-enable */
	uint8_t		gpo3_slave_tx_enable;	/* adi,gpo3-slave-tx-enable */
	uint8_t		gpo0_rx_delay_us;	/* adi,gpo0-rx-delay-us */
	uint8_t		gpo0_tx_delay_us;	/* adi,gpo0-tx-delay-us */
	uint8_t		gpo1_rx_delay_us;	/* adi,gpo1-rx-delay-us */
	uint8_t		gpo1_tx_delay_us;	/* adi,gpo1-tx-delay-us */
	uint8_t		gpo2_rx_delay_us;	/* adi,gpo2-rx-delay-us */
	uint8_t		gpo2_tx_delay_us;	/* adi,gpo2-tx-delay-us */
	uint8_t		gpo3_rx_delay_us;	/* adi,gpo3-rx-delay-us */
	uint8_t		gpo3_tx_delay_us;	/* adi,gpo3-tx-delay-us */
	/* Tx Monitor Control */
	uint32_t	low_high_gain_threshold_mdB;	/* adi,txmon-low-high-thresh */
	uint32_t	low_gain_dB;	/* adi,txmon-low-gain */
	uint32_t	high_gain_dB;	/* adi,txmon-high-gain */
	uint8_t		tx_mon_track_en;	/* adi,txmon-dc-tracking-enable */
	uint8_t		one_shot_mode_en;	/* adi,txmon-one-shot-mode-enable */
	uint32_t	tx_mon_delay;	/* adi,txmon-delay */
	uint32_t	tx_mon_duration;	/* adi,txmon-duration */
	uint32_t	tx1_mon_front_end_gain;	/* adi,txmon-1-front-end-gain */
	uint32_t	tx2_mon_front_end_gain;	/* adi,txmon-2-front-end-gain */
	uint32_t	tx1_mon_lo_cm;	/* adi,txmon-1-lo-cm */
	uint32_t	tx2_mon_lo_cm;	/* adi,txmon-2-lo-cm */
	/* GPIO definitions */
	int32_t		gpio_resetb;	/* reset-gpios */
	/* MCS Sync */
	int32_t		gpio_sync;		/* sync-gpios */
	int32_t		gpio_cal_sw1;	/* cal-sw1-gpios */
	int32_t		gpio_cal_sw2;	/* cal-sw2-gpios */
	/* External LO clocks */
	uint32_t	(*ad9361_rfpll_ext_recalc_rate)(struct refclk_scale *clk_priv);
	int32_t		(*ad9361_rfpll_ext_round_rate)(struct refclk_scale *clk_priv,
			uint32_t rate);
	int32_t		(*ad9361_rfpll_ext_set_rate)(struct refclk_scale *clk_priv,
			uint32_t rate);
} AD9361_InitParam;

typedef struct {
	uint32_t	rx;				/* 1, 2, 3(both) */
	int32_t		rx_gain;		/* -12, -6, 0, 6 */
	uint32_t	rx_dec;			/* 1, 2, 4 */
	int16_t		rx_coef[128];
	uint8_t		rx_coef_size;
	uint32_t	rx_path_clks[6];
	uint32_t	rx_bandwidth;
} AD9361_RXFIRConfig;

typedef struct {
	uint32_t	tx;				/* 1, 2, 3(both) */
	int32_t		tx_gain;		/* -6, 0 */
	uint32_t	tx_int;			/* 1, 2, 4 */
	int16_t		tx_coef[128];
	uint8_t		tx_coef_size;
	uint32_t	tx_path_clks[6];
	uint32_t	tx_bandwidth;
} AD9361_TXFIRConfig;

enum ad9361_ensm_mode {
	ENSM_MODE_TX,
	ENSM_MODE_RX,
	ENSM_MODE_ALERT,
	ENSM_MODE_FDD,
	ENSM_MODE_WAIT,
	ENSM_MODE_SLEEP,
	ENSM_MODE_PINCTRL,
	ENSM_MODE_PINCTRL_FDD_INDEP,
};

#define ENABLE		1
#define DISABLE		0

#define RX1			0
#define RX2			1

#define TX1			0
#define TX2			1

#define A_BALANCED	0
#define B_BALANCED	1
#define C_BALANCED	2
#define A_N			3
#define A_P			4
#define B_N			5
#define B_P			6
#define C_N			7
#define C_P			8
#define TX_MON1		9
#define TX_MON2		10
#define TX_MON1_2	11

#define TXA			0
#define TXB			1

#define MODE_1x1	1
#define MODE_2x2	2

#define HIGHEST_OSR	0
#define NOMINAL_OSR	1

#define INT_LO		0
#define EXT_LO		1

#define ON			0
#define OFF			1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the AD9361 part. */
int32_t ad9361_init (struct ad9361_rf_phy **ad9361_phy,
		     AD9361_InitParam *init_param);
/* Set the Enable State Machine (ENSM) mode. */
int32_t ad9361_set_en_state_machine_mode (struct ad9361_rf_phy *phy,
		uint32_t mode);
/* Get the Enable State Machine (ENSM) mode. */
int32_t ad9361_get_en_state_machine_mode (struct ad9361_rf_phy *phy,
		uint32_t *mode);
/* Set the receive RF gain for the selected channel. */
int32_t ad9361_set_rx_rf_gain (struct ad9361_rf_phy *phy, uint8_t ch,
			       int32_t gain_db);
/* Get current receive RF gain for the selected channel. */
int32_t ad9361_get_rx_rf_gain (struct ad9361_rf_phy *phy, uint8_t ch,
			       int32_t *gain_db);
/* Set the RX RF bandwidth. */
int32_t ad9361_set_rx_rf_bandwidth (struct ad9361_rf_phy *phy,
				    uint32_t bandwidth_hz);
/* Get the RX RF bandwidth. */
int32_t ad9361_get_rx_rf_bandwidth (struct ad9361_rf_phy *phy,
				    uint32_t *bandwidth_hz);
/* Set the RX sampling frequency. */
int32_t ad9361_set_rx_sampling_freq (struct ad9361_rf_phy *phy,
				     uint32_t sampling_freq_hz);
/* Get current RX sampling frequency. */
int32_t ad9361_get_rx_sampling_freq (struct ad9361_rf_phy *phy,
				     uint32_t *sampling_freq_hz);
/* Set the RX LO frequency. */
int32_t ad9361_set_rx_lo_freq (struct ad9361_rf_phy *phy, uint64_t lo_freq_hz);
/* Get current RX LO frequency. */
int32_t ad9361_get_rx_lo_freq (struct ad9361_rf_phy *phy, uint64_t *lo_freq_hz);
/* Switch between internal and external LO. */
int32_t ad9361_set_rx_lo_int_ext(struct ad9361_rf_phy *phy, uint8_t int_ext);
/* Get the RSSI for the selected channel. */
int32_t ad9361_get_rx_rssi (struct ad9361_rf_phy *phy, uint8_t ch,
			    struct rf_rssi *rssi);
/* Set the gain control mode for the selected channel. */
int32_t ad9361_set_rx_gain_control_mode (struct ad9361_rf_phy *phy, uint8_t ch,
		uint8_t gc_mode);
/* Get the gain control mode for the selected channel. */
int32_t ad9361_get_rx_gain_control_mode (struct ad9361_rf_phy *phy, uint8_t ch,
		uint8_t *gc_mode);
/* Set the RX FIR filter configuration. */
int32_t ad9361_set_rx_fir_config (struct ad9361_rf_phy *phy,
				  AD9361_RXFIRConfig fir_cfg);
/* Get the RX FIR filter configuration. */
int32_t ad9361_get_rx_fir_config(struct ad9361_rf_phy *phy, uint8_t rx_ch,
				 AD9361_RXFIRConfig *fir_cfg);
/* Enable/disable the RX FIR filter. */
int32_t ad9361_set_rx_fir_en_dis (struct ad9361_rf_phy *phy, uint8_t en_dis);
/* Get the status of the RX FIR filter. */
int32_t ad9361_get_rx_fir_en_dis (struct ad9361_rf_phy *phy, uint8_t *en_dis);
/* Enable/disable the RX RFDC Tracking. */
int32_t ad9361_set_rx_rfdc_track_en_dis (struct ad9361_rf_phy *phy,
		uint8_t en_dis);
/* Get the status of the RX RFDC Tracking. */
int32_t ad9361_get_rx_rfdc_track_en_dis (struct ad9361_rf_phy *phy,
		uint8_t *en_dis);
/* Enable/disable the RX BasebandDC Tracking. */
int32_t ad9361_set_rx_bbdc_track_en_dis (struct ad9361_rf_phy *phy,
		uint8_t en_dis);
/* Get the status of the RX BasebandDC Tracking. */
int32_t ad9361_get_rx_bbdc_track_en_dis (struct ad9361_rf_phy *phy,
		uint8_t *en_dis);
/* Enable/disable the RX Quadrature Tracking. */
int32_t ad9361_set_rx_quad_track_en_dis (struct ad9361_rf_phy *phy,
		uint8_t en_dis);
/* Get the status of the RX Quadrature Tracking. */
int32_t ad9361_get_rx_quad_track_en_dis (struct ad9361_rf_phy *phy,
		uint8_t *en_dis);
/* Set the RX RF input port. */
int32_t ad9361_set_rx_rf_port_input (struct ad9361_rf_phy *phy, uint32_t mode);
/* Get the selected RX RF input port. */
int32_t ad9361_get_rx_rf_port_input (struct ad9361_rf_phy *phy, uint32_t *mode);
/* Store RX fastlock profile. */
int32_t ad9361_rx_fastlock_store(struct ad9361_rf_phy *phy, uint32_t profile);
/* Recall RX fastlock profile. */
int32_t ad9361_rx_fastlock_recall(struct ad9361_rf_phy *phy, uint32_t profile);
/* Load RX fastlock profile. */
int32_t ad9361_rx_fastlock_load(struct ad9361_rf_phy *phy, uint32_t profile,
				uint8_t *values);
/* Save RX fastlock profile. */
int32_t ad9361_rx_fastlock_save(struct ad9361_rf_phy *phy, uint32_t profile,
				uint8_t *values);
/* Power down the RX Local Oscillator. */
int32_t ad9361_rx_lo_powerdown(struct ad9361_rf_phy *phy, uint8_t option);
/* Get the RX Local Oscillator power status. */
int32_t ad9361_get_rx_lo_power(struct ad9361_rf_phy *phy, uint8_t *option);
/* Set the transmit attenuation for the selected channel. */
int32_t ad9361_set_tx_attenuation (struct ad9361_rf_phy *phy, uint8_t ch,
				   uint32_t attenuation_mdb);
/* Get current transmit attenuation for the selected channel. */
int32_t ad9361_get_tx_attenuation (struct ad9361_rf_phy *phy, uint8_t ch,
				   uint32_t *attenuation_mdb);
/* Set the TX RF bandwidth. */
int32_t ad9361_set_tx_rf_bandwidth (struct ad9361_rf_phy *phy,
				    uint32_t bandwidth_hz);
/* Get the TX RF bandwidth. */
int32_t ad9361_get_tx_rf_bandwidth (struct ad9361_rf_phy *phy,
				    uint32_t *bandwidth_hz);
/* Set the TX sampling frequency. */
int32_t ad9361_set_tx_sampling_freq (struct ad9361_rf_phy *phy,
				     uint32_t sampling_freq_hz);
/* Get current TX sampling frequency. */
int32_t ad9361_get_tx_sampling_freq (struct ad9361_rf_phy *phy,
				     uint32_t *sampling_freq_hz);
/* Set the TX LO frequency. */
int32_t ad9361_set_tx_lo_freq (struct ad9361_rf_phy *phy, uint64_t lo_freq_hz);
/* Get current TX LO frequency. */
int32_t ad9361_get_tx_lo_freq (struct ad9361_rf_phy *phy, uint64_t *lo_freq_hz);
/* Switch between internal and external LO. */
int32_t ad9361_set_tx_lo_int_ext(struct ad9361_rf_phy *phy, uint8_t int_ext);
/* Set the TX FIR filter configuration. */
int32_t ad9361_set_tx_fir_config (struct ad9361_rf_phy *phy,
				  AD9361_TXFIRConfig fir_cfg);
/* Get the TX FIR filter configuration. */
int32_t ad9361_get_tx_fir_config(struct ad9361_rf_phy *phy, uint8_t tx_ch,
				 AD9361_TXFIRConfig *fir_cfg);
/* Enable/disable the TX FIR filter. */
int32_t ad9361_set_tx_fir_en_dis (struct ad9361_rf_phy *phy, uint8_t en_dis);
/* Get the status of the TX FIR filter. */
int32_t ad9361_get_tx_fir_en_dis (struct ad9361_rf_phy *phy, uint8_t *en_dis);
/* Get the TX RSSI for the selected channel. */
int32_t ad9361_get_tx_rssi (struct ad9361_rf_phy *phy, uint8_t ch,
			    uint32_t *rssi_db_x_1000);
/* Set the TX RF output port. */
int32_t ad9361_set_tx_rf_port_output (struct ad9361_rf_phy *phy, uint32_t mode);
/* Get the selected TX RF output port. */
int32_t ad9361_get_tx_rf_port_output (struct ad9361_rf_phy *phy,
				      uint32_t *mode);
/* Enable/disable the auto calibration. */
int32_t ad9361_set_tx_auto_cal_en_dis (struct ad9361_rf_phy *phy,
				       uint8_t en_dis);
/* Get the status of the auto calibration flag. */
int32_t ad9361_get_tx_auto_cal_en_dis (struct ad9361_rf_phy *phy,
				       uint8_t *en_dis);
/* Store TX fastlock profile. */
int32_t ad9361_tx_fastlock_store(struct ad9361_rf_phy *phy, uint32_t profile);
/* Recall TX fastlock profile. */
int32_t ad9361_tx_fastlock_recall(struct ad9361_rf_phy *phy, uint32_t profile);
/* Load TX fastlock profile. */
int32_t ad9361_tx_fastlock_load(struct ad9361_rf_phy *phy, uint32_t profile,
				uint8_t *values);
/* Save TX fastlock profile. */
int32_t ad9361_tx_fastlock_save(struct ad9361_rf_phy *phy, uint32_t profile,
				uint8_t *values);
/* Power down the TX Local Oscillator. */
int32_t ad9361_tx_lo_powerdown(struct ad9361_rf_phy *phy, uint8_t option);
/* Get the TX Local Oscillator power status. */
int32_t ad9361_get_tx_lo_power(struct ad9361_rf_phy *phy, uint8_t *option);
/* Set the RX and TX path rates. */
int32_t ad9361_set_trx_path_clks(struct ad9361_rf_phy *phy,
				 uint32_t *rx_path_clks, uint32_t *tx_path_clks);
/* Get the RX and TX path rates. */
int32_t ad9361_get_trx_path_clks(struct ad9361_rf_phy *phy,
				 uint32_t *rx_path_clks, uint32_t *tx_path_clks);
/* Set the number of channels mode. */
int32_t ad9361_set_no_ch_mode(struct ad9361_rf_phy *phy, uint8_t no_ch_mode);
/* Do multi chip synchronization. */
int32_t ad9361_do_mcs(struct ad9361_rf_phy *phy_master,
		      struct ad9361_rf_phy *phy_slave);
/* Enable/disable the TRX FIR filters. */
int32_t ad9361_set_trx_fir_en_dis (struct ad9361_rf_phy *phy, uint8_t en_dis);
/* Set the OSR rate governor. */
int32_t ad9361_set_trx_rate_gov (struct ad9361_rf_phy *phy, uint32_t rate_gov);
/* Get the OSR rate governor. */
int32_t ad9361_get_trx_rate_gov (struct ad9361_rf_phy *phy, uint32_t *rate_gov);
/* Perform the selected calibration. */
int32_t ad9361_do_calib(struct ad9361_rf_phy *phy, uint32_t cal, int32_t arg);
/* Load and enable TRX FIR filters configurations. */
int32_t ad9361_trx_load_enable_fir(struct ad9361_rf_phy *phy,
				   AD9361_RXFIRConfig rx_fir_cfg,
				   AD9361_TXFIRConfig tx_fir_cfg);
/* Do DCXO coarse tuning. */
int32_t ad9361_do_dcxo_tune_coarse(struct ad9361_rf_phy *phy,
				   uint32_t coarse);
/* Do DCXO fine tuning. */
int32_t ad9361_do_dcxo_tune_fine(struct ad9361_rf_phy *phy,
				 uint32_t fine);
/* Get the temperature. */
int32_t ad9361_get_temperature(struct ad9361_rf_phy *phy,
			       int32_t *temp);
#endif

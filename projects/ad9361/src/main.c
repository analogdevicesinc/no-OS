/***************************************************************************//**
 *   @file   ad9361/src/main.c
 *   @brief  Implementation of Main Function.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <inttypes.h>
#include "app_config.h"
#include "ad9361_api.h"
#include "parameters.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#ifdef XILINX_PLATFORM
#include <xparameters.h>
#include <xil_cache.h>
#include "spi_extra.h"
#include "gpio_extra.h"
#include "no_os_irq.h"
#endif
#ifdef LINUX_PLATFORM
#include "linux_spi.h"
#include "linux_gpio.h"
#else
#include "irq_extra.h"
#endif //LINUX

#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "no_os_error.h"

#ifdef IIO_SUPPORT

#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_ad9361.h"
#include "no_os_uart.h"
#include "iio_app.h"

#ifdef XILINX_PLATFORM
#include "uart_extra.h"
#include "xil_cache.h"
#endif //XILINX

#if defined LINUX_PLATFORM || defined GENERIC_PLATFORM
static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
static uint8_t out_buff[MAX_SIZE_BASE_ADDR];
#endif

#endif // IIO_SUPPORT

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/

#if defined(DAC_DMA_EXAMPLE) || defined(IIO_SUPPORT)
uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((
			aligned));
#endif

#define AD9361_ADC_DAC_BYTES_PER_SAMPLE 2

#ifdef XILINX_PLATFORM
struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
	.type = SPI_PL,
#else
	.type = SPI_PS,
#endif
	.flags = 0
};

struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
	.type = GPIO_PL,
#else
	.type = GPIO_PS,
#endif
	.device_id = GPIO_DEVICE_ID
};

#define GPIO_OPS	&xil_gpio_ops
#define SPI_OPS		&xil_spi_ops
#define GPIO_PARAM	&xil_gpio_param
#define SPI_PARAM	&xil_spi_param
#endif

#ifdef GENERIC_PLATFORM
#define GPIO_OPS	&generic_gpio_ops
#define SPI_OPS		&generic_spi_ops
#define GPIO_PARAM	NULL
#define SPI_PARAM	NULL
#endif
#ifdef XILINX_PLATFORM
#endif
#ifdef LINUX_PLATFORM
#define GPIO_OPS	&linux_gpio_ops
#define SPI_OPS		&linux_spi_ops
#define GPIO_PARAM	NULL
#define SPI_PARAM	NULL
#endif

struct axi_adc_init rx_adc_init = {
	"cf-ad9361-lpc",
	RX_CORE_BASEADDR,
	4
};
struct axi_dac_init tx_dac_init = {
	"cf-ad9361-dds-core-lpc",
	TX_CORE_BASEADDR,
	4,
	NULL
};
struct axi_dmac_init rx_dmac_init = {
	"rx_dmac",
	CF_AD9361_RX_DMA_BASEADDR,
	DMA_DEV_TO_MEM,
	0
};
struct axi_dmac *rx_dmac;
struct axi_dmac_init tx_dmac_init = {
	"tx_dmac",
	CF_AD9361_TX_DMA_BASEADDR,
	DMA_MEM_TO_DEV,
	DMA_CYCLIC
};
struct axi_dmac *tx_dmac;

AD9361_InitParam default_init_param = {
	/* Device selection */
	ID_AD9361,	// dev_sel
	/* Reference Clock */
	40000000UL,	//reference_clk_rate
	/* Base Configuration */
	1,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
	1,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
	1,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
	1,		//frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
	0,		//frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
	0,		//tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
	0,		//tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
	0,		//tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
	0,		//rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
	0,		//rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
	0,		//tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
	0,		//external_rx_lo_enable *** adi,external-rx-lo-enable
	0,		//external_tx_lo_enable *** adi,external-tx-lo-enable
	5,		//dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
	6,		//dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
	5,		//dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
	0x28,	//dc_offset_count_high_range *** adi,dc-offset-count-high-range
	0x32,	//dc_offset_count_low_range *** adi,dc-offset-count-low-range
	0,		//split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
	MAX_SYNTH_FREF,	//trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
	0,		// qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
	/* ENSM Control */
	0,		//ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
	0,		//ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
	/* LO Control */
	2400000000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
	2400000000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
	1,				//tx_lo_powerdown_managed_enable *** adi,tx-lo-powerdown-managed-enable
	/* Rate & BW Control */
	{983040000, 245760000, 122880000, 61440000, 30720000, 30720000},// rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
	{983040000, 122880000, 122880000, 61440000, 30720000, 30720000},// tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
	18000000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
	18000000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
	/* RF Port Control */
	0,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select
	0,		//tx_rf_port_input_select *** adi,tx-rf-port-input-select
	/* TX Attenuation Control */
	10000,	//tx_attenuation_mdB *** adi,tx-attenuation-mdB
	0,		//update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
	/* Reference Clock Control */
	0,		//xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
	{8, 5920},	//dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
	CLKOUT_DISABLE,	//clk_output_mode_select *** adi,clk-output-mode-select
	/* Gain Control */
	2,		//gc_rx1_mode *** adi,gc-rx1-mode
	2,		//gc_rx2_mode *** adi,gc-rx2-mode
	58,		//gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
	4,		//gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
	47,		//gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
	8192,	//gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
	0,		//gc_dig_gain_enable *** adi,gc-dig-gain-enable
	800,	//gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
	704,	//gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
	24,		//gc_low_power_thresh *** adi,gc-low-power-thresh
	15,		//gc_max_dig_gain *** adi,gc-max-dig-gain
	0,		//gc_use_rx_fir_out_for_dec_pwr_meas_enable *** adi,gc-use-rx-fir-out-for-dec-pwr-meas-enable
	/* Gain MGC Control */
	2,		//mgc_dec_gain_step *** adi,mgc-dec-gain-step
	2,		//mgc_inc_gain_step *** adi,mgc-inc-gain-step
	0,		//mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
	0,		//mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
	0,		//mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
	/* Gain AGC Control */
	10,		//agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
	2,		//agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
	0,		//agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
	10,		//agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
	4,		//agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
	3,		//agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
	1000,	// agc_gain_update_interval_us *** adi,agc-gain-update-interval-us
	0,		//agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
	0,		//agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
	10,		//agc_inner_thresh_high *** adi,agc-inner-thresh-high
	1,		//agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
	12,		//agc_inner_thresh_low *** adi,agc-inner-thresh-low
	1,		//agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
	10,		//agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
	2,		//agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
	10,		//agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
	5,		//agc_outer_thresh_high *** adi,agc-outer-thresh-high
	2,		//agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
	18,		//agc_outer_thresh_low *** adi,agc-outer-thresh-low
	2,		//agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
	1,		//agc_attack_delay_extra_margin_us; *** adi,agc-attack-delay-extra-margin-us
	0,		//agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
	/* Fast AGC */
	64,		//fagc_dec_pow_measuremnt_duration ***  adi,fagc-dec-pow-measurement-duration
	260,	//fagc_state_wait_time_ns ***  adi,fagc-state-wait-time-ns
	/* Fast AGC - Low Power */
	0,		//fagc_allow_agc_gain_increase ***  adi,fagc-allow-agc-gain-increase-enable
	5,		//fagc_lp_thresh_increment_time ***  adi,fagc-lp-thresh-increment-time
	1,		//fagc_lp_thresh_increment_steps ***  adi,fagc-lp-thresh-increment-steps
	/* Fast AGC - Lock Level (Lock Level is set via slow AGC inner high threshold) */
	1,		//fagc_lock_level_lmt_gain_increase_en ***  adi,fagc-lock-level-lmt-gain-increase-enable
	5,		//fagc_lock_level_gain_increase_upper_limit ***  adi,fagc-lock-level-gain-increase-upper-limit
	/* Fast AGC - Peak Detectors and Final Settling */
	1,		//fagc_lpf_final_settling_steps ***  adi,fagc-lpf-final-settling-steps
	1,		//fagc_lmt_final_settling_steps ***  adi,fagc-lmt-final-settling-steps
	3,		//fagc_final_overrange_count ***  adi,fagc-final-overrange-count
	/* Fast AGC - Final Power Test */
	0,		//fagc_gain_increase_after_gain_lock_en ***  adi,fagc-gain-increase-after-gain-lock-enable
	/* Fast AGC - Unlocking the Gain */
	0,		//fagc_gain_index_type_after_exit_rx_mode ***  adi,fagc-gain-index-type-after-exit-rx-mode
	1,		//fagc_use_last_lock_level_for_set_gain_en ***  adi,fagc-use-last-lock-level-for-set-gain-enable
	1,		//fagc_rst_gla_stronger_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable
	5,		//fagc_optimized_gain_offset ***  adi,fagc-optimized-gain-offset
	10,		//fagc_rst_gla_stronger_sig_thresh_above_ll ***  adi,fagc-rst-gla-stronger-sig-thresh-above-ll
	1,		//fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable
	1,		//fagc_rst_gla_engergy_lost_goto_optim_gain_en ***  adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable
	10,		//fagc_rst_gla_engergy_lost_sig_thresh_below_ll ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll
	8,		//fagc_energy_lost_stronger_sig_gain_lock_exit_cnt ***  adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt
	1,		//fagc_rst_gla_large_adc_overload_en ***  adi,fagc-rst-gla-large-adc-overload-enable
	1,		//fagc_rst_gla_large_lmt_overload_en ***  adi,fagc-rst-gla-large-lmt-overload-enable
	0,		//fagc_rst_gla_en_agc_pulled_high_en ***  adi,fagc-rst-gla-en-agc-pulled-high-enable
	0,		//fagc_rst_gla_if_en_agc_pulled_high_mode ***  adi,fagc-rst-gla-if-en-agc-pulled-high-mode
	64,		//fagc_power_measurement_duration_in_state5 ***  adi,fagc-power-measurement-duration-in-state5
	2,		//fagc_large_overload_inc_steps *** adi,fagc-adc-large-overload-inc-steps
	/* RSSI Control */
	1,		//rssi_delay *** adi,rssi-delay
	1000,	//rssi_duration *** adi,rssi-duration
	3,		//rssi_restart_mode *** adi,rssi-restart-mode
	0,		//rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
	1,		//rssi_wait *** adi,rssi-wait
	/* Aux ADC Control */
	256,	//aux_adc_decimation *** adi,aux-adc-decimation
	40000000UL,	//aux_adc_rate *** adi,aux-adc-rate
	/* AuxDAC Control */
	1,		//aux_dac_manual_mode_enable ***  adi,aux-dac-manual-mode-enable
	0,		//aux_dac1_default_value_mV ***  adi,aux-dac1-default-value-mV
	0,		//aux_dac1_active_in_rx_enable ***  adi,aux-dac1-active-in-rx-enable
	0,		//aux_dac1_active_in_tx_enable ***  adi,aux-dac1-active-in-tx-enable
	0,		//aux_dac1_active_in_alert_enable ***  adi,aux-dac1-active-in-alert-enable
	0,		//aux_dac1_rx_delay_us ***  adi,aux-dac1-rx-delay-us
	0,		//aux_dac1_tx_delay_us ***  adi,aux-dac1-tx-delay-us
	0,		//aux_dac2_default_value_mV ***  adi,aux-dac2-default-value-mV
	0,		//aux_dac2_active_in_rx_enable ***  adi,aux-dac2-active-in-rx-enable
	0,		//aux_dac2_active_in_tx_enable ***  adi,aux-dac2-active-in-tx-enable
	0,		//aux_dac2_active_in_alert_enable ***  adi,aux-dac2-active-in-alert-enable
	0,		//aux_dac2_rx_delay_us ***  adi,aux-dac2-rx-delay-us
	0,		//aux_dac2_tx_delay_us ***  adi,aux-dac2-tx-delay-us
	/* Temperature Sensor Control */
	256,	//temp_sense_decimation *** adi,temp-sense-decimation
	1000,	//temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
	0xCE,	//temp_sense_offset_signed *** adi,temp-sense-offset-signed
	1,		//temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
	/* Control Out Setup */
	0xFF,	//ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
	0,		//ctrl_outs_index *** adi,ctrl-outs-index
	/* External LNA Control */
	0,		//elna_settling_delay_ns *** adi,elna-settling-delay-ns
	0,		//elna_gain_mdB *** adi,elna-gain-mdB
	0,		//elna_bypass_loss_mdB *** adi,elna-bypass-loss-mdB
	0,		//elna_rx1_gpo0_control_enable *** adi,elna-rx1-gpo0-control-enable
	0,		//elna_rx2_gpo1_control_enable *** adi,elna-rx2-gpo1-control-enable
	0,		//elna_gaintable_all_index_enable *** adi,elna-gaintable-all-index-enable
	/* Digital Interface Control */
	0,		//digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
	0,		//digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
	1,		//pp_tx_swap_enable *** adi,pp-tx-swap-enable
	1,		//pp_rx_swap_enable *** adi,pp-rx-swap-enable
	0,		//tx_channel_swap_enable *** adi,tx-channel-swap-enable
	0,		//rx_channel_swap_enable *** adi,rx-channel-swap-enable
	1,		//rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
	0,		//two_t_two_r_timing_enable *** adi,2t2r-timing-enable
	0,		//invert_data_bus_enable *** adi,invert-data-bus-enable
	0,		//invert_data_clk_enable *** adi,invert-data-clk-enable
	0,		//fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
	0,		//invert_rx_frame_enable *** adi,invert-rx-frame-enable
	0,		//fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
	0,		//swap_ports_enable *** adi,swap-ports-enable
	0,		//single_data_rate_enable *** adi,single-data-rate-enable
	1,		//lvds_mode_enable *** adi,lvds-mode-enable
	0,		//half_duplex_mode_enable *** adi,half-duplex-mode-enable
	0,		//single_port_mode_enable *** adi,single-port-mode-enable
	0,		//full_port_enable *** adi,full-port-enable
	0,		//full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
	0,		//delay_rx_data *** adi,delay-rx-data
	0,		//rx_data_clock_delay *** adi,rx-data-clock-delay
	4,		//rx_data_delay *** adi,rx-data-delay
	7,		//tx_fb_clock_delay *** adi,tx-fb-clock-delay
	0,		//tx_data_delay *** adi,tx-data-delay
#ifdef ALTERA_PLATFORM
	300,	//lvds_bias_mV *** adi,lvds-bias-mV
#else
	150,	//lvds_bias_mV *** adi,lvds-bias-mV
#endif
	1,		//lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
	0,		//rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
	0xFF,	//lvds_invert1_control *** adi,lvds-invert1-control
	0x0F,	//lvds_invert2_control *** adi,lvds-invert2-control
	/* GPO Control */
	0,		//gpo_manual_mode_enable *** adi,gpo-manual-mode-enable
	0,		//gpo_manual_mode_enable_mask *** adi,gpo-manual-mode-enable-mask
	0,		//gpo0_inactive_state_high_enable *** adi,gpo0-inactive-state-high-enable
	0,		//gpo1_inactive_state_high_enable *** adi,gpo1-inactive-state-high-enable
	0,		//gpo2_inactive_state_high_enable *** adi,gpo2-inactive-state-high-enable
	0,		//gpo3_inactive_state_high_enable *** adi,gpo3-inactive-state-high-enable
	0,		//gpo0_slave_rx_enable *** adi,gpo0-slave-rx-enable
	0,		//gpo0_slave_tx_enable *** adi,gpo0-slave-tx-enable
	0,		//gpo1_slave_rx_enable *** adi,gpo1-slave-rx-enable
	0,		//gpo1_slave_tx_enable *** adi,gpo1-slave-tx-enable
	0,		//gpo2_slave_rx_enable *** adi,gpo2-slave-rx-enable
	0,		//gpo2_slave_tx_enable *** adi,gpo2-slave-tx-enable
	0,		//gpo3_slave_rx_enable *** adi,gpo3-slave-rx-enable
	0,		//gpo3_slave_tx_enable *** adi,gpo3-slave-tx-enable
	0,		//gpo0_rx_delay_us *** adi,gpo0-rx-delay-us
	0,		//gpo0_tx_delay_us *** adi,gpo0-tx-delay-us
	0,		//gpo1_rx_delay_us *** adi,gpo1-rx-delay-us
	0,		//gpo1_tx_delay_us *** adi,gpo1-tx-delay-us
	0,		//gpo2_rx_delay_us *** adi,gpo2-rx-delay-us
	0,		//gpo2_tx_delay_us *** adi,gpo2-tx-delay-us
	0,		//gpo3_rx_delay_us *** adi,gpo3-rx-delay-us
	0,		//gpo3_tx_delay_us *** adi,gpo3-tx-delay-us
	/* Tx Monitor Control */
	37000,	//low_high_gain_threshold_mdB *** adi,txmon-low-high-thresh
	0,		//low_gain_dB *** adi,txmon-low-gain
	24,		//high_gain_dB *** adi,txmon-high-gain
	0,		//tx_mon_track_en *** adi,txmon-dc-tracking-enable
	0,		//one_shot_mode_en *** adi,txmon-one-shot-mode-enable
	511,	//tx_mon_delay *** adi,txmon-delay
	8192,	//tx_mon_duration *** adi,txmon-duration
	2,		//tx1_mon_front_end_gain *** adi,txmon-1-front-end-gain
	2,		//tx2_mon_front_end_gain *** adi,txmon-2-front-end-gain
	48,		//tx1_mon_lo_cm *** adi,txmon-1-lo-cm
	48,		//tx2_mon_lo_cm *** adi,txmon-2-lo-cm
	/* GPIO definitions */
	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_resetb *** reset-gpios
	/* MCS Sync */
	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_sync *** sync-gpios

	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_cal_sw1 *** cal-sw1-gpios

	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_cal_sw2 *** cal-sw2-gpios

	{
		.device_id = SPI_DEVICE_ID,
		.mode = NO_OS_SPI_MODE_1,
		.chip_select = SPI_CS,
		.platform_ops = SPI_OPS,
		.extra = SPI_PARAM
	},

	/* External LO clocks */
	NULL,	//(*ad9361_rfpll_ext_recalc_rate)()
	NULL,	//(*ad9361_rfpll_ext_round_rate)()
	NULL,	//(*ad9361_rfpll_ext_set_rate)()
#ifndef AXI_ADC_NOT_PRESENT
	&rx_adc_init,	// *rx_adc_init
	&tx_dac_init,   // *tx_dac_init
#endif
};

AD9361_RXFIRConfig rx_fir_config = {	// BPF PASSBAND 3/20 fs to 1/4 fs
	3, // rx
	0, // rx_gain
	1, // rx_dec
	{
		-4, -6, -37, 35, 186, 86, -284, -315,
			107, 219, -4, 271, 558, -307, -1182, -356,
			658, 157, 207, 1648, 790, -2525, -2553, 748,
			865, -476, 3737, 6560, -3583, -14731, -5278, 14819,
			14819, -5278, -14731, -3583, 6560, 3737, -476, 865,
			748, -2553, -2525, 790, 1648, 207, 157, 658,
			-356, -1182, -307, 558, 271, -4, 219, 107,
			-315, -284, 86, 186, 35, -37, -6, -4,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0
		}, // rx_coef[128]
	64, // rx_coef_size
	{0, 0, 0, 0, 0, 0}, //rx_path_clks[6]
	0 // rx_bandwidth
};

AD9361_TXFIRConfig tx_fir_config = {	// BPF PASSBAND 3/20 fs to 1/4 fs
	3, // tx
	-6, // tx_gain
	1, // tx_int
	{
		-4, -6, -37, 35, 186, 86, -284, -315,
			107, 219, -4, 271, 558, -307, -1182, -356,
			658, 157, 207, 1648, 790, -2525, -2553, 748,
			865, -476, 3737, 6560, -3583, -14731, -5278, 14819,
			14819, -5278, -14731, -3583, 6560, 3737, -476, 865,
			748, -2553, -2525, 790, 1648, 207, 157, 658,
			-356, -1182, -307, 558, 271, -4, 219, 107,
			-315, -284, 86, 186, 35, -37, -6, -4,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0
		}, // tx_coef[128]
	64, // tx_coef_size
	{0, 0, 0, 0, 0, 0}, // tx_path_clks[6]
	0 // tx_bandwidth
};
struct ad9361_rf_phy *ad9361_phy;
#ifdef FMCOMMS5
struct ad9361_rf_phy *ad9361_phy_b;
#endif


/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;
#ifdef XILINX_PLATFORM
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	default_init_param.spi_param.extra = &xil_spi_param;
	default_init_param.spi_param.platform_ops = &xil_spi_ops;
#endif

#ifdef ALTERA_PLATFORM
	default_init_param.spi_param.platform_ops = &altera_spi_ops;

	if (altera_bridge_init()) {
		printf("Altera Bridge Init Error!\n");
		return -1;
	}
#endif

	// NOTE: The user has to choose the GPIO numbers according to desired
	// carrier board.
	default_init_param.gpio_resetb.number = GPIO_RESET_PIN;

#ifdef FMCOMMS5
	default_init_param.gpio_sync.number = GPIO_SYNC_PIN;
	default_init_param.gpio_cal_sw1.number = GPIO_CAL_SW1_PIN;
	default_init_param.gpio_cal_sw2.number = GPIO_CAL_SW2_PIN;
	default_init_param.rx1rx2_phase_inversion_en = 1;
#else
	default_init_param.gpio_sync.number = -1;
	default_init_param.gpio_cal_sw1.number = -1;
	default_init_param.gpio_cal_sw2.number = -1;
#endif

	if (AD9364_DEVICE)
		default_init_param.dev_sel = ID_AD9364;
	if (AD9363A_DEVICE)
		default_init_param.dev_sel = ID_AD9363A;

#if defined FMCOMMS5 || defined ADI_RF_SOM || defined ADI_RF_SOM_CMOS
	default_init_param.xo_disable_use_ext_refclk_enable = 1;
#endif

#ifdef ADI_RF_SOM_CMOS
	default_init_param.swap_ports_enable = 1;
	default_init_param.lvds_mode_enable = 0;
	default_init_param.lvds_rx_onchip_termination_enable = 0;
	default_init_param.full_port_enable = 1;
	default_init_param.digital_interface_tune_fir_disable = 1;
#endif

	ad9361_init(&ad9361_phy, &default_init_param);

	ad9361_set_tx_fir_config(ad9361_phy, tx_fir_config);
	ad9361_set_rx_fir_config(ad9361_phy, rx_fir_config);

#ifdef FMCOMMS5
#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_sync);
#endif
	default_init_param.spi_param.chip_select = SPI_CS_2;
	default_init_param.gpio_resetb.number = GPIO_RESET_PIN_2;
#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_resetb);
#endif
	default_init_param.gpio_sync.number = -1;
	default_init_param.gpio_cal_sw1.number = -1;
	default_init_param.gpio_cal_sw2.number = -1;
	default_init_param.rx_synthesizer_frequency_hz = 2300000000UL;
	default_init_param.tx_synthesizer_frequency_hz = 2300000000UL;

	rx_adc_init.base = AD9361_RX_1_BASEADDR;
	tx_dac_init.base = AD9361_TX_1_BASEADDR;

	ad9361_init(&ad9361_phy_b, &default_init_param);

	ad9361_set_tx_fir_config(ad9361_phy_b, tx_fir_config);
	ad9361_set_rx_fir_config(ad9361_phy_b, rx_fir_config);
#endif
	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status < 0) {
		printf("axi_dmac_init tx init error: %"PRIi32"\n", status);
		return status;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status < 0) {
		printf("axi_dmac_init rx init error: %"PRIi32"\n", status);
		return status;
	}
#ifndef AXI_ADC_NOT_PRESENT
#if defined XILINX_PLATFORM || defined LINUX_PLATFORM || defined ALTERA_PLATFORM
#ifdef DAC_DMA_EXAMPLE
#ifdef FMCOMMS5
	axi_dac_init(&ad9361_phy->tx_dac, &tx_dac_init);
	axi_dac_set_datasel(ad9361_phy_b->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
#endif
	axi_dac_init(&ad9361_phy->tx_dac, &tx_dac_init);
	axi_adc_init(&ad9361_phy->rx_adc, &rx_adc_init);
	extern const uint32_t sine_lut_iq[1024];
	axi_dac_set_datasel(ad9361_phy->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	axi_dac_load_custom_data(ad9361_phy->tx_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq),
				 (uintptr_t)dac_buffer);
#ifdef XILINX_PLATFORM
	Xil_DCacheFlush();
#endif
#else
#ifdef FMCOMMS5
	axi_dac_init(&ad9361_phy_b->tx_dac, &tx_dac_init);
	axi_dac_set_datasel(ad9361_phy_b->tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
#endif
	axi_dac_init(&ad9361_phy->tx_dac, &tx_dac_init);
	axi_dac_set_datasel(ad9361_phy->tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
#endif
#endif
#endif

#ifdef FMCOMMS5
	ad9361_do_mcs(ad9361_phy, ad9361_phy_b);
#endif

#ifndef AXI_ADC_NOT_PRESENT
#if (defined XILINX_PLATFORM || defined ALTERA_PLATFORM) && \
	(defined ADC_DMA_EXAMPLE)
	uint32_t samples = 16384;
#if (defined ADC_DMA_IRQ_EXAMPLE)
	/**
	 * Xilinx platform dependent initialization for IRQ.
	 */
	struct xil_irq_init_param xil_irq_init_par = {
		.type = IRQ_PS,
	};

	/**
	 * IRQ initial configuration.
	 */
	struct no_os_irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.platform_ops = &xil_irq_ops,
		.extra = &xil_irq_init_par,
	};

	/**
	 * IRQ instance.
	 */
	struct no_os_irq_ctrl_desc *irq_desc;

	status = no_os_irq_ctrl_init(&irq_desc, &irq_init_param);
	if(status < 0)
		return status;

	status = no_os_irq_global_enable(irq_desc);
	if (status < 0)
		return status;

	struct no_os_callback_desc rx_dmac_callback = {
		.ctx = rx_dmac,
		.legacy_callback = axi_dmac_default_isr,
	};

	status = no_os_irq_register_callback(irq_desc,
					     XPAR_FABRIC_AXI_AD9361_ADC_DMA_IRQ_INTR, &rx_dmac_callback);
	if(status < 0)
		return status;

	status = no_os_irq_trigger_level_set(irq_desc,
					     XPAR_FABRIC_AXI_AD9361_ADC_DMA_IRQ_INTR, NO_OS_IRQ_LEVEL_HIGH);
	if(status < 0)
		return status;

	status = no_os_irq_enable(irq_desc, XPAR_FABRIC_AXI_AD9361_ADC_DMA_IRQ_INTR);
	if(status < 0)
		return status;

	samples = 2097150;
#endif
	// NOTE: To prevent unwanted data loss, it's recommended to invalidate
	// cache after each axi_dmac_transfer() call, keeping in mind that the
	// size of the capture and the start address must be alinged to the size
	// of the cache line.

#ifdef DAC_DMA_EXAMPLE
#ifdef ADC_DMA_IRQ_EXAMPLE
	struct no_os_callback_desc tx_dmac_callback = {
		.ctx = tx_dmac,
		.legacy_callback = axi_dmac_default_isr,
	};

	status = no_os_irq_register_callback(irq_desc,
					     XPAR_FABRIC_AXI_AD9361_DAC_DMA_IRQ_INTR, &tx_dmac_callback);
	if(status < 0)
		return status;

	status = no_os_irq_enable(irq_desc, XPAR_FABRIC_AXI_AD9361_DAC_DMA_IRQ_INTR);
	if(status < 0)
		return status;
#endif

#ifdef FMCOMMS5
	axi_dmac_transfer_nonblocking(tx_dmac, DAC_DDR_BASEADDR,
				      samples * AD9361_ADC_DAC_BYTES_PER_SAMPLE *
				      (ad9361_phy_b->tx_dac->num_channels + ad9361_phy->tx_dac->num_channels));
#else
	axi_dmac_transfer_nonblocking(tx_dmac, (uintptr_t)dac_buffer,
				      sizeof(sine_lut_iq));
	no_os_mdelay(1000);
#endif
#endif
#ifdef FMCOMMS5
	axi_dmac_transfer(rx_dmac, ADC_DDR_BASEADDR,
			  samples * AD9361_ADC_DAC_BYTES_PER_SAMPLE *
			  (ad9361_phy_b->rx_adc->num_channels + ad9361_phy->rx_adc->num_channels));
#else
	axi_dmac_transfer(rx_dmac, (uintptr_t)adc_buffer, sizeof(adc_buffer));
#endif
#ifdef XILINX_PLATFORM
#ifdef FMCOMMS5
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR,
				  samples * AD9361_ADC_DAC_BYTES_PER_SAMPLE * (ad9361_phy_b->rx_adc->num_channels
						  +
						  ad9361_phy->rx_adc->num_channels));
#else
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));
#endif
	printf("DAC_DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%lu\n",
	       (uintptr_t)adc_buffer, NO_OS_ARRAY_SIZE(adc_buffer), rx_adc_init.num_channels,
	       8 * sizeof(adc_buffer[0]));
#endif
#endif
#endif

#ifdef IIO_SUPPORT

	/**
	 * iio application configurations.
	 */
	//struct iio_init_param iio_init_par;

	/**
	 * iio axi adc configurations.
	 */
	struct iio_axi_adc_init_param iio_axi_adc_init_par;

	/**
	 * iio axi dac configurations.
	 */
	struct iio_axi_dac_init_param iio_axi_dac_init_par;

	/**
	 * iio ad9361 configurations.
	 */
	struct iio_ad9361_init_param iio_ad9361_init_param;

	/**
	 * iio instance descriptor.
	 */
	struct iio_axi_adc_desc *iio_axi_adc_desc;

	/**
	 * iio instance descriptor.
	 */
	struct iio_axi_dac_desc *iio_axi_dac_desc;

	/**
	 * iio ad9361 instance descriptor.
	 */
	struct iio_ad9361_desc *iio_ad9361_desc;

	/**
	 * iio devices corresponding to every device.
	 */
	struct iio_device *adc_dev_desc, *dac_dev_desc, *ad9361_dev_desc;

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if(status < 0)
		return status;

	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9361_phy->rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if(status < 0)
		return status;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = ad9361_phy->tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};

	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (status < 0)
		return status;
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);

	struct iio_data_buffer write_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	iio_ad9361_init_param = (struct iio_ad9361_init_param) {
		.ad9361_phy = ad9361_phy,
	};

	status = iio_ad9361_init(&iio_ad9361_desc, &iio_ad9361_init_param);
	if (status < 0)
		return status;
	iio_ad9361_get_dev_descriptor(iio_ad9361_desc, &ad9361_dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("cf-ad9361-lpc", iio_axi_adc_desc, adc_dev_desc, &read_buff, NULL),
		IIO_APP_DEVICE("cf-ad9361-dds-core-lpc", iio_axi_dac_desc, dac_dev_desc, NULL, &write_buff),
		IIO_APP_DEVICE("ad9361-phy", ad9361_phy, ad9361_dev_desc, NULL, NULL)
	};

	iio_app_run(devices, NO_OS_ARRAY_SIZE(devices));

#endif // IIO_SUPPORT

	printf("Done.\n");

#ifdef TDD_SWITCH_STATE_EXAMPLE
	uint32_t ensm_mode;
	struct no_os_gpio_init_param  gpio_init = {
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	};
	struct no_os_gpio_desc 	*gpio_enable_pin;
	struct no_os_gpio_desc 	*gpio_txnrx_pin;
	if (!ad9361_phy->pdata->fdd) {
		if (ad9361_phy->pdata->ensm_pin_ctrl) {
			gpio_init.number = GPIO_ENABLE_PIN;
			status = no_os_gpio_get(&gpio_enable_pin, &gpio_init);
			if (status != 0) {
				printf("no_os_gpio_get() error: %"PRIi32"\n", status);
				return status;
			}
			no_os_gpio_direction_output(gpio_enable_pin, 1);
			gpio_init.number = GPIO_TXNRX_PIN;
			status = no_os_gpio_get(&gpio_txnrx_pin, &gpio_init);
			if (status != 0) {
				printf("no_os_gpio_get() error: %"PRIi32"\n", status);
				return status;
			}
			no_os_gpio_direction_output(gpio_txnrx_pin, 0);
			no_os_udelay(10);
			ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
			printf("TXNRX control - Alert: %s\n",
			       ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
			no_os_mdelay(1000);

			if (ad9361_phy->pdata->ensm_pin_pulse_mode) {
				while(1) {
					no_os_gpio_set_value(gpio_txnrx_pin, 0);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 1);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - RX: %s\n",
					       ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
					no_os_mdelay(1000);

					no_os_gpio_set_value(gpio_enable_pin, 1);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - Alert: %s\n",
					       ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					no_os_mdelay(1000);

					no_os_gpio_set_value(gpio_txnrx_pin, 1);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 1);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - TX: %s\n",
					       ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
					no_os_mdelay(1000);

					no_os_gpio_set_value(gpio_enable_pin, 1);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - Alert: %s\n",
					       ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					no_os_mdelay(1000);
				}
			} else {
				while(1) {
					no_os_gpio_set_value(gpio_txnrx_pin, 0);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 1);
					no_os_udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - RX: %s\n",
					       ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
					no_os_mdelay(1000);

					no_os_gpio_set_value(gpio_enable_pin, 0);
					no_os_udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - Alert: %s\n",
					       ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					no_os_mdelay(1000);

					no_os_gpio_set_value(gpio_txnrx_pin, 1);
					no_os_udelay(10);
					no_os_gpio_set_value(gpio_enable_pin, 1);
					no_os_udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - TX: %s\n",
					       ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
					no_os_mdelay(1000);

					no_os_gpio_set_value(gpio_enable_pin, 0);
					no_os_udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - Alert: %s\n",
					       ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					no_os_mdelay(1000);
				}
			}
		} else {
			while(1) {
				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_RX);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - RX: %s\n",
				       ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
				no_os_mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_ALERT);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - Alert: %s\n",
				       ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
				no_os_mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_TX);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - TX: %s\n",
				       ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
				no_os_mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_ALERT);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - Alert: %s\n",
				       ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
				no_os_mdelay(1000);
			}
		}
	}
#endif

	ad9361_remove(ad9361_phy);
#ifdef FMCOMMS5
	ad9361_remove(ad9361_phy_b);
#endif

#ifdef XILINX_PLATFORM
	Xil_DCacheDisable();
	Xil_ICacheDisable();
#endif

#ifdef ALTERA_PLATFORM
	if (altera_bridge_uninit()) {
		printf("Altera Bridge Uninit Error!\n");
		return -1;
	}
#endif

	return 0;
}

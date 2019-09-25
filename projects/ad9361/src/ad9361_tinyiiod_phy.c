/***************************************************************************//**
 *   @file   ad9361_tinyiiod_phy.c
 *   @brief  Implementation of ad9361_tinyiiod_phy
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include "ad9361_tinyiiod_phy.h"
#include "util.h"

static const char * const  phy_xml =
    "<device id=\"ad9361-phy\" name=\"ad9361-phy\" >"
    "<channel id=\"altvoltage1\" name=\"TX_LO\" type=\"output\" >"
    "<attribute name=\"external\" filename=\"out_altvoltage1_TX_LO_external\" />"
    "<attribute name=\"frequency\" filename=\"out_altvoltage1_TX_LO_frequency\" />"
    "<attribute name=\"fastlock_store\" filename=\"out_altvoltage1_TX_LO_fastlock_store\" />"
    "<attribute name=\"fastlock_recall\" filename=\"out_altvoltage1_TX_LO_fastlock_recall\" />"
    "<attribute name=\"powerdown\" filename=\"out_altvoltage1_TX_LO_powerdown\" />"
    "<attribute name=\"fastlock_save\" filename=\"out_altvoltage1_TX_LO_fastlock_save\" />"
    "<attribute name=\"frequency_available\" filename=\"out_altvoltage1_TX_LO_frequency_available\" />"
    "<attribute name=\"fastlock_load\" filename=\"out_altvoltage1_TX_LO_fastlock_load\" />"
    "</channel>"
    "<channel id=\"voltage1\" type=\"output\" >"
    "<attribute name=\"hardwaregain\" filename=\"out_voltage1_hardwaregain\" />"
    "<attribute name=\"rf_port_select\" filename=\"out_voltage1_rf_port_select\" />"
    "<attribute name=\"hardwaregain_available\" filename=\"out_voltage1_hardwaregain_available\" />"
    "<attribute name=\"rssi\" filename=\"out_voltage1_rssi\" />"
    "<attribute name=\"sampling_frequency_available\" filename=\"out_voltage_sampling_frequency_available\" />"
    "<attribute name=\"rf_port_select_available\" filename=\"out_voltage_rf_port_select_available\" />"
    "<attribute name=\"filter_fir_en\" filename=\"out_voltage_filter_fir_en\" />"
    "<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
    "<attribute name=\"rf_bandwidth_available\" filename=\"out_voltage_rf_bandwidth_available\" />"
    "<attribute name=\"rf_bandwidth\" filename=\"out_voltage_rf_bandwidth\" />"
    "</channel>"
    "<channel id=\"voltage0\" type=\"input\" >"
    "<attribute name=\"hardwaregain_available\" filename=\"in_voltage0_hardwaregain_available\" />"
    "<attribute name=\"hardwaregain\" filename=\"in_voltage0_hardwaregain\" />"
    "<attribute name=\"rssi\" filename=\"in_voltage0_rssi\" />"
    "<attribute name=\"rf_port_select\" filename=\"in_voltage0_rf_port_select\" />"
    "<attribute name=\"gain_control_mode\" filename=\"in_voltage0_gain_control_mode\" />"
    "<attribute name=\"rf_port_select_available\" filename=\"in_voltage_rf_port_select_available\" />"
    "<attribute name=\"rf_bandwidth\" filename=\"in_voltage_rf_bandwidth\" />"
    "<attribute name=\"rf_dc_offset_tracking_en\" filename=\"in_voltage_rf_dc_offset_tracking_en\" />"
    "<attribute name=\"sampling_frequency_available\" filename=\"in_voltage_sampling_frequency_available\" />"
    "<attribute name=\"quadrature_tracking_en\" filename=\"in_voltage_quadrature_tracking_en\" />"
    "<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
    "<attribute name=\"gain_control_mode_available\" filename=\"in_voltage_gain_control_mode_available\" />"
    "<attribute name=\"filter_fir_en\" filename=\"in_voltage_filter_fir_en\" />"
    "<attribute name=\"rf_bandwidth_available\" filename=\"in_voltage_rf_bandwidth_available\" />"
    "<attribute name=\"bb_dc_offset_tracking_en\" filename=\"in_voltage_bb_dc_offset_tracking_en\" />"
    "</channel>"
    "<channel id=\"voltage1\" type=\"input\" >"
    "<attribute name=\"rssi\" filename=\"in_voltage1_rssi\" />"
    "<attribute name=\"rf_port_select\" filename=\"in_voltage1_rf_port_select\" />"
    "<attribute name=\"hardwaregain\" filename=\"in_voltage1_hardwaregain\" />"
    "<attribute name=\"hardwaregain_available\" filename=\"in_voltage1_hardwaregain_available\" />"
    "<attribute name=\"gain_control_mode\" filename=\"in_voltage1_gain_control_mode\" />"
    "<attribute name=\"rf_port_select_available\" filename=\"in_voltage_rf_port_select_available\" />"
    "<attribute name=\"rf_bandwidth\" filename=\"in_voltage_rf_bandwidth\" />"
    "<attribute name=\"rf_dc_offset_tracking_en\" filename=\"in_voltage_rf_dc_offset_tracking_en\" />"
    "<attribute name=\"sampling_frequency_available\" filename=\"in_voltage_sampling_frequency_available\" />"
    "<attribute name=\"quadrature_tracking_en\" filename=\"in_voltage_quadrature_tracking_en\" />"
    "<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
    "<attribute name=\"gain_control_mode_available\" filename=\"in_voltage_gain_control_mode_available\" />"
    "<attribute name=\"filter_fir_en\" filename=\"in_voltage_filter_fir_en\" />"
    "<attribute name=\"rf_bandwidth_available\" filename=\"in_voltage_rf_bandwidth_available\" />"
    "<attribute name=\"bb_dc_offset_tracking_en\" filename=\"in_voltage_bb_dc_offset_tracking_en\" />"
    "</channel>"
    "<channel id=\"voltage3\" type=\"output\" >"
    "<attribute name=\"scale\" filename=\"out_voltage3_scale\" />"
    "<attribute name=\"raw\" filename=\"out_voltage3_raw\" />"
    "<attribute name=\"sampling_frequency_available\" filename=\"out_voltage_sampling_frequency_available\" />"
    "<attribute name=\"rf_port_select_available\" filename=\"out_voltage_rf_port_select_available\" />"
    "<attribute name=\"filter_fir_en\" filename=\"out_voltage_filter_fir_en\" />"
    "<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
    "<attribute name=\"rf_bandwidth_available\" filename=\"out_voltage_rf_bandwidth_available\" />"
    "<attribute name=\"rf_bandwidth\" filename=\"out_voltage_rf_bandwidth\" />"
    "</channel>"
    "<channel id=\"altvoltage0\" name=\"RX_LO\" type=\"output\" >"
    "<attribute name=\"frequency_available\" filename=\"out_altvoltage0_RX_LO_frequency_available\" />"
    "<attribute name=\"fastlock_save\" filename=\"out_altvoltage0_RX_LO_fastlock_save\" />"
    "<attribute name=\"powerdown\" filename=\"out_altvoltage0_RX_LO_powerdown\" />"
    "<attribute name=\"fastlock_load\" filename=\"out_altvoltage0_RX_LO_fastlock_load\" />"
    "<attribute name=\"fastlock_store\" filename=\"out_altvoltage0_RX_LO_fastlock_store\" />"
    "<attribute name=\"frequency\" filename=\"out_altvoltage0_RX_LO_frequency\" />"
    "<attribute name=\"external\" filename=\"out_altvoltage0_RX_LO_external\" />"
    "<attribute name=\"fastlock_recall\" filename=\"out_altvoltage0_RX_LO_fastlock_recall\" />"
    "</channel>"
    "<channel id=\"voltage2\" type=\"output\" >"
    "<attribute name=\"raw\" filename=\"out_voltage2_raw\" />"
    "<attribute name=\"scale\" filename=\"out_voltage2_scale\" />"
    "<attribute name=\"sampling_frequency_available\" filename=\"out_voltage_sampling_frequency_available\" />"
    "<attribute name=\"rf_port_select_available\" filename=\"out_voltage_rf_port_select_available\" />"
    "<attribute name=\"filter_fir_en\" filename=\"out_voltage_filter_fir_en\" />"
    "<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
    "<attribute name=\"rf_bandwidth_available\" filename=\"out_voltage_rf_bandwidth_available\" />"
    "<attribute name=\"rf_bandwidth\" filename=\"out_voltage_rf_bandwidth\" />"
    "</channel>"
    "<channel id=\"temp0\" type=\"input\" >"
    "<attribute name=\"input\" filename=\"in_temp0_input\" />"
    "</channel>"
    "<channel id=\"voltage0\" type=\"output\" >"
    "<attribute name=\"rf_port_select\" filename=\"out_voltage0_rf_port_select\" />"
    "<attribute name=\"hardwaregain\" filename=\"out_voltage0_hardwaregain\" />"
    "<attribute name=\"rssi\" filename=\"out_voltage0_rssi\" />"
    "<attribute name=\"hardwaregain_available\" filename=\"out_voltage0_hardwaregain_available\" />"
    "<attribute name=\"sampling_frequency_available\" filename=\"out_voltage_sampling_frequency_available\" />"
    "<attribute name=\"rf_port_select_available\" filename=\"out_voltage_rf_port_select_available\" />"
    "<attribute name=\"filter_fir_en\" filename=\"out_voltage_filter_fir_en\" />"
    "<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
    "<attribute name=\"rf_bandwidth_available\" filename=\"out_voltage_rf_bandwidth_available\" />"
    "<attribute name=\"rf_bandwidth\" filename=\"out_voltage_rf_bandwidth\" />"
    "</channel>"
    "<channel id=\"voltage2\" type=\"input\" >"
    "<attribute name=\"offset\" filename=\"in_voltage2_offset\" />"
    "<attribute name=\"scale\" filename=\"in_voltage2_scale\" />"
    "<attribute name=\"raw\" filename=\"in_voltage2_raw\" />"
    "<attribute name=\"rf_port_select_available\" filename=\"in_voltage_rf_port_select_available\" />"
    "<attribute name=\"rf_bandwidth\" filename=\"in_voltage_rf_bandwidth\" />"
    "<attribute name=\"rf_dc_offset_tracking_en\" filename=\"in_voltage_rf_dc_offset_tracking_en\" />"
    "<attribute name=\"sampling_frequency_available\" filename=\"in_voltage_sampling_frequency_available\" />"
    "<attribute name=\"quadrature_tracking_en\" filename=\"in_voltage_quadrature_tracking_en\" />"
    "<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
    "<attribute name=\"gain_control_mode_available\" filename=\"in_voltage_gain_control_mode_available\" />"
    "<attribute name=\"filter_fir_en\" filename=\"in_voltage_filter_fir_en\" />"
    "<attribute name=\"rf_bandwidth_available\" filename=\"in_voltage_rf_bandwidth_available\" />"
    "<attribute name=\"bb_dc_offset_tracking_en\" filename=\"in_voltage_bb_dc_offset_tracking_en\" />"
    "</channel>"
    "<channel id=\"out\" type=\"input\" >"
    "<attribute name=\"voltage_filter_fir_en\" filename=\"in_out_voltage_filter_fir_en\" />"
    "</channel>"
    "<attribute name=\"dcxo_tune_coarse\" />"
    "<attribute name=\"rx_path_rates\" />"
    "<attribute name=\"trx_rate_governor\" />"
    "<attribute name=\"calib_mode_available\" />"
    "<attribute name=\"xo_correction_available\" />"
    "<attribute name=\"gain_table_config\" />"
    "<attribute name=\"dcxo_tune_fine\" />"
    "<attribute name=\"dcxo_tune_fine_available\" />"
    "<attribute name=\"ensm_mode_available\" />"
    "<attribute name=\"multichip_sync\" />"
    "<attribute name=\"rssi_gain_step_error\" />"
    "<attribute name=\"dcxo_tune_coarse_available\" />"
    "<attribute name=\"tx_path_rates\" />"
    "<attribute name=\"trx_rate_governor_available\" />"
    "<attribute name=\"xo_correction\" />"
    "<attribute name=\"ensm_mode\" />"
    "<attribute name=\"filter_fir_config\" />"
    "<attribute name=\"calib_mode\" />"
    "<debug-attribute name=\"digital_tune\" />"
    "<debug-attribute name=\"calibration_switch_control\" />"
    "<debug-attribute name=\"multichip_sync\" />"
    "<debug-attribute name=\"gaininfo_rx2\" />"
    "<debug-attribute name=\"gaininfo_rx1\" />"
    "<debug-attribute name=\"bist_timing_analysis\" />"
    "<debug-attribute name=\"bist_tone\" />"
    "<debug-attribute name=\"bist_prbs\" />"
    "<debug-attribute name=\"loopback\" />"
    "<debug-attribute name=\"initialize\" />"
    "<debug-attribute name=\"adi,txmon-2-lo-cm\" />"
    "<debug-attribute name=\"adi,txmon-1-lo-cm\" />"
    "<debug-attribute name=\"adi,txmon-2-front-end-gain\" />"
    "<debug-attribute name=\"adi,txmon-1-front-end-gain\" />"
    "<debug-attribute name=\"adi,txmon-duration\" />"
    "<debug-attribute name=\"adi,txmon-delay\" />"
    "<debug-attribute name=\"adi,txmon-one-shot-mode-enable\" />"
    "<debug-attribute name=\"adi,txmon-dc-tracking-enable\" />"
    "<debug-attribute name=\"adi,txmon-high-gain\" />"
    "<debug-attribute name=\"adi,txmon-low-gain\" />"
    "<debug-attribute name=\"adi,txmon-low-high-thresh\" />"
    "<debug-attribute name=\"adi,gpo3-tx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo3-rx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo2-tx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo2-rx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo1-tx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo1-rx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo0-tx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo0-rx-delay-us\" />"
    "<debug-attribute name=\"adi,gpo3-slave-tx-enable\" />"
    "<debug-attribute name=\"adi,gpo3-slave-rx-enable\" />"
    "<debug-attribute name=\"adi,gpo2-slave-tx-enable\" />"
    "<debug-attribute name=\"adi,gpo2-slave-rx-enable\" />"
    "<debug-attribute name=\"adi,gpo1-slave-tx-enable\" />"
    "<debug-attribute name=\"adi,gpo1-slave-rx-enable\" />"
    "<debug-attribute name=\"adi,gpo0-slave-tx-enable\" />"
    "<debug-attribute name=\"adi,gpo0-slave-rx-enable\" />"
    "<debug-attribute name=\"adi,gpo3-inactive-state-high-enable\" />"
    "<debug-attribute name=\"adi,gpo2-inactive-state-high-enable\" />"
    "<debug-attribute name=\"adi,gpo1-inactive-state-high-enable\" />"
    "<debug-attribute name=\"adi,gpo0-inactive-state-high-enable\" />"
    "<debug-attribute name=\"adi,gpo-manual-mode-enable-mask\" />"
    "<debug-attribute name=\"adi,gpo-manual-mode-enable\" />"
    "<debug-attribute name=\"adi,aux-dac2-tx-delay-us\" />"
    "<debug-attribute name=\"adi,aux-dac2-rx-delay-us\" />"
    "<debug-attribute name=\"adi,aux-dac2-active-in-alert-enable\" />"
    "<debug-attribute name=\"adi,aux-dac2-active-in-tx-enable\" />"
    "<debug-attribute name=\"adi,aux-dac2-active-in-rx-enable\" />"
    "<debug-attribute name=\"adi,aux-dac2-default-value-mV\" />"
    "<debug-attribute name=\"adi,aux-dac1-tx-delay-us\" />"
    "<debug-attribute name=\"adi,aux-dac1-rx-delay-us\" />"
    "<debug-attribute name=\"adi,aux-dac1-active-in-alert-enable\" />"
    "<debug-attribute name=\"adi,aux-dac1-active-in-tx-enable\" />"
    "<debug-attribute name=\"adi,aux-dac1-active-in-rx-enable\" />"
    "<debug-attribute name=\"adi,aux-dac1-default-value-mV\" />"
    "<debug-attribute name=\"adi,aux-dac-manual-mode-enable\" />"
    "<debug-attribute name=\"adi,aux-adc-decimation\" />"
    "<debug-attribute name=\"adi,aux-adc-rate\" />"
    "<debug-attribute name=\"adi,temp-sense-decimation\" />"
    "<debug-attribute name=\"adi,temp-sense-periodic-measurement-enable\" />"
    "<debug-attribute name=\"adi,temp-sense-offset-signed\" />"
    "<debug-attribute name=\"adi,temp-sense-measurement-interval-ms\" />"
    "<debug-attribute name=\"adi,elna-gaintable-all-index-enable\" />"
    "<debug-attribute name=\"adi,elna-rx2-gpo1-control-enable\" />"
    "<debug-attribute name=\"adi,elna-rx1-gpo0-control-enable\" />"
    "<debug-attribute name=\"adi,elna-bypass-loss-mdB\" />"
    "<debug-attribute name=\"adi,elna-gain-mdB\" />"
    "<debug-attribute name=\"adi,elna-settling-delay-ns\" />"
    "<debug-attribute name=\"adi,ctrl-outs-enable-mask\" />"
    "<debug-attribute name=\"adi,ctrl-outs-index\" />"
    "<debug-attribute name=\"adi,rssi-duration\" />"
    "<debug-attribute name=\"adi,rssi-wait\" />"
    "<debug-attribute name=\"adi,rssi-delay\" />"
    "<debug-attribute name=\"adi,rssi-unit-is-rx-samples-enable\" />"
    "<debug-attribute name=\"adi,rssi-restart-mode\" />"
    "<debug-attribute name=\"adi,fagc-adc-large-overload-inc-steps\" />"
    "<debug-attribute name=\"adi,fagc-power-measurement-duration-in-state5\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-if-en-agc-pulled-high-mode\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-en-agc-pulled-high-enable\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-large-lmt-overload-enable\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-large-adc-overload-enable\" />"
    "<debug-attribute name=\"adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-stronger-sig-thresh-above-ll\" />"
    "<debug-attribute name=\"adi,fagc-optimized-gain-offset\" />"
    "<debug-attribute name=\"adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable\" />"
    "<debug-attribute name=\"adi,fagc-use-last-lock-level-for-set-gain-enable\" />"
    "<debug-attribute name=\"adi,fagc-gain-index-type-after-exit-rx-mode\" />"
    "<debug-attribute name=\"adi,fagc-gain-increase-after-gain-lock-enable\" />"
    "<debug-attribute name=\"adi,fagc-final-overrange-count\" />"
    "<debug-attribute name=\"adi,fagc-lmt-final-settling-steps\" />"
    "<debug-attribute name=\"adi,fagc-lpf-final-settling-steps\" />"
    "<debug-attribute name=\"adi,fagc-lock-level-gain-increase-upper-limit\" />"
    "<debug-attribute name=\"adi,fagc-lock-level-lmt-gain-increase-enable\" />"
    "<debug-attribute name=\"adi,fagc-lp-thresh-increment-steps\" />"
    "<debug-attribute name=\"adi,fagc-lp-thresh-increment-time\" />"
    "<debug-attribute name=\"adi,fagc-allow-agc-gain-increase-enable\" />"
    "<debug-attribute name=\"adi,fagc-state-wait-time-ns\" />"
    "<debug-attribute name=\"adi,fagc-dec-pow-measurement-duration\" />"
    "<debug-attribute name=\"adi,agc-immed-gain-change-if-large-lmt-overload-enable\" />"
    "<debug-attribute name=\"adi,agc-immed-gain-change-if-large-adc-overload-enable\" />"
    "<debug-attribute name=\"adi,agc-gain-update-interval-us\" />"
    "<debug-attribute name=\"adi,agc-sync-for-gain-counter-enable\" />"
    "<debug-attribute name=\"adi,agc-dig-gain-step-size\" />"
    "<debug-attribute name=\"adi,agc-dig-saturation-exceed-counter\" />"
    "<debug-attribute name=\"adi,agc-lmt-overload-large-inc-steps\" />"
    "<debug-attribute name=\"adi,agc-lmt-overload-small-exceed-counter\" />"
    "<debug-attribute name=\"adi,agc-lmt-overload-large-exceed-counter\" />"
    "<debug-attribute name=\"adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable\" />"
    "<debug-attribute name=\"adi,agc-adc-large-overload-inc-steps\" />"
    "<debug-attribute name=\"adi,agc-adc-large-overload-exceed-counter\" />"
    "<debug-attribute name=\"adi,agc-adc-small-overload-exceed-counter\" />"
    "<debug-attribute name=\"adi,agc-outer-thresh-low-inc-steps\" />"
    "<debug-attribute name=\"adi,agc-outer-thresh-low\" />"
    "<debug-attribute name=\"adi,agc-inner-thresh-low-inc-steps\" />"
    "<debug-attribute name=\"adi,agc-inner-thresh-low\" />"
    "<debug-attribute name=\"adi,agc-inner-thresh-high-dec-steps\" />"
    "<debug-attribute name=\"adi,agc-inner-thresh-high\" />"
    "<debug-attribute name=\"adi,agc-outer-thresh-high-dec-steps\" />"
    "<debug-attribute name=\"adi,agc-outer-thresh-high\" />"
    "<debug-attribute name=\"adi,agc-attack-delay-extra-margin-us\" />"
    "<debug-attribute name=\"adi,mgc-split-table-ctrl-inp-gain-mode\" />"
    "<debug-attribute name=\"adi,mgc-dec-gain-step\" />"
    "<debug-attribute name=\"adi,mgc-inc-gain-step\" />"
    "<debug-attribute name=\"adi,mgc-rx2-ctrl-inp-enable\" />"
    "<debug-attribute name=\"adi,mgc-rx1-ctrl-inp-enable\" />"
    "<debug-attribute name=\"adi,gc-use-rx-fir-out-for-dec-pwr-meas-enable\" />"
    "<debug-attribute name=\"adi,gc-max-dig-gain\" />"
    "<debug-attribute name=\"adi,gc-dig-gain-enable\" />"
    "<debug-attribute name=\"adi,gc-low-power-thresh\" />"
    "<debug-attribute name=\"adi,gc-dec-pow-measurement-duration\" />"
    "<debug-attribute name=\"adi,gc-lmt-overload-low-thresh\" />"
    "<debug-attribute name=\"adi,gc-lmt-overload-high-thresh\" />"
    "<debug-attribute name=\"adi,gc-adc-large-overload-thresh\" />"
    "<debug-attribute name=\"adi,gc-adc-small-overload-thresh\" />"
    "<debug-attribute name=\"adi,gc-adc-ovr-sample-size\" />"
    "<debug-attribute name=\"adi,gc-rx2-mode\" />"
    "<debug-attribute name=\"adi,gc-rx1-mode\" />"
    "<debug-attribute name=\"adi,update-tx-gain-in-alert-enable\" />"
    "<debug-attribute name=\"adi,tx-attenuation-mdB\" />"
    "<debug-attribute name=\"adi,rf-tx-bandwidth-hz\" />"
    "<debug-attribute name=\"adi,rf-rx-bandwidth-hz\" />"
    "<debug-attribute name=\"adi,qec-tracking-slow-mode-enable\" />"
    "<debug-attribute name=\"adi,dc-offset-count-low-range\" />"
    "<debug-attribute name=\"adi,dc-offset-count-high-range\" />"
    "<debug-attribute name=\"adi,dc-offset-attenuation-low-range\" />"
    "<debug-attribute name=\"adi,dc-offset-attenuation-high-range\" />"
    "<debug-attribute name=\"adi,dc-offset-tracking-update-event-mask\" />"
    "<debug-attribute name=\"adi,clk-output-mode-select\" />"
    "<debug-attribute name=\"adi,external-rx-lo-enable\" />"
    "<debug-attribute name=\"adi,external-tx-lo-enable\" />"
    "<debug-attribute name=\"adi,xo-disable-use-ext-refclk-enable\" />"
    "<debug-attribute name=\"adi,trx-synthesizer-target-fref-overwrite-hz\" />"
    "<debug-attribute name=\"adi,rx1-rx2-phase-inversion-enable\" />"
    "<debug-attribute name=\"adi,tx-rf-port-input-select-lock-enable\" />"
    "<debug-attribute name=\"adi,rx-rf-port-input-select-lock-enable\" />"
    "<debug-attribute name=\"adi,tx-rf-port-input-select\" />"
    "<debug-attribute name=\"adi,rx-rf-port-input-select\" />"
    "<debug-attribute name=\"adi,split-gain-table-mode-enable\" />"
    "<debug-attribute name=\"adi,1rx-1tx-mode-use-tx-num\" />"
    "<debug-attribute name=\"adi,1rx-1tx-mode-use-rx-num\" />"
    "<debug-attribute name=\"adi,2rx-2tx-mode-enable\" />"
    "<debug-attribute name=\"adi,digital-interface-tune-fir-disable\" />"
    "<debug-attribute name=\"adi,digital-interface-tune-skip-mode\" />"
    "<debug-attribute name=\"adi,tx-fastlock-pincontrol-enable\" />"
    "<debug-attribute name=\"adi,rx-fastlock-pincontrol-enable\" />"
    "<debug-attribute name=\"adi,rx-fastlock-delay-ns\" />"
    "<debug-attribute name=\"adi,tx-fastlock-delay-ns\" />"
    "<debug-attribute name=\"adi,tdd-skip-vco-cal-enable\" />"
    "<debug-attribute name=\"adi,tdd-use-dual-synth-mode-enable\" />"
    "<debug-attribute name=\"adi,debug-mode-enable\" />"
    "<debug-attribute name=\"adi,ensm-enable-txnrx-control-enable\" />"
    "<debug-attribute name=\"adi,ensm-enable-pin-pulse-mode-enable\" />"
    "<debug-attribute name=\"adi,frequency-division-duplex-independent-mode-enable\" />"
    "<debug-attribute name=\"adi,frequency-division-duplex-mode-enable\" />"
    "<debug-attribute name=\"direct_reg_access\" />"
    "</device>";
//static struct ad9361_rf_phy *ad9361_phy;
extern const char *ad9361_ensm_states[12];

static const char * const ad9361_calib_mode[] =
{"auto", "manual", "tx_quad", "rf_dc_offs", "rssi_gain_step"};

static const char * const ad9361_rf_rx_port[] = {
    "A_BALANCED", "B_BALANCED", "C_BALANCED",
    "A_N", "A_P", "B_N", "B_P", "C_N", "C_P", "TX_MONITOR1",
    "TX_MONITOR2", "TX_MONITOR1_2"
};

static const char * const ad9361_rf_tx_port[] =
{"A", "B"};

static const char * const ad9361_agc_modes[] =
{"manual", "fast_attack", "slow_attack", "hybrid"};

/**
 * get_rf_port_select
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rf_port_select(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    ssize_t ret = 0;
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if(channel->ch_out) {
        uint32_t mode;
        ret = ad9361_get_tx_rf_port_output(ad9361_phy, &mode);
        return ret < 0 ? ret : sprintf(buf, "%s", ad9361_rf_tx_port[mode]);
    } else {
        uint32_t mode;
        ret = ad9361_get_rx_rf_port_input(ad9361_phy, &mode);
        return ret < 0 ? ret : sprintf(buf, "%s", ad9361_rf_rx_port[mode]);
    }
}

/**
 * get_hardwaregain
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_hardwaregain(void *device, char *buf, size_t len,
                                const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    struct rf_rx_gain rx_gain = {0};
    int32_t i = 0, ret, val1, val2;

    if(channel->ch_out) {
        ret = ad9361_get_tx_atten(ad9361_phy, channel->ch_num + 1);
        if (ret < 0) {
            return -EINVAL;
        }
        val1 = -1 * (ret / 1000);
        val2 = (ret % 1000) * 1000;
        if (!val1)
            val2 *= -1;
        if(val2 < 0 && val1 >= 0) {
            ret = (ssize_t) snprintf(buf, len, "-");
            i++;
        }
        ret = i + (ssize_t) snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32" dB", val1,
                                     labs(val2));

        return ret;
    } else {
        ret = ad9361_get_rx_gain(ad9361_phy,
                                 ad9361_1rx1tx_channel_map(ad9361_phy,
                                         false, channel->ch_num + 1), &rx_gain);
        if (ret < 0)
            return ret;

        return (ssize_t) snprintf(buf, len, "%d.000000 dB", (int)rx_gain.gain_db);
    }
}

/**
 * get_rssi
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rssi(void *device, char *buf, size_t len, const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    if(channel->ch_out) {
        uint32_t rssi_db_x_1000;
        ret = ad9361_get_tx_rssi(ad9361_phy, channel->ch_num, &rssi_db_x_1000);
        if (ret < 0) {
            return -EINVAL;
        }
        return ret < 0 ? ret : sprintf(buf, "%"PRIu32".%02"PRIu32" dB",
                                       rssi_db_x_1000 / 1000, rssi_db_x_1000 % 1000);
    } else {
        struct rf_rssi rssi = {0};
        ret = ad9361_get_rx_rssi (ad9361_phy, channel->ch_num, &rssi);
        return ret < 0 ? ret : sprintf(buf, "%"PRIu32".%02"PRIu32" dB",
                                       rssi.symbol / rssi.multiplier, rssi.symbol % rssi.multiplier);
    }
}

/**
 * get_hardwaregain_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_hardwaregain_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if (channel->ch_out) {
        return (ssize_t) snprintf(buf, len, "[%d, %d, %d]", 0, 250, 89750);
    } else {
        return (ssize_t) snprintf(buf, len, "[%"PRIi32", %d, %"PRIi32"]",
                                  ad9361_phy->rx_gain[ad9361_phy->current_table].starting_gain_db,
                                  1,
                                  ad9361_phy->rx_gain[ad9361_phy->current_table].max_gain_db);
    }
}

/**
 * get_sampling_frequency_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_sampling_frequency_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int32_t int_dec;
    uint32_t max;

    if (ad9361_phy->pdata->port_ctrl.pp_conf[2] & LVDS_MODE) {
        max = 61440000U;
    } else {
        max = 61440000U / (ad9361_phy->pdata->rx2tx2 ? 2 : 1);
    }

    if (channel->ch_out) {
        if (ad9361_phy->bypass_tx_fir) {
            int_dec = 1;
        } else {
            int_dec = ad9361_phy->tx_fir_int;
        }

    } else {
        if (ad9361_phy->bypass_rx_fir) {
            int_dec = 1;
        } else {
            int_dec = ad9361_phy->rx_fir_dec;
        }
    }

    return (ssize_t) snprintf(buf, len, "[%"PRIu32" %d %"PRIu32"]",
                              MIN_ADC_CLK / (12 * int_dec), 1, max);
}

/**
 * get_rf_port_select_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rf_port_select_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    if(channel->ch_out) {
        return (ssize_t) sprintf(buf, "%s %s",
                                 ad9361_rf_tx_port[0],
                                 ad9361_rf_tx_port[1]);
    } else {
        ssize_t bytes_no = 0;
        for(int16_t i = 0; i < sizeof(ad9361_rf_rx_port) / sizeof(ad9361_rf_rx_port[0]);
                i++) {
            if(i > 0 ) {
                bytes_no += sprintf(buf + bytes_no, " ");
            }
            bytes_no += sprintf(buf + bytes_no, "%s", ad9361_rf_rx_port[i]);
            if(bytes_no < 0) {
                break;
            }
        }
        return bytes_no;
    }
}

/**
 * get_filter_fir_en
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_filter_fir_en(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint8_t en_dis;
    if(channel->ch_out)
        ad9361_get_tx_fir_en_dis (ad9361_phy, &en_dis);
    else
        ad9361_get_rx_fir_en_dis (ad9361_phy, &en_dis);

    return (ssize_t) snprintf(buf, len, "%d", en_dis);
}

/**
 * get_sampling_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_sampling_frequency(void *device, char *buf, size_t len,
                                      const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t sampling_freq_hz;
    ad9361_get_rx_sampling_freq (ad9361_phy, &sampling_freq_hz);
    return (ssize_t) snprintf(buf, len, "%d", (int)sampling_freq_hz);
}

/**
 * get_rf_bandwidth_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rf_bandwidth_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    if(channel->ch_out) {
        return sprintf(buf, "[200000 1 40000000]");
    } else {
        return sprintf(buf, "[200000 1 56000000]");
    }
}

/**
 * get_rf_bandwidth
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rf_bandwidth(void *device, char *buf, size_t len,
                                const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if(channel->ch_out) {
        return sprintf(buf, "%"PRIu32"", ad9361_phy->current_tx_bw_Hz);
    } else {
        return sprintf(buf, "%"PRIu32"", ad9361_phy->current_rx_bw_Hz);
    }
}

static attribute_map voltage_output_write_map[] = {
    {.name = "rf_port_select", .exec = get_rf_port_select},
    {.name = "hardwaregain", .exec = get_hardwaregain},
    {.name = "rssi", .exec = get_rssi},
    {.name = "hardwaregain_available", .exec = get_hardwaregain_available},
    {.name = "sampling_frequency_available", .exec = get_sampling_frequency_available},
    {.name = "rf_port_select_available", .exec = get_rf_port_select_available},
    {.name = "filter_fir_en", .exec = get_filter_fir_en},
    {.name = "sampling_frequency", .exec = get_sampling_frequency},
    {.name = "rf_bandwidth_available", .exec = get_rf_bandwidth_available},
    {.name = "rf_bandwidth", .exec = get_rf_bandwidth},
    {.name = NULL},
};

/**
 * get_gain_control_mode
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_gain_control_mode(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    return (ssize_t) sprintf(buf, "%s",
                             ad9361_agc_modes[ad9361_phy->agc_mode[channel->ch_num]]);
}

/**
 * get_rf_dc_offset_tracking_en
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rf_dc_offset_tracking_en(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if(!channel->ch_out) {
        buf[1] = 0;
        return (ssize_t) sprintf(buf, "%d", ad9361_phy->rfdc_track_en) + 1;
    }

    return -ENOENT;
}

/**
 * get_quadrature_tracking_en
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_quadrature_tracking_en(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if(!channel->ch_out) {
        buf[1] = 0;
        return (ssize_t) sprintf(buf, "%d", ad9361_phy->quad_track_en) + 1;
    }

    return -ENOENT;
}

/**
 * get_gain_control_mode_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_gain_control_mode_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return (ssize_t) sprintf(buf, "%s %s %s %s",
                             ad9361_agc_modes[0],
                             ad9361_agc_modes[1],
                             ad9361_agc_modes[2],
                             ad9361_agc_modes[3]);
}

/**
 * get_bb_dc_offset_tracking_en
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_bb_dc_offset_tracking_en(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if(!channel->ch_out) {
        buf[1] = 0;
        return (ssize_t) sprintf(buf, "%d", ad9361_phy->bbdc_track_en) + 1;
    }

    return -ENOENT;
}

static attribute_map voltage_input_read_map[] = {
    {.name = "hardwaregain_available", .exec = get_hardwaregain_available},
    {.name = "hardwaregain", .exec = get_hardwaregain},
    {.name = "rssi", .exec = get_rssi},
    {.name = "rf_port_select", .exec = get_rf_port_select},
    {.name = "gain_control_mode", .exec = get_gain_control_mode},
    {.name = "rf_port_select_available", .exec = get_rf_port_select_available},
    {.name = "rf_bandwidth", .exec = get_rf_bandwidth},
    {.name = "rf_dc_offset_tracking_en", .exec = get_rf_dc_offset_tracking_en},
    {.name = "sampling_frequency_available", .exec = get_sampling_frequency_available},
    {.name = "quadrature_tracking_en", .exec = get_quadrature_tracking_en},
    {.name = "sampling_frequency", .exec = get_sampling_frequency},
    {.name = "gain_control_mode_available", .exec = get_gain_control_mode_available},
    {.name = "filter_fir_en", .exec = get_filter_fir_en},
    {.name = "rf_bandwidth_available", .exec = get_rf_bandwidth_available},
    {.name = "bb_dc_offset_tracking_en", .exec = get_bb_dc_offset_tracking_en},
    {.name = NULL},
};

/**
 * get_frequency_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_frequency_available(void *device, char *buf, size_t len,
                                       const struct channel_info *channel)
{
    return sprintf(buf, "[%llu 1 %llu]", AD9363A_MIN_CARRIER_FREQ_HZ,
                   AD9363A_MAX_CARRIER_FREQ_HZ);
}

/**
 * get_fastlock_save
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_fastlock_save(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint8_t faslock_vals[16];
    size_t length;
    int32_t ret = 0;
    int32_t i;
    ret = ad9361_fastlock_save(ad9361_phy, channel->ch_num == 1,
                               ad9361_phy->fastlock.save_profile, faslock_vals);
    length = sprintf(buf, "%u ", ad9361_phy->fastlock.save_profile);

    for (i = 0; i < RX_FAST_LOCK_CONFIG_WORD_NUM; i++)
        length += sprintf(buf + length, "%u%c", faslock_vals[i],
                          i == 15 ? '\n' : ',');
    if(ret < 0)
        return ret;

    return length;
}

/**
 * get_powerdown
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_powerdown(void *device, char *buf, size_t len,
                             const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint64_t val = 0;
    val = !!(ad9361_phy->cached_synth_pd[channel->ch_num ? 0 : 1] &
             RX_LO_POWER_DOWN);
    return sprintf(buf, "%llu", val);
}

/**
 * get_fastlock_load
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_fastlock_load(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * get_fastlock_store
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_fastlock_store(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * get_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_frequency(void *device, char *buf, size_t len,
                             const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint64_t val = 0;
    val = ad9361_from_clk(clk_get_rate(ad9361_phy,
                                       ad9361_phy->ref_clk_scale[channel->ch_num ?
                                                               TX_RFPLL : RX_RFPLL]));
    return sprintf(buf, "%llu", (val));
}

/**
 * get_external
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_external(void *device, char *buf, size_t len, const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if(channel->ch_num == 0)
        return (ssize_t) sprintf(buf, "%d", ad9361_phy->pdata->use_ext_rx_lo);
    else
        return (ssize_t) sprintf(buf, "%d", ad9361_phy->pdata->use_ext_tx_lo);
}

/**
 * get_fastlock_recall
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_fastlock_recall(void *device, char *buf, size_t len,
                                   const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    return sprintf(buf, "%d",
                   ad9361_phy->fastlock.current_profile[channel->ch_num]);
}

static attribute_map altvoltage_read_attrtibute_map[] = {
    {.name = "frequency_available", .exec = get_frequency_available},
    {.name = "fastlock_save", .exec = get_fastlock_save},
    {.name = "powerdown", .exec = get_powerdown},
    {.name = "fastlock_load", .exec = get_fastlock_load},
    {.name = "fastlock_store", .exec = get_fastlock_store},
    {.name = "frequency", .exec = get_frequency},
    {.name = "external", .exec = get_external},
    {.name = "fastlock_recall", .exec = get_fastlock_recall},
    {NULL},
};

/**
 * get_temp0_input
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_temp0_input(void *device, char *buf, size_t len,
                               const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int32_t temp;
    ad9361_get_temperature(ad9361_phy, &temp);
    return (ssize_t) snprintf(buf, len, "%d", (int)temp);
}

static attribute_map tmp0_map[] = {
    {.name = "input", .exec = get_temp0_input},
    {.name = NULL},
};

/**
 * get_voltage_filter_fir_en
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_voltage_filter_fir_en(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint8_t en_dis_tx, en_dis_rx;

    ad9361_get_tx_fir_en_dis (ad9361_phy, &en_dis_tx);
    ad9361_get_rx_fir_en_dis (ad9361_phy, &en_dis_rx);

    return (ssize_t) snprintf(buf, len, "%d", en_dis_rx && en_dis_tx);
}

static attribute_map out_read_map[] = {
    {.name = "voltage_filter_fir_en", .exec = get_voltage_filter_fir_en},
    {.name = NULL},
};

static attribute_map ch_read_phy_attr_map[] = {
    {.name = "voltage0", .exec = NULL, .map_in = voltage_input_read_map, .map_out = voltage_output_write_map},
    {.name = "voltage1", .exec = NULL, .map_in = voltage_input_read_map, .map_out = voltage_output_write_map},
    {.name = "voltage2", .exec = NULL, .map_in = voltage_input_read_map, .map_out = voltage_output_write_map},
    {.name = "voltage3", .exec = NULL, .map_in = voltage_input_read_map, .map_out = voltage_output_write_map},
    {.name = "altvoltage0", .exec = NULL, .map_in = altvoltage_read_attrtibute_map, .map_out = altvoltage_read_attrtibute_map},
    {.name = "altvoltage1", .exec = NULL, .map_in = altvoltage_read_attrtibute_map, .map_out = altvoltage_read_attrtibute_map},
    {.name = "temp0", .exec = NULL, .map_in = tmp0_map, .map_out = tmp0_map},
    {.name = "out", .exec = NULL, .map_in = out_read_map, .map_out = out_read_map},
    {.name = NULL},
};

/**
 * set_hardwaregain_available
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_hardwaregain_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * set_hardwaregain
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_hardwaregain(void *device, char *buf, size_t len,
                                const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    float gain = strtof(buf, NULL);
    int32_t val1 = (int32_t)gain;
    int32_t val2 = (int32_t)(gain * 1000) % 1000;
    if (channel->ch_out) {
        int32_t ch;
        if (val1 > 0 || (val1 == 0 && val2 > 0)) {
            return -EINVAL;
        }
        uint32_t code = ((abs(val1) * 1000) + (abs(val2)/* / 1000*/));
        ch = ad9361_1rx1tx_channel_map(ad9361_phy, true, channel->ch_num);
        ret = ad9361_set_tx_atten(ad9361_phy, code, ch == 0, ch == 1,
                                  !ad9361_phy->pdata->update_tx_gain_via_alert);
        if (ret < 0) {
            return -EINVAL;
        }
    } else {
        struct rf_rx_gain rx_gain = {0};
        rx_gain.gain_db = val1;
        ret = ad9361_set_rx_gain(ad9361_phy,
                                 ad9361_1rx1tx_channel_map(ad9361_phy, false, channel->ch_num + 1), &rx_gain);
        if (ret < 0) {
            return -EINVAL;
        }
    }

    return len;
}

/**
 * set_rssi
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_rssi(void *device, char *buf, size_t len, const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * set_rf_port_select
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_rf_port_select(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    uint32_t i = 0;
    if(channel->ch_out) {
        for(i = 0; i < sizeof(ad9361_rf_tx_port) / sizeof(ad9361_rf_tx_port[0]); i++) {
            if(strequal(ad9361_rf_tx_port[i], buf)) {
                break;
            }
        }
        if(i >= sizeof(ad9361_rf_tx_port) / sizeof(ad9361_rf_tx_port[0])) {
            return -EINVAL;
        }
        ret = ad9361_set_tx_rf_port_output(ad9361_phy, i);
        return ret < 0 ? ret : len;
    } else {
        for(i = 0; i < sizeof(ad9361_rf_rx_port) / sizeof(ad9361_rf_rx_port[0]); i++) {
            if(strequal(ad9361_rf_rx_port[i], buf)) {
                break;
            }
        }
        if(i >= sizeof(ad9361_rf_tx_port) / sizeof(ad9361_rf_tx_port[0])) {
            return -EINVAL;
        }
        ret = ad9361_set_rx_rf_port_input(ad9361_phy, i);
        return ret < 0 ? ret : len;
    }
}

/**
 * set_gain_control_mode
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_gain_control_mode(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    struct rf_gain_ctrl gc = {0};
    int32_t i;
    for(i = 0; i < sizeof(ad9361_agc_modes) / sizeof(ad9361_agc_modes[0]); i++) {
        if(strequal(ad9361_agc_modes[i], buf)) {
            break;
        }
    }
    if(i >= sizeof(ad9361_agc_modes) / sizeof(ad9361_agc_modes[0])) {
        return -EINVAL;
    }
    uint32_t mode = i;
    if (ad9361_phy->agc_mode[channel->ch_num] == mode)
        return len;
    gc.ant = ad9361_1rx1tx_channel_map(ad9361_phy, false, channel->ch_num + 1);
    gc.mode = ad9361_phy->agc_mode[channel->ch_num] = mode;
    ad9361_set_gain_ctrl_mode(ad9361_phy, &gc);

    return len;
}

/**
 * set_rf_port_select_available
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_rf_port_select_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * set_rf_bandwidth
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_rf_bandwidth(void *device, char *buf, size_t len,
                                const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    uint32_t rf_bandwidth = read_ul_value(buf);
    rf_bandwidth = ad9361_validate_rf_bw(ad9361_phy, rf_bandwidth);
    if(channel->ch_out) {
        if(ad9361_phy->current_tx_bw_Hz != rf_bandwidth) {
            ret = ad9361_update_rf_bandwidth(ad9361_phy, ad9361_phy->current_rx_bw_Hz,
                                             rf_bandwidth);
        }
    } else {
        if(ad9361_phy->current_rx_bw_Hz != rf_bandwidth) {
            ret = ad9361_update_rf_bandwidth(ad9361_phy, rf_bandwidth,
                                             ad9361_phy->current_tx_bw_Hz);
        }
    }
    if(ret < 0) {
        return ret;
    }

    return len;
}

/**
 * set_rf_dc_offset_tracking_en
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_rf_dc_offset_tracking_en(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int8_t en_dis = read_value(buf);
    if(en_dis < 0) {
        return en_dis;
    }
    ad9361_phy->rfdc_track_en = en_dis ? 1 : 0;
    if(!channel->ch_out) {
        return ad9361_tracking_control(ad9361_phy, ad9361_phy->bbdc_track_en,
                                       ad9361_phy->rfdc_track_en, ad9361_phy->quad_track_en);
    }

    return -ENOENT;
}

/**
 * set_sampling_frequency_available
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_sampling_frequency_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * set_quadrature_tracking_en
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_quadrature_tracking_en(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int8_t en_dis = read_value(buf);
    if(en_dis < 0) {
        return en_dis;
    }
    ad9361_phy->quad_track_en = en_dis ? 1 : 0;
    if(!channel->ch_out) {
        return ad9361_tracking_control(ad9361_phy, ad9361_phy->bbdc_track_en,
                                       ad9361_phy->rfdc_track_en, ad9361_phy->quad_track_en);
    }

    return -ENOENT;
}

/**
 * set_sampling_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_sampling_frequency(void *device, char *buf, size_t len,
                                      const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t sampling_freq_hz = read_ul_value(buf);
    ad9361_set_rx_sampling_freq (ad9361_phy, sampling_freq_hz);

    return len;
}

/**
 * set_gain_control_mode_available
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_gain_control_mode_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    struct rf_gain_ctrl gc = {0};
    int32_t i;
    for(i = 0; i < sizeof(ad9361_agc_modes) / sizeof(ad9361_agc_modes[0]); i++) {
        if(strequal(ad9361_agc_modes[i], buf)) {
            break;
        }
    }
    if(i >= sizeof(ad9361_agc_modes) / sizeof(ad9361_agc_modes[0])) {
        return -EINVAL;
    }
    uint32_t mode = i;
    if (ad9361_phy->agc_mode[channel->ch_num] == mode)
        return len;
    gc.ant = ad9361_1rx1tx_channel_map(ad9361_phy, false, channel->ch_num + 1);
    gc.mode = ad9361_phy->agc_mode[channel->ch_num] = mode;
    ad9361_set_gain_ctrl_mode(ad9361_phy, &gc);

    return len;
}

/**
 * set_filter_fir_en
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_filter_fir_en(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int8_t en_dis = read_value(buf);
    if(en_dis < 0) {
        return en_dis;
    }
    en_dis = en_dis ? 1 : 0;
    if(channel->ch_out) {
        ad9361_set_tx_fir_en_dis (ad9361_phy, en_dis);
    } else {
        ad9361_set_rx_fir_en_dis (ad9361_phy, en_dis);
    }

    return len;
}

/**
 * set_rf_bandwidth_available
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_rf_bandwidth_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * set_bb_dc_offset_tracking_en
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_bb_dc_offset_tracking_en(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int8_t en_dis = read_value(buf);
    if(en_dis < 0) {
        return en_dis;
    }
    ad9361_phy->bbdc_track_en = en_dis ? 1 : 0;
    if(!channel->ch_out) {
        return ad9361_tracking_control(ad9361_phy, ad9361_phy->bbdc_track_en,
                                       ad9361_phy->rfdc_track_en, ad9361_phy->quad_track_en);
    }

    return -ENOENT;
}

static attribute_map ch_in_write_attrtibute_map[] = {
    {.name = "hardwaregain_available", .exec = set_hardwaregain_available},
    {.name = "hardwaregain", .exec = set_hardwaregain},
    {.name = "rssi", .exec = set_rssi},
    {.name = "rf_port_select", .exec = set_rf_port_select},
    {.name = "gain_control_mode", .exec = set_gain_control_mode},
    {.name = "rf_port_select_available", .exec = set_rf_port_select_available},
    {.name = "rf_bandwidth", .exec = set_rf_bandwidth},
    {.name = "rf_dc_offset_tracking_en", .exec = set_rf_dc_offset_tracking_en},
    {.name = "sampling_frequency_available", .exec = set_sampling_frequency_available},
    {.name = "quadrature_tracking_en", .exec = set_quadrature_tracking_en},
    {.name = "sampling_frequency", .exec = set_sampling_frequency},
    {.name = "gain_control_mode_available", .exec = set_gain_control_mode_available},
    {.name = "filter_fir_en", .exec = set_filter_fir_en},
    {.name = "rf_bandwidth_available", .exec = set_rf_bandwidth_available},
    {.name = "bb_dc_offset_tracking_en", .exec = set_bb_dc_offset_tracking_en},
    {.name = NULL},
};

static attribute_map ch_out_write_attrtibute_map[] = {
    {.name = "rf_port_select", .exec = set_rf_port_select},
    {.name = "hardwaregain", .exec = set_hardwaregain},
    {.name = "rssi", .exec = set_rssi},
    {.name = "hardwaregain_available", .exec = set_hardwaregain_available},
    {.name = "sampling_frequency_available", .exec = set_sampling_frequency_available},
    {.name = "rf_port_select_available", .exec = set_rf_port_select_available},
    {.name = "filter_fir_en", .exec = set_filter_fir_en},
    {.name = "sampling_frequency", .exec = set_sampling_frequency},
    {.name = "rf_bandwidth_available", .exec = set_rf_bandwidth_available},
    {.name = "rf_bandwidth", .exec = set_rf_bandwidth},
    {.name = NULL},
};

/**
 * set_frequency_available
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_frequency_available(void *device, char *buf, size_t len,
                                       const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * set_fastlock_save
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_fastlock_save(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t readin = read_ul_value(buf);
    ad9361_phy->fastlock.save_profile = readin;

    return len;
}

/**
 * set_powerdown
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_powerdown(void *device, char *buf, size_t len,
                             const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    bool res = read_value(buf) ? 1 : 0;
    switch (channel->ch_num) {
    case 0:
        ret = ad9361_synth_lo_powerdown(ad9361_phy, res ? LO_OFF : LO_ON, LO_DONTCARE);
        break;
    case 1:
        ret = ad9361_synth_lo_powerdown(ad9361_phy, LO_DONTCARE, res ? LO_OFF : LO_ON);
        break;
    }
    if(ret < 0)
        return ret;

    return len;
}

/**
 * set_fastlock_load
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_fastlock_load(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    char *line, *ptr = buf;
    uint8_t faslock_vals[16];
    uint32_t profile = 0, val, val2, i = 0;

    while ((line = strsep(&ptr, ","))) {
        if (line >= buf + len)
            break;

        ret = sscanf(line, "%"PRIu32" %"PRIu32"", &val, &val2);
        if (ret == 1) {
            faslock_vals[i++] = val;
            continue;
        } else if (ret == 2) {
            profile = val;
            faslock_vals[i++] = val2;
            continue;
        }
    }
    if (i == 16)
        ret = ad9361_fastlock_load(ad9361_phy, channel->ch_num == 1,
                                   profile, faslock_vals);
    else
        ret = -EINVAL;
    if(ret < 0)
        return ret;

    return len;
}

/**
 * set_fastlock_store
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_fastlock_store(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t profile = read_ul_value(buf);

    return ad9361_fastlock_store(ad9361_phy, channel->ch_num == 1, profile);
}

/**
 * set_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_frequency(void *device, char *buf, size_t len,
                             const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint64_t lo_freq_hz = read_ul_value(buf);
    ssize_t ret = 0;
    switch (channel->ch_num) {
    case 0:
        ret = clk_set_rate(ad9361_phy, ad9361_phy->ref_clk_scale[RX_RFPLL],
                           ad9361_to_clk(lo_freq_hz));
        break;
    case 1:
        ret = clk_set_rate(ad9361_phy, ad9361_phy->ref_clk_scale[TX_RFPLL],
                           ad9361_to_clk(lo_freq_hz));
        break;
    default:
        ret = -EINVAL;
    }
    if(ret < 0)
        return ret;

    return len;
}

/**
 * set_external
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_external(void *device, char *buf, size_t len, const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    bool select = read_value(buf) ? 1 : 0;
    ssize_t ret = 0;
    if(channel->ch_num == 0)
        ret = ad9361_set_rx_lo_int_ext(ad9361_phy, select);
    else
        ret = ad9361_set_tx_lo_int_ext(ad9361_phy, select);
    if(ret < 0)
        return ret;

    return len;
}

/**
 * set_fastlock_recall
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_fastlock_recall(void *device, char *buf, size_t len,
                                   const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    uint32_t profile = read_ul_value(buf);
    ret = ad9361_fastlock_recall(ad9361_phy, channel->ch_num == 1, profile);
    if(ret < 0)
        return ret;
    return len;
}

static attribute_map altvoltage_write_attrtibute_map[] = {
    {.name = "frequency_available", .exec = set_frequency_available},
    {.name = "fastlock_save", .exec = set_fastlock_save},
    {.name = "powerdown", .exec = set_powerdown},
    {.name = "fastlock_load", .exec = set_fastlock_load},
    {.name = "fastlock_store", .exec = set_fastlock_store},
    {.name = "frequency", .exec = set_frequency},
    {.name = "external", .exec = set_external},
    {.name = "fastlock_recall", .exec = set_fastlock_recall},
    {.name = NULL},
};

static ssize_t voltage_filter_fir_en(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int8_t en_dis = read_value(buf) ? 1 : 0;

    ad9361_set_tx_fir_en_dis (ad9361_phy, en_dis);
    ad9361_set_rx_fir_en_dis (ad9361_phy, en_dis);

    return len;
}

static attribute_map out_wr_map[] = {
    {.name = "voltage_filter_fir_en", .exec = voltage_filter_fir_en},
    {.name = NULL},
};

static attribute_map ch_write_phy_attr_map[] = {
    {.name = "voltage0", .exec = NULL, .map_in = ch_in_write_attrtibute_map, .map_out = ch_out_write_attrtibute_map},
    {.name = "voltage1", .exec = NULL, .map_in = ch_in_write_attrtibute_map, .map_out = ch_out_write_attrtibute_map},
    {.name = "voltage2", .exec = NULL, .map_in = ch_in_write_attrtibute_map, .map_out = ch_out_write_attrtibute_map},
    {.name = "voltage3", .exec = NULL, .map_in = ch_in_write_attrtibute_map, .map_out = ch_out_write_attrtibute_map},
    {.name = "altvoltage0", .exec = NULL, .map_in = altvoltage_write_attrtibute_map, .map_out = altvoltage_write_attrtibute_map},
    {.name = "altvoltage1", .exec = NULL, .map_in = altvoltage_write_attrtibute_map, .map_out = altvoltage_write_attrtibute_map},
    {.name = "out", .exec = NULL, .map_in = out_wr_map, .map_out = out_wr_map},
    {.name = NULL},
};

/**
 * get_ch_write_phy_attr_map
 * get map between attribute name and corresponding function
 * @return map
 */
attribute_map *get_ch_write_phy_attr_map()
{
    return ch_write_phy_attr_map;
}

/**
 * get_dcxo_tune_coarse
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_dcxo_tune_coarse(void *device, char *buf, size_t len,
                                    const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if (ad9361_phy->pdata->use_extclk)
        return -ENODEV;
    else
        return sprintf(buf, "%d", (int)ad9361_phy->pdata->dcxo_coarse);
}

/**
 * get_rx_path_rates
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rx_path_rates(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    unsigned long clk[6];
    ad9361_get_trx_clock_chain(ad9361_phy, clk, NULL);
    return sprintf(buf,
                   "BBPLL:%"PRIu32" ADC:%"PRIu32" R2:%"PRIu32" R1:%"PRIu32" RF:%"PRIu32" RXSAMP:%"PRIu32"",
                   clk[0], clk[1], clk[2], clk[3], clk[4], clk[5]);
}

/**
 * get_trx_rate_governor
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_trx_rate_governor(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t rate_governor;
    ad9361_get_trx_rate_gov (ad9361_phy, &rate_governor);

    return sprintf(buf, "%s", rate_governor ? "nominal" : "highest_osr");
}

/**
 * get_calib_mode_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_calib_mode_available(void *device, char *buf, size_t len,
                                        const struct channel_info *channel)
{
    return (ssize_t) sprintf(buf, "%s %s %s %s %s", ad9361_calib_mode[0],
                             ad9361_calib_mode[1], ad9361_calib_mode[2],
                             ad9361_calib_mode[3], ad9361_calib_mode[4]);
}

/**
 * get_xo_correction_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_xo_correction_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return (ssize_t) sprintf(buf, "%d", 0); /* dummy */
}

/**
 * get_gain_table_config
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_gain_table_config(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    return (ssize_t) sprintf(buf, "%d", 0); /* dummy */
}

/**
 * get_dcxo_tune_fine
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_dcxo_tune_fine(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    if (ad9361_phy->pdata->use_extclk)
        return -ENODEV;
    else
        return sprintf(buf, "%d", (int)ad9361_phy->pdata->dcxo_fine);
}

/**
 * get_dcxo_tune_fine_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_dcxo_tune_fine_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    return sprintf(buf, "%s",
                   ad9361_phy->pdata->use_extclk ? "[0 0 0]" : "[0 1 8191]");
}

/**
 * get_ensm_mode_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_ensm_mode_available(void *device, char *buf, size_t len,
                                       const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    return (ssize_t) sprintf(buf, "%s", ad9361_phy->pdata->fdd ?
                             "sleep wait alert fdd pinctrl pinctrl_fdd_indep" :
                             "sleep wait alert rx tx pinctrl");
}

/**
 * get_multichip_sync
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_multichip_sync(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    return (ssize_t) sprintf(buf, "%d", 0);  /* dummy */
}

/**
 * get_rssi_gain_step_error
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_rssi_gain_step_error(void *device, char *buf, size_t len,
                                        const struct channel_info *channel)
{
    return (ssize_t) sprintf(buf, "%d", 0);  /* dummy */
}

/**
 * get_dcxo_tune_coarse_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_dcxo_tune_coarse_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    return (ssize_t) sprintf(buf, "%s",
                             ad9361_phy->pdata->use_extclk ? "[0 0 0]" : "[0 1 63]");
}

/**
 * get_tx_path_rates
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_tx_path_rates(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    unsigned long clk[6];
    ad9361_get_trx_clock_chain(ad9361_phy, NULL, clk);

    return sprintf(buf, "BBPLL:%lu DAC:%lu T2:%lu T1:%lu TF:%lu TXSAMP:%lu",
                   clk[0], clk[1], clk[2], clk[3], clk[4], clk[5]);
}

/**
 * get_trx_rate_governor_available
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_trx_rate_governor_available(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return sprintf(buf, "%s", "nominal highest_osr");
}

/**
 * get_xo_correction
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_xo_correction(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    return (ssize_t) sprintf(buf, "%d", 0); /* dummy */
}

/**
 * get_ensm_mode
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_ensm_mode(void *device, char *buf, size_t len,
                             const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    ret = ad9361_ensm_get_state(ad9361_phy);
    if (ret < 0)
        return ret;
    if (ret >= ARRAY_SIZE(ad9361_ensm_states) ||
            ad9361_ensm_states[ret] == NULL)
        return -EIO;

    return sprintf(buf, "%s", ad9361_ensm_states[ret]);
}

/**
 * get_filter_fir_config
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_filter_fir_config(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    return sprintf(buf, "FIR Rx: %d,%d Tx: %d,%d",
                   ad9361_phy->rx_fir_ntaps, ad9361_phy->rx_fir_dec,
                   ad9361_phy->tx_fir_ntaps, ad9361_phy->tx_fir_int);
}

/**
 * get_calib_mode
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_calib_mode(void *device, char *buf, size_t len,
                              const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint8_t en_dis;
    ad9361_get_tx_auto_cal_en_dis(ad9361_phy, &en_dis);

    return (ssize_t) snprintf(buf, len, "%s", en_dis ? "auto" : "manual");
}

static attribute_map global_read_attrtibute_map[] = {
    {.name = "dcxo_tune_coarse", .exec = get_dcxo_tune_coarse},
    {.name = "rx_path_rates", .exec = get_rx_path_rates},
    {.name = "trx_rate_governor", .exec = get_trx_rate_governor},
    {.name = "calib_mode_available", .exec = get_calib_mode_available},
    {.name = "xo_correction_available", .exec = get_xo_correction_available},
    {.name = "gain_table_config", .exec = get_gain_table_config},
    {.name = "dcxo_tune_fine", .exec = get_dcxo_tune_fine},
    {.name = "dcxo_tune_fine_available", .exec = get_dcxo_tune_fine_available},
    {.name = "ensm_mode_available", .exec = get_ensm_mode_available},
    {.name = "multichip_sync", .exec = get_multichip_sync},
    {.name = "rssi_gain_step_error", .exec = get_rssi_gain_step_error},
    {.name = "dcxo_tune_coarse_available", .exec = get_dcxo_tune_coarse_available},
    {.name = "tx_path_rates", .exec = get_tx_path_rates},
    {.name = "trx_rate_governor_available", .exec = get_trx_rate_governor_available},
    {.name = "xo_correction", .exec = get_xo_correction},
    {.name = "ensm_mode", .exec = get_ensm_mode},
    {.name = "filter_fir_config", .exec = get_filter_fir_config},
    {.name = "calib_mode", .exec = get_calib_mode},
    {.name = NULL},
};

/**
 * set_trx_rate_governor
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_trx_rate_governor(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    ssize_t ret = 0;
    if(strequal(buf, "nominal")) {
        ad9361_set_trx_rate_gov (ad9361_phy, 1);
    } else if(strequal(buf, "highest_osr")) {
        ad9361_set_trx_rate_gov (ad9361_phy, 0);
    } else {
        ret =  -ENOENT;
    }

    return ret;
}

/**
 * set_dcxo_tune_coarse
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_dcxo_tune_coarse(void *device, char *buf, size_t len,
                                    const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t dcxo_coarse = read_ul_value(buf);
    dcxo_coarse = clamp_t(uint32_t, dcxo_coarse, 0, 63U);
    ad9361_phy->pdata->dcxo_coarse = dcxo_coarse;

    return ad9361_set_dcxo_tune(ad9361_phy, ad9361_phy->pdata->dcxo_coarse,
                                ad9361_phy->pdata->dcxo_fine);
}

/**
 * set_dcxo_tune_fine
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_dcxo_tune_fine(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t dcxo_fine = read_ul_value(buf);
    dcxo_fine = clamp_t(uint32_t, dcxo_fine, 0, 8191U);
    ad9361_phy->pdata->dcxo_fine = dcxo_fine;

    return ad9361_set_dcxo_tune(ad9361_phy, ad9361_phy->pdata->dcxo_coarse,
                                ad9361_phy->pdata->dcxo_fine);
}

/**
 * set_calib_mode
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_calib_mode(void *device, char *buf, size_t len,
                              const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    int32_t arg = -1;
    ssize_t ret = 0;
    uint32_t val = 0;
    val = 0;
    if (strequal(buf, "auto")) {
        ad9361_set_tx_auto_cal_en_dis (ad9361_phy, 1);
    } else if (strequal(buf, "manual")) {
        ad9361_set_tx_auto_cal_en_dis (ad9361_phy, 0);
    } else if (!strncmp(buf, "tx_quad", 7)) {
        ret = sscanf(buf, "tx_quad %"PRIi32, &arg);
        if (ret != 1)
            arg = -1;
        val = TX_QUAD_CAL;
    } else if (strequal(buf, "rf_dc_offs"))
        val = RFDC_CAL;
    else if (strequal(buf, "rssi_gain_step"))
        ret = ad9361_rssi_gain_step_calib(ad9361_phy);
    else
        return -ENOENT;

    if (val)
        ret = ad9361_do_calib(ad9361_phy, val, arg);

    return ret ? ret : len;
}

/**
 * set_ensm_mode
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_ensm_mode(void *device, char *buf, size_t len,
                             const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t val = 0;
    bool res = false;
    ad9361_phy->pdata->fdd_independent_mode = false;

    if (strequal(buf, "tx")) {
        val = ENSM_STATE_TX;
    } else if (strequal(buf, "rx")) {
        val = ENSM_STATE_RX;
    } else if (strequal(buf, "alert")) {
        val = ENSM_STATE_ALERT;
    } else if (strequal(buf, "fdd")) {
        val = ENSM_STATE_FDD;
    } else if (strequal(buf, "wait")) {
        val = ENSM_STATE_SLEEP_WAIT;
    } else if (strequal(buf, "sleep")) {
        val = ENSM_STATE_SLEEP;
    } else if (strequal(buf, "pinctrl")) {
        res = true;
        val = ENSM_STATE_SLEEP_WAIT;
    } else if (strequal(buf, "pinctrl_fdd_indep")) {
        val = ENSM_STATE_FDD;
        ad9361_phy->pdata->fdd_independent_mode = true;
    } else {
        return -ENOENT;
    }

    ad9361_set_ensm_mode(ad9361_phy, ad9361_phy->pdata->fdd, res);

    return ad9361_ensm_set_state(ad9361_phy, val, res);
}

/**
 * set_multichip_sync
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_multichip_sync(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    uint32_t readin = read_ul_value(buf);

    return ad9361_mcs(ad9361_phy, readin);
}

extern int32_t ad9361_parse_fir(struct ad9361_rf_phy *phy,
                                char *data, uint32_t size);

/**
 * set_filter_fir_config
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_filter_fir_config(void *device, char *buf, size_t len,
                                     const struct channel_info *channel)
{
    struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
    return ad9361_parse_fir(ad9361_phy, (char *)buf, len);
}

static attribute_map global_write_attrtibute_map[] = {
    {.name = "trx_rate_governor", .exec = set_trx_rate_governor},
    {.name = "dcxo_tune_coarse", .exec = set_dcxo_tune_coarse},
    {.name = "dcxo_tune_fine", .exec = set_dcxo_tune_fine},
    {.name = "calib_mode", .exec = set_calib_mode},
    {.name = "ensm_mode", .exec = set_ensm_mode},
    {.name = "multichip_sync", .exec = set_multichip_sync},
    {.name = "filter_fir_config", .exec = set_filter_fir_config},
    {.name = NULL},
};

ssize_t get_phy_xml(char** xml, const char *device_name, uint8_t ch_no)
{
    *xml = (char*)phy_xml;

    return 0;
}

static attribute_map phy_attr_map[] = {
    {.name = "", .exec = NULL, .map_in = ch_read_phy_attr_map, .map_out = ch_write_phy_attr_map, .map_in_global = global_read_attrtibute_map, .map_out_global = global_write_attrtibute_map},
    {.name = NULL},
};
/**
 * get_ch_read_phy_attr_map
 * get map between attribute name and corresponding function
 * @return map
 */
attribute_map *get_phy_attr_map(const char *device_name)
{
    phy_attr_map->name = device_name;

    return phy_attr_map;
}

/***************************************************************************//**
 *   @file   ad9361_api.c
 *   @brief  Implementation of AD9361 API Driver.
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
#include "ad9361.h"
#include "ad9361_api.h"

/***************************************************************************//**
 * @brief ad9361_init
*******************************************************************************/
struct ad9361_rf_phy *ad9361_init (AD9361_InitParam *init_param)
{
	struct ad9361_rf_phy *phy;
	int32_t ret = 0;
	int32_t rev = 0;
	int32_t i   = 0;

	phy = malloc(sizeof(*phy));
	if (!phy) {
		return ERR_PTR(-ENOMEM);
	}

	phy->pdata = malloc(sizeof(*phy->pdata));
	if (!phy->pdata) {
		return ERR_PTR(-ENOMEM);
	}

	/* Mode Setup */
	phy->pdata->rx2tx2 = init_param->frequency_division_duplex_mode_enable;
	phy->pdata->fdd = init_param->two_rx_two_tx_mode_enable;
	phy->pdata->split_gt = init_param->split_gain_table_mode_enable;
	phy->pdata->tdd_use_fdd_tables = init_param->tdd_use_fdd_vco_tables_enable;
	phy->pdata->tdd_use_dual_synth = init_param->tdd_use_dual_synth_mode_enable;
	phy->pdata->dcxo_coarse = init_param->dcxo_coarse_and_fine_tune[0];
	phy->pdata->dcxo_fine = init_param->dcxo_coarse_and_fine_tune[1];
	if((phy->pdata->dcxo_coarse) || (phy->pdata->dcxo_fine)) {
		phy->pdata->use_extclk = 0;
	}
	else {
		phy->pdata->use_extclk = 1;
	}
	phy->pdata->ensm_pin_pulse_mode = init_param->ensm_enable_pin_pulse_mode_enable;
	phy->pdata->ensm_pin_ctrl = init_param->ensm_enable_txnrx_control_enable;
	phy->pdata->debug_mode = true;
	phy->pdata->rf_rx_input_sel = init_param->rx_rf_port_input_select;
	phy->pdata->rf_tx_output_sel = init_param->tx_rf_port_output_select;
	for(i = 0; i < 6; i++) {
		phy->pdata->rx_path_clks[i] = init_param->rx_path_clock_frequencies[i];
	}
	for(i = 0; i < 6; i++) {
		phy->pdata->tx_path_clks[i] = init_param->tx_path_clock_frequencies[i];
	}
	phy->pdata->rx_synth_freq = init_param->rx_synthesizer_frequency_hz;
	phy->pdata->tx_synth_freq = init_param->tx_synthesizer_frequency_hz;
	phy->pdata->rf_rx_bandwidth_Hz = init_param->rf_rx_bandwidth_hz;
	phy->pdata->rf_tx_bandwidth_Hz = init_param->rf_tx_bandwidth_hz;
	phy->pdata->tx_atten = init_param->tx_attenuation_mdB;
	phy->pdata->update_tx_gain_via_alert = init_param->update_tx_gain_in_alert_enable;
	phy->pdata->gpio_resetb = 54 + 45;
	/* Gain Control */
	phy->pdata->gain_ctrl.rx1_mode = init_param->gc_rx1_mode;
	phy->pdata->gain_ctrl.rx2_mode = init_param->gc_rx2_mode;
	phy->pdata->gain_ctrl.adc_ovr_sample_size = init_param->gc_adc_ovr_sample_size;
	phy->pdata->gain_ctrl.adc_small_overload_thresh = init_param->gc_adc_small_overload_thresh;
	phy->pdata->gain_ctrl.adc_large_overload_thresh = init_param->gc_adc_large_overload_thresh;
	phy->pdata->gain_ctrl.lmt_overload_high_thresh = init_param->gc_lmt_overload_high_thresh;
	phy->pdata->gain_ctrl.lmt_overload_low_thresh = init_param->gc_lmt_overload_low_thresh;
	phy->pdata->gain_ctrl.analog_settling_time = init_param->gc_analog_settling_time;
	phy->pdata->gain_ctrl.dec_pow_measuremnt_duration = init_param->gc_dec_pow_measurement_duration;
	phy->pdata->gain_ctrl.low_power_thresh = init_param->gc_low_power_thresh;
	phy->pdata->gain_ctrl.dig_gain_en = init_param->gc_dig_gain_enable;
	phy->pdata->gain_ctrl.max_dig_gain = init_param->gc_max_dig_gain;
	/* Manual Gain Control Setup */
	phy->pdata->gain_ctrl.mgc_rx1_ctrl_inp_en = init_param->mgc_rx1_ctrl_inp_enable;
	phy->pdata->gain_ctrl.mgc_rx2_ctrl_inp_en = init_param->mgc_rx2_ctrl_inp_enable;
	phy->pdata->gain_ctrl.mgc_inc_gain_step = init_param->mgc_inc_gain_step;
	phy->pdata->gain_ctrl.mgc_dec_gain_step = init_param->mgc_dec_gain_step;
	phy->pdata->gain_ctrl.mgc_split_table_ctrl_inp_gain_mode = init_param->mgc_split_table_ctrl_inp_gain_mode;
	/* Automatic Gain Control Setup */
	phy->pdata->gain_ctrl.agc_attack_delay_us = init_param->agc_attack_delay_us;
	phy->pdata->gain_ctrl.agc_settling_delay = init_param->agc_settling_delay;
	phy->pdata->gain_ctrl.agc_outer_thresh_high = init_param->agc_outer_thresh_high;
	phy->pdata->gain_ctrl.agc_outer_thresh_high_dec_steps = init_param->agc_outer_thresh_high_dec_steps;
	phy->pdata->gain_ctrl.agc_inner_thresh_high = init_param->agc_inner_thresh_high;
	phy->pdata->gain_ctrl.agc_inner_thresh_high_dec_steps = init_param->agc_inner_thresh_high_dec_steps;
	phy->pdata->gain_ctrl.agc_inner_thresh_low = init_param->agc_inner_thresh_low;
	phy->pdata->gain_ctrl.agc_inner_thresh_low_inc_steps = init_param->agc_inner_thresh_low_inc_steps;
	phy->pdata->gain_ctrl.agc_outer_thresh_low = init_param->agc_outer_thresh_low;
	phy->pdata->gain_ctrl.agc_outer_thresh_low_inc_steps = init_param->agc_outer_thresh_low_inc_steps;
	phy->pdata->gain_ctrl.adc_small_overload_exceed_counter = init_param->agc_adc_small_overload_exceed_counter;
	phy->pdata->gain_ctrl.adc_large_overload_exceed_counter = init_param->agc_adc_large_overload_exceed_counter;
	phy->pdata->gain_ctrl.adc_large_overload_inc_steps = init_param->agc_adc_large_overload_inc_steps;
	phy->pdata->gain_ctrl.adc_lmt_small_overload_prevent_gain_inc = init_param->agc_adc_lmt_small_overload_prevent_gain_inc_enable;
	phy->pdata->gain_ctrl.lmt_overload_large_exceed_counter = init_param->agc_lmt_overload_large_exceed_counter;
	phy->pdata->gain_ctrl.lmt_overload_small_exceed_counter = init_param->agc_lmt_overload_small_exceed_counter;
	phy->pdata->gain_ctrl.lmt_overload_large_inc_steps = init_param->agc_lmt_overload_large_inc_steps;
	phy->pdata->gain_ctrl.dig_saturation_exceed_counter = init_param->agc_dig_saturation_exceed_counter;
	phy->pdata->gain_ctrl.dig_gain_step_size = init_param->agc_dig_gain_step_size;
	phy->pdata->gain_ctrl.sync_for_gain_counter_en = init_param->agc_sync_for_gain_counter_enable;
	phy->pdata->gain_ctrl.gain_update_counter = init_param->agc_gain_update_counter;
	phy->pdata->gain_ctrl.immed_gain_change_if_large_adc_overload = init_param->agc_immed_gain_change_if_large_adc_overload_enable;
	phy->pdata->gain_ctrl.immed_gain_change_if_large_lmt_overload = init_param->agc_immed_gain_change_if_large_lmt_overload_enable;
	/* RSSI */
	phy->pdata->rssi_ctrl.restart_mode = init_param->rssi_restart_mode;
	phy->pdata->rssi_ctrl.rssi_unit_is_rx_samples = init_param->rssi_unit_is_rx_samples_enable;
	phy->pdata->rssi_ctrl.rssi_delay = init_param->rssi_delay;
	phy->pdata->rssi_ctrl.rssi_wait = init_param->rssi_wait;
	phy->pdata->rssi_ctrl.rssi_duration = init_param->rssi_duration;
	/* FIX - Port Control */
	phy->pdata->port_ctrl.pp_conf[0] = 0xC8;
	phy->pdata->port_ctrl.pp_conf[1] = 0x0;
	phy->pdata->port_ctrl.pp_conf[2] = 0x10;
	phy->pdata->port_ctrl.rx_clk_data_delay = 0x4;
	phy->pdata->port_ctrl.tx_clk_data_delay = 0x70;
	phy->pdata->port_ctrl.digital_io_ctrl = 0;
	phy->pdata->port_ctrl.lvds_bias_ctrl = 0x22;
	phy->pdata->port_ctrl.lvds_invert[0] = 0;
	phy->pdata->port_ctrl.lvds_invert[1] = 0;
	/* Control Outputs */
	phy->pdata->ctrl_outs_ctrl.index = init_param->ctrl_outs_index;
	phy->pdata->ctrl_outs_ctrl.en_mask = init_param->ctrl_outs_enable_mask;
	/* FIX - Elna Control */
	phy->pdata->elna_ctrl.gain_mdB = 0;
	phy->pdata->elna_ctrl.bypass_loss_mdB = 0;
	phy->pdata->elna_ctrl.elna_1_control_en = 0;
	phy->pdata->elna_ctrl.elna_2_control_en = 0;
	/* AuxADC Temp Sense Control */
	phy->pdata->auxadc_ctrl.offset = init_param->temp_sense_offset_signed;
	phy->pdata->auxadc_ctrl.temp_time_inteval_ms = init_param->temp_sense_measurement_interval_ms;
	phy->pdata->auxadc_ctrl.temp_sensor_decimation = 256;
	phy->pdata->auxadc_ctrl.periodic_temp_measuremnt = init_param->temp_sense_periodic_measurement_enable;
	phy->pdata->auxadc_ctrl.auxadc_clock_rate = 40000000;
	phy->pdata->auxadc_ctrl.auxadc_decimation = 256;

	phy->rx_eq_2tx = false;

	phy->clk_refin = malloc(sizeof(*phy->clk_refin));
	phy->clk_refin->rate = 40000000;

	phy->current_table = RXGAIN_TBLS_END;
	phy->bypass_tx_fir = true;
	phy->bypass_rx_fir = true;
	phy->rate_governor = 1;
	phy->rfdc_track_en = true;
	phy->bbdc_track_en = true;
	phy->quad_track_en = true;

	if (ad9361_reset(phy)) {
		ad9361_spi_write(REG_SPI_CONF, SOFT_RESET | _SOFT_RESET); /* RESET */
		ad9361_spi_write(REG_SPI_CONF, 0x0);
	}

	ret = ad9361_spi_read(REG_PRODUCT_ID);
	if ((ret & PRODUCT_ID_MASK) != PRODUCT_ID_9361) {
		printf("%s : Unsupported PRODUCT_ID 0x%X", __func__, (unsigned int)ret);
		ret = -ENODEV;
		goto out;
	}
	rev = ret & REV_MASK;

	ret = register_clocks(phy);
	if (ret < 0)
		goto out;

	ad9361_init_gain_tables(phy);

	ret = ad9361_setup(phy);
	if (ret < 0)
		goto out;

	printf("%s : AD9361 Rev %d successfully initialized\n", __func__, (int)rev);

	return phy;

out:
	printf("%s : AD9361 initialization error\n", __func__);

	return ERR_PTR(ENODEV);
}

/***************************************************************************//**
 * @brief ad9361_set_en_state_machine_mode
*******************************************************************************/
int32_t ad9361_set_en_state_machine_mode (struct ad9361_rf_phy *phy,
										  uint32_t mode)
{
	ad9361_ensm_force_state(phy, mode);

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_get_en_state_machine_mode
*******************************************************************************/
int32_t ad9361_get_en_state_machine_mode (struct ad9361_rf_phy *phy,
										  uint32_t *mode)
{
	*mode = phy->curr_ensm_state;

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_rf_gain
*******************************************************************************/
int32_t ad9361_set_rx_rf_gain (struct ad9361_rf_phy *phy,
							   uint8_t ch, int32_t gain_db)
{
	struct rf_rx_gain rx_gain = {0};
	int32_t ret = 0;

	rx_gain.gain_db = gain_db;
	ret = ad9361_set_rx_gain(phy, ch + 1, &rx_gain);

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_rf_gain
*******************************************************************************/
int32_t ad9361_get_rx_rf_gain (struct ad9361_rf_phy *phy,
							   uint8_t ch, int32_t *gain_db)
{
	struct rf_rx_gain rx_gain = {0};
	int32_t ret = 0;

	ret = ad9361_get_rx_gain(phy, ch + 1, &rx_gain);
	*gain_db = rx_gain.gain_db;

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_rf_bandwidth
*******************************************************************************/
int32_t ad9361_set_rx_rf_bandwidth (struct ad9361_rf_phy *phy,
									uint32_t bandwidth_hz)
{
	int32_t ret = 0;

	if (phy->current_rx_bw_Hz != bandwidth_hz)
		ret = ad9361_update_rf_bandwidth(phy, bandwidth_hz,
				phy->current_tx_bw_Hz);
	else
		ret = 0;

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_rx_rf_bandwidth
*******************************************************************************/
int32_t ad9361_get_rx_rf_bandwidth (struct ad9361_rf_phy *phy,
									uint32_t *bandwidth_hz)
{
	*bandwidth_hz = phy->current_rx_bw_Hz;

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_sampling_freq
*******************************************************************************/
int32_t ad9361_set_rx_sampling_freq (struct ad9361_rf_phy *phy,
									 uint32_t sampling_freq_hz)
{
	int32_t ret;
	uint32_t rx[6], tx[6];

	ret = ad9361_calculate_rf_clock_chain(phy, sampling_freq_hz,
		phy->rate_governor, rx, tx);
	if (ret < 0)
		return ret;

	ad9361_set_trx_clock_chain(phy, rx, tx);

	ret = ad9361_update_rf_bandwidth(phy, phy->current_rx_bw_Hz,
					phy->current_tx_bw_Hz);

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_rx_sampling_freq
*******************************************************************************/
int32_t ad9361_get_rx_sampling_freq (struct ad9361_rf_phy *phy,
									 uint32_t *sampling_freq_hz)
{
	*sampling_freq_hz = (uint32_t)clk_get_rate(phy,
										phy->ref_clk_scale[RX_SAMPL_CLK]);

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_lo_freq
*******************************************************************************/
int32_t ad9361_set_rx_lo_freq (struct ad9361_rf_phy *phy,
							   uint64_t lo_freq_hz)
{
	int32_t ret;

	ret = clk_set_rate(phy, phy->ref_clk_scale[RX_RFPLL],
			   ad9361_to_clk(lo_freq_hz));

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_rx_lo_freq
*******************************************************************************/
int32_t ad9361_get_rx_lo_freq (struct ad9361_rf_phy *phy,
							   uint64_t *lo_freq_hz)
{
	*lo_freq_hz = ad9361_from_clk(clk_get_rate(phy,
										phy->ref_clk_scale[RX_RFPLL]));

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_get_rx_rssi
*******************************************************************************/
int32_t ad9361_get_rx_rssi (struct ad9361_rf_phy *phy,
							uint8_t ch, struct rf_rssi *rssi)
{
	int32_t ret;

	rssi->ant = ch + 1;
	rssi->duration = 1;
	ret = ad9361_read_rssi(phy, rssi);

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_gain_control_mode
*******************************************************************************/
int32_t ad9361_set_rx_gain_control_mode (struct ad9361_rf_phy *phy,
										 uint8_t ch, uint8_t gc_mode)
{
	struct rf_gain_ctrl gc = {0};

	gc.ant = ch + 1;
	gc.mode = phy->agc_mode[ch] = gc_mode;

	ad9361_set_gain_ctrl_mode(phy, &gc);

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_get_rx_gain_control_mode
*******************************************************************************/
int32_t ad9361_get_rx_gain_control_mode (struct ad9361_rf_phy *phy,
										 uint8_t ch, uint8_t *gc_mode)
{
	*gc_mode = phy->agc_mode[ch];

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_fir_config
*******************************************************************************/
int32_t ad9361_set_rx_fir_config (struct ad9361_rf_phy *phy,
								  AD9361_RXFIRConfig fir_cfg)
{
	int32_t ret;

	ret = ad9361_load_fir_filter_coef(phy, fir_cfg.rx | FIR_IS_RX,
			fir_cfg.rx_gain, 64, fir_cfg.rx_coef);
	phy->rx_fir_dec = fir_cfg.rx_dec;

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_fir_en_dis
*******************************************************************************/
int32_t ad9361_set_rx_fir_en_dis (struct ad9361_rf_phy *phy,
								  uint8_t en_dis)
{
	int32_t ret = 0;

	if(phy->bypass_rx_fir == !en_dis)
		return ret;

	phy->bypass_rx_fir = !en_dis;
	ret = ad9361_validate_enable_fir(phy);
	if (ret < 0) {
		phy->bypass_rx_fir = true;
	}

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_rx_fir_en_dis
*******************************************************************************/
int32_t ad9361_get_rx_fir_en_dis (struct ad9361_rf_phy *phy,
								  uint8_t *en_dis)
{
	*en_dis = !phy->bypass_rx_fir;

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_tx_attenuation
*******************************************************************************/
int32_t ad9361_set_tx_attenuation (struct ad9361_rf_phy *phy,
								   uint8_t ch, uint32_t attenuation_mdb)
{
	int32_t ret;

	ret = ad9361_set_tx_atten(phy, attenuation_mdb,
		ch == 0, ch == 1,
		!phy->pdata->update_tx_gain_via_alert);

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_tx_attenuation
*******************************************************************************/
int32_t ad9361_get_tx_attenuation (struct ad9361_rf_phy *phy,
								   uint8_t ch, uint32_t *attenuation_db)
{
	uint32_t ret;

	ret = ad9361_get_tx_atten(phy, ch + 1);
	if(ret < 0)
		return EINVAL;
	*attenuation_db = ret;

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_get_tx_attenuation
*******************************************************************************/
int32_t ad9361_set_tx_rf_bandwidth (struct ad9361_rf_phy *phy,
									uint32_t  bandwidth_hz)
{
	int32_t ret = 0;

	if (phy->current_tx_bw_Hz != bandwidth_hz)
		ret = ad9361_update_rf_bandwidth(phy,
				phy->current_rx_bw_Hz, bandwidth_hz);
	else
		ret = 0;

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_tx_attenuation
*******************************************************************************/
int32_t ad9361_get_tx_rf_bandwidth (struct ad9361_rf_phy *phy,
									uint32_t *bandwidth_hz)
{
	*bandwidth_hz = phy->current_tx_bw_Hz;

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_tx_sampling_freq
*******************************************************************************/
int32_t ad9361_set_tx_sampling_freq (struct ad9361_rf_phy *phy,
									 uint32_t sampling_freq_hz)
{
	int32_t ret;
	uint32_t rx[6], tx[6];

	ret = ad9361_calculate_rf_clock_chain(phy, sampling_freq_hz,
		phy->rate_governor, rx, tx);
	if (ret < 0)
		return ret;

	ad9361_set_trx_clock_chain(phy, rx, tx);

	ret = ad9361_update_rf_bandwidth(phy, phy->current_rx_bw_Hz,
					phy->current_tx_bw_Hz);

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_tx_sampling_freq
*******************************************************************************/
int32_t ad9361_get_tx_sampling_freq (struct ad9361_rf_phy *phy,
									 uint32_t *sampling_freq_hz)
{
	*sampling_freq_hz = (uint32_t)clk_get_rate(phy,
										phy->ref_clk_scale[TX_SAMPL_CLK]);

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_tx_lo_freq
*******************************************************************************/
int32_t ad9361_set_tx_lo_freq (struct ad9361_rf_phy *phy,
							   uint64_t lo_freq_hz)
{
	int32_t ret;

	ret = clk_set_rate(phy, phy->ref_clk_scale[TX_RFPLL],
			   ad9361_to_clk(lo_freq_hz));

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_tx_lo_freq
*******************************************************************************/
int32_t ad9361_get_tx_lo_freq (struct ad9361_rf_phy *phy,
							   uint64_t *lo_freq_hz)
{
	*lo_freq_hz = ad9361_from_clk(clk_get_rate(phy,
										phy->ref_clk_scale[TX_RFPLL]));

	return 0;
}

/***************************************************************************//**
 * @brief ad9361_set_tx_fir_config
*******************************************************************************/
int32_t ad9361_set_tx_fir_config (struct ad9361_rf_phy *phy,
								  AD9361_TXFIRConfig fir_cfg)
{
	int32_t ret;

	ret = ad9361_load_fir_filter_coef(phy, fir_cfg.tx,
			fir_cfg.tx_gain, 64, fir_cfg.tx_coef);
	phy->tx_fir_int = fir_cfg.tx_int;

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_set_rx_fir_en_dis
*******************************************************************************/
int32_t ad9361_set_tx_fir_en_dis (struct ad9361_rf_phy *phy,
								  uint8_t en_dis)
{
	int32_t ret = 0;

	if(phy->bypass_tx_fir == !en_dis)
		return ret;

	phy->bypass_tx_fir = !en_dis;
	ret = ad9361_validate_enable_fir(phy);
	if (ret < 0) {
		phy->bypass_tx_fir = true;
	}

	return ret;
}

/***************************************************************************//**
 * @brief ad9361_get_rx_fir_en_dis
*******************************************************************************/
int32_t ad9361_get_tx_fir_en_dis (struct ad9361_rf_phy *phy,
								  uint8_t *en_dis)
{
	*en_dis = !phy->bypass_tx_fir;

	return 0;
}

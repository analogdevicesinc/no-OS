
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>
#include <xil_cache.h>
#include "ad9361_api.h"
#include "adc_core.h"
#include "dac_core.h"
#include "console.h"
#include "command.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
extern command      cmd_list[];
extern char			cmd_no;
extern cmd_function	cmd_functions[11];
unsigned char		cmd				 =  0;
double				param[5]		 = {0, 0, 0, 0, 0};
char				param_no		 =  0;
int					cmd_type		 = -1;
char				invalid_cmd		 =  0;
char				received_cmd[30] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
AD9361_InitParam default_init_param = {
	/* Mode Setup */
	1,			// two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
	1,			// frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
	0,			// split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
	{8, 4096},	// dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
	0,			// ensm_enable_pin_level_mode_enable *** adi,ensm-enable-pin-level-mode-enable
	0,			// ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
	0,			// rx_rf_port_input_select *** adi,rx-rf-port-input-select
	0,			// tx_rf_port_input_select *** adi,tx-rf-port-input-select
	{983040000, 245760000, 122880000, 61440000, 30720000, 30720000},	// rx_path_clock_frequencies[6]	*** adi,rx-path-clock-frequencies
	{983040000, 122880000, 122880000, 61440000, 30720000, 30720000},	// tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
	2400000000UL,	// rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
	2400000000UL,	// tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
	18000000,	// rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
	18000000,	// rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
	10000,		// tx_attenuation_mdB *** adi,tx-attenuation-mdB
	/* Gain Control */
	2,			// gc_rx1_mode *** adi,gc-rx1-mode
	2,			// gc_rx2_mode *** adi,gc-rx2-mode
	4,			// gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
	47,			// gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
	58,			// gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
	800,		// gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
	704,		// gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
	8,			// gc_analog_settling_time *** adi,gc-analog-settling-time
	8192,		// gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
	24,			// gc_low_power_thresh *** adi,gc-low-power-thresh
	0,			// gc_dig_gain_enable *** adi,gc-dig-gain-enable
	15,			// gc_max_dig_gain *** adi,gc-max-dig-gain
	/* Manual Gain Control Setup */
	0,			// mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
	0,			// mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
	2,			// mgc_inc_gain_step *** adi,mgc-inc-gain-step
	2,			// mgc_dec_gain_step *** adi,mgc-dec-gain-step
	0,			// mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
	/* Automatic Gain Control Setup */
	10,			// agc_attack_delay_us *** adi,agc-attack-delay-us
	10,			// agc_settling_delay *** adi,agc-settling-delay
	5,			// agc_outer_thresh_high *** adi,agc-outer-thresh-high
	2,			// agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
	10,			// agc_inner_thresh_high *** adi,agc-inner-thresh-high
	1,			// agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
	12,			// agc_inner_thresh_low *** adi,agc-inner-thresh-low
	1,			// agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
	18,			// agc_outer_thresh_low *** adi,agc-outer-thresh-low
	2,			// agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
	10,			// agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
	10,			// agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
	2,			// agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
	0,			// agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
	10,			// agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
	10,			// agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
	2,			// agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
	3,			// agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
	4,			// agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
	0,			// agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
	30698,		// agc_gain_update_counter *** adi,agc-gain-update-counter
	0,			// agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
	0,			// agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
	/* RSSI */
	3,			// rssi_restart_mode *** adi,rssi-restart-mode
	0,			// rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
	1,			// rssi_delay *** adi,rssi-delay
	1,			// rssi_wait *** adi,rssi-wait
	1000,		// rssi_duration *** adi,rssi-duration
	/* Control Outputs */
	0,			// ctrl_outs_index *** adi,ctrl-outs-index
	0xFF,		// ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
	/* AuxADC Temp Sense Control */
	1000,		// temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
	0xCE,		// temp_sense_offset_signed *** adi,temp-sense-offset-signed
	1,			// temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
};

AD9361_RXFIRConfig rx_fir_config = {
	3, // rx;
	0, // rx_gain;
	1, // rx_dec;
	{-4, -6, -37, 35, 186, 86, -284, 315,
	 107, 219, -4, 271, 558, -307, -1182, -356,
	 658, 157, 207, 1648, 790, -2525, -2553, 748,
	 865, -476, 3737, 6560, -3583, -14731, -5278, 14819,
	 14819, -5278, -14731, -3583, 6560, 3737, -476, 865,
	 748, -2553, -2525, 790, 1648, 207, 157, 658,
	 -356, -1182, -307, 558, 271, -4, 219, 107,
	 -315, -284, 86, 186, 35, -37, -6, -4} // rx_coef[64];
};

AD9361_TXFIRConfig tx_fir_config = {
	3, // tx;
	-6, // tx_gain;
	1, // tx_inc;
	{-4, -6, -37, 35, 186, 86, -284, 315,
	 107, 219, -4, 271, 558, -307, -1182, -356,
	 658, 157, 207, 1648, 790, -2525, -2553, 748,
	 865, -476, 3737, 6560, -3583, -14731, -5278, 14819,
	 14819, -5278, -14731, -3583, 6560, 3737, -476, 865,
	 748, -2553, -2525, 790, 1648, 207, 157, 658,
	 -356, -1182, -307, 558, 271, -4, 219, 107,
	 -315, -284, 86, 186, 35, -37, -6, -4} // tx_coef[64];
};
struct ad9361_rf_phy *ad9361_phy;

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
    Xil_ICacheEnable();
    Xil_DCacheEnable();

	gpio_init(XPAR_PS7_GPIO_0_DEVICE_ID);
    gpio_direction(54 + 45, 1);
	spi_init(XPAR_PS7_SPI_0_DEVICE_ID, 1, 0);

	ad9361_phy = ad9361_init(&default_init_param);

    ad9361_set_tx_fir_config(ad9361_phy, tx_fir_config);
    ad9361_set_rx_fir_config(ad9361_phy, rx_fir_config);

    adc_init();
    dac_init(DATA_SEL_DDS);

    usleep(1000000);

    //adc_capture(16384, 0x800000);

    //while(1);

    get_help(NULL, 0);

    while(1)
    {
    	console_get_command(received_cmd);
        invalid_cmd = 0;
        for(cmd = 0; cmd < cmd_no; cmd++)
        {
            param_no = 0;
            cmd_type = console_check_commands(received_cmd, cmd_list[cmd].name,
                                              param, &param_no);
            if(cmd_type == UNKNOWN_CMD)
            {
                invalid_cmd++;
            }
            else
            {
            	cmd_list[cmd].function(param, param_no);
            }
        }
        if(invalid_cmd == cmd_no)
        {
        	console_print("Invalid command!\n");
        }
    }

    Xil_DCacheDisable();
    Xil_ICacheDisable();

	return 0;
}

/*!
 * \brief     APIs for Spectrum Sniffer
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SNIFFER
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_private_device.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_rx_spectrum_sniffer.h"
#include "adi_apollo_bf_rx_datin.h"
#include "adi_apollo_sniffer.h"
#include "adi_utils.h"

#define DEFAULT_FFT_TIMEOUT_US 1000

static uint32_t calc_rx_sniffer_base(int32_t sniffer_index);
static uint32_t calc_rx_datin_base(int32_t sniffer_index);
static uint32_t calc_adc_mux(adi_apollo_adc_idx_e adc);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_sniffer_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t enable) {
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint16_t side_index;
    adi_apollo_blk_sel_t side;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    for (side_index = 0; side_index < ADI_APOLLO_SNIFFER_NUM; side_index++) {
        side = sniffers & (ADI_APOLLO_SNIFFER_A << side_index);
        if (side > 0) {

            regmap_base_addr = calc_rx_sniffer_base(side_index);
            // Spectrum sniffer enable
            err = adi_apollo_hal_bf_set(device, BF_SPECTRUM_SNIFFER_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return err;   
}

int32_t adi_apollo_sniffer_adc_mux_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint16_t adc) {
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint16_t side_index;
    adi_apollo_blk_sel_t side;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN( ! (adc == ADI_APOLLO_ADC_0 || adc == ADI_APOLLO_ADC_1 || adc == ADI_APOLLO_ADC_2 || adc == ADI_APOLLO_ADC_3));
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    for (side_index = 0; side_index < ADI_APOLLO_SNIFFER_NUM; side_index++) {
        side = sniffers & (ADI_APOLLO_SNIFFER_A << side_index);
        if (side > 0) {

            regmap_base_addr = calc_rx_datin_base(side_index);

            // ADC mux   
            err = adi_apollo_hal_bf_set(device, BF_ADC_0_MUX_SEL_INFO(regmap_base_addr), calc_adc_mux(adc));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return err;   
}

int32_t adi_apollo_sniffer_init(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_init_t *config) {
    // Must be called prior to oneshot sync
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint16_t side_index;
    adi_apollo_blk_sel_t side;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    for (side_index = 0; side_index < ADI_APOLLO_SNIFFER_NUM; side_index++) {
        side = sniffers & (ADI_APOLLO_SNIFFER_A << side_index);
        if (side > 0) {

            regmap_base_addr = calc_rx_sniffer_base(side_index);
            // Spectrum sniffer enable
            err = adi_apollo_hal_bf_set(device, BF_SPECTRUM_SNIFFER_EN_INFO(regmap_base_addr), config->sniffer_enable);
            ADI_APOLLO_ERROR_RETURN(err);

            // fft_hold_sel : To select between GPIO and regmap
            err = adi_apollo_hal_bf_set(device, BF_FFT_HOLD_SEL_INFO(regmap_base_addr), config->fft_hold_sel);
            ADI_APOLLO_ERROR_RETURN(err);

            // fft_enable_sel : To select between GPIO and regmap
            err = adi_apollo_hal_bf_set(device, BF_FFT_ENABLE_SEL_INFO(regmap_base_addr), config->fft_enable_sel);
            ADI_APOLLO_ERROR_RETURN(err);

            // real_mode : To select between real fft and complex fft
            err = adi_apollo_hal_bf_set(device, BF_REAL_MODE_INFO(regmap_base_addr), config->real_mode);
            ADI_APOLLO_ERROR_RETURN(err);

            // max_threshold: To set max threshold.
            err = adi_apollo_hal_bf_set(device, BF_MAX_THRESHOLD_INFO(regmap_base_addr), config->max_threshold);
            ADI_APOLLO_ERROR_RETURN(err);

            // min_threshold: To set min threshold
            err = adi_apollo_hal_bf_set(device, BF_MIN_THRESHOLD_INFO(regmap_base_addr), config->min_threshold);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return err;
}

int32_t adi_apollo_sniffer_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_pgm_t *config) {
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint8_t mag_iq, force_sort_store;
    uint16_t side_index;
    adi_apollo_blk_sel_t side;

    // Check parameters: some configurations are invalid per user guide

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    // Get parameters for different modes

    if (config->sniffer_mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE) {  // IQ mode
        mag_iq = 0;
    } else {                                                            // Magnitude mode
        mag_iq = 1;
    }

    if (config->sniffer_mode % 2 == 1) {                                // Instantaneous/debug mode
        force_sort_store = 1;
    } else {                                                            // Normal mode
        force_sort_store = 0;
    }

    if (config->sniffer_mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE) {
        ADI_APOLLO_INVALID_PARAM_RETURN(config->sort_enable != 0);
        ADI_APOLLO_INVALID_PARAM_RETURN(config->bottom_fft_enable != 0);
        ADI_APOLLO_INVALID_PARAM_RETURN(config->continuous_mode == 1);
    }

    if (config->continuous_mode == 1) {
        ADI_APOLLO_INVALID_PARAM_RETURN(config->alpha_factor != 0);
    }

    ADI_APOLLO_INVALID_PARAM_RETURN(config->alpha_factor > 9); // alpha factor can be 0 - 9

    // Set values per side

    err = adi_apollo_sniffer_adc_mux_set(device, sniffers, config->adc);
    ADI_APOLLO_ERROR_RETURN(err);
    
    for (side_index = 0; side_index < ADI_APOLLO_SNIFFER_NUM; side_index++) {
        side = sniffers & (ADI_APOLLO_SNIFFER_A << side_index);
        if (side > 0) {

            regmap_base_addr = calc_rx_sniffer_base(side_index);

            // bottom_fft_enable    
            err = adi_apollo_hal_bf_set(device, BF_BOTTOM_FFT_ENABLE_INFO(regmap_base_addr), config->bottom_fft_enable);
            ADI_APOLLO_ERROR_RETURN(err);

            // window_enable
            err = adi_apollo_hal_bf_set(device, BF_WINDOW_ENABLE_INFO(regmap_base_addr), config->window_enable);
            ADI_APOLLO_ERROR_RETURN(err);

            // sort_enable
            err = adi_apollo_hal_bf_set(device, BF_SORT_EN_INFO(regmap_base_addr), config->sort_enable);
            ADI_APOLLO_ERROR_RETURN(err);

            // magnitude_iq_n : To select between magnitude or IQ mode
            err = adi_apollo_hal_bf_set(device, BF_MAGNITUDE_IQ_N_INFO(regmap_base_addr), mag_iq);
            ADI_APOLLO_ERROR_RETURN(err);

            // force_sort_store_enable : to force instantaneous sorting/storing of data
            err = adi_apollo_hal_bf_set(device, BF_FORCE_SORT_STORE_INFO(regmap_base_addr), force_sort_store);
            ADI_APOLLO_ERROR_RETURN(err);

            // low_power_enable
            err = adi_apollo_hal_bf_set(device, BF_LOW_POWER_INFO(regmap_base_addr), config->low_power_enable);
            ADI_APOLLO_ERROR_RETURN(err);

            // dither_enable
            err = adi_apollo_hal_bf_set(device, BF_DITHER_ENABLE_INFO(regmap_base_addr), config->dither_enable);
            ADI_APOLLO_ERROR_RETURN(err);

            // continuous_mode
            err = adi_apollo_hal_bf_set(device, BF_CONTINUOUS_MODE_INFO(regmap_base_addr), config->continuous_mode);
            ADI_APOLLO_ERROR_RETURN(err);

            // alpha_factor
            err = adi_apollo_hal_bf_set(device, BF_ALPHA_FACTOR_INFO(regmap_base_addr), config->alpha_factor);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return err;
}

int32_t adi_apollo_sniffer_fft_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t enable) {
    int32_t err = API_CMS_ERROR_OK;
    uint32_t regmap_base_addr = 0;
    uint16_t s;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    for (s = 0; s < ADI_APOLLO_SNIFFER_NUM; s++) {
        if (sniffers & (1 << s)) {
            regmap_base_addr = calc_rx_sniffer_base(s);
            err = adi_apollo_hal_bf_set(device, BF_FFT_ENABLE_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return err;
}

int32_t adi_apollo_sniffer_fft_hold_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t hold) {
    int32_t err = API_CMS_ERROR_OK;
    uint32_t regmap_base_addr = 0;
    uint16_t s;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    for (s = 0; s < ADI_APOLLO_SNIFFER_NUM; s++) {
        if (sniffers & (1 << s)) {
            regmap_base_addr = calc_rx_sniffer_base(s);
            err = adi_apollo_hal_bf_set(device, BF_FFT_HOLD_INFO(regmap_base_addr), hold);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return err;
}

int32_t adi_apollo_sniffer_fft_done_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, uint8_t *done) {
    int32_t err = API_CMS_ERROR_OK;
    uint32_t regmap_base_addr = 0;
    uint16_t s;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(sniffers) != 1);
    ADI_APOLLO_NULL_POINTER_RETURN(done);
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    for (s = 0; s < ADI_APOLLO_SNIFFER_NUM; s++) {
        if (sniffers & (1 << s)) {
            regmap_base_addr = calc_rx_sniffer_base(s);
            err = adi_apollo_hal_bf_get(device, BF_FFT_DONE_INFO(regmap_base_addr), done, 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return err;
}

int32_t adi_apollo_sniffer_fft_data_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_param_t *config, adi_apollo_sniffer_fft_data_t *fft_data_params) {
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint16_t side;

    uint8_t max_threshold_tmp;
    uint8_t min_threshold_tmp;

    uint16_t k, j;

    // Check parameters

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(fft_data_params);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(sniffers > ADI_APOLLO_SNIFFER_B);
    ADI_APOLLO_INVALID_PARAM_RETURN(config->init.real_mode && config->pgm.sniffer_mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE); // Real mode has no meaning in IQ mode
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);

    side = (sniffers == ADI_APOLLO_SNIFFER_A) ? 0 : 1;
    regmap_base_addr = calc_rx_sniffer_base(side);

    // Update fft data with meta info

    fft_data_params->data_mode = config->pgm.sniffer_mode;
    fft_data_params->valid_data_length = (config->init.real_mode) ? (ADI_APOLLO_SNIFFER_FFT_LENGTH / 2) : ADI_APOLLO_SNIFFER_FFT_LENGTH;
    fft_data_params->max_threshold = config->init.max_threshold;
    fft_data_params->min_threshold = config->init.min_threshold;
    fft_data_params->fft_is_complex = config->init.real_mode ? 0 : 1;

    // Read data
    for (k = 0; k < fft_data_params->valid_data_length; k++) {
        
        // Read Magnitude / I data
        err = adi_apollo_hal_bf_get(device, BF_MAGNITUDE_I_INFO(regmap_base_addr, k),
                                    (uint8_t *) &fft_data_params->mag_i_data[k], 2);
        ADI_APOLLO_ERROR_RETURN(err);
        
        // Read Bin number / Q data
        if (config->pgm.sniffer_mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE || config->pgm.sort_enable) { // IQ mode or sorting enabled
            err = adi_apollo_hal_bf_get(device, BF_BIN_NUMBER_Q_INFO(regmap_base_addr, k),
                                        (uint8_t *) &fft_data_params->bin_q_data[k], 2);
            ADI_APOLLO_ERROR_RETURN(err);
        } else {
            fft_data_params->bin_q_data[k] = k; // When sorting is disabled, every bin_number returns as 0 so we manually set it in software
        }
    }

    // Read bin threshold values (magnitude mode only)
    if (config->pgm.sniffer_mode <= ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE) { // Mag mode
        for (k = 0; k < fft_data_params->valid_data_length/8; k++) {
            //for reading bins above threshold and below threshold

            err = adi_apollo_hal_bf_get(device, BF_MAX_THRESHOLD_BIN_INFO(regmap_base_addr, k), &max_threshold_tmp, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_MIN_THRESHOLD_BIN_INFO(regmap_base_addr, k), &min_threshold_tmp, 1);
            ADI_APOLLO_ERROR_RETURN(err);
            
            for (j = 0 ; j < 8; j++) {

                fft_data_params->bin_above_threshold[k*8 + j] = ((max_threshold_tmp & (1 << j)) > 0) ? 1 : 0;
                fft_data_params->bin_below_threshold[k*8 + j] = ((min_threshold_tmp & (1 << j)) > 0) ? 1 : 0;
            }
        }

    }

    return err;
}

int32_t adi_apollo_sniffer_data_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t sniffers, adi_apollo_sniffer_param_t *config, adi_apollo_sniffer_fft_data_t *fft_data_params) {
    int32_t err;
    uint8_t fft_done = 0;
    uint64_t fft_timeout; 
    uint8_t do_fft_timeout;

    // Check parameters
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(fft_data_params);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(sniffers > ADI_APOLLO_SNIFFER_B);
    ADI_APOLLO_INVALID_PARAM_RETURN(config->init.real_mode && config->pgm.sniffer_mode > ADI_APOLLO_SNIFFER_INSTANT_MAGNITUDE); // Real mode has no meaning in IQ mode
    ADI_APOLLO_INVALID_PARAM_RETURN(config->init.fft_enable_sel == ADI_APOLLO_SNIFFER_GPIO_INPUT);
    ADI_APOLLO_INVALID_PARAM_RETURN(config->init.fft_hold_sel == ADI_APOLLO_SNIFFER_GPIO_INPUT);
    ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sniffers);


    fft_timeout = config->read.timeout_us ? config->read.timeout_us : DEFAULT_FFT_TIMEOUT_US;
    do_fft_timeout = (config->pgm.sniffer_mode % 2 == 1 || config->read.timeout_us) ? 1 : 0;  // Do timeout for instantaneous mode or if timeout is set

    // Data request sequence

    // Set FFT enable high
    err = adi_apollo_sniffer_fft_enable_set(device, sniffers, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Set FFT hold low
    err = adi_apollo_sniffer_fft_hold_set(device, sniffers, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    // Wait for fft_done to go high
    while(fft_done == 0) {
        err = adi_apollo_sniffer_fft_done_get(device, sniffers, &fft_done);
        ADI_APOLLO_ERROR_RETURN(err);

        if (do_fft_timeout) {
            device->hal_info.delay_us(device, 1);
            fft_timeout--;
            if (! fft_timeout ) {
                return API_CMS_ERROR_OPERATION_TIMEOUT;
            }
        }
    }

    // Disable fft engine
    if (! config->read.run_fft_engine_background) {
        err = adi_apollo_sniffer_fft_enable_set(device, sniffers, 0);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    // Hold fft
    err = adi_apollo_sniffer_fft_hold_set(device, sniffers, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_sniffer_fft_data_get(device, sniffers, config, fft_data_params);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

static uint32_t calc_rx_sniffer_base(int32_t sniffer_index)
{
    static uint32_t sniffer_addr[] = {
        RX_SPECTRUM_SNIFFER_RX_SLICE_0_RX_DIGITAL0, RX_SPECTRUM_SNIFFER_RX_SLICE_0_RX_DIGITAL1
    };

    return sniffer_addr[sniffer_index];
}

static uint32_t calc_rx_datin_base(int32_t sniffer_index)
{
    static uint32_t datin_addr[] = {
        RX_DATIN_RX_SLICE_0_RX_DIGITAL0, RX_DATIN_RX_SLICE_0_RX_DIGITAL1
    };

    return datin_addr[sniffer_index];
}

static uint32_t calc_adc_mux(adi_apollo_adc_idx_e adc)
{
    uint8_t i;

    for (i = 0; i < ADI_APOLLO_ADC_PER_SIDE_NUM; i++) {
        if ((1 << i) & adc) {
            return i;
        }
    }

    return 0;
}

/*! @} */
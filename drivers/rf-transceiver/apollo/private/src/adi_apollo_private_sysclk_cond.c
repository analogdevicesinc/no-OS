/*!
 * \brief:   Apollo private system clock calibration functions
 *
 * \copyright copyright(c) 2023 Analog Devices, Inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated Analog Devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SYSCLK_COND
 * @{
 */

#include "adi_apollo_private_sysclk_cond.h"
#include "adi_apollo_cfg_types.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

int32_t adi_apollo_private_sysclk_cond_defaults_set(adi_apollo_device_t *device, adi_apollo_clk_cfg_t *clk_cfg)
{
    int32_t err;
    /* Initial clock conditioning constants. ***Do not modify*** */
    uint32_t cc_config_address, cc_cal_data_address_a, cc_cal_data_address_b;
    uint32_t cal_data[88] = { 0 };
    uint8_t low_speed = clk_cfg->dev_clk_freq_kHz < 15000000UL;
    adi_apollo_sysclk_cond_calconfig_t calconfig = {
        .adc_centering_capture_length_Kb    = low_speed ? 1 : 8,
        .adc_centering_capture_avg          = low_speed ? 1 : 8,
        .adc_centering_capture_runs         = 1,
        .adc_centering_done                 = 0,
        .adc_centering_use_debubbler        = 1,
        .adc_centering_use_noise_not_spread = 1,
        .adc_use_caldata                    = 0,
        .adc_centering_min_valid_offset     = -26,
        .adc_centering_max_valid_offset     = 14,
        .adc_centering_low_threshold        = 3395, // floor(10^(-155dB/10dB) * 10e9 * 2^30 + 0.5) = 3395
        .adc_centering_high_threshold       = low_speed ? 10737418 : 2142397,
        .adc_centering_precapture_delayus   = 10000,
        .adc_post_correction_delayus        = 0,
    };

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_reg32_get(device, APOLLO_CPU_1_FW_CLK_COND_CONFIG_PTR, &cc_config_address);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_reg32_get(device, APOLLO_CPU_1_FW_CLK_COND_0_CALDATA_PTR, &cc_cal_data_address_a);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_reg32_get(device, APOLLO_CPU_1_FW_CLK_COND_1_CALDATA_PTR, &cc_cal_data_address_b);
    ADI_APOLLO_ERROR_RETURN(err);

    cal_data[0] = (-5 << 16) & 0x00ff0000; // Side A default
    err = adi_apollo_hal_stream_reg32_set(device, cc_cal_data_address_a, cal_data, sizeof(cal_data) / sizeof(cal_data[0]), false);
    ADI_APOLLO_ERROR_RETURN(err);

    cal_data[0] = (-8 << 16) & 0x00ff0000; // Side B default
    err = adi_apollo_hal_stream_reg32_set(device, cc_cal_data_address_b, cal_data, sizeof(cal_data) / sizeof(cal_data[0]), false);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_set(device,
        cc_config_address + 0,
        calconfig.adc_centering_capture_runs << 16 |
        calconfig.adc_centering_capture_avg  << 8  |
        calconfig.adc_centering_capture_length_Kb);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_set(device,
        cc_config_address + 4,
        calconfig.adc_centering_min_valid_offset     << 24 |
        calconfig.adc_use_caldata                    << 16 |
        calconfig.adc_centering_use_noise_not_spread << 8  |
        calconfig.adc_centering_use_debubbler        << 0);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_set(device, cc_config_address + 8, calconfig.adc_centering_max_valid_offset);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_set(device, cc_config_address + 12, calconfig.adc_centering_low_threshold);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_set(device, cc_config_address + 16, calconfig.adc_centering_high_threshold);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_set(device,
        cc_config_address + 20,
        calconfig.adc_post_correction_delayus         << 16  |
        calconfig.adc_centering_precapture_delayus    << 0);

    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

/*! @} */
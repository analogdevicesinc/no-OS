/*!
 * \brief     APIs for ADC
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_ADC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_bf_master_bias_ctrl.h"
#include "adi_apollo_bf_mcs_c_only.h"
#include "adi_apollo_clk_mcs.h"
#include "adi_apollo_private_blk_sel_types.h"
#include "adi_apollo_bf_custom.h"
#include "adi_apollo_bf_venus_fcal_open.h"

#include "adi_apollo_adc.h"
#include "adi_apollo_config.h"
#include "adi_apollo_common.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_mailbox.h"
#include "adi_apollo_cfg.h"
#include "adi_utils.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_4T4R_ADC_TIMEOUT                 60
#define ADI_APOLLO_8T8R_ADC_TIMEOUT                 120
#define ADC_OVR_SAMPLES_MIN                         1
#define ADC_OVR_SAMPLES_MAX                         10000
#define ADC_OVR_CYCLES_MIN                          1
#define ADC_OVR_CYCLES_MAX                          1000

#define ADI_APOLLO_ADC_FAST_DETECT_SETTINGS_STRUCT  2           /*!< [in    ] Set     the Fast Detect Settings (defined by adi_apollo_adc_fast_detect_pgm_t) */
#define ADI_APOLLO_ADC_INPUT_NYQUIST_ZONE_UINT32    6           /*!< [in,out] Set/Get the Input Nyquist Zone (0=disabled, 1=1st, 2=2nd, etc.) */
#define ADI_APOLLO_ADC_FAST_DETECT_STATUS_UINT32    16          /*!< [   out]     Get the Fast Detect Status. */
#define ADI_APOLLO_ADC_DEBUG0_UINT32                19          /*!< [in,out] Set/Get Debug_0 Capability (deliberately vague) */
#define ADI_APOLLO_ADC_CAL_GATING_GROUP_UINT32      20          /*!< [in,out] Set/Get Calibration Group Gating */
#define ADI_APOLLO_ADC_INPUT_STATUS_UINT32          21          /*!< [   out]     Get the Input Condition Status. */
#define ADI_APOLLO_ADC_OVER_RANGE_THRESHOLD_UINT32  22          /*!< [in,out] Set/Get Over Range Protection Threshold */
#define ADI_APOLLO_ADC_OVER_RANGE_SAMPLES_UINT32    23          /*!< [in,out] Set/Get Over Range Protection Number of Samples */

static uint32_t calc_master_bias_base(int32_t index);

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_adc_cal(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs)
{
    int32_t err;
    uint8_t is_adc_nvm_fused = 0;
    adi_apollo_init_cal_cfg_e init_cal_cfg = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    // Check if Apollo has NVM data for ADC CAL
    err = adi_apollo_hal_bf_get(device, BF_ADC_NVM_CALDATA_FUSED_INFO, &is_adc_nvm_fused, 1);
    ADI_CMS_ERROR_RETURN(err);
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_MSG, "is_adc_nvm_fused: 0x%02X", is_adc_nvm_fused);

    // If there is NVM data, start ADC cal with that else start a full ADC cal
    init_cal_cfg = (is_adc_nvm_fused == 1)  ? ADI_APOLLO_INIT_CAL_ENABLED_WARMBOOT_FROM_NVM
                                            : ADI_APOLLO_INIT_CAL_ENABLED;
    err = adi_apollo_adc_init_cal(device, adcs, init_cal_cfg);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_init_cal_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_init_cal_cfg_e init_cal_cfg)
{
    int32_t err;
    adi_apollo_mailbox_cmd_run_init_t run_init_cmd = {0};
    adi_apollo_mailbox_resp_run_init_t run_init_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    /* Init the mailbox command struct */
    run_init_cmd.cal_mask = APOLLO_INIT_CAL_MSK_IC_ADC_RX;
    run_init_cmd.rx_channel_mask = adcs;
    run_init_cmd.tx_channel_mask = 0;
    run_init_cmd.serdes_rx_pack_mask = 0;
    run_init_cmd.serdes_tx_pack_mask = 0;
    run_init_cmd.linearx_chan_mask = 0;


    /* Set ADC Init calibration config. Once enabled they can be started with mailbox commands */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "ADC Init cal config: %d. ref: adi_apollo_init_cal_cfg_e\n", init_cal_cfg);
    if (err = adi_apollo_cfg_adc_init_cal_cfg_set(device, adcs, init_cal_cfg), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_cfg_adc_init_cal_cfg_set() %d", err);
        goto end;
    }

    /* Run the ADC foreground cal */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Starting ADC foreground cal\n");
    if (err = adi_apollo_mailbox_run_init(device, &run_init_cmd, &run_init_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_run_init() %d", err);
        goto end;
    }

    if (run_init_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "ADC Init cal error code = 0x%x\n", run_init_resp.status);
        err = API_CMS_ERROR_MAILBOX_RESP_STATUS;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_adc_init_cal_complete(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs)
{
    int32_t err;
    uint8_t i;
    uint8_t cal_complete = 0;
    uint32_t max_delay_us = 0;
    uint32_t poll_delay_us = 1000000;
    uint32_t delay_us;
    adi_apollo_mailbox_resp_run_init_get_completion_t run_init_complete_resp = {0};
    adi_apollo_mailbox_resp_run_init_get_detailed_status_t run_init_cal_detailed_status_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);


    max_delay_us = (device->dev_info.is_8t8r ? ADI_APOLLO_8T8R_ADC_TIMEOUT : ADI_APOLLO_4T4R_ADC_TIMEOUT) * 1000000;

    /* Wait for ADC Init cal to complete */
    for (delay_us = 0; delay_us < max_delay_us; delay_us += poll_delay_us) {
        if (err = adi_apollo_mailbox_run_init_get_completion(device, &run_init_complete_resp), err != API_CMS_ERROR_OK) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_mailbox_run_init_get_completion() err %d\n");
            goto end;
        }

        if ((delay_us % (5 * poll_delay_us)) == 0) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "ADC Init Cal: status=%d in_progress=%d success=%d  %ds\n",
                                     run_init_complete_resp.status, run_init_complete_resp.in_progress, run_init_complete_resp.success, (delay_us / poll_delay_us));
        }

        if (run_init_complete_resp.in_progress == 0) {
            cal_complete = 1;
            break;
        }

        adi_apollo_hal_delay_us(device, poll_delay_us);
    }

    if (run_init_complete_resp.success != 1) {
        if (err = adi_apollo_mailbox_run_init_get_detailed_status(device, &run_init_cal_detailed_status_resp), err != API_CMS_ERROR_OK) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_run_init_get_detailed_status() %d\n", err);
            goto end;
        }

        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cpu_status = 0x%02X", run_init_cal_detailed_status_resp.status);
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cals_duration_msec = %d", run_init_cal_detailed_status_resp.cals_duration_msec);

        // ADC init cal populates 4 index/channel data for 4T4R and 8 for 8T8R devices
        for (i = 0; i < (device->dev_info.is_8t8r ? 8 : 4); ++i) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "init_err_codes[%d] = 0x%02X \t init_err_cals[%d] = 0x%02X",
                                     i, run_init_cal_detailed_status_resp.init_err_codes[i], i, run_init_cal_detailed_status_resp.init_err_cals[i]);
            // adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cals_since_power_ups[%d] = %d \t cals_last_runs[%d] = %d",
            //                          i, run_init_cal_detailed_status_resp.cals_since_power_up[i], i, run_init_cal_detailed_status_resp.cals_last_run[i]);
        }
    }
    if (cal_complete) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "ADC Init cal completed %s in %ds\n",
                                 run_init_complete_resp.success ? "successfully" : "w/ ERROR", delay_us / 1000000);
        err = run_init_complete_resp.success ? API_CMS_ERROR_OK : API_CMS_ERROR_ADC_INIT_CAL_ERROR;
        goto end;
    } else {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "ADC Init cal timeout after %ds\n", max_delay_us / 1000000);
        err = API_CMS_ERROR_ADC_CAL_TIMEOUT;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_adc_init_cal(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_init_cal_cfg_e init_cal_cfg)
{
    int32_t err;

    err = adi_apollo_adc_init_cal_start(device, adcs, init_cal_cfg);
    ADI_APOLLO_ERROR_RETURN(err);

    return adi_apollo_adc_init_cal_complete(device, adcs);
}

int32_t adi_apollo_adc_tlines_offset_set(adi_apollo_device_t *device, int8_t offset, adi_apollo_side_select_e side_sel)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;

    uint8_t t1_r_pup, t1_rterm_pup_msb, t1_rterm_pup_lsb;
    uint8_t t1_r_pdn, t1_rterm_pdn_msb, t1_rterm_pdn_lsb;
    uint8_t t2_r_pup, t2_rterm_pup_msb, t2_rterm_pup_lsb;
    uint8_t t2_r_pdn, t2_rterm_pdn_msb, t2_rterm_pdn_lsb;
    uint8_t t1_rterm_pup, t1_rterm_pdn, t2_rterm_pup, t2_rterm_pdn;
    uint8_t off_t1_pup, off_t1_pdn, off_t2_pup, off_t2_pdn;
    uint8_t off_t1_rterm_pup, off_t1_rterm_pdn, off_t2_rterm_pup, off_t2_rterm_pdn;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {

            regmap_base_addr = calc_master_bias_base(side_index);

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL1_CLKP_R_PUP_INFO(regmap_base_addr), &t1_r_pup, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL1_CLKP_RTERM_PUP_INFO(regmap_base_addr), &t1_rterm_pup, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            t1_rterm_pup_msb = (t1_rterm_pup >> 1) & 0x1;
            t1_rterm_pup_lsb = t1_rterm_pup & 0x1;

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL1_CLKP_R_PDN_INFO(regmap_base_addr), &t1_r_pdn, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL1_CLKP_RTERM_PDN_INFO(regmap_base_addr), &t1_rterm_pdn, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            t1_rterm_pdn_msb = (t1_rterm_pdn >> 1) & 0x1;
            t1_rterm_pdn_lsb = t1_rterm_pdn & 0x1;

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL2_CLKP_R_PUP_INFO(regmap_base_addr), &t2_r_pup, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL2_CLKP_RTERM_PUP_INFO(regmap_base_addr), &t2_rterm_pup, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            t2_rterm_pup_msb = (t2_rterm_pup >> 1) & 0x1;
            t2_rterm_pup_lsb = t2_rterm_pup & 0x1;

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL2_CLKP_R_PDN_INFO(regmap_base_addr), &t2_r_pdn, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_ADC_TL2_CLKP_RTERM_PDN_INFO(regmap_base_addr), &t2_rterm_pdn, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            t2_rterm_pdn_msb = (t2_rterm_pdn >> 1) & 0x1;
            t2_rterm_pdn_lsb = t2_rterm_pdn & 0x1;

            off_t1_pup = t1_rterm_pup_msb*32 + t1_r_pup;
            off_t1_pdn = t1_rterm_pdn_msb*32 + t1_r_pdn;
            off_t2_pup = t2_rterm_pup_msb*32 + t2_r_pup;
            off_t2_pdn = t2_rterm_pdn_msb*32 + t2_r_pdn;

            off_t1_pup = off_t1_pup + offset;
            off_t1_pdn = off_t1_pdn - offset;
            off_t2_pup = off_t2_pup + offset;
            off_t2_pdn = off_t2_pdn - offset;

            err = adi_apollo_hal_bf_set(device, BF_ADC_TL1_CLKP_R_PUP_INFO(regmap_base_addr), off_t1_pup & 0x1F);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_ADC_TL1_CLKP_R_PDN_INFO(regmap_base_addr), off_t1_pdn & 0x1F);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_ADC_TL2_CLKP_R_PUP_INFO(regmap_base_addr), off_t2_pup & 0x1F);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_ADC_TL2_CLKP_R_PDN_INFO(regmap_base_addr), off_t2_pdn & 0x1F);
            ADI_APOLLO_ERROR_RETURN(err);

            off_t1_rterm_pup = 2 * ((off_t1_pup & 0x20) >> 5) + t1_rterm_pup_lsb;
            err = adi_apollo_hal_bf_set(device, BF_ADC_TL1_CLKP_RTERM_PUP_INFO(regmap_base_addr), off_t1_rterm_pup);
            ADI_APOLLO_ERROR_RETURN(err);

            off_t1_rterm_pdn = 2 * ((off_t1_pdn & 0x20) >> 5) + t1_rterm_pdn_lsb;
            err = adi_apollo_hal_bf_set(device, BF_ADC_TL1_CLKP_RTERM_PDN_INFO(regmap_base_addr), off_t1_rterm_pdn);
            ADI_APOLLO_ERROR_RETURN(err);

            off_t2_rterm_pup = 2 * ((off_t2_pup & 0x20) >> 5) + t2_rterm_pup_lsb;
            err = adi_apollo_hal_bf_set(device, BF_ADC_TL2_CLKP_RTERM_PUP_INFO(regmap_base_addr), off_t2_rterm_pup);
            ADI_APOLLO_ERROR_RETURN(err);

            off_t2_rterm_pdn = 2 * ((off_t2_pdn & 0x20) >> 5) + t2_rterm_pdn_lsb;
            err = adi_apollo_hal_bf_set(device, BF_ADC_TL2_CLKP_RTERM_PDN_INFO(regmap_base_addr), off_t2_rterm_pdn);
            ADI_APOLLO_ERROR_RETURN(err);

        }
    }

    return API_CMS_ERROR_OK;

}

int32_t adi_apollo_adc_nyquist_zone_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs,
                                        uint32_t nyquist_zone)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t adc;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_ADC_INPUT_NYQUIST_ZONE_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = (nyquist_zone & 0xFFU),
        .data_buffer[5]    = (nyquist_zone & 0xFF00U) >> 8,
        .data_buffer[6]    = (nyquist_zone & 0xFF0000U) >> 16,
        .data_buffer[7]    = (nyquist_zone & 0xFF000000U) >> 24,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(nyquist_zone > 2);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);

        if (adc > 0) {
            set_ctrl_cmd.channel_num = i;

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_MAILBOX_RESP_STATUS;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_nyquist_zone_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc,
                                        uint32_t *nyquist_zone)
{
    int32_t err;
    uint16_t i, adc_index;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_ADC_INPUT_NYQUIST_ZONE_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(nyquist_zone);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adc) != 1);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adc);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        if ( (adc & (ADI_APOLLO_ADC_A0 << i)) > 0 ) {
            adc_index = i;
            break;
        }
    }

    set_ctrl_cmd.channel_num = adc_index;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract Nyquist Zone info from response structure */
    *nyquist_zone = (set_ctrl_resp.data_buffer[4+0]) |  (set_ctrl_resp.data_buffer[4+1] << 8) |
                    (set_ctrl_resp.data_buffer[4+2] << 16) | (set_ctrl_resp.data_buffer[4+3] << 24);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_sync_path_delay_set(adi_apollo_device_t *device, adi_apollo_sync_path_delay_e delay_type)
{
    int32_t err;
    uint8_t i;
    uint32_t mbias;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_bf_set(device, BF_C2A_TL1_SYNC_R_INFO, delay_type);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_C2A_TL1_SYNC_C_INFO, delay_type);
    ADI_APOLLO_ERROR_RETURN(err);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        mbias = calc_master_bias_base(i);

        err = adi_apollo_hal_bf_set(device, BF_C2S_SYNC_C_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_ADC_TL1_SYNC_C_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_ADC_TL2_SYNC_C_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_C2B_TL2_SYNC_C_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_S01_SYNC_C_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_S23_SYNC_C_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_C2S_SYNC_R_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_ADC_TL1_SYNC_R_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_ADC_TL2_SYNC_R_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_C2B_TL2_SYNC_R_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_S01_SYNC_R_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_bf_set(device, BF_S23_SYNC_R_INFO(mbias), delay_type);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_bgcal_state_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_adc_bgcal_state_t state[], uint32_t len)
{
    int32_t err = API_CMS_ERROR_OK;
    uint8_t i;
    adi_apollo_blk_sel_t adc;
    adi_apollo_mailbox_resp_get_tracking_cal_state_t track_cal_state_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(state);
    ADI_APOLLO_INVALID_PARAM_RETURN(len < ADI_APOLLO_ADC_NUM);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    if (err = adi_apollo_mailbox_get_tracking_cal_state(device, &track_cal_state_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_get_tracking_cal_state() %d", err);
        goto end;
    }

    if (track_cal_state_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "ADC BG cal state error code = 0x%x\n", track_cal_state_resp.status);
        err = API_CMS_ERROR_MAILBOX_RESP_STATUS;
        goto end;
    }

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);
        if (adc > 0) {
            state[i].state_valid = 1;
            state[i].bgcal_error = track_cal_state_resp.tracking_cal_state.adc_rx_cal_error[i];
            state[i].bgcal_state = track_cal_state_resp.tracking_cal_state.adc_rx_cal_state[i];
        } else {
            state[i].state_valid = 0;
            state[i].bgcal_error = track_cal_state_resp.tracking_cal_state.adc_rx_cal_error[i];
            state[i].bgcal_state = track_cal_state_resp.tracking_cal_state.adc_rx_cal_state[i];
        }
    }

end:
    return err;
}

int32_t adi_apollo_adc_bgcal_freeze(adi_apollo_device_t* device, adi_apollo_blk_sel_t adcs)
{
    int32_t err = API_CMS_ERROR_OK;
    adi_apollo_mailbox_cmd_set_enabled_tracking_cals_t track_cal_cmd = {0};
    adi_apollo_mailbox_resp_set_enabled_tracking_cals_t track_cal_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    /* Init the mailbox command struct */
    track_cal_cmd.adc_rx_channel_mask = adcs;
    track_cal_cmd.enable_disable = 0;
    track_cal_cmd.serdes_rx_pack_mask = 0;
    track_cal_cmd.mcs_tc_mask = 0;

    /*
    * Freeze the background cals
    */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Freezing ADC background tracking cals\n");


    if (err = adi_apollo_mailbox_set_enabled_tracking_cals(device, &track_cal_cmd, &track_cal_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_enabled_tracking_cals() (freeze) %d\n", err);
        goto end;
    }

    if (track_cal_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "ADC BG cal error code = 0x%x\n", track_cal_resp.status);
        err = API_CMS_ERROR_ADC_TRACK_CAL_ERROR;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_adc_bgcal_unfreeze(adi_apollo_device_t* device, adi_apollo_blk_sel_t adcs)
{
    int32_t err = API_CMS_ERROR_OK;
    adi_apollo_mailbox_cmd_set_enabled_tracking_cals_t track_cal_cmd = {0};
    adi_apollo_mailbox_resp_set_enabled_tracking_cals_t track_cal_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    /* Init the mailbox command struct */
    track_cal_cmd.adc_rx_channel_mask = adcs;
    track_cal_cmd.enable_disable = 1;
    track_cal_cmd.serdes_rx_pack_mask = 0;
    track_cal_cmd.mcs_tc_mask = 0;
    /*
    * Unfreeze the background cals
    */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Unfreeze ADC background tracking cals\n");
    if (err = adi_apollo_mailbox_set_enabled_tracking_cals(device, &track_cal_cmd, &track_cal_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_enabled_tracking_cals() (unfreeze) %d\n", err);
        goto end;
    }

    if (track_cal_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "ADC BG cal error code = 0x%x\n", track_cal_resp.status);
        err = API_CMS_ERROR_ADC_TRACK_CAL_ERROR;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_adc_mode_switch_enable_set(adi_apollo_device_t *device, uint8_t enable)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_reg_set(device, REG_ADC_SLICE_MODE_SWITCH_ENABLE_ADDR, enable);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_adc_mode_switch_prepare(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs)
{
    int32_t err = API_CMS_ERROR_OK;
    uint8_t trig_status;

    adi_apollo_mailbox_resp_set_adc_slice_mode_fast_switch_action_t resp;
    adi_apollo_mailbox_cmd_set_adc_slice_mode_fast_switch_action_t cmd;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    /* Init the mailbox command */
    cmd.adc_rx_channel_mask = adcs;
    cmd.adc_slice_mode_fast_switch_action = APOLLO_ADC_PREP_FAST_MODE_SWITCH;

    err = adi_apollo_clk_mcs_trig_sync_enable_get(device, &trig_status);
    ADI_APOLLO_ERROR_RETURN(err);
    ADI_CMS_CHECK(trig_status != 0, API_CMS_ERROR_BAD_STATE);

    // Abort all background calibrations.
    err = adi_apollo_adc_bgcal_freeze(device, adcs);
    ADI_APOLLO_ERROR_RETURN(err);

    // Wait 10ms.
    err = adi_apollo_hal_delay_us(device, 10000);
    ADI_APOLLO_ERROR_RETURN(err);

    // Set the ADC Channel Mask, where, Reg bits[7:0] corresponds to [B3 B2 B1 B0 A3 A2 A1 A0].
    err = adi_apollo_hal_reg_set(device, REG_ADC_SLICE_MODE_SWITCH_CHANNEL_MASK_ADDR, adcs);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_mailbox_set_adc_slice_mode_fast_switch_action(device, &cmd, &resp);
    ADI_APOLLO_ERROR_RETURN(err);
    if (resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_adc_slice_mode_fast_switch_action() 0x%X", resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    return err;
}

int32_t adi_apollo_adc_mode_switch_execute(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_adc_mode_switch_method_e method)
{
    int32_t err = API_CMS_ERROR_OK;

    adi_apollo_mailbox_resp_set_adc_slice_mode_fast_switch_action_t resp;
    adi_apollo_mailbox_cmd_set_adc_slice_mode_fast_switch_action_t cmd;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(method > ADI_APOLLO_ADC_MODE_SWITCH_BY_REGMAP);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    /* Init the mailbox command */
    cmd.adc_rx_channel_mask = adcs;
    cmd.adc_slice_mode_fast_switch_action = APOLLO_ADC_EXEC_FAST_MODE_SWITCH;

    if (method == ADI_APOLLO_ADC_MODE_SWITCH_BY_COMMAND) {
        err = adi_apollo_mailbox_set_adc_slice_mode_fast_switch_action(device, &cmd, &resp);
        ADI_APOLLO_ERROR_RETURN(err);
        if (resp.status != API_CMS_ERROR_OK) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_adc_slice_mode_fast_switch_action() 0x%X", resp.status);
            return API_CMS_ERROR_MAILBOX_RESP_STATUS;
        }
    } else {
        // Trigger the slice mode switch for above selected ADC channels.
        err = adi_apollo_hal_reg_set(device, REG_ADC_SLICE_MODE_SWITCH_TRIGGER_ADDR, 1);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return err;
}

int32_t adi_apollo_adc_mode_switch_restore(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint8_t enable_bgcal)
{
    int32_t err = API_CMS_ERROR_OK;

    adi_apollo_mailbox_resp_set_adc_slice_mode_fast_switch_action_t resp;
    adi_apollo_mailbox_cmd_set_adc_slice_mode_fast_switch_action_t cmd;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    /* Init the mailbox command */
    cmd.adc_rx_channel_mask = adcs;
    cmd.adc_slice_mode_fast_switch_action = APOLLO_ADC_RESUME_FAST_MODE_SWITCH;

    err = adi_apollo_mailbox_set_adc_slice_mode_fast_switch_action(device, &cmd, &resp);
    ADI_APOLLO_ERROR_RETURN(err);
    if (resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_adc_slice_mode_fast_switch_action() 0x%X", resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    if (enable_bgcal) {
        // Restart all background calibrations.
        err = adi_apollo_adc_bgcal_unfreeze(device, adcs);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return err;
}

int32_t adi_apollo_adc_fast_detect_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_adc_fast_detect_pgm_t *config)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {0};
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t adc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(config->lower_threshold > config->upper_threshold)
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    set_ctrl_cmd.sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX;
    set_ctrl_cmd.ctrl_cmd          = CTRL_CMD_PARAM_SET;
    set_ctrl_cmd.data_buffer[0]    = ADI_APOLLO_ADC_FAST_DETECT_SETTINGS_STRUCT;
    set_ctrl_cmd.data_buffer[4]    = config->enable;
    set_ctrl_cmd.data_buffer[8]    = (config->upper_threshold) & 0xFFU;
    set_ctrl_cmd.data_buffer[9]    = (config->upper_threshold >> 8) & 0xFFU;
    set_ctrl_cmd.data_buffer[10]   = (config->lower_threshold) & 0xFFU;
    set_ctrl_cmd.data_buffer[11]   = (config->lower_threshold >> 8) & 0xFFU;
    set_ctrl_cmd.data_buffer[12]   = (config->dwell_cycles) & 0xFFU;
    set_ctrl_cmd.data_buffer[13]   = (config->dwell_cycles >> 8) & 0xFFU;
    set_ctrl_cmd.data_buffer[14]   = (config->dwell_cycles >> 16) & 0xFFU;
    set_ctrl_cmd.data_buffer[15]   = (config->dwell_cycles >> 24) & 0xFFU;
    set_ctrl_cmd.length            = 16;

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);

        if (adc > 0) {

            set_ctrl_cmd.channel_num = i;

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_MAILBOX_RESP_STATUS;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_fast_detect_status_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint8_t *fast_detect)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {0};
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};
    int32_t err;
    uint32_t fast_detect_status;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fast_detect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adc) != 1);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adc);

    set_ctrl_cmd.sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX;
    set_ctrl_cmd.ctrl_cmd          = CTRL_CMD_PARAM_GET;
    set_ctrl_cmd.data_buffer[0]    = ADI_APOLLO_ADC_FAST_DETECT_STATUS_UINT32;
    set_ctrl_cmd.length            = 4;
    set_ctrl_cmd.channel_num       = adi_api_utils_select_lsb_get(adc);

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract fast detect info from response structure */
    fast_detect_status = (set_ctrl_resp.data_buffer[4+0]) |  (set_ctrl_resp.data_buffer[4+1] << 8) |
                         (set_ctrl_resp.data_buffer[4+2] << 16) | (set_ctrl_resp.data_buffer[4+3] << 24);

    *fast_detect = (fast_detect_status == 0) ? 0 : 1;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_status_get(adi_apollo_device_t *device, adi_apollo_adc_status_t *status)
{
    int32_t err = API_CMS_ERROR_OK;
    adi_apollo_mailbox_resp_run_init_get_detailed_status_t init_response = {0};
    adi_apollo_mailbox_resp_get_enabled_tracking_cals_t enabled_cals = {0};
    adi_apollo_mailbox_resp_get_tracking_cal_state_t cal_state = {0};
    adi_apollo_mailbox_resp_get_adc_slice_modes_t slice_modes = {0};
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);

    err = adi_apollo_mailbox_run_init_get_detailed_status(device, &init_response);
    ADI_APOLLO_ERROR_RETURN(err);
    if (init_response.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_run_init_get_detailed_status() 0x%X", init_response.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    err = adi_apollo_mailbox_get_enabled_tracking_cals(device, &enabled_cals);
    ADI_APOLLO_ERROR_RETURN(err);
    if (enabled_cals.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_get_enabled_tracking_cals() 0x%X", enabled_cals.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    err = adi_apollo_mailbox_get_tracking_cal_state(device, &cal_state);
    ADI_APOLLO_ERROR_RETURN(err);
    if (cal_state.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_get_tracking_cal_state() 0x%X", cal_state.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    err = adi_apollo_mailbox_get_adc_slice_modes(device, &slice_modes);
    ADI_APOLLO_ERROR_RETURN(err);
    if (slice_modes.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_get_adc_slice_modes() 0x%X", slice_modes.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    status->fg_cal.duration_ms = init_response.cals_duration_msec;
    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        status->fg_cal.last_run[i] = ((init_response.cals_last_run[i] & APOLLO_INIT_CAL_MSK_IC_ADC_RX) > 0) ? 1 : 0;
        status->fg_cal.since_power_up[i] = ((init_response.cals_since_power_up[i] & APOLLO_INIT_CAL_MSK_IC_ADC_RX) > 0) ? 1 : 0;
        status->fg_cal.err_status[i] = ((init_response.init_err_cals[i] & APOLLO_INIT_CAL_MSK_IC_ADC_RX) > 0) ? 1 : 0;

        status->bg_cal.enabled[i] = (enabled_cals.tracking_cal_enable_masks.adc_rx_enable_masks & (1 << i)) ? 1 : 0;
        status->bg_cal.error[i] = cal_state.tracking_cal_state.adc_rx_cal_error[i];
        status->bg_cal.state[i] = cal_state.tracking_cal_state.adc_rx_cal_state[i];

        status->mode[i] = slice_modes.adc_slice_mode[i];
    }

    return err;
}

int32_t adi_apollo_adc_bg_cal_grp_gate_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t cal_gates)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t adc;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_ADC_CAL_GATING_GROUP_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = (cal_gates & 0xFFU),
        .data_buffer[5]    = (cal_gates & 0xFF00U) >> 8,
        .data_buffer[6]    = (cal_gates & 0xFF0000U) >> 16,
        .data_buffer[7]    = (cal_gates & 0xFF000000U) >> 24,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);

        if (adc > 0) {
            set_ctrl_cmd.channel_num = i;

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_MAILBOX_RESP_STATUS;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_bg_cal_grp_gate_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *cal_gates)
{
    int32_t err;
    uint16_t i, adc_index;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_ADC_CAL_GATING_GROUP_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_gates);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adc) != 1);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adc);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        if ( (adc & (ADI_APOLLO_ADC_A0 << i)) > 0 ) {
            adc_index = i;
            break;
        }
    }

    set_ctrl_cmd.channel_num = adc_index;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract ADC BG Calibration gating info from response structure */
    *cal_gates = (set_ctrl_resp.data_buffer[4+0]) |  (set_ctrl_resp.data_buffer[4+1] << 8) |
                 (set_ctrl_resp.data_buffer[4+2] << 16) | (set_ctrl_resp.data_buffer[4+3] << 24);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_debug0_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t debug_val)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t adc;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_ADC_DEBUG0_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = (debug_val & 0xFFU),
        .data_buffer[5]    = (debug_val & 0xFF00U) >> 8,
        .data_buffer[6]    = (debug_val & 0xFF0000U) >> 16,
        .data_buffer[7]    = (debug_val & 0xFF000000U) >> 24,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);

        if (adc > 0) {
            set_ctrl_cmd.channel_num = i;

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_MAILBOX_RESP_STATUS;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_debug0_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *debug_val)
{
    int32_t err;
    uint16_t i, adc_index;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_ADC_DEBUG0_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(debug_val);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adc) != 1);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adc);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        if ( (adc & (ADI_APOLLO_ADC_A0 << i)) > 0 ) {
            adc_index = i;
            break;
        }
    }

    set_ctrl_cmd.channel_num = adc_index;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract ADC BG Calibration gating info from response structure */
    *debug_val = (set_ctrl_resp.data_buffer[4+0]) |  (set_ctrl_resp.data_buffer[4+1] << 8) |
                 (set_ctrl_resp.data_buffer[4+2] << 16) | (set_ctrl_resp.data_buffer[4+3] << 24);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_input_status_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, adi_apollo_adc_input_status_t *input_status)
{
    int32_t err;
    uint16_t i, adc_index;
    uint32_t status_readback = 0;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_ADC_INPUT_STATUS_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(input_status);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adc) != 1);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adc);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        if ( (adc & (ADI_APOLLO_ADC_A0 << i)) > 0 ) {
            adc_index = i;
            break;
        }
    }

    set_ctrl_cmd.channel_num = adc_index;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract ADC input status from response structure */
    status_readback = (set_ctrl_resp.data_buffer[4+0]) |  (set_ctrl_resp.data_buffer[4+1] << 8) |
                      (set_ctrl_resp.data_buffer[4+2] << 16) | (set_ctrl_resp.data_buffer[4+3] << 24);

    input_status->power = (status_readback >> 0) & 0x01;
    input_status->amp_diversity = (status_readback >> 1) & 0x01;
    input_status->over_range = (status_readback >> 2) & 0x01;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_ovr_threshold_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t ovr_threshold)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t adc;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_ADC_OVER_RANGE_THRESHOLD_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = (ovr_threshold & 0xFFU),
        .data_buffer[5]    = (ovr_threshold & 0xFF00U) >> 8,
        .data_buffer[6]    = (ovr_threshold & 0xFF0000U) >> 16,
        .data_buffer[7]    = (ovr_threshold & 0xFF000000U) >> 24,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_RANGE_CHECK(ovr_threshold, ADI_APOLLO_ADC_OVR_THRESH_M0P5_DB, ADI_APOLLO_ADC_OVR_THRESH_M4P0_DB);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);

        if (adc > 0) {
            set_ctrl_cmd.channel_num = i;

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_MAILBOX_RESP_STATUS;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_ovr_threshold_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *ovr_threshold)
{
    int32_t err;
    uint16_t i, adc_index;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_ADC_OVER_RANGE_THRESHOLD_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(ovr_threshold);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adc) != 1);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adc);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        if ( (adc & (ADI_APOLLO_ADC_A0 << i)) > 0 ) {
            adc_index = i;
            break;
        }
    }

    set_ctrl_cmd.channel_num = adc_index;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract Over-Range Threshold info from response structure */
    *ovr_threshold = (set_ctrl_resp.data_buffer[4+0]) |  (set_ctrl_resp.data_buffer[4+1] << 8) |
                     (set_ctrl_resp.data_buffer[4+2] << 16) | (set_ctrl_resp.data_buffer[4+3] << 24);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_ovr_samples_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t num_samples)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t adc;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_ADC_OVER_RANGE_SAMPLES_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = (num_samples & 0xFFU),
        .data_buffer[5]    = (num_samples & 0xFF00U) >> 8,
        .data_buffer[6]    = (num_samples & 0xFF0000U) >> 16,
        .data_buffer[7]    = (num_samples & 0xFF000000U) >> 24,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_RANGE_CHECK(num_samples, ADC_OVR_SAMPLES_MIN, ADC_OVR_SAMPLES_MAX);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adcs);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);

        if (adc > 0) {
            set_ctrl_cmd.channel_num = i;

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_MAILBOX_RESP_STATUS;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_adc_ovr_samples_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *num_samples)
{
    int32_t err;
    uint16_t i, adc_index;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_ADC_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_ADC_OVER_RANGE_SAMPLES_UINT32,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(num_samples);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adc) != 1);
    ADI_APOLLO_ADC_BLK_SEL_MASK(adc);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        if ( (adc & (ADI_APOLLO_ADC_A0 << i)) > 0 ) {
            adc_index = i;
            break;
        }
    }

    set_ctrl_cmd.channel_num = adc_index;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract Num of samples at or above over-range threshold from response structure */
    *num_samples = (set_ctrl_resp.data_buffer[4+0]) |  (set_ctrl_resp.data_buffer[4+1] << 8) |
                   (set_ctrl_resp.data_buffer[4+2] << 16) | (set_ctrl_resp.data_buffer[4+3] << 24);

    return API_CMS_ERROR_OK;
}



static uint32_t calc_master_bias_base(int32_t index)
{
    static uint32_t master_bias_base_addr[] = {
        MBIAS0, MBIAS1
    };

    return master_bias_base_addr[index];
}

/*! @} */

/*!
 * \brief     APIs for Multi Chip Sync Calibration
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_MCS_CAL
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_mcs_cal.h"
#include "adi_apollo_mailbox.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_mcs_cal_config_set(adi_apollo_device_t *device, adi_apollo_mcs_cal_config_t *cal_config)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t config_size = sizeof(adi_apollo_mcs_cal_config_t);
    uint8_t expected_config_size = 96;      // Expected byte size for adi_apollo_mcs_cal_config_t
    uint8_t mcs_parameter_len = 4;          // sub command like mcs_parameter has its length set to 4 bytes in set_ctrl_cmd.data_buffer[0:3].

    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {0};
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_config);

    if (config_size != expected_config_size) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Cal Config struct size does not match! Expected: %d Got: %d.\n", expected_config_size, config_size);
        return API_CMS_ERROR_INVALID_PARAM;
    }

    set_ctrl_cmd.sys_cal_object_id = APOLLO_CPU_OBJID_TC_MCS;
    set_ctrl_cmd.ctrl_cmd          = CTRL_CMD_PARAM_SET;
    set_ctrl_cmd.channel_num       = 0;   // For MCS config channel num isn't used, so set to 0.
    set_ctrl_cmd.length            = expected_config_size + mcs_parameter_len;

    memset(set_ctrl_cmd.data_buffer, 0, set_ctrl_cmd.length); // Clear data buffer
    set_ctrl_cmd.data_buffer[0] = MCS_PARAMS_ALL;             // \ref adi_apollo_mcs_parameter_e. 4 bytes for mcs_parameter.
    set_ctrl_cmd.data_buffer[1] = 0;
    set_ctrl_cmd.data_buffer[2] = 0;
    set_ctrl_cmd.data_buffer[3] = 0;

    // Copy the cal_config struct data into the data_buffer
    memcpy(&set_ctrl_cmd.data_buffer[4], cal_config, config_size);

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    ADI_APOLLO_ERROR_RETURN(err);

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_init_run(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t i = 0;
    adi_apollo_mailbox_cmd_run_init_t run_init_cmd = {0};
    adi_apollo_mailbox_resp_run_init_t run_init_resp = {0};
    adi_apollo_mailbox_resp_run_init_get_completion_t run_init_complete_resp = {0};
    adi_apollo_mailbox_resp_run_init_get_detailed_status_t run_init_cal_detailed_status_resp = {0};
    uint32_t max_delay_us = 60 * 1000000;
    uint32_t poll_delay_us = 1000000;
    uint32_t delay_us;
    uint8_t cal_complete = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    run_init_cmd.cal_mask = APOLLO_INIT_CAL_MSK_IC_MCS;
    run_init_cmd.rx_channel_mask = 0;
    run_init_cmd.tx_channel_mask = 0;
    run_init_cmd.serdes_rx_pack_mask = 0;
    run_init_cmd.serdes_tx_pack_mask = 0;
    run_init_cmd.linearx_chan_mask = 0;

    // Run the MCS init cal
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Starting MCS init cal\n");
    if (err = adi_apollo_mailbox_run_init(device, &run_init_cmd, &run_init_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_run_init() %d.\n", err);
        goto end;
    }

    if (run_init_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "MCS Init cal error code = %d.\n", run_init_resp.status);
        err = API_CMS_ERROR_MAILBOX_RESP_STATUS;
        goto end;
    }

    // Wait for init cal to complete
    for (delay_us = 0; delay_us < max_delay_us; delay_us += poll_delay_us) {
        if (err = adi_apollo_mailbox_run_init_get_completion(device, &run_init_complete_resp), err != API_CMS_ERROR_OK) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_run_init_get_completion() %d.\n", err);
            goto end;
        }

        if ((delay_us % (5 * poll_delay_us)) == 0) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Init Cal: status=%d in_progress=%d success=%d  %ds.\n",
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
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_run_init_get_detailed_status() %d.\n", err);
            goto end;
        }

        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cpu_status = 0x%02X", run_init_cal_detailed_status_resp.status);
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cals_duration_msec = %d", run_init_cal_detailed_status_resp.cals_duration_msec);

        // MCS init cal populates 1 index/channel data
        for (i = 0; i < 1; ++i) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "init_err_codes[%d] = 0x%02X \t init_err_cals[%d] = 0x%02X",
                                     i, run_init_cal_detailed_status_resp.init_err_codes[i], i, run_init_cal_detailed_status_resp.init_err_cals[i]);
            // adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cals_since_power_ups[%d] = %d \t cals_last_runs[%d] = %d",
            //                          i, run_init_cal_detailed_status_resp.cals_since_power_up[i], i, run_init_cal_detailed_status_resp.cals_last_run[i]);
        }
    }

    if (cal_complete) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Init cal completed %s in %ds\n",
                                 run_init_complete_resp.success ? "successfully" : "w/ ERROR", delay_us / 1000000);
        err = run_init_complete_resp.success ? API_CMS_ERROR_OK : API_CMS_ERROR_MCS_INIT_CAL_ERROR;
        goto end;
    } else {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "MCS Init cal timeout after %ds\n", max_delay_us / 1000000);
        err = API_CMS_ERROR_MCS_CAL_TIMEOUT;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_mcs_cal_init_status_get(adi_apollo_device_t *device, adi_apollo_mcs_cal_init_status_t *cal_status)
{
    int32_t err = API_CMS_ERROR_ERROR;
    adi_apollo_mailbox_cmd_get_cal_status_t cal_status_cmd = {0};
    adi_apollo_mailbox_resp_get_cal_status_t cal_status_resp = {0};
    uint8_t cal_status_size = sizeof(adi_apollo_mcs_cal_init_status_t);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_status);

    // CAL_STATUS_SPECIFIC will let apollo_fw know that we want all of MCC FW CalData. Not just error codes.
    cal_status_cmd.cal_status_type = CAL_STATUS_SPECIFIC;

    // MCS Cal type: init cal.
    cal_status_cmd.sys_cal_object_id = APOLLO_CPU_OBJID_IC_MCS;

    // For MCS config channel num isn't used, so set to 0.
    cal_status_cmd.channel_num = 0;

    if (err = adi_apollo_mailbox_get_cal_status(device, &cal_status_cmd, &cal_status_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_get_cal_status() %d.\n", err);
        goto end;
    }

    // Check if the cal status response matches the size of adi_apollo_mcs_cal_init_status_t struct.
    if (cal_status_size != cal_status_resp.length) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Init Cal Status Data buffer size does not match! Expected: %d Got: %d.\n", cal_status_size, cal_status_resp.length);
        err = API_CMS_ERROR_MAILBOX_RESP_STATUS;
        goto end;
    }

    memcpy(cal_status, cal_status_resp.data_buffer, cal_status_resp.length);

end:
    return err;
}

int32_t adi_apollo_mcs_cal_parameter_set(adi_apollo_device_t *device, adi_apollo_mcs_parameter_e mcs_parameter, uint64_t data)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t i = 0;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {0};
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};
    uint8_t mcs_parameter_len = 4;      // sub command like mcs_parameter has its length set to 4 bytes in set_ctrl_cmd.data_buffer[0:3].
    uint8_t parameter_data_len = 8;     // 8 bytes for mcs_parameter data in set_ctrl_cmd.data_buffer[4:11].

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    set_ctrl_cmd.sys_cal_object_id = APOLLO_CPU_OBJID_TC_MCS;
    set_ctrl_cmd.ctrl_cmd          = CTRL_CMD_PARAM_SET;
    set_ctrl_cmd.channel_num       = 0;     // For MCS config channel num isn't used, so set to 0.
    set_ctrl_cmd.length            = mcs_parameter_len + parameter_data_len;

    memset(set_ctrl_cmd.data_buffer, 0, set_ctrl_cmd.length); // Clear data buffer
    set_ctrl_cmd.data_buffer[0] = mcs_parameter;              // \ref adi_apollo_mcs_parameter_e
    set_ctrl_cmd.data_buffer[1] = 0;
    set_ctrl_cmd.data_buffer[2] = 0;
    set_ctrl_cmd.data_buffer[3] = 0;

    for (i = 0; i < parameter_data_len; ++i) {
        set_ctrl_cmd.data_buffer[4 + i] = (data >> (8 * i)) & 0xFF;
    }

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    ADI_APOLLO_ERROR_RETURN(err);

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_tracking_decimation_set(adi_apollo_device_t *device, uint16_t decimation)
{
    int32_t err = API_CMS_ERROR_ERROR;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_MEASUREMENT_DECIMATION_RATE_UINT16, decimation);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_tracking_enable(adi_apollo_device_t *device, uint8_t enable)
{
    int32_t err = API_CMS_ERROR_ERROR;
    adi_apollo_mailbox_cmd_set_enabled_tracking_cals_t cmd = {0};
    adi_apollo_mailbox_resp_set_enabled_tracking_cals_t resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(enable > 1);

    cmd.serdes_rx_pack_mask = 0; // For MCS tracking, serdes mask isn't used, so set to 0.
    cmd.adc_rx_channel_mask = 0; // For MCS tracking, channel mask isn't used, so set to 0.
    cmd.mcs_tc_mask = 1;
    cmd.enable_disable = enable;

    err = adi_apollo_mailbox_set_enabled_tracking_cals(device, &cmd, &resp);
    ADI_APOLLO_ERROR_RETURN(err);

    if (resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_enabled_tracking_cals() 0x%X", resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_tracking_initialize_set(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_INITIALIZE_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_fg_tracking_run(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t i = 0;
    adi_apollo_mcs_cal_status_t tracking_cal_status = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_FOREGROUND_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Wait for fg tracking cal to complete
    for (i = 0; i < 60; ++i) {
        err = adi_apollo_mcs_cal_tracking_status_get(device, &tracking_cal_status);
        ADI_APOLLO_ERROR_RETURN(err);

        if ((i > 0) && ((i % 5) == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS FG Tracking Cal: foreground_done = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.foreground_done, i);
        }

        if ((tracking_cal_status.mcs_tracking_cal_status.foreground_done == 1) && (tracking_cal_status.hdr.errorCode == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS FG Tracking Cal: foreground_done = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.foreground_done, i);
            break;
        }

        adi_apollo_hal_delay_us(device, 1 * 1000000);
    }

    if (tracking_cal_status.mcs_tracking_cal_status.foreground_done != 1) {
        return API_CMS_ERROR_MCS_CAL_TIMEOUT;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_bg_tracking_run(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_BACKGROUND_0_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_bg_tracking_freeze(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t i = 0;
    adi_apollo_mcs_cal_status_t tracking_cal_status = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_HALT_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Wait for bg tracking cal to halt
    for (i = 0; i < 60; ++i) {
        err = adi_apollo_mcs_cal_tracking_status_get(device, &tracking_cal_status);
        ADI_APOLLO_ERROR_RETURN(err);

        if ((i > 0) && ((i % 5) == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS BG Tracking Cal: halt_active = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.halt_active, i);
        }

        if ((tracking_cal_status.mcs_tracking_cal_status.halt_active == 1) && (tracking_cal_status.hdr.errorCode == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS BG Tracking Cal: halt_active = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.halt_active, i);
            break;
        }

        adi_apollo_hal_delay_us(device, 1 * 1000000);
    }

    if (tracking_cal_status.mcs_tracking_cal_status.halt_active != 1) {
        return API_CMS_ERROR_MCS_CAL_TIMEOUT;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_bg_tracking_unfreeze(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_HALT_UINT8, 0);
    ADI_APOLLO_ERROR_RETURN(err);


    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_bg_tracking_abort(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t i = 0;
    adi_apollo_mcs_cal_status_t tracking_cal_status = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_ABORT_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Wait for bg tracking cal to halt
    for (i = 0; i < 60; ++i) {
        err = adi_apollo_mcs_cal_tracking_status_get(device, &tracking_cal_status);
        ADI_APOLLO_ERROR_RETURN(err);

        if ((i > 0) && ((i % 5) == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS BG Tracking Cal: abort_done = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.abort_done, i);
        }

        if ((tracking_cal_status.mcs_tracking_cal_status.abort_done == 1) && (tracking_cal_status.hdr.errorCode == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS BG Tracking Cal: abort_done = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.abort_done, i);
            break;
        }

        adi_apollo_hal_delay_us(device, 1 * 1000000);
    }

    if (tracking_cal_status.mcs_tracking_cal_status.abort_done != 1) {
        return API_CMS_ERROR_MCS_CAL_TIMEOUT;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_force_fg_tracking_run(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t i = 0;
    adi_apollo_mcs_cal_status_t tracking_cal_status = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_FORCE_FOREGROUND_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Wait for fg tracking cal to complete
    for (i = 0; i < 60; ++i) {
        err = adi_apollo_mcs_cal_tracking_status_get(device, &tracking_cal_status);
        ADI_APOLLO_ERROR_RETURN(err);

        if ((i > 0) && ((i % 5) == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Forced FG Tracking Cal: foreground_done = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.foreground_done, i);
        }

        if ((tracking_cal_status.mcs_tracking_cal_status.foreground_done == 1) && (tracking_cal_status.hdr.errorCode == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Forced FG Tracking Cal: foreground_done = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.foreground_done, i);
            break;
        }

        adi_apollo_hal_delay_us(device, 1 * 1000000);
    }

    if (tracking_cal_status.mcs_tracking_cal_status.foreground_done != 1) {
        return API_CMS_ERROR_MCS_CAL_TIMEOUT;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_force_bg_tracking_run(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint8_t i = 0;
    adi_apollo_mcs_cal_status_t tracking_cal_status = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_FORCE_BACKGROUND_STEP_0_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Wait for fg tracking cal to complete
    for (i = 0; i < 60; ++i) {
        err = adi_apollo_mcs_cal_tracking_status_get(device, &tracking_cal_status);
        ADI_APOLLO_ERROR_RETURN(err);

        if ((i > 0) && ((i % 5) == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Forced BG Tracking Cal: force_background_done[0] = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.force_background_done[0], i);
        }

        if ((tracking_cal_status.mcs_tracking_cal_status.force_background_done[0] == 1) && (tracking_cal_status.hdr.errorCode == 0)) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Forced BG Tracking Cal: force_background_done[0] = %d. %dsec.\n",
                                     tracking_cal_status.mcs_tracking_cal_status.force_background_done[0], i);
            break;
        }

        adi_apollo_hal_delay_us(device, 1 * 1000000);
    }

    if (tracking_cal_status.mcs_tracking_cal_status.force_background_done[0] != 1) {
        return API_CMS_ERROR_MCS_CAL_TIMEOUT;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_coarse_jump_set(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mcs_cal_parameter_set(device, MCS_TRACK_COARSE_JUMP_0_UINT8, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mcs_cal_tracking_status_get(adi_apollo_device_t *device, adi_apollo_mcs_cal_status_t *cal_status)
{
    int32_t err = API_CMS_ERROR_ERROR;
    adi_apollo_mailbox_cmd_get_cal_status_t cal_status_cmd = {0};
    adi_apollo_mailbox_resp_get_cal_status_t cal_status_resp = {0};
    uint8_t cal_status_size = sizeof(adi_apollo_mcs_cal_status_t);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_status);

    // CAL_STATUS_SPECIFIC will let apollo_fw know that we want all of MCC FW CalData. Not just error codes.
    cal_status_cmd.cal_status_type = CAL_STATUS_SPECIFIC;

    // MCS Cal type: tracking cal.
    cal_status_cmd.sys_cal_object_id = APOLLO_CPU_OBJID_TC_MCS;

    // For MCS config channel num isn't used, so set to 0.
    cal_status_cmd.channel_num = 0;

    if (err = adi_apollo_mailbox_get_cal_status(device, &cal_status_cmd, &cal_status_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_get_cal_status() %d.\n", err);
        goto end;
    }

    // Check if the cal status response matches the size of adi_apollo_mcs_cal_status_t struct.
    if (cal_status_size != cal_status_resp.length) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "MCS Tracking Cal Status Data buffer size does not match! Expected: %d Got: %d.\n", cal_status_size, cal_status_resp.length);
        err = API_CMS_ERROR_MCS_TRACK_CAL_ERROR;
        goto end;
    }

    memcpy(cal_status, cal_status_resp.data_buffer, cal_status_resp.length);

end:
    return err;
}

/*! @} */
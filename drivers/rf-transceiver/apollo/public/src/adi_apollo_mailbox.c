/*!
 * \brief     APIs for MAILBOX
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_MAILBOX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_mailbox.h"
#include "adi_apollo_mailbox_handler_local.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_mailbox_ping(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_ping_t *cmd, adi_apollo_mailbox_resp_ping_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_PING, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_ping_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_ping_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_run_init(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_run_init_t *cmd, adi_apollo_mailbox_resp_run_init_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_RUN_INIT, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_run_init_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_run_init_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_run_init_get_completion(adi_apollo_device_t *device, adi_apollo_mailbox_resp_run_init_get_completion_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_RUN_INIT_GET_COMPLETION, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_run_init_get_completion_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_run_init_get_detailed_status(adi_apollo_device_t *device, adi_apollo_mailbox_resp_run_init_get_detailed_status_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_RUN_INIT_GET_DETAILED_STATUS, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_run_init_get_detailed_status_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_run_init_abort(adi_apollo_device_t *device, adi_apollo_mailbox_resp_run_init_abort_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_RUN_INIT_ABORT, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_run_init_abort_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_enabled_tracking_cals(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_enabled_tracking_cals_t *cmd, adi_apollo_mailbox_resp_set_enabled_tracking_cals_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_ENABLED_TRACKING_CALS, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_enabled_tracking_cals_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_enabled_tracking_cals_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_enabled_tracking_cals(adi_apollo_device_t *device, adi_apollo_mailbox_resp_get_enabled_tracking_cals_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_ENABLED_TRACKING_CALS, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_enabled_tracking_cals_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_tracking_cal_state(adi_apollo_device_t *device, adi_apollo_mailbox_resp_get_tracking_cal_state_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_TRACKING_CAL_STATE, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_tracking_cal_state_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_cal_status(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_get_cal_status_t *cmd, adi_apollo_mailbox_resp_get_cal_status_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_CAL_STATUS, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_get_cal_status_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_cal_status_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_sys_status(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_get_sys_status_t *cmd, adi_apollo_mailbox_resp_get_sys_status_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_SYS_STATUS, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_get_sys_status_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_sys_status_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_device_temperature(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_get_device_temperature_t *cmd, adi_apollo_mailbox_resp_get_device_temperature_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_DEVICE_TEMPERATURE, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_get_device_temperature_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_device_temperature_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_enabled_temp_sensors(adi_apollo_device_t *device, adi_apollo_mailbox_resp_get_enabled_temp_sensors_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_ENABLED_TEMP_SENSORS, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_enabled_temp_sensors_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_enabled_temp_sensors(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_enabled_temp_sensors_t *cmd, adi_apollo_mailbox_resp_set_enabled_temp_sensors_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_ENABLED_TEMP_SENSORS, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_enabled_temp_sensors_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_enabled_temp_sensors_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_unlock_config(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_unlock_config_t *cmd, adi_apollo_mailbox_resp_unlock_config_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_UNLOCK_CONFIG, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_unlock_config_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_unlock_config_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_config(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_config_t *cmd, adi_apollo_mailbox_resp_set_config_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_CONFIG, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_config_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_config_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_config(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_get_config_t *cmd, adi_apollo_mailbox_resp_get_config_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_CONFIG, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_get_config_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_config_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_ctrl(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_ctrl_t *cmd, adi_apollo_mailbox_resp_set_ctrl_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_CTRL, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_ctrl_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_ctrl_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_enter_debug_mode(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_enter_debug_mode_t *cmd, adi_apollo_mailbox_resp_enter_debug_mode_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_ENTER_DEBUG_MODE, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_enter_debug_mode_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_enter_debug_mode_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_debug(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_debug_t *cmd, adi_apollo_mailbox_resp_debug_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_DEBUG, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_debug_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_debug_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_log_filters(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_log_filters_t *cmd, adi_apollo_mailbox_resp_set_log_filters_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_LOG_FILTERS, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_log_filters_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_log_filters_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_resume_bkpt(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_resume_bkpt_t *cmd, adi_apollo_mailbox_resp_resume_bkpt_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_RESUME_BKPT, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_resume_bkpt_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_resume_bkpt_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_run_serdes_eye_sweep(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_run_serdes_eye_sweep_t *cmd, adi_apollo_mailbox_resp_run_serdes_eye_sweep_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_RUN_SERDES_EYE_SWEEP, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_run_serdes_eye_sweep_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_run_serdes_eye_sweep_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_run_serdes_vert_eye_sweep(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_run_serdes_vert_eye_sweep_t *cmd, adi_apollo_mailbox_resp_run_serdes_vert_eye_sweep_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_RUN_SERDES_VERT_EYE_SWEEP, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_run_serdes_vert_eye_sweep_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_run_serdes_vert_eye_sweep_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_gpio(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_gpio_t *cmd, adi_apollo_mailbox_resp_set_gpio_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_GPIO, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_gpio_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_gpio_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_gpio(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_get_gpio_t *cmd, adi_apollo_mailbox_resp_get_gpio_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_GPIO, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_get_gpio_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_gpio_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_mbias_pre_clock_init(adi_apollo_device_t *device, adi_apollo_mailbox_resp_mbias_pre_clock_init_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_MBIAS_PRE_CLOCK_INIT, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_mbias_pre_clock_init_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_mbias_post_clock_init(adi_apollo_device_t *device, adi_apollo_mailbox_resp_mbias_post_clock_init_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_MBIAS_POST_CLOCK_INIT, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_mbias_post_clock_init_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_sysclk_configuration(adi_apollo_device_t *device, adi_apollo_mailbox_resp_sysclk_configuration_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SYSCLK_CONFIGURATION, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_sysclk_configuration_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_sysclk_conditioning(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_sysclk_conditioning_t *cmd, adi_apollo_mailbox_resp_sysclk_conditioning_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SYSCLK_CONDITIONING, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_sysclk_conditioning_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_sysclk_conditioning_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_sysclk_switch_to_hsdig(adi_apollo_device_t *device, adi_apollo_mailbox_resp_sysclk_switch_to_hsdig_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SYSCLK_SWITCH_TO_HSDIG, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_sysclk_switch_to_hsdig_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_master_bias_set_dac_bias_mode(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_master_bias_set_dac_bias_mode_t *cmd, adi_apollo_mailbox_resp_master_bias_set_dac_bias_mode_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_MASTER_BIAS_SET_DAC_BIAS_MODE, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_master_bias_set_dac_bias_mode_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_master_bias_set_dac_bias_mode_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_sysclk_switch_to_ringosc(adi_apollo_device_t *device, adi_apollo_mailbox_resp_sysclk_switch_to_ringosc_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SYSCLK_SWITCH_TO_RINGOSC, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_sysclk_switch_to_ringosc_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_pgm_pll(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_pgm_pll_t *cmd, adi_apollo_mailbox_resp_pgm_pll_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_PGM_PLL, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_pgm_pll_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_pgm_pll_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_update_cal_data_crc(adi_apollo_device_t *device, adi_apollo_mailbox_resp_update_cal_data_crc_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_UPDATE_CAL_DATA_CRC, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_update_cal_data_crc_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_fw_version(adi_apollo_device_t *device, adi_apollo_mailbox_resp_get_fw_version_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_FW_VERSION, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_fw_version_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_request_challenge(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_request_challenge_t *cmd, adi_apollo_mailbox_resp_request_challenge_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_REQUEST_CHALLENGE, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_request_challenge_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_request_challenge_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_challenge(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_challenge_t *cmd, adi_apollo_mailbox_resp_set_challenge_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_CHALLENGE, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_challenge_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_challenge_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_power_up_jtx(adi_apollo_device_t *device, adi_apollo_mailbox_resp_power_up_jtx_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_POWER_UP_JTX, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_power_up_jtx_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_mcs_bsync_set_config(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_mcs_bsync_set_config_t *cmd, adi_apollo_mailbox_resp_mcs_bsync_set_config_t *resp)
{
	int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_MCS_BSYNC_SET_CONFIG, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_mcs_bsync_set_config_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_mcs_bsync_set_config_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_mcs_bsync_get_config(adi_apollo_device_t *device, adi_apollo_mailbox_resp_mcs_bsync_get_config_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_MCS_BSYNC_GET_CONFIG, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_mcs_bsync_get_config_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_mailbox_mcs_bsync_go(adi_apollo_device_t *device, adi_apollo_mailbox_resp_mcs_bsync_go_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_MCS_BSYNC_GO, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_mcs_bsync_go_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_get_adc_slice_modes(adi_apollo_device_t *device, adi_apollo_mailbox_resp_get_adc_slice_modes_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_GET_ADC_SLICE_MODES, NULL, 0, (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_get_adc_slice_modes_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_mailbox_set_adc_slice_mode_fast_switch_action(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_set_adc_slice_mode_fast_switch_action_t *cmd, adi_apollo_mailbox_resp_set_adc_slice_mode_fast_switch_action_t *resp)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(resp);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_mailbox_xfer(device, ADI_APOLLO_MAILBOX_SET_ADC_SLICE_MODE_FAST_SWITCH_ACTION, (uint8_t *)cmd, sizeof(adi_apollo_mailbox_cmd_set_adc_slice_mode_fast_switch_action_t), (uint8_t *)resp, sizeof(adi_apollo_mailbox_resp_set_adc_slice_mode_fast_switch_action_t));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}
/*! @} */
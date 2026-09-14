/*!
 * \brief     APIs for Apollo system clock calibration
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SYSCLK_COND
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_sysclk_cond.h"

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_sysclk_cond_cal(adi_apollo_device_t* device)
{
    int32_t err;
    adi_apollo_mailbox_resp_sysclk_conditioning_t sysclk_cond_resp;
    adi_apollo_mailbox_cmd_sysclk_conditioning_t cc_cmd;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Starting clock conditioning cal (can take up to %d secs)...\n", ADI_APOLLO_SYSCLK_COND_CENTER_MAX_TO);

    /*** ADC and SERDES tracking cals must be disabled ***/

    /*
     * Run clock conditioning calibration
    */
    cc_cmd.clk_path_segment = APOLLO_SYSCLK_ALL_WITH_RINGOSC_SWITCH;
    if (err = adi_apollo_mailbox_sysclk_conditioning(device, &cc_cmd, &sysclk_cond_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_sysclk_cond_cal returned err = %d", err);
        goto end;
    }
    if (sysclk_cond_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_sysclk_cond_cal response = %d", sysclk_cond_resp.status);
        err = API_CMS_ERROR_ERROR;
        goto end;
    }
    else {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "End clock conditioning: OK");
    }

end:
    return err;
}

int32_t adi_apollo_sysclk_cond_bg_cal_start(adi_apollo_device_t* device)
{
    int32_t err;
    adi_apollo_mailbox_resp_sysclk_conditioning_t sysclk_cond_resp;
    adi_apollo_mailbox_cmd_sysclk_conditioning_t cc_cmd;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    cc_cmd.clk_path_segment = APOLLO_SYSCLK_START_BG_CAL;
    if (err = adi_apollo_mailbox_sysclk_conditioning(device, &cc_cmd, &sysclk_cond_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_mailbox_sysclk_conditioning returned err = %d", err);
    }
    if (sysclk_cond_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_sysclk_bg_cond_caldata_start_validate response = %d", sysclk_cond_resp.status);
        err = API_CMS_ERROR_ERROR;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_sysclk_cond_bg_cal_stop(adi_apollo_device_t* device)
{
    int32_t err;
    adi_apollo_mailbox_resp_sysclk_conditioning_t sysclk_cond_resp;
    adi_apollo_mailbox_cmd_sysclk_conditioning_t cc_cmd;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    cc_cmd.clk_path_segment = APOLLO_SYSCLK_STOP_BG_CAL;
    if (err = adi_apollo_mailbox_sysclk_conditioning(device, &cc_cmd, &sysclk_cond_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_mailbox_sysclk_conditioning returned err = %d", err);
    }
    if (sysclk_cond_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_sysclk_bg_cond_caldata_stop_validate response = %d", sysclk_cond_resp.status);
        err = API_CMS_ERROR_ERROR;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_sysclk_cond_bg_cal_resume(adi_apollo_device_t* device)
{
    int32_t err;
    adi_apollo_mailbox_resp_sysclk_conditioning_t sysclk_cond_resp;
    adi_apollo_mailbox_cmd_sysclk_conditioning_t cc_cmd;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    cc_cmd.clk_path_segment = APOLLO_SYSCLK_RESUME_BG_CAL;
    if (err = adi_apollo_mailbox_sysclk_conditioning(device, &cc_cmd, &sysclk_cond_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_mailbox_sysclk_conditioning returned err = %d", err);
    }
    if (sysclk_cond_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_sysclk_bg_cond_caldata_resume_validate response = %d", sysclk_cond_resp.status);
        err = API_CMS_ERROR_ERROR;
    }

    return API_CMS_ERROR_OK;
}
/*! @} */

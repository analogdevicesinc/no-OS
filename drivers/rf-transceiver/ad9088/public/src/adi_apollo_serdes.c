/*!
 * \brief     APIs for SerDes related blocks
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SERDES
 * @{
 */

#define ADI_APOLLO_SERDES_JRX_TIMEOUT                 60

/*============= I N C L U D E S ============*/
#include "adi_apollo_serdes.h"
#include "adi_apollo_config.h"

#include "adi_apollo_bf_pll_mem_map.h"
#include "adi_apollo_bf_rtclk_gen.h"
#include "adi_apollo_bf_serdes_txdig_12pack_apollo_core1p1.h"
#include "adi_apollo_bf_serdes_txdig_phy_core1p2.h"
#include "adi_apollo_bf_serdes_rxdig_phy_core1p3.h"

#include "adi_apollo_hal.h"
#include "adi_apollo_cfg.h"
#include "adi_apollo_mailbox.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_SERDES_RX_LANES_MAX_PER_SIDE         (12U)
#define ADI_APOLLO_SERDES_RX_TEST_HORIZ_EYE_RESULTS     (1U)
#define ADI_APOLLO_SERDES_RX_TEST_VERT_EYE_RESULTS      (3U)


static uint32_t calc_serdes_tx_12pack_base(int32_t index);
static uint32_t calc_serdes_tx_dig_base(int32_t index);
static uint32_t calc_serdes_rx_dig_lane_base(int32_t index);
static uint32_t calc_serdes_tx_dig_lane_base(int32_t index);

static int32_t reset_synth(adi_apollo_device_t *device);
static int32_t reset_odiv(adi_apollo_device_t *device);

/* Table of reset functions */
static int32_t (*reset_tbl[])(adi_apollo_device_t *device) = {reset_synth, reset_odiv};

int32_t adi_apollo_serdes_jrx_cal(adi_apollo_device_t* device)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    return adi_apollo_serdes_jrx_init_cal(device, ADI_APOLLO_TXRX_SERDES_12PACK_ALL, ADI_APOLLO_INIT_CAL_ENABLED);
}

int32_t adi_apollo_serdes_jrx_init_cal(adi_apollo_device_t* device, uint16_t serdes, adi_apollo_init_cal_cfg_e init_cal_cfg)
{
    int32_t err = API_CMS_ERROR_OK;
    uint8_t i = 0;
    adi_apollo_mailbox_resp_run_init_get_completion_t run_init_get_completion = {0};
    adi_apollo_mailbox_resp_run_init_t run_init_resp = {0};
    uint32_t max_delay_us = ADI_APOLLO_SERDES_JRX_TIMEOUT * 1000000;
    uint32_t poll_delay_us = 1000000;
    uint32_t delay_us;
    uint8_t cal_complete = 0;

    adi_apollo_mailbox_cmd_run_init_t run_init_cmd = {0};
    adi_apollo_mailbox_resp_run_init_get_detailed_status_t run_init_cal_detailed_status_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(serdes > ADI_APOLLO_TXRX_SERDES_12PACK_ALL);

    /* Init the mailbox command struct */
    run_init_cmd.cal_mask = APOLLO_INIT_CAL_MSK_IC_SERDES_RX;
    run_init_cmd.serdes_rx_pack_mask = serdes;
    run_init_cmd.rx_channel_mask = 0;
    run_init_cmd.tx_channel_mask = 0;
    run_init_cmd.serdes_tx_pack_mask = 0;
    run_init_cmd.linearx_chan_mask = 0;

    /* Set SerDes Init Cal Configuration */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "SerDes JRx Init cal config: %d. ref: adi_apollo_init_cal_cfg_e\n", init_cal_cfg);
    if (err = adi_apollo_cfg_serdes_rx_init_cal_cfg_set(device, serdes, init_cal_cfg), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_cfg_serdes_rx_init_cal_cfg_set() %d", err);
        goto end;
    }

    /* Send mailbox command to start cal */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Starting SerDes JRx foreground cal\n");
    if (err = adi_apollo_mailbox_run_init(device, &run_init_cmd, &run_init_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_run_init() %d", err);
        goto end;
    }

    if (run_init_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "SerDes JRx Init cal error code = 0x%x\n", run_init_resp.status);
        err = API_CMS_ERROR_MAILBOX_RESP_STATUS;
        goto end;
    }

    /* Wait for SERDES cal to finish. */
    for (delay_us = 0; delay_us < max_delay_us; delay_us += poll_delay_us) {
        if (err = adi_apollo_mailbox_run_init_get_completion(device, &run_init_get_completion), err != API_CMS_ERROR_OK) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "adi_apollo_mailbox_run_init_get_completion() err %d\n");
            goto end;
        }

        if ((delay_us % (5 * poll_delay_us)) == 0) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "SerDes JRx Init Cal: status=%d in_progress=%d success=%d  %ds\n",
                                     run_init_get_completion.status, run_init_get_completion.in_progress, run_init_get_completion.success, (delay_us / poll_delay_us));
        }

        if (run_init_get_completion.in_progress == 0) {
            cal_complete = 1;
            break;
        }

        adi_apollo_hal_delay_us(device, poll_delay_us);
    }

    if (run_init_get_completion.success != 1) {
        if (err = adi_apollo_mailbox_run_init_get_detailed_status(device, &run_init_cal_detailed_status_resp), err != API_CMS_ERROR_OK) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_run_init_get_detailed_status() %d.\n", err);
            goto end;
        }

        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cpu_status = 0x%02X", run_init_cal_detailed_status_resp.status);
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cals_duration_msec = %d", run_init_cal_detailed_status_resp.cals_duration_msec);

        // SerDes init cal populates 2 index/channel data
        for (i = 0; i < 2; ++i) {
            adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "init_err_codes[%d] = 0x%02X \t init_err_cals[%d] = 0x%02X",
                                     i, run_init_cal_detailed_status_resp.init_err_codes[i], i, run_init_cal_detailed_status_resp.init_err_cals[i]);
            // adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "cals_since_power_ups[%d] = %d \t cals_last_runs[%d] = %d",
            //                          i, run_init_cal_detailed_status_resp.cals_since_power_up[i], i, run_init_cal_detailed_status_resp.cals_last_run[i]);
        }
    }

    if (cal_complete) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "SerDes JRx Init cal completed %s in %ds\n",
                                 run_init_get_completion.success ? "successfully" : "w/ ERROR", delay_us / 1000000);
        err = run_init_get_completion.success ? API_CMS_ERROR_OK : API_CMS_ERROR_SERDES_CAL_ERROR;
        goto end;
    } else {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "SerDes JRx Init cal timeout after %ds\n", max_delay_us / 1000000);
        err = API_CMS_ERROR_SERDES_CAL_TIMEOUT;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_serdes_jrx_bgcal_freeze(adi_apollo_device_t* device, uint16_t serdes)
{
    int32_t err = API_CMS_ERROR_OK;
    adi_apollo_mailbox_cmd_set_enabled_tracking_cals_t track_cal_cmd = {0};
    adi_apollo_mailbox_resp_set_enabled_tracking_cals_t track_cal_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(serdes > ADI_APOLLO_TXRX_SERDES_12PACK_ALL);

    /* Init the mailbox command struct */
    track_cal_cmd.serdes_rx_pack_mask = serdes;
    track_cal_cmd.enable_disable = 0;
    track_cal_cmd.adc_rx_channel_mask = 0;
    track_cal_cmd.mcs_tc_mask = 0;

    /*
    * Freeze the background cals
    */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Freezing SERDES JRx background tracking cals\n");


    if (err = adi_apollo_mailbox_set_enabled_tracking_cals(device, &track_cal_cmd, &track_cal_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_enabled_tracking_cals() (freeze) %d\n", err);
        goto end;
    }

    if (track_cal_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "SerDes JRx BG cal error code = 0x%x\n", track_cal_resp.status);
        err = API_CMS_ERROR_SERDES_CAL_ERROR;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_serdes_jrx_bgcal_unfreeze(adi_apollo_device_t* device, uint16_t serdes)
{
    int32_t err = API_CMS_ERROR_OK;
    adi_apollo_mailbox_cmd_set_enabled_tracking_cals_t track_cal_cmd = {0};
    adi_apollo_mailbox_resp_set_enabled_tracking_cals_t track_cal_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(serdes > ADI_APOLLO_TXRX_SERDES_12PACK_ALL);

    /* Init the mailbox command struct */
    track_cal_cmd.serdes_rx_pack_mask = serdes;
    track_cal_cmd.enable_disable = 1;
    track_cal_cmd.adc_rx_channel_mask = 0;
    track_cal_cmd.mcs_tc_mask = 0;

    /*
    * Unfreeze the background cals
    */
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "Unfreeze SERDES JRx background tracking cals\n");
    if (err = adi_apollo_mailbox_set_enabled_tracking_cals(device, &track_cal_cmd, &track_cal_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_enabled_tracking_cals() (unfreeze) %d\n", err);
        goto end;
    }

    if (track_cal_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "SerDes JRx BG cal error code = 0x%x\n", track_cal_resp.status);
        err = API_CMS_ERROR_SERDES_CAL_ERROR;
        goto end;
    }

end:
    return err;
}

int32_t adi_apollo_serdes_jrx_bgcal_state_get(adi_apollo_device_t *device, uint16_t serdes, adi_apollo_serdes_bgcal_state_t state[], uint32_t len)
{
    int32_t err = API_CMS_ERROR_OK;
    uint8_t i;
    uint16_t s = 0;
    adi_apollo_mailbox_resp_get_tracking_cal_state_t track_cal_state_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(serdes > ADI_APOLLO_TXRX_SERDES_12PACK_ALL);
    ADI_APOLLO_NULL_POINTER_RETURN(state);
    ADI_APOLLO_INVALID_PARAM_RETURN(len != ADI_APOLLO_NUM_SIDES);

    if (err = adi_apollo_mailbox_get_tracking_cal_state(device, &track_cal_state_resp), err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_get_tracking_cal_state() %d", err);
        goto end;
    }

    if (track_cal_state_resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "SerDes JRx BG cal state error code = 0x%x\n", track_cal_state_resp.status);
        err = API_CMS_ERROR_MAILBOX_RESP_STATUS;
        goto end;
    }

    for (i = 0; i < ADI_APOLLO_NUM_JRX_SERDES_12PACKS; i++) {
        s = serdes & (ADI_APOLLO_TXRX_SERDES_12PACK_A << i);
        if (s > 0) {
            state[i].state_valid = 1;
            state[i].bgcal_error = track_cal_state_resp.tracking_cal_state.serdes_rx_cal_error[i];
            state[i].bgcal_state = track_cal_state_resp.tracking_cal_state.serdes_rx_cal_state[i];
        } else {
            state[i].state_valid = 0;
            state[i].bgcal_error = track_cal_state_resp.tracking_cal_state.serdes_rx_cal_error[i];
            state[i].bgcal_state = track_cal_state_resp.tracking_cal_state.serdes_rx_cal_state[i];
        }
    }

end:
    return err;
}

int32_t adi_apollo_serdes_pll_config(adi_apollo_device_t *device, adi_apollo_serdes_pll_pgm_t *config)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    err = adi_apollo_hal_bf_set(device, BF_SDM_INT_INFO(SERDES_PLL), config->sdm_int);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_REF_CLK_DIVIDE_RATIO_INFO(SERDES_PLL), config->ref_clk_byte0);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_SERDES_PLL_ODIV_INFO(SERDES_PLL), config->serdes_pll_odiv);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_ROOT_CLKDIV_FUND_INFO(SERDES_PLL), config->root_clkdiv_fund);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_ROOT_CLKDIV_DIV2_INFO(SERDES_PLL), config->root_clkdiv_div2);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_VCO_SEL_INFO(SERDES_PLL), config->vco_sel);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_serializer_config(adi_apollo_device_t *device, const uint16_t sides, adi_apollo_serializer_pgm_t *config)
{
    int32_t err, i;
    uint32_t serdes_tx_12pack_base_addr = 0;
    uint32_t serdes_tx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        if ((1 << i) & sides) {
            serdes_tx_12pack_base_addr = calc_serdes_tx_12pack_base(i);
            serdes_tx_dig_base_addr = calc_serdes_tx_dig_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SER_CLK_DIV_INFO(serdes_tx_12pack_base_addr), config->tx_clock_div);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PARDATAMODE_SER_RC_INFO(serdes_tx_dig_base_addr), config->tx_data_width);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PD_SER_INFO(serdes_tx_dig_base_addr), config->pd_ser);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_serdes_pll_power(adi_apollo_device_t *device, adi_apollo_serdes_pll_power_up_down_e pwr_up)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_reg_set(device, REG_MISC_PD_ADDR(SERDES_PLL), pwr_up ? 0x00 : 0xff);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_LOGEN_CLKGEN_LOSYNC_PD_ADDR(SERDES_PLL), pwr_up ? 0x00 : 0xff);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_serdes_pll_reset(adi_apollo_device_t *device, adi_apollo_serdes_pll_reset_e reset_type)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(reset_type > ADI_APOLLO_SERDES_PLL_RESET_NUM-1);

    // Call the specific reset in index in table
    return reset_tbl[reset_type](device);
}

int32_t adi_apollo_serdes_prbs_generator_enable(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len,
        adi_apollo_serdes_prbs_generator_enable_t *config)
{
    int32_t err, i;
    uint8_t lane;
    uint32_t serdes_tx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lanes);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(lanes_len > ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2);

    for (i=0; i<lanes_len; i++) {

        lane = lanes[i];
        ADI_APOLLO_INVALID_PARAM_RETURN(lane > (ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2 - 1));

        serdes_tx_dig_base_addr = calc_serdes_tx_dig_lane_base(lane);

        err = adi_apollo_hal_bf_set(device, BF_DATA_GEN_MODE_INFO(serdes_tx_dig_base_addr), config->mode);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_bf_set(device, BF_DATA_GEN_EN_INFO(serdes_tx_dig_base_addr), config->enable);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_spo_dir_set(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len, uint8_t dir)
{
    int32_t err, i;
    uint8_t lane;
    uint32_t serdes_rx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lanes);
    ADI_APOLLO_INVALID_PARAM_RETURN(lanes_len > ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2);

    for (i=0; i<lanes_len; i++) {

        lane = lanes[i];
        ADI_APOLLO_INVALID_PARAM_RETURN(lane > (ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2 - 1));

        serdes_rx_dig_base_addr = calc_serdes_rx_dig_lane_base(lane);

        err = adi_apollo_hal_bf_set(device, BF_CK_SPO_UP_DN_INFO(serdes_rx_dig_base_addr), dir);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_set_flash_mask(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len, uint8_t flash_mask)
{
    int32_t err, i;
    uint8_t lane;
    uint32_t serdes_rx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lanes);
    ADI_APOLLO_INVALID_PARAM_RETURN(lanes_len > ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2);

    for (i=0; i<lanes_len; i++) {

        lane = lanes[i];
        ADI_APOLLO_INVALID_PARAM_RETURN(lane > (ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2 - 1));

        serdes_rx_dig_base_addr = calc_serdes_rx_dig_lane_base(lane);

        err = adi_apollo_hal_bf_set(device, BF_EN_FLASH_MASK_DES_RC_INFO(serdes_rx_dig_base_addr), flash_mask);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_clock_strobe(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len,
        adi_apollo_serdes_ck_spo_strobe_e spo_strobe)
{
    int32_t err, i;
    uint8_t lane;
    uint32_t serdes_rx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lanes);
    ADI_APOLLO_INVALID_PARAM_RETURN(lanes_len > ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2);

    for (i=0; i<lanes_len; i++) {

        lane = lanes[i];
        ADI_APOLLO_INVALID_PARAM_RETURN(lane > (ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2 - 1));

        serdes_rx_dig_base_addr = calc_serdes_rx_dig_lane_base(lane);

        if(spo_strobe == ADI_APOLLO_SERDES_CK_SPO_ISTROBE) {
            err = adi_apollo_hal_bf_set(device, BF_CK_SPO_ISTROBE_INFO(serdes_rx_dig_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CK_SPO_ISTROBE_INFO(serdes_rx_dig_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
        else if(spo_strobe == ADI_APOLLO_SERDES_CK_SPO_QSTROBE) {
            err = adi_apollo_hal_bf_set(device, BF_CK_SPO_QSTROBE_INFO(serdes_rx_dig_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CK_SPO_QSTROBE_INFO(serdes_rx_dig_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
        else {
            err = adi_apollo_hal_bf_set(device, BF_CK_SPO_ISTROBE_INFO(serdes_rx_dig_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CK_SPO_QSTROBE_INFO(serdes_rx_dig_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }

    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_prbs_clear_error(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len)
{
    int32_t err, i;
    uint8_t lane;
    uint32_t serdes_rx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lanes);
    ADI_APOLLO_INVALID_PARAM_RETURN(lanes_len > ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2);

    for (i=0; i<lanes_len; i++) {

        lane = lanes[i];
        ADI_APOLLO_INVALID_PARAM_RETURN(lane > (ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2 - 1));

        serdes_rx_dig_base_addr = calc_serdes_rx_dig_lane_base(lane);

        /* clear the error count */
        err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_DATAREC_CLR_INFO(serdes_rx_dig_base_addr), 1);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_DATAREC_CLR_INFO(serdes_rx_dig_base_addr), 0);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_prbs_checker_enable(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len,
        adi_apollo_serdes_prbs_checker_enable_t *config)
{
    int32_t err, i;
    uint8_t lane;
    uint32_t serdes_rx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lanes);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(lanes_len > ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2);

    for (i=0; i<lanes_len; i++) {

        lane = lanes[i];
        ADI_APOLLO_INVALID_PARAM_RETURN(lane > (ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2 - 1));

        serdes_rx_dig_base_addr = calc_serdes_rx_dig_lane_base(lane);

        err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_DATAREC_MODE_INFO(serdes_rx_dig_base_addr), config->prbs_mode);
        ADI_APOLLO_ERROR_RETURN(err);

        if (config->auto_mode == 0) {    //Regular mode
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_EN_INFO(serdes_rx_dig_base_addr), config->enable);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_DATAREC_EN_INFO(serdes_rx_dig_base_addr), config->enable);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_DATAREC_CLR_INFO(serdes_rx_dig_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_DATAREC_CLR_INFO(serdes_rx_dig_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
        else {
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_DATAREC_CLR_INFO(serdes_rx_dig_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_AUTO_MODE_THRESH_INFO(serdes_rx_dig_base_addr), config->auto_mode_thres);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_AUTO_MODE_INFO(serdes_rx_dig_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PRBS_RCV_AUTO_MODE_INFO(serdes_rx_dig_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_prbs_checker_status(adi_apollo_device_t *device,
            uint8_t lanes[],
            adi_apollo_serdes_prbs_checker_status_t status[],
            uint32_t lanes_len)
{
    int32_t err, i;
    uint8_t lane;
    uint32_t serdes_rx_dig_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lanes);
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(lanes_len > ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2);

    for (i=0; i<lanes_len; i++) {
        lane = lanes[i];
        ADI_APOLLO_INVALID_PARAM_RETURN(lane > (ADI_APOLLO_JESD_MAX_LANES_PER_SIDE*2 - 1));

        serdes_rx_dig_base_addr = calc_serdes_rx_dig_lane_base(lane);

        err = adi_apollo_hal_bf_get(device, BF_PRBS_RCV_ERR_INFO(serdes_rx_dig_base_addr), &(status[lane].err), 1);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_bf_get(device, BF_PRBS_RCV_ERR_STICKY_INFO(serdes_rx_dig_base_addr), &(status[lane].err_sticky), 1);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_bf_get(device, BF_PRBS_RCV_AUTO_MODE_DONE_INFO(serdes_rx_dig_base_addr), &(status[lane].auto_mode_done), 1);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_bf_get(device, BF_PRBS_RCV_ERR_CNT_INFO(serdes_rx_dig_base_addr), (uint8_t *) &(status[lane].err_count), 4);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_horiz_eye_sweep(adi_apollo_device_t *device,
                                              const uint16_t lane,
                                              const uint8_t prbs_pattern)
{
    int32_t err = API_CMS_ERROR_ERROR;
    adi_apollo_mailbox_cmd_run_serdes_eye_sweep_t horz_eye_cmd = {0};
    adi_apollo_mailbox_resp_run_serdes_eye_sweep_t resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_SERDES_LANE_LEN);

    horz_eye_cmd.lane = lane;
    horz_eye_cmd.prbs_pattern = prbs_pattern;
    horz_eye_cmd.force_using_outer = 0;
    horz_eye_cmd.prbs_check_duration_ms = 10;

    err = adi_apollo_mailbox_run_serdes_eye_sweep(device, &horz_eye_cmd, &resp);
    ADI_APOLLO_ERROR_RETURN(err);

    if (resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Horizontal Eye Sweep Mailbox Response Status Error code = %d\n", resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_horiz_eye_sweep_resp_get(adi_apollo_device_t *device,
                                                       const uint16_t lane,
                                                       adi_apollo_serdes_jrx_horiz_eye_resp_t *resp)
{
    int32_t err = API_CMS_ERROR_ERROR;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_RX_TEST_HORIZ_EYE_RESULTS,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_SERDES_LANE_LEN);

    /*
    set_ctrl_cmd is generic cmd and for SERDES Rx/Tx related cmds,
    requires the channel_num to be set to 0 for side A and 1 for side B
    */
    set_ctrl_cmd.channel_num = ((lane / ADI_APOLLO_SERDES_RX_LANES_MAX_PER_SIDE) == 0) ? APOLLO_SERDES_PACK_NUM_PACK_0_EAST : APOLLO_SERDES_PACK_NUM_PACK_1_WEST;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    ADI_APOLLO_ERROR_RETURN(err);

    if (set_ctrl_resp.status != API_CMS_ERROR_OK){
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    resp->ver = set_ctrl_resp.data_buffer[4];
    resp->spo_left = set_ctrl_resp.data_buffer[5];
    resp->spo_right = set_ctrl_resp.data_buffer[6];

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_vert_eye_sweep(adi_apollo_device_t *device,
                                             const uint16_t lane)
{
    int32_t err = API_CMS_ERROR_ERROR;
    adi_apollo_mailbox_cmd_run_serdes_vert_eye_sweep_t vert_eye_cmd = {0};
    adi_apollo_mailbox_resp_run_serdes_vert_eye_sweep_t resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_SERDES_LANE_LEN);

    vert_eye_cmd.lane = lane;

    err = adi_apollo_mailbox_run_serdes_vert_eye_sweep(device, &vert_eye_cmd, &resp);
    ADI_APOLLO_ERROR_RETURN(err);

    if (resp.status != APOLLO_CPU_NO_ERROR) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Vertical Eye Sweep Mailbox Response Status Error code = %d\n", resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_jrx_vert_eye_sweep_resp_get(adi_apollo_device_t *device,
                                                      const uint16_t lane,
                                                      adi_apollo_serdes_jrx_vert_eye_resp_t *resp)
{
    int32_t err = API_CMS_ERROR_ERROR;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_RX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_RX_TEST_VERT_EYE_RESULTS,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_SERDES_LANE_LEN);

    /*
    set_ctrl_cmd is generic cmd and for SERDES Rx/Tx related cmds,
    requires the channel_num to be set to 0 for side A and 1 for side B
    */
    set_ctrl_cmd.channel_num = ((lane / ADI_APOLLO_SERDES_RX_LANES_MAX_PER_SIDE) == 0) ? APOLLO_SERDES_PACK_NUM_PACK_0_EAST : APOLLO_SERDES_PACK_NUM_PACK_1_WEST;

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    ADI_APOLLO_ERROR_RETURN(err);

    if (set_ctrl_resp.status != API_CMS_ERROR_OK){
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    resp->ver = set_ctrl_resp.data_buffer[4];
    memcpy(resp->eye_heights_at_spo, set_ctrl_resp.data_buffer + 5, ADI_APOLLO_SERDES_JRX_VERT_EYE_TEST_RESP_BUF_SIZE * sizeof(int8_t));

    return API_CMS_ERROR_OK;
}

static int32_t reset_synth(adi_apollo_device_t *device)
{
    int err;

    // Assert reset
    err = adi_apollo_hal_reg_set(device, REG_SYNTH_RESETB_ADDR(SERDES_PLL), 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    // Lift reset
    err = adi_apollo_hal_reg_set(device, REG_SYNTH_RESETB_ADDR(SERDES_PLL), 0xff);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static int32_t reset_odiv(adi_apollo_device_t *device)
{
    int err;

    // Assert reset
    err = adi_apollo_hal_bf_set(device, BF_SERDES_PLL_ODIV_RB_INFO(SERDES_PLL), 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    // Lift reset
    err = adi_apollo_hal_bf_set(device, BF_SERDES_PLL_ODIV_RB_INFO(SERDES_PLL), 0x01);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_serdes_pll_locked_status(adi_apollo_device_t *device,
            adi_apollo_sides_e side,
            uint16_t lane_mask,
            uint16_t *status)
{
    int32_t err, i;
    uint32_t serdes_rx_dig_base_addr = 0;
    uint8_t lane;
    uint8_t pll_locked = 0;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(side >= ADI_APOLLO_NUM_SIDES);
    ADI_APOLLO_INVALID_PARAM_RETURN(lane_mask > 0xFFF);

    /* clear the status */
    *status = 0;

    for(i = 0; i < ADI_APOLLO_JESD_MAX_LANES_PER_SIDE; i++) {
        if ((1 << i) & lane_mask) {
            lane = i + (side * ADI_APOLLO_JESD_MAX_LANES_PER_SIDE);
            serdes_rx_dig_base_addr = calc_serdes_rx_dig_lane_base(lane);

            err = adi_apollo_hal_bf_get(device, BF_RFPLL_LOCKED_INFO(serdes_rx_dig_base_addr), (uint8_t *) &pll_locked, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            if(pll_locked) {
                *status |= (1 << i);
            }
        }
    }
    return API_CMS_ERROR_OK;
}

static uint32_t calc_serdes_tx_12pack_base(int32_t index)
{
    static uint32_t tx_12pack_regmap[] = {
        SER_PHY_TOP_12PACK_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY_TOP_12PACK_SERDES_TX_JTX_TOP_RX_DIGITAL1
    };

    return (tx_12pack_regmap[index]);
}

static uint32_t calc_serdes_tx_dig_base(int32_t index)
{
    static uint32_t serdes_tx_dig_regmap[] = {
        SER_PHY_ALL_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY_ALL_SERDES_TX_JTX_TOP_RX_DIGITAL1
    };

    return (serdes_tx_dig_regmap[index]);
}

static uint32_t calc_serdes_rx_dig_lane_base(int32_t index)
{
    static uint32_t serdes_rx_dig_lane_regmap[] = {
        DESER_PHY0_SERDES_RX_JRX_TX_DIGITAL0, DESER_PHY1_SERDES_RX_JRX_TX_DIGITAL0,
        DESER_PHY2_SERDES_RX_JRX_TX_DIGITAL0, DESER_PHY3_SERDES_RX_JRX_TX_DIGITAL0,
        DESER_PHY4_SERDES_RX_JRX_TX_DIGITAL0, DESER_PHY5_SERDES_RX_JRX_TX_DIGITAL0,
        DESER_PHY6_SERDES_RX_JRX_TX_DIGITAL0, DESER_PHY7_SERDES_RX_JRX_TX_DIGITAL0,
        DESER_PHY8_SERDES_RX_JRX_TX_DIGITAL0, DESER_PHY9_SERDES_RX_JRX_TX_DIGITAL0,
        DESER_PHY10_SERDES_RX_JRX_TX_DIGITAL0, DESER_PHY11_SERDES_RX_JRX_TX_DIGITAL0,
        DESER_PHY0_SERDES_RX_JRX_TX_DIGITAL1, DESER_PHY1_SERDES_RX_JRX_TX_DIGITAL1,
        DESER_PHY2_SERDES_RX_JRX_TX_DIGITAL1, DESER_PHY3_SERDES_RX_JRX_TX_DIGITAL1,
        DESER_PHY4_SERDES_RX_JRX_TX_DIGITAL1, DESER_PHY5_SERDES_RX_JRX_TX_DIGITAL1,
        DESER_PHY6_SERDES_RX_JRX_TX_DIGITAL1, DESER_PHY7_SERDES_RX_JRX_TX_DIGITAL1,
        DESER_PHY8_SERDES_RX_JRX_TX_DIGITAL1, DESER_PHY9_SERDES_RX_JRX_TX_DIGITAL1,
        DESER_PHY10_SERDES_RX_JRX_TX_DIGITAL1, DESER_PHY11_SERDES_RX_JRX_TX_DIGITAL1
    };

    return (serdes_rx_dig_lane_regmap[index]);
}

static uint32_t calc_serdes_tx_dig_lane_base(int32_t index)
{
    static uint32_t serdes_tx_dig_lane_regmap[] = {
        SER_PHY0_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY1_SERDES_TX_JTX_TOP_RX_DIGITAL0,
        SER_PHY2_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY3_SERDES_TX_JTX_TOP_RX_DIGITAL0,
        SER_PHY4_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY5_SERDES_TX_JTX_TOP_RX_DIGITAL0,
        SER_PHY6_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY7_SERDES_TX_JTX_TOP_RX_DIGITAL0,
        SER_PHY8_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY9_SERDES_TX_JTX_TOP_RX_DIGITAL0,
        SER_PHY10_SERDES_TX_JTX_TOP_RX_DIGITAL0, SER_PHY11_SERDES_TX_JTX_TOP_RX_DIGITAL0,
        SER_PHY0_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY1_SERDES_TX_JTX_TOP_RX_DIGITAL1,
        SER_PHY2_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY3_SERDES_TX_JTX_TOP_RX_DIGITAL1,
        SER_PHY4_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY5_SERDES_TX_JTX_TOP_RX_DIGITAL1,
        SER_PHY6_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY7_SERDES_TX_JTX_TOP_RX_DIGITAL1,
        SER_PHY8_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY9_SERDES_TX_JTX_TOP_RX_DIGITAL1,
        SER_PHY10_SERDES_TX_JTX_TOP_RX_DIGITAL1, SER_PHY11_SERDES_TX_JTX_TOP_RX_DIGITAL1
    };

    return (serdes_tx_dig_lane_regmap[index]);
}
/*! @} */
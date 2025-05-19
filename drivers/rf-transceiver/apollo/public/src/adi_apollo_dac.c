/*!
 * \brief     APIs for DAC
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DAC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_dac.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"
#include "adi_apollo_private_blk_sel_types.h"
#include "adi_apollo_bf_tx_hsdout.h"
#include "adi_apollo_mailbox.h"


#define ADI_APOLLO_DAC_DECODER_ENABLE_UINT8                 0        /*!< Enable decoder */
#define ADI_APOLLO_DAC_BIAS_FORCE_STANDBY_EN_UINT8          4        /*!< Bias force standby enable */
#define ADI_APOLLO_DAC_CLK_DUTYCTRL_OFFSET_PRG_UINT8        5        /*!< ADI_APOLLO_DAC_CLK_TRIM_2 */
#define ADI_APOLLO_DAC_CLK_PHASECTRL_OFFSET_PRG_UINT8       6        /*!< ADI_APOLLO_DAC_CLK_TRIM_3 */
#define ADI_APOLLO_DAC_CLK_PHASECTRL_SKEW_PRG_UINT8         7        /*!< ADI_APOLLO_DAC_CLK_TRIM_1 */


static uint32_t calc_tx_hsdout_base(int32_t idx);
static uint8_t calc_dac_trim_param(adi_apollo_dac_clk_trim_e trim_attr);

int32_t adi_apollo_dac_clk_trim_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, adi_apollo_dac_clk_trim_e trim_attr, uint8_t trim_val)
{
    int32_t err;
    uint8_t i;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_DAC_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = calc_dac_trim_param(trim_attr),
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = trim_val,
        .data_buffer[5]    = 0,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_RANGE_CHECK(trim_attr, ADI_APOLLO_DAC_CLK_TRIM_1, ADI_APOLLO_DAC_CLK_TRIM_3);
    ADI_APOLLO_DAC_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        if ( (dacs & (ADI_APOLLO_DAC_A0 << i)) > 0 ) {
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
            adi_apollo_hal_delay_us(device, 100 * 1000);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dac_clk_trim_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, adi_apollo_dac_clk_trim_e trim_attr, uint8_t *trim_val)
{
    int32_t err;
    uint8_t i;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_DAC_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = calc_dac_trim_param(trim_attr),
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(trim_val);
    ADI_CMS_RANGE_CHECK(trim_attr, ADI_APOLLO_DAC_CLK_TRIM_1, ADI_APOLLO_DAC_CLK_TRIM_3);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(dacs) != 1);
    ADI_APOLLO_DAC_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        if ( (dacs & (ADI_APOLLO_DAC_A0 << i)) > 0 ) {
            set_ctrl_cmd.channel_num = i;
            break;
        }
    }

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract DAC Trim info from response structure */
    *trim_val = set_ctrl_resp.data_buffer[4];

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dac_scrambler_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t scrambler_sel, uint8_t enable)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t scrambler_chan;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DAC_BLK_SEL_MASK(scrambler_sel);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        scrambler_chan = scrambler_sel & (ADI_APOLLO_DAC_A0 << i);
        if (scrambler_chan > 0) {
            regmap_base_addr = calc_tx_hsdout_base(i);

            /*  Enable DAC Scrambler */
            err = adi_apollo_hal_bf_set(device, BF_DAC_SCRAMBLER_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_dac_scrambler_enable_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t scrambler_sel, uint8_t *enable)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t scrambler_chan;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(scrambler_sel) != 1);
    ADI_APOLLO_DAC_BLK_SEL_MASK(scrambler_sel);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        scrambler_chan = scrambler_sel & (ADI_APOLLO_DAC_A0 << i);
        if (scrambler_chan > 0) {
            regmap_base_addr = calc_tx_hsdout_base(i);

	        /* Get Scrambler enable status */
            err = adi_apollo_hal_bf_get(device, BF_DAC_SCRAMBLER_EN_INFO(regmap_base_addr), enable, sizeof(*enable));
            ADI_APOLLO_ERROR_RETURN(err);

            break;      /* only one chan per call */
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dac_data_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t enable)
{
    int32_t err;
    uint8_t i;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_DAC_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_DAC_DECODER_ENABLE_UINT8,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = enable,
        .data_buffer[5]    = 0,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_RANGE_CHECK(enable, 0, 1);
    ADI_APOLLO_DAC_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        if ( (dacs & (ADI_APOLLO_DAC_A0 << i)) > 0 ) {
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
            adi_apollo_hal_delay_us(device, 100 * 1000);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dac_standby_lock_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t lock_state)
{
    int32_t err;
    uint8_t i;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_DAC_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_DAC_BIAS_FORCE_STANDBY_EN_UINT8,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = lock_state,
        .data_buffer[5]    = 0,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(lock_state > 1);
    ADI_APOLLO_DAC_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        if ( (dacs & (ADI_APOLLO_DAC_A0 << i)) > 0 ) {
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
            adi_apollo_hal_delay_us(device, 100 * 1000);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dac_standby_lock_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t *lock_state)
{
    int32_t err;
    uint8_t i;
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_DAC_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_DAC_BIAS_FORCE_STANDBY_EN_UINT8,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4
    };
    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp = {0};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lock_state);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(dacs) != 1);
    ADI_APOLLO_DAC_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        if ( (dacs & (ADI_APOLLO_DAC_A0 << i)) > 0 ) {
            set_ctrl_cmd.channel_num = i;
            break;
        }
    }

    err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
    if (err != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
        return err;
    }

    if (set_ctrl_resp.status != API_CMS_ERROR_OK) {
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
        return API_CMS_ERROR_MAILBOX_RESP_STATUS;
    }

    /* Extract DAC STANDBY Control lock state info from response structure */
    *lock_state = set_ctrl_resp.data_buffer[4];

    return API_CMS_ERROR_OK;
}


__maybe_unused uint32_t calc_tx_hsdout_base(int32_t index)
{
    /*
     * This table is organized for DAC context usage (fifo, scrambler) of hsdout,
     * different from datapath (e.g. inv sinc)
     */
    static uint32_t tx_hsdout_regmap[ADI_APOLLO_DAC_NUM] = {
        TX_HSDOUT0_TX_SLICE_0_TX_DIGITAL0,      /* A0  */
        TX_HSDOUT1_TX_SLICE_0_TX_DIGITAL0,      /* A1* */
        TX_HSDOUT0_TX_SLICE_1_TX_DIGITAL0,      /* A2* */
        TX_HSDOUT1_TX_SLICE_1_TX_DIGITAL0,      /* A3  */

        TX_HSDOUT0_TX_SLICE_0_TX_DIGITAL1,      /* B0  */
        TX_HSDOUT1_TX_SLICE_0_TX_DIGITAL1,      /* B1* */
        TX_HSDOUT0_TX_SLICE_1_TX_DIGITAL1,      /* B2* */
        TX_HSDOUT1_TX_SLICE_1_TX_DIGITAL1       /* B3  */
    };
    return tx_hsdout_regmap[index];
}

static uint8_t calc_dac_trim_param(adi_apollo_dac_clk_trim_e trim_attr)
{
    ADI_CMS_RANGE_CHECK(trim_attr, ADI_APOLLO_DAC_CLK_TRIM_1, ADI_APOLLO_DAC_CLK_TRIM_3);

    if (trim_attr == ADI_APOLLO_DAC_CLK_TRIM_1) {
        return ADI_APOLLO_DAC_CLK_PHASECTRL_SKEW_PRG_UINT8;
    } else if (trim_attr == ADI_APOLLO_DAC_CLK_TRIM_2) {
        return ADI_APOLLO_DAC_CLK_DUTYCTRL_OFFSET_PRG_UINT8;
    } else {
        return ADI_APOLLO_DAC_CLK_PHASECTRL_OFFSET_PRG_UINT8;
    }
}

/*! @} */

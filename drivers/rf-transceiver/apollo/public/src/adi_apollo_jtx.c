/*!
 * \brief     APIs for JTx
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_JTX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_utils.h"
#include "adi_apollo_jtx.h"
#include "adi_apollo_dformat_local.h"

#include "adi_apollo_bf_jtx_dformat.h"
#include "adi_apollo_bf_jtx_dual_link.h"
#include "adi_apollo_bf_jtx_qbf_txfe.h"
#include "adi_apollo_bf_serdes_txdig_phy_core1p2.h"

#include "adi_apollo_mailbox.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_JTX_LANES_MAX                    12
#define ADI_APOLLO_SERDES_TX_DRIVE_SWING_STRUCT      0
#define ADI_APOLLO_SERDES_TX_PRE_EMPHASIS_STRUCT     1
#define ADI_APOLLO_SERDES_TX_POST_EMPHASIS_STRUCT    2
#define ADI_APOLLO_SERDES_TX_SWING_EMPHASIS_STRUCT   3
#define ADI_APOLLO_SERDES_TX_LANES_SWING_EMPHASIS_STRUCT   4

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_jtx_link_enable_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t link_en)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_en > 1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dual_link_base(i);
            err = adi_apollo_hal_bf_set(device, BF_JTX_LINK_EN_INFO(regmap_base_addr), link_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_link_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    uint16_t *status)
{
    int32_t  err, i, link_count, link_index;
    uint32_t regmap_base_addr = 0;
    uint16_t res;
    uint8_t  reg8;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);

    link_count = 0;
    link_index = 0;
    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & link) {
            /*Reading status for only one link*/
            link_count++;
            ADI_APOLLO_INVALID_PARAM_RETURN(link_count > 1);
            link_index = i;
        }
    }

    if (((1 << link_index) & link)) {

        regmap_base_addr = calc_jtx_dual_link_base(link_index);

        err = adi_apollo_hal_bf_get(device, BF_JTX_SYNC_N_SEL_INFO(regmap_base_addr), &reg8, 1); /*  QBF status */
        ADI_APOLLO_ERROR_RETURN(err);
        res = reg8 & 0x0F;

        err = adi_apollo_hal_bf_get(device, BF_JTX_DL_204B_SYNC_N_INFO(regmap_base_addr), &reg8, 1); /* frame sync */
        ADI_APOLLO_ERROR_RETURN(err);
        res += (reg8 & 0x1) << 4;

        /* PLL status applicable for only link0 registers*/
        regmap_base_addr = calc_jtx_qbf_txfe_base(link_index == 1 ? 0 : link_index  == 3 ? 2 : link_index);

        err = adi_apollo_hal_bf_get(device, BF_JTX_PLL_LOCKED_INFO(regmap_base_addr), &reg8, 1); /* PLL status */
        ADI_APOLLO_ERROR_RETURN(err);
        res += (reg8 & 0x1) << 5;

        regmap_base_addr = calc_jtx_qbf_txfe_base(link_index);

        err = adi_apollo_hal_bf_get(device, BF_JTX_PHASE_ESTABLISHED_INFO(regmap_base_addr), &reg8, 1); /* Phase Established Readback */
        ADI_APOLLO_ERROR_RETURN(err);
        res += (reg8 & 0x1) << 6;

        err = adi_apollo_hal_bf_get(device, BF_JTX_INVALID_MODE_INFO(regmap_base_addr), &reg8, 1); /* Invalid Mode */
        ADI_APOLLO_ERROR_RETURN(err);
        res += (reg8 & 0x1) << 7;
    }
    *status = res;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_link_inspect(adi_apollo_device_t *device,
    const uint16_t links,
    adi_apollo_jesd_tx_inspect_t *jtx_inspect)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(jtx_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(links) != 1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_jesd_param_base(i);

            err = adi_apollo_hal_bf_get(device, BF_JTX_L_CFG_INFO(regmap_base_addr), (uint8_t* )&(jtx_inspect->l_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_F_CFG_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->f_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_M_CFG_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->m_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_S_CFG_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->s_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_N_CFG_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->n_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_NP_CFG_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->np_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_K_CFG_INFO(regmap_base_addr), &(jtx_inspect->k_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_CS_CFG_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->cs), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_SUBCLASSV_CFG_INFO(regmap_base_addr), &(jtx_inspect->subclass), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            regmap_base_addr = calc_jtx_dual_link_base(i);
            err = adi_apollo_hal_bf_get(device, BF_JTX_LINK_EN_INFO(regmap_base_addr), &(jtx_inspect->link_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JTX_NS_CFG_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->ns_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            regmap_base_addr = calc_jtx_dual_link_base((i/2)*2);    // bf only valid on link A0/B0
            err = adi_apollo_hal_bf_get(device, BF_JTX_LINK_204C_SEL_INFO(regmap_base_addr), (uint8_t*)&(jtx_inspect->ver), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            regmap_base_addr = calc_jtx_qbf_txfe_base(i);
            err = adi_apollo_hal_bf_get(device, BF_JTX_MODE_INFO(regmap_base_addr), (uint8_t*) &(jtx_inspect->jesd_mode), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_phase_adjust_set(adi_apollo_device_t* device,
    const uint16_t links,
    const uint16_t phase_adjust)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {

            if ((1 << i) & links) {
                regmap_base_addr = calc_jtx_dual_link_base(i);

                err = adi_apollo_hal_bf_set(device, BF_JTX_TPL_PHASE_ADJUST_INFO(regmap_base_addr), (uint8_t)phase_adjust);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_subclass_set(adi_apollo_device_t *device,
    const uint16_t links,
    const uint16_t subclass)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {

            if ((1 << i) & links) {
                regmap_base_addr = calc_jtx_dual_link_base(i);

                err = adi_apollo_hal_bf_set(device, BF_JTX_SUBCLASSV_CFG_INFO(regmap_base_addr), subclass);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lane_xbar_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint8_t physical_lane, uint8_t logical_lane)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(logical_lane > ADI_APOLLO_JTX_LANES_MAX-1);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dual_link_base(i);
            err = adi_apollo_hal_bf_set(device, BF_JTX_LANE_ASSIGN_INFO(regmap_base_addr, physical_lane), logical_lane);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lanes_xbar_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t logical_lanes[], uint32_t length)
{
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(length > ADI_APOLLO_JTX_LANES_MAX);

    for (i = 0; i < length; i++) {
        err = adi_apollo_jtx_lane_xbar_set(device, links, i, logical_lanes[i]);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lane_force_pd_set(adi_apollo_device_t *device,
    const uint16_t link_sides,
    uint8_t physical_lane,
    uint8_t power_down)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);
    ADI_APOLLO_INVALID_PARAM_RETURN(power_down > 1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            regmap_base_addr = calc_jtx_dual_link_base(i);
            err = adi_apollo_hal_bf_set(device, BF_JTX_FORCE_LANE_PD_INFO(regmap_base_addr, physical_lane), power_down);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lane_drive_swing_set(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t drive_swing)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_DRIVE_SWING_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = physical_lane,
        .data_buffer[5]    = drive_swing,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };

    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);
    ADI_APOLLO_INVALID_PARAM_RETURN(drive_swing > ADI_APOLLO_SER_SWING_500);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lane_pre_emphasis_set(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t pre_emp)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_PRE_EMPHASIS_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = physical_lane,
        .data_buffer[5]    = pre_emp,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };

    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);
    ADI_APOLLO_INVALID_PARAM_RETURN(pre_emp > ADI_APOLLO_SER_PRE_EMP_6DB);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lane_post_emphasis_set(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t post_emp)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_POST_EMPHASIS_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = physical_lane,
        .data_buffer[5]    = post_emp,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };

    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);
    ADI_APOLLO_INVALID_PARAM_RETURN(post_emp > ADI_APOLLO_SER_POST_EMP_12DB);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_multi_lane_swing_emphasis_set(adi_apollo_device_t *device,
                                                uint8_t link_sides,
                                                uint8_t* physical_lane,
                                                uint8_t* drive_swing,
                                                uint8_t* pre_emp,
                                                uint8_t* post_emp,
                                                uint32_t num_lanes)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_SET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_LANES_SWING_EMPHASIS_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4 + (num_lanes * 4)
    };


    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(num_lanes > ADI_APOLLO_JTX_LANES_MAX);

    for (int i = 0; i < num_lanes; i++) {
        ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane[i] > ADI_APOLLO_JTX_LANES_MAX-1);
        ADI_APOLLO_INVALID_PARAM_RETURN(drive_swing[i] > ADI_APOLLO_SER_SWING_500);
        ADI_APOLLO_INVALID_PARAM_RETURN(pre_emp[i] > ADI_APOLLO_SER_PRE_EMP_6DB);
        ADI_APOLLO_INVALID_PARAM_RETURN(post_emp[i] > ADI_APOLLO_SER_POST_EMP_12DB);
        set_ctrl_cmd.data_buffer[4 + (i*4)] = physical_lane[i];
        set_ctrl_cmd.data_buffer[5 + (i*4)] = drive_swing[i];
        set_ctrl_cmd.data_buffer[6 + (i*4)] = pre_emp[i];
        set_ctrl_cmd.data_buffer[7 + (i*4)] = post_emp[i];
    }

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lr_adapt_set(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t lr_adapt)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX - 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(lr_adapt > ADI_APOLLO_SERDES_LR_ADAPT_RATIO_32);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            regmap_base_addr = calc_jtx_dual_link_base(i);
            err = adi_apollo_hal_bf_set(device, BF_JTX_BR_LOG2_RATIO_INFO(regmap_base_addr, physical_lane), lr_adapt);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;      
}


int32_t adi_apollo_jtx_lane_drive_swing_get(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t *drive_swing)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_DRIVE_SWING_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = physical_lane,
        .data_buffer[5]    = 0,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };

    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }

            /*  Extract Drive Swing info from response structure
                set_ctrl_resp.data_buffer[4] --> physical_lane
                set_ctrl_resp.data_buffer[5] --> drive_swing    */
            *drive_swing = set_ctrl_resp.data_buffer[5];
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lane_pre_emphasis_get(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t *pre_emp)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_PRE_EMPHASIS_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = physical_lane,
        .data_buffer[5]    = 0,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };

    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }

            /*  Extract Drive Swing info from response structure
                set_ctrl_resp.data_buffer[4] --> physical_lane
                set_ctrl_resp.data_buffer[5] --> drive_swing    */
            *pre_emp = set_ctrl_resp.data_buffer[5];
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lane_post_emphasis_get(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t *post_emp)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_POST_EMPHASIS_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .data_buffer[4]    = physical_lane,
        .data_buffer[5]    = 0,
        .data_buffer[6]    = 0,
        .data_buffer[7]    = 0,
        .length            = 8
    };

    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX-1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }

            /*  Extract Drive Swing info from response structure
                set_ctrl_resp.data_buffer[4] --> physical_lane
                set_ctrl_resp.data_buffer[5] --> post_emp    */
            *post_emp = set_ctrl_resp.data_buffer[5];
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_multi_lane_swing_emphasis_get(adi_apollo_device_t *device,
                                                uint8_t link_sides,
                                                uint8_t* physical_lane,
                                                uint8_t* drive_swing,
                                                uint8_t* pre_emp,
                                                uint8_t* post_emp,
                                                uint32_t num_lanes)
{
    adi_apollo_mailbox_cmd_set_ctrl_t set_ctrl_cmd = {
        .sys_cal_object_id = APOLLO_CPU_OBJID_IC_SERDES_TX,
        .ctrl_cmd          = CTRL_CMD_PARAM_GET,
        .data_buffer[0]    = ADI_APOLLO_SERDES_TX_LANES_SWING_EMPHASIS_STRUCT,
        .data_buffer[1]    = 0,
        .data_buffer[2]    = 0,
        .data_buffer[3]    = 0,
        .length            = 4 + (num_lanes * 4)
    };

    adi_apollo_mailbox_resp_set_ctrl_t set_ctrl_resp;
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(physical_lane);
    ADI_APOLLO_NULL_POINTER_RETURN(drive_swing);
    ADI_APOLLO_NULL_POINTER_RETURN(pre_emp);
    ADI_APOLLO_NULL_POINTER_RETURN(post_emp);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(num_lanes > ADI_APOLLO_JTX_LANES_MAX);

    for (int i = 0; i < num_lanes; i++) {
        ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane[i] > ADI_APOLLO_JTX_LANES_MAX-1);
        set_ctrl_cmd.data_buffer[4 + (i*4)] = physical_lane[i];
    }

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            set_ctrl_cmd.channel_num = (i >> 1);     // 0 for side A and 1 for side B

            err = adi_apollo_mailbox_set_ctrl(device, &set_ctrl_cmd, &set_ctrl_resp);
            if (err != API_CMS_ERROR_OK) {
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Error from adi_apollo_mailbox_set_ctrl() %d", err);
                return err;
            }

            if (set_ctrl_resp.status != API_CMS_ERROR_OK){
                adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, "Mailbox Response Status Error from adi_apollo_mailbox_set_ctrl() 0x%X", set_ctrl_resp.status);
                return API_CMS_ERROR_ERROR;
            }

            /*  Extract Drive Swing info from response structure
                set_ctrl_resp.data_buffer[4] --> physical_lane
                set_ctrl_resp.data_buffer[5] --> drive_swing    
                set_ctrl_resp.data_buffer[6] --> pre_emp
                set_ctrl_resp.data_buffer[7] --> post_emp    */
            for (int i = 0; i < num_lanes; i++) {
                drive_swing[i] = set_ctrl_resp.data_buffer[(i*4)+5];
                pre_emp[i] = set_ctrl_resp.data_buffer[(i*4)+6];
                post_emp[i] = set_ctrl_resp.data_buffer[(i*4)+7];
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jtx_lr_adapt_get(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t *lr_adapt)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JTX_LANES_MAX - 1);
    ADI_APOLLO_NULL_POINTER_RETURN(lr_adapt);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i += ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            regmap_base_addr = calc_jtx_dual_link_base(i);
            err = adi_apollo_hal_bf_get(device, BF_JTX_BR_LOG2_RATIO_INFO(regmap_base_addr, physical_lane), lr_adapt, 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;      
}


int32_t adi_apollo_jtx_force_invalids_set(adi_apollo_device_t* device,
    const uint16_t links,
    const uint8_t enable)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FORCE_INVALID_EN_INFO(regmap_base_addr, i % 2), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_jtx_dual_link_base(int32_t link)
{
    static uint32_t jtx_dual_link_regmap[4] = {
        JTX_DUAL_LINK_0_JTX_TOP_RX_DIGITAL0, JTX_DUAL_LINK_1_JTX_TOP_RX_DIGITAL0,
        JTX_DUAL_LINK_0_JTX_TOP_RX_DIGITAL1, JTX_DUAL_LINK_1_JTX_TOP_RX_DIGITAL1
    };
    return jtx_dual_link_regmap[link];
}

uint32_t calc_jtx_qbf_txfe_base(int32_t link)
{
    static uint32_t jtx_qbf_regmap[4] = {
        JTX_QBF_TXFE_0_JTX_TOP_RX_DIGITAL0, JTX_QBF_TXFE_1_JTX_TOP_RX_DIGITAL0,
        JTX_QBF_TXFE_0_JTX_TOP_RX_DIGITAL1, JTX_QBF_TXFE_1_JTX_TOP_RX_DIGITAL1
    };
    return jtx_qbf_regmap[link];
}

uint32_t calc_jtx_jesd_param_base(int32_t link)
{
    static uint32_t jtx_jesd_param_regmap[4] = {
        JTX_DUAL_LINK_0_JTX_TOP_RX_DIGITAL0, JTX_DUAL_LINK_1_JTX_TOP_RX_DIGITAL0,
        JTX_DUAL_LINK_0_JTX_TOP_RX_DIGITAL1, JTX_DUAL_LINK_1_JTX_TOP_RX_DIGITAL1
    };
    return jtx_jesd_param_regmap[link];
}
/*! @} */
/*!
 * \brief     APIs for RECONFIG
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RECONFIG
 * \{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_reconfig.h"
#include "adi_apollo_reconfig_local.h"
#include "adi_apollo_private_device.h"

#include "adi_apollo_bf_txrx_prefsrc_reconf.h"
#include "adi_apollo_bf_txrx_postfsrc_reconf.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_reconfig_ctrl_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint8_t sides, adi_apollo_reconfig_ctrl_pgm_t *config)
{
    int32_t err;
    uint8_t i, side;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_DYN_CFG_LOCK);

    for(i = 0; i < ADI_APOLLO_NUM_SIDES; i ++) {
        side = sides & (ADI_APOLLO_SIDE_A << i);
        if (side > 0) {
            regmap_base_addr = calc_prefsrc_reconfig_base(terminal, i);
            
            err = adi_apollo_hal_bf_set(device, BF_TRIG_RECONF_MODE_INFO(regmap_base_addr), config->trig_reconfig_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CNCO_RESET_EN_INFO(regmap_base_addr), config->cnco_reset);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_RESET_EN_INFO(regmap_base_addr), config->timestamp_reset_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_RESYNC_EN_INFO(regmap_base_addr), config->resync_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TZERO_COHERENCE_EN_INFO(regmap_base_addr), config->tzero_coherence_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FNCO_RESET_EN_INFO(regmap_base_addr), config->fnco_reset_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PREFSRC_LCM_INFO(regmap_base_addr), config->prefsrc_lcm);
            ADI_APOLLO_ERROR_RETURN(err);
            
            regmap_base_addr = calc_postfsrc_reconfig_base(terminal, i);
            
            err = adi_apollo_hal_bf_set(device, BF_POSTFSRC_LCM_INFO(regmap_base_addr), config->postfsrc_lcm);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
            
    return API_CMS_ERROR_OK; 
}


int32_t adi_apollo_reconfig_trig_evt_cnt_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint8_t sides, adi_apollo_reconfig_type_e rec_type, uint16_t *count)
{
    int32_t err;
    uint8_t i, side;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_DYN_CFG_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(sides) != 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(rec_type >= ADI_APOLLO_RECONFIG_TYPE_NUM);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        side = sides & (ADI_APOLLO_SIDE_A << i);
        if (side > 0) {
            regmap_base_addr = calc_prefsrc_reconfig_base(terminal, i);

            if (rec_type == ADI_APOLLO_RECONFIG_EXTERNAL) {
                err = adi_apollo_hal_bf_get(device, BF_SYNC_INPUT_COUNT_INFO(regmap_base_addr), (uint8_t *)count, sizeof(*count));
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_get(device, BF_TRIG_INPUT_COUNT_INFO(regmap_base_addr), (uint8_t*)count, sizeof(*count));
                ADI_APOLLO_ERROR_RETURN(err);
            }

            break;  // single return val
        }
    }

    return API_CMS_ERROR_OK;
}




uint32_t calc_prefsrc_reconfig_base(adi_apollo_terminal_e terminal, int32_t reconfig_index)
{
    static uint32_t rx_prefsrc_reconfig_regmap[ADI_APOLLO_NUM_SIDES] = {
        TXRX_PREFSRC_RECONF_RX_SLICE_0_RX_DIGITAL0, TXRX_PREFSRC_RECONF_RX_SLICE_0_RX_DIGITAL1
    };
    static uint32_t tx_prefsrc_reconfig_regmap[ADI_APOLLO_NUM_SIDES] = {
        TXRX_PREFSRC_RECONF_TX_SLICE_0_TX_DIGITAL0, TXRX_PREFSRC_RECONF_TX_SLICE_0_TX_DIGITAL1
    };
    if (terminal == ADI_APOLLO_RX)
        return rx_prefsrc_reconfig_regmap[reconfig_index];
    else
        return tx_prefsrc_reconfig_regmap[reconfig_index];
}

uint32_t calc_postfsrc_reconfig_base(adi_apollo_terminal_e terminal, int32_t reconfig_index)
{
    static uint32_t rx_postfsrc_reconfig_regmap[ADI_APOLLO_NUM_SIDES] = {
        TXRX_POSTFSRC_RECONF_RX_SLICE_0_RX_DIGITAL0, TXRX_POSTFSRC_RECONF_RX_SLICE_0_RX_DIGITAL1
    };
    static uint32_t tx_postfsrc_reconfig_regmap[ADI_APOLLO_NUM_SIDES] = {
        TXRX_POSTFSRC_RECONF_TX_SLICE_0_TX_DIGITAL0, TXRX_POSTFSRC_RECONF_TX_SLICE_0_TX_DIGITAL1
    };
    if (terminal == ADI_APOLLO_RX)
        return rx_postfsrc_reconfig_regmap[reconfig_index];
    else
        return tx_postfsrc_reconfig_regmap[reconfig_index];
}
/*! @} */
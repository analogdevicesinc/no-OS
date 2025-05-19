/*!
 * \brief     FNCO functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FNCO
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_fnco.h"
#include "adi_apollo_nco_local.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_txrx_fine_nco.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

#include "adi_utils.h"

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_fnco_profile_load(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos,
                adi_apollo_nco_profile_word_sel_e word_sel, uint8_t first, uint32_t words[], uint32_t length)
{
    int32_t err;
    uint16_t i, j;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(words);
    ADI_APOLLO_INVALID_PARAM_RETURN((first+length) > ADI_APOLLO_FNCO_PROFILE_NUM)
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {

        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);

        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            for(j = first; j < first+length; j++) {
                err = adi_apollo_hal_paged_bf_set(device, BF_HOP_PROFILE_PAGE_INFO(regmap_base_addr), j);
                ADI_APOLLO_ERROR_RETURN(err);

                if (word_sel == ADI_APOLLO_NCO_PROFILE_PHASE_INCREMENT) {
                    err = adi_apollo_hal_paged_bf_set(device, BF_HOP_PHASE_INC_INFO(regmap_base_addr), words[j]);
                    ADI_APOLLO_ERROR_RETURN(err);
                } else {
                    err = adi_apollo_hal_paged_bf_set(device, BF_HOP_PHASE_OFFSET_INFO(regmap_base_addr), words[j]);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
           }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_hop_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_fine_nco_hop_t *config)
{
    int32_t err;
    adi_apollo_blk_sel_t fnco;
    uint16_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for(i = 0; i < ADI_APOLLO_FNCO_NUM; i ++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_TRIG_HOP_SEL0_INFO(regmap_base_addr), config->nco_trig_hop_sel);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_FREQ_COHRNCE_TXRX_FINE_NCO_INFO(regmap_base_addr), config->phase_handling);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_AUTOFLIP_INCDIR_FTW_TXRX_FINE_NCO_INFO(regmap_base_addr),
                    (config->phase_inc_auto_mode == ADI_APOLLO_NCO_AUTO_HOP_FLIP) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_AUTOFLIP_INCDIR_PHOFST_TXRX_FINE_NCO_INFO(regmap_base_addr),
                    (config->phase_offset_auto_mode == ADI_APOLLO_NCO_AUTO_HOP_FLIP) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);


            err = adi_apollo_hal_bf_set(device, BF_HOP_HIGHLIMIT_PR0_INFO(regmap_base_addr), config->phase_inc_high_limit);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HOP_LOWLIMIT_PR0_INFO(regmap_base_addr), config->phase_inc_low_limit);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HOP_HIGHLIMIT_PR2_INFO(regmap_base_addr), config->phase_offset_high_limit);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HOP_LOWLIMIT_PR2_INFO(regmap_base_addr), config->phase_offset_low_limit);
            ADI_APOLLO_ERROR_RETURN(err);

            /* if phase_inc and phase_offset are both in auto_flip mode then auto inc/dec register is not programmed */
            if (config->phase_inc_auto_mode != ADI_APOLLO_NCO_AUTO_HOP_FLIP && config->phase_offset_auto_mode != ADI_APOLLO_NCO_AUTO_HOP_FLIP) {
                err = adi_apollo_hal_bf_set(device, BF_AUTO_INC_DECB_FTW_TXRX_FINE_NCO_INFO(regmap_base_addr), config->phase_inc_auto_mode);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_AUTO_INC_DECB_PHOFST_TXRX_FINE_NCO_INFO(regmap_base_addr), config->phase_offset_auto_mode);
                ADI_APOLLO_ERROR_RETURN(err);
            } else if (config->phase_inc_auto_mode != ADI_APOLLO_NCO_AUTO_HOP_FLIP) {
                err = adi_apollo_hal_bf_set(device, BF_AUTO_INC_DECB_FTW_TXRX_FINE_NCO_INFO(regmap_base_addr), config->phase_inc_auto_mode);
                ADI_APOLLO_ERROR_RETURN(err);
            } else if  (config->phase_offset_auto_mode != ADI_APOLLO_NCO_AUTO_HOP_FLIP) {
                err = adi_apollo_hal_bf_set(device, BF_AUTO_INC_DECB_PHOFST_TXRX_FINE_NCO_INFO(regmap_base_addr), config->phase_offset_auto_mode);
                ADI_APOLLO_ERROR_RETURN(err);
            }

            err = adi_apollo_hal_bf_set(device, BF_PROFILE_SEL_MODE_TXRX_FINE_NCO_INFO(regmap_base_addr), config->profile_sel_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TXRX_GPIOSHARE_INFO(regmap_base_addr),
                    (config->profile_sel_mode == ADI_APOLLO_NCO_CHAN_SEL_TRIG_GPIO || config->profile_sel_mode == ADI_APOLLO_NCO_CHAN_SEL_DIRECT_GPIO) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_CTRL_INIT_TXRX_FINE_NCO_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HOP_CTRL_INIT_TXRX_FINE_NCO_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_chan_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num, adi_apollo_fine_nco_chan_pgm_t *config)
{
    int32_t err;
    adi_apollo_blk_sel_t fnco;
    uint16_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for(i = 0; i < ADI_APOLLO_FNCO_NUM; i ++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_paged_bf_set(device, BF_HOP_PROFILE_PAGE_INFO(regmap_base_addr), profile_num);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_PHASE_INC_INFO(regmap_base_addr), config->drc_phase_inc);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HOP_PHASE_OFFSET_INFO(regmap_base_addr), config->drc_phase_offset);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_main_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_fine_nco_main_pgm_t *config)
{
    int32_t err;
    adi_apollo_blk_sel_t fnco;
    uint16_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for(i = 0; i < ADI_APOLLO_FNCO_NUM; i ++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_MAIN_PHASE_INC_INFO(regmap_base_addr), config->main_phase_inc);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_MAIN_PHASE_OFFSET_INFO(regmap_base_addr), config->main_phase_offset);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DRC0_PHASE_INC_FRAC_A_INFO(regmap_base_addr), config->drc_phase_inc_frac_a);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DRC0_PHASE_INC_FRAC_B_INFO(regmap_base_addr), config->drc_phase_inc_frac_b);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_fnco_pgm_t *config)
{
    int32_t err;
    adi_apollo_blk_sel_t fnco;
    uint16_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for(i = 0; i < ADI_APOLLO_FNCO_NUM; i ++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_DRC_IF_MODE_TXRX_FINE_NCO_INFO(regmap_base_addr), config->if_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DRC_MXR_SEL_INFO(regmap_base_addr), config->mixer_sel);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CMPLX_MXR_MULT_SCALE_EN_INFO(regmap_base_addr), config->cmplx_mxr_scale_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HOP_MODE_EN_INFO(regmap_base_addr), config->hop_mode_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FINE_DRC_EN0_INFO(regmap_base_addr), config->drc_en);

            err = adi_apollo_hal_bf_set(device, BF_FDRC_CLK_EN_INFO(regmap_base_addr),config->debug_drc_clkoff_n);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_PROFILE_SEL_MODE_TXRX_FINE_NCO_INFO(regmap_base_addr), config->profile_sel_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TXRX_GPIOSHARE_INFO(regmap_base_addr),
                    (config->profile_sel_mode == ADI_APOLLO_NCO_CHAN_SEL_TRIG_GPIO || config->profile_sel_mode == ADI_APOLLO_NCO_CHAN_SEL_DIRECT_GPIO) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_paged_bf_set(device, BF_HOP_PROFILE_PAGE_INFO(regmap_base_addr), config->profile_num);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_REGMAP_HOPPROF_INFO(regmap_base_addr), config->profile_num);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_MAIN_PHASE_INC_INFO(regmap_base_addr), config->main_phase_inc);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_MAIN_PHASE_OFFSET_INFO(regmap_base_addr), config->main_phase_offset);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DRC0_PHASE_INC_FRAC_A_INFO(regmap_base_addr), config->drc_phase_inc_frac_a);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DRC0_PHASE_INC_FRAC_B_INFO(regmap_base_addr), config->drc_phase_inc_frac_b);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_MIXER_TEST_MODE_VAL_TXRX_FINE_NCO_INFO(regmap_base_addr), config->dc_testmode_value);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_main_phase_inc_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint64_t ftw)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_MAIN_PHASE_INC_INFO(regmap_base_addr), ftw);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_main_phase_offset_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint64_t pow)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_MAIN_PHASE_OFFSET_INFO(regmap_base_addr), pow);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fnco, adi_apollo_fnco_inspect_t *fnco_inspect)
{
    int32_t err;
    uint16_t i, j;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fnco_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(fnco) != 1);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fnco);

    for(i = 0; i < ADI_APOLLO_FNCO_NUM; i ++) {
        if ((fnco & (ADI_APOLLO_FNCO_A0 << i)) > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_get(device, BF_DRC_IF_MODE_TXRX_FINE_NCO_INFO(regmap_base_addr), &(fnco_inspect->dp_cfg.nco_if_mode), sizeof(fnco_inspect->dp_cfg.nco_if_mode));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_DRC_MXR_SEL_INFO(regmap_base_addr), &(fnco_inspect->dp_cfg.drc_mxr_sel), sizeof(fnco_inspect->dp_cfg.drc_mxr_sel));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_CMPLX_MXR_MULT_SCALE_EN_INFO(regmap_base_addr), &(fnco_inspect->dp_cfg.cmplx_mxr_mult_scale_en), sizeof(fnco_inspect->dp_cfg.cmplx_mxr_mult_scale_en));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_HOP_MODE_EN_INFO(regmap_base_addr), &(fnco_inspect->hop_mode_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FINE_DRC_EN0_INFO(regmap_base_addr), (uint8_t *) &(fnco_inspect->dp_cfg.drc_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FDRC_CLK_EN_INFO(regmap_base_addr), 
                (uint8_t *) &(fnco_inspect->dp_cfg.debug_fdrc_clkoff_n), sizeof(fnco_inspect->dp_cfg.debug_fdrc_clkoff_n));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_TRIG_HOP_SEL0_INFO(regmap_base_addr), 
                (uint8_t *) &(fnco_inspect->dp_cfg.nco_trig_hop_sel), sizeof(fnco_inspect->dp_cfg.nco_trig_hop_sel));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_AUTO_INC_DECB_FTW_TXRX_FINE_NCO_INFO(regmap_base_addr), 
                (uint8_t *) &(fnco_inspect->dp_cfg.nco_auto_inc_dec_freq), sizeof(fnco_inspect->dp_cfg.nco_auto_inc_dec_freq));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_AUTO_INC_DECB_PHOFST_TXRX_FINE_NCO_INFO(regmap_base_addr), 
                (uint8_t *) &(fnco_inspect->dp_cfg.nco_auto_inc_dec_phase), sizeof(fnco_inspect->dp_cfg.nco_auto_inc_dec_phase));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_PROFILE_SEL_MODE_TXRX_FINE_NCO_INFO(regmap_base_addr), 
                (uint8_t *) &(fnco_inspect->dp_cfg.nco_profile_sel_mode), sizeof(fnco_inspect->dp_cfg.nco_profile_sel_mode));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_HOP_PROFILE_PAGE_INFO(regmap_base_addr), &(fnco_inspect->profile_num), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_REGMAP_HOPPROF_INFO(regmap_base_addr), &(fnco_inspect->regmap_hopprof), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_MAIN_PHASE_INC_INFO(regmap_base_addr), (uint8_t *) (&(fnco_inspect->dp_cfg.nco_phase_inc)), 8);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_MAIN_PHASE_OFFSET_INFO(regmap_base_addr), (uint8_t *) (&(fnco_inspect->dp_cfg.nco_phase_offset)), 8);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_DRC0_PHASE_INC_FRAC_A_INFO(regmap_base_addr), (uint8_t *) (&(fnco_inspect->dp_cfg.nco_phase_inc_frac_a)), 8);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_DRC0_PHASE_INC_FRAC_B_INFO(regmap_base_addr), (uint8_t *) (&(fnco_inspect->dp_cfg.nco_phase_inc_frac_b)), 8);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_MIXER_TEST_MODE_VAL_TXRX_FINE_NCO_INFO(regmap_base_addr), 
                (uint8_t *) (&(fnco_inspect->dp_cfg.dc_testmode_value)), sizeof(fnco_inspect->dp_cfg.dc_testmode_value));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_DRC0_ACTIVE_PHASE_INC_INFO(regmap_base_addr), (uint8_t *) (&(fnco_inspect->active_phase_inc)), 8);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_DRC0_ACTIVE_PHASE_OFFSET_INFO(regmap_base_addr), (uint8_t *) (&(fnco_inspect->active_phase_offset)), 8);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_DRC0_PHASE_DITHER_EN_INFO(regmap_base_addr), &(fnco_inspect->drc_phase_dither_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_DRC0_AMP_DITHER_EN_INFO(regmap_base_addr), &(fnco_inspect->drc_amp_dither_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            for (j = 0; j < ADI_APOLLO_FNCO_PROFILE_NUM; j++) {
                err = adi_apollo_hal_paged_reg_set(device, REG_HOP_PROFILE_PAGE_ADDR(regmap_base_addr), j);     // only bf in reg, use wr reg
                ADI_APOLLO_ERROR_RETURN(err);

                err = adi_apollo_hal_bf_get(device, BF_HOP_PHASE_INC_INFO(regmap_base_addr), (uint8_t *) &(fnco_inspect->dp_cfg.nco_phase_inc_words[j]), 4);
                ADI_APOLLO_ERROR_RETURN(err);
                
                err = adi_apollo_hal_bf_get(device, BF_HOP_PHASE_OFFSET_INFO(regmap_base_addr), (uint8_t *) &(fnco_inspect->dp_cfg.nco_phase_offset_words[j]), 4);
                ADI_APOLLO_ERROR_RETURN(err);
            }

            /* Return REG_HOP_PROFILE_PAGE_ADDR back to original state */
            err = adi_apollo_hal_paged_reg_set(device, REG_HOP_PROFILE_PAGE_ADDR(regmap_base_addr), fnco_inspect->profile_num);
            ADI_APOLLO_ERROR_RETURN(err);
            
            /* Only one inspect per call */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_nco_mixer_mode_e mode)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_DRC_IF_MODE_TXRX_FINE_NCO_INFO(regmap_base_addr), mode);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_mixer_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_drc_mixer_sel_e mixer)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_DRC_MXR_SEL_INFO(regmap_base_addr), mixer);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t enable)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FINE_DRC_EN0_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_test_mode_val_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint16_t val)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_MIXER_TEST_MODE_VAL_TXRX_FINE_NCO_INFO(regmap_base_addr), val);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_ftw_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num, uint8_t active_en, uint32_t ftw)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(profile_num > ADI_APOLLO_FNCO_PROFILE_NUM-1);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            /* Select profile to update */
            err = adi_apollo_hal_paged_bf_set(device, BF_HOP_PROFILE_PAGE_INFO(regmap_base_addr), profile_num);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_PHASE_INC_INFO(regmap_base_addr), ftw);
            ADI_APOLLO_ERROR_RETURN(err);

            if (active_en) {
                err = adi_apollo_hal_bf_set(device, BF_REGMAP_HOPPROF_INFO(regmap_base_addr), profile_num);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_pow_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num, uint8_t active_en, uint32_t pow)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(profile_num > ADI_APOLLO_FNCO_PROFILE_NUM-1);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            /* Select profile to update */
            err = adi_apollo_hal_paged_bf_set(device, BF_HOP_PROFILE_PAGE_INFO(regmap_base_addr), profile_num);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_PHASE_OFFSET_INFO(regmap_base_addr), pow);
            ADI_APOLLO_ERROR_RETURN(err);

            if (active_en) {
                err = adi_apollo_hal_bf_set(device, BF_REGMAP_HOPPROF_INFO(regmap_base_addr), profile_num);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_fnco_profile_sel_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_nco_profile_sel_mode_e profile_sel_mode)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_PROFILE_SEL_MODE_TXRX_FINE_NCO_INFO(regmap_base_addr), profile_sel_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TXRX_GPIOSHARE_INFO(regmap_base_addr),
                    (profile_sel_mode == ADI_APOLLO_NCO_CHAN_SEL_TRIG_GPIO || profile_sel_mode == ADI_APOLLO_NCO_CHAN_SEL_DIRECT_GPIO) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_fnco_active_profile_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t profile_num)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(profile_num > ADI_APOLLO_FNCO_PROFILE_NUM-1);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_REGMAP_HOPPROF_INFO(regmap_base_addr), profile_num);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_hop_enable(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, uint8_t hop_en)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_bf_set(device, BF_HOP_MODE_EN_INFO(regmap_base_addr), hop_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fnco_next_hop_num_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, int16_t hop_num_freq, int16_t hop_num_phase)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fnco;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(hop_num_freq > ADI_APOLLO_FNCO_PROFILE_NUM);
    ADI_APOLLO_INVALID_PARAM_RETURN(hop_num_phase > ADI_APOLLO_FNCO_PROFILE_NUM);
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fncos);

    for (i = 0; i < ADI_APOLLO_FNCO_NUM; i++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << i);
        if (fnco > 0) {
            regmap_base_addr = (terminal == ADI_APOLLO_RX) ? calc_rx_fnco_base(i) : calc_tx_fnco_base(i);

            err = adi_apollo_hal_reg_set(device, REG_NEXT_HOP_NUMBER_PR0_ADDR(regmap_base_addr), hop_num_freq);      /* Freq hop num */
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_reg_set(device, REG_NEXT_HOP_NUMBER_PR2_ADDR(regmap_base_addr), hop_num_phase);     /* Phase hop num */
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}


uint32_t calc_tx_fnco_base(int32_t idx)
{
    static const uint32_t tx_fnco_regmap[ADI_APOLLO_FNCO_NUM] = {
        TX_FINE_NCO0_TX_SLICE_0_TX_DIGITAL0, TX_FINE_NCO2_TX_SLICE_0_TX_DIGITAL0,
        TX_FINE_NCO0_TX_SLICE_1_TX_DIGITAL0, TX_FINE_NCO2_TX_SLICE_1_TX_DIGITAL0,
        TX_FINE_NCO1_TX_SLICE_0_TX_DIGITAL0, TX_FINE_NCO3_TX_SLICE_0_TX_DIGITAL0,
        TX_FINE_NCO1_TX_SLICE_1_TX_DIGITAL0, TX_FINE_NCO3_TX_SLICE_1_TX_DIGITAL0,

        TX_FINE_NCO0_TX_SLICE_0_TX_DIGITAL1, TX_FINE_NCO2_TX_SLICE_0_TX_DIGITAL1,
        TX_FINE_NCO0_TX_SLICE_1_TX_DIGITAL1, TX_FINE_NCO2_TX_SLICE_1_TX_DIGITAL1,
        TX_FINE_NCO1_TX_SLICE_0_TX_DIGITAL1, TX_FINE_NCO3_TX_SLICE_0_TX_DIGITAL1,
        TX_FINE_NCO1_TX_SLICE_1_TX_DIGITAL1, TX_FINE_NCO3_TX_SLICE_1_TX_DIGITAL1
    };

    return tx_fnco_regmap[idx];
}

uint32_t calc_rx_fnco_base(int32_t idx)
{
    static uint32_t rx_fine_nco_regmap[ADI_APOLLO_FNCO_NUM] = {
        RX_FINE_NCO0_RX_SLICE_0_RX_DIGITAL0, RX_FINE_NCO2_RX_SLICE_0_RX_DIGITAL0, RX_FINE_NCO0_RX_SLICE_1_RX_DIGITAL0, RX_FINE_NCO2_RX_SLICE_1_RX_DIGITAL0,
        RX_FINE_NCO1_RX_SLICE_0_RX_DIGITAL0, RX_FINE_NCO3_RX_SLICE_0_RX_DIGITAL0, RX_FINE_NCO1_RX_SLICE_1_RX_DIGITAL0, RX_FINE_NCO3_RX_SLICE_1_RX_DIGITAL0,
        RX_FINE_NCO0_RX_SLICE_0_RX_DIGITAL1, RX_FINE_NCO2_RX_SLICE_0_RX_DIGITAL1, RX_FINE_NCO0_RX_SLICE_1_RX_DIGITAL1, RX_FINE_NCO2_RX_SLICE_1_RX_DIGITAL1,
        RX_FINE_NCO1_RX_SLICE_0_RX_DIGITAL1, RX_FINE_NCO3_RX_SLICE_0_RX_DIGITAL1, RX_FINE_NCO1_RX_SLICE_1_RX_DIGITAL1, RX_FINE_NCO3_RX_SLICE_1_RX_DIGITAL1
    };
    return rx_fine_nco_regmap[idx];
}
/*! @} */
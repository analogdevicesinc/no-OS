/*!
 * \brief     APIs for FSRC
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FSRC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_fsrc.h"
#include "adi_apollo_fsrc_local.h"
#include "adi_apollo_dformat_local.h"
#include "adi_apollo_private_device.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_txrx_fsrc.h"
#include "adi_apollo_bf_jtx_dformat.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

#include "adi_utils.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_fsrc_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fsrcs, adi_apollo_fsrc_pgm_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t fsrc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_FSRC_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FSRC_BLK_SEL_MASK(fsrcs);

    /* Set the rate parameters */
    err = adi_apollo_fsrc_rate_set(device, terminal, fsrcs,
            config->fsrc_rate_int, config->fsrc_rate_frac_a, config->fsrc_rate_frac_b, config->gain_reduction);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Set the mode 1x */
    err = adi_apollo_fsrc_mode_1x_enable_set(device, terminal, fsrcs, config->fsrc_1x_mode);
    ADI_APOLLO_ERROR_RETURN(err);

    for(i = 0; i < ADI_APOLLO_FSRC_NUM; i += 2) {                 //Common bitfield for 0 & 1 fsrcs              
        fsrc = fsrcs & ((ADI_APOLLO_FSRC_A0 << i) | (ADI_APOLLO_FSRC_A0 << (i+1)));
        if (fsrc > 0) {
            regmap_base_addr = calc_fsrc_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_FRAC_DELAY_INFO(regmap_base_addr), config->sample_frac_delay);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PTR_SYNCRSTVAL_REGMAP_OVERWRITE_INFO(regmap_base_addr), config->ptr_overwrite);
            ADI_APOLLO_ERROR_RETURN(err);
            if (config->ptr_overwrite == 1) {
                err = adi_apollo_hal_bf_set(device, BF_PTR_SYNCRSTVAL_INFO(regmap_base_addr), config->ptr_syncrstval);
                ADI_APOLLO_ERROR_RETURN(err);
            }
            err = adi_apollo_hal_bf_set(device, BF_FSRC_DATA_MULT_DITHER_EN_INFO(regmap_base_addr), config->fsrc_data_mult_dither_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FSRC_DITHER_EN_INFO(regmap_base_addr), config->fsrc_dither_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FSRC_4T4R_SPLIT_INFO(regmap_base_addr), config->fsrc_4t4r_split);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FSRC_BYPASS_INFO(regmap_base_addr), config->fsrc_bypass);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }    
            
    /* Set the stream enables */
    for(i = 0; i < ADI_APOLLO_FSRC_NUM; i ++) {
        fsrc = fsrcs & (ADI_APOLLO_FSRC_A0 << i);
        if (fsrc > 0) {
            regmap_base_addr = calc_fsrc_base(terminal, i);
            
            if (i % 2 == 0) {
                err = adi_apollo_hal_bf_set(device, BF_FSRC_EN0_INFO(regmap_base_addr), config->fsrc_en);
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_set(device, BF_FSRC_EN1_INFO(regmap_base_addr), config->fsrc_en);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }
            
    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_fsrc_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t  fsrc, adi_apollo_fsrc_inspect_t *fsrc_inspect)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t fsrc_temp;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_FSRC_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(fsrc_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(fsrc)!= 1);
    ADI_APOLLO_FSRC_BLK_SEL_MASK(fsrc);


    for(i = 0; i < ADI_APOLLO_FSRC_NUM; i += 2) {                 //Common bitfield for 0 & 1 fsrcs              
        fsrc_temp = fsrc & ((ADI_APOLLO_FSRC_A0 << i) | (ADI_APOLLO_FSRC_A0 << (i+1)));
        if (fsrc_temp > 0) {
            regmap_base_addr = calc_fsrc_base(terminal, i);

            err = adi_apollo_hal_bf_get(device, BF_FSRC_RATE_INT_INFO(regmap_base_addr), (uint8_t*)  &(fsrc_inspect->dp_cfg.fsrc_rate_int), 8);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FSRC_RATE_FRAC_A_INFO(regmap_base_addr), (uint8_t*)  &(fsrc_inspect->dp_cfg.fsrc_rate_frac_a), 8);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FSRC_RATE_FRAC_B_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.fsrc_rate_frac_b), 8);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SAMPLE_FRAC_DELAY_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.fsrc_delay), 2);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_GAIN_REDUCTION_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.gain_reduction), 2);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_PTR_SYNCRSTVAL_REGMAP_OVERWRITE_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.ptr_overwrite), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            if (fsrc_inspect->dp_cfg.ptr_overwrite == 1){
                err = adi_apollo_hal_bf_get(device, BF_PTR_SYNCRSTVAL_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.ptr_syncrstval), 1);
                ADI_APOLLO_ERROR_RETURN(err);
            }
            err = adi_apollo_hal_bf_get(device, BF_FSRC_DATA_MULT_DITHER_EN_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.data_mult_dither_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FSRC_DITHER_EN_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.dither_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FSRC_4T4R_SPLIT_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.split_4t4r), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FSRC_BYPASS_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->fsrc_bypass), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FSRC_1X_MODE_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.mode_1x), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }    
            
    for(i = 0; i < ADI_APOLLO_FSRC_NUM; i ++) {
        fsrc_temp = fsrc & (ADI_APOLLO_FSRC_A0 << i);
        if (fsrc_temp > 0) {
            regmap_base_addr = calc_fsrc_base(terminal, i);
            
            if (i % 2 == 0) {
                err = adi_apollo_hal_bf_get(device, BF_FSRC_EN0_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.enable), 1);
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_get(device, BF_FSRC_EN1_INFO(regmap_base_addr), (uint8_t*) &(fsrc_inspect->dp_cfg.enable), 1);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }
            
    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_fsrc_rx_rm_fifo_pgm(adi_apollo_device_t *device, const uint16_t links, adi_apollo_rx_rm_fifo_pgm_t *config)
{ 
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_FSRC_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) {                                              /* Link 0 */
                err = adi_apollo_hal_bf_set(device, BF_LINK_DDC_DEC_0_INFO(regmap_base_addr), config->dcm_ratio);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_TOTAL_DEC_0_INFO(regmap_base_addr), config->total_dcm);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_INVALID_EN_0_INFO(regmap_base_addr), config->invalid_en);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_DDC_DITHER_EN_0_INFO(regmap_base_addr), config->dfor_ddc_dither_en);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_SAMPLE_REPEAT_EN_0_INFO(regmap_base_addr), config->sample_repeat_en);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_STARTUP_FRCE_INVALID_EN_INFO(regmap_base_addr, 0), config->startup_force_inv_en);
                ADI_APOLLO_ERROR_RETURN(err);
            
            } else {                                                                                     /* Link 1 */
                err = adi_apollo_hal_bf_set(device, BF_LINK_DDC_DEC_1_INFO(regmap_base_addr), config->dcm_ratio);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_TOTAL_DEC_1_INFO(regmap_base_addr), config->total_dcm);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_INVALID_EN_1_INFO(regmap_base_addr), config->invalid_en); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_DDC_DITHER_EN_1_INFO(regmap_base_addr), config->dfor_ddc_dither_en); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_SAMPLE_REPEAT_EN_1_INFO(regmap_base_addr), config->sample_repeat_en);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_STARTUP_FRCE_INVALID_EN_INFO(regmap_base_addr, 1), config->startup_force_inv_en);
                ADI_APOLLO_ERROR_RETURN(err);
            
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fsrc_rate_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fsrcs, 
    uint64_t rate_int, uint64_t rate_frac_a, uint64_t rate_frac_b, uint16_t gain_red)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t fsrc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_FSRC_LOCK);
    ADI_APOLLO_FSRC_BLK_SEL_MASK(fsrcs);

    for (i = 0; i < ADI_APOLLO_FSRC_NUM; i += 2) {
        fsrc = fsrcs & ((ADI_APOLLO_FSRC_A0 << i) | (ADI_APOLLO_FSRC_A0 << (i+1)));
        if (fsrc > 0) {
            regmap_base_addr = calc_fsrc_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_FSRC_RATE_INT_INFO(regmap_base_addr), rate_int);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FSRC_RATE_FRAC_A_INFO(regmap_base_addr), rate_frac_a);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FSRC_RATE_FRAC_B_INFO(regmap_base_addr), rate_frac_b);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_GAIN_REDUCTION_INFO(regmap_base_addr), gain_red);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fsrc_ratio_set(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fsrcs, uint32_t n, uint32_t m)
{
    int32_t err;
    uint64_t rate_int, rate_frac_a, rate_frac_b, gain_red;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_FSRC_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN( (!(m > 0)) || (n == m) || (n / m >= 2) );
    ADI_APOLLO_FSRC_BLK_SEL_MASK(fsrcs);

    err = adi_api_utils_ratio_decomposition(m, n, 48, &rate_int, &rate_frac_a, &rate_frac_b);
    ADI_APOLLO_ERROR_RETURN(err);

    gain_red = adi_api_utils_div_floor_u64(4096ull * (uint64_t)m, (uint64_t)n);

    return adi_apollo_fsrc_rate_set(device, terminal, fsrcs, rate_int, rate_frac_a, rate_frac_b, gain_red);
}


int32_t adi_apollo_fsrc_mode_1x_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fsrcs, uint8_t enable)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t fsrc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_FSRC_LOCK);
    ADI_APOLLO_FSRC_BLK_SEL_MASK(fsrcs);

    for (i = 0; i < ADI_APOLLO_FSRC_NUM; i += 2) {             
        fsrc = fsrcs & ((ADI_APOLLO_FSRC_A0 << i) | (ADI_APOLLO_FSRC_A0 << (i+1)));
        if (fsrc > 0) {
            regmap_base_addr = calc_fsrc_base(terminal, i);
            err = adi_apollo_hal_bf_set(device, BF_FSRC_1X_MODE_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_fsrc_base(adi_apollo_terminal_e terminal, int32_t fsrc_index)
{
    static uint32_t rx_fsrc_regmap[ADI_APOLLO_FSRC_NUM] = {
        RX_FSRC_RX_SLICE_0_RX_DIGITAL0, RX_FSRC_RX_SLICE_0_RX_DIGITAL0,
        RX_FSRC_RX_SLICE_0_RX_DIGITAL1, RX_FSRC_RX_SLICE_0_RX_DIGITAL1
    };
    static uint32_t tx_fsrc_regmap[ADI_APOLLO_FSRC_NUM] = {
        TX_FSRC_TX_SLICE_0_TX_DIGITAL0, TX_FSRC_TX_SLICE_0_TX_DIGITAL0,
        TX_FSRC_TX_SLICE_0_TX_DIGITAL1, TX_FSRC_TX_SLICE_0_TX_DIGITAL1
    };
    if (terminal == ADI_APOLLO_RX)
        return rx_fsrc_regmap[fsrc_index];
    else
        return tx_fsrc_regmap[fsrc_index];
}
/*! @} */
/*!
 * \brief     APIs for CFIR
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CFIR
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_cfir.h"
#include "adi_apollo_cfir_local.h"
#include "adi_apollo_trigts_types.h"
#include "adi_apollo_trigts_local.h"
#include "adi_apollo_private_device.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_txrx_cfir_top.h"
#include "adi_apollo_bf_txrx_cfir_coeff.h"
#include "adi_apollo_bf_txrx_trigger_ts.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_cfir_pgm(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_cfir_pgm_t* config)
{
    int32_t err;
    uint16_t cfir;
    uint8_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_CFIR_BYPASS_INFO(regmap_base_addr), config->cfir_bypass);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CFIR_SPARSE_FILT_EN_INFO(regmap_base_addr), config->cfir_sparse_filt_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CFIR_32TAPS_EN_INFO(regmap_base_addr), config->cfir_32taps_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_COEFF_TRANSFER_INFO(regmap_base_addr), config->cfir_coeff_transfer);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfir_coeff_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint16_t cfir_coeff_i[], uint16_t cfir_coeff_q[], uint32_t len)
{
    int32_t err;
    uint16_t cfir;
    uint16_t cfir_profile;
    uint16_t cfir_dp;
    uint8_t i, j, k, dp;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(cfir_coeff_i);
    ADI_APOLLO_NULL_POINTER_RETURN(cfir_coeff_q);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_coeff_base(terminal, i);

            /* Set reg_map base address for subsequent paged writes */
            adi_apollo_hal_paged_base_addr_set(device, regmap_base_addr);

            for (dp = 0; dp < ADI_APOLLO_CFIR_PROFILE_NUM; dp++) {
                cfir_profile = cfir_profiles & (ADI_APOLLO_CFIR_DP_0 << dp);
                if (cfir_profile > 0) {

                    for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                        cfir_dp = cfir_dps & (ADI_APOLLO_CFIR_DP_0 << j);
                        if (cfir_dp > 0) {

                            if (dp == 0) { /* profile 1 */
                                switch (j) {
                                case 0:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_0_1_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_0_1_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;

                                case 1:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_1_1_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_1_1_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;

                                case 2:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_2_1_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_2_1_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;
                                case 3:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_3_1_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_3_1_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;
                                }
                            } else { /* profile #2 */
                                switch (j) {
                                case 0:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_0_2_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_0_2_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;

                                case 1:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_1_2_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_1_2_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;

                                case 2:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_2_2_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_2_2_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;

                                case 3:
                                    for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                        err = adi_apollo_hal_paged_bf_set(device, BF_I_COEFF_3_2_INFO(regmap_base_addr, k), cfir_coeff_i[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_paged_bf_set(device, BF_Q_COEFF_3_2_INFO(regmap_base_addr, k), cfir_coeff_q[k]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }


        }

    }    

    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_cfir_scalar_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint16_t scalar_i, uint16_t scalar_q)
{
    int32_t err;
    uint16_t cfir;
    uint16_t cfir_profile;
    uint16_t cfir_dp;
    uint8_t i, j, dp;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            for (dp = 0; dp < ADI_APOLLO_CFIR_PROFILE_NUM; dp++) {
                cfir_profile = cfir_profiles & (ADI_APOLLO_CFIR_DP_0 << dp);
                if (cfir_profile > 0) {
                
                    for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                        cfir_dp = cfir_dps & (ADI_APOLLO_CFIR_DP_0 << j);
                        if (cfir_dp > 0) {

                            if (dp == 0) {      /* profile 1 */
                                switch (j) {
                                    case 0:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 0), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 0), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 1:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 1), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 1), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 2:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 2), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 2), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 3:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 3), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 3), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                        }
                            } else {              /* profile #2 */
                                switch (j) {
                                    case 0:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 0), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 0), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 1:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 1), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 1), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 2:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 2), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 2), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 3:
                                        err = adi_apollo_hal_bf_set(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 3), scalar_i);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 3), scalar_q);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                        }
                            }
                        }
                    }
                }
            }


        }

    }    

    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_cfir_gain_pgm(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint8_t gain)
{
    int32_t err;
    adi_apollo_blk_sel_t cfir;
    uint16_t cfir_profile;
    uint16_t cfir_dp;
    uint8_t i, j, dp;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            for (dp = 0; dp < ADI_APOLLO_CFIR_PROFILE_NUM; dp++) {
                cfir_profile = cfir_profiles & (ADI_APOLLO_CFIR_DP_0 << dp);
                if (cfir_profile > 0) {

                    for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                        cfir_dp = cfir_dps & (ADI_APOLLO_CFIR_DP_0 << j);
                        if (cfir_dp > 0) {

                            if (dp == 0) {      /* profile 1 */
                                switch (j) {
                                    case 0:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN0_1_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 1:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN1_1_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 2:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN2_1_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 3:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN3_1_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                }
                            } else {              /* profile #2 */
                                switch (j) {
                                    case 0:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN0_2_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 1:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN1_2_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 2:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN2_2_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 3:
                                        err = adi_apollo_hal_bf_set(device, BF_CFIR_GAIN3_2_INFO(regmap_base_addr), gain);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                }
                            }
                        }
                    }
                }
            }


        }

    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfir_sparse_coeff_sel_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint16_t cfir_coeff_sel[], uint32_t len)
{
    int32_t err;
    adi_apollo_blk_sel_t cfir;
    uint16_t cfir_profile;
    uint16_t cfir_dp;
    uint8_t i, j, k, dp;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(cfir_coeff_sel);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {

            regmap_base_addr = calc_cfir_coeff_base(terminal, i);

            for (dp = 0; dp < ADI_APOLLO_CFIR_PROFILE_NUM; dp++) {
                cfir_profile = cfir_profiles & (ADI_APOLLO_CFIR_DP_0 << dp);
                if (cfir_profile > 0) {
                
                    for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                        cfir_dp = cfir_dps & (ADI_APOLLO_CFIR_DP_0 << j);
                        if (cfir_dp > 0) {

                            if (dp == 0) {        /* profile 1 */
                                switch (j) {
                                    case 0:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_0_1_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                    case 1:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_1_1_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                    case 2:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_2_1_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                    case 3:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_3_1_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                }
                            } else {                /* profile #2 */
                                switch (j) {
                                    case 0:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_0_2_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                    case 1:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_1_2_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                    case 2:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_2_2_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                    case 3:
                                        for(k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                            err = adi_apollo_hal_bf_set(device, BF_COEFF_SEL_3_2_INFO(regmap_base_addr, k), cfir_coeff_sel[k]);
                                            ADI_APOLLO_ERROR_RETURN(err);
                                        }
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }

    }    

    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_cfir_sparse_mem_sel_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_profiles, uint8_t cfir_dps, uint8_t cfir_mem_sel[], uint32_t len)
{
    int32_t err;
    adi_apollo_blk_sel_t cfir;
    uint16_t cfir_profile;
    uint16_t cfir_dp;
    uint8_t i, j, dp;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(cfir_mem_sel);
    ADI_APOLLO_INVALID_PARAM_RETURN(len != ADI_APOLLO_CFIR_MEM_SEL_NUM)
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            for (dp = 0; dp < ADI_APOLLO_CFIR_PROFILE_NUM; dp++) {
                cfir_profile = cfir_profiles & (ADI_APOLLO_CFIR_DP_0 << dp);
                if (cfir_profile > 0) {

                    for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                        cfir_dp = cfir_dps & (ADI_APOLLO_CFIR_DP_0 << j);
                        if (cfir_dp > 0) {

                            if (dp == 0) {      /* profile 1 */
                                switch (j) {
                                    case 0:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH0_MEM_SEL0_1_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH0_MEM_SEL1_1_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH0_MEM_SEL2_1_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 1:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH1_MEM_SEL0_1_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH1_MEM_SEL1_1_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH1_MEM_SEL2_1_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 2:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH2_MEM_SEL0_1_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH2_MEM_SEL1_1_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH2_MEM_SEL2_1_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 3:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH3_MEM_SEL0_1_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH3_MEM_SEL1_1_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH3_MEM_SEL2_1_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                }
                            } else {              /* profile #2 */
                                switch (j) {
                                    case 0:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH0_MEM_SEL0_2_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH0_MEM_SEL1_2_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH0_MEM_SEL2_2_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 1:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH1_MEM_SEL0_2_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH1_MEM_SEL1_2_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH1_MEM_SEL2_2_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 2:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH2_MEM_SEL0_2_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH2_MEM_SEL1_2_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH2_MEM_SEL2_2_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                    case 3:
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH3_MEM_SEL0_2_INFO(regmap_base_addr), cfir_mem_sel[0]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH3_MEM_SEL1_2_INFO(regmap_base_addr), cfir_mem_sel[1]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        err = adi_apollo_hal_bf_set(device, BF_DPATH3_MEM_SEL2_2_INFO(regmap_base_addr), cfir_mem_sel[2]);
                                        ADI_APOLLO_ERROR_RETURN(err);
                                        break;
                                }
                            }
                        }
                    }
                }
            }


        }

    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfir_profile_sel(adi_apollo_device_t* device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_dps, uint8_t coeff_profile_sel)
{
    int32_t err;
    adi_apollo_blk_sel_t cfir;
    uint16_t cfir_dp;
    uint8_t i, j;
    uint32_t regmap_base_addr = 0;
    uint8_t profile_sel_bf;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(coeff_profile_sel) != 1);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    /* single bit is used to select between profile 0 or 1 */
    profile_sel_bf = coeff_profile_sel == ADI_APOLLO_CFIR_PROFILE_0 ? 0 : 1;

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                cfir_dp = cfir_dps & (ADI_APOLLO_CFIR_DP_0 << j);
                if (cfir_dp > 0) {

                    switch (j) {
                        case 0:
                            err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL0_INFO(regmap_base_addr), profile_sel_bf);
                            ADI_APOLLO_ERROR_RETURN(err);
                            break;
                        case 1:
                            err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL1_INFO(regmap_base_addr), profile_sel_bf);
                            ADI_APOLLO_ERROR_RETURN(err);
                            break;
                        case 2:
                            err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL2_INFO(regmap_base_addr), profile_sel_bf);
                            ADI_APOLLO_ERROR_RETURN(err);
                            break;
                        case 3:
                            err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL3_INFO(regmap_base_addr), profile_sel_bf);
                            ADI_APOLLO_ERROR_RETURN(err);
                            break;
                    }
                }
            }

            /* Generate 0->1 pulse to transfer selected back to active */
            err = adi_apollo_hal_bf_set(device, BF_COEFF_TRANSFER_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_COEFF_TRANSFER_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }

    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfir_mode_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_cfir_enable_sel_e mode)
{
    int32_t err;
    adi_apollo_blk_sel_t cfir;
    uint8_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_CFIR_BYPASS_INFO(regmap_base_addr), (mode == ADI_APOLLO_CFIR_BYPASS) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfir_profile_sel_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, const adi_apollo_cfir_profile_sel_mode_e prof_sel_mode)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cfir;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(prof_sel_mode > (ADI_APOLLO_CFIR_CHAN_SEL_NUM - 1));
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    const uint8_t bank_regs[ADI_APOLLO_CFIR_CHAN_SEL_NUM][3] = {
        {0, 0}, // trigger_en, gpio_profile_en
        {0, 1},
        {1, 0},
        {1, 1}};

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_TRIGGER_EN_TXRX_CFIR_TOP_INFO(regmap_base_addr), bank_regs[prof_sel_mode][0]);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_GPIO_PROFILE_EN_INFO(regmap_base_addr), bank_regs[prof_sel_mode][1]);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfir_next_hop_num_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, uint8_t cfir_dps, int16_t hop_num)
{
    int32_t err;
    uint8_t i, j;
    adi_apollo_blk_sel_t cfir;
    uint16_t cfir_dp;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(hop_num > (ADI_APOLLO_CFIR_PROFILE_NUM - 1));
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);
        
            for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                cfir_dp = cfir_dps & (ADI_APOLLO_CFIR_DP_0 << j);
            
                if (cfir_dp > 0) {

                    switch (j) {
                    case 0:
                        err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL0_INFO(regmap_base_addr), hop_num);
                        ADI_APOLLO_ERROR_RETURN(err);
                        break;
                    case 1:
                        err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL1_INFO(regmap_base_addr), hop_num);
                        ADI_APOLLO_ERROR_RETURN(err);
                        break;
                    case 2:
                        err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL2_INFO(regmap_base_addr), hop_num);
                        ADI_APOLLO_ERROR_RETURN(err);
                        break;
                    case 3:
                        err = adi_apollo_hal_bf_set(device, BF_COEFF_PROFILE_SEL3_INFO(regmap_base_addr), hop_num);
                        ADI_APOLLO_ERROR_RETURN(err);
                        break;
                    }
                }
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfir_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_cfir_inspect_t *cfir_inspect)
{
    int32_t err;
    adi_apollo_blk_sel_t cfir;
    uint8_t i, j, k, dp, is_bypassed;
    uint32_t regmap_base_addr = 0, regmap_coeff_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_CFIR_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(cfirs)!= 1);
    ADI_APOLLO_NULL_POINTER_RETURN(cfir_inspect);
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for (i = 0; i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            regmap_base_addr = calc_cfir_base(terminal, i);

            /* cfir_pgm */
            err = adi_apollo_hal_bf_get(device, BF_CFIR_BYPASS_INFO(regmap_base_addr), &is_bypassed, 1);
            ADI_APOLLO_ERROR_RETURN(err);
            
            cfir_inspect->dp_cfg.enable = !is_bypassed;      // not-bypassed means cfir is enabled

            err = adi_apollo_hal_bf_get(device, BF_CFIR_SPARSE_FILT_EN_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.sparse_mode), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_CFIR_32TAPS_EN_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_mode), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_COEFF_TRANSFER_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->cfir_coeff_transfer), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            regmap_coeff_base_addr = calc_cfir_coeff_base(terminal, i);
            for (dp = 0; dp < ADI_APOLLO_CFIR_PROFILE_NUM; dp++) {                
                for (j = 0; j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                    if (dp == 0) {        /* profile 1 */
                        switch (j) {
                            case 0:
                                
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    /* coeff_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_0_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_0_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_0_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }

                                /* scalar_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 0), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 0), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN0_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH0_MEM_SEL0_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH0_MEM_SEL1_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH0_MEM_SEL2_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;

                            case 1:
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_1_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_1_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_1_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 1), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 1), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN1_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH1_MEM_SEL0_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH1_MEM_SEL1_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH1_MEM_SEL2_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;

                            case 2:
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_2_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_2_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_2_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 2), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 2), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN2_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH2_MEM_SEL0_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH2_MEM_SEL1_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH2_MEM_SEL2_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;
                            case 3:
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_3_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_3_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_3_1_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 3), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_1_INFO(regmap_base_addr, 3), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN3_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH3_MEM_SEL0_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH3_MEM_SEL1_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH3_MEM_SEL2_1_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;
                        }
                    } else {                    /* profile #2 */
                        switch (j) {
                            case 0:
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_0_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_0_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_0_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 0), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 0), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN0_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH0_MEM_SEL0_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH0_MEM_SEL1_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH0_MEM_SEL2_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;

                            case 1:
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_1_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_1_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_1_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 1), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 1), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN1_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH1_MEM_SEL0_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH1_MEM_SEL1_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH1_MEM_SEL2_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;

                            case 2:
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_2_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_2_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_2_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 2), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 2), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN2_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH2_MEM_SEL0_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH2_MEM_SEL1_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH2_MEM_SEL2_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;

                            case 3:
                                for (k = 0; k < ADI_APOLLO_CFIR_COEFF_NUM; k++) {
                                    err = adi_apollo_hal_bf_get(device, BF_I_COEFF_3_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_i[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                    err = adi_apollo_hal_bf_get(device, BF_Q_COEFF_3_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->dp_cfg.coeffs_q[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);

                                    /* sparse_coeff_sel_pgm */
                                    err = adi_apollo_hal_bf_get(device, BF_COEFF_SEL_3_2_INFO(regmap_coeff_base_addr, k), (uint8_t*)&(cfir_inspect->sparse_coeff[dp][j][k]), 2);
                                    ADI_APOLLO_ERROR_RETURN(err);
                                }
                                err = adi_apollo_hal_bf_get(device, BF_I_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 3), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_i[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_Q_COMPLEX_SCALAR_2_INFO(regmap_base_addr, 3), (uint8_t*)&(cfir_inspect->dp_cfg.scalar_q[dp][j]), 2);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* gain_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_CFIR_GAIN3_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->dp_cfg.cfir_gain_dB[dp][j]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);

                                /* sparse_mem_sel_pgm */
                                err = adi_apollo_hal_bf_get(device, BF_DPATH3_MEM_SEL0_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][0]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH3_MEM_SEL1_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][1]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                err = adi_apollo_hal_bf_get(device, BF_DPATH3_MEM_SEL2_2_INFO(regmap_base_addr), (uint8_t*)&(cfir_inspect->sparse_mem[dp][j][2]), 1);
                                ADI_APOLLO_ERROR_RETURN(err);
                                break;
                        }
                    }   
                }   
            }
        }
    }



    return API_CMS_ERROR_OK; 
}

uint32_t calc_cfir_base(adi_apollo_terminal_e terminal, int32_t cfir_index)
{
    static uint32_t rx_cfir_regmap[ADI_APOLLO_CFIR_NUM] = {
        RX_CFIR_TOP_RX_SLICE_0_RX_DIGITAL0,
        RX_CFIR_TOP_RX_SLICE_1_RX_DIGITAL0,
        RX_CFIR_TOP_RX_SLICE_0_RX_DIGITAL1,
        RX_CFIR_TOP_RX_SLICE_1_RX_DIGITAL1
    };
    static uint32_t tx_cfir_regmap[ADI_APOLLO_CFIR_NUM] = {
        TX_CFIR_TOP_TX_SLICE_0_TX_DIGITAL0,
        TX_CFIR_TOP_TX_SLICE_1_TX_DIGITAL0,
        TX_CFIR_TOP_TX_SLICE_0_TX_DIGITAL1,
        TX_CFIR_TOP_TX_SLICE_1_TX_DIGITAL1,
    };
    if (terminal == ADI_APOLLO_RX)
        return rx_cfir_regmap[cfir_index];
    else
        return tx_cfir_regmap[cfir_index];
}

uint32_t calc_cfir_coeff_base(adi_apollo_terminal_e terminal, int32_t cfir_index)
{
    static uint32_t rx_cfir_coeff_regmap[ADI_APOLLO_CFIR_NUM] = {
        RX_CFIR_COEFF_RX_SLICE_0_RX_DIGITAL0,
        RX_CFIR_COEFF_RX_SLICE_1_RX_DIGITAL0,
        RX_CFIR_COEFF_RX_SLICE_0_RX_DIGITAL1,
        RX_CFIR_COEFF_RX_SLICE_1_RX_DIGITAL1,
    };
    static uint32_t tx_cfir_coeff_regmap[ADI_APOLLO_CFIR_NUM] = {
        TX_CFIR_COEFF_TX_SLICE_0_TX_DIGITAL0,
        TX_CFIR_COEFF_TX_SLICE_1_TX_DIGITAL0,
        TX_CFIR_COEFF_TX_SLICE_0_TX_DIGITAL1,
        TX_CFIR_COEFF_TX_SLICE_1_TX_DIGITAL1,
    };
    if (terminal == ADI_APOLLO_RX)
        return rx_cfir_coeff_regmap[cfir_index];
    else
        return tx_cfir_coeff_regmap[cfir_index];
}
/*! @} */
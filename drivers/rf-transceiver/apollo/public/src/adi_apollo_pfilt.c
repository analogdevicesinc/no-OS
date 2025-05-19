/*!
 * \brief     APIs for PFILT
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PFILT
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_pfilt.h"
#include "adi_apollo_pfilt_local.h"
#include "adi_apollo_private_device.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_txrx_pfilt_top.h"
#include "adi_apollo_bf_txrx_pfilt_coeff.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

static int32_t write_bank(adi_apollo_device_t *device, uint32_t bank_num, uint32_t regmap_base_addr, int first_idx, int16_t ceoffs[], uint32_t n_taps);

typedef struct
{
    uint16_t n_taps;
    uint16_t idx[ADI_APOLLO_PFILT_STREAMS_NUM];
    uint16_t idx_fmtx[ADI_APOLLO_PFILT_STREAMS_NUM];
} stream_mode_loc_map_t;

static const stream_mode_loc_map_t stream_mode_loc_map[] = {
    {32, {0, 16}, {0, 0}},      // N tap
    {16, {0, 16}, {0, 0}},      // N/2 tap
    { 8, {0, 16}, {0, 0}},      // N/4 tap
    {16, {0, 16}, {0, 0}},      // Half-Matrix
    { 8, {0, 16}, {8, 24}}      // Full-Matrix
};

/* Map pfilt mode enum to coeff layout for that mode */
static const int pfilt_mode_to_idx[8] = {
    0, // bypass, n/a
    2, // n/4 tap
    1, // n/2 tap
    0, // n/a
    4, // full matrix
    0, // n/a
    3, // half-cmplx
    0  // n tap
};

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_pfilt_mode_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, adi_apollo_pfilt_mode_pgm_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i += 2) {                               //Common bitfield for 0 & 1 pfilts
        pfilt = pfilts & ((ADI_APOLLO_PFILT_A0 << i) | (ADI_APOLLO_PFILT_A0 << (i+1)));
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_QUAD_MODE_INFO(regmap_base_addr), config->dq_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_REAL_DATA_INFO(regmap_base_addr), config->data);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_MODE_SWITCH_INFO(regmap_base_addr), config->mode_switch);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_ADD_SUB_SEL_INFO(regmap_base_addr), config->add_sub_sel);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_PFIR_I_MODE_INFO(regmap_base_addr, i%2), config->pfir_i_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PFIR_Q_MODE_INFO(regmap_base_addr, i%2), config->pfir_q_mode);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_gain_dly_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks, adi_apollo_pfilt_gain_dly_pgm_t *config)
{
    int32_t err;
    uint8_t i, j, pfilt_bank, pfilt_local;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            for(j = 0; j < ADI_APOLLO_PFILT_BANK_NUM; j ++) {
                pfilt_bank = pfilt_banks & (ADI_APOLLO_PFILT_BANK0 <<j);
                if (pfilt_bank > 0) {
                    pfilt_local = (((i%ADI_APOLLO_PFILT_PER_SIDE)*ADI_APOLLO_PFILT_BANK_NUM) + j);

                    err = adi_apollo_hal_bf_set(device, BF_PFIR_IX_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_ix_gain);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_PFIR_IY_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_iy_gain);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_PFIR_QX_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_qx_gain);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_PFIR_QY_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_qy_gain);
                    ADI_APOLLO_ERROR_RETURN(err);

                    err = adi_apollo_hal_bf_set(device, BF_PFIR_IX_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_ix_scalar_gain);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_PFIR_IY_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_iy_scalar_gain);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_PFIR_QX_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_qx_scalar_gain);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_PFIR_QY_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), config->pfir_qy_scalar_gain);
                    ADI_APOLLO_ERROR_RETURN(err);

                    err = adi_apollo_hal_bf_set(device, BF_HC_PROG_DELAY_INFO(regmap_base_addr, pfilt_local), config->hc_delay);
                    ADI_APOLLO_ERROR_RETURN(err);
               }
            }
        }
    }
    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_pfilt_coeff_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks, uint16_t pfilt_coeff[], uint32_t length)
{
    int32_t err;
    uint8_t i, j, k, pfilt_bank;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(length > ADI_APOLLO_PFILT_COEFF_NUM);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            for(j = 0; j < ADI_APOLLO_PFILT_BANK_NUM; j ++) {
                pfilt_bank = pfilt_banks & (ADI_APOLLO_PFILT_BANK0 <<j);
                if (pfilt_bank > 0) {
                    regmap_base_addr = calc_pfilt_coeff_base(terminal, (i*ADI_APOLLO_PFILT_BANK_NUM)+j);
                    if (j % ADI_APOLLO_PFILT_BANKS_PER_REGMAP == 0) {               //BANK0 and BANK2
                        for(k = 0; k < length; k++) {
                            err = adi_apollo_hal_bf_set(device, BF_COEFF_0_INFO(regmap_base_addr, k), pfilt_coeff[k]);
                            ADI_APOLLO_ERROR_RETURN(err);
                        }
                    }
                    else {                                                          //BANK1 and BANK3
                        for(k = 0; k < length; k++) {
                            err = adi_apollo_hal_bf_set(device, BF_COEFF_1_INFO(regmap_base_addr, k), pfilt_coeff[k]);
                            ADI_APOLLO_ERROR_RETURN(err);
                        }
                    }
               }
            }
        }
    }
    return API_CMS_ERROR_OK;

}

int32_t adi_apollo_pfilt_coeff_ntap_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks,
                                        uint16_t streams, adi_apollo_pfilt_mode_e mode, int16_t coeffs[], uint32_t coeffs_len)
{
    int32_t err;
    uint8_t i, j, s, pfilt_bank;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;
    stream_mode_loc_map_t stream_ceoff_loc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(coeffs);
    ADI_APOLLO_INVALID_PARAM_RETURN(terminal > ADI_APOLLO_TX);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilts & ~ADI_APOLLO_PFILT_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilt_banks & ~ADI_APOLLO_PFILT_BANK_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(streams & ~ADI_APOLLO_PFILT_STREAM_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(!((mode == ADI_APOLLO_PFILT_MODE_N_REAL) || (mode == ADI_APOLLO_PFILT_MODE_N_DIV_BY_2_REAL) || (mode == ADI_APOLLO_PFILT_MODE_N_DIV_BY_4_REAL)));
    ADI_APOLLO_INVALID_PARAM_RETURN((mode == ADI_APOLLO_PFILT_MODE_N_REAL) && ((coeffs_len != 32) || (adi_api_utils_num_selected(streams) != 1)) );
    ADI_APOLLO_INVALID_PARAM_RETURN((mode == ADI_APOLLO_PFILT_MODE_N_DIV_BY_2_REAL) && (coeffs_len != 16));
    ADI_APOLLO_INVALID_PARAM_RETURN((mode == ADI_APOLLO_PFILT_MODE_N_DIV_BY_4_REAL) && (coeffs_len != 8));
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    /* Get coeff bank location info for given mode */
    stream_ceoff_loc = stream_mode_loc_map[pfilt_mode_to_idx[mode]];
    
    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            for (j = 0; j < ADI_APOLLO_PFILT_BANK_NUM; j++) {
                pfilt_bank = pfilt_banks & (ADI_APOLLO_PFILT_BANK0 << j);
                if (pfilt_bank > 0) {
                    regmap_base_addr = calc_pfilt_coeff_base(terminal, (i * ADI_APOLLO_PFILT_BANK_NUM) + j);
                    for (s = 0; s < ADI_APOLLO_PFILT_STREAMS_NUM; s++) {
                        if (streams & (ADI_APOLLO_PFILT_STREAM_0 << s)) {
                            err = write_bank(device, j, regmap_base_addr, stream_ceoff_loc.idx[s], coeffs, stream_ceoff_loc.n_taps);
                            ADI_APOLLO_ERROR_RETURN(err);
                        }
                    }
                    
                }
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_coeff_half_complex_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks,
                                               int16_t coeffs0[], int16_t coeffs1[], uint32_t coeffs_len)
{
    int32_t err;
    uint8_t i, j, pfilt_bank;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;
    stream_mode_loc_map_t stream_ceoff_loc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(coeffs0);
    ADI_APOLLO_NULL_POINTER_RETURN(coeffs1);
    ADI_APOLLO_INVALID_PARAM_RETURN(terminal > ADI_APOLLO_TX);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilts & ~ADI_APOLLO_PFILT_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilt_banks & ~ADI_APOLLO_PFILT_BANK_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(coeffs_len != 16);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    /* Get coeff bank location info for half-complex */
    stream_ceoff_loc = stream_mode_loc_map[pfilt_mode_to_idx[ADI_APOLLO_PFILT_MODE_HALF_COMPLEX]];

    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            for (j = 0; j < ADI_APOLLO_PFILT_BANK_NUM; j++) {
                pfilt_bank = pfilt_banks & (ADI_APOLLO_PFILT_BANK0 << j);
                if (pfilt_bank > 0) {
                    regmap_base_addr = calc_pfilt_coeff_base(terminal, (i * ADI_APOLLO_PFILT_BANK_NUM) + j);
                    err = write_bank(device, j, regmap_base_addr, stream_ceoff_loc.idx[0], coeffs0, stream_ceoff_loc.n_taps);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = write_bank(device, j, regmap_base_addr, stream_ceoff_loc.idx[1], coeffs1, stream_ceoff_loc.n_taps);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
            }
        }
    }
    
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_coeff_full_matrix_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks,
                                               int16_t coeffs0[], int16_t coeffs1[], int16_t coeffs2[], int16_t coeffs3[], uint32_t coeffs_len)
{
    int32_t err;
    uint8_t i, j, pfilt_bank;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;
    stream_mode_loc_map_t stream_ceoff_loc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(coeffs0);
    ADI_APOLLO_NULL_POINTER_RETURN(coeffs1);
    ADI_APOLLO_NULL_POINTER_RETURN(coeffs2);
    ADI_APOLLO_NULL_POINTER_RETURN(coeffs3);
    ADI_APOLLO_INVALID_PARAM_RETURN(terminal > ADI_APOLLO_TX);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilts & ~ADI_APOLLO_PFILT_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilt_banks & ~ADI_APOLLO_PFILT_BANK_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(coeffs_len != 8);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    /* Get coeff bank locations info for full matrix */
    stream_ceoff_loc = stream_mode_loc_map[pfilt_mode_to_idx[ADI_APOLLO_PFILT_MODE_MATRIX]];

    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            for (j = 0; j < ADI_APOLLO_PFILT_BANK_NUM; j++) {
                pfilt_bank = pfilt_banks & (ADI_APOLLO_PFILT_BANK0 << j);
                if (pfilt_bank > 0) {
                    regmap_base_addr = calc_pfilt_coeff_base(terminal, (i * ADI_APOLLO_PFILT_BANK_NUM) + j);
                    err = write_bank(device, j, regmap_base_addr, stream_ceoff_loc.idx[0], coeffs0, stream_ceoff_loc.n_taps);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = write_bank(device, j, regmap_base_addr, stream_ceoff_loc.idx[1], coeffs1, stream_ceoff_loc.n_taps);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = write_bank(device, j, regmap_base_addr, stream_ceoff_loc.idx_fmtx[0], coeffs2, stream_ceoff_loc.n_taps);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = write_bank(device, j, regmap_base_addr, stream_ceoff_loc.idx_fmtx[1], coeffs3, stream_ceoff_loc.n_taps);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_half_complex_delay_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t pfilt_banks, uint8_t delay)
{
    int32_t err;
    uint8_t i, j, pfilt_bank, pfilt_local;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(terminal > ADI_APOLLO_TX);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilts & ~ADI_APOLLO_PFILT_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(pfilt_banks & ~ADI_APOLLO_PFILT_BANK_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(delay > 127);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            for(j = 0; j < ADI_APOLLO_PFILT_BANK_NUM; j ++) {
                pfilt_bank = pfilt_banks & (ADI_APOLLO_PFILT_BANK0 <<j);
                if (pfilt_bank > 0) {
                    pfilt_local = (((i%ADI_APOLLO_PFILT_PER_SIDE)*ADI_APOLLO_PFILT_BANK_NUM) + j);

                    err = adi_apollo_hal_bf_set(device, BF_HC_PROG_DELAY_INFO(regmap_base_addr, pfilt_local), delay);
                    ADI_APOLLO_ERROR_RETURN(err);
               }
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_coeff_transfer(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t bank_sel)
{
    int32_t err;
    uint8_t i, pfilt_bank;
    adi_apollo_blk_sel_t pfilt;
    uint8_t bank_number = 0;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(bank_sel)!= 1);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for(i = 0; i < ADI_APOLLO_PFILT_BANK_NUM; i ++) {
        pfilt_bank = bank_sel & (ADI_APOLLO_PFILT_BANK0 <<i);
        if (pfilt_bank > 0) {
            bank_number = i;
            break;
        }
    }

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_RD_COEFF_PAGE_SEL_INFO(regmap_base_addr, i%2), bank_number);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Generate 0->1 pulse to transfer selected back to active */
            err = adi_apollo_hal_bf_set(device, BF_PFIR_COEFF_TRANSFER_INFO(regmap_base_addr, i%2), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PFIR_COEFF_TRANSFER_INFO(regmap_base_addr, i%2), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t  pfilt, adi_apollo_pfilt_inspect_t *pfilt_inspect)
{
    int32_t err;
    uint8_t i, j, k, pfilt_local;
    adi_apollo_blk_sel_t pfilt_temp;
    uint32_t regmap_base_addr = 0;
    uint32_t regmap_coeff_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(pfilt_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(pfilt)!= 1);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilt);

    /* pfilt_transfer_coeff_regmode: read the bank number*/
    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt_temp = pfilt & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt_temp > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_get(device, BF_PFIR_COEFF_TRANSFER_INFO(regmap_base_addr, i % 2), (uint8_t*)&(pfilt_inspect->pfir_coeff_transfer), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_RD_COEFF_PAGE_SEL_INFO(regmap_base_addr, i % 2), (uint8_t*)&(pfilt_inspect->bank_sel), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            break;
        }
    }


    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i+= 2) {                               //Common bitfield for 0 & 1 pfilts
        pfilt_temp = pfilt & ((ADI_APOLLO_PFILT_A0 << i) | (ADI_APOLLO_PFILT_A0 << (i+1)));
        if (pfilt_temp > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            /* pfilt_mode_pgm and pfilt_pgm*/
            err = adi_apollo_hal_bf_get(device, BF_QUAD_MODE_INFO(regmap_base_addr), (uint8_t*) &(pfilt_inspect->dp_cfg.dq_mode), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_REAL_DATA_INFO(regmap_base_addr), (uint8_t*) &(pfilt_inspect->dp_cfg.real_data), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_MODE_SWITCH_INFO(regmap_base_addr), (uint8_t*) &(pfilt_inspect->dp_cfg.mode_switch), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_ADD_SUB_SEL_INFO(regmap_base_addr), (uint8_t*) &(pfilt_inspect->dp_cfg.add_sub_sel), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* pfilt_setup_gpio_transfer */
            err = adi_apollo_hal_bf_get(device, BF_EQ_GPIO_EN_INFO(regmap_base_addr), (uint8_t*) &(pfilt_inspect->eq_gpio_sel), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_GPIO_CONFIG1_INFO(regmap_base_addr), (uint8_t*) &(pfilt_inspect->gpio_config1), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            break;
        }
    }

    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt_temp = pfilt & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt_temp > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_get(device, BF_PFIR_I_MODE_INFO(regmap_base_addr, i%2), (uint8_t*) &(pfilt_inspect->dp_cfg.i_mode), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_PFIR_Q_MODE_INFO(regmap_base_addr, i%2), (uint8_t*) &(pfilt_inspect->dp_cfg.q_mode), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            for (j = 0; j < ADI_APOLLO_PFILT_BANK_NUM; j++) {
                /* pfilt_gain_dly_pgm */
                pfilt_local = (((i % ADI_APOLLO_PFILT_PER_SIDE) * ADI_APOLLO_PFILT_BANK_NUM) + j);

                err = adi_apollo_hal_bf_get(device, BF_PFIR_IX_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_ix_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_PFIR_IY_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_iy_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_PFIR_QX_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_qx_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_PFIR_QY_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_qy_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);

                err = adi_apollo_hal_bf_get(device, BF_PFIR_IX_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_ix_scalar_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_PFIR_IY_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_iy_scalar_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_PFIR_QX_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_qx_scalar_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_PFIR_QY_SCALAR_GAIN_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.pfir_qy_scalar_gain_db[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);

                err = adi_apollo_hal_bf_get(device, BF_HC_PROG_DELAY_INFO(regmap_base_addr, pfilt_local), (uint8_t*) &(pfilt_inspect->dp_cfg.hc_prog_delay[j]), 1);
                ADI_APOLLO_ERROR_RETURN(err);

                /* pfilt_coeff_pgm */
                regmap_coeff_base_addr = calc_pfilt_coeff_base(terminal, (i * ADI_APOLLO_PFILT_BANK_NUM) + j);
                if (j % ADI_APOLLO_PFILT_BANKS_PER_REGMAP == 0) {               //BANK0 and BANK2
                    for (k = 0; k < ADI_APOLLO_PFILT_COEFF_NUM; k++) {
                        err = adi_apollo_hal_bf_get(device, BF_COEFF_0_INFO(regmap_coeff_base_addr, k), (uint8_t*) &(pfilt_inspect->dp_cfg.coeffs[j][k]), 2);
                        ADI_APOLLO_ERROR_RETURN(err);
                    }
                }
                else {                                                          //BANK1 and BANK3
                    for (k = 0; k < ADI_APOLLO_PFILT_COEFF_NUM; k++) {
                        err = adi_apollo_hal_bf_get(device, BF_COEFF_1_INFO(regmap_coeff_base_addr, k), (uint8_t*) &(pfilt_inspect->dp_cfg.coeffs[j][k]), 2);
                        ADI_APOLLO_ERROR_RETURN(err);
                    }
                }

            }

            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_mode_enable_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, uint8_t iq_sel, adi_apollo_pfilt_mode_e mode)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            if (iq_sel & ADI_APOLLO_PFILT_STREAM_0) {
                err = adi_apollo_hal_bf_set(device, BF_PFIR_I_MODE_INFO(regmap_base_addr, i%2), mode);
                ADI_APOLLO_ERROR_RETURN(err);
            }
            if (iq_sel & ADI_APOLLO_PFILT_STREAM_1) {
                err = adi_apollo_hal_bf_set(device, BF_PFIR_Q_MODE_INFO(regmap_base_addr, i%2), mode);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_profile_sel_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, const adi_apollo_pfilt_profile_sel_mode_e prof_sel_mode)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(prof_sel_mode > (ADI_APOLLO_PFILT_CHAN_SEL_NUM-1));
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    const uint8_t bank_regs[ADI_APOLLO_PFILT_CHAN_SEL_NUM][3] = {
        {0, 0, 0},          // BF_PFILT_TRIGGER_EN_INFO, BF_EQ_GPIO_EN_INFO, BF_GPIO_CONFIG1_INFO
        {0, 1, 0},
        {0, 1, 1},
        {1, 0, 0},
        {1, 1, 0},
        {1, 1, 1}
    };

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_PFILT_TRIGGER_EN_INFO(regmap_base_addr), bank_regs[prof_sel_mode][0]);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_EQ_GPIO_EN_INFO(regmap_base_addr), bank_regs[prof_sel_mode][1]);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_GPIO_CONFIG1_INFO(regmap_base_addr), bank_regs[prof_sel_mode][2]);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_next_hop_num_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, int16_t hop_num)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(hop_num > ADI_APOLLO_PFILT_BANK_NUM-1);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_RD_COEFF_PAGE_SEL_INFO(regmap_base_addr, i % 2), hop_num);   // Next active coeff bank on trig event
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_ave_mode_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t pfilts, adi_apollo_pfilt_ave_mode_e ave_mode)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_RX, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(ave_mode > ADI_APOLLO_PFILT_AVE_NUM - 1);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(ADI_APOLLO_RX, i);

            err = adi_apollo_hal_bf_set(device, BF_MODE_SWITCH_INFO(regmap_base_addr), ave_mode != ADI_APOLLO_PFILT_AVE_DISABLE);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_ADD_SUB_SEL_INFO(regmap_base_addr), ave_mode == ADI_APOLLO_PFILT_AVE_ENABLE_ADD);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_pfilt_data_type_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, adi_apollo_pfilt_data_e data_type)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t pfilt;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(terminal, ADI_APOLLO_EC_PFILT_LOCK);
    ADI_APOLLO_INVALID_PARAM_RETURN(data_type > ADI_APOLLO_PFILT_REAL_DATA);
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for (i = 0; i < ADI_APOLLO_PFILT_NUM; i++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            regmap_base_addr = calc_pfilt_base(terminal, i);

            err = adi_apollo_hal_bf_set(device, BF_REAL_DATA_INFO(regmap_base_addr), data_type);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

uint32_t calc_pfilt_coeff_base(adi_apollo_terminal_e terminal, int32_t global_bank_base)
{
    static uint32_t rx_pfilt_coeff_regmap[ADI_APOLLO_PFILT_NUM * ADI_APOLLO_PFILT_BANK_NUM] = {
        RX_PFILT_COEFF0_RX_SLICE_0_RX_DIGITAL0, RX_PFILT_COEFF0_RX_SLICE_0_RX_DIGITAL0, RX_PFILT_COEFF0_RX_SLICE_1_RX_DIGITAL0, RX_PFILT_COEFF0_RX_SLICE_1_RX_DIGITAL0,
        RX_PFILT_COEFF1_RX_SLICE_0_RX_DIGITAL0, RX_PFILT_COEFF1_RX_SLICE_0_RX_DIGITAL0, RX_PFILT_COEFF1_RX_SLICE_1_RX_DIGITAL0, RX_PFILT_COEFF1_RX_SLICE_1_RX_DIGITAL0,
        RX_PFILT_COEFF0_RX_SLICE_0_RX_DIGITAL1, RX_PFILT_COEFF0_RX_SLICE_0_RX_DIGITAL1, RX_PFILT_COEFF0_RX_SLICE_1_RX_DIGITAL1, RX_PFILT_COEFF0_RX_SLICE_1_RX_DIGITAL1,
        RX_PFILT_COEFF1_RX_SLICE_0_RX_DIGITAL1, RX_PFILT_COEFF1_RX_SLICE_0_RX_DIGITAL1, RX_PFILT_COEFF1_RX_SLICE_1_RX_DIGITAL1, RX_PFILT_COEFF1_RX_SLICE_1_RX_DIGITAL1,
    };
    static uint32_t tx_pfilt_coeff_regmap[ADI_APOLLO_PFILT_NUM * ADI_APOLLO_PFILT_BANK_NUM] = {
        TX_PFILT_COEFF0_TX_SLICE_0_TX_DIGITAL0, TX_PFILT_COEFF0_TX_SLICE_0_TX_DIGITAL0, TX_PFILT_COEFF0_TX_SLICE_1_TX_DIGITAL0, TX_PFILT_COEFF0_TX_SLICE_1_TX_DIGITAL0,
        TX_PFILT_COEFF1_TX_SLICE_0_TX_DIGITAL0, TX_PFILT_COEFF1_TX_SLICE_0_TX_DIGITAL0, TX_PFILT_COEFF1_TX_SLICE_1_TX_DIGITAL0, TX_PFILT_COEFF1_TX_SLICE_1_TX_DIGITAL0,
        TX_PFILT_COEFF0_TX_SLICE_0_TX_DIGITAL1, TX_PFILT_COEFF0_TX_SLICE_0_TX_DIGITAL1, TX_PFILT_COEFF0_TX_SLICE_1_TX_DIGITAL1, TX_PFILT_COEFF0_TX_SLICE_1_TX_DIGITAL1,
        TX_PFILT_COEFF1_TX_SLICE_0_TX_DIGITAL1, TX_PFILT_COEFF1_TX_SLICE_0_TX_DIGITAL1, TX_PFILT_COEFF1_TX_SLICE_1_TX_DIGITAL1, TX_PFILT_COEFF1_TX_SLICE_1_TX_DIGITAL1,
    };
    if (terminal == ADI_APOLLO_RX)
        return rx_pfilt_coeff_regmap[global_bank_base];
    else
        return tx_pfilt_coeff_regmap[global_bank_base];
}

uint32_t calc_pfilt_base(adi_apollo_terminal_e terminal, int32_t pfilt_index)
{
    static uint32_t rx_pfilt_regmap[ADI_APOLLO_PFILT_NUM] = {
        RX_PFILT_TOP_RX_SLICE_0_RX_DIGITAL0, RX_PFILT_TOP_RX_SLICE_0_RX_DIGITAL0,
        RX_PFILT_TOP_RX_SLICE_0_RX_DIGITAL1, RX_PFILT_TOP_RX_SLICE_0_RX_DIGITAL1
    };
    static uint32_t tx_pfilt_regmap[ADI_APOLLO_PFILT_NUM] = {
        TX_PFILT_TOP_TX_SLICE_0_TX_DIGITAL0, TX_PFILT_TOP_TX_SLICE_0_TX_DIGITAL0,
        TX_PFILT_TOP_TX_SLICE_0_TX_DIGITAL1, TX_PFILT_TOP_TX_SLICE_0_TX_DIGITAL1
    };
    if (terminal == ADI_APOLLO_RX)
        return rx_pfilt_regmap[pfilt_index];
    else
        return tx_pfilt_regmap[pfilt_index];
}

static int32_t write_bank(adi_apollo_device_t *device, uint32_t bank_num, uint32_t regmap_base_addr, int first_idx, int16_t ceoffs[], uint32_t n_taps)
{
    int32_t err;
    uint32_t i;
    
    if ((bank_num % ADI_APOLLO_PFILT_BANKS_PER_REGMAP == 0)) { //BANK0 and BANK2
        for (i = 0; i < n_taps; i++) {
            err = adi_apollo_hal_bf_set(device, BF_COEFF_0_INFO(regmap_base_addr, (i + first_idx) % ADI_APOLLO_PFILT_COEFF_NUM), ceoffs[i]);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    } else { //BANK1 and BANK3
        for (i = 0; i < n_taps; i++) {
            err = adi_apollo_hal_bf_set(device, BF_COEFF_1_INFO(regmap_base_addr, (i + first_idx) % ADI_APOLLO_PFILT_COEFF_NUM), ceoffs[i]);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return err;
}
/*! @} */
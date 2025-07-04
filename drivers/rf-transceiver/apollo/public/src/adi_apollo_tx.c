/*!
 * \brief     TX data path top level APIs
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo.h"
#include "adi_apollo_tx.h"
#include "adi_apollo_config.h"
#include "adi_apollo_cfg.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_duc_local.h"
#include "adi_apollo_dac.h"
#include "adi_apollo_jrx.h"
#include "adi_apollo_private_device.h"

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_tx_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_txpath_t *config, adi_apollo_jesd_rx_cfg_t* jrx_config)
{
    int32_t err;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(jrx_config);
    ADI_APOLLO_INVALID_PARAM_RETURN((side > ADI_APOLLO_NUM_SIDES - 1));

    /* Release TX datapath DP reset */
    adi_apollo_txmisc_dp_reset(device, (adi_apollo_side_select_e)(1 << side), 0);

    /* CDUC/CNCO config */
    for (i = 0; i < ADI_APOLLO_CDUCS_PER_SIDE; i ++) {
        err = adi_apollo_tx_cduc_configure(device, side, (adi_apollo_cduc_idx_e)i, &(config->tx_cduc[i]));
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* FDUC/FNCO config */
    for (i = 0; i < ADI_APOLLO_FDUCS_PER_SIDE; i ++) {
        err = adi_apollo_tx_fduc_configure(device, side, (adi_apollo_fduc_idx_e)i, &(config->tx_fduc[i]));
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Summer, XBAR and misc config */
    err = adi_apollo_tx_txpath_misc_configure(device, side, &(config->tx_mux_summer_xbar));
    ADI_APOLLO_ERROR_RETURN(err);

    /* PFILT config */
    if (!adi_apollo_private_device_lockout_get(device, ADI_APOLLO_TX, ADI_APOLLO_EC_PFILT_LOCK)) {
        for (i = 0; i < ADI_APOLLO_PFILTS_PER_SIDE; i ++) {
            err = adi_apollo_tx_pfilt_configure(device, side, (adi_apollo_pfilt_idx_e)i, &(config->tx_pfilt[i]));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* CFIR config */
    if (!adi_apollo_private_device_lockout_get(device, ADI_APOLLO_TX, ADI_APOLLO_EC_CFIR_LOCK)) {
        for(i = 0; i < ADI_APOLLO_CFIRS_PER_SIDE; i ++) {
            err = adi_apollo_tx_cfir_configure(device, side, (adi_apollo_cfir_idx_e)i, &(config->tx_cfir[i]));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* FSRC config */
    if (!adi_apollo_private_device_lockout_get(device, ADI_APOLLO_TX, ADI_APOLLO_EC_FSRC_LOCK)) {
        for(i = 0; i < ADI_APOLLO_FSRCS_PER_SIDE; i ++) {
            err = adi_apollo_tx_fsrc_configure(device, side, (adi_apollo_fsrc_idx_e)i, &(config->tx_fsrc[i]), &(jrx_config->rx_link_cfg[i]));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* Inverse SINC config */
    for (i = 0; i < ADI_APOLLO_CDUC_PATHS_PER_SIDE; i ++) {
        err = adi_apollo_tx_inv_sinc_configure(device, side, (adi_apollo_cduc_path_idx_e)i, config->inv_sinc_en[i] ? 1 : 0);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Tx Sample Repeat config */
    for (i = 0; i < ADI_APOLLO_JESD_LINKS; i++) {
        err = adi_apollo_tx_sample_repeat_configure(device, side, (adi_apollo_jesd_links_e)i, config, jrx_config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_tx_cduc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cduc_idx_e idx, adi_apollo_cduc_cfg_t *config)
{
    int32_t err;
    uint8_t cduc_sel;
    uint8_t cnco_sel;
    int cnco_idx;
    adi_apollo_cduc_pgm_t duc_pgm_config;
    adi_apollo_cnco_pgm_t nco_pgm_config;
    adi_apollo_coarse_nco_hop_t nco_hop_profile_config;
    adi_apollo_trig_mst_config_t nco_trig_mst_config;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* Determine block api select from index */
    cduc_sel = ADI_APOLLO_CDUC_IDX2B(side, idx);

    /* Program the coarse DUC */
    duc_pgm_config.interp = config->drc_ratio;
    duc_pgm_config.chb1_int_time_dly = 0;               // TODO: should delays be include in dev profile
    duc_pgm_config.chb2_int_time_dly = 0;               // TODO: should delays be include in dev profile
    duc_pgm_config.chb3_int_time_dly = 0;               // TODO: should delays be include in dev profile
    duc_pgm_config.ctb1_int_time_dly = 0;               // TODO: should delays be include in dev profile
    duc_pgm_config.test_mux = 0;                        // TODO: add to dev profile?
    duc_pgm_config.cduc_spien_en = 0;                   // TODO: add to dev profile?
    duc_pgm_config.cduc_spi_en = 0x00;                  // TODO: add to dev profile?
    duc_pgm_config.cduc_irq_en = 0;                     // TODO: add to dev profile?
    duc_pgm_config.cduc_irq_status = 0xff;              // TODO: add to dev profile?
    err = adi_apollo_cduc_pgm(device, cduc_sel, &duc_pgm_config);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Program the CNCOs (there are two CNCOs per CDUC) */
    for (cnco_idx = 0; cnco_idx < ADI_APOLLO_NUM_NCOS_PER_DRC; cnco_idx++) {

        cnco_sel = cduc_sel << (2 * cnco_idx);       // Selects A0/A2, A1/A3, ... B1/B3 (pairs for 8t8r)

        /* Program the coarse NCO */
        nco_pgm_config.drc_en = config->nco[cnco_idx].drc_en;
        //nco_pgm_config.debug_drc_clkoff_n = config->debug_cdrc_clkoff_n;     // TODO: add to device profile
        nco_pgm_config.debug_drc_clkoff_n = 0xff;
        nco_pgm_config.profile_num = 0;                         // TODO: add to dev profile
        nco_pgm_config.profile_sel_mode = config->nco[cnco_idx].nco_profile_sel_mode;
        nco_pgm_config.cmplx_mxr_scale_en = config->nco[cnco_idx].cmplx_mxr_mult_scale_en;
        nco_pgm_config.drc_phase_inc = config->nco[cnco_idx].nco_phase_inc;
        nco_pgm_config.drc_phase_offset = config->nco[cnco_idx].nco_phase_offset;
        nco_pgm_config.drc_phase_inc_frac_a = config->nco[cnco_idx].nco_phase_inc_frac_a;
        nco_pgm_config.drc_phase_inc_frac_b = config->nco[cnco_idx].nco_phase_inc_frac_b;
        nco_pgm_config.if_mode = config->nco[cnco_idx].nco_if_mode;
        nco_pgm_config.mixer_sel = config->nco[cnco_idx].drc_mxr_sel;
        nco_pgm_config.dc_testmode_value = config->nco[cnco_idx].dc_testmode_value;
        err = adi_apollo_cnco_pgm(device, ADI_APOLLO_TX, cnco_sel, &nco_pgm_config);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Load the coarse nco profiles */
        err = adi_apollo_cnco_profile_load(device, ADI_APOLLO_TX, cnco_sel,
                ADI_APOLLO_NCO_PROFILE_PHASE_INCREMENT, 0, config->nco[cnco_idx].nco_phase_inc_words, ADI_APOLLO_CNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_cnco_profile_load(device, ADI_APOLLO_TX, cnco_sel,
                ADI_APOLLO_NCO_PROFILE_PHASE_OFFSET, 0, config->nco[cnco_idx].nco_phase_offset_words, ADI_APOLLO_CNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Auto hop flip, incr or decr */
        nco_hop_profile_config.profile_sel_mode = config->nco[cnco_idx].nco_profile_sel_mode;
        nco_hop_profile_config.auto_mode = config->nco[cnco_idx].nco_auto_inc_dec; /* TODO - add autoflip to enum */
        nco_hop_profile_config.low_limit = 0;       /* TODO - get from device profile */
        nco_hop_profile_config.high_limit = 15;     /* TODO - get from device profile */
        nco_hop_profile_config.next_hop_number_wr_en = 0;
        nco_hop_profile_config.hop_ctrl_init = 0;
        nco_hop_profile_config.phase_handling = 0;
        err = adi_apollo_cnco_hop_enable(device, ADI_APOLLO_TX, cnco_sel, &nco_hop_profile_config);
        ADI_APOLLO_ERROR_RETURN(err);

        nco_trig_mst_config.trig_enable = ADI_APOLLO_TRIG_DISABLE;       /* TODO - post v1.1.0 dev profile */
        nco_trig_mst_config.trig_period = 0; // config->trig_mst_period; /* TODO - post v1.1.0 dev profile */
        nco_trig_mst_config.trig_offset = 0; // config->trig_mst_offset; /* TODO - post v1.1.0 dev profile */
        err = adi_apollo_trigts_cnco_trig_mst_config(device, ADI_APOLLO_TX, cnco_sel, &nco_trig_mst_config);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Set initial phase inc and offset. Select profile 0 as default. */
        err = adi_apollo_cnco_ftw_set(device, ADI_APOLLO_TX, cnco_sel, 0, 1, config->nco[cnco_idx].nco_phase_inc);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_cnco_pow_set(device, ADI_APOLLO_TX, cnco_sel, 0, 1, config->nco[cnco_idx].nco_phase_offset);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    // Trigger selection mux here
    err = adi_apollo_trigts_cdrc_trig_sel_mux_set(device, ADI_APOLLO_TX, cduc_sel, ADI_APOLLO_TRIG_SPI);    /* TODO: add to dev profile */
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tx_fduc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fduc_idx_e idx, adi_apollo_fduc_cfg_t *config)
{
    int32_t err;
    uint16_t fduc_sel;
    uint16_t fnco_sel;
    int fnco_idx;
    adi_apollo_fduc_pgm_t duc_pgm_config;
    adi_apollo_fnco_pgm_t nco_pgm_config;
    adi_apollo_fine_nco_hop_t nco_hop_profile_config;
    adi_apollo_trig_mst_config_t nco_trig_mst_config;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN((idx > ADI_APOLLO_FDUCS_PER_SIDE-1))

    /* Determine block api select from index */
    fduc_sel = ADI_APOLLO_FDUC_IDX2B(side, idx);

    /* Program the FDUC functional block */
    duc_pgm_config.interp = config->drc_ratio;
    duc_pgm_config.int_tdly_hb = 0,                     /* post v1.1.0 dev profile */
    duc_pgm_config.sub_dp_gain_en = config->sub_dp_gain_en;
    duc_pgm_config.subdp_gain = config->subdp_gain;
    duc_pgm_config.fduc_spien_en = 0x00;                // disable FDUC enable by SPI, use auto FDUC enable summer mux
    err = adi_apollo_fduc_pgm(device, fduc_sel, &duc_pgm_config);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Program the FNCOs (there are two FNCOs per FDUC) */
    for (fnco_idx = 0; fnco_idx < ADI_APOLLO_NUM_NCOS_PER_DRC; fnco_idx++) {

        fnco_sel = fduc_sel << (4 * fnco_idx);       // Selects A0/A4, A1/A5, ... B3/B7 (pairs for 8t8r)

        nco_pgm_config.drc_en = config->nco[fnco_idx].drc_en;
        nco_pgm_config.debug_drc_clkoff_n = config->nco[fnco_idx].debug_fdrc_clkoff_n;
        nco_pgm_config.if_mode = config->nco[fnco_idx].nco_if_mode;
        nco_pgm_config.mixer_sel = config->nco[fnco_idx].drc_mxr_sel;
        nco_pgm_config.cmplx_mxr_scale_en = config->nco[fnco_idx].cmplx_mxr_mult_scale_en;
        nco_pgm_config.hop_mode_en = config->nco[fnco_idx].hop_mode_en;
        nco_pgm_config.profile_num = 0;                                         // TODO: add to dev profile
        nco_pgm_config.profile_sel_mode = config->nco[fnco_idx].nco_profile_sel_mode;
        nco_pgm_config.main_phase_inc =  config->nco[fnco_idx].nco_phase_inc;
        nco_pgm_config.main_phase_offset = config->nco[fnco_idx].nco_phase_offset;
        nco_pgm_config.drc_phase_inc_frac_a = config->nco[fnco_idx].nco_phase_inc_frac_a;
        nco_pgm_config.drc_phase_inc_frac_b = config->nco[fnco_idx].nco_phase_inc_frac_b;
        nco_pgm_config.dc_testmode_value = config->nco[fnco_idx].dc_testmode_value;
        err = adi_apollo_fnco_pgm(device, ADI_APOLLO_TX, fnco_sel, &nco_pgm_config);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Load fine nco profiles */
        err = adi_apollo_fnco_profile_load(device, ADI_APOLLO_TX, fnco_sel,
            ADI_APOLLO_NCO_PROFILE_PHASE_INCREMENT, 0, config->nco[fnco_idx].nco_phase_inc_words, ADI_APOLLO_FNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_fnco_profile_load(device, ADI_APOLLO_TX, fnco_sel,
            ADI_APOLLO_NCO_PROFILE_PHASE_OFFSET, 0, config->nco[fnco_idx].nco_phase_offset_words, ADI_APOLLO_FNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Configure hopping */
        nco_hop_profile_config.profile_sel_mode = config->nco[fnco_idx].nco_profile_sel_mode;
        nco_hop_profile_config.nco_trig_hop_sel = config->nco[fnco_idx].nco_trig_hop_sel;
        nco_hop_profile_config.phase_inc_auto_mode = (adi_apollo_nco_auto_flip_incdir_e)config->nco[fnco_idx].nco_auto_inc_dec_freq;         /* TODO - add autoflip to enum */
        nco_hop_profile_config.phase_offset_auto_mode = (adi_apollo_nco_auto_flip_incdir_e)config->nco[fnco_idx].nco_auto_inc_dec_phase;     /* TODO - add autoflip to enum */
        nco_hop_profile_config.phase_inc_low_limit = 0;         /* TODO - load from device profile */
        nco_hop_profile_config.phase_inc_high_limit = 31;       /* TODO - load from device profile*/
        nco_hop_profile_config.phase_offset_low_limit = 0;      /* TODO - load from device profile*/
        nco_hop_profile_config.phase_offset_high_limit = 31;    /* TODO - load from device profile*/
        nco_hop_profile_config.phase_handling = 0;
        err = adi_apollo_fnco_hop_pgm(device, ADI_APOLLO_TX, fnco_sel, &nco_hop_profile_config);

        nco_trig_mst_config.trig_enable = ADI_APOLLO_TRIG_DISABLE;       /* TODO -post v1.1.0 dev profile */
        nco_trig_mst_config.trig_period = 0; // config->trig_mst_period; /* TODO -post v1.1.0 dev profile */
        nco_trig_mst_config.trig_offset = 0; // config->trig_mst_offset; /* TODO -post v1.1.0 dev profile */
        err = adi_apollo_trigts_fnco_trig_mst_config(device, ADI_APOLLO_TX, fnco_sel, &nco_trig_mst_config);
        ADI_APOLLO_ERROR_RETURN(err);

        // Trigger selection mux here
        err = adi_apollo_trigts_fdrc_trig_sel_mux_set(device, ADI_APOLLO_TX, fduc_sel, ADI_APOLLO_TRIG_SPI);    /* TODO add to dev profile */
        ADI_APOLLO_ERROR_RETURN(err);

        /* Set initial hop index to 0. This is relevant when hop_mode_en is true  */
        err = adi_apollo_fnco_ftw_set(device, ADI_APOLLO_TX, fnco_sel, 0, 1, config->nco[fnco_idx].nco_phase_inc_words[0]);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_fnco_pow_set(device, ADI_APOLLO_TX, fnco_sel, 0, 1, config->nco[fnco_idx].nco_phase_offset_words[0]);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tx_pfilt_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_pfilt_idx_e idx_pfilt, adi_apollo_pfilt_cfg_t *config)
{
    int32_t err;
    uint8_t coeff_set_idx;
    adi_apollo_pfilt_mode_pgm_t blk_mode_config;
    adi_apollo_pfilt_gain_dly_pgm_t blk_gain_dly_config;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN((idx_pfilt > ADI_APOLLO_PFILTS_PER_SIDE-1))

    for (coeff_set_idx = 0; coeff_set_idx<ADI_APOLLO_PFILT_COEFF_SETS; coeff_set_idx++) {

        /* Program pfilt coefficient sets */
        err = adi_apollo_pfilt_coeff_pgm(device, ADI_APOLLO_TX,
                            ADI_APOLLO_PFILT_IDX2B(side, idx_pfilt),
                            ADI_APOLLO_PFILT_COEFF_IDX2B(coeff_set_idx),
                            config->coeffs[coeff_set_idx], ADI_APOLLO_PFILT_COEFFS);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Configure pfilt gain and delay sets */
        blk_gain_dly_config.pfir_ix_gain = config->pfir_ix_gain_db[coeff_set_idx];
        blk_gain_dly_config.pfir_iy_gain = config->pfir_iy_gain_db[coeff_set_idx];
        blk_gain_dly_config.pfir_qx_gain = config->pfir_qx_gain_db[coeff_set_idx];
        blk_gain_dly_config.pfir_qy_gain = config->pfir_qy_gain_db[coeff_set_idx];
        blk_gain_dly_config.pfir_ix_scalar_gain = config->pfir_ix_scalar_gain_db[coeff_set_idx];
        blk_gain_dly_config.pfir_iy_scalar_gain = config->pfir_iy_scalar_gain_db[coeff_set_idx];
        blk_gain_dly_config.pfir_qx_scalar_gain = config->pfir_qx_scalar_gain_db[coeff_set_idx];
        blk_gain_dly_config.pfir_qy_scalar_gain = config->pfir_qy_scalar_gain_db[coeff_set_idx];
        blk_gain_dly_config.hc_delay = config->hc_prog_delay[coeff_set_idx];

        err = adi_apollo_pfilt_gain_dly_pgm(device, ADI_APOLLO_TX,
                        ADI_APOLLO_PFILT_IDX2B(side, idx_pfilt),
                        ADI_APOLLO_PFILT_COEFF_IDX2B(coeff_set_idx),
                        &blk_gain_dly_config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Program pfilt mode */
    blk_mode_config.data = config->real_data;
    blk_mode_config.add_sub_sel = config->add_sub_sel;
    blk_mode_config.dq_mode = device->dev_info.is_8t8r ? ADI_APOLLO_PFILT_QUAD_MODE : ADI_APOLLO_PFILT_DUAL_MODE;
    blk_mode_config.mode_switch = config->mode_switch;
    blk_mode_config.pfir_i_mode = config->i_mode;
    blk_mode_config.pfir_q_mode = config->q_mode;

    err = adi_apollo_pfilt_mode_pgm(device, ADI_APOLLO_TX,
                    ADI_APOLLO_PFILT_IDX2B(side, idx_pfilt),
                    &blk_mode_config);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tx_cfir_configure(adi_apollo_device_t* device, adi_apollo_sides_e side, adi_apollo_cfir_idx_e idx_cfir, adi_apollo_cfir_cfg_t* config)
{
    int32_t err;
    uint8_t idx_profile, idx_set;
    adi_apollo_cfir_pgm_t blk_mode_config;
    uint16_t sparse_coeff_sel_sel[ADI_APOLLO_CFIR_NUM_TAPS] = { 0 };    /* TODO: add to device profile */
    uint8_t sparse_mem_sel[ADI_APOLLO_CFIR_MEM_SEL_NUM] = { 0 };        /* TODO: add to device profile */

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN((idx_cfir > ADI_APOLLO_CFIRS_PER_SIDE - 1))

    /* CFIR top parameters */
    blk_mode_config.cfir_bypass = config->enable ? 0 : 1; //Bypass config
    blk_mode_config.cfir_sparse_filt_en = config->sparse_mode;
    blk_mode_config.cfir_32taps_en = config->cfir_mode; //cfir_32taps_en config
    blk_mode_config.cfir_coeff_transfer = 0; //coeff_transfer should go from 0 to 1

    err = adi_apollo_cfir_pgm(device, ADI_APOLLO_TX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir), &blk_mode_config);
    ADI_APOLLO_ERROR_RETURN(err);

   /* Program CFIR coefficients, gain, scalar gain and sparse values for all profiles */
    for (idx_profile = 0; idx_profile < ADI_APOLLO_CFIR_NUM_PROFILES; idx_profile++) {
        for (idx_set = 0; idx_set < ADI_APOLLO_CFIR_COEFF_SETS; idx_set++) {
            err = adi_apollo_cfir_coeff_pgm(device, ADI_APOLLO_TX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                config->coeffs_i[idx_profile][idx_set], config->coeffs_q[idx_profile][idx_set], ADI_APOLLO_CFIR_NUM_TAPS);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_scalar_pgm(device, ADI_APOLLO_TX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                config->scalar_i[idx_profile][idx_set], config->scalar_q[idx_profile][idx_set]);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_gain_pgm(device, ADI_APOLLO_TX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                config->cfir_gain_dB[idx_profile][idx_set]);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_sparse_coeff_sel_pgm(device, ADI_APOLLO_TX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                sparse_coeff_sel_sel, ADI_APOLLO_CFIR_NUM_TAPS);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_sparse_mem_sel_pgm(device, ADI_APOLLO_TX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                sparse_mem_sel, ADI_APOLLO_CFIR_MEM_SEL_NUM);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* Make profile #0 active for all data paths. Transfers profile 0 coeffs and gains to CFIR block */
    err = adi_apollo_cfir_profile_sel(device, ADI_APOLLO_TX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
        ADI_APOLLO_CFIR_DP_ALL, ADI_APOLLO_CFIR_PROFILE_0);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tx_fsrc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fsrc_idx_e idx_fsrc, 
    adi_apollo_fsrc_cfg_t *config, adi_apollo_jesd_rx_link_cfg_t *jrx_link_config)
{
    int32_t err;
    adi_apollo_fsrc_pgm_t blk_mode_config;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(jrx_link_config);
    ADI_APOLLO_INVALID_PARAM_RETURN((idx_fsrc > ADI_APOLLO_FSRCS_PER_SIDE-1))

    /* Program fsrc */
    blk_mode_config.fsrc_rate_int = config->fsrc_rate_int;
    blk_mode_config.fsrc_rate_frac_a = config->fsrc_rate_frac_a;
    blk_mode_config.fsrc_rate_frac_b = config->fsrc_rate_frac_b;
    blk_mode_config.sample_frac_delay = config->fsrc_delay;
    blk_mode_config.fsrc_en = config->enable;
    blk_mode_config.gain_reduction = config->gain_reduction;
    blk_mode_config.ptr_overwrite = config->ptr_overwrite;
    blk_mode_config.ptr_syncrstval = config->ptr_syncrstval;
    blk_mode_config.fsrc_data_mult_dither_en = config->data_mult_dither_en;
    blk_mode_config.fsrc_dither_en = config->dither_en;
    blk_mode_config.fsrc_4t4r_split = config->split_4t4r;
    blk_mode_config.fsrc_bypass = config->enable ? 0 : 1;
    blk_mode_config.fsrc_1x_mode = config->mode_1x;

    err = adi_apollo_fsrc_pgm(device, ADI_APOLLO_TX,
                                        ADI_APOLLO_FSRC_IDX2B(side, idx_fsrc),
                                        &blk_mode_config);
    ADI_APOLLO_ERROR_RETURN(err);

    // TODO: direct reference to BF not compliant with API structure -
    #include "adi_apollo_bf_jrx_wrapper.h"
    uint32_t jrx_wrapper;
    jrx_wrapper = side ? JRX_WRAPPER_JRX_TX_DIGITAL1 : JRX_WRAPPER_JRX_TX_DIGITAL0;
    if (jrx_link_config->link_in_use) {
        /* Enable Invalid Samples */
        err = adi_apollo_hal_bf_set(device, BF_INVALID_DATA_EN_INFO(jrx_wrapper, idx_fsrc), config->enable);
        ADI_APOLLO_ERROR_RETURN(err);
        /* Number of Invalid Samples */
        err = adi_apollo_hal_bf_set(device, BF_NUM_OF_INVALID_SAMPLE_INFO(jrx_wrapper, idx_fsrc), jrx_link_config->ns_minus1);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tx_txpath_misc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_txpath_misc_t *config)
{
    int32_t err;
    uint8_t i, summer_sel;
    uint16_t fduc_sel;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* Summer crossbar (Tx mux2) */
    for (i = 0; i < ADI_APOLLO_CDUC_PATHS_PER_SIDE; i++) {
        /* Determine block api select from index. (summer/cduc are 1-to-1) */
        summer_sel = ADI_APOLLO_CDUC_IDX2B(side, i);

        /* Convert FDUC side selection to api bit mask */
        fduc_sel = config->fduc_cduc_summer[i] << ((side == ADI_APOLLO_SIDE_IDX_A) ? 0 : ADI_APOLLO_TX_SUMMER_NUM);

        /* Program the FDUC-to-CDUC summer crossbar */
        err = adi_apollo_txmux_summer_block_set(device, summer_sel, fduc_sel);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* HSDOUT (Tx mux0) */
    err = adi_apollo_txmux_hsdout_set(device, ADI_APOLLO_SIDE_IDX2B(side), config->mux0_sel, ADI_APOLLO_NUM_DAC_PER_SIDE);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Modulator Switch (MODSW, Tx mux1) */
    err = adi_apollo_txmux_modsw_set(device, ADI_APOLLO_SIDE_IDX2B(side), (adi_apollo_modsw_select_e)config->mux1_sel[0], (adi_apollo_modsw_select_e)config->mux1_sel[1]);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Low sample mode */
    err = adi_apollo_txmisc_low_samp_set(device, ADI_APOLLO_SIDE_IDX2B(side), config->low_samp_en? 1 : 0);
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tx_inv_sinc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cduc_path_idx_e idx, uint8_t enable)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_invsinc_enable(device, ADI_APOLLO_CDUC_IDX2B(side, idx), enable);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tx_sample_repeat_configure(adi_apollo_device_t *device, adi_apollo_sides_e side,
                                              adi_apollo_jesd_links_e link_idx, adi_apollo_txpath_t *tx_path_config, adi_apollo_jesd_rx_cfg_t *jrx_config)
{
    int32_t err;
    uint16_t links = side ? ADI_APOLLO_LINK_SIDE_B : ADI_APOLLO_LINK_SIDE_A;
    adi_apollo_fsrc_cfg_t *fsrc_config;
    adi_apollo_jesd_rx_link_cfg_t *jrx_link_config;
    uint8_t is_1x1x_mode = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_idx > ADI_APOLLO_JESD_LINK_1);
    ADI_APOLLO_NULL_POINTER_RETURN(tx_path_config);
    ADI_APOLLO_NULL_POINTER_RETURN(jrx_config);

    fsrc_config = &(tx_path_config->tx_fsrc[link_idx]);
    jrx_link_config = &(jrx_config->rx_link_cfg[link_idx]);

    if (!fsrc_config->enable && jrx_link_config->link_in_use) {

        // TODO: This should be an explict parameter in device profile, since enabling sample repeat removal may be unnecessary.
        // If link dp ratio does not match the greatest duc ratio, then sample repeat should be enabled.
        is_1x1x_mode = (tx_path_config->tx_cduc[0].drc_ratio == ADI_APOLLO_CDUC_INTERP_1) && (tx_path_config->tx_fduc[0].drc_ratio == ADI_APOLLO_FDUC_RATIO_1);
        err = adi_apollo_jesd_rx_sample_repeat_en(device, links << (int32_t)link_idx, !is_1x1x_mode);

        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}
/*! @} */
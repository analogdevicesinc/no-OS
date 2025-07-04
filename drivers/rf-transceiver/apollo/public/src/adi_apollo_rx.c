/*!
 * \brief     RX data path top level APIs
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo.h"
#include "adi_apollo_rx.h"
#include "adi_apollo_config.h"
#include "adi_apollo_cfg.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_private_device.h"
#include "adi_apollo_ddc_local.h"

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_rx_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_rxpath_t *config, adi_apollo_jesd_tx_cfg_t *jtx_config)
{
    int32_t err;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(jtx_config);
    ADI_APOLLO_INVALID_PARAM_RETURN((side > ADI_APOLLO_NUM_SIDES - 1));

    /* Release RX datapath DP reset */
    adi_apollo_rxmisc_dp_reset(device, (adi_apollo_side_select_e)(1 << side), 0);

    /* CDDC/CNCO config */
    for(i = 0; i < ADI_APOLLO_CDDCS_PER_SIDE; i ++) {
        err = adi_apollo_rx_cddc_configure(device, side, (adi_apollo_cddc_idx_e)i, &(config->rx_cddc[i]));
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* FDDC/FNCO config */
    for(i = 0; i < ADI_APOLLO_FDDCS_PER_SIDE; i ++) {
        err = adi_apollo_rx_fddc_configure(device, side, (adi_apollo_fddc_idx_e)i, &(config->rx_fddc[i]));
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* XBARs  config */
    err = adi_apollo_rx_rxpath_misc_configure(device, side, &(config->rx_mux_splitter_xbar));
    ADI_APOLLO_ERROR_RETURN(err);

    /* PFILT CONFIG */
    if (!adi_apollo_private_device_lockout_get(device, ADI_APOLLO_RX, ADI_APOLLO_EC_PFILT_LOCK)) {
        for(i = 0; i < ADI_APOLLO_PFILTS_PER_SIDE; i ++) {
            err = adi_apollo_rx_pfilt_configure(device, side, (adi_apollo_pfilt_idx_e)i, &(config->rx_pfilt[i]));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* CFIR config */
    if (!adi_apollo_private_device_lockout_get(device, ADI_APOLLO_RX, ADI_APOLLO_EC_CFIR_LOCK)) {
        for(i = 0; i < ADI_APOLLO_CFIRS_PER_SIDE; i ++) {
            err = adi_apollo_rx_cfir_configure(device, side, (adi_apollo_cfir_idx_e)i, &(config->rx_cfir[i]));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

   /* FSRC config */
   if (!adi_apollo_private_device_lockout_get(device, ADI_APOLLO_RX, ADI_APOLLO_EC_FSRC_LOCK)) {
        for(i = 0; i < ADI_APOLLO_FSRCS_PER_SIDE; i ++) {
            err = adi_apollo_rx_fsrc_configure(device, side, (adi_apollo_fsrc_idx_e)i, &(config->rx_fsrc[i]));
            ADI_APOLLO_ERROR_RETURN(err);
        }
   }

   /* DFORMAT config */
   for (i = 0; i < ADI_APOLLO_JESD_LINKS; i++) {        // # of JESD links per side
       err = adi_apollo_rx_dformat_configure(device, side, (adi_apollo_jesd_links_e)i, &(config->rx_dformat[i]), &(jtx_config->tx_link_cfg[i]));
       ADI_APOLLO_ERROR_RETURN(err);
   }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_rx_cddc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cddc_idx_e idx, adi_apollo_cddc_cfg_t *config)
{
    int32_t err;
    uint8_t cddc_sel;
    uint8_t cnco_sel;
    int cnco_idx;
    adi_apollo_cddc_pgm_t ddc_pgm_config;
    adi_apollo_cnco_pgm_t nco_pgm_config;
    adi_apollo_coarse_nco_hop_t nco_hop_profile_config;
    adi_apollo_trig_mst_config_t nco_trig_mst_config;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* Determine block api select from index */
    cddc_sel = ADI_APOLLO_CDDC_IDX2B(side, idx);

    /* Program the coarse ddc */
    ddc_pgm_config.dcm = config->drc_ratio;
    ddc_pgm_config.fine_bypass = config->fine_bypass;
    ddc_pgm_config.link_num = config->link_num;
    ddc_pgm_config.debug_clkoff_n = config->debug_cddc_clkoff_n;
    ddc_pgm_config.hb1_filt_dly_en = config->hb1_filt_dly_en;
    ddc_pgm_config.hb2_filt_dly_en = config->hb2_filt_dly_en;
    ddc_pgm_config.tb1_filt_dly = config->tb1_filt_dly_cycles;
    ddc_pgm_config.hb1_gain_6db_en = config->hb1_gain_6db_en;
    ddc_pgm_config.tb1_gain_6db_en = config->tb1_gain_6db_en;
    err = adi_apollo_cddc_pgm(device, cddc_sel, &ddc_pgm_config);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Program the CNCOs (there are two CNCOs per CDDC) */
    for (cnco_idx = 0; cnco_idx < ADI_APOLLO_NUM_NCOS_PER_DRC; cnco_idx++) {

        cnco_sel = cddc_sel << (2 * cnco_idx);       // Selects A0/A2, A1/A3, ... B1/B3 (pairs for 8t8r)

        /* Program the coarse nco */
        nco_pgm_config.drc_en = config->nco[cnco_idx].drc_en;
        nco_pgm_config.debug_drc_clkoff_n = config->nco[cnco_idx].debug_cdrc_clkoff_n;
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
        err = adi_apollo_cnco_pgm(device, ADI_APOLLO_RX, cnco_sel, &nco_pgm_config);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Load the coarse nco profiles */
        err = adi_apollo_cnco_profile_load(device, ADI_APOLLO_RX, cnco_sel,
                ADI_APOLLO_NCO_PROFILE_PHASE_INCREMENT, 0, config->nco[cnco_idx].nco_phase_inc_words, ADI_APOLLO_CNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_cnco_profile_load(device, ADI_APOLLO_RX, cnco_sel,
                ADI_APOLLO_NCO_PROFILE_PHASE_OFFSET, 0, config->nco[cnco_idx].nco_phase_offset_words, ADI_APOLLO_CNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Profile trigger or direct based profile sel */
        nco_hop_profile_config.profile_sel_mode = config->nco[cnco_idx].nco_profile_sel_mode;

        /* Auto hop flip, incr or decr */
        nco_hop_profile_config.auto_mode = config->nco[cnco_idx].nco_auto_inc_dec; /* TODO: add autoflip to enum */
        nco_hop_profile_config.low_limit = 0;       /* TODO */
        nco_hop_profile_config.high_limit = 15;     /* TODO */
        nco_hop_profile_config.next_hop_number_wr_en = 0;
        nco_hop_profile_config.hop_ctrl_init = 0;
        nco_hop_profile_config.phase_handling = 0;

        err = adi_apollo_cnco_hop_enable(device, ADI_APOLLO_RX, cnco_sel, &nco_hop_profile_config);
        ADI_APOLLO_ERROR_RETURN(err);

        nco_trig_mst_config.trig_enable = ADI_APOLLO_TRIG_DISABLE;      /* TODO: TODO: add to dev profile */
        nco_trig_mst_config.trig_period = config->trig_mst_period;
        nco_trig_mst_config.trig_offset = config->trig_mst_offset;
        err = adi_apollo_trigts_cnco_trig_mst_config(device, ADI_APOLLO_RX, cnco_sel, &nco_trig_mst_config);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Set initial phase inc and offset. Select profile 0 as default. */
        err = adi_apollo_cnco_ftw_set(device, ADI_APOLLO_RX, cnco_sel, 0, 1, config->nco[cnco_idx].nco_phase_inc);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_cnco_pow_set(device, ADI_APOLLO_RX, cnco_sel, 0, 1, config->nco[cnco_idx].nco_phase_offset);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    
    // Trigger selection mux here
    err = adi_apollo_trigts_cdrc_trig_sel_mux_set(device, ADI_APOLLO_RX, cddc_sel, ADI_APOLLO_TRIG_SPI);    /* TODO: add to dev profile */
    ADI_APOLLO_ERROR_RETURN(err);
    
    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_rx_fddc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fddc_idx_e idx, adi_apollo_fddc_cfg_t *config)
{
    int32_t err;
    uint16_t fddc_sel;
    uint16_t fnco_sel;
    int fnco_idx;
    adi_apollo_fddc_pgm_t ddc_pgm_config;
    adi_apollo_fnco_pgm_t nco_pgm_config;
    adi_apollo_fine_nco_hop_t nco_hop_profile_config;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN((idx > ADI_APOLLO_FDDCS_PER_SIDE-1))

    /* Determine block api select from index */
    fddc_sel = ADI_APOLLO_FDDC_IDX2B(side, idx);

    /* Program the fddc functional block */
    ddc_pgm_config.dcm = config->drc_ratio;
    ddc_pgm_config.debug_clkoff_n = config->debug_fddc_clkoff_n;
    ddc_pgm_config.link_num = config->link_num;
    ddc_pgm_config.hb1_gain_6db_en = config->hb1_gain_6db_en;
    err = adi_apollo_fddc_pgm(device, fddc_sel, &ddc_pgm_config);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Program the FNCOs (there are two FNCOs per FDDC) */
    for (fnco_idx = 0; fnco_idx < ADI_APOLLO_NUM_NCOS_PER_DRC; fnco_idx++) {

        fnco_sel = fddc_sel << (4 * fnco_idx);       // Selects A0/A4, A1/A5, ... B3/B7 (pairs for 8t8r)

        /* Program fine nco */
        nco_pgm_config.drc_en = config->nco[fnco_idx].drc_en;
        nco_pgm_config.debug_drc_clkoff_n = config->nco[fnco_idx].debug_fdrc_clkoff_n;
        nco_pgm_config.if_mode = config->nco[fnco_idx].nco_if_mode;
        nco_pgm_config.mixer_sel = config->nco[fnco_idx].drc_mxr_sel;
        nco_pgm_config.cmplx_mxr_scale_en = config->nco[fnco_idx].cmplx_mxr_mult_scale_en;
        nco_pgm_config.profile_num = 0;         // TODO: add to dev profile
        nco_pgm_config.profile_sel_mode = config->nco[fnco_idx].nco_profile_sel_mode;
        nco_pgm_config.hop_mode_en = config->nco[fnco_idx].hop_mode_en;
        nco_pgm_config.main_phase_inc =  config->nco[fnco_idx].nco_phase_inc;
        nco_pgm_config.main_phase_offset = config->nco[fnco_idx].nco_phase_offset;
        nco_pgm_config.drc_phase_inc_frac_a = config->nco[fnco_idx].nco_phase_inc_frac_a;
        nco_pgm_config.drc_phase_inc_frac_b = config->nco[fnco_idx].nco_phase_inc_frac_b;
        nco_pgm_config.dc_testmode_value = config->nco[fnco_idx].dc_testmode_value;
        err = adi_apollo_fnco_pgm(device, ADI_APOLLO_RX, fnco_sel, &nco_pgm_config);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Load fine nco profiles */
        err = adi_apollo_fnco_profile_load(device, ADI_APOLLO_RX, fnco_sel,
                ADI_APOLLO_NCO_PROFILE_PHASE_INCREMENT, 0, config->nco[fnco_idx].nco_phase_inc_words, ADI_APOLLO_FNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_fnco_profile_load(device, ADI_APOLLO_RX, fnco_sel,
                ADI_APOLLO_NCO_PROFILE_PHASE_OFFSET, 0, config->nco[fnco_idx].nco_phase_offset_words, ADI_APOLLO_FNCO_PROFILE_NUM);
        ADI_APOLLO_ERROR_RETURN(err);

        /* Configure hopping */
        nco_hop_profile_config.profile_sel_mode = config->nco[fnco_idx].nco_profile_sel_mode;

        nco_hop_profile_config.nco_trig_hop_sel = config->nco[fnco_idx].nco_trig_hop_sel;
        nco_hop_profile_config.phase_inc_auto_mode = (adi_apollo_nco_auto_flip_incdir_e) config->nco[fnco_idx].nco_auto_inc_dec_freq;         /* TODO - add autoflip to enum */
        nco_hop_profile_config.phase_offset_auto_mode = (adi_apollo_nco_auto_flip_incdir_e )config->nco[fnco_idx].nco_auto_inc_dec_phase;     /* TODO - add autoflip to enum */
        nco_hop_profile_config.phase_inc_low_limit = 0;         // TODO: add to dev profile
        nco_hop_profile_config.phase_inc_high_limit = 31;       // TODO: add to dev profile
        nco_hop_profile_config.phase_offset_low_limit = 0;      // TODO: add to dev profile
        nco_hop_profile_config.phase_offset_high_limit = 31;    // TODO: add to dev profile
        nco_hop_profile_config.phase_handling = 0;
        err = adi_apollo_fnco_hop_pgm(device, ADI_APOLLO_RX, fnco_sel, &nco_hop_profile_config);

        /* Set initial hop index to 0. This is relevant when hop_mode_en is true  */
        err = adi_apollo_fnco_ftw_set(device, ADI_APOLLO_RX, fnco_sel, 0, 1, config->nco[fnco_idx].nco_phase_inc_words[0]);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_fnco_pow_set(device, ADI_APOLLO_RX, fnco_sel, 0, 1, config->nco[fnco_idx].nco_phase_offset_words[0]);
        ADI_APOLLO_ERROR_RETURN(err);

    }

    // Trigger selection mux here
    err = adi_apollo_trigts_fdrc_trig_sel_mux_set(device, ADI_APOLLO_RX, fddc_sel, ADI_APOLLO_TRIG_SPI);    /* TODO add to dev profile */
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rx_pfilt_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_pfilt_idx_e idx_pfilt, adi_apollo_pfilt_cfg_t *config)
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
        err = adi_apollo_pfilt_coeff_pgm(device, ADI_APOLLO_RX,
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

        err = adi_apollo_pfilt_gain_dly_pgm(device, ADI_APOLLO_RX,
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
    blk_mode_config.pfir_i_mode = config->enable ? config->i_mode : ADI_APOLLO_PFILT_MODE_DISABLED;
    blk_mode_config.pfir_q_mode = config->enable ? config->q_mode : ADI_APOLLO_PFILT_MODE_DISABLED;

    err = adi_apollo_pfilt_mode_pgm(device, ADI_APOLLO_RX,
                    ADI_APOLLO_PFILT_IDX2B(side, idx_pfilt),
                    &blk_mode_config);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rx_rxpath_misc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_rxpath_misc_t *config)
{
    int32_t err;
    adi_apollo_rx_misc_pgm_t rx_misc_pgm_params;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* RX crossbar1, ADC */
    err = adi_apollo_rxmux_xbar1_set(device,  ADI_APOLLO_SIDE_IDX2B(side), config->mux0_out_adc_sel,
            device->dev_info.is_8t8r ? ADI_APOLLO_RX_MUX0_NUM_8T8R : ADI_APOLLO_RX_MUX0_NUM_4T4R);
    ADI_APOLLO_ERROR_RETURN(err);

    /* RX crossbar2, CDDC-to-FDDC */
    err = adi_apollo_rxmux_xbar2_set(device,  ADI_APOLLO_SIDE_IDX2B(side), config->mux2_fddc_input_sel, ADI_APOLLO_RX_MUX2_NUM);
    ADI_APOLLO_ERROR_RETURN(err);

    /* RX datapath clock config and low sample rate  */
    rx_misc_pgm_params.datapath_clk = ADI_APOLLO_DATAPATH_CLK_ENABLE_INPHASE | ADI_APOLLO_DATAPATH_CLK_ENABLE_QPHASE;
    rx_misc_pgm_params.low_samp_en = config->low_samp_en;
    err = adi_apollo_rxmisc_pgm(device, ADI_APOLLO_SIDE_IDX2B(side), &rx_misc_pgm_params);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rx_cfir_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_cfir_idx_e idx_cfir, adi_apollo_cfir_cfg_t *config)
{
    int32_t err;
    uint8_t idx_profile, idx_set;
    adi_apollo_cfir_pgm_t blk_mode_config;
    uint16_t sparse_coeff_sel_sel[ADI_APOLLO_CFIR_NUM_TAPS] = {0};      /* TODO: add to device profile */
    uint8_t sparse_mem_sel[ADI_APOLLO_CFIR_MEM_SEL_NUM] = {0};          /* TODO: add to device profile */

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN((idx_cfir > ADI_APOLLO_CFIRS_PER_SIDE-1))

    /* CFIR top parameters */
    blk_mode_config.cfir_bypass = config->enable ? 0 : 1; //Bypass config
    blk_mode_config.cfir_sparse_filt_en = config->sparse_mode;
    blk_mode_config.cfir_32taps_en = config->cfir_mode; //cfir_32taps_en config
    blk_mode_config.cfir_coeff_transfer = 0; //coeff_transfer should go from 0 to 1

    err = adi_apollo_cfir_pgm(device, ADI_APOLLO_RX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir), &blk_mode_config);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Program CFIR coefficients, gain, scalar gain and sparse values for all profiles */
    for (idx_profile = 0; idx_profile < ADI_APOLLO_CFIR_NUM_PROFILES; idx_profile++) {
        for (idx_set = 0; idx_set < ADI_APOLLO_CFIR_COEFF_SETS; idx_set++) {
            err = adi_apollo_cfir_coeff_pgm(device, ADI_APOLLO_RX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                config->coeffs_i[idx_profile][idx_set], config->coeffs_q[idx_profile][idx_set], ADI_APOLLO_CFIR_NUM_TAPS);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_scalar_pgm(device, ADI_APOLLO_RX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                config->scalar_i[idx_profile][idx_set], config->scalar_q[idx_profile][idx_set]);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_gain_pgm(device, ADI_APOLLO_RX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                config->cfir_gain_dB[idx_profile][idx_set]);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_sparse_coeff_sel_pgm(device, ADI_APOLLO_RX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                sparse_coeff_sel_sel, ADI_APOLLO_CFIR_NUM_TAPS);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_cfir_sparse_mem_sel_pgm(device, ADI_APOLLO_RX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
                ADI_APOLLO_CFIR_PROFILE_IDX2B(idx_profile),
                ADI_APOLLO_CFIR_COEFF_SET_IDX2B(idx_set),
                sparse_mem_sel, ADI_APOLLO_CFIR_MEM_SEL_NUM);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* Make profile #0 active for all data paths. Transfers profile 0 coeffs and gains to CFIR block */
    err = adi_apollo_cfir_profile_sel(device, ADI_APOLLO_RX, ADI_APOLLO_CFIR_IDX2B(side, idx_cfir),
            ADI_APOLLO_CFIR_DP_ALL, ADI_APOLLO_CFIR_PROFILE_0);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rx_fsrc_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, adi_apollo_fsrc_idx_e idx_fsrc, adi_apollo_fsrc_cfg_t *config)
{
    int32_t err;
    adi_apollo_fsrc_pgm_t blk_mode_config;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
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

    err = adi_apollo_fsrc_pgm(device, ADI_APOLLO_RX,
                                        ADI_APOLLO_FSRC_IDX2B(side, idx_fsrc),
                                        &blk_mode_config);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rx_dformat_configure(adi_apollo_device_t *device, adi_apollo_sides_e side, 
        adi_apollo_jesd_links_e link_idx, adi_apollo_dformat_cfg_t *config, adi_apollo_jesd_tx_link_cfg_t *jtx_link_config)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(jtx_link_config);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN((side > ADI_APOLLO_NUM_SIDES - 1));
    ADI_APOLLO_INVALID_PARAM_RETURN((link_idx > ADI_APOLLO_JESD_LINKS - 1))

    adi_apollo_dformat_pgm_t dformat_config = {
        .dcm_ratio = jtx_link_config->link_dp_ratio,
        .total_dcm = jtx_link_config->link_total_ratio,
        .link_en = jtx_link_config->link_in_use,
        .dfor_ddc_dither_en = config->ddc_dither_en,
        .dfor_inv = config->inv,
        .dfor_res = config->res,
        .dfor_sel = config->sel,
        .invalid_en = config->rm_fifo.invalid_en,
        .sample_repeat_en = config->rm_fifo.sample_repeat_en
    };

    err = adi_apollo_dformat_pgm(device, ADI_APOLLO_DFMT_IDX2B(side, link_idx), &dformat_config);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}
/*! @} */
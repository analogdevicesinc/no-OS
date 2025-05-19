/*!
 * \brief     Coarse DDC functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CDDC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_cddc.h"
#include "adi_apollo_cnco.h"
#include "adi_apollo_ddc_local.h"
#include "adi_apollo_nco_local.h"
#include "adi_apollo_duc_local.h"
#include "adi_apollo_pfilt_local.h" //TODO: PFILT pgm dependency; Move to pfilt module
#include "adi_apollo_private_blk_sel_types.h"
#include "adi_apollo_bf_rx_cddc.h"
#include "adi_apollo_bf_rx_fine_ddc.h"
#include "adi_apollo_bf_txrx_coarse_nco.h"
#include "adi_apollo_bf_txrx_fine_nco.h"
#include "adi_apollo_bf_txrx_pfilt_top.h"
#include "adi_apollo_bf_jtx_dformat.h"
#include "adi_apollo_bf_rx_misc.h"
#include "adi_apollo_bf_custom.h"
#include "adi_apollo_bf_rx_datin.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_cddc_dcm_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, adi_apollo_coarse_ddc_dcm_e dcm)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddcs);

    for(i = 0; i < ADI_APOLLO_CDDC_NUM; i ++) {
        cddc = cddcs & (ADI_APOLLO_CDDC_A0 << i);
        if (cddc > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_COARSE_DDC_DEC_SEL_INFO(regmap_base_addr), dcm);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_link_num_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint8_t link_num)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddcs);

    for(i = 0; i < ADI_APOLLO_CDDC_NUM; i ++) {
        cddc = cddcs & (ADI_APOLLO_CDDC_A0 << i);
        if (cddc > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_LINK_NUM_RX_CDDC_INFO(regmap_base_addr), link_num);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_debug_clkoff_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint8_t clkoff_n)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddcs);

    for(i = 0; i < ADI_APOLLO_CDDC_NUM; i ++) {
        cddc = cddcs & (ADI_APOLLO_CDDC_A0 << i);
        if (cddc > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_CDDC_CLK_EN_INFO(regmap_base_addr), clkoff_n);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, adi_apollo_cddc_pgm_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddcs);

    for(i = 0; i < ADI_APOLLO_CDDC_NUM; i ++) {
        cddc = cddcs & (ADI_APOLLO_CDDC_A0 << i);
        if (cddc > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_COARSE_DDC_DEC_SEL_INFO(regmap_base_addr), config->dcm);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_LINK_NUM_RX_CDDC_INFO(regmap_base_addr), config->link_num);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CDDC_CLK_EN_INFO(regmap_base_addr), config->debug_clkoff_n);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_FINE_BYPASS_INFO(regmap_base_addr), config->fine_bypass);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HB1_DEL_RX_CDDC_INFO(regmap_base_addr), config->hb1_filt_dly_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HB2_DEL_RX_CDDC_INFO(regmap_base_addr), config->hb2_filt_dly_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TB1_DEL_INFO(regmap_base_addr), config->tb1_filt_dly);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HB1_GAIN_EN_RX_CDDC_INFO(regmap_base_addr), config->hb1_gain_6db_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TB1_GAIN_EN_INFO(regmap_base_addr), config->tb1_gain_6db_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddc, adi_apollo_cddc_inspect_t *cddc_inspect)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    adi_apollo_blk_sel_t cddc_x;
    adi_apollo_cnco_inspect_t cnco_inspect = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cddc_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(cddc) != 1);
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddc);

    // For 8t8r, regmap is shared for each split CDUC (A0/A2, A1/A3, ...)
    cddc_x = ((cddc & 0xCC) >> 2) | (cddc & 0x33);

    for(i = 0; i < ADI_APOLLO_CDDC_NUM; i ++) {
        if ((cddc & (ADI_APOLLO_CDDC_A0 << i)) > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            // CNCO A0-A1, B0-B1
            err = adi_apollo_cnco_inspect(device, ADI_APOLLO_RX, cddc_x, &cnco_inspect);
            ADI_APOLLO_ERROR_RETURN(err);
            cddc_inspect->dp_cfg.nco[0] = cnco_inspect.dp_cfg;

            // CNCO A2-A3, B2-B3
            if (device->dev_info.is_8t8r) {
                err = adi_apollo_cnco_inspect(device, ADI_APOLLO_RX, (cddc_x << 2), &cnco_inspect);
                ADI_APOLLO_ERROR_RETURN(err);
                cddc_inspect->dp_cfg.nco[1] = cnco_inspect.dp_cfg;
            }

            err = adi_apollo_hal_bf_get(device, BF_COARSE_DDC_DEC_SEL_INFO(regmap_base_addr), (uint8_t*) &(cddc_inspect->dp_cfg.drc_ratio), sizeof(cddc_inspect->dp_cfg.drc_ratio));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_LINK_NUM_RX_CDDC_INFO(regmap_base_addr), &(cddc_inspect->dp_cfg.link_num), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_FINE_BYPASS_INFO(regmap_base_addr), (uint8_t*) &(cddc_inspect->dp_cfg.fine_bypass), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_HB1_DEL_RX_CDDC_INFO(regmap_base_addr), (uint8_t*) &(cddc_inspect->dp_cfg.hb1_filt_dly_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_HB2_DEL_RX_CDDC_INFO(regmap_base_addr), (uint8_t*) &(cddc_inspect->dp_cfg.hb2_filt_dly_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_TB1_DEL_INFO(regmap_base_addr), (uint8_t *)&(cddc_inspect->dp_cfg.tb1_filt_dly_cycles), sizeof(cddc_inspect->dp_cfg.tb1_filt_dly_cycles));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_HB1_GAIN_EN_RX_CDDC_INFO(regmap_base_addr), (uint8_t*) &(cddc_inspect->dp_cfg.hb1_gain_6db_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_TB1_GAIN_EN_INFO(regmap_base_addr), (uint8_t*) &(cddc_inspect->dp_cfg.tb1_gain_6db_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Only one inspect per call */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_fine_bypass_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint8_t bypass_enable)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddcs);

    for(i = 0; i < ADI_APOLLO_CDDC_NUM; i ++) {
        cddc = cddcs & (ADI_APOLLO_CDDC_A0 << i);
        if (cddc > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FINE_BYPASS_INFO(regmap_base_addr), bypass_enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_dcm_bf_to_val(adi_apollo_device_t *device, adi_apollo_coarse_ddc_dcm_e bf_enum, uint32_t *val)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(val);

    switch (bf_enum) {
        case ADI_APOLLO_CDDC_DCM_1:
        case ADI_APOLLO_CDDC_DCM_2:
        case ADI_APOLLO_CDDC_DCM_3:
        case ADI_APOLLO_CDDC_DCM_4:
            *val = bf_enum + 1;
        break;

        case ADI_APOLLO_CDDC_DCM_6:
            *val = 6;
        break;

        case ADI_APOLLO_CDDC_DCM_12:
            *val = 12;
        break;

        default:
            ADI_APOLLO_LOG_ERR("Invalid adi_apollo_coarse_ddc_dcm_e enum");
            return API_CMS_ERROR_INVALID_PARAM;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_gain_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint16_t filt_sel, uint8_t enable)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t cddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddcs);

    for (i = 0; i < ADI_APOLLO_CDDC_NUM; i++) {
        cddc = cddcs & (ADI_APOLLO_CDDC_A0 << i);
        if (cddc > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            if ((filt_sel & ADI_APOLLO_CDDC_GAIN_HB1) != 0) {
                err = adi_apollo_hal_bf_set(device, BF_HB1_GAIN_EN_RX_CDDC_INFO(regmap_base_addr), (enable != 0) ? 1 : 0);
            }
            if ((filt_sel & ADI_APOLLO_CDDC_GAIN_TB1) != 0) {
                err = adi_apollo_hal_bf_set(device, BF_TB1_GAIN_EN_INFO(regmap_base_addr), (enable != 0) ? 1 : 0);
            }

            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cddc_gain_enable_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t cddcs, uint16_t filt_sel, uint8_t *enable)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t cddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(enable);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(cddcs) != 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(filt_sel) != 1);
    ADI_APOLLO_CDDC_BLK_SEL_MASK(cddcs);

    for (i = 0; i < ADI_APOLLO_CDDC_NUM; i++) {
        cddc = cddcs & (ADI_APOLLO_CDDC_A0 << i);
        if (cddc > 0) {
            regmap_base_addr = calc_rx_cddc_base(i);

            if ((filt_sel & ADI_APOLLO_CDDC_GAIN_HB1) != 0) {
                err = adi_apollo_hal_bf_get(device, BF_HB1_GAIN_EN_RX_CDDC_INFO(regmap_base_addr), enable, sizeof(*enable));
            }
            if ((filt_sel & ADI_APOLLO_CDDC_GAIN_TB1) != 0) {
                err = adi_apollo_hal_bf_get(device, BF_TB1_GAIN_EN_INFO(regmap_base_addr), enable, sizeof(*enable));
            }
            ADI_APOLLO_ERROR_RETURN(err);

            break;
        }
    }

    return API_CMS_ERROR_OK;
}



uint32_t calc_rx_cddc_base(int32_t cddc_index)
{
    static uint32_t rx_cddc_regmap[ADI_APOLLO_CDDC_NUM] = {
        RX_CDDC_RX_SLICE_0_RX_DIGITAL0, RX_CDDC_RX_SLICE_1_RX_DIGITAL0,
        RX_CDDC_RX_SLICE_0_RX_DIGITAL0, RX_CDDC_RX_SLICE_1_RX_DIGITAL0,
        RX_CDDC_RX_SLICE_0_RX_DIGITAL1, RX_CDDC_RX_SLICE_1_RX_DIGITAL1,
        RX_CDDC_RX_SLICE_0_RX_DIGITAL1, RX_CDDC_RX_SLICE_1_RX_DIGITAL1,
    };
    return rx_cddc_regmap[cddc_index];
}
/*! @} */
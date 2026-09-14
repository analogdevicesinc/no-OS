/*!
 * \brief     Fine DDC functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FDDC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_fddc.h"
#include "adi_apollo_fnco.h"
#include "adi_apollo_ddc_local.h"
#include "adi_apollo_nco_local.h"
#include "adi_apollo_duc_local.h"
#include "adi_apollo_pfilt_local.h"
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

int32_t adi_apollo_fddc_dcm_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, adi_apollo_fddc_ratio_e dcm)
{
    int32_t err;
    adi_apollo_blk_sel_t fddc;
    uint8_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddcs);

    for(i = 0; i < ADI_APOLLO_FDDC_NUM; i ++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_rx_fddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FINE_DDC_DEC_SEL_INFO(regmap_base_addr), dcm);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fddc_link_num_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t link_num)
{
    int32_t err;
    adi_apollo_blk_sel_t fddc;
    uint8_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddcs);

    for(i = 0; i < ADI_APOLLO_FDDC_NUM; i ++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_rx_fddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_LINK_NUM_RX_FINE_DDC_INFO(regmap_base_addr), link_num);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fddc_debug_clkoff_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t clkoff_n)
{
    int32_t err;
    adi_apollo_blk_sel_t fddc;
    uint8_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddcs);

    for(i = 0; i < ADI_APOLLO_FDDC_NUM; i ++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_rx_fddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FDDC_CLK_EN_INFO(regmap_base_addr), clkoff_n);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fddc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, adi_apollo_fddc_pgm_t *config)
{
    int32_t err;
    adi_apollo_blk_sel_t fddc;
    uint8_t i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddcs);

    for(i = 0; i < ADI_APOLLO_FDDC_NUM; i ++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_rx_fddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FINE_DDC_DEC_SEL_INFO(regmap_base_addr), config->dcm);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_LINK_NUM_RX_FINE_DDC_INFO(regmap_base_addr), config->link_num);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_FDDC_CLK_EN_INFO(regmap_base_addr), config->debug_clkoff_n);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_HB1_GAIN_EN_RX_FINE_DDC_INFO(regmap_base_addr), config->hb1_gain_6db_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fddc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddc, adi_apollo_fddc_inspect_t *fddc_inspect)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    adi_apollo_blk_sel_t fddc_x;
    adi_apollo_fnco_inspect_t fnco_inspect = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fddc_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(fddc) != 1);
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddc);

    // For 8t8r, regmap is shared for each split FDUC (A0/A4, A1/A5, ...)
    fddc_x = ((fddc & 0xF0F0) >> 4) | (fddc & 0x0F0F);

    for (i = 0; i < ADI_APOLLO_FDDC_NUM; i ++) {
        if ((fddc & (ADI_APOLLO_FDDC_A0 << i)) > 0) {
            regmap_base_addr = calc_rx_fddc_base(i);

            // FNCO A0-A3, B0-B3
            err = adi_apollo_fnco_inspect(device, ADI_APOLLO_RX, fddc_x, &fnco_inspect);
            ADI_APOLLO_ERROR_RETURN(err);
            fddc_inspect->dp_cfg.nco[0] = fnco_inspect.dp_cfg;

            // FNCO A4-A7, B4-B7
            if (device->dev_info.is_8t8r) {
                err = adi_apollo_fnco_inspect(device, ADI_APOLLO_RX, (fddc_x << 4), &fnco_inspect);
                ADI_APOLLO_ERROR_RETURN(err);
                fddc_inspect->dp_cfg.nco[1] = fnco_inspect.dp_cfg;
            }

            err = adi_apollo_hal_bf_get(device, BF_FINE_DDC_DEC_SEL_INFO(regmap_base_addr),(uint8_t*) &(fddc_inspect->dp_cfg.drc_ratio), sizeof(fddc_inspect->dp_cfg.drc_ratio));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_LINK_NUM_RX_FINE_DDC_INFO(regmap_base_addr), &(fddc_inspect->dp_cfg.link_num), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_FDDC_CLK_EN_INFO(regmap_base_addr), &(fddc_inspect->dp_cfg.debug_fddc_clkoff_n), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            
            err = adi_apollo_hal_bf_get(device, BF_HB1_GAIN_EN_RX_FINE_DDC_INFO(regmap_base_addr), (uint8_t*) &(fddc_inspect->dp_cfg.hb1_gain_6db_en), sizeof(fddc_inspect->dp_cfg.hb1_gain_6db_en));
            ADI_APOLLO_ERROR_RETURN(err);

            /* Only one inspect per call */
            break;            
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fddc_dcm_bf_to_val(adi_apollo_device_t *device, adi_apollo_fddc_ratio_e bf_enum, uint32_t *val)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(val);
    ADI_APOLLO_INVALID_PARAM_RETURN((bf_enum < ADI_APOLLO_FDDC_RATIO_1) || (bf_enum > ADI_APOLLO_FDDC_RATIO_64));

    *val = 1u << bf_enum;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fddc_gain_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t enable)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddcs);

    for (i = 0; i < ADI_APOLLO_FDDC_NUM; i++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_rx_fddc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_HB1_GAIN_EN_RX_FINE_DDC_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fddc_gain_enable_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t fddcs, uint8_t *enable)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(enable);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(fddcs) != 1);
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddcs);

    for (i = 0; i < ADI_APOLLO_FDDC_NUM; i++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_rx_fddc_base(i);

            err = adi_apollo_hal_bf_get(device, BF_HB1_GAIN_EN_RX_FINE_DDC_INFO(regmap_base_addr), enable, sizeof(*enable));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}


uint32_t calc_rx_fddc_base(int32_t fddc_index)
{
    static uint32_t rx_fddc_regmap[ADI_APOLLO_FDDC_NUM] = {
        RX_FINE_DDC0_RX_SLICE_0_RX_DIGITAL0, RX_FINE_DDC1_RX_SLICE_0_RX_DIGITAL0, RX_FINE_DDC0_RX_SLICE_1_RX_DIGITAL0, RX_FINE_DDC1_RX_SLICE_1_RX_DIGITAL0,
        RX_FINE_DDC0_RX_SLICE_0_RX_DIGITAL0, RX_FINE_DDC1_RX_SLICE_0_RX_DIGITAL0, RX_FINE_DDC0_RX_SLICE_1_RX_DIGITAL0, RX_FINE_DDC1_RX_SLICE_1_RX_DIGITAL0,
        RX_FINE_DDC0_RX_SLICE_0_RX_DIGITAL1, RX_FINE_DDC1_RX_SLICE_0_RX_DIGITAL1, RX_FINE_DDC0_RX_SLICE_1_RX_DIGITAL1, RX_FINE_DDC1_RX_SLICE_1_RX_DIGITAL1,
        RX_FINE_DDC0_RX_SLICE_0_RX_DIGITAL1, RX_FINE_DDC1_RX_SLICE_0_RX_DIGITAL1, RX_FINE_DDC0_RX_SLICE_1_RX_DIGITAL1, RX_FINE_DDC1_RX_SLICE_1_RX_DIGITAL1,
    };
    return rx_fddc_regmap[fddc_index];
}
/*! @} */
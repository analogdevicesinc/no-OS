/*!
 * \brief     Coarse DUC functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CDUC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_cduc.h"
#include "adi_apollo_cnco.h"
#include "adi_apollo_duc_local.h"
#include "adi_apollo_txmisc_local.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_tx_cduc.h"
#include "adi_apollo_bf_tx_misc.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

static const uint8_t cduc_sel_to_reg_bit[4] = { 0x01, 0x04, 0x10, 0x40 };

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_cduc_interp_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, uint8_t interp)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t cduc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDUC_BLK_SEL_MASK(cducs);

    for (i = 0; i < ADI_APOLLO_CDUC_NUM; i ++) {
        cduc = cducs & (ADI_APOLLO_CDUC_A0 << i);

        if (cduc > 0) {
            regmap_base_addr = calc_tx_cduc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_POST_INTERP_INFO(regmap_base_addr), interp);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cduc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, const adi_apollo_cduc_pgm_t *config)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t cduc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_CDUC_BLK_SEL_MASK(cducs);

    for (i = 0; i < ADI_APOLLO_CDUC_NUM; i ++) {
        cduc = cducs & (ADI_APOLLO_CDUC_A0 << i);

        if (cduc > 0) {
            regmap_base_addr = calc_tx_cduc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_POST_INTERP_INFO(regmap_base_addr), config->interp);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_TEST_MUX_INFO(regmap_base_addr), config->test_mux);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_CHB1_INT_TIME_DLY_INFO(regmap_base_addr), config->chb1_int_time_dly);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_CHB2_INT_TIME_DLY_INFO(regmap_base_addr), config->chb2_int_time_dly);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_CHB3_INT_TIME_DLY_INFO(regmap_base_addr), config->chb3_int_time_dly);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_CTB1_INT_TIME_DLY_INFO(regmap_base_addr), config->ctb1_int_time_dly);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* CDUC SPI enable configuration */
    err = adi_apollo_cduc_enable_set(device, cducs, config->cduc_spien_en, config->cduc_spi_en);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cduc_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, uint8_t cduc_spien_en, uint8_t cduc_spi_en)
{
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint8_t side_index;
    uint8_t bf_cduc_en = 0;
    uint8_t bits2change[2] = {
        (cducs >> 0)                            & ((1 << ADI_APOLLO_CDUC_PER_SIDE_NUM) - 1),
        (cducs >> ADI_APOLLO_CDUC_PER_SIDE_NUM) & ((1 << ADI_APOLLO_CDUC_PER_SIDE_NUM) - 1)
    };

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDUC_BLK_SEL_MASK(cducs);

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        if (bits2change[side_index]) {
            regmap_base_addr = calc_tx_misc_base(side_index); // One tx_misc regmap per side

            err = adi_apollo_hal_bf_set(device, BF_CDUC_SPI_EN_INFO(regmap_base_addr), cduc_spien_en ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_CDUC_EN_INFO(regmap_base_addr), &bf_cduc_en, sizeof(bf_cduc_en));
            ADI_APOLLO_ERROR_RETURN(err);

            bf_cduc_en &= ~bits2change[side_index];
            bf_cduc_en |= (cduc_spi_en ? bits2change[side_index] : 0);
            err = adi_apollo_hal_bf_set(device, BF_CDUC_EN_INFO(regmap_base_addr), bf_cduc_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cduc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t cduc, adi_apollo_cduc_inspect_t *cduc_inspect)
{
    int32_t err;
    uint8_t i;
    uint8_t duc_en_cfg_reg;
    uint32_t regmap_base_addr = 0;
    adi_apollo_blk_sel_t cduc_x;
    adi_apollo_cnco_inspect_t cnco_inspect = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cduc_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(cduc)!= 1);
    ADI_APOLLO_CDUC_BLK_SEL_MASK(cduc);

    // For 8t8r, regmap is shared for each split CDUC (A0/A2, A1/A3, ...)
    cduc_x = ((cduc & 0xCC) >> 2) | (cduc & 0x33);

    for(i = 0; i < ADI_APOLLO_CDUC_NUM; i ++) {
        if ((cduc & (ADI_APOLLO_CDUC_A0 << i)) > 0) {
            regmap_base_addr = calc_tx_cduc_base(i);

            // CNCO A0-A1, B0-B1
            err = adi_apollo_cnco_inspect(device, ADI_APOLLO_TX, cduc_x, &cnco_inspect);
            ADI_APOLLO_ERROR_RETURN(err);
            cduc_inspect->dp_cfg.nco[0] = cnco_inspect.dp_cfg;

            // CNCO A2-A3, B2-B3
            if (device->dev_info.is_8t8r) {
                err = adi_apollo_cnco_inspect(device, ADI_APOLLO_TX, (cduc_x << 2), &cnco_inspect);
                ADI_APOLLO_ERROR_RETURN(err);
                cduc_inspect->dp_cfg.nco[1] = cnco_inspect.dp_cfg;
            }

            err = adi_apollo_hal_bf_get(device, BF_POST_INTERP_INFO(regmap_base_addr), (uint8_t *) &(cduc_inspect->dp_cfg.drc_ratio), sizeof(cduc_inspect->dp_cfg.drc_ratio));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_TEST_MUX_INFO(regmap_base_addr), &(cduc_inspect->test_mux), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_CHB1_INT_TIME_DLY_INFO(regmap_base_addr), &(cduc_inspect->chb1_int_time_dly), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_CHB2_INT_TIME_DLY_INFO(regmap_base_addr), &(cduc_inspect->chb2_int_time_dly), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_CHB3_INT_TIME_DLY_INFO(regmap_base_addr), &(cduc_inspect->chb3_int_time_dly), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_CTB1_INT_TIME_DLY_INFO(regmap_base_addr), &(cduc_inspect->ctb1_int_time_dly), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* CDUC SPI enable */
            regmap_base_addr = calc_tx_misc_base(i / ADI_APOLLO_CDUC_PER_SIDE_NUM);        // Regmap base address for side

            err = adi_apollo_hal_bf_get(device, BF_CDUC_SPI_EN_INFO(regmap_base_addr), &(cduc_inspect->cduc_spien_en), sizeof(cduc_inspect->cduc_spien_en)); // spi control enable for side
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_reg_get(device, REG_DUC_EN_CFG_ADDR(regmap_base_addr), &duc_en_cfg_reg);
            ADI_APOLLO_ERROR_RETURN(err);
            cduc_inspect->cduc_spi_en = duc_en_cfg_reg & cduc_sel_to_reg_bit[i % ADI_APOLLO_CDUC_PER_SIDE_NUM];

            break;      // Only one block inspect per call
        }

    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cduc_interp_bf_to_val(adi_apollo_device_t* device, adi_apollo_coarse_duc_dcm_e bf_enum, uint32_t* val)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(val);

    switch (bf_enum) {
    case ADI_APOLLO_CDUC_INTERP_1:
    case ADI_APOLLO_CDUC_INTERP_2:
    case ADI_APOLLO_CDUC_INTERP_3:
    case ADI_APOLLO_CDUC_INTERP_4:
    case ADI_APOLLO_CDUC_INTERP_6:
    case ADI_APOLLO_CDUC_INTERP_8:
    case ADI_APOLLO_CDUC_INTERP_12:
        *val = bf_enum;
        break;

    default:
        ADI_APOLLO_LOG_ERR("Invalid adi_apollo_coarse_duc_dcm_e enum");
        return API_CMS_ERROR_INVALID_PARAM;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cduc_irq_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t cducs, uint8_t enable)
{
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CDUC_BLK_SEL_MASK(cducs);

    for(i = 0; i < ADI_APOLLO_CDUC_NUM; i ++) {
        if ((cducs & (ADI_APOLLO_CDUC_A0 << i)) > 0) {
            regmap_base_addr = calc_tx_cduc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_CDUC_IRQ_EN0_INFO(regmap_base_addr), enable ? 0x3FF : 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_tx_cduc_base(int32_t idx)
{
    static const uint32_t tx_cduc_regmap[ADI_APOLLO_CDUC_NUM] = {
        TX_CDUC_TX_SLICE_0_TX_DIGITAL0, TX_CDUC_TX_SLICE_1_TX_DIGITAL0, TX_CDUC_TX_SLICE_0_TX_DIGITAL0, TX_CDUC_TX_SLICE_1_TX_DIGITAL0,
        TX_CDUC_TX_SLICE_0_TX_DIGITAL1, TX_CDUC_TX_SLICE_1_TX_DIGITAL1, TX_CDUC_TX_SLICE_0_TX_DIGITAL1, TX_CDUC_TX_SLICE_1_TX_DIGITAL1,
    };

    return tx_cduc_regmap[idx];
}

/*! @} */

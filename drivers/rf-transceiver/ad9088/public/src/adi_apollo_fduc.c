/*!
 * \brief     Fine DUC functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FDUC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_fduc.h"
#include "adi_apollo_fnco.h"
#include "adi_apollo_duc_local.h"
#include "adi_apollo_txmisc_local.h"
#include "adi_apollo_txmisc_types.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_tx_fduc.h"
#include "adi_apollo_bf_tx_misc.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

static const uint8_t fduc_sel_to_reg_bit[8] = { 0x01, 0x04, 0x10, 0x40, 0x02, 0x08, 0x20, 0x80 };

uint32_t calc_fduc_irq_offset(int32_t idx);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_fduc_interp_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t interp)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fduc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fducs);

    for (i = 0; i < ADI_APOLLO_FDUC_NUM; i++) {
        fduc = fducs & (ADI_APOLLO_FDUC_A0 << i);
        if (fduc > 0) {
            regmap_base_addr = calc_tx_fduc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FDUC_INTERP_INFO(regmap_base_addr), interp);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fduc_subdp_gain_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t enable)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fduc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fducs);

    for (i = 0; i < ADI_APOLLO_FDUC_NUM; i++) {
        fduc = fducs & (ADI_APOLLO_FDUC_A0 << i);
        if (fduc > 0) {
            regmap_base_addr = calc_tx_fduc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SUB_DP_GAIN_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fduc_subdp_gain_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint16_t gain)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fduc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fducs);

    for (i = 0; i < ADI_APOLLO_FDUC_NUM; i++) {
        fduc = fducs & (ADI_APOLLO_FDUC_A0 << i);
        if (fduc > 0) {
            regmap_base_addr = calc_tx_fduc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SUBDP_GAIN_INFO(regmap_base_addr), gain);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fduc_int_tdly_hb_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t dly)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fduc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fducs);

    for (i = 0; i < ADI_APOLLO_FDUC_NUM; i++) {
        fduc = fducs & (ADI_APOLLO_FDUC_A0 << i);
        if (fduc > 0) {
            regmap_base_addr = calc_tx_fduc_base(i);

            err = adi_apollo_hal_reg_set(device, REG_INT_TIME_DELAY_ADDR(regmap_base_addr), dly);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fduc_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, const adi_apollo_fduc_pgm_t *config)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t fduc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fducs);

    for (i = 0; i < ADI_APOLLO_FDUC_NUM; i++) {
        fduc = fducs & (ADI_APOLLO_FDUC_A0 << i);

        if (fduc > 0) {
            regmap_base_addr = calc_tx_fduc_base(i);

            err = adi_apollo_hal_bf_set(device, BF_FDUC_INTERP_INFO(regmap_base_addr), config->interp);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_SUB_DP_GAIN_EN_INFO(regmap_base_addr), config->sub_dp_gain_en);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_SUBDP_GAIN_INFO(regmap_base_addr), config->subdp_gain);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_reg_set(device, REG_INT_TIME_DELAY_ADDR(regmap_base_addr), config->int_tdly_hb);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* FDUC SPI enable configuration */
    err = adi_apollo_fduc_enable_set(device, fducs, config->fduc_spien_en, config->fduc_spi_en);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fduc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t fduc, adi_apollo_fduc_inspect_t *fduc_inspect)
{
    int32_t err;
    uint8_t i;
    uint8_t fduc_en_reg;
    uint32_t regmap_base_addr = 0;
    adi_apollo_blk_sel_t fduc_x;
    adi_apollo_fnco_inspect_t fnco_inspect = {{0}};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fduc_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(fduc) != 1);
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fduc);

    // For 8t8r, regmap is shared for each split FDUC (A0/A4, A1/A5, ...)
    fduc_x = ((fduc & 0xF0F0) >> 4) | (fduc & 0x0F0F);

    for (i = 0; i < ADI_APOLLO_FDUC_NUM; i++) {
        if ((fduc_x & (ADI_APOLLO_FDUC_A0 << i)) > 0) {
            regmap_base_addr = calc_tx_fduc_base(i);

            // FNCO A0-A3, B0-B3
            err = adi_apollo_fnco_inspect(device, ADI_APOLLO_TX, fduc_x, &fnco_inspect);
            ADI_APOLLO_ERROR_RETURN(err);
            fduc_inspect->dp_cfg.nco[0] = fnco_inspect.dp_cfg;

            // FNCO A4-A7, B4-B7
            if (device->dev_info.is_8t8r) {
                err = adi_apollo_fnco_inspect(device, ADI_APOLLO_TX, (fduc_x << 4), &fnco_inspect);
                ADI_APOLLO_ERROR_RETURN(err);
                fduc_inspect->dp_cfg.nco[1] = fnco_inspect.dp_cfg;
            }
            
            err = adi_apollo_hal_bf_get(device, BF_FDUC_INTERP_INFO(regmap_base_addr), &(fduc_inspect->dp_cfg.drc_ratio), sizeof(fduc_inspect->dp_cfg.drc_ratio));
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_SUB_DP_GAIN_EN_INFO(regmap_base_addr), (uint8_t *) &(fduc_inspect->dp_cfg.sub_dp_gain_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_get(device, BF_SUBDP_GAIN_INFO(regmap_base_addr), (uint8_t *) &(fduc_inspect->dp_cfg.subdp_gain), 2);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_reg_get(device, REG_INT_TIME_DELAY_ADDR(regmap_base_addr), &(fduc_inspect->int_tdly_hb));
            ADI_APOLLO_ERROR_RETURN(err);

            /* FDUC SPI enable (these are usually disabled in favor of auto select) */
            regmap_base_addr = calc_tx_misc_base(i / ADI_APOLLO_FDUC_PER_SIDE_NUM);        // Regmap base address for side

            err = adi_apollo_hal_bf_get(device, BF_FDUC_SPI_EN_INFO(regmap_base_addr), &(fduc_inspect->fduc_spien_en), sizeof(fduc_inspect->fduc_spien_en)); // spi control enable for side
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_reg_get(device, REG_FDUC_EN_SPI_ADDR(regmap_base_addr), &fduc_en_reg);
            ADI_APOLLO_ERROR_RETURN(err);
            fduc_inspect->fduc_spi_en = fduc_en_reg & fduc_sel_to_reg_bit[i % ADI_APOLLO_FDUC_PER_SIDE_NUM];

            /* Only one inspect per call */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fduc_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t fduc_spien_en, uint8_t fduc_spi_en)
{
    int32_t err;
    uint16_t i, j;
    adi_apollo_blk_sel_t fduc;
    uint32_t regmap_base_addr;
    adi_apollo_blk_sel_t fducs_side;
    uint8_t fduc_en_reg;
    uint8_t fduc_en_reg_mask, fduc_en_reg_val;
    uint8_t sel_mask = (1 << ADI_APOLLO_FDUC_PER_SIDE_NUM) - 1;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fducs);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {

        fducs_side = (fducs >> ((i == 0) ? 0 : ADI_APOLLO_FDUC_PER_SIDE_NUM)) & sel_mask;

        if (!fducs_side) {
            continue;
        }

        regmap_base_addr = calc_tx_misc_base(i);        // Regmap base address for side

        err = adi_apollo_hal_bf_set(device, BF_FDUC_SPI_EN_INFO(regmap_base_addr), fduc_spien_en != 0 ? 1 : 0); // enable spi control for side
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_reg_get(device, REG_FDUC_EN_SPI_ADDR(regmap_base_addr), &fduc_en_reg);
        ADI_APOLLO_ERROR_RETURN(err);

        fduc_en_reg_mask = 0;
        fduc_en_reg_val = 0;
        for (j = 0; j < ADI_APOLLO_FDUC_PER_SIDE_NUM; j++) {
            fduc = fducs_side & (ADI_APOLLO_FDUC_A0 << j);

            /* Only modify selected FDUCs */
            if (fduc) {
                fduc_en_reg_mask |= fduc_sel_to_reg_bit[j];
                fduc_en_reg_val |= (fduc_sel_to_reg_bit[j] & (fduc_spi_en ? 0xFF : 00));
            }
        }

        fduc_en_reg &= ~(fduc_en_reg_mask);
        fduc_en_reg |= fduc_en_reg_val;

        err = adi_apollo_hal_reg_set(device, REG_FDUC_EN_SPI_ADDR(regmap_base_addr), fduc_en_reg);  // set the FDUC enable mask
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_fduc_interp_bf_to_val(adi_apollo_device_t* device, adi_apollo_fduc_ratio_e bf_enum, uint32_t* val)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(val);

    switch (bf_enum) {
    case ADI_APOLLO_FDUC_RATIO_1:
    case ADI_APOLLO_FDUC_RATIO_2:
    case ADI_APOLLO_FDUC_RATIO_4:
    case ADI_APOLLO_FDUC_RATIO_8:
    case ADI_APOLLO_FDUC_RATIO_16:
    case ADI_APOLLO_FDUC_RATIO_32:
    case ADI_APOLLO_FDUC_RATIO_64:
        *val = bf_enum;
        break;

    default:
        ADI_APOLLO_LOG_ERR("Invalid adi_apollo_fduc_ratio_e enum");
        return API_CMS_ERROR_INVALID_PARAM;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_fduc_irq_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t fducs, uint8_t enable)
{
    int32_t err;
    uint16_t fduc_index;
    adi_apollo_blk_sel_t fduc;
    uint32_t regmap_base_addr = 0, regmap_info = 0x600, regmap_offset = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FDUC_BLK_SEL_MASK(fducs);

    for (fduc_index = 0;  fduc_index < ADI_APOLLO_FDUC_NUM; fduc_index++) {
        fduc = fducs & (ADI_APOLLO_FDUC_A0 << fduc_index);
        if (fduc > 0) {
            regmap_base_addr = calc_tx_misc_base(fduc_index / ADI_APOLLO_FDUC_PER_SIDE_NUM);
            regmap_offset = calc_fduc_irq_offset(fduc % ADI_APOLLO_FDUC_PER_SIDE_NUM);
            
            err = adi_apollo_hal_bf_set(device, regmap_base_addr + regmap_offset, regmap_info, enable ? 0x3F : 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_tx_fduc_base(int32_t idx)
{
    static const uint32_t tx_fduc_regmap[ADI_APOLLO_FDUC_NUM] = {
        TX_FDUC0_TX_SLICE_0_TX_DIGITAL0, TX_FDUC1_TX_SLICE_0_TX_DIGITAL0, TX_FDUC0_TX_SLICE_1_TX_DIGITAL0, TX_FDUC1_TX_SLICE_1_TX_DIGITAL0,
        TX_FDUC0_TX_SLICE_0_TX_DIGITAL0, TX_FDUC1_TX_SLICE_0_TX_DIGITAL0, TX_FDUC0_TX_SLICE_1_TX_DIGITAL0, TX_FDUC1_TX_SLICE_1_TX_DIGITAL0,
        TX_FDUC0_TX_SLICE_0_TX_DIGITAL1, TX_FDUC1_TX_SLICE_0_TX_DIGITAL1, TX_FDUC0_TX_SLICE_1_TX_DIGITAL1, TX_FDUC1_TX_SLICE_1_TX_DIGITAL1,
        TX_FDUC0_TX_SLICE_0_TX_DIGITAL1, TX_FDUC1_TX_SLICE_0_TX_DIGITAL1, TX_FDUC0_TX_SLICE_1_TX_DIGITAL1, TX_FDUC1_TX_SLICE_1_TX_DIGITAL1,
    };

    return tx_fduc_regmap[idx];
}

uint32_t calc_fduc_irq_offset(int32_t idx)
{
    static const uint32_t tx_misc_regmap[ADI_APOLLO_FDUC_PER_SIDE_NUM] = {
        REG_SL0_FDUC0_HB_IRQ_EN0_ADDR(0),
        REG_SL0_FDUC1_IRQ_ADDR(0),
        REG_SL1_FDUC0_HB_IRQ_EN0_ADDR(0),
        REG_SL1_FDUC1_IRQ_ADDR(0)        
    };

    return tx_misc_regmap[idx];
}

/*! @} */

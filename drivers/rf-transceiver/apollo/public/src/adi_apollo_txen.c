/*!
 * \brief    APIs for TxEn
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXEN
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_txen.h"
#include "adi_apollo_txen_local.h"
#include "adi_apollo_pfilt.h"
#include "adi_apollo_cduc.h"
#include "adi_apollo_fduc.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_txen_power_ctrl.h"
#include "adi_apollo_bf_txrx_enable.h"
#include "adi_apollo_bf_tx_hsdout.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_txen_pwrup_ctrl_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_blk_config_t *config)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_TXEN_BLK_SEL_MASK(dacs);

    err = adi_apollo_txen_pwrup_ctrl_set(device, dacs, &config->ctrl);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_txen_pwrup_ctrl_edge_set(device, dacs, &config->edge);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_txen_pwrup_ctrl_count_set(device, dacs, &config->count);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_txen_pwrup_ctrl_pin_set(device, dacs, config->pin_en);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txen_pwrup_ctrl_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_ctrl_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t dac;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_TXEN_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_TXEN_DAC_A0 << i);
        if (dac > 0) {
            regmap_base_addr = calc_tx_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SM_EN_INFO(regmap_base_addr), config->sm_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SPI_TXEN_ENA_INFO(regmap_base_addr), config->spi_txen_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SPI_TXEN_INFO(regmap_base_addr), config->spi_txen);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SEL_CNT_RATE_INFO(regmap_base_addr), 
                                        (config->sm_clk_rate == ADI_APOLLO_PUC_CLK_RATE_FS_DIV_256) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txen_pwrup_ctrl_edge_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_ctrl_edge_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t dac;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_TXEN_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_TXEN_DAC_A0 << i);
        if (dac > 0) {
            regmap_base_addr = calc_tx_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_DAC_RISE_INFO(regmap_base_addr), config->dac_rise);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DAC_FALL_INFO(regmap_base_addr), config->dac_fall);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DIG_RISE_INFO(regmap_base_addr), config->dig_rise);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DIG_FALL_INFO(regmap_base_addr), config->dig_fall);
            ADI_APOLLO_ERROR_RETURN(err);            
            err = adi_apollo_hal_bf_set(device, BF_PA_RISE_INFO(regmap_base_addr), config->pa_rise);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_PA_FALL_INFO(regmap_base_addr), config->pa_fall);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txen_pwrup_ctrl_count_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, const adi_apollo_txen_pwrup_ctrl_count_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t dac;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_TXEN_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_TXEN_DAC_A0 << i);
        if (dac > 0) {
            regmap_base_addr = calc_tx_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_COUNT_MAXA_INFO(regmap_base_addr), config->count_maxa);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_COUNT_MAXB_INFO(regmap_base_addr), config->count_maxb);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_COUNT_MAXC_INFO(regmap_base_addr), config->count_maxc);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_COUNT_MAXD_INFO(regmap_base_addr), config->count_maxd);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_COUNT_MAXE_INFO(regmap_base_addr), config->count_maxe);
            ADI_APOLLO_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_COUNT_MAXF_INFO(regmap_base_addr), config->count_maxf);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txen_pwrup_ctrl_pin_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, adi_apollo_puc_en_pin_sel_e pin)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t dac;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_TXEN_BLK_SEL_MASK(dacs);

    for (i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_TXEN_DAC_A0 << i);
        if (dac > 0) {
            regmap_base_addr = calc_tx_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_TXEN_SEL_INFO(regmap_base_addr), pin);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txen_pwrup_ctrl_sm_en_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t sm_en)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t dac;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_TXEN_BLK_SEL_MASK(dacs);
    
    for (i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_TXEN_DAC_A0 << i);
        if (dac > 0) {
            regmap_base_addr = calc_tx_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SM_EN_INFO(regmap_base_addr), sm_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;

}

int32_t adi_apollo_txen_pwrup_ctrl_sel_cnt_rate_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t sel_cnt_rate)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t dac;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_TXEN_BLK_SEL_MASK(dacs);
    
    for (i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_TXEN_DAC_A0 << i);
        if (dac > 0) {
            regmap_base_addr = calc_tx_txen_power_ctrl_base(i);
            err = adi_apollo_hal_bf_set(device, BF_SEL_CNT_RATE_INFO(regmap_base_addr), 
                                        (sel_cnt_rate == ADI_APOLLO_PUC_CLK_RATE_FS_DIV_256) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;

}

int32_t adi_apollo_txen_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t tx_enables, const adi_apollo_txen_blk_config_t *config)
{
    int32_t err;
    uint8_t i, side;
    adi_apollo_blk_sel_t tx_en;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_TXEN_BLK_SEL_MASK(tx_enables);

    for (i = 0; i < ADI_APOLLO_NUM_TX_EN; i ++) {
        tx_en = tx_enables & (ADI_APOLLO_TX_EN_A0 << i);        
        if (tx_en > 0) {
            side = i / ADI_APOLLO_NUM_TX_EN_PER_SIDE;
            regmap_base_addr = calc_tx_enable_base(side);

            /* Enable polarity */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_POL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)), 
                                        (config->enable_polarity == ADI_APOLLO_TXEN_EN_ACTIVE_HIGH) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* SPI enable */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_SPI_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)), 
                                        (config->spi_en == ADI_APOLLO_DISABLE) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* SPIEN enable */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_SPIEN_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)), 
                                        (config->spien_en == ADI_APOLLO_DISABLE) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Slice select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_SLICE_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->slice_sel : config->slice_sel >> ADI_APOLLO_MAX_SLICES_PER_SIDE_NUM);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Linx select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_LINX_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->linx_sel : config->linx_sel >> ADI_APOLLO_NUM_LINX_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* PFILT select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_PFILT_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->pfilt_sel : config->pfilt_sel >> ADI_APOLLO_PFILT_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* CDUC select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_CDUC_CDDC_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->cduc_sel : config->cduc_sel >> ADI_APOLLO_CDUC_PER_SIDE_NUM);
            ADI_APOLLO_ERROR_RETURN(err);

            /* FDUC select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_FDUC_FDDC_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->fduc_sel : config->fduc_sel >> ADI_APOLLO_FDUC_PER_SIDE_NUM);
            ADI_APOLLO_ERROR_RETURN(err);

            /* CFIR select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_CFIR_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->cfir_sel : config->cfir_sel >> ADI_APOLLO_NUM_CFIR_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* FSRC select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_FSRC_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->fsrc_sel : config->fsrc_sel >> ADI_APOLLO_NUM_FSRC_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* JRX link select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_JRX_JTX_LINK_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->jrx_link_sel : config->jrx_link_sel >> ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);
            
            /* JRX phy0 & phy1 select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_JRX_JTX_PHY_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->jrx_phy_sel_side_a : config->jrx_phy_sel_side_b);
            ADI_APOLLO_ERROR_RETURN(err);            
        
            /* MODSW select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_MODSW_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (config->modsw_sel == ADI_APOLLO_DISABLE) ? 0 : 1);                                        
            ADI_APOLLO_ERROR_RETURN(err);

            /* INVSINC select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_INVSINC_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->invsinc_sel : config->invsinc_sel >> ADI_APOLLO_NUM_TX_INVSINC_PER_SIDE);                                        
            ADI_APOLLO_ERROR_RETURN(err);

            /* GAIN select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_GAIN_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->gain_sel : config->gain_sel >> ADI_APOLLO_NUM_TX_GAIN_PER_SIDE);                                        
            ADI_APOLLO_ERROR_RETURN(err);

            /* SRD select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_SRD_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_TX_EN_PER_SIDE)),
                                        (side == 0) ? config->srd_sel : config->srd_sel >> ADI_APOLLO_NUM_TX_SRD_PER_SIDE);                                        
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_tx_txen_power_ctrl_base(int32_t dac_index)
{
    static uint32_t tx_txen_power_ctrl_regmap[] = {
        TXEN_POWER_CTRL0_TX_SLICE_0_TX_DIGITAL0, TXEN_POWER_CTRL1_TX_SLICE_0_TX_DIGITAL0,
        TXEN_POWER_CTRL0_TX_SLICE_1_TX_DIGITAL0, TXEN_POWER_CTRL1_TX_SLICE_1_TX_DIGITAL0,
        TXEN_POWER_CTRL0_TX_SLICE_0_TX_DIGITAL1, TXEN_POWER_CTRL1_TX_SLICE_0_TX_DIGITAL1,
        TXEN_POWER_CTRL0_TX_SLICE_1_TX_DIGITAL1, TXEN_POWER_CTRL1_TX_SLICE_1_TX_DIGITAL1
    };

    return (tx_txen_power_ctrl_regmap[dac_index]);
}

uint32_t calc_tx_enable_base(int32_t index)
{
    static uint32_t tx_enable_regmap[] = {
        TXRX_ENABLE_TX_SLICE_0_TX_DIGITAL0, TXRX_ENABLE_TX_SLICE_0_TX_DIGITAL1
    };

    return (tx_enable_regmap[index]);
}

/*! @} */

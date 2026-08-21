/*!
 * \brief     APIs for Rx power control
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXEN
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_rxen.h"
#include "adi_apollo_rxen_local.h"
#include "adi_apollo_pfilt.h"
#include "adi_apollo_cddc.h"
#include "adi_apollo_fddc.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_conv_pow_onoff.h"
#include "adi_apollo_bf_txen_power_ctrl.h"
#include "adi_apollo_bf_txrx_enable.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_rxen_pwrup_ctrl_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_blk_config_t *config)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_RXEN_BLK_SEL_MASK(adcs);

    err = adi_apollo_rxen_pwrup_ctrl_set(device, adcs, &config->ctrl);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_rxen_pwrup_ctrl_edge_set(device, adcs, &config->edge);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_rxen_pwrup_ctrl_count_set(device, adcs, &config->count);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_rxen_pwrup_ctrl_pin_set(device, adcs, config->pin_en);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxen_pwrup_ctrl_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_ctrl_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t adc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_RXEN_BLK_SEL_MASK(adcs);

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i ++) {
        adc = adcs & (ADI_APOLLO_RXEN_ADC_A0 << i);
        if (adc > 0) {
            regmap_base_addr = calc_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SM_EN_INFO(regmap_base_addr), config->sm_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SPI_TXEN_ENA_INFO(regmap_base_addr), config->spi_rxen_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SPI_TXEN_INFO(regmap_base_addr), config->spi_rxen);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SEL_CNT_RATE_INFO(regmap_base_addr), (config->sm_clk_rate == ADI_APOLLO_PUC_CLK_RATE_FS_DIV_256) ? 1 : 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxen_pwrup_ctrl_edge_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_ctrl_edge_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t adc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_RXEN_BLK_SEL_MASK(adcs);

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i ++) {
        adc = adcs & (ADI_APOLLO_RXEN_ADC_A0 << i);
        if (adc > 0) {
            regmap_base_addr = calc_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_DAC_RISE_INFO(regmap_base_addr), config->adc_rise);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_DAC_FALL_INFO(regmap_base_addr), config->adc_fall);
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

int32_t adi_apollo_rxen_pwrup_ctrl_count_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_rxen_pwrup_ctrl_count_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t adc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_RXEN_BLK_SEL_MASK(adcs);

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i ++) {
        adc = adcs & (ADI_APOLLO_RXEN_ADC_A0 << i);
        if (adc > 0) {
            regmap_base_addr = calc_txen_power_ctrl_base(i);

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

int32_t adi_apollo_rxen_pwrup_ctrl_pin_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_puc_en_pin_sel_e pin)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t adc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_RXEN_BLK_SEL_MASK(adcs);

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i ++) {
        adc = adcs & (ADI_APOLLO_RXEN_ADC_A0 << i);
        if (adc > 0) {
            regmap_base_addr = calc_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_TXEN_SEL_INFO(regmap_base_addr), pin);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxen_pwrup_ctrl_spien_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint8_t spi_en)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t adc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_RXEN_BLK_SEL_MASK(adcs);

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i ++) {
        adc = adcs & (ADI_APOLLO_RXEN_ADC_A0 << i);
        if (adc > 0) {
            regmap_base_addr = calc_txen_power_ctrl_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SPI_TXEN_INFO(regmap_base_addr), spi_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}



int32_t adi_apollo_rxen_config_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dig_enables, const adi_apollo_rxen_blk_config_t *config) 
{
    int32_t err;
    uint8_t i, side;
    adi_apollo_blk_sel_t dig_en;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(config->pfilt_sel > ADI_APOLLO_PFILT_ALL);
    ADI_APOLLO_RXEN_BLK_SEL_MASK(dig_enables);
    
    for(i = 0; i < ADI_APOLLO_DIG_EN_NUM; i ++) {
        dig_en = dig_enables & (ADI_APOLLO_RXEN_EN_A0 << i);
        if (dig_en > 0) {
            side = i / ADI_APOLLO_DIG_EN_PER_SIDE_NUM;
            regmap_base_addr = calc_rx_enable_base(side);

            /* Enable polarity */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_POL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (config->enable_polarity == ADI_APOLLO_DIG_EN_ACTIVE_HIGH) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* SPI enable */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_SPI_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (config->spi_en == ADI_APOLLO_DISABLE) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* SPIEN enable */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_SPIEN_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (config->spien_en == ADI_APOLLO_DISABLE) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Slice select - other selects are OR'd with slice */
            err = adi_apollo_hal_bf_set(device,
                                        BF_SLICE_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)),
                                        (side == 0) ? config->slice_sel : config->slice_sel >> ADI_APOLLO_MAX_SLICES_PER_SIDE_NUM);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Linx select */
            err = adi_apollo_hal_bf_set(device,
                                        BF_LINX_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)),
                                        (side == 0) ? config->linx_sel : config->linx_sel >> ADI_APOLLO_NUM_LINX_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* PFILT select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_PFILT_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (side == 0) ? config->pfilt_sel : config->pfilt_sel >> ADI_APOLLO_PFILT_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* CDDC select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_CDUC_CDDC_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (side == 0) ? config->cddc_sel : config->cddc_sel >> ADI_APOLLO_CDDC_PER_SIDE_NUM);
            ADI_APOLLO_ERROR_RETURN(err);

            /* FDDC select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_FDUC_FDDC_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (side == 0) ? config->fddc_sel : config->fddc_sel >> ADI_APOLLO_FDDC_PER_SIDE_NUM);
            ADI_APOLLO_ERROR_RETURN(err);

            /* CFIR select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_CFIR_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (side == 0) ? config->cfir_sel : config->cfir_sel >> ADI_APOLLO_NUM_CFIR_PER_SIDE);

            /* FSRC select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_FSRC_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (side == 0) ? config->fsrc_sel : config->fsrc_sel >> ADI_APOLLO_NUM_FSRC_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* JTX link select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_JRX_JTX_LINK_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (side == 0) ? config->jtx_link_sel : config->jtx_link_sel >> ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE);
            ADI_APOLLO_ERROR_RETURN(err);

            /* JTX phy select */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_JRX_JTX_PHY_SEL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (side == 0) ? config->jtx_phy_sel_side_a : config->jtx_phy_sel_side_b);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxen_ctrl_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dig_enables, const adi_apollo_rxen_ctrl_t *config)
{
    int32_t err;
    uint8_t i, side;
    adi_apollo_blk_sel_t dig_en;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_RXEN_BLK_SEL_MASK(dig_enables);
    
    for(i = 0; i < ADI_APOLLO_DIG_EN_NUM; i ++) {
        dig_en = dig_enables & (ADI_APOLLO_RXEN_EN_A0 << i);
        if (dig_en > 0) {
            side = i / ADI_APOLLO_DIG_EN_PER_SIDE_NUM;
            regmap_base_addr = calc_rx_enable_base(side);

            /* Enable polarity */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_POL_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (config->enable_polarity == ADI_APOLLO_DIG_EN_ACTIVE_HIGH) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* SPI enable */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_SPI_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (config->spi_en == ADI_APOLLO_DISABLE) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* SPIEN enable */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_SPIEN_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (config->spien_en == ADI_APOLLO_DISABLE) ? 0 : 1);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxen_spien_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dig_enables, uint8_t spi_en)
{
    int32_t err;
    uint8_t i, side;
    adi_apollo_blk_sel_t dig_en;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_RXEN_BLK_SEL_MASK(dig_enables);
    
    for(i = 0; i < ADI_APOLLO_DIG_EN_NUM; i ++) {
        dig_en = dig_enables & (ADI_APOLLO_RXEN_EN_A0 << i);
        if (dig_en > 0) {
            side = i / ADI_APOLLO_DIG_EN_PER_SIDE_NUM;
            regmap_base_addr = calc_rx_enable_base(side);

            /* SPI enable */
            err = adi_apollo_hal_bf_set(device, 
                                        BF_ENABLE_SPI_INFO(regmap_base_addr, (i % ADI_APOLLO_DIG_EN_PER_SIDE_NUM)), 
                                        (spi_en == ADI_APOLLO_DISABLE) ? 0 : 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}



uint32_t calc_txen_power_ctrl_base(int32_t adc_index)
{
    static uint32_t txen_power_ctrl_regmap[] = {
        RXEN_POWER_CTRL0_RX_SLICE_0_RX_DIGITAL0, RXEN_POWER_CTRL1_RX_SLICE_0_RX_DIGITAL0,
        RXEN_POWER_CTRL0_RX_SLICE_1_RX_DIGITAL0, RXEN_POWER_CTRL1_RX_SLICE_1_RX_DIGITAL0,
        RXEN_POWER_CTRL0_RX_SLICE_0_RX_DIGITAL1, RXEN_POWER_CTRL1_RX_SLICE_0_RX_DIGITAL1,
        RXEN_POWER_CTRL0_RX_SLICE_1_RX_DIGITAL1, RXEN_POWER_CTRL1_RX_SLICE_1_RX_DIGITAL1
    };

    return (txen_power_ctrl_regmap[adc_index]);
}

uint32_t calc_rx_enable_base(int32_t index)
{
    static uint32_t rx_enable_regmap[] = {
        TXRX_ENABLE_RX_DIGITAL0, TXRX_ENABLE_RX_DIGITAL1
    };

    return (rx_enable_regmap[index]);
}
/*! @} */
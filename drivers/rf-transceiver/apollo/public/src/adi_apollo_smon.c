
/*!
 * \brief     APIs for SMON
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SMON
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_smon.h"
#include "adi_apollo_smon_local.h"
#include "adi_apollo_dformat_local.h"
#include "adi_apollo_trigts_local.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_rx_smon.h"
#include "adi_apollo_bf_jtx_dformat.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_smon_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_pgm_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t smon;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);
        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);
            
            err = adi_apollo_hal_bf_set(device, BF_SMON_SFRAMER_MODE_EN_INFO(regmap_base_addr), config->sframer_mode_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_PERIOD_INFO(regmap_base_addr), config->smon_period);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_THRESH_LOW_INFO(regmap_base_addr), config->thresh_low);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_THRESH_HIGH_INFO(regmap_base_addr), config->thresh_high);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_SYNC_EN_INFO(regmap_base_addr), config->sync_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_SYNC_NEXT_INFO(regmap_base_addr), config->sync_next);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_SFRAMER_EN_INFO(regmap_base_addr), config->sframer_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_SFRAMER_MODE_INFO(regmap_base_addr), config->sframer_mode);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_SFRAMER_INSEL_INFO(regmap_base_addr), config->sframer_insel);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_PEAK_EN_INFO(regmap_base_addr), config->peak_en);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_STATUS_RDSEL_INFO(regmap_base_addr), config->status_rdsel);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_JLINK_SEL_INFO(regmap_base_addr), config->jlink_sel);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_GPIO_EN_INFO(regmap_base_addr), config->gpio_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
        
    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_smon_dfor_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t links, adi_apollo_blk_sel_t fddcs, adi_apollo_smon_dfor_pgm_t *config)
{
    int32_t err, i;
    adi_apollo_blk_sel_t fddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) {                                                                     /* LINK 0 */
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_0_SEL_0_INFO(regmap_base_addr), config->ctrl_bit_0); 
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_1_SEL_0_INFO(regmap_base_addr), config->ctrl_bit_1);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_2_SEL_0_INFO(regmap_base_addr), config->ctrl_bit_2);
            } else {                                                                                                                /* LINK 1 */
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_0_SEL_1_INFO(regmap_base_addr), config->ctrl_bit_0); 
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_1_SEL_1_INFO(regmap_base_addr), config->ctrl_bit_1);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_2_SEL_1_INFO(regmap_base_addr), config->ctrl_bit_2);
            }

            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* Select SMON status bits for the fine DDCs */
    for (i = 0; i < ADI_APOLLO_FDDC_NUM; i++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_jtx_dformat_base(i / ADI_APOLLO_FDDC_PER_SIDE_NUM);

            switch (i % ADI_APOLLO_FDDC_PER_SIDE_NUM) {
                case 0:
                    err = adi_apollo_hal_bf_set(device, BF_FINE0_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE0_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
                case 1:
                    err = adi_apollo_hal_bf_set(device, BF_FINE1_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE1_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
                case 2:
                    err = adi_apollo_hal_bf_set(device, BF_FINE2_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE2_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
                case 3:
                    err = adi_apollo_hal_bf_set(device, BF_FINE3_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE3_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
                case 4:
                    err = adi_apollo_hal_bf_set(device, BF_FINE4_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE4_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
                case 5:
                    err = adi_apollo_hal_bf_set(device, BF_FINE5_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE5_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
                case 6:
                    err = adi_apollo_hal_bf_set(device, BF_FINE6_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE6_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
                case 7:
                    err = adi_apollo_hal_bf_set(device, BF_FINE7_ADC_I_SEL_INFO(regmap_base_addr), config->fine_adc_i_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    err = adi_apollo_hal_bf_set(device, BF_FINE7_ADC_Q_SEL_INFO(regmap_base_addr), config->fine_adc_q_sel[i]);
                    ADI_APOLLO_ERROR_RETURN(err);
                    break;
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_smon_status_update(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint8_t update)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t smon;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);
        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);
            
            err = adi_apollo_hal_bf_set(device, BF_SMON_STATUS_UPDATE_INFO(regmap_base_addr), update); //Toggle bit
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
            
    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_smon_read(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_read_t *data)
{
    int32_t err;
    uint8_t i, j, reg8;
    adi_apollo_blk_sel_t smon;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);
        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);
 
            err = adi_apollo_hal_bf_get(device, BF_SMON_STATUS_FCNT_INFO(regmap_base_addr), &data->status_fcnt, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            data->status = 0;           
            for (j = 0; j < ADI_APOLLO_SMON_STATUS_SIZE; j++) {
                err = adi_apollo_hal_bf_get(device, BF_SMON_STATUS_INFO(regmap_base_addr, j), &reg8, 1);
                ADI_APOLLO_ERROR_RETURN(err);
                data->status |= reg8 << (j*8);
            }
        }
    }        
    return API_CMS_ERROR_OK; 
}

uint32_t calc_smon_base(int32_t smon_index)
{
    static uint32_t rx_smon_regmap[ADI_APOLLO_SMON_NUM] = {
        RX_SMON0_RX_SLICE_0_RX_DIGITAL0, RX_SMON0_RX_SLICE_1_RX_DIGITAL0,
        RX_SMON1_RX_SLICE_0_RX_DIGITAL0, RX_SMON1_RX_SLICE_1_RX_DIGITAL0,
        RX_SMON0_RX_SLICE_0_RX_DIGITAL1, RX_SMON0_RX_SLICE_1_RX_DIGITAL1,
        RX_SMON1_RX_SLICE_0_RX_DIGITAL1, RX_SMON1_RX_SLICE_1_RX_DIGITAL1,
    };

    return rx_smon_regmap[smon_index];
}
/*! @} */
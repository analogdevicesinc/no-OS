
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

#include "adi_utils.h"

#define FRAMER_5_SUB_FRAME_NUM_BITS         5
#define FRAMER_5_SUB_FRAME_ID_NUM_BITS      (1 * FRAMER_5_SUB_FRAME_NUM_BITS)
#define FRAMER_5_SUB_FRAME_DATA_NUM_BITS    (4 * FRAMER_5_SUB_FRAME_NUM_BITS)
#define FRAMER_5_SUB_FRAME_IDLE_NUM_BITS    (1 * FRAMER_5_SUB_FRAME_NUM_BITS)
#define FRAMER_5_MIN_SAMPLES                (FRAMER_5_SUB_FRAME_ID_NUM_BITS + FRAMER_5_SUB_FRAME_DATA_NUM_BITS + FRAMER_5_SUB_FRAME_IDLE_NUM_BITS)

#define FRAMER_10_SUB_FRAME_NUM_BITS         10
#define FRAMER_10_SUB_FRAME_ID_NUM_BITS      (1 * FRAMER_10_SUB_FRAME_NUM_BITS)
#define FRAMER_10_SUB_FRAME_DATA_NUM_BITS    (2 * FRAMER_10_SUB_FRAME_NUM_BITS)
#define FRAMER_10_SUB_FRAME_IDLE_NUM_BITS    (1 * FRAMER_10_SUB_FRAME_NUM_BITS)
#define FRAMER_10_MIN_SAMPLES                (FRAMER_10_SUB_FRAME_ID_NUM_BITS + FRAMER_10_SUB_FRAME_DATA_NUM_BITS + FRAMER_10_SUB_FRAME_IDLE_NUM_BITS)

typedef int32_t (*framer_sof_get_t)(int16_t *samples, uint32_t num_samples, uint32_t ofst, uint16_t smon_ctrl_bit_mask, uint32_t *sof_idx);
typedef int32_t (*framer_peak_val_get_t)(int16_t *samples, uint32_t num_samples, uint32_t sof_idx, uint16_t smon_ctrl_bit_mask, uint16_t *peak_val);

static int32_t sof_framer5_get(int16_t *samples, uint32_t num_samples, uint32_t ofst, uint16_t smon_ctrl_bit_mask, uint32_t *sof_idx);
static int32_t peak_val_framer5_get(int16_t *samples, uint32_t num_samples, uint32_t sof_idx, uint16_t smon_ctrl_bit_mask, uint16_t *peak_val);

static int32_t sof_framer10_get(int16_t *samples, uint32_t num_samples, uint32_t ofst, uint16_t smon_ctrl_bit_mask, uint32_t *sof_idx);
static int32_t peak_val_framer10_get(int16_t *samples, uint32_t num_samples, uint32_t sof_idx, uint16_t smon_ctrl_bit_mask, uint16_t *peak_val);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_smon_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_pgm_t *config)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t smon;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
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
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_FDDC_BLK_SEL_MASK(fddcs);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) {                                                                     /* LINK 0 (A0/B0) */
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_0_SEL_0_INFO(regmap_base_addr), config->ctrl_bit_0);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_1_SEL_0_INFO(regmap_base_addr), config->ctrl_bit_1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_2_SEL_0_INFO(regmap_base_addr), config->ctrl_bit_2);
                ADI_APOLLO_ERROR_RETURN(err);
            } else {                                                                                                                /* LINK 1 (A1/B1) */
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_0_SEL_1_INFO(regmap_base_addr), config->ctrl_bit_0);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_1_SEL_1_INFO(regmap_base_addr), config->ctrl_bit_1);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_2_SEL_1_INFO(regmap_base_addr), config->ctrl_bit_2);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    /* Select SMON status bits for the fine DDCs */
    for (i = 0; i < ADI_APOLLO_FDDC_NUM; i++) {
        fddc = fddcs & (ADI_APOLLO_FDDC_A0 << i);
        if (fddc > 0) {
            regmap_base_addr = calc_jtx_dformat_base((i / ADI_APOLLO_FDDC_PER_SIDE_NUM) * ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE);

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
                    
                /* FINE4-FINE7 is 8T8R only */
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
    ADI_APOLLO_NULL_POINTER_RETURN(data);
    ADI_CMS_SINGLE_SELECT_CHECK(smons);
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

int32_t adi_apollo_smon_peak_val_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint16_t *pk_val)
{
    int32_t err;
    uint8_t i;
    uint8_t j;
    uint8_t reg8;
    adi_apollo_blk_sel_t smon;
    uint32_t status;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(pk_val);
    ADI_CMS_SINGLE_SELECT_CHECK(smons);
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);
        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);

            // Toggle for pk val update
            err = adi_apollo_hal_bf_set(device, BF_SMON_STATUS_UPDATE_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_STATUS_UPDATE_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            status = 0;
            for (j = 0; j < ADI_APOLLO_SMON_STATUS_SIZE; j++) {
                err = adi_apollo_hal_bf_get(device, BF_SMON_STATUS_INFO(regmap_base_addr, j), &reg8, 1);
                ADI_APOLLO_ERROR_RETURN(err);
                status |= reg8 << (j * 8);
            }

            *pk_val = (uint16_t)((status & 0x000ffe00) >> 9);

            /* Only one get result per call */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_smon_thresh_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint16_t thresh_low, uint16_t thresh_high)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t smon;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(thresh_low > ADI_APOLLO_SMON_MAX_THRESH_VAL);
    ADI_APOLLO_INVALID_PARAM_RETURN(thresh_high > ADI_APOLLO_SMON_MAX_THRESH_VAL);
    ADI_APOLLO_INVALID_PARAM_RETURN(thresh_low > thresh_high);
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);
        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);

            err = adi_apollo_hal_bf_set(device, BF_SMON_THRESH_LOW_INFO(regmap_base_addr), thresh_low);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_SMON_THRESH_HIGH_INFO(regmap_base_addr), thresh_high);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_smon_thresh_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, uint16_t *thresh_low, uint16_t *thresh_high)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t smon;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(thresh_low);
    ADI_APOLLO_NULL_POINTER_RETURN(thresh_high);
    ADI_CMS_SINGLE_SELECT_CHECK(smons);
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);
        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);

            err = adi_apollo_hal_bf_get(device, BF_SMON_THRESH_LOW_INFO(regmap_base_addr), (uint8_t *) thresh_low, sizeof(*thresh_low));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_THRESH_HIGH_INFO(regmap_base_addr), (uint8_t *) thresh_high, sizeof(*thresh_high));
            ADI_APOLLO_ERROR_RETURN(err);

            /* Only one get result per call */
            break;
        }
    }
    
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_smon_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons, adi_apollo_smon_inspect_t *smon_inspect)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t smon;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(smon_inspect);
    ADI_CMS_SINGLE_SELECT_CHECK(smons);
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);
        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);

            err = adi_apollo_hal_bf_get(device, BF_SMON_PERIOD_INFO(regmap_base_addr), (uint8_t *) &(smon_inspect->dp_cfg.period), sizeof(smon_inspect->dp_cfg.period));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_GPIO_EN_INFO(regmap_base_addr), (uint8_t *) &(smon_inspect->dp_cfg.gpio_en), sizeof(smon_inspect->dp_cfg.gpio_en));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_PEAK_EN_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.peak_en), sizeof(smon_inspect->dp_cfg.peak_en));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_JLINK_SEL_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.jlink_sel), sizeof(smon_inspect->dp_cfg.jlink_sel));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_SFRAMER_EN_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.sframer_en), sizeof(smon_inspect->dp_cfg.sframer_en));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_SFRAMER_MODE_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.sframer_mode), sizeof(smon_inspect->dp_cfg.sframer_mode));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_SYNC_EN_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.sync_en), sizeof(smon_inspect->dp_cfg.sync_en));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_SYNC_NEXT_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.sync_next), sizeof(smon_inspect->dp_cfg.sync_next));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_THRESH_HIGH_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.thresh_high), sizeof(smon_inspect->dp_cfg.thresh_high));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_SMON_THRESH_LOW_INFO(regmap_base_addr), (uint8_t *)&(smon_inspect->dp_cfg.thresh_low), sizeof(smon_inspect->dp_cfg.thresh_low));
            ADI_APOLLO_ERROR_RETURN(err);

            /* Only one inspect per call */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_smon_peak_val_from_cap_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t smons,
                                              int16_t cap_array[], uint32_t cap_len, uint32_t offset,
                                              uint16_t smon_ctrl_bit_mask,
                                              uint32_t *sof_idx, uint16_t *pk_val)
{
    int32_t err;
    uint16_t i;
    adi_apollo_blk_sel_t smon;
    framer_sof_get_t sof_get;
    framer_peak_val_get_t pk_val_get;
    uint32_t regmap_base_addr = 0;
    uint8_t framer_mode;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cap_array);
    ADI_APOLLO_NULL_POINTER_RETURN(sof_idx);
    ADI_APOLLO_NULL_POINTER_RETURN(pk_val);
    ADI_CMS_SINGLE_SELECT_CHECK(smons);
    ADI_APOLLO_SMON_BLK_SEL_MASK(smons);

    for (i = 0; i < ADI_APOLLO_SMON_NUM; i++) {
        smon = smons & (ADI_APOLLO_SMON_A0 << i);

        if (smon > 0) {
            regmap_base_addr = calc_smon_base(i);

            err = adi_apollo_hal_bf_get(device, BF_SMON_SFRAMER_MODE_INFO(regmap_base_addr), &framer_mode, sizeof(framer_mode));
            ADI_APOLLO_ERROR_RETURN(err);

            // Assign the 5-bit or 10-bit framer de-serialization methods
            sof_get = (framer_mode == ADI_APOLLO_SMON_SFRAMER_TEN_BIT) ? sof_framer10_get : sof_framer5_get;
            pk_val_get = (framer_mode == ADI_APOLLO_SMON_SFRAMER_TEN_BIT) ? peak_val_framer10_get : peak_val_framer5_get;

            /* Find the start of frame */
            err = sof_get(cap_array, cap_len, offset, smon_ctrl_bit_mask, sof_idx);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Extract the pk mag */
            err = pk_val_get(cap_array, cap_len, *sof_idx, smon_ctrl_bit_mask, pk_val);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

static int32_t sof_framer5_get(int16_t *samples, uint32_t num_samples, uint32_t ofst, uint16_t smon_ctrl_bit_mask, uint32_t *sof_idx)
{
    int32_t s_idx;
    int32_t k_idx;
    uint8_t sof_found = 0;
    int16_t key_seq[11] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0};
    int32_t key_seq_len = sizeof(key_seq) / sizeof(key_seq[0]);

    *sof_idx = 0;

    if ((num_samples - ofst) < FRAMER_5_MIN_SAMPLES) {
        return API_CMS_ERROR_SMON_FRAMER_MIN_SAMPLES_ERROR;
    }
    
    for (s_idx = ofst; s_idx < num_samples - key_seq_len; s_idx++) {
        sof_found = 0;
        for (k_idx = 0; k_idx < key_seq_len; k_idx++) {
            if (((samples[s_idx + k_idx] & smon_ctrl_bit_mask) ? 1 : 0) != key_seq[k_idx]) {
                break;
            }
        }
        if (k_idx == key_seq_len) {
            sof_found = 1;
            break;
        }
    }

    if (sof_found) {
        *sof_idx = s_idx + FRAMER_5_SUB_FRAME_IDLE_NUM_BITS; // Skip over last idle sub-frame, advance to start of identifier sub-frame
    } else {
        return API_CMS_ERROR_SMON_FRAMER_SOF_NOT_FOUND;
    }

    return API_CMS_ERROR_OK;
 }

static int32_t peak_val_framer5_get(int16_t *samples, uint32_t num_samples, uint32_t sof_idx, uint16_t smon_ctrl_bit_mask, uint16_t *peak_val)
{

    *peak_val = 0;
    int16_t shft = 15;

    if ((num_samples - sof_idx - FRAMER_5_SUB_FRAME_ID_NUM_BITS) < (FRAMER_5_SUB_FRAME_DATA_NUM_BITS)) {
        return API_CMS_ERROR_SMON_FRAMER_MIN_SAMPLES_ERROR;
    }

    // Skip over identifier sub-frame
    for (int i = FRAMER_5_SUB_FRAME_ID_NUM_BITS; i < (FRAMER_5_SUB_FRAME_ID_NUM_BITS + FRAMER_5_SUB_FRAME_DATA_NUM_BITS); i++) {
        if (!(i % FRAMER_5_SUB_FRAME_NUM_BITS)) {
            continue; // skip sub-frame leading 0 start bit
        }

        *peak_val |= ((samples[sof_idx + i] & smon_ctrl_bit_mask) ? 1 : 0) << shft;
        shft--;
    }

    *peak_val = ((uint16_t)(*peak_val)) >> 5; // shift down to 11 bits (resolution of peak hold detector)

    return API_CMS_ERROR_OK;
}

static int32_t sof_framer10_get(int16_t *samples, uint32_t num_samples, uint32_t ofst, uint16_t smon_ctrl_bit_mask, uint32_t *sof_idx)
{
    int32_t s_idx;
    int32_t k_idx;
    uint8_t sof_found = 0;
    int16_t key_seq[20] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  0,  0, 0, 0, 0, 0, 0, 0, 1,  1};
    int32_t key_seq_len = sizeof(key_seq) / sizeof(key_seq[0]);

    *sof_idx = 0;

    if ((num_samples - ofst) < FRAMER_10_MIN_SAMPLES) {
        return API_CMS_ERROR_SMON_FRAMER_MIN_SAMPLES_ERROR;
    }

    for (s_idx = ofst; s_idx < num_samples - key_seq_len; s_idx++) {
        sof_found = 0;
        for (k_idx = 0; k_idx < key_seq_len; k_idx++) {

            //printf("%6d    %d\n", s_idx + k_idx, ((samples[s_idx + k_idx] & ctrl_bit_mask) ? 1 : 0));

            if (((samples[s_idx + k_idx] & smon_ctrl_bit_mask) ? 1 : 0) != key_seq[k_idx]) {
                break;
            }
        }
        if (k_idx == key_seq_len) {
            sof_found = 1;
            break;
        }
    }

    if (sof_found) {
        *sof_idx = s_idx + FRAMER_10_SUB_FRAME_IDLE_NUM_BITS; // Skip over last idle sub-frame, advance to start of identifier sub-frame
    } else {
        return API_CMS_ERROR_SMON_FRAMER_SOF_NOT_FOUND;
    }

    return API_CMS_ERROR_OK;}

static int32_t peak_val_framer10_get(int16_t *samples, uint32_t num_samples, uint32_t sof_idx, uint16_t smon_ctrl_bit_mask, uint16_t *peak_val)
{
    *peak_val = 0;
    int16_t shft = 15;

    if ((num_samples - sof_idx - FRAMER_10_SUB_FRAME_ID_NUM_BITS) < (FRAMER_10_SUB_FRAME_DATA_NUM_BITS)) {
        return API_CMS_ERROR_SMON_FRAMER_MIN_SAMPLES_ERROR;
    }

    // Skip over identifier sub-frame
    for (int i = FRAMER_10_SUB_FRAME_ID_NUM_BITS; i < (FRAMER_10_SUB_FRAME_ID_NUM_BITS + FRAMER_10_SUB_FRAME_DATA_NUM_BITS); i++) {
        if ( (i % FRAMER_10_SUB_FRAME_NUM_BITS == 0) || (i %FRAMER_10_SUB_FRAME_NUM_BITS == 9) ) {
            continue; // skip sub-frame start and stop sub-frame bits
        }

        *peak_val |= ((samples[sof_idx + i] & smon_ctrl_bit_mask) ? 1 : 0) << shft;
        shft--;
    }

    *peak_val = ((uint16_t)(*peak_val)) >> 5; // shift down to 11 bits (resolution of peak hold detector)

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
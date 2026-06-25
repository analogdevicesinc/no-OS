/*!
 * \brief     Tx Misc functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXMISC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_txmisc.h"
#include "adi_apollo_txmisc_local.h"

#include "adi_apollo_bf_core.h"
#include "adi_apollo_bf_tx_misc.h"

#include "adi_apollo_cduc.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

static int32_t tx_summer_inspect(adi_apollo_device_t* device, uint16_t side_idx, adi_apollo_txpath_misc_t* dp);
static int32_t tx_cduc_dac_inspect(adi_apollo_device_t* device, uint16_t side_idx, adi_apollo_txpath_misc_t* dp);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_txmisc_low_samp_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t low_samp_en)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_tx_misc_base(side_index);

            err = adi_apollo_hal_bf_set(device, BF_LOW_SAMP_TX_MISC_INFO(regmap_base_addr), low_samp_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txmisc_dp_reset(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t reset_en)
{
    int32_t err;
    uint8_t side, side_index;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            if (side_index == 0)
                err = adi_apollo_hal_bf_set(device, BF_TX_DP_RESET_A_INFO, reset_en);
            else
                err = adi_apollo_hal_bf_set(device, BF_TX_DP_RESET_B_INFO, reset_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txmisc_cduc_dac_enable_set(adi_apollo_device_t *device, const uint16_t cducs, const uint8_t enable)
{
    int32_t err;
    uint16_t i, cducs_sel;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_CDUC_NUM; i++) {
        /* Select one CDUC at a time */
        cducs_sel = cducs & (ADI_APOLLO_CDUC_A0 << i);
        if (cducs_sel > 0) {
            /* Base address is side-dependent */
            regmap_base_addr = calc_tx_misc_base(i / ADI_APOLLO_CDUC_PATHS_PER_SIDE);

            /* Program the CDUC_DAC_ENABLESx fields */
            switch (i % ADI_APOLLO_CDUC_PATHS_PER_SIDE)
            {
            case 0:
                err = adi_apollo_hal_bf_set(device, BF_CDUC_DAC_ENABLES0_INFO(regmap_base_addr), enable);
                break;
            case 1:
                err = adi_apollo_hal_bf_set(device, BF_CDUC_DAC_ENABLES1_INFO(regmap_base_addr), enable);
                break;
            case 2:
                err = adi_apollo_hal_bf_set(device, BF_CDUC_DAC_ENABLES2_INFO(regmap_base_addr), enable);
                break;
            case 3:
                err = adi_apollo_hal_bf_set(device, BF_CDUC_DAC_ENABLES3_INFO(regmap_base_addr), enable);
                break;                
            }
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_txmisc_inspect(adi_apollo_device_t *device, uint16_t side_sel, adi_apollo_txmisc_inspect_t *txmisc_inspect)
{
    int32_t err;
    uint8_t side, side_index;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(txmisc_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(side_sel) != 1);

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            err = tx_summer_inspect(device, side_index, &txmisc_inspect->dp);
            ADI_APOLLO_ERROR_RETURN(err);
            
            err = tx_cduc_dac_inspect(device, side_index, &txmisc_inspect->dp);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Inspect only one side per call */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

static int32_t tx_summer_inspect(adi_apollo_device_t* device, uint16_t side_idx, adi_apollo_txpath_misc_t *dp)
{
    int32_t err;
    uint8_t fduc_enables[4];

    uint32_t regmap_base_addr = calc_tx_misc_base(side_idx);

    err = adi_apollo_hal_reg_get(device, REG_FDUC_ENABLES00_ADDR(regmap_base_addr), &fduc_enables[0]);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_get(device, REG_FDUC_ENABLES10_ADDR(regmap_base_addr), &fduc_enables[2]);
    ADI_APOLLO_ERROR_RETURN(err);


    if (device->dev_info.is_8t8r) {
        /* Summer A0 */
        dp->fduc_cduc_summer[0] =
            ((fduc_enables[0] & 0x01) >> 0) | ((fduc_enables[0] & 0x04) >> 1) | ((fduc_enables[0] & 0x10) >> 2) | ((fduc_enables[0] & 0x40) >> 3) |
            ((fduc_enables[0] & 0x02) << 3) | ((fduc_enables[0] & 0x08) >> 2) | ((fduc_enables[0] & 0x20) >> 1) | ((fduc_enables[0] & 0x80) >> 0);
        /* Summer A1 */
        dp->fduc_cduc_summer[1] =
            ((fduc_enables[2] & 0x01) >> 0) | ((fduc_enables[2] & 0x04) >> 1) | ((fduc_enables[2] & 0x10) >> 2) | ((fduc_enables[2] & 0x40) >> 3) |
            ((fduc_enables[2] & 0x02) << 3) | ((fduc_enables[2] & 0x08) >> 2) | ((fduc_enables[2] & 0x20) >> 1) | ((fduc_enables[2] & 0x80) >> 0);
    } else {
        /* Summer A0 */
        dp->fduc_cduc_summer[0] = fduc_enables[0] & 0x0f;

        /* Summer A1 */
        dp->fduc_cduc_summer[1] = fduc_enables[2] & 0x0f;
    }

    return API_CMS_ERROR_OK;
}

static int32_t tx_cduc_dac_inspect(adi_apollo_device_t* device, uint16_t side_idx, adi_apollo_txpath_misc_t *dp)
{
    int32_t err;
    uint32_t regmap_base_addr = calc_tx_misc_base(side_idx);
    
    err = adi_apollo_hal_bf_get(device, BF_CDUC_DAC_ENABLES0_INFO(regmap_base_addr), &dp->cduc_dac_enables[0], 1);    
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_bf_get(device, BF_CDUC_DAC_ENABLES1_INFO(regmap_base_addr), &dp->cduc_dac_enables[1], 1);    
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_bf_get(device, BF_CDUC_DAC_ENABLES2_INFO(regmap_base_addr), &dp->cduc_dac_enables[2], 1);    
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_bf_get(device, BF_CDUC_DAC_ENABLES3_INFO(regmap_base_addr), &dp->cduc_dac_enables[3], 1); 
    
    return API_CMS_ERROR_OK;
}

uint32_t calc_tx_misc_base(int32_t idx)
{
    static const uint32_t tx_misc_regmap[ADI_APOLLO_TX_MISC_REGMAP_NUM] = {
        TX_MISC_TX_TOP_TX_DIGITAL0,

        TX_MISC_TX_TOP_TX_DIGITAL1
    };

    return tx_misc_regmap[idx];
}
/*! @} */
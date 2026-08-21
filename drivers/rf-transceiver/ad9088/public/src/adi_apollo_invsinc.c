/*!
 * \brief     APIs for Tx Inverse Sinc
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_INVSINC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_utils.h"
#include "adi_apollo_invsinc.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_tx_hsdout.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/
static uint32_t calc_tx_hsdout_base(int32_t index);

int32_t adi_apollo_invsinc_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t invsincs, const uint8_t enable)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t invsinc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVSINC_BLK_SEL_MASK(invsincs);
    
    for (i = 0; i < ADI_APOLLO_NUM_INVSINC; i++) {
        invsinc = invsincs & (ADI_APOLLO_INVSINC_A0 << i) & (device->dev_info.is_8t8r ? 0xff : 0x33);
        if (invsinc > 0) {
            regmap_base_addr = calc_tx_hsdout_base(i);

	        /* Enable INVSINC CLK */
            err = adi_apollo_hal_bf_set(device, BF_INVSINC_CLK_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);

	        /* Enable INVSINC */
            err = adi_apollo_hal_bf_set(device, BF_INVSINC_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
  
            if (device->dev_info.is_8t8r == 0) { // In 4T4R, in the current design, Invsinc enable control uses 2 bits to control each slice.
                regmap_base_addr = calc_tx_hsdout_base(i + 2); 

                err = adi_apollo_hal_bf_set(device, BF_INVSINC_EN_INFO(regmap_base_addr), enable);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_invsinc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t invsincs, adi_apollo_invsinc_inspect_t *invsinc_inspect)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t invsinc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(invsinc_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(invsincs) != 1);
    ADI_APOLLO_INVSINC_BLK_SEL_MASK(invsincs);

    for (i = 0; i < ADI_APOLLO_NUM_INVSINC; i++) {
        invsinc = invsincs & (ADI_APOLLO_INVSINC_A0 << i);
        if (invsinc > 0) {
            regmap_base_addr = calc_tx_hsdout_base(i);

	        /* Get INVSINC CLK enable state */
            err = adi_apollo_hal_bf_get(device, BF_INVSINC_CLK_EN_INFO(regmap_base_addr), &(invsinc_inspect->invsinc_clk_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);

	        /* Get INVSINC enable state */
            err = adi_apollo_hal_bf_get(device, BF_INVSINC_EN_INFO(regmap_base_addr), &(invsinc_inspect->invsinc_en), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            break;      /* only one chan per call */
        }
    }

    return API_CMS_ERROR_OK;
}

static uint32_t calc_tx_hsdout_base(int32_t index)
{
    static uint32_t tx_hsdout_regmap[ADI_APOLLO_DAC_NUM] = {
        TX_HSDOUT0_TX_SLICE_0_TX_DIGITAL0,      /* A0 */
        TX_HSDOUT0_TX_SLICE_1_TX_DIGITAL0,      /* A1 */
        TX_HSDOUT1_TX_SLICE_0_TX_DIGITAL0,      /* A2 */
        TX_HSDOUT1_TX_SLICE_1_TX_DIGITAL0,      /* A3 */

        TX_HSDOUT0_TX_SLICE_0_TX_DIGITAL1,      /* B0 */
        TX_HSDOUT0_TX_SLICE_1_TX_DIGITAL1,      /* B1 */
        TX_HSDOUT1_TX_SLICE_0_TX_DIGITAL1,      /* B2 */
        TX_HSDOUT1_TX_SLICE_1_TX_DIGITAL1       /* B3 */
    };
    return tx_hsdout_regmap[index];
}

/*! @} */

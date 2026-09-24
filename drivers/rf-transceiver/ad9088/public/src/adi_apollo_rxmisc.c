/*!
 * \brief     Rx Misc functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXMISC
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_rxmisc.h"
#include "adi_apollo_rxmisc_local.h"

#include "adi_apollo_bf_core.h"
#include "adi_apollo_bf_rx_misc.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_rxmisc_low_samp_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t low_samp_en)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_rx_misc_base(side_index);

            err = adi_apollo_hal_bf_set(device, BF_LOW_SAMP_RX_MISC_INFO(regmap_base_addr), low_samp_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxmisc_pgm(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, adi_apollo_rx_misc_pgm_t *config)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_rx_misc_base(side_index);

            err = adi_apollo_hal_bf_set(device, BF_DATAPATH_CLK_EN_INFO(regmap_base_addr), config->datapath_clk); 
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_LOW_SAMP_RX_MISC_INFO(regmap_base_addr), config->low_samp_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxmisc_dp_reset(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t reset_en)
{
    int32_t err;
    uint8_t side, side_index;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            if (side_index == 0)
                err = adi_apollo_hal_bf_set(device, BF_RX_DP_RESET_A_INFO, reset_en);
            else
                err = adi_apollo_hal_bf_set(device, BF_RX_DP_RESET_B_INFO, reset_en);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_rx_misc_base(int32_t side_idx)
{
    static uint32_t rx_misc_regmap[ADI_APOLLO_NUM_SIDES] = {
        RX_MISC_RX_DIGITAL0,
        RX_MISC_RX_DIGITAL1
    };
    return rx_misc_regmap[side_idx];
}
/*! @} */
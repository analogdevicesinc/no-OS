/*!
 * \brief     APIs for Tx MUXs
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXMUX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_txmux.h"
#include "adi_apollo_txen_local.h"
#include "adi_apollo_duc_local.h"
#include "adi_apollo_fduc_types.h"
#include "adi_apollo_txmisc_local.h"

#include "adi_apollo_bf_tx_misc.h"
#include "adi_apollo_bf_tx_hsdout.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_txmux_summer_block_set(adi_apollo_device_t *device, const uint16_t summers, const uint16_t fducs_connected)
{
    int32_t err;
    uint8_t fduc, fducs, fduc_sel_mask;
    uint16_t i, j, reg_ofst, summer;
    uint32_t regmap_base_addr = 0;

    /* align summer reg A                   A0    A1    A2    A3   */
    /*                                      B0    B1    B2    B3   */
    const static uint8_t reg_ofst_tab[4] = {0x00, 0x02, 0x01, 0x03};

    /* fduc selects for 8T devices               A0    A1    A2    A3    A4    A5    A6    A7  */
    /*                                           B0    B1    B2    B3    B4    B5    B6    B7  */
    const static uint8_t fduc_sel_map_8t8r[] = {0x01, 0x04, 0x10, 0x40, 0x02, 0x08, 0x20, 0x80};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_TX_SUMMER_NUM; i ++) {
        summer = summers & (ADI_APOLLO_TX_SUMMER_A0 << i);

        if (summer > 0) {
            regmap_base_addr = calc_tx_misc_base(i / ADI_APOLLO_TX_SUMMER_PER_SIDE_NUM);

            // Determine proper fduc enable register
            reg_ofst = reg_ofst_tab[i % ADI_APOLLO_TX_SUMMER_PER_SIDE_NUM];

            fducs = (i < ADI_APOLLO_TX_SUMMER_PER_SIDE_NUM) ? fducs_connected : fducs_connected >> ADI_APOLLO_FDUC_PER_SIDE_NUM;

            fduc_sel_mask = 0;
            for (j = 0; j < ADI_APOLLO_FDUC_PER_SIDE_NUM; j++) {
                fduc = fducs & (ADI_APOLLO_FDUC_A0 << j);
                if (fduc > 0) {
                    fduc_sel_mask |= (device->dev_info.is_8t8r == 1) ? fduc_sel_map_8t8r[j] : (1<<j);
                }
            }
            err = adi_apollo_hal_reg_set(device, REG_FDUC_ENABLES00_ADDR(regmap_base_addr + reg_ofst), fduc_sel_mask);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txmux_hsdout_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, const uint8_t dac[], const uint32_t length)
{
    int32_t err;
    uint16_t i;
    uint8_t xbar = 0;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(length > ADI_APOLLO_NUM_DAC_PER_SIDE);

    for (i = 0; i < length; i ++) {
        xbar |= (0x03 & dac[i]) << (2*i); 
    }

    if ((side_sel & ADI_APOLLO_SIDE_A) > 0) {
        regmap_base_addr = calc_tx_misc_base(0);
        err = adi_apollo_hal_bf_set(device, BF_HS_XBAR_CTRL_INFO(regmap_base_addr), (uint8_t)xbar);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if ((side_sel & ADI_APOLLO_SIDE_B) > 0) {
        regmap_base_addr = calc_tx_misc_base(1);
        err = adi_apollo_hal_bf_set(device, BF_HS_XBAR_CTRL_INFO(regmap_base_addr), (uint8_t)xbar);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_txmux_modsw_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, adi_apollo_modsw_select_e modsw0, adi_apollo_modsw_select_e modsw1)
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

            err = adi_apollo_hal_bf_set(device, BF_MODSW0_INFO(regmap_base_addr), modsw0);
            ADI_APOLLO_ERROR_RETURN(err);

            if (device->dev_info.is_8t8r) {
                err = adi_apollo_hal_bf_set(device, BF_MODSW1_INFO(regmap_base_addr), modsw1);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

/*! @} */
